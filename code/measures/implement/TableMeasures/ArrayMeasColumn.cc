//# Copyright (C) 1997,1998,1999,2000
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
#include <aips/TableMeasures/ArrayMeasColumn.h>
#include <aips/TableMeasures/ScalarMeasColumn.h>
#include <aips/TableMeasures/TableMeasDescBase.h>
#include <aips/TableMeasures/TableMeasOffsetDesc.h>
#include <aips/TableMeasures/TableMeasRefDesc.h>
#include <aips/Measures/MeasConvert.h>
#include <aips/Quanta/MeasValue.h>
#include <aips/Tables/ArrayColumn.h>
#include <aips/Tables/ColumnDesc.h>
#include <aips/Tables/ScalarColumn.h>
#include <aips/Tables/Table.h>
#include <aips/Tables/TableDesc.h>
#include <aips/Tables/TableError.h>
#include <aips/Arrays/Vector.h>
#include <aips/Utilities/Assert.h>
#include <aips/Utilities/String.h>


template<class M>
ROArrayMeasColumn<M>::ROArrayMeasColumn()
: itsDataCol      (0),
  itsRefIntCol    (0),
  itsArrRefIntCol (0),
  itsRefStrCol    (0),
  itsArrRefStrCol (0),
  itsOffsetCol    (0),
  itsArrOffsetCol (0)
{}

template<class M>
ROArrayMeasColumn<M>::ROArrayMeasColumn (const Table& tab,
					 const String& columnName)
: ROTableMeasColumn (tab, columnName),
  itsDataCol      (0),
  itsRefIntCol    (0),
  itsArrRefIntCol (0),
  itsRefStrCol    (0),
  itsArrRefStrCol (0),
  itsOffsetCol    (0),
  itsArrOffsetCol (0)
{
  const TableMeasDescBase& tmDesc = measDesc();
  AlwaysAssert(M::showMe() == tmDesc.type(), AipsError);
  itsDataCol = new ROArrayColumn<Double>(tab, columnName);

  // Determine the number of values in the Measure.
  M tMeas;
  itsNvals = tMeas.getValue().getRecordValue().nelements();

  // Set up the reference code component of the MeasRef. It can be variable
  // and therefore stored in a column which may be either an array or scalar
  // column. Additionally, the references may be stored as strings or
  // refcodes.
  if (tmDesc.isRefCodeVariable()) {
    const String& rcName = tmDesc.refColumnName();
    const ColumnDesc& cd = tab.tableDesc().columnDesc(rcName);
    if (cd.isScalar()) {
      if (cd.dataType() == TpString) {
	itsRefStrCol = new ROScalarColumn<String>(tab, rcName);
      } else {
	itsRefIntCol = new ROScalarColumn<Int>(tab, rcName);
      }
    } else {
      if (cd.dataType() == TpString) {
	itsArrRefStrCol = new ROArrayColumn<String>(tab, rcName);
      } else {
	itsArrRefIntCol = new ROArrayColumn<Int>(tab, rcName);
      }
    }
  } else {
    itsMeasRef.set (tmDesc.getRefCode());
  }

  // Set up the offset component of the MeasRef
  if (tmDesc.hasOffset()) {
    if (tmDesc.isOffsetVariable()) {
      const String& ocName = tmDesc.offsetColumnName();
      if (tmDesc.isOffsetArray()) {
	itsArrOffsetCol = new ROArrayMeasColumn<M>(tab, ocName);
      } else {
	itsOffsetCol = new ROScalarMeasColumn<M>(tab, ocName);
      }
    } else {
      itsMeasRef.set (tmDesc.getOffset());
    }
  }
}

template<class M>
ROArrayMeasColumn<M>::ROArrayMeasColumn (const ROArrayMeasColumn<M>& that)
: itsDataCol     (0),
  itsRefIntCol   (0),
  itsArrRefIntCol(0),
  itsRefStrCol   (0),
  itsArrRefStrCol(0),
  itsOffsetCol   (0),
  itsArrOffsetCol(0)
{
  reference (that);
}

