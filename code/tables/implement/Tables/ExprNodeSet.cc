//# ExprNodeSet.cc: Classes representing an set in table select expression
//# Copyright (C) 1997
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

#include <aips/Tables/ExprNodeSet.h>
#include <aips/Tables/ExprNode.h>
#include <aips/Tables/ExprDerNode.h>
#include <aips/Tables/ExprDerNodeArray.h>
#include <aips/Tables/TableError.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/ArrayIO.h>
#include <aips/Mathematics/Math.h>
#include <aips/Utilities/Assert.h>


TableExprNodeSetElem::TableExprNodeSetElem (const TableExprNode& value)
: TableExprNodeRep (NTDouble, VTSetElem, OtUndef, 0),
  itsStart (0),
  itsEnd   (0),
  itsIncr  (0),
  itsDiscrete (True),
  itsSingle   (True)
{
    //# Note that the TableExprNode copy ctor is needed to get rid of const.
    TableExprNode tmp(value);
    itsStart = getRep(tmp)->link();
    dtype_p = itsStart->dataType();
    checkTable();
}

TableExprNodeSetElem::TableExprNodeSetElem (const TableExprNode* start,
					    const TableExprNode* end,
					    const TableExprNode* incr)
: TableExprNodeRep (NTDouble, VTSetElem, OtUndef, 0),
  itsStart (0),
  itsEnd   (0),
  itsIncr  (0),
  itsDiscrete (True),
  itsSingle   (False)
{
    // Link to the nodes and determine the data types.
    //# Note that the TableExprNode copy ctor is needed to get rid of const.
    NodeDataType dts = NTDouble;
    if (start != 0) {
	TableExprNode tmp(*start);
	itsStart = getRep(tmp)->link();
	dts = itsStart->dataType();
    }
    NodeDataType dte = dts;
    if (end != 0) {
	TableExprNode tmp(*end);
	itsEnd = getRep(tmp)->link();
	dte = itsEnd->dataType();
    }
    NodeDataType dti = NTDouble;
    if (incr != 0) {
	TableExprNode tmp(*incr);
	itsIncr = getRep(tmp)->link();
	dti = itsIncr->dataType();
    }
    if ((dts != NTDouble  &&  dts != NTDate)
    ||  dte != dts  ||  dti != NTDouble) {
	throw (TableInvExpr ("start:end should have equal data types ( Double"
			     " or Date) and incr should have Double"));
    }
    dtype_p = dts;
    checkTable();
}

TableExprNodeSetElem::TableExprNodeSetElem (Bool isLeftClosed,
					    const TableExprNode& start,
					    const TableExprNode& end,
					    Bool isRightClosed)
: TableExprNodeRep (NTDouble, VTSetElem, OtUndef, 0)
{
    setup (isLeftClosed, &start, &end, isRightClosed);
}

TableExprNodeSetElem::TableExprNodeSetElem (Bool isLeftClosed,
					    const TableExprNode& start)
: TableExprNodeRep (NTDouble, VTSetElem, OtUndef, 0)
{
    setup (isLeftClosed, &start, 0, False);
}

TableExprNodeSetElem::TableExprNodeSetElem (const TableExprNode& end,
					    Bool isRightClosed)
: TableExprNodeRep (NTDouble, VTSetElem, OtUndef, 0)
{
    setup (False, 0, &end, isRightClosed);
}

TableExprNodeSetElem::TableExprNodeSetElem (const TableExprNodeSetElem& that)
: TableExprNodeRep (that),
  itsStart         (that.itsStart),
  itsEnd           (that.itsEnd),
  itsIncr          (that.itsIncr),
  itsLeftClosed    (that.itsLeftClosed),
  itsRightClosed   (that.itsRightClosed),
  itsDiscrete      (that.itsDiscrete),
  itsSingle        (that.itsSingle)
{
    if (itsStart != 0) {
	itsStart->link();
    }
    if (itsEnd != 0) {
	itsEnd->link();
    }
    if (itsIncr != 0) {
	itsIncr->link();
    }
}

TableExprNodeSetElem::TableExprNodeSetElem (const TableExprNodeSetElem& that,
					    TableExprNodeRep* start,
					    TableExprNodeRep* end,
					    TableExprNodeRep* incr)
