//# RecordRep.cc: A hierarchical collection of named fields of various types
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

#include <aips/Containers/RecordRep.h>
#include <aips/Containers/Record.h>
#include <aips/Arrays/Array.h>
#include <aips/Arrays/ArrayError.h>
#include <aips/Lattices/IPosition.h>
#include <aips/IO/AipsIO.h>
#include <aips/Utilities/Assert.h>
#include <aips/Exceptions/Error.h>


RecordRep::RecordRep ()
: nused_p (0),
  data_p  (0)
{}
	
RecordRep::RecordRep (const RecordDesc& description)
: nused_p (0),
  data_p  (0),
  desc_p  (description)
{
    restructure (desc_p);
}

RecordRep::RecordRep (const RecordRep& other)
: nused_p (0),
  data_p  (0),
  desc_p  (other.desc_p)
{
    restructure (desc_p);
    copy_other (other);
}

RecordRep& RecordRep::operator= (const RecordRep& other)
{
    if (this != &other) {
	restructure (other.desc_p);
	copy_other (other);
    }
    return *this;
}

RecordRep::~RecordRep()
{
    delete_myself (desc_p.nfields());
}

void RecordRep::restructure (const RecordDesc& newDescription)
{
    delete_myself (desc_p.nfields());
    desc_p  = newDescription;
    nused_p = desc_p.nfields();
    data_p.resize (nused_p);
    for (uInt i=0; i<nused_p; i++) {
	if (desc_p.type(i) == TpRecord) {
	    data_p[i] = new Record (this, desc_p.subRecord(i));
	}else{
	    data_p[i] = createDataField (desc_p.type(i), desc_p.shape(i));
	}
	AlwaysAssert (data_p[i], AipsError);
    }
}

Int RecordRep::fieldNumber (const String& name) const
{
    return desc_p.fieldNumber (name);
}

void RecordRep::addDataPtr (void* ptr)
{
    if (nused_p >= data_p.nelements()) {
	data_p.resize (nused_p + 16);
    }
    data_p[nused_p++] = ptr;
}

void RecordRep::removeDataPtr (Int index)
{
    nused_p--;
    if (index < nused_p) {
	memmove (&data_p[index], &data_p[index+1],
		 (nused_p-index) * sizeof(void*));
    }
}

void RecordRep::removeData (Int whichField, void* ptr)
{
    DataType type = desc_p.type(whichField);
    if (type == TpRecord) {
	delete (Record*)ptr;
    }else{
	deleteDataField (type, ptr);
    }
}

void RecordRep::removeField (Int whichField)
{
    removeData (whichField, data_p[whichField]);
    removeDataPtr (whichField);
    removeFieldFromDesc (whichField);
}

void RecordRep::addFieldToDesc (const String& name, DataType type,
				const IPosition& shape, Bool fixedShape)
{
    if (fixedShape) {
	desc_p.addField (name, type, shape);
    }else{
	desc_p.addField (name, type);
    }
}

void RecordRep::removeFieldFromDesc (Int whichField)
{
    desc_p.removeField (whichField);
}

void RecordRep::addDataField (const String& name, DataType type,
			      const IPosition& shape, Bool fixedShape,
			      const void* data)
{
    AlwaysAssert (type == TpBool      ||  type == TpArrayBool
              ||  type == TpUChar     ||  type == TpArrayUChar
              ||  type == TpShort     ||  type == TpArrayShort
              ||  type == TpInt       ||  type == TpArrayInt
              ||  type == TpUInt      ||  type == TpArrayUInt
              ||  type == TpFloat     ||  type == TpArrayFloat
              ||  type == TpDouble    ||  type == TpArrayDouble
              ||  type == TpComplex   ||  type == TpArrayComplex
              ||  type == TpDComplex  ||  type == TpArrayDComplex
              ||  type == TpString    ||  type == TpArrayString
                  , AipsError);
    addFieldToDesc (name, type, shape, fixedShape);
    void* ptr = createDataField (type, shape);
    copyDataField (type, ptr, data);
    addDataPtr (ptr);
}

