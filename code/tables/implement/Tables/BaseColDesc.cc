//# BaseColDesc.cc: Abstract base class for table column descriptions
//# Copyright (C) 1994,1995,1996,1997,1999,2000
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

#include <aips/Tables/BaseColDesc.h>
#include <aips/Tables/ColumnDesc.h>
#include <aips/Tables/RefColumn.h>
#include <aips/Tables/DataManager.h>
#include <aips/Tables/TableRecord.h>
#include <aips/Tables/TableError.h>
#include <aips/IO/AipsIO.h>
#include <aips/Arrays/IPosition.h>
#include <iostream.h>


BaseColumnDesc::BaseColumnDesc (const String& name, const String& comment,
				const String& dataManType,
				const String& dataManGroup,
				DataType dt, const String& dtId,
				int opt, uInt ndim, const IPosition& shape,
				Bool isScalar, Bool isArray, Bool isTable)
: colName_p     (name),
  comment_p     (comment),
  dataManType_p (dataManType),
  dataManGroup_p(dataManGroup),
  dtype_p       (dt),
  dtypeId_p     (dtId),
  option_p      (opt),
  nrdim_p       (ndim),
  shape_p       (shape),
  maxLength_p   (0),
  isScalar_p    (isScalar),
  isArray_p     (isArray),
  isTable_p     (isTable)
{
    //# Option Direct forces FixedShape.
    if ((option_p & ColumnDesc::Direct)  ==  ColumnDesc::Direct) {
	option_p |= ColumnDesc::FixedShape;
    }
    // A shape can only be given for a FixedShape array.
    if (shape_p.nelements() > 0) {
	if ((option_p & ColumnDesc::FixedShape)  !=  ColumnDesc::FixedShape) {
	    throw (TableInvColumnDesc (name,
			          "Shape only allowed for FixedShape arrays"));
	}
    }
    // Option Undefined can only be set for standard types.
    if (dtype_p == TpOther) {
	if ((option_p & ColumnDesc::Undefined)  ==  ColumnDesc::Undefined) {
	    throw (TableInvColumnDesc (name,
		    "Option Undefined only allowed for standard data types"));
	}
    }
    // The default data manager for standard types is StandardStMan.
    // For other types it is the virtual scalar column engine handling
    // that type.
    if (dataManType_p.empty()) {
	if (dtype_p == TpOther) {
	    dataManType_p = dtypeId_p + "VSCEngine";
	}else{
	    dataManType_p = "StandardStMan";
	}
    }
    // The default data manager group for standard types is data manager type.
    // For other types it is the column name to make the group unique.
    if (dataManGroup_p.empty()) {
	if (dtype_p == TpOther) {
	    dataManGroup_p = colName_p;
	}else{
	    dataManGroup_p = dataManType_p;
	}
    }
    keySetPtr_p = new TableRecord();
    if (keySetPtr_p == 0) {
	throw (AllocError("BaseColumnDesc",1));
    }
}

BaseColumnDesc::BaseColumnDesc (const BaseColumnDesc& that)
: colName_p     (that.colName_p),
  comment_p     (that.comment_p),
  dataManType_p (that.dataManType_p),
  dataManGroup_p(that.dataManGroup_p),
  dtype_p       (that.dtype_p),
  dtypeId_p     (that.dtypeId_p),
  option_p      (that.option_p),
  nrdim_p       (that.nrdim_p),
  shape_p       (that.shape_p),
  maxLength_p   (that.maxLength_p),
  isScalar_p    (that.isScalar_p),
  isArray_p     (that.isArray_p),
  isTable_p     (that.isTable_p)
{
    keySetPtr_p = new TableRecord(*that.keySetPtr_p);
    if (keySetPtr_p == 0) {
	throw (AllocError("BaseColumnDesc",1));
    }
}
  
BaseColumnDesc::~BaseColumnDesc ()
    { delete keySetPtr_p; }

BaseColumnDesc& BaseColumnDesc::operator= (const BaseColumnDesc& that)
{
    colName_p      = that.colName_p;
    comment_p      = that.comment_p;
    dataManType_p  = that.dataManType_p;
    dataManGroup_p = that.dataManGroup_p;
    dtype_p        = that.dtype_p;
    dtypeId_p      = that.dtypeId_p;
    option_p       = that.option_p;
    nrdim_p        = that.nrdim_p;
    shape_p.resize (that.shape_p.nelements());
    shape_p        = that.shape_p;
    maxLength_p    = that.maxLength_p;
    *keySetPtr_p   = *that.keySetPtr_p;
    isScalar_p     = that.isScalar_p;
    isArray_p      = that.isArray_p;
    isTable_p      = that.isTable_p;
    return *this;
}