: TableExprNodeRep (that.dataType(), VTSetElem, OtUndef, 0),
  itsStart       (start),
  itsEnd         (end),
  itsIncr        (incr),
  itsLeftClosed  (that.itsLeftClosed),
  itsRightClosed (that.itsRightClosed),
  itsDiscrete    (that.itsDiscrete),
  itsSingle      (that.itsSingle)
{
    if (itsStart != 0) {
	itsStart->link();
    }
    if (itsEnd != 0) {
	itsEnd->link();
    }
    if (itsIncr != 0) {
	itsIncr->link();
    }
}

TableExprNodeSetElem::~TableExprNodeSetElem()
{
    unlink (itsStart);
    unlink (itsEnd);
    unlink (itsIncr);
}

void TableExprNodeSetElem::setup (Bool isLeftClosed,
				  const TableExprNode* start,
				  const TableExprNode* end,
				  Bool isRightClosed)
{
    itsStart = 0;
    itsEnd   = 0;
    itsIncr  = 0;
    itsLeftClosed  = isLeftClosed;
    itsRightClosed = isRightClosed;
    itsDiscrete    = False;
    itsSingle      = False;
    //# Note that the TableExprNode copy ctor is needed to get rid of const.
    if (start != 0) {
	TableExprNode tmp(*start);
	itsStart = getRep(tmp)->link();
	dtype_p = itsStart->dataType();
    }
    if (end != 0) {
	TableExprNode tmp(*end);
	itsEnd = getRep(tmp)->link();
	if (start != 0  &&  itsEnd->dataType() != dtype_p) {
	    throw (TableInvExpr ("start=:=end must have equal data types"));
	}
	dtype_p = itsEnd->dataType();
    }
    if (dataType() != NTDouble  &&  dataType() != NTString
    &&  dataType() != NTDate) {
	throw (TableInvExpr ("start:=:end only valid for Double,"
			     " String or Date"));
    }
    checkTable();
}

void TableExprNodeSetElem::show (ostream& os, uInt indent) const
{
    TableExprNodeRep::show (os, indent);
    if (itsStart != 0) {
	os << "start: ";
	itsStart->show (os, indent+2);
    }
    if (itsEnd != 0) {
	os << "end:   ";
	itsEnd->show (os, indent+2);
    }
    if (itsIncr != 0) {
	os << "incr:  ";
	itsIncr->show (os, indent+2);
    }
}

void TableExprNodeSetElem::checkTable()
{
    baseTabPtr_p = 0;
    checkTablePtr (itsStart);
    checkTablePtr (itsEnd);
    checkTablePtr (itsIncr);
    exprtype_p = Constant;
    fillExprType (itsStart);
    fillExprType (itsEnd);
    fillExprType (itsIncr);
}

void TableExprNodeSetElem::replaceTablePtr (const Table& table,
					    const BaseTable* baseTablePtr)
{
    baseTabPtr_p = baseTablePtr;
    if (itsStart != 0) {
	itsStart->replaceTablePtr (table, baseTablePtr);
    }
    if (itsEnd != 0) {
	itsEnd->replaceTablePtr (table, baseTablePtr);
    }
    if (itsIncr != 0) {
	itsIncr->replaceTablePtr (table, baseTablePtr);
    }
}