void RecordRep::addField (const String& name, const Record& rec,
			  RecordInterface::RecordType type)
{
    // When the record is empty, it is variable structured.
    if (rec.nfields() == 0) {
	type = RecordInterface::Variable;
    }
    // When the new field is fixed, add its description too.
    if (type == RecordInterface::Fixed) {
	desc_p.addField (name, rec.description());
    }else{
	desc_p.addField (name, TpRecord);
    }
    // Use default ctor and assignment to be sure that the
    // new record gets the correct record type.
    Record* ptr = new Record (type);
    *ptr = rec;
    addDataPtr (ptr);
}

void RecordRep::checkShape (DataType type, const IPosition& shape,
			    const void* value)
{
    IPosition arrShape;
    switch (type) {
    case TpArrayBool:
	arrShape = ((Array<Bool>*)value)->shape();
	break;
    case TpArrayUChar:
	arrShape = ((Array<uChar>*)value)->shape();
	break;
    case TpArrayShort:
	arrShape = ((Array<Short>*)value)->shape();
	break;
    case TpArrayInt:
	arrShape = ((Array<Int>*)value)->shape();
	break;
    case TpArrayUInt:
	arrShape = ((Array<uInt>*)value)->shape();
	break;
    case TpArrayFloat:
	arrShape = ((Array<float>*)value)->shape();
	break;
    case TpArrayDouble:
	arrShape = ((Array<double>*)value)->shape();
	break;
    case TpArrayComplex:
	arrShape = ((Array<Complex>*)value)->shape();
	break;
    case TpArrayDComplex:
	arrShape = ((Array<DComplex>*)value)->shape();
	break;
    case TpArrayString:
	arrShape = ((Array<String>*)value)->shape();
	break;
    default:
	throw (AipsError ("RecordRep::checkShape"));
    }
    if (shape != arrShape) {
	throw (ArrayConformanceError
                           ("Record::define; fixed array conformance error"));
    }
}

void RecordRep::defineDataField (Int whichField, DataType type,
				 const void* value)
{
    AlwaysAssert (whichField >= 0  &&  whichField < nused_p
		  &&  desc_p.type(whichField) == type, AipsError);
    if (type == TpRecord) {
	*(Record*)data_p[whichField] = *(const Record*)value;
    }else{
	if (desc_p.isArray(whichField)) {
	    const IPosition& shape = desc_p.shape(whichField);
	    if (shape.nelements() > 0  &&  shape(0) > 0) {
		checkShape (type, shape, value);
	    }
	}
	copyDataField (type, data_p[whichField], value);
    }
}


