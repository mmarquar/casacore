//# LCEllipsoid.cc: Define an N-dimensional ellipsoidal region of interest
//# Copyright (C) 1997,1998
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


#include <trial/Lattices/LCEllipsoid.h>
#include <aips/Tables/TableRecord.h>
#include <aips/Arrays/Vector.h>
#include <aips/Exceptions/Error.h>
#include <aips/Utilities/Assert.h>

typedef Vector<Int> lcellipsoid_gppbug1;


LCEllipsoid::LCEllipsoid()
{}

LCEllipsoid::LCEllipsoid (const IPosition& center, Float radius,
			  const IPosition& latticeShape)
: LCRegionFixed (latticeShape),
  itsRadii      (latticeShape.nelements())
{
    fillCenter (center);
    itsRadii = radius;
    setBox (makeBox (itsCenter, itsRadii, latticeShape));
    defineMask();
}

LCEllipsoid::LCEllipsoid (const Vector<Float>& center, Float radius,
			  const IPosition& latticeShape)
: LCRegionFixed (latticeShape),
  itsCenter     (center),
  itsRadii      (latticeShape.nelements())
{
    itsRadii = radius;
    setBox (makeBox (itsCenter, itsRadii, latticeShape));
    defineMask();
}

LCEllipsoid::LCEllipsoid (const Vector<Float>& center,
			  const Vector<Float>& radii,
			  const IPosition& latticeShape)
: LCRegionFixed (latticeShape),
  itsCenter     (center),
  itsRadii      (radii)
{
    setBox (makeBox (itsCenter, itsRadii, latticeShape));
    defineMask();
}

LCEllipsoid::LCEllipsoid (const LCEllipsoid& that)
: LCRegionFixed (that),
  itsCenter     (that.itsCenter),
  itsRadii      (that.itsRadii)
{}

LCEllipsoid::~LCEllipsoid()
{}

LCEllipsoid& LCEllipsoid::operator= (const LCEllipsoid& that)
{
    if (this != &that) {
	LCRegionFixed::operator= (that);
	itsCenter.resize (that.itsCenter.nelements());
	itsRadii.resize  (that.itsCenter.nelements());
	itsCenter = that.itsCenter;
	itsRadii  = that.itsRadii;
    }
    return *this;
}

LCRegion* LCEllipsoid::clone() const
{
    return new LCEllipsoid(*this);
}

LCRegion* LCEllipsoid::doTranslate (const Vector<Float>& translateVector,
				    const IPosition& newLatticeShape) const
{
    uInt ndim = latticeShape().nelements();
    if (translateVector.nelements() != ndim
    ||  newLatticeShape.nelements() != ndim) {
        throw (AipsError ("LCEllipsoid::translate - "
			  "dimensionalities mismatch"));
    }
    Vector<Float> center;
    center = itsCenter;
    for (uInt i=0; i<ndim; i++) {
        center(i) += translateVector(i);
    }
    return new LCEllipsoid (center, itsRadii, newLatticeShape);
}

String LCEllipsoid::className()
{
    return "LCEllipsoid";
}

TableRecord LCEllipsoid::toRecord (const String&) const
{
    TableRecord rec;
    rec.define ("name", className());
    rec.define ("center", itsCenter);
    rec.define ("radii", itsRadii);
    rec.define ("shape", latticeShape().asVector());
    return rec;
}

LCEllipsoid* LCEllipsoid::fromRecord (const TableRecord& rec,
				      const String&)
{
    return new LCEllipsoid (rec.asArrayFloat ("center"),
			    rec.asArrayFloat ("radii"),
			    Vector<Int>(rec.asArrayInt ("shape")));
}

void LCEllipsoid::fillCenter (const IPosition& center)
{
    itsCenter.resize (center.nelements());
    for (uInt i=0; i<center.nelements(); i++) {
	itsCenter(i) = center(i);
    }
}