TableExprNodeSetElem* TableExprNodeSetElem::evaluate (uInt rownr) const
{
    TableExprNodeRep* start = 0;
    TableExprNodeRep* end = 0;
    TableExprNodeRep* incr = 0;
    switch (dataType()) {
    case NTBool:
	if (itsStart != 0) {
	    start = new TableExprNodeConstBool (itsStart->getBool (rownr));
	}
	break;
    case NTDouble:
	if (itsStart != 0) {
	    start = new TableExprNodeConstDouble (itsStart->getDouble (rownr));
	}
	if (itsEnd != 0) {
	    end = new TableExprNodeConstDouble (itsEnd->getDouble (rownr));
	}
	if (itsIncr != 0) {
	    incr = new TableExprNodeConstDouble (itsIncr->getDouble (rownr));
	}
	break;
    case NTComplex:
	if (itsStart != 0) {
	    start = new TableExprNodeConstDComplex
                                            (itsStart->getDComplex (rownr));
	}
	break;
    case NTString:
	if (itsStart != 0) {
	    start = new TableExprNodeConstString (itsStart->getString (rownr));
	}
	if (itsEnd != 0) {
	    end = new TableExprNodeConstString (itsEnd->getString (rownr));
	}
	break;
    case NTDate:
	if (itsStart != 0) {
	    start = new TableExprNodeConstDate (itsStart->getDate (rownr));
	}
	if (itsEnd != 0) {
	    end = new TableExprNodeConstDate (itsEnd->getDate (rownr));
	}
	if (itsIncr != 0) {
	    incr = new TableExprNodeConstDouble (itsIncr->getDouble (rownr));
	}
	break;
    default:
	TableExprNode::throwInvDT ("TableExprNodeSetElem::evaluate");
    }
    return new TableExprNodeSetElem (*this, start, end, incr);
}

void TableExprNodeSetElem::fillVector (Vector<Bool>& vec, uInt& cnt,
				       uInt rownr) const
{
    DebugAssert (itsSingle, AipsError);
    uInt n = vec.nelements();
    if (n < cnt+1) {
	vec.resize (cnt+1, True);
    }
    vec(cnt++) = itsStart->getBool (rownr);
}
void TableExprNodeSetElem::fillVector (Vector<Double>& vec, uInt& cnt,
				       uInt rownr) const
{
    DebugAssert (itsDiscrete, AipsError);
    Double start = itsStart==0  ?  0 : itsStart->getDouble (rownr);
    Double end   = itsEnd==0  ?  start : itsEnd->getDouble (rownr);
    Double incr  = itsIncr==0  ?  1 : itsIncr->getDouble (rownr);
    if (start > end) {
	return;
    }
    uInt nval = 1 + uInt((end - start) / incr);
    uInt n = vec.nelements();
    if (n < cnt+nval) {
	vec.resize (cnt+nval, True);
    }
    for (uInt i=0; i<nval; i++) {
	vec(cnt++) = start;
	start += incr;
    }
}
void TableExprNodeSetElem::fillVector (Vector<DComplex>& vec, uInt& cnt,
				       uInt rownr) const
{
    DebugAssert (itsSingle, AipsError);
    uInt n = vec.nelements();
    if (n < cnt+1) {
	vec.resize (cnt+1, True);
    }
    vec(cnt++) = itsStart->getDComplex (rownr);
}
void TableExprNodeSetElem::fillVector (Vector<String>& vec, uInt& cnt,
				       uInt rownr) const
{
    DebugAssert (itsSingle, AipsError);
    uInt n = vec.nelements();
    if (n < cnt+1) {
	vec.resize (cnt+1, True);
    }
    vec(cnt++) = itsStart->getString (rownr);
}
void TableExprNodeSetElem::fillVector (Vector<MVTime>& vec, uInt& cnt,
				       uInt rownr) const
{
    DebugAssert (itsDiscrete, AipsError);
    Double start = itsStart==0  ?  0 : itsStart->getDate (rownr);
    Double end   = itsEnd==0  ?  start : itsEnd->getDate (rownr);
    Double incr  = itsIncr==0  ?  1 : itsIncr->getDouble (rownr);
    if (start > end) {
	return;
    }
    uInt nval = 1 + uInt((end - start) / incr);
    uInt n = vec.nelements();
    if (n < cnt+nval) {
	vec.resize (cnt+nval, True);
    }
    for (uInt i=0; i<nval; i++) {
	vec(cnt++) = start;
	start += incr;
    }
}