void* RecordRep::createDataField (DataType type, const IPosition& shape)
{
    IPosition arrayShape;
    if (shape.nelements() > 0  &&  shape(0) > 0) {
	arrayShape = shape;
    }
    switch (type) {
    case TpBool:
	{
	    Bool* ptr = new Bool;
	    *ptr = False;
	    return ptr;
	}
    case TpUChar:
	{
	    uChar* ptr = new uChar;
	    *ptr = 0;
	    return ptr;
	}
    case TpShort:
	{
	    Short* ptr = new Short;
	    *ptr = 0;
	    return ptr;
	}
    case TpInt:
	{
	    Int* ptr = new Int;
	    *ptr = 0;
	    return ptr;
	}
    case TpUInt:
	{
	    uInt* ptr = new uInt;
	    *ptr = 0;
	    return ptr;
	}
    case TpFloat:
	{
	    float* ptr = new float;
	    *ptr = 0.0;
	    return ptr;
	}
    case TpDouble:
	{
	    double* ptr = new double;
	    *ptr = 0.0;
	    return ptr;
	}
    case TpComplex:
	return new Complex;
    case TpDComplex:
	return new DComplex;
    case TpString:
	return new String;
    case TpArrayBool:
	{
	    Array<Bool>* ptr = new Array<Bool> (arrayShape);
	    *ptr = False;
	    return ptr;
	}
    case TpArrayUChar:
	{
	    Array<uChar>* ptr = new Array<uChar> (arrayShape);
	    *ptr = 0;
	    return ptr;
	}
    case TpArrayShort:
	{
	    Array<Short>* ptr = new Array<Short> (arrayShape);
	    *ptr = 0;
	    return ptr;
	}
    case TpArrayInt:
	{
	    Array<Int>* ptr = new Array<Int> (arrayShape);
	    *ptr = 0;
	    return ptr;
	}
    case TpArrayUInt:
	{
	    Array<uInt>* ptr = new Array<uInt> (arrayShape);
	    *ptr = 0;
	    return ptr;
	}
    case TpArrayFloat:
	{
	    Array<float>* ptr = new Array<float> (arrayShape);
	    *ptr = 0.0;
	    return ptr;
	}
    case TpArrayDouble:
	{
	    Array<double>* ptr = new Array<double> (arrayShape);
	    *ptr = 0.0;
	    return ptr;
	}
    case TpArrayComplex:
	return new Array<Complex> (arrayShape);
    case TpArrayDComplex:
	return new Array<DComplex> (arrayShape);
    case TpArrayString:
	return new Array<String> (arrayShape);
    default:
	throw (AipsError ("RecordRep::createDataField: unknown data type"));
    }
    return 0;
}

void RecordRep::delete_myself (uInt nfields)
{
    if (nfields > nused_p) {
	nfields = nused_p;
    }
    for (uInt i=0; i<nfields; i++) {
	removeData (i, data_p[i]);
	data_p[i] = 0;
    }
}

void RecordRep::deleteDataField (DataType type, void* ptr)
{
    switch (type) {
    case TpBool:
	delete (Bool*)ptr;
	break;
    case TpUChar:
	delete (uChar*)ptr;
	break;
    case TpShort:
	delete (Short*)ptr;
	break;
    case TpInt:
	delete (Int*)ptr;
	break;
    case TpUInt:
	delete (uInt*)ptr;
	break;
    case TpFloat:
	delete (float*)ptr;
	break;
    case TpDouble:
	delete (double*)ptr;
	break;
    case TpComplex:
	delete (Complex*)ptr;
	break;
    case TpDComplex:
	delete (DComplex*)ptr;
	break;
    case TpString:
	delete (String*)ptr;
	break;
    case TpArrayBool:
	delete (Array<Bool>*)ptr;
	break;
    case TpArrayUChar:
	delete (Array<uChar>*)ptr;
	break;
    case TpArrayShort:
	delete (Array<Short>*)ptr;
	break;
    case TpArrayInt:
	delete (Array<Int>*)ptr;
	break;
    case TpArrayUInt:
	delete (Array<uInt>*)ptr;
	break;
    case TpArrayFloat:
	delete (Array<float>*)ptr;
	break;
    case TpArrayDouble:
	delete (Array<double>*)ptr;
	break;
    case TpArrayComplex:
	delete (Array<Complex>*)ptr;
	break;
    case TpArrayDComplex:
	delete (Array<DComplex>*)ptr;
	break;
    case TpArrayString:
	delete (Array<String>*)ptr;
	break;
    default:
	throw (AipsError ("RecordRep::deleteDataField"));
    }
}

Bool RecordRep::conform (const RecordRep& other) const
{
    // First check (non-recursively) if the descriptions conform.
    if (! desc_p.conform (other.desc_p)) {
	return False;
    }
    // Now check for each fixed sub-record if it conforms.
    for (Int i=0; i<nused_p; i++) {
	if (desc_p.type(i) == TpRecord) {
	    const Record& thisRecord = *(const Record*)data_p[i];
	    if (thisRecord.isFixed()) {
		const Record& thatRecord = *(const Record*)other.data_p[i];
		if (! thisRecord.conform (thatRecord)) {
		    return False;
		}
	    }
	}
    }
    return True;
}