template<class M>
ROArrayMeasColumn<M>::~ROArrayMeasColumn()
{
  cleanUp();
}

template<class M>
void ROArrayMeasColumn<M>::cleanUp()
{
  delete itsDataCol;
  delete itsRefIntCol;
  delete itsArrRefIntCol;
  delete itsRefStrCol;
  delete itsArrRefStrCol;
  delete itsOffsetCol;
  delete itsArrOffsetCol;
}

template<class M>
void ROArrayMeasColumn<M>::reference (const ROArrayMeasColumn<M>& that)
{
  cleanUp();
  ROTableMeasColumn::reference (that);
  itsDataCol      = that.itsDataCol;
  itsRefIntCol    = that.itsRefIntCol;
  itsArrRefIntCol = that.itsArrRefIntCol;
  itsRefStrCol    = that.itsRefStrCol;
  itsArrRefStrCol = that.itsArrRefStrCol;
  itsOffsetCol    = that.itsOffsetCol;
  itsArrOffsetCol = that.itsArrOffsetCol;
  itsMeasRef      = that.itsMeasRef;
  if (itsDataCol != 0) {
    itsDataCol = new ROArrayColumn<Double>(*itsDataCol);
  }
  if (itsRefIntCol != 0) {
    itsRefIntCol = new ROScalarColumn<Int>(*itsRefIntCol);
  }
  if (itsArrRefIntCol != 0) {
    itsArrRefIntCol = new ROArrayColumn<Int>(*itsArrRefIntCol);
  }
  if (itsRefStrCol != 0) {
    itsRefStrCol = new ROScalarColumn<String>(*itsRefStrCol);
  }
  if (itsArrRefStrCol != 0) {
    itsArrRefStrCol = new ROArrayColumn<String>(*itsArrRefStrCol);
  }
  if (itsOffsetCol != 0) {
    itsOffsetCol = new ROScalarMeasColumn<M>(*itsOffsetCol);
  }
  if (itsArrOffsetCol != 0) {
    itsArrOffsetCol = new ROArrayMeasColumn<M>(*itsArrOffsetCol);
  }
}

template<class M>
void ROArrayMeasColumn<M>::attach (const Table& tab,
				   const String& columnName)
{
  reference (ROArrayMeasColumn<M>(tab, columnName));
}