void TableExprNodeSetElem::matchBool (Bool* match, const Bool* value,
				      uInt nval, uInt rownr) const
{
    DebugAssert (itsSingle, AipsError);
    Bool start = itsStart->getBool (rownr);
    Bool* lastVal = match + nval;
    while (match < lastVal) {
	if (*value == start) {
	    *match = True;
	}
	value++;
	match++;
    }
}
void TableExprNodeSetElem::matchDouble (Bool* match, const Double* value,
					uInt nval, uInt rownr) const
{
    Double start = itsStart==0  ?  0 : itsStart->getDouble (rownr);
    Double end   = itsEnd==0  ?  start : itsEnd->getDouble (rownr);
    Double incr  = itsIncr==0  ?  1 : itsIncr->getDouble (rownr);
    if (start > end) {
	return;
    }
    Bool* lastVal = match + nval;
    if (itsSingle) {
	while (match < lastVal) {
	    if (*value == start) {
		*match = True;
	    }
	    value++;
	    match++;
	}
    } else if (itsDiscrete) {
	end -= start;
	while (match < lastVal) {
	    Double tmp = *value - start;
	    if (tmp >= 0  &&  (itsEnd == 0  ||  tmp <= end)) {
		if (fmod(tmp, incr) == 0) {
		    *match = True;
		}
	    }
	    value++;
	    match++;
	}
    }else{
	while (match < lastVal) {
	    Double tmp = *value;
	    if ((itsStart == 0
             ||  tmp > start  ||  itsLeftClosed  &&  tmp == start)
	    &&  (itsEnd == 0
             ||  tmp < end  ||  itsRightClosed  &&  tmp == end)) {
		*match = True;
	    }
	    value++;
	    match++;
	}
    }
}
void TableExprNodeSetElem::matchDComplex (Bool* match, const DComplex* value,
					  uInt nval, uInt rownr) const
{
    DebugAssert (itsSingle, AipsError);
    DComplex start = itsStart->getDComplex (rownr);
    Bool* lastVal = match + nval;
    while (match < lastVal) {
	if (*value == start) {
	    *match = True;
	}
	value++;
	match++;
    }
}
void TableExprNodeSetElem::matchString (Bool* match, const String* value,
					uInt nval, uInt rownr) const
{
    String start;
    if (itsStart != 0) {
	start = itsStart->getString (rownr);
    }
    String end;
    if (itsEnd != 0) {
	end = itsEnd->getString (rownr);
    }
    if (start > end) {
	return;
    }
    Bool* lastVal = match + nval;
    if (itsDiscrete) {
	DebugAssert (itsSingle, AipsError);
	while (match < lastVal) {
	    if (*value == start) {
		*match = True;
	    }
	    value++;
	    match++;
	}
    }else{
	while (match < lastVal) {
	    if ((itsStart == 0
             ||  *value > start  ||  itsLeftClosed  &&  *value == start)
	    &&  (itsEnd == 0
             ||  *value < end  ||  itsRightClosed  &&  *value == end)) {
		*match = True;
	    }
	    value++;
	    match++;
	}
    }
}
void TableExprNodeSetElem::matchDate (Bool* match, const MVTime* value,
				      uInt nval, uInt rownr) const
{
    Double start = itsStart==0  ?  0 : itsStart->getDate (rownr);
    Double end   = itsEnd==0  ?  start : itsEnd->getDate (rownr);
    Double incr  = itsIncr==0  ?  1 : itsIncr->getDouble (rownr);
    if (start > end) {
	return;
    }
    Bool* lastVal = match + nval;
    if (itsSingle) {
	while (match < lastVal) {
	    if (Double(*value) == start) {
		*match = True;
	    }
	    value++;
	    match++;
	}
    } else if (itsDiscrete) {
	end -= start;
	while (match < lastVal) {
	    Double tmp = Double(*value) - start;
	    if (tmp >= 0  &&  (itsEnd == 0  ||  tmp <= end)) {
		Double div = tmp/incr;
		if (int(div) == div) {
		    *match = True;
		}
	    }
	    value++;
	    match++;
	}
    }else{
	while (match < lastVal) {
	    Double tmp = *value;
	    if ((itsStart == 0
             ||  tmp > start  ||  itsLeftClosed  &&  tmp == start)
	    &&  (itsEnd == 0
             ||  tmp < end  ||  itsRightClosed  &&  tmp == end)) {
		*match = True;
	    }
	    value++;
	    match++;
	}
    }
}



TableExprNodeSet::TableExprNodeSet()
: TableExprNodeRep (NTNumeric, VTSet, OtUndef, 0),
  itsSingle        (True),
  itsDiscrete      (True),
  itsBounded       (True)
{}