Slicer LCEllipsoid::makeBox (const Vector<Float>& center,
			     const Vector<Float>& radii,
			     const IPosition& latticeShape)
{
    uInt nrdim = center.nelements();
    // First make sure dimensionalities conform.
    if (latticeShape.nelements() != nrdim  ||  radii.nelements() != nrdim) {
	throw (AipsError ("LCEllipsoid::LCEllipsoid - "
			  "dimensionality of center,radii,lattice mismatch"));
    }
    // Determine blc and trc. Note that float to int conversion truncates,
    // so add almost 1 to get correct blc.
    IPosition blc(nrdim);
    IPosition trc(nrdim);
    for (uInt i=0; i<nrdim; i++) {
	if (center(i) > latticeShape(i)-1  ||  center(i) < 0) {
	    throw (AipsError ("LCEllipsoid::LCEllipsoid - "
			      "invalid center (outside lattice)"));
	}
	blc(i) = Int(center(i) - radii(i) + 0.99);
	if (blc(i) < 0) {
	    blc(i) = 0;
	}
	trc(i) = Int(center(i) + radii(i));
	if (trc(i) >= latticeShape(i)) {
	    trc(i) = latticeShape(i) - 1;
	}
	if (blc(i) > trc(i)) {
	    throw (AipsError ("LCEllipsoid::LCEllipsoid - "
			      "ellipsoid is empty (radii too small)"));
	}
    }
    // In principle it is possible that e.g. the top of the ellipsoid falls
    // between 2 grid points, so we could leave out that line of pixels.
    return Slicer(blc, trc, Slicer::endIsLast);
}

void LCEllipsoid::defineMask()
{
    uInt i;
    // Create the mask with the shape of the bounding box.
    // Set the mask initially to False.
    const IPosition& length = box().length();
    uInt nrdim = length.nelements();
    Array<Bool> mask(length);
    mask = False;
    // Get access to the mask storage.
    Bool deleteIt;
    Bool* maskData = mask.getStorage (deleteIt);
    // Initialize some variables for the loop below.
    Float center0 = itsCenter(0) - box().start()(0);
    Float radsq0 = itsRadii(0) * itsRadii(0);
    Int np = length(0);
    IPosition pos (nrdim, 0);
    Vector<Float> center (nrdim);
    Vector<Float> radsq (nrdim);
    Vector<Float> dist (nrdim, 0.0);
    Float distsq = 0;
    for (i=1; i<nrdim; i++) {
	center(i) = itsCenter(i) - box().start()(i);
	Float d = center(i) / itsRadii(i);
	dist(i) = d * d;
	distsq += dist(i);
    }
    // Loop through all pixels in the ellipsoid.
    // The outer loop iterates over all lines.
    // The inner loop set the pixel mask for a line by calculating
    // the start and end of the ellipsoid for that line.
    // The variable distsq contains the 'distance' of the line to the center.
    for (;;) {
	// Ignore the line when the distance exceeds the radii.
	Float d = 1 - distsq;
	if (d >= 0) {
	    d = sqrt(d * radsq0);
	    Int st = Int(center0 - d + 0.99);
	    if (st < 0) {
		st = 0;
	    }
	    Int end = Int(center0 + d);
	    if (end >= np) {
		end = np-1;
	    }
	    for (Int j=st; j<=end; j++) {
		maskData[j] = True;
	    }
	}
	// Go to the next line and update the line distance.
	maskData += np;
	for (i=1; i<nrdim; i++) {
	    distsq -= dist(i);
	    if (++pos(i) < length(i)) {
		Float d = (center(i) - pos(i)) / itsRadii(i);
		dist(i) = d*d;
		distsq += dist(i);
		break;
	    }
	    // This dimension is done. Reset it and continue with the next.
	    pos(i) = 0;
	    Float d = center(i) / itsRadii(i);
	    dist(i) = d*d;
	    distsq += dist(i);
        }
	// End the iteration when all dimensions are done.
	if (i == nrdim) {
	    break;
	}
	DebugAssert (maskData == &mask(pos), AipsError);
    }
    mask.putStorage (maskData, deleteIt);
    setMask (mask);
}