template<class M>
void ROArrayMeasColumn<M>::get (uInt rownr, Array<M>& meas,
				Bool resize) const
{
  // This will fail if array in rownr is undefined.
  Array<Double> tmpData((*itsDataCol)(rownr));
  Bool deleteData;
  const Double* d_ptr = tmpData.getStorage(deleteData);
  const Double* d_p = d_ptr;

  // Determine the dimensionality of the resulting Array<Measure>.
  IPosition shp (tmpData.shape());
  if (itsNvals > 1  &&  shp.nelements() > 0) {
    if (shp.nelements() == 1) {
      shp(0) = 1;
    } else {
      shp.resize (shp.nelements() - 1);
    }
  }
  if (! shp.isEqual (meas.shape())) {
    if (resize  ||  meas.nelements() == 0) {
      meas.resize (shp);
    } else {
      throw(TableArrayConformanceError("ROArrayMeasColumn::get"));
    }
  }
  Bool deleteMeas;
  M* meas_p = meas.getStorage (deleteMeas);

  // Set up get() for reference component of measure.  Three possibilities:
  //   1. A column reference is used (itsMeasRef)
  //   2. The reference varies per row and is stored in a ScalarColumn.
  //   3. Ref varies per element of array (hence stored in an ArrayColumn).
  // With options 2 and 3 the reference could be either stored as a string
  // or int.

  MeasRef<M> locMRef = itsMeasRef;
  Bool refPerElem = ToBool((itsArrRefIntCol != 0) || (itsArrRefStrCol != 0));
  Bool strRefs = ToBool(itsArrRefStrCol != 0);
  Array<Int> intRefArr;
  Array<String> strRefArr;
  const Int* r_p;
  const String* sr_p;
  Bool deleteRef;
  if (refPerElem) {
    if (strRefs) {
      itsArrRefStrCol->get (rownr, strRefArr, True);
      sr_p = strRefArr.getStorage (deleteRef);
    } else {
      itsArrRefIntCol-> get (rownr, intRefArr, True);
      r_p = intRefArr.getStorage (deleteRef);
    }
  } else {
    if (itsRefIntCol != 0) {
      locMRef.set ((*itsRefIntCol)(rownr));
    } else if (itsRefStrCol != 0) {
      typename M::Types tp;
      M::getType (tp, (*itsRefStrCol)(rownr));
      locMRef.set (tp);
    }
  }

  // Setup for offset component of MeasRef.
  Bool offsetPerElem = ToBool(itsArrOffsetCol != 0);
  Array<M> offsetArr;
  const M* os_p;
  Bool deleteOffset;
  if (offsetPerElem) {
    itsArrOffsetCol->get (rownr, offsetArr, True);
    os_p = offsetArr.getStorage(deleteOffset);
  } else {
    if (itsOffsetCol != 0) {
      locMRef.set ((*itsOffsetCol)(rownr));
    }
  }

  // Fill the measure array
  typename M::MVType measVal;
  const Vector<Unit>& units = measDesc().getUnits();
  Vector<Quantum<Double> > qvec(itsNvals);
  for (uInt j=0; j<itsNvals; j++) {
    qvec(j).setUnit (units(j));
  }
  uInt n = meas.nelements();
  for (uInt i=0; i<n; i++) {
    // get the data component of the measure
    for (uInt j=0; j<itsNvals; j++) {
      qvec(j).setValue (*d_p++);
    }
    measVal.putValue (qvec);
    // the reference
    // Note that MeasRef uses reference semantics when copying.
    // So a separate MeasRef instance for each array element is needed if
    // the reference code or offset varies per element.
    if (!refPerElem && !offsetPerElem) {
      meas_p[i].set (measVal, locMRef);
    } else {
      MeasRef<M> tmpMRef;
      if (refPerElem) {
	if (strRefs) {
	  typename M::Types tp;
	  M::getType (tp, sr_p[i]);
	  tmpMRef.set (tp);
	} else {
	  tmpMRef.set (r_p[i]);
	}
      } else {
	tmpMRef.set (locMRef.getType());
      }
      // the offset
      if (offsetPerElem) {
	tmpMRef.set (os_p[i]);
      } else if (locMRef.offset()) {
	tmpMRef.set (M(locMRef.offset()));
      }
      meas_p[i].set (measVal, tmpMRef);
    }
  }

  // clean up
  meas.putStorage (meas_p, deleteMeas);
  tmpData.freeStorage (d_ptr, deleteData);
  if (refPerElem) {
    if (strRefs) {
      strRefArr.freeStorage (sr_p, deleteRef);
    } else {
      intRefArr.freeStorage (r_p, deleteRef);
    }
  }
  if (offsetPerElem) {
    offsetArr.freeStorage (os_p, deleteOffset);
  }
}
    	
template<class M>
Array<M> ROArrayMeasColumn<M>::operator() (uInt rownr) const
{
  Array<M> meas;
  get(rownr, meas);
  return meas;
}

template<class M>
Array<M> ROArrayMeasColumn<M>::convert (uInt rownr,
					const MeasRef<M>& measRef) const
{
  typename M::Convert conv;
  conv.setOut (measRef);
  return doConvert (rownr, conv);
}


template<class M>
Array<M> ROArrayMeasColumn<M>::convert (uInt rownr, uInt refCode) const
{
  typename M::Convert conv;
  conv.setOut (refCode);
  return doConvert (rownr, conv);
}

