//# ImageInterface.cc: defines the Image base class non pure virtual stuff
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

#include <aips/aips.h>

#include <aips/Arrays/Vector.h> // Put these early to work around g++ bug
#include <aips/Arrays/Matrix.h>

#include <trial/Coordinates/StokesCoordinate.h>

#include <trial/Images/ImageInterface.h>
#include <trial/Lattices/LatticeIterator.h>

#include <aips/Arrays/ArrayMath.h>
#include <aips/Measures/Unit.h>
#include <aips/Utilities/Assert.h>

#include <aips/Logging/LogIO.h>

#include <strstream.h>


template <class T> 
ImageInterface<T>::ImageInterface (Bool masking)
: throughmask_p(masking)
{
  logSink() << LogOrigin("ImageInterface<T>",
	    "ImageInterface(Bool masking)",
			 WHERE) << LogIO::DEBUGGING <<
    "Creating ImageInterfac with masking="
	    << masking << LogIO::POST;
}

template <class T> 
ImageInterface<T>::ImageInterface (const ImageInterface& other)
: throughmask_p(other.throughmask_p),
  coords_p     (other.coords_p),
  log_p        (other.log_p)
{
  logSink() << LogOrigin("ImageInterface<T>",
	    "ImageInterface(const ImageInterface&)",
			 WHERE) << LogIO::DEBUGGING << LogIO::POST;
}

template <class T> 
ImageInterface<T>& ImageInterface<T>::operator= (const ImageInterface& other)
{
  if (this != &other) {
    throughmask_p = other.throughmask_p;
    coords_p      = other.coords_p;
    log_p         = other.log_p;
  }
  return *this;
}

template <class T> 
ImageInterface<T>::~ImageInterface()
{
}

// reset coords
template <class T> 
Bool ImageInterface<T>::setCoordinateInfo(const CoordinateSystem &coords)
{
    ostrstream errmsg;
    errmsg << "Cannot set coordinate system: ";

    Bool ok = ToBool(coords.nPixelAxes() == shape().nelements());
    if (!ok) {
	errmsg << "coords.nPixelAxes() == " << coords.nPixelAxes() << 
	    ", image.ndim() == " << shape().nelements();
    } else {
	// Check that the shape is compatible with the stokes coordinates
	Int stkcrd = -1;
	while (ok && (stkcrd = coords.findCoordinate(Coordinate::STOKES, 
							  stkcrd)) >= 0) {
	    ok = True;
	    Int axis = coords.pixelAxes(stkcrd)(0);
	    const StokesCoordinate &stokes = coords.stokesCoordinate(stkcrd);
	    if (axis >= 0) {
		Int nstokes = stokes.stokes().nelements();
		Int axislength = shape()(axis);
		if (axislength > nstokes) {
		    ok = False;
		    errmsg << "Stokes axis is length " << axislength <<
			" but we only have " << nstokes << " stokes values"
			   << endl;
		}
	    }
	}
    }

    if (ok) {
	coords_p = coords;
	logSink() << LogIO::DEBUGGING << 
	    "Changing coordinate system:\n" <<
	    "        ndim = " << shape().nelements() << endl <<
	    "        axes = " << coords_p.worldAxisNames().ac() << endl <<
	    "     ref val = " << coords_p.referenceValue().ac() << endl <<
	    "     ref pix = " << coords_p.referencePixel().ac() << endl <<
	    "       delta = " << coords_p.increment().ac() << " units = " <<
	    coords_p.worldAxisUnits().ac() << endl <<
	    "linear xfrom = " << coords_p.linearTransform().ac() << 
	    LogIO::POST;
    } else {
	// !ok
	logSink() << LogIO::SEVERE << String(errmsg) << LogIO::POST;
    }
    return ok;
}

template <class T> 
const CoordinateSystem &ImageInterface<T>::coordinates() const 
{
    return coords_p;
}


// function to toggle the ability to write through the image mask.
// True => writes to Image will alter the map and return True, 
// False => writes to Image will fail and return False.
template <class T> void ImageInterface<T>::writeThroughMask(Bool newValue)
{
    logSink() << LogOrigin("ImageInterface<T>", 
			   "writeThroughMask(Bool newValue)", WHERE);
    logSink() << "newValue is " << newValue << " changes to the image will be ";
    if (newValue) {
        logSink() << "recorded ignoring mask values";
    } else {
        logSink() << "recorded only where the mask is False";
    }
    logSink() << LogIO::POST;
    throughmask_p = newValue;
}

// function return the value of the mask toggle
template <class T> Bool ImageInterface<T>::writeThroughMask() const
{
  return throughmask_p;
}

template<class T> void ImageInterface<T>::
putSlice(const Array <T> & sourceBuffer, const IPosition & where){
  Lattice<T>::putSlice(sourceBuffer, where);
};
