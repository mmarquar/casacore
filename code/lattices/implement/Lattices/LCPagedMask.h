//# LCPagedMask.h: Class to define a rectangular mask as a region
//# Copyright (C) 1998,1999
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

#if !defined(AIPS_LCPAGEDMASK_H)
#define AIPS_LCPAGEDMASK_H


//# Includes
#include <trial/Lattices/LCBox.h>
#include <trial/Lattices/PagedArray.h>


// <summary>
// Class to define a rectangular mask as a region
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=LCRegion>LCRegion</linkto>
// </prerequisite>

// <synopsis> 
// The LCPagedMask class is a specialization of class
// <linkto class=LCRegion>LCRegion</linkto>.
// </synopsis> 

// <example>
// <srcblock>
// </srcblock>
// </example>

// <todo asof="1997/11/11">
// </todo>

class LCPagedMask: public LCRegionSingle
{
public:
    LCPagedMask();

    // Construct a PagedMask object for (part of) a lattice.
    // The box defines the position of the mask.
    // The default mask shape is the lattice shape.
    // <group>
    LCPagedMask (const TiledShape& latticeShape, const String& tableName);
    LCPagedMask (const TiledShape& maskShape, const LCBox& box,
		 const String& tableName);
    LCPagedMask (PagedArray<Bool>& mask, const LCBox& box);
    // </group>

    // Copy constructor (copy semantics).
    LCPagedMask (const LCPagedMask& other);

    // Destructor
    virtual ~LCPagedMask();

    // Assignment (reference semantics).
    LCPagedMask& operator= (const LCPagedMask& other);

    // Comparison
    virtual Bool operator==(const LCRegion& other) const;

    // Make a copy of the derived object.
    virtual LCRegion* cloneRegion() const;

    // Handle deletion of the region by deleting the associated table.
    virtual void handleDelete();

    // Handle renaming the region by renaming the associated table.
    // If overwrite=False, an exception will be thrown if a table with the
    // new name already exists.
    virtual void handleRename (const String& newName, Bool overwrite);

    // Get the class name (to store in the record).
    static String className();

    // Region type. Returns class name.
    virtual String type() const;

    // Convert the (derived) object to a record.
    virtual TableRecord toRecord (const String& tableName) const;

    // Convert correct object from a record.
    static LCPagedMask* fromRecord (const TableRecord&,
				    const String& tablename);

    // An LCPagedMask is writable if the underlying PagedArray is.
    virtual Bool isWritable() const;

protected:
    // Construct another LCPagedMask (for e.g. another lattice) by moving
    // this one. It recalculates the bounding mask.
    // A positive translation value indicates "to right".
    virtual LCRegion* doTranslate (const Vector<Float>& translateVector,
				   const IPosition& newLatticeShape) const;

private:
    // Create the object from a record (for an existing mask).
    LCPagedMask (PagedArray<Bool>& mask,
		 const IPosition& blc,
		 const IPosition& latticeShape);


    LCBox            itsBox;
    PagedArray<Bool> itsMask;
};


#endif