template<class M>
Array<M> ROArrayMeasColumn<M>::doConvert (uInt rownr,
					  typename M::Convert& conv) const
{
  Array<M> tmp;
  get (rownr, tmp);
  uInt n = tmp.nelements();
  Bool deleteIt;
  M* data = tmp.getStorage (deleteIt);
  for (uInt i=0; i<n; i++) {
    data[i] = conv(data[i]);
  }
  tmp.putStorage (data, deleteIt);
  return tmp;
}




template<class M>
ArrayMeasColumn<M>::ArrayMeasColumn()
: ROArrayMeasColumn<M>(),
  itsDataCol     (0),
  itsRefIntCol   (0),
  itsArrRefIntCol(0),
  itsRefStrCol   (0),
  itsArrRefStrCol(0),
  itsOffsetCol   (0),
  itsArrOffsetCol(0)
{}

template<class M>
ArrayMeasColumn<M>::ArrayMeasColumn (const Table& tab,
				     const String& columnName)
: ROArrayMeasColumn<M>(tab, columnName),
  itsDataCol     (0),
  itsRefIntCol   (0),
  itsArrRefIntCol(0),
  itsRefStrCol   (0),
  itsArrRefStrCol(0),
  itsOffsetCol   (0),
  itsArrOffsetCol(0)
{
  const TableMeasDescBase& tmDesc = measDesc();
  itsDataCol = new ArrayColumn<Double>(tab, columnName);

  // Set up the reference code component of the MeasRef
  if (tmDesc.isRefCodeVariable()) {
    const String rcName = tmDesc.refColumnName();
    const ColumnDesc& cd = tab.tableDesc().columnDesc(rcName);
    if (cd.isScalar()) {
      if (cd.dataType() == TpString) {
	itsRefStrCol = new ScalarColumn<String>(tab, rcName);
      } else {
	itsRefIntCol = new ScalarColumn<Int>(tab, rcName);
      }
    } else {
      if (cd.dataType() == TpString) {
	itsArrRefStrCol = new ArrayColumn<String>(tab, rcName);
      } else {
	itsArrRefIntCol = new ArrayColumn<Int>(tab, rcName);
      }
    }
  }

  // Set up the offset component of the MeasRef
  if (tmDesc.hasOffset()) {
    if (tmDesc.isOffsetVariable()) {
      const String ocName = tmDesc.offsetColumnName();
      if (tmDesc.isOffsetArray()) {
	itsArrOffsetCol = new ArrayMeasColumn<M>(tab, ocName);
      } else {
	itsOffsetCol = new ScalarMeasColumn<M>(tab, ocName);
      }
    }
  }
}

template<class M>
ArrayMeasColumn<M>::ArrayMeasColumn (const ArrayMeasColumn<M>& that)
: ROArrayMeasColumn<M>(),
  itsDataCol     (0),
  itsRefIntCol   (0),
  itsArrRefIntCol(0),
  itsRefStrCol   (0),
  itsArrRefStrCol(0),
  itsOffsetCol   (0),
  itsArrOffsetCol(0)
{
  reference (that);
}

template<class M>
ArrayMeasColumn<M>::~ArrayMeasColumn()
{
  cleanUp();
}

template<class M>
void ArrayMeasColumn<M>::cleanUp()
{
  delete itsDataCol;
  delete itsRefIntCol;
  delete itsArrRefIntCol;
  delete itsRefStrCol;
  delete itsArrRefStrCol;
  delete itsOffsetCol;
  delete itsArrOffsetCol;
}

