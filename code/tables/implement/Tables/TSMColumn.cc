//# TSMColumn.cc: Tiled Hypercube Storage Manager for table columns
//# Copyright (C) 1995,1996,1997
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

//# Includes
#include <aips/Tables/TSMColumn.h>
#include <aips/Tables/TSMDataColumn.h>
#include <aips/Tables/TSMCoordColumn.h>
#include <aips/Tables/TSMIdColumn.h>
#include <aips/Utilities/ValType.h>



TSMColumn::TSMColumn (TiledStMan* stman, int dataType,
		      const String& columnName)
: StManColumn (dataType),
  dtype_p     (dataType),
  stmanPtr_p  (stman),
  name_p      (columnName),
  colPtr_p    (0)
{}

TSMColumn::TSMColumn (const TSMColumn& that)
: StManColumn   (that.dtype_p),
  dtype_p       (that.dtype_p),
  stmanPtr_p    (that.stmanPtr_p),
  name_p        (that.name_p),
  columnShape_p (that.columnShape_p),
  colPtr_p      (0)
{}

TSMColumn::~TSMColumn()
{
    delete colPtr_p;
}

int TSMColumn::dataType() const
{
    return dtype_p;
}

void TSMColumn::setShapeColumn (const IPosition& shape)
{
    columnShape_p = shape;
}


TSMDataColumn* TSMColumn::makeDataColumn()
{
    TSMDataColumn* colPtr = new TSMDataColumn (*this);
    colPtr_p = colPtr;
    return colPtr;
}
TSMCoordColumn* TSMColumn::makeCoordColumn (uInt axesNumber)
{
    TSMCoordColumn* colPtr = new TSMCoordColumn (*this, axesNumber);
    colPtr_p = colPtr;
    return colPtr;
}
TSMIdColumn* TSMColumn::makeIdColumn()
{
    TSMIdColumn* colPtr = new TSMIdColumn (*this);
    colPtr_p = colPtr;
    return colPtr;
}

TSMColumn* TSMColumn::unlink()
{
    TSMColumn* ptr = colPtr_p;
    colPtr_p = 0;               // do not delete linked object in destructor
    return ptr;
}