//# Derived classes can implement their own versions
//# and call this basic implementation when needed.

void BaseColumnDesc::checkAdd (const ColumnDescSet&) const
{}
void BaseColumnDesc::checkRename (const ColumnDescSet&, const String&) const
{}
void BaseColumnDesc::handleAdd (ColumnDescSet&)
{}
void BaseColumnDesc::handleRename (ColumnDescSet&, const String&)
{}
void BaseColumnDesc::handleRemove (ColumnDescSet&)
{}
void BaseColumnDesc::renameAction (const String&, const String&)
{}


// Dimensionality can only be changed if not set yet.
void BaseColumnDesc::setNdim (uInt ndim)
{
    if (!isArray()) {
	throw (TableInvOper ("setNdim: column "
			     + colName_p + " is no array"));
    }
    if (nrdim_p > 0) {
	throw (TableInvOper ("setNdim(): dimensionality of column "
			     + colName_p + " already defined"));
    }
    nrdim_p = ndim;
}
void BaseColumnDesc::setShape (const IPosition& shape)
{
    if (!isArray()) {
	throw (TableInvOper ("setShape: column "
			     + colName_p + " is no array"));
    }
    if (shape_p.nelements() > 0) {
	throw (TableInvOper ("setShape(): shape of column "
			     + colName_p + " already defined"));
    }
    if (nrdim_p > 0  &&  Int(shape.nelements()) != nrdim_p) {
	throw (TableInvOper ("setShape(): dimensionality of column "
			     + colName_p + " mismatches new shape"));
    }
    shape_p = shape;
    nrdim_p = shape.nelements();
    option_p |= ColumnDesc::FixedShape;
}

void BaseColumnDesc::setMaxLength (uInt maxLength)
{
    if (dtype_p != TpString) {
	throw (TableInvOper ("setMaxLength: column "
			     + colName_p + " contains no string values"));
    }
    maxLength_p = maxLength;
}


//# By default no table description gets returned.
TableDesc* BaseColumnDesc::tableDesc()
{
    throw (TableInvOper
	   ("tableDesc(): column " + colName_p + " is no subtable"));
    return 0;
}


//# Put the XXXColumnDesc object.
//# First the base class variables are written, then the virtual
//# function putDesc is called to write the specific variables.
//# Note that the data is read back by the ctor taking AipsIO.
//# It was felt that putstart takes too much space, so therefore
//# the version is put "manually".
void BaseColumnDesc::putFile (AipsIO& ios, const String& parentTableName) const
{
    ios << (uInt)1;                  // class version 1
    ios << colName_p;
    ios << comment_p;
    ios << dataManType_p;
    ios << dataManGroup_p;
    int dt = dtype_p;
    ios << dt;
    ios << option_p;
    ios << nrdim_p;
    if (!isScalar_p > 0) {
	ios << shape_p;
    }
    ios << maxLength_p;
    keySetPtr_p->putRecord (ios, parentTableName);
    putDesc(ios);
}


void BaseColumnDesc::getFile (AipsIO& ios, Bool isTableWritable,
			      const String& parentTableName)
{
    uInt version;
    ios >> version;
    ios >> colName_p;
    ios >> comment_p;
    ios >> dataManType_p;
    ios >> dataManGroup_p;
    int dtype;
    ios >> dtype;
    if (dtype != dtype_p) {
	throw (TableInternalError ("BaseColumnDesc: data type read mismatch"));
    }
    ios >> option_p;
    ios >> nrdim_p;
    if (!isScalar_p > 0) {
	ios >> shape_p;
    }
    ios >> maxLength_p;
    keySetPtr_p->getRecord (ios, isTableWritable, parentTableName);
    getDesc (ios);
}


//# Create a RefColumn object from the description.
RefColumn* BaseColumnDesc::makeRefColumn (RefTable* rtp, BaseColumn* bcp) const
{
    RefColumn* rcp = new RefColumn (this, rtp, bcp);
    if (rcp == 0) {
	throw (AllocError ("BaseColumnDesc::makeRefColumn", 1));
    }
    return rcp;
}
