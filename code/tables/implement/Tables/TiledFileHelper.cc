//# TiledFileHelper.cc: Helper class for tiled access to an array in a file
//# Copyright (C) 2001
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


#include <trial/Tables/TiledFileHelper.h>
#include <aips/Tables/TSMFile.h>
#include <aips/Tables/ArrColDesc.h>
#include <aips/Tables/TableError.h>
#include <aips/Arrays/Vector.h>


TiledFileHelper::TiledFileHelper (const String& fileName,
				  const IPosition& shape,
				  DataType dtype,
				  uInt maximumCacheSize,
				  Bool writable,
				  Bool canonical)
: TiledStMan ("TiledFileHelper", maximumCacheSize)
{
  switch (dtype) {
  case TpFloat:
    itsDesc.addColumn (ArrayColumnDesc<Float> ("DATA", shape,
					       ColumnDesc::FixedShape));
    break;
  case TpDouble:
    itsDesc.addColumn (ArrayColumnDesc<Double> ("DATA", shape,
						ColumnDesc::FixedShape));
    break;
  case TpComplex:
    itsDesc.addColumn (ArrayColumnDesc<Complex> ("DATA", shape,
						 ColumnDesc::FixedShape));
    break;
  case TpDComplex:
    itsDesc.addColumn (ArrayColumnDesc<DComplex> ("DATA", shape,
						  ColumnDesc::FixedShape));
    break;
  default:
    throw TableError ("TiledFileHelper: invalid data type");
  }
  itsDesc.defineHypercolumn ("TiledFileHelper", shape.nelements(),
			     Vector<String>(1, "DATA"));
  createDirArrColumn ("DATA", dtype, "");
  TiledStMan::setup (canonical);
  fileSet_p[0] = new TSMFile (fileName, writable);
}

TiledFileHelper::~TiledFileHelper()
{}

const TableDesc& TiledFileHelper::getDesc() const
{
  return itsDesc;
}


String TiledFileHelper::dataManagerType() const
{
  return "TiledFileHelper";
}

DataManager* TiledFileHelper::clone() const
{
  throw AipsError ("TileFileHelper::clone - not implemented");
}
Bool TiledFileHelper::flush (AipsIO&, Bool)
{
  throw AipsError ("TileFileHelper::flush - not implemented");
  return False;
}
void TiledFileHelper::create (uInt)
{
  throw AipsError ("TileFileHelper::create - not implemented");
}
TSMCube* TiledFileHelper::getHypercube (uInt)
{
  throw AipsError ("TileFileHelper::getHypercube - not implemented");
  return 0;
}
TSMCube* TiledFileHelper::getHypercube (uInt, IPosition&)
{
  throw AipsError ("TileFileHelper:getHypercube: - not implemented");
  return 0;
}
void TiledFileHelper::readHeader (uInt, Bool)
{
  throw AipsError ("TileFileHelper::readHeader - not implemented");
}
