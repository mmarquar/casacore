//# SkyCompRep.cc:  this defines SkyCompRep
//# Copyright (C) 1996,1997
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

#include <trial/ComponentModels/SkyCompRep.h>
#include <trial/Coordinates/CoordinateSystem.h>
#include <trial/Coordinates/CoordinateUtil.h>
#include <trial/Coordinates/Coordinate.h>
#include <trial/Coordinates/DirectionCoordinate.h>
#include <trial/Images/ImageInterface.h>
#include <trial/Lattices/LatticeIterator.h>
#include <trial/Lattices/ArrayLattice.h>
#include <trial/Lattices/ArrLatticeIter.h>

#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/Array.h>
#include <aips/Arrays/Vector.h>
#include <aips/Containers/Block.h>
#include <aips/Exceptions/Error.h>
#include <aips/Lattices/IPosition.h>
#include <aips/Mathematics/Math.h>
#include <aips/Measures/Quantum.h>
#include <aips/Measures/QLogical.h>
#include <aips/Measures/MVAngle.h>
#include <aips/Measures/MVDirection.h>
#include <aips/Measures/MDirection.h>
#include <aips/Measures/Stokes.h>
#include <aips/Utilities/Assert.h>
#include <iostream.h>

SkyCompRep::~SkyCompRep()
{
};

void SkyCompRep::project(ImageInterface<Float> & image) const {
  const CoordinateSystem coords = image.coordinates();
  const IPosition imageShape = image.shape();
  const uInt naxis = imageShape.nelements();
  
  // I currently REQUIRE that the image has one direction coordinate (only).
  // All other coordinates (ie. polarization and frequency) are optional. 
  const Vector<uInt> dirAxes = findDirectionAxes(coords);
  AlwaysAssert(dirAxes.nelements() != 0, AipsError);
  const uInt nPixAxes = dirAxes.nelements();
  Vector<Double> pixelCoord(nPixAxes); pixelCoord = 0.0;
  Vector<Double> worldCoord(2);

  const DirectionCoordinate dirCoord = 
    coords.directionCoordinate(coords.findCoordinate(Coordinate::DIRECTION));
  MDirection pixelDir(MVDirection(0.0), dirCoord.directionType());
  Vector<Quantum<Double> > dirVal(2);
  MVAngle pixelSize;
  {
    Vector<String> units = dirCoord.worldAxisUnits();
    dirVal(0).setUnit(units(0));
    dirVal(1).setUnit(units(1));
    Vector<Double> inc = dirCoord.increment();
    Quantum<Double> inc0(inc(0), units(0));
    Quantum<Double> inc1(inc(1), units(1));
    AlwaysAssert(near(inc0, inc1), AipsError);
    pixelSize = MVAngle(inc0);
  }
  
  // Setup an iterator to step through the image in chunks that can fit into
  // memory. Go to a bit of effort to make the chunck size as large as
  // possible but still minimize the number of tiles in the cache.
  IPosition elementShape = imageShape;
  IPosition chunckShape = imageShape;
  uInt axis;
  {
    const IPosition tileShape(image.niceCursorShape(image.maxPixels()));
    cout << "Image shape: " << image.shape() << endl;
    cout << "Tile shape: " << tileShape << endl;
    for (uInt k = 0; k < nPixAxes; k++) {
      axis = dirAxes(k);
      elementShape(axis) = 1;
      chunckShape(axis) = tileShape(axis);
    }
  }

  // Check if there is a Stokes Axes and if so which polarizations. Otherwise
  // only grid the I polarisation.
  Vector<Int> stokes; // Vector stating which polarisations is on each plane
  const Int polAxis = findStokesAxis(stokes, coords);  // The stokes pixel axis
  const uInt nStokes = stokes.nelements(); 
  if (polAxis >= 0)
    AlwaysAssert(imageShape(polAxis) == nStokes, AipsError);
  for (uInt p = 0; p < nStokes; p++)
    AlwaysAssert(stokes(p) == Stokes::I || stokes(p) == Stokes::Q ||
		 stokes(p) == Stokes::U || stokes(p) == Stokes::V, 
		 AipsError);

  Block<IPosition> blc;
  Block<IPosition> trc;
  if (nStokes > 1) {
    blc.resize(nStokes);
    blc = IPosition(naxis,0);
    trc.resize(nStokes);
    trc = elementShape - 1;
    for (uInt p = 0; p < nStokes; p++) {
      blc[p](polAxis) = p;
      trc[p](polAxis) = p;
    }
  }

  LatticeIterator<Float> chunkIter(image, chunckShape);
  Vector<Double> pixelVal(4);
  IPosition chunkOrigin(naxis), elementPosition(naxis);
  for (chunkIter.reset(); !chunkIter.atEnd(); chunkIter++) {
    ArrayLattice<Float> array(chunkIter.cursor());
    ArrLatticeIter<Float> elementIter(array, elementShape);
    chunkOrigin = chunkIter.position();
    for (elementIter.reset(); !elementIter.atEnd(); elementIter++) {
      elementPosition = elementIter.position();
      for (uInt k = 0; k < nPixAxes; k++) {
	axis = dirAxes(k);
	pixelCoord(k) = elementPosition(axis) + chunkOrigin(axis);
      }
      if (dirCoord.toWorld(worldCoord, pixelCoord) == False) {
 	cerr << " SkyCompRep::Pixel at " << pixelCoord 
 	     << " cannot be projected" << endl;
      }
      else {
	dirVal(0).setValue(worldCoord(0));
	dirVal(1).setValue(worldCoord(1));
	pixelDir.set(MVDirection(dirVal));
	sample(pixelVal, pixelDir, pixelSize);
	if (nStokes == 1) {
	  switch (stokes(0)) {
	  case Stokes::I:
	    elementIter.cursor() += Float(pixelVal(0)); break;
	  case Stokes::Q:
	    elementIter.cursor() += Float(pixelVal(1)); break;
	  case Stokes::U:
	    elementIter.cursor() += Float(pixelVal(2)); break;
	  case Stokes::V:
	    elementIter.cursor() += Float(pixelVal(3)); break;
	  }
	}
	else if (elementShape.product() == nStokes)
	  for (uInt p = 0; p < nStokes; p++) {
	    switch (stokes(p)) {
	    case Stokes::I:
	      elementIter.cursor()(blc[p]) += Float(pixelVal(0)); break;
	    case Stokes::Q:
	      elementIter.cursor()(blc[p]) += Float(pixelVal(1)); break;
	    case Stokes::U:
	      elementIter.cursor()(blc[p]) += Float(pixelVal(2)); break;
	    case Stokes::V:
	      elementIter.cursor()(blc[p]) += Float(pixelVal(3)); break;
	    }
	  }
	else
	for (uInt p = 0; p < nStokes; p++) {
	  switch (stokes(p)) {
	  case Stokes::I:
	    elementIter.cursor()(blc[p], trc[p]).ac() += Float(pixelVal(0)); break;
	  case Stokes::Q:
	    elementIter.cursor()(blc[p], trc[p]).ac() += Float(pixelVal(1)); break;
	  case Stokes::U:
	    elementIter.cursor()(blc[p], trc[p]).ac() += Float(pixelVal(2)); break;
	  case Stokes::V:
	    elementIter.cursor()(blc[p], trc[p]).ac() += Float(pixelVal(3)); break;
	  }
	}
      }
    }
  }
};

Bool SkyCompRep::ok() const {
  return True;
};

// Local Variables: 
// compile-command: "gmake OPTLIB=1 SkyCompRep"
// End: 