TableExprNodeSet::TableExprNodeSet (uInt n, const TableExprNodeSetElem& elem)
: TableExprNodeRep (elem.dataType(), VTSet, OtUndef, 0),
  itsSingle        (elem.isSingle()),
  itsDiscrete      (elem.isDiscrete()),
  itsBounded       (True),
  itsElems         (n)
{
    // Set is not bounded if the element is not discrete and if end is defined.
    if (!(itsSingle  ||  (itsDiscrete && elem.end() != 0))) {
	itsBounded  = False;
    }
    for (uInt i=0; i<n; i++) {
	itsElems[i] = elem.evaluate (i);
    }
}

TableExprNodeSet::TableExprNodeSet (const TableExprNodeSet& that)
: TableExprNodeRep (that),
  itsSingle        (that.itsSingle),
  itsDiscrete      (that.itsDiscrete),
  itsBounded       (that.itsBounded)
{
    uInt n = that.itsElems.nelements();
    itsElems.resize (n);
    for (uInt i=0; i<n; i++) {
	itsElems[i] = new TableExprNodeSetElem (*(that.itsElems[i]));
    }
}

TableExprNodeSet::~TableExprNodeSet()
{
    uInt n = itsElems.nelements();
    for (uInt i=0; i<n; i++) {
	delete itsElems[i];
    }
}

void TableExprNodeSet::add (const TableExprNodeSetElem& elem)
{
    uInt n = itsElems.nelements();
    itsElems.resize (n+1);
    itsElems[n] = new TableExprNodeSetElem (elem);
    if (! elem.isSingle()) {
	itsSingle = False;
	if (! elem.isDiscrete()) {
	    itsDiscrete = False;
	    itsBounded  = False;
	}else{
	    if (elem.end() == 0) {
		itsBounded = False;
	    }
	}
    }
    if (n == 0) {
	dtype_p = elem.dataType();
    }else{
	if (dtype_p != elem.dataType()) {
	    throw (TableInvExpr ("Set elements must have equal data types"));
	}
    }
    checkTablePtr (itsElems[n]);
    fillExprType  (itsElems[n]);
}

void TableExprNodeSet::show (ostream& os, uInt indent) const
{
    TableExprNodeRep::show (os, indent);
    for (uInt j=0; j<itsElems.nelements(); j++) {
	itsElems[j]->show (os, indent+2);
    }
}

Bool TableExprNodeSet::hasArrays() const
{
    //# Check if a value is an array?
    uInt n = itsElems.nelements();
    for (uInt i=0; i<n; i++) {
	const TableExprNodeSetElem& elem = *(itsElems[i]);
	if (elem.start() != 0
        &&  elem.start()->valueType() == VTArray) {
	    return True;
	}
	if (elem.end() != 0
        &&  elem.end()->valueType() == VTArray) {
	    return True;
	}
	if (elem.increment() != 0
        &&  elem.increment()->valueType() == VTArray) {
	    return True;
	}
    }
    return False;
}

void TableExprNodeSet::replaceTablePtr (const Table& table,
					const BaseTable* baseTablePtr)
{
    baseTabPtr_p = baseTablePtr;
    uInt n = nelements();
    for (uInt i=0; i<n; i++) {
	itsElems[i]->replaceTablePtr (table, baseTablePtr);
    }
}

TableExprNodeRep* TableExprNodeSet::setOrArray()
{
    // The set should not contain array elements.
    if (hasArrays()) {
	throw (TableInvExpr ("A set cannot contain elements having arrays"));
    }
    // When discrete, all start values should be filled in.
    if (itsDiscrete) {
	uInt n = nelements();
	for (uInt i=0; i<n; i++) {
	    if (itsElems[i]->start() == 0) {
		throw (TableInvExpr ("no start value in discrete interval"));
	    }
	}
    }
    // When the set is bounded, it can be converted to an array.
    if (itsBounded) {
	// When it is const, that can be done immediately.
	if (isConstant()) {
	    return toArray();
	}
	// Otherwise the type is set to VTArray and the getArray
	// functions convert the set to an array for each row.
	vtype_p = VTArray;
    }
    return new TableExprNodeSet (*this);
}

