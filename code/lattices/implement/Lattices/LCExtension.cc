//# LCExtension.cc: Extend an LCRegion along straight lines to other dimensions
//# Copyright (C) 1998
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


#include <trial/Lattices/LCExtension.h>
#include <aips/Arrays/Vector.h>
#include <aips/Tables/TableRecord.h>
#include <aips/Exceptions/Error.h>

typedef Vector<Int> lcextension_gppbug1;


LCExtension::LCExtension()
{}

LCExtension::LCExtension (const LCRegion& region,
			  const IPosition& expandAxes,
			  const IPosition& latticeShape)
: LCRegionMulti (region.clone(), latticeShape),
  itsExpandAxes (expandAxes),
  itsBlc (IPosition(expandAxes.nelements(), 0)),
  itsTrc (IPosition(expandAxes.nelements(), 0))
{
    //# Default trc is lattice shape.
    for (uInt i=0; i<expandAxes.nelements(); i++) {
        uInt axis = expandAxes(i);
	if (axis < latticeShape.nelements()) {
	    itsTrc = latticeShape(axis) - 1;
	}
    }
    defineBox();
}

LCExtension::LCExtension (const LCRegion& region,
			  const IPosition& expandAxes,
			  const IPosition& expandBlc,
			  const IPosition& expandTrc,
			  const IPosition& latticeShape)
: LCRegionMulti (region.clone(), latticeShape),
  itsExpandAxes (expandAxes),
  itsBlc (expandBlc),
  itsTrc (expandTrc)
{
    defineBox();
}

LCExtension::LCExtension (const LCExtension& other)
: LCRegionMulti (other),
  itsExpandAxes (other.itsExpandAxes),
  itsRegionAxes (other.itsRegionAxes),
  itsBlc (other.itsBlc),
  itsTrc (other.itsTrc)
{}

LCExtension::~LCExtension()
{}

LCExtension& LCExtension::operator= (const LCExtension& other)
{
    if (this != &other) {
	LCRegionMulti::operator= (other);
	itsExpandAxes.resize (other.itsExpandAxes.nelements());
	itsRegionAxes.resize (other.itsRegionAxes.nelements());
	itsBlc.resize (other.itsExpandAxes.nelements());
	itsTrc.resize (other.itsExpandAxes.nelements());
	itsExpandAxes = other.itsExpandAxes;
	itsRegionAxes = other.itsRegionAxes;
	itsBlc = other.itsBlc;
	itsTrc = other.itsTrc;
    }
    return *this;
}

LCRegion* LCExtension::clone() const
{
    return new LCExtension (*this);
}

LCRegion* LCExtension::doTranslate (const Vector<Float>& translateVector,
				    const IPosition& newLatticeShape) const
{
    uInt i;
    // We need to separate the region axes and the expand axes.
    uInt nre = itsExpandAxes.nelements();
    uInt nrr = itsRegionAxes.nelements();
    IPosition expBlc (itsBlc);
    IPosition expTrc (itsTrc);
    Vector<Float> transReg (nrr);
    IPosition newShape (nrr);
    for (i=0; i<nre; i++) {
        uInt axis = itsExpandAxes(i);
        expBlc(i) += Int(translateVector(axis));
        expTrc(i) += Int(translateVector(axis));
    }
    for (i=0; i<nrr; i++) {
        uInt axis = itsRegionAxes(i);
	transReg(i) = translateVector(axis);
	newShape(i) = newLatticeShape(axis);
    }
    // Translate the region and create a new LCExtension with
    // the translated blc/trc.
    LCRegion* reg = region().translate (transReg, newShape);
    LCExtension* ext = new LCExtension (*reg, itsExpandAxes, expBlc, expTrc,
					newLatticeShape);
    delete reg;
    return ext;
}

String LCExtension::className()
{
    return "LCExtension";
}

TableRecord LCExtension::toRecord (const String& tableName) const
{
    TableRecord rec;
    rec.define ("name", className());
    rec.defineRecord ("region", region().toRecord(tableName));
    rec.define ("axes", itsExpandAxes.asVector());
    rec.define ("blc", itsBlc.asVector());
    rec.define ("trc", itsTrc.asVector());
    rec.define ("shape", latticeShape().asVector());
    return rec;
}

