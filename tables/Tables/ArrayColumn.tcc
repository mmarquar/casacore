//# ArrayColumn.cc: Access to an array table column with arbitrary data type
//# Copyright (C) 1994,1995,1996,1997,1998,1999,2000,2001
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

#include <tables/Tables/ArrayColumn.h>
#include <tables/Tables/ArrayColumnFunc.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/RefRows.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayIter.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Slicer.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/ValTypeId.h>
#include <tables/Tables/TableError.h>


namespace casa { //# NAMESPACE CASA - BEGIN

template<class T>
ROArrayColumn<T>::ROArrayColumn()
: ROTableColumn (),
  canAccessSlice_p         (False),
  canAccessColumn_p        (False),
  canAccessColumnSlice_p   (False),
  reaskAccessSlice_p       (True),
  reaskAccessColumn_p      (True),
  reaskAccessColumnSlice_p (True)
{}

template<class T>
ROArrayColumn<T>::ROArrayColumn (const Table& tab,
				 const String& columnName)
: ROTableColumn (tab, columnName),
  canAccessSlice_p         (False),
  canAccessColumn_p        (False),
  canAccessColumnSlice_p   (False),
  reaskAccessSlice_p       (True),
  reaskAccessColumn_p      (True),
  reaskAccessColumnSlice_p (True)
{
    checkDataType();
}

template<class T>
ROArrayColumn<T>::ROArrayColumn (const ROTableColumn& column)
: ROTableColumn (column),
  canAccessSlice_p         (False),
  canAccessColumn_p        (False),
  canAccessColumnSlice_p   (False),
  reaskAccessSlice_p       (True),
  reaskAccessColumn_p      (True),
  reaskAccessColumnSlice_p (True)
{
    checkDataType();
}

template<class T>
ROArrayColumn<T>::ROArrayColumn (const ROArrayColumn<T>& that)
: ROTableColumn (that),
  canAccessSlice_p         (that.canAccessSlice_p),
  canAccessColumn_p        (that.canAccessColumn_p),
  canAccessColumnSlice_p   (that.canAccessColumnSlice_p),
  reaskAccessSlice_p       (that.reaskAccessSlice_p),
  reaskAccessColumn_p      (that.reaskAccessColumn_p),
  reaskAccessColumnSlice_p (that.reaskAccessColumnSlice_p)
{}

template<class T>
ROTableColumn* ROArrayColumn<T>::clone() const
{
    return new ROArrayColumn<T> (*this);
}

template<class T>
void ROArrayColumn<T>::reference (const ROArrayColumn<T>& that)
{
    ROTableColumn::reference (that);
    canAccessSlice_p         = that.canAccessSlice_p;
    canAccessColumn_p        = that.canAccessColumn_p;
    canAccessColumnSlice_p   = that.canAccessColumnSlice_p;
    reaskAccessSlice_p       = that.reaskAccessSlice_p;
    reaskAccessColumn_p      = that.reaskAccessColumn_p;
    reaskAccessColumnSlice_p = that.reaskAccessColumnSlice_p;
}

template<class T>
ROArrayColumn<T>::~ROArrayColumn()
{}


template<class T>
void ROArrayColumn<T>::checkDataType() const
{
    //# Check if the data type matches.
    const ColumnDesc& cd = baseColPtr_p->columnDesc();
    DataType dtype = cd.dataType();
    if (dtype != ValType::getType(static_cast<T*>(0))  ||  !cd.isArray()) {
	throw (TableInvDT (" in ROArrayColumn ctor for column " + cd.name()));
    }
    if (dtype == TpOther) {
	if (cd.dataTypeId() != valDataTypeId(static_cast<T*>(0))) {
	    throw (TableInvDT (" in ROArrayColumn ctor for column "
			       + cd.name() + "; using data type id "
			       + valDataTypeId(static_cast<T*>(0))
			       + ", expected " + cd.dataTypeId()));
	}
    }
}

template<class T>
void ROArrayColumn<T>::checkShape (const IPosition& shp,
                                   Array<T>& arr, Bool resize,
                                   const String& where) const
{
    if (! shp.isEqual (arr.shape())) {
	if (resize  ||  arr.nelements() == 0) {
	    arr.resize (shp);
	} else {
          throw TableArrayConformanceError(where);
	}
    }
}

template<class T>
Array<T> ROArrayColumn<T>::operator() (uInt rownr) const
{
    Array<T> arr;
    get (rownr, arr);
    return arr;
}

template<class T>
Array<T> ROArrayColumn<T>::get (uInt rownr) const
{
    Array<T> arr;
    get (rownr, arr);
    return arr;
}

template<class T>
void ROArrayColumn<T>::get (uInt rownr, Array<T>& arr, Bool resize) const
{
    TABLECOLUMNCHECKROW(rownr);
    // Check array conformance and resize if needed and possible.
    checkShape (shape(rownr), arr, resize, "ArrayColumn::get");
    baseColPtr_p->get (rownr, &arr);
}


template<class T>
Array<T> ROArrayColumn<T>::getSlice (uInt rownr,
				     const Slicer& arraySection) const
{
    Array<T> arr;
    getSlice (rownr, arraySection, arr);
    return arr;
}

template<class T>
void ROArrayColumn<T>::getSlice (uInt rownr, const Slicer& arraySection,
				 Array<T>& arr, Bool resize) const
{
    TABLECOLUMNCHECKROW(rownr);
    // Check array conformance and resize if needed and possible.
    IPosition arrayShape (shape(rownr));
    IPosition blc,trc,inc;
    IPosition shp = arraySection.inferShapeFromSource (arrayShape,
						       blc, trc, inc);
    checkShape (shp, arr, resize, "ArrayColumn::getSlice");
    //# Ask if we can access the slice (if that is not known yet).
    if (reaskAccessSlice_p) {
	canAccessSlice_p = baseColPtr_p->canAccessSlice (reaskAccessSlice_p);
    }
    //# Access the slice if possible.
    //# Otherwise get the entire array and slice it.
    if (canAccessSlice_p) {
        //# Creating a Slicer is somewhat expensive, so use the slicer
        //# itself if it contains no undefined values.
        if (arraySection.isFixed()) {
	    baseColPtr_p->getSlice (rownr,
				    arraySection,
				    &arr);
	} else {
	    baseColPtr_p->getSlice (rownr,
				    Slicer(blc, trc, inc,
					   Slicer::endIsLast),
				    &arr);
	}
    }else{
	Array<T> array(arrayShape);
	baseColPtr_p->get (rownr, &array);
	arr = array(blc, trc, inc);
    }
}


template<class T>
Array<T> ROArrayColumn<T>::getSlice
(uInt rownr, const Vector<Vector<Slice> >& arraySlices) const
{
    Array<T> arr;
    getSlice (rownr, arraySlices, arr);
    return arr;
}

template<class T>
void ROArrayColumn<T>::getSlice (uInt rownr,
                                 const Vector<Vector<Slice> >& arraySlices,
				 Array<T>& arr, Bool resize) const
{
  TABLECOLUMNCHECKROW(rownr);
  // Use shape of row.
  IPosition colShp = shape(rownr);
  Vector<Vector<Slice> > slices(arraySlices);
  Slicer slicer;
  IPosition shp = Slice::checkSlices (slices, slicer, colShp);
  // Check array conformance and resize if needed and possible.
  checkShape (shp, arr, resize, "ArrayColumn::getSlice");
  // Now loop through all the slices and fill the array in parts.
  IPosition arrEnd (slicer.length() - 1);
  GetCellSlices<T> functor(*this, rownr);
  handleSlices (slices, functor, slicer, arrEnd, arr);
}

template<class T>
void ROArrayColumn<T>::handleSlices (const Vector<Vector<Slice> >& slices,
                                     BaseSlicesFunctor<T>& functor,
                                     const Slicer& slicer,
                                     IPosition& arrEnd,
                                     Array<T>& arr) const
{
  uInt nrdim = slicer.ndim();
  IPosition arrStart (arrEnd.size(), 0); // for getColumn arrEnd.size() > nrdim
  IPosition colStart (slicer.start());
  IPosition colLen   (slicer.length());
  IPosition colIncr  (slicer.stride());
  IPosition pos(nrdim, 0);
  while (True) {
    Array<T> refArr (arr(arrStart, arrEnd));
    functor.apply (Slicer(colStart, colLen, colIncr), refArr);
    uInt i;
    for (i=0; i<nrdim; ++i) {
      pos[i]++;
      if (uInt(pos[i]) < slices[i].size()) {
        const Slice& slice = slices[i][pos[i]];
        colStart[i] = slice.start();
        colLen[i]   = slice.length();
        colIncr[i]  = slice.inc();
        arrStart[i] = arrEnd[i] + 1;
        arrEnd[i]   = arrStart[i] + slice.length() - 1;
        break;
      }
      pos[i] = 0;
      arrStart[i] = 0;
      arrEnd[i]   = slicer.length()[i] - 1;
      colStart[i] = slicer.start()[i];
      colLen[i]   = slicer.length()[i];
      colIncr[i]  = slicer.stride()[i];
    }
    if (i == nrdim) {
      break;
    }
  }
}


template<class T>
Array<T> ROArrayColumn<T>::getColumn() const
{
    Array<T> arr;
    getColumn (arr);
    return arr;
}

template<class T>
void ROArrayColumn<T>::getColumn (Array<T>& arr, Bool resize) const
{
    uInt nrrow = nrow();
    //# Take shape of array in first row.
    IPosition shp;
    if (nrrow > 0) {
	shp = shape(0);
    }
    //# Total shape is array shape plus nr of table rows.
    shp.append (IPosition(1,nrrow));
    // Check array conformance and resize if needed and possible.
    checkShape (shp, arr, resize, "ArrayColumn::getColumn");
    //# Ask if we can access the column (if that is not known yet).
    if (reaskAccessColumn_p) {
	canAccessColumn_p = baseColPtr_p->canAccessArrayColumn
	                                           (reaskAccessColumn_p);
    }
    //# Access the column if possible.
    //# Otherwise fill the entire array by looping through all cells.
    if (canAccessColumn_p) {
	baseColPtr_p->getArrayColumn (&arr);
    }else{
        if (arr.nelements() > 0) {
	    ArrayIterator<T> iter(arr, arr.ndim()-1);
	    for (uInt rownr=0; rownr<nrrow; rownr++) {
	        Array<T>& darr = iter.array();
		if (! darr.shape().isEqual (baseColPtr_p->shape (rownr))) {
		  throw TableArrayConformanceError
                    ("ArrayColumn::getColumn cannot be done for column " +
                     baseColPtr_p->columnDesc().name() +
                     "; the array shapes vary");
		}
	        baseColPtr_p->get (rownr, &darr);
		iter.next();
	    }
	}
    }
}


template<class T>
Array<T> ROArrayColumn<T>::getColumn (const Slicer& arraySection) const
{
    Array<T> arr;
    getColumn (arraySection, arr);
    return arr;
}

template<class T>
void ROArrayColumn<T>::getColumn (const Slicer& arraySection,
				  Array<T>& arr, Bool resize) const
{
    uInt nrrow = nrow();
    //# Use shape of array in first row.
    IPosition shp, blc,trc,inc;
    if (nrrow > 0) {
	shp = arraySection.inferShapeFromSource (shape(0), blc, trc, inc);
    }
    //# Total shape is slice shape plus nr of table rows.
    shp.append (IPosition(1,nrrow));
    // Check array conformance and resize if needed and possible.
    checkShape (shp, arr, resize, "ArrayColumn::getColumn");
    //# Ask if we can access the column slice (if that is not known yet).
    if (reaskAccessColumnSlice_p) {
	canAccessColumnSlice_p = baseColPtr_p->canAccessColumnSlice
	                                       (reaskAccessColumnSlice_p);
    }
    //# Access the column slice if possible.
    //# Otherwise fill the entire array by looping through all cells.
    Slicer defSlicer (blc, trc, inc, Slicer::endIsLast);
    if (canAccessColumnSlice_p) {
        baseColPtr_p->getColumnSlice (defSlicer, &arr);
    }else{
        if (arr.nelements() > 0) {
	    ArrayIterator<T> iter(arr, arr.ndim()-1);
	    for (uInt rownr=0; rownr<nrrow; rownr++) {
	        getSlice (rownr, defSlicer, iter.array());
		iter.next();
	    }
	}
    }
}


template<class T>
Array<T> ROArrayColumn<T>::getColumn
(const Vector<Vector<Slice> >& arraySlices) const
{
    Array<T> arr;
    getColumn (arraySlices, arr);
    return arr;
}

template<class T>
void ROArrayColumn<T>::getColumn (const Vector<Vector<Slice> >& arraySlices,
				  Array<T>& arr, Bool resize) const
{
  uInt nrrow = nrow();
  // Get total shape.
  // Use shape of first row (if there) as overall array shape.
  IPosition colShp;
  if (nrrow > 0) {
    colShp = shape(0);
  }
  Vector<Vector<Slice> > slices(arraySlices);
  Slicer slicer;
  IPosition shp = Slice::checkSlices (slices, slicer, colShp);
  // Total shape is slice shape plus nr of table rows.
  shp.append (IPosition(1,nrrow));
  // Check array conformance and resize if needed and possible.
  checkShape (shp, arr, resize, "ArrayColumn::getColumn");
  // Now loop through all the slices and fill the array in parts.
  IPosition arrEnd (slicer.length() - 1);
  arrEnd.append (IPosition(1,nrrow-1));
  GetColumnSlices<T> functor(*this);
  this->handleSlices (slices, functor, slicer, arrEnd, arr);
}


template<class T>
Array<T> ROArrayColumn<T>::getColumnRange (const Slicer& rowRange) const
{
    Array<T> arr;
    getColumnRange (rowRange, arr);
    return arr;
}

template<class T>
void ROArrayColumn<T>::getColumnRange (const Slicer& rowRange,
				       Array<T>& arr, Bool resize) const
{
    uInt nrrow = nrow();
    IPosition shp, blc, trc, inc;
    shp = rowRange.inferShapeFromSource (IPosition(1,nrrow), blc, trc, inc);
    //# If the entire column is accessed, use that function.
    if (blc(0) == 0  &&  shp(0) == Int(nrrow)  &&  inc(0) == 1) {
	getColumn (arr, resize);
    } else {
	getColumnCells (RefRows(blc(0), trc(0), inc(0)), arr, resize);
    }
}

template<class T>
Array<T> ROArrayColumn<T>::getColumnCells (const RefRows& rownrs) const
{
    Array<T> arr;
    getColumnCells (rownrs, arr);
    return arr;
}

template<class T>
void ROArrayColumn<T>::getColumnCells (const RefRows& rownrs,
				       Array<T>& arr, Bool resize) const
{
    uInt nrrow = rownrs.nrow();
     //# Take shape of array in first row.
    IPosition arrshp;
    if (nrrow > 0) {
	arrshp = shape(rownrs.firstRow());
    }
    //# Total shape is array shape plus nr of table rows.
    arrshp.append (IPosition(1,nrrow));
    // Check array conformance and resize if needed and possible.
    checkShape (arrshp, arr, resize, "ArrayColumn::getColumnCells");
    baseColPtr_p->getArrayColumnCells (rownrs, &arr);
}


template<class T>
Array<T> ROArrayColumn<T>::getColumnRange (const Slicer& rowRange,
					   const Slicer& arraySection) const
{
    Array<T> arr;
    getColumnRange (rowRange, arraySection, arr);
    return arr;
}

template<class T>
void ROArrayColumn<T>::getColumnRange (const Slicer& rowRange,
				       const Slicer& arraySection,
				       Array<T>& arr, Bool resize) const
{
    uInt nrrow = nrow();
    IPosition shp, blc, trc, inc;
    shp = rowRange.inferShapeFromSource (IPosition(1,nrrow), blc, trc, inc);
    //# If the entire column is accessed, use that function.
    if (blc(0) == 0  &&  shp(0) == Int(nrrow)  &&  inc(0) == 1) {
	getColumn (arraySection, arr, resize);
    } else {
	getColumnCells (RefRows(blc(0), trc(0), inc(0)),
			arraySection, arr, resize);
    }
}

template<class T>
Array<T> ROArrayColumn<T>::getColumnCells (const RefRows& rownrs,
					   const Slicer& arraySection) const
{
    Array<T> arr;
    getColumnCells (rownrs, arraySection, arr);
    return arr;
}

template<class T>
void ROArrayColumn<T>::getColumnCells (const RefRows& rownrs,
				       const Slicer& arraySection,
				       Array<T>& arr, Bool resize) const
{
    uInt nrrow = rownrs.nrow();
    IPosition arrshp, arrblc, arrtrc, arrinc;
    if (nrrow > 0) {
	arrshp = arraySection.inferShapeFromSource (shape(rownrs.firstRow()),
						    arrblc, arrtrc, arrinc);
    }
    //# Total shape is slice shape plus nr of table rows.
    arrshp.append (IPosition(1,nrrow));
    // Check array conformance and resize if needed and possible.
    checkShape (arrshp, arr, resize, "ArrayColumn::getColumnCells");
    baseColPtr_p->getColumnSliceCells (rownrs, Slicer(arrblc, arrtrc, arrinc,
						      Slicer::endIsLast),
				       &arr);
}




template<class T>
ArrayColumn<T>::ArrayColumn()
: ROTableColumn    (),
  ROArrayColumn<T> (),
  TableColumn      ()
{}

template<class T>
ArrayColumn<T>::ArrayColumn (const Table& tab, const String& columnName)
: ROTableColumn    (tab, columnName),
  ROArrayColumn<T> (tab, columnName),
  TableColumn      (tab, columnName)
{}

template<class T>
ArrayColumn<T>::ArrayColumn (const TableColumn& column)
: ROTableColumn    (column),
  ROArrayColumn<T> (column),
  TableColumn      (column)
{}

template<class T>
ArrayColumn<T>::ArrayColumn (const ArrayColumn<T>& that)
: ROTableColumn    (that),
  ROArrayColumn<T> (that),
  TableColumn      (that)
{}

template<class T>
ROTableColumn* ArrayColumn<T>::clone() const
{
    return new ArrayColumn<T> (*this);
}

template<class T>
void ArrayColumn<T>::reference (const ArrayColumn<T>& that)
    { ROArrayColumn<T>::reference (that); }

template<class T>
ArrayColumn<T>::~ArrayColumn()
{}


template<class T>
void ArrayColumn<T>::setShape (uInt rownr, const IPosition& shape)
{
    TABLECOLUMNCHECKROW(rownr); 
    //# Set shape if not defined yet or if changed (if possible).
    //# Throw exception if already defined with a different shape.
    if (canChangeShape_p  ||  !isDefined(rownr)) {
	baseColPtr_p->setShape (rownr, shape);
    }else{
	if (! shape.isEqual (baseColPtr_p->shape (rownr))) {
	    throw (TableInvOper
		   ("ArrayColumn::setShape; shape cannot be changed for row " +
                    String::toString(rownr) + " column " +
                    baseColPtr_p->columnDesc().name()));	
	}
    }
}
	
template<class T>
void ArrayColumn<T>::setShape (uInt rownr, const IPosition& shape,
			       const IPosition& tileShape)
{
    TABLECOLUMNCHECKROW(rownr); 
    //# Only set shape if not defined yet.
    //# Throw exception if already defined with a different shape.
    if (canChangeShape_p  ||  !isDefined(rownr)) {
	baseColPtr_p->setShape (rownr, shape, tileShape);
    }else{
	if (! shape.isEqual (baseColPtr_p->shape (rownr))) {
	    throw (TableInvOper
		   ("ArrayColumn::setShape; shape cannot be changed for row " +
                    String::toString(rownr) + " column " +
                    baseColPtr_p->columnDesc().name()));	
        }
    }
}
	
template<class T>
void ArrayColumn<T>::put (uInt rownr, const Array<T>& arr)
{
    TABLECOLUMNCHECKROW(rownr); 
    //# Define the shape if not defined yet.
    //# If defined, check if shape conforms.
    if (!isDefined(rownr)) {
	baseColPtr_p->setShape (rownr, arr.shape());
    }else{
	if (! arr.shape().isEqual (baseColPtr_p->shape (rownr))) {
	    if (!canChangeShape_p) {
		throw (TableArrayConformanceError
                       ("ArrayColumn::put for row " + String::toString(rownr) +
                        " in column " + baseColPtr_p->columnDesc().name()));
    }
	    baseColPtr_p->setShape (rownr, arr.shape());
	}
    }
    baseColPtr_p->put (rownr, &arr);
}

template<class T>
void ArrayColumn<T>::putSlice (uInt rownr, const Slicer& arraySection,
			       const Array<T>& arr)
{
    TABLECOLUMNCHECKROW(rownr); 
    //# Check the array conformance.
    IPosition arrayShape (shape(rownr));
    IPosition blc,trc,inc;
    IPosition shp = arraySection.inferShapeFromSource (arrayShape, blc,trc,inc);
    if (! shp.isEqual (arr.shape())) {
	throw (TableArrayConformanceError
               ("ArrayColumn::putSlice for row " + String::toString(rownr) +
                " in column " + baseColPtr_p->columnDesc().name()));
    }
    //# Ask if we can access the slice (if that is not known yet).
    if (reaskAccessSlice_p) {
	canAccessSlice_p = baseColPtr_p->canAccessSlice (reaskAccessSlice_p);
    }
    //# Access the slice if possible.
    //# Otherwise get the entire array, put the slice and put it back.
    if (canAccessSlice_p) {
	baseColPtr_p->putSlice (rownr, arraySection, &arr);
    }else{
	Array<T> array(arrayShape);
	baseColPtr_p->get (rownr, &array);
	array(blc, trc, inc) = arr;
	baseColPtr_p->put (rownr, &array);
    }
}

template<class T>
void ArrayColumn<T>::putSlice (uInt rownr,
                               const Vector<Vector<Slice> >& arraySlices,
			       const Array<T>& arr)
{
  TABLECOLUMNCHECKROW(rownr);
  // Use shape of the row.
  IPosition colShp = shape(rownr);
  Vector<Vector<Slice> > slices(arraySlices);
  Slicer slicer;
  IPosition shp = Slice::checkSlices (slices, slicer, colShp);
  if (! shp.isEqual (arr.shape())) {
    throw (TableArrayConformanceError
           ("ArrayColumn::putSlice for row " + String::toString(rownr) +
            " in column " + baseColPtr_p->columnDesc().name()));
  }
  // Now loop through all the slices and fill the array in parts.
  IPosition arrEnd (slicer.length() - 1);
  PutCellSlices<T> functor(*this, rownr);
  Array<T> arrc(arr);     // make non-const
  this->handleSlices (slices, functor, slicer, arrEnd, arrc);
}

template<class T>
void ArrayColumn<T>::put (uInt thisRownr, const ROTableColumn& that,
			  uInt thatRownr)
{
    TableColumn::put (thisRownr, that, thatRownr);
}

template<class T>
void ArrayColumn<T>::putColumn (const Array<T>& arr)
{
    //# First check if number of rows matches.
    uInt nrrow = nrow();
    IPosition shp  = arr.shape();
    uInt last = shp.nelements() - 1;
    if (shp(last) != Int(nrrow)) {
	throw (TableArrayConformanceError
               ("ArrayColumn::putColumn for column " +
                baseColPtr_p->columnDesc().name()));
    }
    //# Remove #rows from shape to get the shape of each cell.
    shp.resize (last);
    //# If the array is fixed shape, check if the shape matches.
    if ((columnDesc().options() & ColumnDesc::FixedShape)
	                                     == ColumnDesc::FixedShape) {
	if (! shp.isEqual (shapeColumn())) {
	    throw (TableArrayConformanceError
                   ("ArrayColumn::putColumn for column " +
                    baseColPtr_p->columnDesc().name()));
	}
    }else{
	//# Otherwise set the shape of each cell (as far as needed).
	for (uInt i=0; i<nrrow; i++) {
	    setShape (i, shp);
	}
    }
    //# Ask if we can access the column (if that is not known yet).
    if (reaskAccessColumn_p) {
	canAccessColumn_p = baseColPtr_p->canAccessArrayColumn
	                                             (reaskAccessColumn_p);
    }
    //# Access the column if possible.
    //# Otherwise put the entire array by looping through all cells.
    if (canAccessColumn_p) {
	baseColPtr_p->putArrayColumn (&arr);
    }else{
        if (arr.nelements() > 0) {
	    ReadOnlyArrayIterator<T> iter(arr, arr.ndim()-1);
	    for (uInt rownr=0; rownr<nrrow; rownr++) {
	        baseColPtr_p->put (rownr, &(iter.array()));
		iter.next();
	    }
	}
    }
}

template<class T>
void ArrayColumn<T>::putColumn (const Slicer& arraySection, const Array<T>& arr)
{
    uInt nrrow = nrow();
    //# First check if number of rows matches.
    IPosition arrshp = arr.shape();
    uInt last = arrshp.nelements() - 1;
    if (arrshp(last) != Int(nrrow)) {
	throw (TableArrayConformanceError
               ("ArrayColumn::putColumn for column " +
                baseColPtr_p->columnDesc().name()));
    }
    //# If the array is fixed shape, check if the shape matches.
    if ((columnDesc().options() & ColumnDesc::FixedShape)
	                                     == ColumnDesc::FixedShape) {
	//# Remove #rows from shape to get the shape of each cell.
	arrshp.resize (last);
	IPosition blc,trc,inc;
	IPosition shp = arraySection.inferShapeFromSource (shapeColumn(),
							   blc,trc,inc);
	if (! shp.isEqual(arrshp)) {
	    throw (TableArrayConformanceError
                   ("ArrayColumn::putColumn for column " +
                    baseColPtr_p->columnDesc().name()));
	}
    }
    //# Ask if we can access the column slice (if that is not known yet).
    if (reaskAccessColumnSlice_p) {
	canAccessColumnSlice_p = baseColPtr_p->canAccessColumnSlice
	                                       (reaskAccessColumnSlice_p);
    }
    //# Access the column slice if possible.
    //# Otherwise put the entire array by looping through all cells.
    if (canAccessColumnSlice_p) {
	baseColPtr_p->putColumnSlice (arraySection, &arr);
    }else{
        if (arr.nelements() > 0) {
	    ReadOnlyArrayIterator<T> iter(arr, arr.ndim()-1);
	    for (uInt rownr=0; rownr<nrrow; rownr++) {
	        putSlice (rownr, arraySection, iter.array());
		iter.next();
	    }
	}
    }
}

template<class T>
void ArrayColumn<T>::putColumn (const Vector<Vector<Slice> >& arraySlices,
                                const Array<T>& arr)
{
  uInt nrrow = nrow();
  // Get total shape.
  // Use shape of first row (if there) as overall array shape.
  IPosition colShp;
  if (nrrow > 0) {
    colShp = shape(0);
  }
  Vector<Vector<Slice> > slices(arraySlices);
  Slicer slicer;
  IPosition shp = Slice::checkSlices (slices, slicer, colShp);
  // Total shape is slice shape plus nr of table rows.
  shp.append (IPosition(1,nrrow));
  // Check array conformance.
  if (! shp.isEqual (arr.shape())) {
    throw (TableArrayConformanceError
           ("ArrayColumn::putColumn for column " +
            baseColPtr_p->columnDesc().name()));
  }
  // Now loop through all the slices and fill the array in parts.
  IPosition arrEnd (slicer.length() - 1);
  arrEnd.append (IPosition(1,nrrow-1));
  PutColumnSlices<T> functor(*this);
  Array<T> arrc(arr);     // make non-const
  this->handleSlices (slices, functor, slicer, arrEnd, arrc);
}

template<class T>
void ArrayColumn<T>::putColumnRange (const Slicer& rowRange,
				     const Array<T>& arr)
{
    uInt nrrow = nrow();
    IPosition shp, blc, trc, inc;
    shp = rowRange.inferShapeFromSource (IPosition(1,nrrow), blc, trc, inc);
    //# If the entire column is accessed, use that function.
    if (blc(0) == 0  &&  shp(0) == Int(nrrow)  &&  inc(0) == 1) {
	putColumn (arr);
    } else {
	putColumnCells (RefRows(blc(0), trc(0), inc(0)), arr);
    }
}

template<class T>
void ArrayColumn<T>::putColumnCells (const RefRows& rownrs,
				     const Array<T>& arr)
{
    //# First check if number of rows matches.
    uInt nrrow = rownrs.nrow();
    IPosition arrshp  = arr.shape();
    uInt last = arrshp.nelements() - 1;
    if (arrshp(last) != Int(nrrow)) {
	throw (TableArrayConformanceError
               ("ArrayColumn::putColumnCells for column " +
                baseColPtr_p->columnDesc().name()));
    }
    //# Remove #rows from shape to get the shape of each cell.
    arrshp.resize (last);
    //# If the array is fixed shape, check if the shape matches.
    if ((columnDesc().options() & ColumnDesc::FixedShape)
	                                     == ColumnDesc::FixedShape) {
	if (! arrshp.isEqual (shapeColumn())) {
	    throw (TableArrayConformanceError
                   ("ArrayColumn::putColumnCells for column " +
                    baseColPtr_p->columnDesc().name()));
	}
    }else{
	//# Otherwise set the shape of each cell (as far as needed).
        RefRowsSliceIter iter(rownrs);
        while (! iter.pastEnd()) {
            uInt rownr = iter.sliceStart();
            uInt end = iter.sliceEnd();
            uInt incr = iter.sliceIncr();
            while (rownr <= end) {
		setShape (rownr, arrshp);
		rownr += incr;
     	    }
	    iter++;
	}
    }
    //# Put the entire array.
    baseColPtr_p->putArrayColumnCells (rownrs, &arr);
}

template<class T>
void ArrayColumn<T>::putColumnRange (const Slicer& rowRange,
				     const Slicer& arraySection,
				     const Array<T>& arr)
{
    uInt nrrow = nrow();
    IPosition shp, blc, trc, inc;
    shp = rowRange.inferShapeFromSource (IPosition(1,nrrow), blc, trc, inc);
    //# If the entire column is accessed, use that function.
    if (blc(0) == 0  &&  shp(0) == Int(nrrow)  &&  inc(0) == 1) {
	putColumn (arraySection, arr);
    } else {
	putColumnCells (RefRows(blc(0), trc(0), inc(0)), arraySection, arr);
    }
}

template<class T>
void ArrayColumn<T>::putColumnCells (const RefRows& rownrs,
				     const Slicer& arraySection,
				     const Array<T>& arr)
{
    //# First check if number of rows matches.
    uInt nrrow = rownrs.nrow();
    IPosition arrshp = arr.shape();
    uInt last = arrshp.nelements() - 1;
    if (arrshp(last) != Int(nrrow)) {
	throw (TableArrayConformanceError
               ("ArrayColumn::putColumnCells for column " +
                baseColPtr_p->columnDesc().name()));
    }
    //# If the array is fixed shape, check if the shape matches.
    if ((columnDesc().options() & ColumnDesc::FixedShape)
	                                     == ColumnDesc::FixedShape) {
	//# Remove #rows from shape to get the shape of each cell.
	arrshp.resize (last);
	IPosition arrshp2,arrblc,arrtrc,arrinc;
	arrshp2 = arraySection.inferShapeFromSource (shapeColumn(),
						     arrblc, arrtrc, arrinc);
	if (! arrshp.isEqual(arrshp2)) {
	    throw (TableArrayConformanceError
                   ("ArrayColumn::putColumnCells for column " +
                    baseColPtr_p->columnDesc().name()));
	}
    }
    //# Put the entire array.
    baseColPtr_p->putColumnSliceCells (rownrs, arraySection, &arr);
}


template<class T>
void ArrayColumn<T>::put (uInt thisRownr, const ROArrayColumn<T>& that,
			  uInt thatRownr)
{
    put (thisRownr, that(thatRownr));
}

//# This is a very simple implementation.
//# However, it does not need to be more fancy, since an array operation
//# is already much more expensive than the virtual function calls
//# involved in each loop iteration.
template<class T>
void ArrayColumn<T>::fillColumn (const Array<T>& value)
{
    uInt nrrow = nrow();
    for (uInt i=0; i<nrrow; i++) {
	put (i, value);
    }
}

template<class T>
void ArrayColumn<T>::putColumn (const ROArrayColumn<T>& that)
{
    //# Check the column lengths.
    uInt nrrow = nrow();
    if (nrrow != that.nrow()) {
      throw (TableConformanceError
             ("Nr of rows differ in ArrayColumn::putColumn for column " +
              baseColPtr_p->columnDesc().name()));
    }
    for (uInt i=0; i<nrrow; i++) {
	put (i, that, i);
    }
}

} //# NAMESPACE CASA - END