void RecordRep::copyData (const RecordRep& other)
{
    // Assume conform has already been called
    DebugAssert (conform (other), AipsError);
    copy_other (other);
}

void RecordRep::copy_other (const RecordRep& other)
{
    for (uInt i=0; i<nused_p; i++) {
	if (desc_p.type(i) == TpRecord) {
	    *(Record*)data_p[i] = *(const Record*)other.data_p[i];
	}else{
	    copyDataField (desc_p.type(i), data_p[i], other.data_p[i]);
	}
    }
}

void RecordRep::copyDataField (DataType type, Int whichField,
                               const void* that) const
{
    copyDataField (type, data_p[whichField], that);
}

void RecordRep::copyDataField (DataType type, void* ptr,
                               const void* that) const
{
    switch (type) {
    case TpBool:
	*(Bool*)ptr = *(const Bool*)that;
	break;
    case TpUChar:
	*(uChar*)ptr = *(const uChar*)that;
	break;
    case TpShort:
	*(Short*)ptr = *(const Short*)that;
	break;
    case TpInt:
	*(Int*)ptr = *(const Int*)that;
	break;
    case TpUInt:
	*(uInt*)ptr = *(const uInt*)that;
	break;
    case TpFloat:
	*(float*)ptr = *(const float*)that;
	break;
    case TpDouble:
	*(double*)ptr = *(const double*)that;
	break;
    case TpComplex:
	*(Complex*)ptr = *(const Complex*)that;
	break;
    case TpDComplex:
	*(DComplex*)ptr = *(const DComplex*)that;
	break;
    case TpString:
	*(String*)ptr = *(const String*)that;
	break;
    case TpArrayBool:
	((Array<Bool>*)ptr)->resize (((const Array<Bool>*)that)->shape());
	*(Array<Bool>*)ptr = *(const Array<Bool>*)that;
	break;
    case TpArrayUChar:
	((Array<uChar>*)ptr)->resize (((const Array<uChar>*)that)->shape());
	*(Array<uChar>*)ptr = *(const Array<uChar>*)that;
	break;
    case TpArrayShort:
	((Array<Short>*)ptr)->resize (((const Array<Short>*)that)->shape());
	*(Array<Short>*)ptr = *(const Array<Short>*)that;
	break;
    case TpArrayInt:
	((Array<Int>*)ptr)->resize (((const Array<Int>*)that)->shape());
	*(Array<Int>*)ptr = *(const Array<Int>*)that;
	break;
    case TpArrayUInt:
	((Array<uInt>*)ptr)->resize (((const Array<uInt>*)that)->shape());
	*(Array<uInt>*)ptr = *(const Array<uInt>*)that;
	break;
    case TpArrayFloat:
	((Array<float>*)ptr)->resize (((const Array<float>*)that)->shape());
	*(Array<float>*)ptr = *(const Array<float>*)that;
	break;
    case TpArrayDouble:
	((Array<double>*)ptr)->resize (((const Array<double>*)that)->shape());
	*(Array<double>*)ptr = *(const Array<double>*)that;
	break;
    case TpArrayComplex:
	((Array<Complex>*)ptr)->resize
	                             (((const Array<Complex>*)that)->shape());
	*(Array<Complex>*)ptr = *(const Array<Complex>*)that;
	break;
    case TpArrayDComplex:
	((Array<DComplex>*)ptr)->resize
	                            (((const Array<DComplex>*)that)->shape());
	*(Array<DComplex>*)ptr = *(const Array<DComplex>*)that;
	break;
    case TpArrayString:
	((Array<String>*)ptr)->resize (((const Array<String>*)that)->shape());
	*(Array<String>*)ptr = *(const Array<String>*)that;
	break;
    default:
	throw (AipsError ("RecordRep::copyDataField"));
    }
}