TableExprNodeRep* TableExprNodeSet::toArray() const
{
    // Construct the correct const array object.
    switch (dataType()) {
    case NTBool:
	return new TableExprNodeArrayConstBool (toArrayBool(0));
    case NTDouble:
	return new TableExprNodeArrayConstDouble (toArrayDouble(0));
    case NTComplex:
	return new TableExprNodeArrayConstDComplex (toArrayDComplex(0));
    case NTString:
	return new TableExprNodeArrayConstString (toArrayString(0));
    case NTDate:
	return new TableExprNodeArrayConstDate (toArrayDate(0));
    default:
	TableExprNode::throwInvDT ("TableExprNodeSet::toArray");
    }
    return 0;               // only to satisfy the compiler
}

Array<Bool> TableExprNodeSet::toArrayBool (uInt rownr) const
{
    DebugAssert (itsBounded, AipsError);
    // First determine (roughly) the number of values needed in
    // the resulting vector. This number is correct in case
    // single values are given (which is usually the case).
    // The fillVector functions will resize when needed.
    // At the end the vector is also resized in case it was too long.
    uInt n = nelements();
    uInt cnt = 0;
    Vector<Bool> result (n);
    for (uInt i=0; i<n; i++) {
	itsElems[i]->fillVector (result, cnt, rownr);
    }
    result.resize (cnt, True);
    return result;
}
Array<Double> TableExprNodeSet::toArrayDouble (uInt rownr) const
{
    DebugAssert (itsBounded, AipsError);
    uInt n = nelements();
    uInt cnt = 0;
    Vector<Double> result (n);
    for (uInt i=0; i<n; i++) {
	itsElems[i]->fillVector (result, cnt, rownr);
    }
    result.resize (cnt, True);
    return result;
}
Array<DComplex> TableExprNodeSet::toArrayDComplex (uInt rownr) const
{
    DebugAssert (itsBounded, AipsError);
    uInt n = nelements();
    uInt cnt = 0;
    Vector<DComplex> result (n);
    for (uInt i=0; i<n; i++) {
	itsElems[i]->fillVector (result, cnt, rownr);
    }
    result.resize (cnt, True);
    return result;
}
Array<String> TableExprNodeSet::toArrayString (uInt rownr) const
{
    DebugAssert (itsBounded, AipsError);
    uInt n = nelements();
    uInt cnt = 0;
    Vector<String> result (n);
    for (uInt i=0; i<n; i++) {
	itsElems[i]->fillVector (result, cnt, rownr);
    }
    result.resize (cnt, True);
    return result;
}
Array<MVTime> TableExprNodeSet::toArrayDate (uInt rownr) const
{
    DebugAssert (itsBounded, AipsError);
    uInt n = nelements();
    uInt cnt = 0;
    Vector<MVTime> result (n);
    for (uInt i=0; i<n; i++) {
	itsElems[i]->fillVector (result, cnt, rownr);
    }
    result.resize (cnt, True);
    return result;
}

Array<Bool> TableExprNodeSet::getArrayBool (uInt rownr)
{
    return toArrayBool (rownr);
}
Array<Double> TableExprNodeSet::getArrayDouble (uInt rownr)
{
    return toArrayDouble (rownr);
}
Array<DComplex> TableExprNodeSet::getArrayDComplex (uInt rownr)
{
    return toArrayDComplex (rownr);
}
Array<String> TableExprNodeSet::getArrayString (uInt rownr)
{
    return toArrayString (rownr);
}
Array<MVTime> TableExprNodeSet::getArrayDate (uInt rownr)
{
    return toArrayDate (rownr);
}

