//# ImageRegion.cc: Class to hold a region of interest in an image
//# Copyright (C) 1998,1999,2000,2001,2002
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

#include <trial/Images/ImageRegion.h>
#include <trial/Images/WCRegion.h>
#include <trial/Images/WCUnion.h>
#include <trial/Images/WCIntersection.h>
#include <trial/Images/WCDifference.h>
#include <trial/Images/WCComplement.h>
#include <trial/Lattices/LCRegion.h>
#include <trial/Lattices/LCSlicer.h>
#include <trial/Lattices/RegionType.h>
#include <trial/Coordinates/CoordinateSystem.h>
#include <aips/Tables/TableRecord.h>
#include <aips/Arrays/Vector.h>
#include <aips/Utilities/String.h>
#include <aips/Utilities/Assert.h>
#include <aips/Exceptions/Error.h>

ImageRegion::ImageRegion()
: LattRegionHolder (uInt(0)),
  itsWC            (0)
{}

ImageRegion::ImageRegion (const LCRegion& region)
: LattRegionHolder (region),
  itsWC            (0)
{}

ImageRegion::ImageRegion (const LCSlicer& slicer)
: LattRegionHolder (slicer),
  itsWC            (0)
{}

ImageRegion::ImageRegion (const WCRegion& region)
: LattRegionHolder (region.ndim()),
  itsWC            (region.cloneRegion())
{}

ImageRegion::ImageRegion (LCRegion* region)
: LattRegionHolder (region),
  itsWC            (0)
{}

ImageRegion::ImageRegion (LCSlicer* slicer)
: LattRegionHolder (slicer),
  itsWC            (0)
{}

ImageRegion::ImageRegion (WCRegion* region)
: LattRegionHolder (region->ndim()),
  itsWC            (region)
{}

ImageRegion::ImageRegion (const ImageRegion& other)
: LattRegionHolder (other),
  itsWC            (other.itsWC)
{
    if (itsWC != 0) {
	itsWC = itsWC->cloneRegion();
    }
}

ImageRegion::~ImageRegion()
{
    delete itsWC;
}

ImageRegion& ImageRegion::operator= (const ImageRegion& other)
{
    if (this != &other) {
	LattRegionHolder::operator= (other);
        delete itsWC;
	itsWC = other.itsWC;
	if (itsWC != 0) {
	    itsWC = itsWC->cloneRegion();
	}
    }
    return *this;
}

ImageRegion* ImageRegion::clone() const
{
    return new ImageRegion (*this);
}

Bool ImageRegion::operator== (const LattRegionHolder& other) const
{
    if (! LattRegionHolder::operator== (other)) {
	return False;
    }
    if (itsWC != 0) {
	return (*itsWC == *other.asWCRegionPtr());
    }
    return True;
}

Bool ImageRegion::isWCRegion() const
{
    return  (itsWC != 0);
}

const WCRegion* ImageRegion::asWCRegionPtr() const
{
    AlwaysAssert (isWCRegion(), AipsError);
    return itsWC;
}

LCRegion& ImageRegion::asMask()
{
    AlwaysAssert (isLCRegion(), AipsError);
    LCRegion* regPtr = const_cast<LCRegion*>(asLCRegionPtr());
    AlwaysAssert (regPtr->isWritable(), AipsError);
    return *regPtr;
}

LatticeRegion ImageRegion::toLatticeRegion (const CoordinateSystem& cSys,
					    const IPosition& shape) const
{
    if (isLCRegion()) {
	return LatticeRegion (asLCRegion());
    }
    if (isLCSlicer()) {
	return LatticeRegion (asLCSlicer().toSlicer (cSys.referencePixel(),
						     shape),
			      shape);
    }
    // LatticeRegion takes over the created LCRegion pointer,
    // so it does not need to be deleted.
    // This is the top conversion, so use all axes.
    return LatticeRegion (toLCRegion (cSys, shape));
}

LCRegion* ImageRegion::toLCRegion (const CoordinateSystem& cSys,
				   const IPosition& shape) const
{
    // Convert the region to an LCRegion.
    LCRegion* region = 0;
    if (isLCRegion()) {
	region = asLCRegion().cloneRegion();
    } else if (isWCRegion()) {
        region = itsWC->toLCRegion (cSys, shape);
    } else {
	throw (AipsError ("ImageRegion::toLCRegion - "
			  " cannot convert its LCSlicer object to LCRegion"));
    }
    return region;
}

TableRecord ImageRegion::toRecord (const String& tableName) const
{
    TableRecord record;
    if (isLCRegion()) {
        return asLCRegion().toRecord (tableName);
    }
    if (isWCRegion()) {
        return itsWC->toRecord (tableName);
    }
    return asLCSlicer().toRecord (tableName);
}

ImageRegion* ImageRegion::fromRecord (const TableRecord& record,
				      const String& tableName)
{
    // See if this is a region record.
    if (! record.isDefined ("isRegion")) {
	throw (AipsError ("ImageRegion::fromRecord - "
			  "record does not define a region"));
    }
    // Convert to correct region object.
    // Note that in the following the ImageRegion constructors take
    // over the pointer returned by fromRecord.
    Int regionType = record.asInt ("isRegion");       
    if (regionType == RegionType::LC) {
	return new ImageRegion (LCRegion::fromRecord (record, tableName));
    }
    if (regionType == RegionType::WC) {
	return new ImageRegion (WCRegion::fromRecord (record, tableName));
    } else if (regionType != RegionType::ArrSlicer) {
	throw (AipsError ("ImageRegion::fromRecord - "
			  "record has an unknown region type"));
    }
    return new ImageRegion (LCSlicer::fromRecord (record, tableName));
}


LattRegionHolder* ImageRegion::makeUnion
                                (const LattRegionHolder& other) const
{
    if (! isWCRegion()) {
    return LattRegionHolder::makeUnion (other);
    }
	return new ImageRegion
           (new WCUnion (*asWCRegionPtr(), *other.asWCRegionPtr()));
}
LattRegionHolder* ImageRegion::makeIntersection
                                (const LattRegionHolder& other) const
{
    if (isWCRegion()) {
	return new ImageRegion
           (new WCIntersection (*asWCRegionPtr(), *other.asWCRegionPtr()));
    }
    return LattRegionHolder::makeIntersection (other);
}
LattRegionHolder* ImageRegion::makeDifference
                                (const LattRegionHolder& other) const
{
    if (isWCRegion()) {
	return new ImageRegion
           (new WCDifference (*asWCRegionPtr(), *other.asWCRegionPtr()));
    }
    return LattRegionHolder::makeDifference (other);
}
LattRegionHolder* ImageRegion::makeComplement() const
{
    if (isWCRegion()) {
	return new ImageRegion
           (new WCComplement (*asWCRegionPtr()));
    }
    return LattRegionHolder::makeComplement();
}