void* RecordRep::get_pointer (Int whichField, DataType type,
			      const String& recordType) const
{
    AlwaysAssert (recordType == "Record", AipsError);
    return get_pointer (whichField, type);
}
void* RecordRep::get_pointer (Int whichField, DataType type) const
{
    AlwaysAssert (whichField >= 0  &&  whichField < nused_p  &&
		  type == desc_p.type(whichField), AipsError);
    return data_p[whichField];
}



void RecordRep::mergeField (const RecordRep& other, Int whichFieldFromOther,
			    RecordInterface::DuplicatesFlag flag)
{
    // If the field exists and if flag tells to overwrite,
    // the field is removed first.
    if (flag == RecordInterface::OverwriteDuplicates) {
	Int fld = desc_p.fieldNumber (other.desc_p.name(whichFieldFromOther));
	if (fld >= 0) {
	    removeField (fld);
	}
    }
    // Try to add the field to the description.
    Int nr = desc_p.nfields();
    Int nrnew = desc_p.mergeField (other.desc_p, whichFieldFromOther, flag);
    // It succeeded if nfields increased.
    // Then the value can be defined.
    if (nrnew > nr) {
	DataType type = desc_p.type (nr);
	void* otherPtr = other.get_pointer (whichFieldFromOther, type);
	void* ptr;
	if (type == TpRecord) {
	    ptr = new Record (*(Record*)otherPtr);
	}else{
	    ptr = createDataField (type, desc_p.shape(nr));
	    copyDataField (type, ptr, otherPtr);
	}
	addDataPtr (ptr);
    }
}

void RecordRep::merge (const RecordRep& other,
		       RecordInterface::DuplicatesFlag flag)
{
    uInt n = other.desc_p.nfields();
    for (Int i=0; i<n; i++) {
	mergeField (other, i, flag);
    }
}
    

void RecordRep::putDataField (AipsIO& os, DataType type, const void* ptr) const
{
    switch (type) {
    case TpBool:
	os << *(const Bool*)ptr;
	break;
    case TpUChar:
	os << *(const uChar*)ptr;
	break;
    case TpShort:
	os << *(const Short*)ptr;
	break;
    case TpInt:
	os << *(const Int*)ptr;
	break;
    case TpUInt:
	os << *(const uInt*)ptr;
	break;
    case TpFloat:
	os << *(const float*)ptr;
	break;
    case TpDouble:
	os << *(const double*)ptr;
	break;
    case TpComplex:
	os << *(const Complex*)ptr;
	break;
    case TpDComplex:
	os << *(const DComplex*)ptr;
	break;
    case TpString:
	os << *(const String*)ptr;
	break;
    case TpArrayBool:
	os << *(const Array<Bool>*)ptr;
	break;
    case TpArrayUChar:
	os << *(const Array<uChar>*)ptr;
	break;
    case TpArrayShort:
	os << *(const Array<Short>*)ptr;
	break;
    case TpArrayInt:
	os << *(const Array<Int>*)ptr;
	break;
    case TpArrayUInt:
	os << *(const Array<uInt>*)ptr;
	break;
    case TpArrayFloat:
	os << *(const Array<float>*)ptr;
	break;
    case TpArrayDouble:
	os << *(const Array<double>*)ptr;
	break;
    case TpArrayComplex:
	os << *(const Array<Complex>*)ptr;
	break;
    case TpArrayDComplex:
	os << *(const Array<DComplex>*)ptr;
	break;
    case TpArrayString:
	os << *(const Array<String>*)ptr;
	break;
    default:
	throw (AipsError ("RecordRep::putDataField"));
    }
}