LCExtension* LCExtension::fromRecord (const TableRecord& rec,
				      const String& tableName)
{
    LCRegion* reg = LCRegion::fromRecord (rec.asRecord("region"), tableName);
    LCExtension* ext = new LCExtension (*reg,
					Vector<Int>(rec.asArrayInt ("axes")),
					Vector<Int>(rec.asArrayInt ("blc")),
					Vector<Int>(rec.asArrayInt ("trc")),
					Vector<Int>(rec.asArrayInt ("shape")));
    delete reg;
    return ext;
}

void LCExtension::defineBox()
{
    uInt i;
    // Check if the basic things are right.
    uInt nr = itsExpandAxes.nelements();
    uInt nrdim = latticeShape().nelements();
    if (itsBlc.nelements() != nr  ||  itsTrc.nelements() != nr) {
	throw (AipsError ("LCExtension::LCExtension - "
			  "lengths of expandAxes, Blc, and Trc differ"));
    }
    const IPosition& regionBlc = region().box().start();
    const IPosition& regionTrc = region().box().end();
    if (regionBlc.nelements() != nrdim-nr) {
	throw (AipsError ("LCExtension::LCExtension - "
			  "#expandAxes should be equal to difference "
			  "in dimensionality of lattice and region"));
    }
    // Expand the axes to all of them, which will also check if the
    // axes have been specified correctly.
    // The specified axes are the first ones, thereafter the remaining axes.
    IPosition allAxes = IPosition::makeAxisPath (nrdim, itsExpandAxes);
    IPosition blc (nrdim);
    IPosition trc (nrdim);
    for (i=0; i<nr; i++) {
        uInt axis = allAxes(i);
        if (itsTrc(i) < itsBlc(i)) {
	    throw (AipsError ("LCExtension::LCExtension - "
			      "expandBlc > expandTrc"));
	}
	blc(axis) = max(0, itsBlc(i));
	trc(axis) = min(latticeShape()(axis)-1, itsTrc(i));
    }
    itsRegionAxes.resize (nrdim-nr);
    for (i=nr; i<nrdim; i++) {
        uInt axis = allAxes(i);
	if (latticeShape()(axis) != region().latticeShape()(i-nr)) {
	    throw (AipsError ("LCExtension::LCExtension - "
			      "lengths of corresponding axes in lattice "
			      "and region lattice should match"));
	}
	blc(axis) = regionBlc(i-nr);
	trc(axis) = regionTrc(i-nr);
	itsRegionAxes(i-nr) = axis;
    }
    setBox (Slicer(blc, trc, Slicer::endIsLast));
    fillHasMask();
}


void LCExtension::multiGetSlice (Array<Bool>& buffer,
				 const Slicer& section)
{
    buffer.resize (section.length());
    uInt i;
    uInt nre = itsExpandAxes.nelements();
    uInt nrr = itsRegionAxes.nelements();
    // Read the required region section.
    // This means we have to create a Slicer for those axes only.
    IPosition blc(nrr);
    IPosition len(nrr);
    IPosition inc(nrr);
    IPosition shape(buffer.ndim(), 1);
    for (i=0; i<nrr; i++) {
        uInt axis = itsRegionAxes(i);
	blc(i) = section.start()(axis);
	len(i) = section.length()(axis);
	inc(i) = section.stride()(axis);
	shape(axis) = len(i);
    }
    Array<Bool> tmpbuf(len);
    LCRegion* reg = (LCRegion*)(regions()[0]);
    reg->doGetSlice (tmpbuf, Slicer(blc, len, inc));
    // Reform tmpbuf, so it has the same dimensionality as buffer.
    Array<Bool> mask = tmpbuf.reform (shape);
    // Now we have to expand tmpbuf along all expand axes.
    const IPosition& length = section.length();
    IPosition pos (buffer.ndim(), 0);
    IPosition end (buffer.shape() - 1);
    //# Iterate along itsExpandAxes (the new axes) through the new mask.
    for (;;) {
	for (i=0; i<nre; i++) {
	    end(itsExpandAxes(i)) = pos(itsExpandAxes(i));
	}
	//# Set each section of the mask to the mask of the region.
	buffer(pos,end) = mask;
	//# Go to the next section.
	for (i=0; i<nre; i++) {
	    if (++pos(itsExpandAxes(i)) < length(itsExpandAxes(i))) {
		break;
	    }
	    // This dimension is done. Reset it and continue with the next.
	    pos(itsExpandAxes(i)) = 0;
        }
	//# End the iteration when all dimensions are done.
	if (i == nre) {
	    break;
	}
    }
}

IPosition LCExtension::doNiceCursorShape (uInt maxPixels) const
{
    return Lattice<Bool>::doNiceCursorShape (maxPixels);
}
