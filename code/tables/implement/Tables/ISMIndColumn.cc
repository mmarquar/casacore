//# ISMIndColumn.cc: Column of Incremental storage manager for indirect arrays
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

//# Includes
#include <aips/Tables/ISMIndColumn.h>
#include <aips/Tables/ISMBucket.h>
#include <aips/Utilities/DataType.h>
#include <aips/Arrays/Array.h>
#include <aips/Lattices/Slicer.h>
#include <aips/Mathematics/Complex.h>
#include <aips/Utilities/String.h>
#include <aips/Utilities/Assert.h>
#include <aips/OS/CanonicalConversion.h>
#include <aips/Tables/DataManError.h>


ISMIndColumn::ISMIndColumn (ISMBase* smptr, int dataType, uInt colnr)
: ISMColumn     (smptr, dataType, colnr),
  seqnr_p       (smptr->uniqueNr()),
  shapeIsFixed_p(False),
  iosfile_p     (0),
  indArray_p    (0)
{}

ISMIndColumn::~ISMIndColumn()
{
    delete (uLong*)lastValue_p;
    lastValue_p = 0;
    delete iosfile_p;
}


//# Create the array file (for a new column).
//# Compose the file name from the mother file name extended with
//# the unique column sequence nr.
void ISMIndColumn::doCreate (ISMBucket* bucket)
{
    getFile (0);
    // Insert a dummy zero offset as the first value.
    *(uLong*)lastValue_p = 0;
    char* buffer = stmanPtr_p->tempBuffer();
    uInt leng = writeFunc_p (buffer, lastValue_p, 1);
    bucket->addData (colnr_p, 0, 0, buffer, leng);
}
void ISMIndColumn::getFile (uInt nrrow)
{
    init();
    //# Create the type 1 file to hold the arrays in the column.
    char strc[8];
    sprintf (strc, "i%i", seqnr_p);
    iosfile_p = new StManArrayFile (stmanPtr_p->fileName() + strc,
				    stmanPtr_p->fileOption(), 1,
				    stmanPtr_p->asCanonical());
    if (iosfile_p == 0) {
	throw (AllocError ("ISMIndColumn::doCreate", 1));
    }
    lastRowPut_p = nrrow;
}
Bool ISMIndColumn::flush (uInt, Bool fsync)
{
    return iosfile_p->flush (fsync);
}
void ISMIndColumn::reopenRW()
{
    iosfile_p->reopenRW();
}

void ISMIndColumn::addRow (uInt newNrrow, uInt oldNrrow)
{
    // If the shape is fixed and if the first row is added, define
    // an array to have an array for all rows.
    // Later rows get the value of a previous row, so we don't have to
    // do anything for them.
    if (oldNrrow == 0  &&  shapeIsFixed_p) {
	    putShape (0, fixedShape_p);
    }
}

void ISMIndColumn::setShapeColumn (const IPosition& shape)
{
    fixedShape_p   = shape;
    shapeIsFixed_p = True;
}

void ISMIndColumn::setShape (uInt rownr, const IPosition& shape)
{
    StIndArray* ptr = getArrayPtr (rownr);
    if (ptr != 0) {
	ptr->getShape (*iosfile_p);
    }
    if (ptr == 0  ||  !shape.isEqual (ptr->shape())) {
	putShape (rownr, shape);
    }
}

//# Get the shape for the array (if any) in the given row.
//# Read shape if not read yet.
StIndArray* ISMIndColumn::getArrayPtr (uInt rownr)
{
    if (isLastValueInvalid (rownr)) {
	getValue (rownr, lastValue_p);
	uLong offset = *(uLong*)lastValue_p;
	if (offset != 0) {
	    indArray_p = StIndArray (offset);
	    foundArray_p = True;
	}else{
	    foundArray_p = False;
	}
    }
    if (foundArray_p) {
	return &indArray_p;
    }
    return 0;
}

//# Get the shape for the array (if any) in the given row.
//# Read shape if not read yet.
StIndArray* ISMIndColumn::getShape (uInt rownr)
{
    StIndArray* ptr = getArrayPtr (rownr);
    if (ptr == 0) {
	throw (DataManInvOper ("StMan: no array in this row"));
    }
    ptr->getShape (*iosfile_p);
    return ptr;
}

//# Set the shape for the array in the given row for a put operation.
StIndArray* ISMIndColumn::putShape (uInt rownr, const IPosition& shape)
{
    //# Insert an entry for this row and set its shape.
    //# Nothing will be done if it is already defined.
    return putArrayPtr (rownr, shape, False);
//    StIndArray* ptr = putArrayPtr (rownr, shape, False);
//    ptr->setShape (*iosfile_p, dataType(), shape);
//    return ptr;
}

//# Set the shape for the array (if any) in the given row for a sliced
//# put operation.
StIndArray* ISMIndColumn::putShapeSliced (uInt rownr)
{
    //# Get the shape of this row and define it again.
    //# Defining is necessary, because the shape gotten may be valid for
    //# row 10-20, while the put is only for row 15. In that case row 15
    //# has to be inserted.
    StIndArray* ptr = getShape (rownr);
    return putArrayPtr (rownr, ptr->shape(), True);
}

Bool ISMIndColumn::isShapeDefined (uInt rownr)
    { return (getArrayPtr(rownr) == 0  ?  False : True); }

uInt ISMIndColumn::ndim (uInt rownr)
    { return getShape(rownr)->shape().nelements(); }

IPosition ISMIndColumn::shape (uInt rownr)
    { return getShape(rownr)->shape(); }