void RecordRep::getDataField (AipsIO& os, DataType type, void* ptr)
{
    switch (type) {
    case TpBool:
	os >> *(Bool*)ptr;
	break;
    case TpUChar:
	os >> *(uChar*)ptr;
	break;
    case TpShort:
	os >> *(Short*)ptr;
	break;
    case TpInt:
	os >> *(Int*)ptr;
	break;
    case TpUInt:
	os >> *(uInt*)ptr;
	break;
    case TpFloat:
	os >> *(float*)ptr;
	break;
    case TpDouble:
	os >> *(double*)ptr;
	break;
    case TpComplex:
	os >> *(Complex*)ptr;
	break;
    case TpDComplex:
	os >> *(DComplex*)ptr;
	break;
    case TpString:
	os >> *(String*)ptr;
	break;
    case TpArrayBool:
	os >> *(Array<Bool>*)ptr;
	break;
    case TpArrayUChar:
	os >> *(Array<uChar>*)ptr;
	break;
    case TpArrayShort:
	os >> *(Array<Short>*)ptr;
	break;
    case TpArrayInt:
	os >> *(Array<Int>*)ptr;
	break;
    case TpArrayUInt:
	os >> *(Array<uInt>*)ptr;
	break;
    case TpArrayFloat:
	os >> *(Array<float>*)ptr;
	break;
    case TpArrayDouble:
	os >> *(Array<double>*)ptr;
	break;
    case TpArrayComplex:
	os >> *(Array<Complex>*)ptr;
	break;
    case TpArrayDComplex:
	os >> *(Array<DComplex>*)ptr;
	break;
    case TpArrayString:
	os >> *(Array<String>*)ptr;
	break;
    default:
	throw (AipsError ("RecordRep::getDataField"));
    }
}


void RecordRep::putRecord (AipsIO& os, int recordType) const
{
    os.putstart ("Record", 1);              // version 1
    os << desc_p;
    os << recordType;
    putData (os);
    os.putend();
}

void RecordRep::putData (AipsIO& os) const
{
    for (uInt i=0; i<nused_p; i++) {
	if (desc_p.type(i) == TpRecord) {
	    const RecordDesc& desc = desc_p.subRecord(i);
	    if (desc.nfields() == 0) {
		os << *(const Record*)data_p[i];
	    }else{
		(*(const Record*)data_p[i]).putData (os);
	    }
	}else{
	    putDataField (os, desc_p.type(i), data_p[i]);
	}
    }
}

void RecordRep::getRecord (AipsIO& os, int& recordType)
{
    // Support reading scalar and array keyword sets as records.
    uInt version;
    String type = os.getNextType();
    if (type == "ScalarKeywordSet") {
	version = os.getstart ("ScalarKeywordSet");
	getKeySet (os, version, 0);
    } else if (type == "ArrayKeywordSet") {
	version = os.getstart ("ArrayKeywordSet");
	getKeySet (os, version, 1);
    }else{
	uInt version = os.getstart ("Record");
	// Get the description and restructure the record.
	RecordDesc desc;
	os >> desc;
	os >> recordType;
	restructure (desc);
	// Read the data.
	getData (os, version);
    }
    os.getend();
}

void RecordRep::getData (AipsIO& os, uInt version)
{
    for (uInt i=0; i<nused_p; i++) {
	if (desc_p.type(i) == TpRecord) {
	    const RecordDesc& desc = desc_p.subRecord(i);
	    if (desc.nfields() == 0) {
		os >> *(Record*)data_p[i];
	    }else{
		(*(Record*)data_p[i]).getData (os, version);
	    }
	}else{
	    getDataField (os, desc_p.type(i), data_p[i]);
	}
    }
}

void RecordRep::getKeySet (AipsIO& os, uInt version, uInt type)
{
    // First build the description from the map of keyword names and
    // attributes.
    RecordDesc desc;
    getKeyDesc (os, desc);
    // Define the record from the description.
    // Read the keyword values and define the corresponding record value.
    restructure (desc);
    getScalarKeys (os);
    if (type == 1) {
	getArrayKeys (os);
    }
    // Newer keyword sets may contain nested keyword sets.
    // We do not support reading those, so throw an exception when they exist.
    if (version > 1) {
	uInt n;
	os >> n;
	AlwaysAssert (n==0, AipsError);
    }
}