Bool TableExprNodeSet::hasBool (uInt rownr, Bool value)
{
    Bool result = False;
    uInt n = itsElems.nelements();
    for (uInt i=0; i<n; i++) {
	itsElems[i]->matchBool (&result, &value, 1, rownr);
    }
    return result;
}
Bool TableExprNodeSet::hasDouble (uInt rownr, Double value)
{
    Bool result = False;
    uInt n = itsElems.nelements();
    for (uInt i=0; i<n; i++) {
	itsElems[i]->matchDouble (&result, &value, 1, rownr);
    }
    return result;
}
Bool TableExprNodeSet::hasDComplex (uInt rownr, const DComplex& value)
{
    Bool result = False;
    uInt n = itsElems.nelements();
    for (uInt i=0; i<n; i++) {
	itsElems[i]->matchDComplex (&result, &value, 1, rownr);
    }
    return result;
}
Bool TableExprNodeSet::hasString (uInt rownr, const String& value)
{
    Bool result = False;
    uInt n = itsElems.nelements();
    for (uInt i=0; i<n; i++) {
	itsElems[i]->matchString (&result, &value, 1, rownr);
    }
    return result;
}
Bool TableExprNodeSet::hasDate (uInt rownr, const MVTime& value)
{
    Bool result = False;
    uInt n = itsElems.nelements();
    for (uInt i=0; i<n; i++) {
	itsElems[i]->matchDate (&result, &value, 1, rownr);
    }
    return result;
}
Array<Bool> TableExprNodeSet::hasArrayBool (uInt rownr,
					    const Array<Bool>& value)
{
    Array<Bool> set = getArrayBool (rownr);
    Array<Bool> result(value.shape());
    result.set (False);
    Bool deleteIn, deleteOut;
    const Bool* in = value.getStorage (deleteIn);
    Bool* out = result.getStorage (deleteOut);
    uInt nval = value.nelements();
    uInt n = itsElems.nelements();
    for (uInt i=0; i<n; i++) {
	itsElems[i]->matchBool (out, in, nval, rownr);
    }
    value.freeStorage (in, deleteIn);
    result.putStorage (out, deleteOut);
    return result;
}
Array<Bool> TableExprNodeSet::hasArrayDouble (uInt rownr,
					      const Array<Double>& value)
{
    Array<Double> set = getArrayDouble (rownr);
    Array<Bool> result(value.shape());
    result.set (False);
    Bool deleteIn, deleteOut;
    const Double* in = value.getStorage (deleteIn);
    Bool* out = result.getStorage (deleteOut);
    uInt nval = value.nelements();
    uInt n = itsElems.nelements();
    for (uInt i=0; i<n; i++) {
	itsElems[i]->matchDouble (out, in, nval, rownr);
    }
    value.freeStorage (in, deleteIn);
    result.putStorage (out, deleteOut);
    return result;
}
Array<Bool> TableExprNodeSet::hasArrayDComplex (uInt rownr,
						const Array<DComplex>& value)
{
    Array<DComplex> set = getArrayDComplex (rownr);
    Array<Bool> result(value.shape());
    result.set (False);
    Bool deleteIn, deleteOut;
    const DComplex* in = value.getStorage (deleteIn);
    Bool* out = result.getStorage (deleteOut);
    uInt nval = value.nelements();
    uInt n = itsElems.nelements();
    for (uInt i=0; i<n; i++) {
	itsElems[i]->matchDComplex (out, in, nval, rownr);
    }
    value.freeStorage (in, deleteIn);
    result.putStorage (out, deleteOut);
    return result;
}
Array<Bool> TableExprNodeSet::hasArrayString (uInt rownr,
					      const Array<String>& value)
{
    Array<String> set = getArrayString (rownr);
    Array<Bool> result(value.shape());
    result.set (False);
    Bool deleteIn, deleteOut;
    const String* in = value.getStorage (deleteIn);
    Bool* out = result.getStorage (deleteOut);
    uInt nval = value.nelements();
    uInt n = itsElems.nelements();
    for (uInt i=0; i<n; i++) {
	itsElems[i]->matchString (out, in, nval, rownr);
    }
    value.freeStorage (in, deleteIn);
    result.putStorage (out, deleteOut);
    return result;
}
Array<Bool> TableExprNodeSet::hasArrayDate (uInt rownr,
					    const Array<MVTime>& value)
{
    Array<MVTime> set = getArrayDate (rownr);
    Array<Bool> result(value.shape());
    result.set (False);
    Bool deleteIn, deleteOut;
    const MVTime* in = value.getStorage (deleteIn);
    Bool* out = result.getStorage (deleteOut);
    uInt nval = value.nelements();
    uInt n = itsElems.nelements();
    for (uInt i=0; i<n; i++) {
	itsElems[i]->matchDate (out, in, nval, rownr);
    }
    value.freeStorage (in, deleteIn);
    result.putStorage (out, deleteOut);
    return result;
}