template<class M>
void ArrayMeasColumn<M>::reference (const ArrayMeasColumn<M>& that)
{
  ROArrayMeasColumn<M>::reference(that);
  itsDataCol      = that.itsDataCol;
  itsRefIntCol    = that.itsRefIntCol;
  itsArrRefIntCol = that.itsArrRefIntCol;
  itsRefStrCol    = that.itsRefStrCol;
  itsArrRefStrCol = that.itsArrRefStrCol;
  itsOffsetCol    = that.itsOffsetCol;
  itsArrOffsetCol = that.itsArrOffsetCol;
  if (itsDataCol != 0) {
    itsDataCol = new ArrayColumn<Double>(*itsDataCol);
  }
  if (itsRefIntCol != 0) {
    itsRefIntCol = new ScalarColumn<Int>(*itsRefIntCol);
  }
  if (itsArrRefIntCol != 0) {
    itsArrRefIntCol = new ArrayColumn<Int>(*itsArrRefIntCol);
  }
  if (itsRefStrCol != 0) {
    itsRefStrCol = new ScalarColumn<String>(*itsRefStrCol);
  }
  if (itsArrRefStrCol != 0) {
    itsArrRefStrCol = new ArrayColumn<String>(*itsArrRefStrCol);
  }
  if (itsOffsetCol != 0) {
    itsOffsetCol = new ScalarMeasColumn<M>(*itsOffsetCol);
  }
  if (itsArrOffsetCol != 0) {
    itsArrOffsetCol = new ArrayMeasColumn<M>(*itsArrOffsetCol);
  }
}

template<class M>
void ArrayMeasColumn<M>::attach (const Table& tab,
				 const String& columnName)
{
  reference (ArrayMeasColumn<M>(tab, columnName));
}

template<class M>
void ArrayMeasColumn<M>::setDescRefCode (uInt refCode)
{
  Table tab = table();
  if (tab.nrow() != 0) {
    throw (AipsError ("ArrayMeasColumn::setDescRefCode cannot be done; "
		      "the table is not empty"));
  }
  itsDescPtr->resetRefCode (refCode);
  itsDescPtr->write (const_cast<TableDesc&>(tab.tableDesc()));
  itsMeasRef.set (refCode);
}

template<class M>
void ArrayMeasColumn<M>::setDescOffset (const Measure& offset)
{
  Table tab = table();
  if (tab.nrow() != 0) {
    throw (AipsError ("ArrayMeasColumn::setDescOffset cannot be done; "
		      "the table is not empty"));
  }
  itsDescPtr->resetOffset (offset);
  itsDescPtr->write (const_cast<TableDesc&>(tab.tableDesc()));
  itsMeasRef.set (offset);
}

template<class M>
void ArrayMeasColumn<M>::setDescUnits (const Vector<Unit>& units)
{
  Table tab = table();
  if (tab.nrow() != 0) {
    throw (AipsError ("ArrayMeasColumn::setDescUnits cannot be done; "
		      "the table is not empty"));
  }
  itsDescPtr->resetUnits (units);
  itsDescPtr->write (const_cast<TableDesc&>(tab.tableDesc()));
}