Bool ISMIndColumn::canChangeShape() const
    { return (shapeIsFixed_p  ?  False : True); }


Bool ISMIndColumn::canAccessSlice (Bool& reask) const
{
    reask = False;
    return True;
}


StIndArray* ISMIndColumn::putArrayPtr (uInt rownr, const IPosition& shape,
				       Bool copyData)
{
    // Start with getting the array pointer. This gives the range
    // for which this array is valid.
    StIndArray* ptr = getArrayPtr (rownr);
    // When the shape is put for the last row ever put, don't do it
    // when the shape is equal. This is needed to get correct behaviour
    // when an array is put. Putting an array calls setShape and then
    // putXXXV resulting in two calls to putShape.
    if (ptr != 0  &&  rownr+1 == lastRowPut_p) {
	ptr->getShape (*iosfile_p);
	if (shape == ptr->shape()) {
	    return ptr;
	}
    }
    // When the interval contains a single row, we can do a simple replace
    // if the value is not shared.
    if (ptr != 0  &&  startRow_p == endRow_p) {
	if (ptr->refCount (*iosfile_p) <= 1) {
	    // The value is not shared, so we can replace it.
	    ptr->setShape (*iosfile_p, dataType(), shape);
	    uLong offset = ptr->fileOffset();
	    putValue (rownr, &offset);
	    return ptr;
	}
    }
    // Make a new IndArray object.
    StIndArray tmp(0);
    tmp.setShape (*iosfile_p, dataType(), shape);
    if (copyData) {
	tmp.copyData (*iosfile_p, dataType(), *ptr);
    }
    indArray_p = tmp;
    uLong offset = indArray_p.fileOffset();
    putValue (rownr, &offset);
    return &indArray_p;
}


void ISMIndColumn::getArrayfloatV (uInt rownr, Array<float>* arr)
    { getShape(rownr)->getArrayfloatV (*iosfile_p, arr); }

void ISMIndColumn::putArrayfloatV (uInt rownr, const Array<float>* arr)
    { putShape(rownr, arr->shape())->putArrayfloatV (*iosfile_p, arr); }

void ISMIndColumn::getSlicefloatV (uInt rownr, const Slicer& ns,
				   Array<float>* arr)
    { getShape(rownr)->getSlicefloatV (*iosfile_p, ns, arr); }

void ISMIndColumn::putSlicefloatV (uInt rownr, const Slicer& ns,
				   const Array<float>* arr)
    { putShapeSliced(rownr)->putSlicefloatV (*iosfile_p, ns, arr); }
    

#define ISMIndColumn_GETPUT(T,NM) \
void ISMIndColumn::aips_name2(getArray,NM) (uInt rownr, Array<T>* arr) \
    { getShape(rownr)->aips_name2(getArray,NM) (*iosfile_p, arr); } \
void ISMIndColumn::aips_name2(putArray,NM) (uInt rownr, const Array<T>* arr) \
    { putShape(rownr, arr->shape())->aips_name2(putArray,NM) \
	                                                (*iosfile_p, arr); } \
void ISMIndColumn::aips_name2(getSlice,NM) \
                             (uInt rownr, const Slicer& ns, Array<T>* arr) \
    { getShape(rownr)->aips_name2(getSlice,NM) (*iosfile_p, ns, arr); } \
void ISMIndColumn::aips_name2(putSlice,NM) \
                        (uInt rownr, const Slicer& ns, const Array<T>* arr) \
    { putShapeSliced(rownr)->aips_name2(putSlice,NM) (*iosfile_p, ns, arr); }

ISMIndColumn_GETPUT(Bool,BoolV)
ISMIndColumn_GETPUT(uChar,uCharV)
ISMIndColumn_GETPUT(Short,ShortV)
ISMIndColumn_GETPUT(uShort,uShortV)
ISMIndColumn_GETPUT(Int,IntV)
ISMIndColumn_GETPUT(uInt,uIntV)
//#//ISMIndColumn_GETPUT(float,floatV)
ISMIndColumn_GETPUT(double,doubleV)
ISMIndColumn_GETPUT(Complex,ComplexV)
ISMIndColumn_GETPUT(DComplex,DComplexV)
ISMIndColumn_GETPUT(String,StringV)


Bool ISMIndColumn::compareValue (const void*, const void*) const
{
    return False;
}

void ISMIndColumn::init()
{
    DebugAssert (nrelem_p==1, AipsError);
    Bool asCanonical = stmanPtr_p->asCanonical();
    if (asCanonical) {
	readFunc_p    = CanonicalConversion::getToLocal ((uLong*)0);
	writeFunc_p   = CanonicalConversion::getFromLocal ((uLong*)0);
	fixedLength_p = CanonicalConversion::canonicalSize ((uLong*)0);
	nrcopy_p      = 1;
    }else{
	readFunc_p = writeFunc_p = Conversion::valueCopy;
	fixedLength_p = nrcopy_p = sizeof(uLong);
    }
    lastValue_p = new uLong;
}


void ISMIndColumn::handleCopy (uInt, const char* value)
{
    uLong offset;
    readFunc_p (&offset, value, nrcopy_p);
    if (offset != 0) {
	StIndArray tmp (offset);
	tmp.incrementRefCount (*iosfile_p);
    }
}

void ISMIndColumn::handleRemove (uInt, const char* value)
{
    uLong offset;
    readFunc_p (&offset, value, nrcopy_p);
    if (offset != 0) {
	StIndArray tmp (offset);
	tmp.decrementRefCount (*iosfile_p);
    }
}