void RecordRep::getKeyDesc (AipsIO& os, RecordDesc& desc)
{
    // Start reading the Map of keyword names and attributes.
    os.getstart ("Map<String,void>");
    int dt;
    String name, comment;
    // Get #names and the default attribute (datatype + comment).
    uInt i, n;
    os >> n;
    os >> dt;
    os >> comment;
    // Get each keyword name and attribute.
    // Add them to the record description.
    for (i=0; i<n; i++) {
	os >> name;
	os >> dt;
	os >> comment;
	desc.addField (name, DataType(dt));
    }
    os.getend();
    // Get the excluded data types and names.
    // Note that exNames was written as a Block<Regex>, but can be
    // read as a Block<String>. This is a template instantiation less.
    Block<int>    exDtype;
    Block<String> exNames;
    os >> exDtype;
    os >> exNames;
}

void RecordRep::getScalarKeys (AipsIO& os)
{
    uInt i, n;
    String name;
    // Read the values per type.
    os >> n;
    for (i=0; i<n; i++) {
	os >> name;
	getDataField (os, TpBool, data_p[fieldNumber (name)]);
    }
    os >> n;
    for (i=0; i<n; i++) {
	os >> name;
	getDataField (os, TpInt, data_p[fieldNumber (name)]);
    }
    os >> n;
    for (i=0; i<n; i++) {
	os >> name;
	getDataField (os, TpUInt, data_p[fieldNumber (name)]);
    }
    os >> n;
    for (i=0; i<n; i++) {
	os >> name;
	getDataField (os, TpFloat, data_p[fieldNumber (name)]);
    }
    os >> n;
    for (i=0; i<n; i++) {
	os >> name;
	getDataField (os, TpDouble, data_p[fieldNumber (name)]);
    }
    os >> n;
    for (i=0; i<n; i++) {
	os >> name;
	getDataField (os, TpComplex, data_p[fieldNumber (name)]);
    }
    os >> n;
    for (i=0; i<n; i++) {
	os >> name;
	getDataField (os, TpDComplex, data_p[fieldNumber (name)]);
    }
    os >> n;
    for (i=0; i<n; i++) {
	os >> name;
	getDataField (os, TpString, data_p[fieldNumber (name)]);
    }
}

void RecordRep::getArrayKeys (AipsIO& os)
{
    uInt i, n;
    String name;
    // Read the values per type.
    os >> n;
    for (i=0; i<n; i++) {
	os >> name;
	getDataField (os, TpArrayBool, data_p[fieldNumber (name)]);
    }
    os >> n;
    for (i=0; i<n; i++) {
	os >> name;
	getDataField (os, TpArrayInt, data_p[fieldNumber (name)]);
    }
    os >> n;
    for (i=0; i<n; i++) {
	os >> name;
	getDataField (os, TpArrayUInt, data_p[fieldNumber (name)]);
    }
    os >> n;
    for (i=0; i<n; i++) {
	os >> name;
	getDataField (os, TpArrayFloat, data_p[fieldNumber (name)]);
    }
    os >> n;
    for (i=0; i<n; i++) {
	os >> name;
	getDataField (os, TpArrayDouble, data_p[fieldNumber (name)]);
    }
    os >> n;
    for (i=0; i<n; i++) {
	os >> name;
	getDataField (os, TpArrayComplex, data_p[fieldNumber (name)]);
    }
    os >> n;
    for (i=0; i<n; i++) {
	os >> name;
	getDataField (os, TpArrayDComplex, data_p[fieldNumber (name)]);
    }
    os >> n;
    for (i=0; i<n; i++) {
	os >> name;
	getDataField (os, TpArrayString, data_p[fieldNumber (name)]);
    }
}