template<class M>
void ArrayMeasColumn<M>::put (uInt rownr, const Array<M>& meas)
{
  // If meas has entries then need to resize the dataColArr to conform
  // to meas.Shape() + one dimension for storing the measure's values.
  const uInt n = meas.nelements();
  IPosition shp(meas.shape());
  if (n > 0  &&  itsNvals > 1) {
    shp.append (IPosition(1, itsNvals));
  }
  Bool deleteData;
  Array<Double> dataArr(shp);
  Double* d_ptr = dataArr.getStorage(deleteData);
  Double* d_p = d_ptr;
  Bool deleteMeas;
  const M* meas_p = meas.getStorage(deleteMeas);

  // Set up put for reference component of measure.  Three possibilities:
  //   1. The reference is non-variable so ignore reference component.
  //   2. The reference varies per row so write reference once using
  //      reference from first measure in the array.  No check is done
  //	  on the reference component of other measures.
  //   3. Ref varies per element of array. An array of references is written.
  // With 2 and 3 references are stored as either Strings or Ints.
  MeasRef<M> locMRef = itsMeasRef;
  Bool refPerElem = ToBool((itsArrRefIntCol != 0) || (itsArrRefStrCol != 0));
  Bool strRefs = ToBool(itsArrRefStrCol != 0);
  Array<Int> intRefArr;
  Array<String> strRefArr;
  Int* r_p;
  String* sr_p;
  Bool deleteRef;
  if (refPerElem) {
    // References are variable per array element.
    if (strRefs) {
      strRefArr.resize (meas.shape());
      sr_p = strRefArr.getStorage (deleteRef);
    } else {
      intRefArr.resize (meas.shape());
      r_p = intRefArr.getStorage (deleteRef);
    }
  } else if (itsVarRefFlag) {
    // References are variable per row only (thus same for entire array).
    // Use the reference of the first element as the reference.
    // Take care in case the array is empty.
    Int tp = 0;
    if (n > 0) {
      tp = meas_p->getRef().getType();
      locMRef.set (tp);
    }
    if (itsRefIntCol != 0) {
      itsRefIntCol->put (rownr, tp);
    } else if (itsRefStrCol != 0) {
      itsRefStrCol->put (rownr, M::showType(tp));
    }
  }

  // Setup for offset.
  Bool offsetPerElem = ToBool(itsArrOffsetCol != 0);
  Array<M> offsetArr;
  M* os_p;
  Bool deleteOffset;
  if (offsetPerElem) {
    // Offsets are variable array element.
    offsetArr.resize (meas.shape());
    os_p = offsetArr.getStorage (deleteOffset);
  } else if (itsVarOffFlag) {
    // Offsets are variable per row only.
    // Use the offset from the first measure (if any).
    const Measure* offptr=0;
    if (n > 0) {
      offptr = meas_p->getRef().offset();
    }
    if (offptr != 0) {
      M moff (offptr);
      locMRef.set (moff);
      itsOffsetCol->put (rownr, moff);
    } else
      itsOffsetCol->put (rownr, M());
  }

  // If reference type and offset are variable per element, conversion
  // is not needed.
  const Vector<Unit>& units = measDesc().getUnits();
  Vector<Quantum<Double> > qvec;
  for (uInt i=0; i<n; i++) {
    const MeasRef<M>& mref = meas_p[i].getRef();
    uInt refCode = mref.getType();
    const Measure* offptr = mref.offset();

    if (refPerElem  &&  offsetPerElem) {
      qvec = meas_p[i].getValue().getRecordValue();
    } else {
      if (refPerElem) {
	locMRef.set (refCode);
      }
      if (offsetPerElem) {
	if (offptr != 0) {
	  locMRef.set (M(offptr));
	} else {
	  locMRef.set (M());
	}
      }
      typename M::Convert conv(meas_p[i], locMRef);
      M locMeas = conv();
      qvec = locMeas.getValue().getRecordValue();
    }

    if (refPerElem) {
      if (strRefs) {
	sr_p[i] = M::showType(refCode);
      } else {
	r_p[i] = refCode;
      }
    }
    if (offsetPerElem) {
      if (offptr != 0) {
	os_p[i] = M(offptr);
      }
    }
    for (uInt j=0; j<itsNvals; j++) {
      *d_p++ = qvec(j).getValue(units(j));
    }
  }

  // Write the real columns.
  dataArr.putStorage (d_ptr, deleteData);
  itsDataCol->put (rownr, dataArr);
  meas.freeStorage (meas_p, deleteMeas);
  if (refPerElem) {
    if (strRefs) {
      strRefArr.putStorage (sr_p, deleteRef);
      itsArrRefStrCol->put (rownr, strRefArr);
    } else {
      intRefArr.putStorage (r_p, deleteRef);
      itsArrRefIntCol->put (rownr, intRefArr);
    }
  }
  if (offsetPerElem) {
    offsetArr.putStorage (os_p, deleteOffset);
    itsArrOffsetCol->put (rownr, offsetArr);
  }
}
