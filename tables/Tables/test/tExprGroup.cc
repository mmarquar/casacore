//# tExprNode.cc: Test program for the grouping aggregate functions
//# Copyright (C) 2013
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: tExprNode.cc 21156 2011-12-12 07:57:36Z gervandiepen $

#include <casa/Containers/Record.h>
#include <tables/Tables/ExprNode.h>
#include <tables/Tables/ExprNodeSet.h>
#include <tables/Tables/ExprAggrNode.h>
#include <tables/Tables/ExprGroupAggrFunc.h>
#include <tables/Tables/RecordExpr.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/ArrayUtil.h>
#include <casa/BasicSL/Constants.h>
#include <casa/Utilities/Assert.h>
#include <casa/stdvector.h>
#include <casa/iostream.h>

#include <casa/namespace.h>
// <summary>
// Test program for class TableExprAggrNode.
// </summary>

// Keeps track if errors occurred.
Bool foundError = False;


#define checkFailure(STR,EXPR)\
{\
  bool failed = False;\
  try {\
    TableExprNode n(EXPR);\
  } catch (std::exception&) {\
    failed = True;\
  }\
  if (!failed) {\
    cout << STR << ": was expected to fail, but did not" << endl;\
  }\
}

void check (const TableExprNode& expr,
            const vector<Record>& recs,
            Bool expVal, const String& str)
{
  cout << "Test " << str << endl;
  // Get the aggregation node.
  TableExprAggrNode* aggr = const_cast<TableExprAggrNode*>
    (static_cast<const TableExprAggrNode*>(expr.getNodeRep()));
  AlwaysAssertExit (aggr);
  TableExprGroupFunc* func = aggr->makeGroupFunc();
  for (uInt i=0; i<recs.size(); ++i) {
    TableExprId id(recs[i]);
    func->apply (*aggr, id);
  }
  func->finish();
  Bool val = func->getBool();
  if (val != expVal) {
    foundError = True;
    cout << str << ": found value " << val << "; expected "
         << expVal << endl;
  }
}

void check (const TableExprNode& expr,
            const vector<Record>& recs,
            Int expVal, const String& str)
{
  cout << "Test " << str << endl;
  // Get the aggregation node.
  TableExprAggrNode* aggr = const_cast<TableExprAggrNode*>
    (static_cast<const TableExprAggrNode*>(expr.getNodeRep()));
  AlwaysAssertExit (aggr);
  TableExprGroupFunc* func = aggr->makeGroupFunc();
  for (uInt i=0; i<recs.size(); ++i) {
    TableExprId id(recs[i]);
    func->apply (*aggr, id);
  }
  func->finish();
  Int val = func->getInt();
  if (val != expVal) {
    foundError = True;
    cout << str << ": found value " << val << "; expected "
         << expVal << endl;
  }
}

void check (const TableExprNode& expr,
            const vector<Record>& recs,
            Double expVal, const String& str)
{
  cout << "Test " << str << endl;
  // Get the aggregation node.
  TableExprAggrNode* aggr = const_cast<TableExprAggrNode*>
    (static_cast<const TableExprAggrNode*>(expr.getNodeRep()));
  AlwaysAssertExit (aggr);
  TableExprGroupFunc* func = aggr->makeGroupFunc();
  for (uInt i=0; i<recs.size(); ++i) {
    TableExprId id(recs[i]);
    func->apply (*aggr, id);
  }
  func->finish();
  Double val = func->getDouble();
  if (!near (val, expVal, 1.e-10)) {
    foundError = True;
    cout << str << ": found value " << val << "; expected "
         << expVal << endl;
  }
}

void check (const TableExprNode& expr,
            const vector<Record>& recs,
            const DComplex& expVal, const String& str)
{
  cout << "Test " << str << endl;
  // Get the aggregation node.
  TableExprAggrNode* aggr = const_cast<TableExprAggrNode*>
    (static_cast<const TableExprAggrNode*>(expr.getNodeRep()));
  AlwaysAssertExit (aggr);
  TableExprGroupFunc* func = aggr->makeGroupFunc();
  for (uInt i=0; i<recs.size(); ++i) {
    TableExprId id(recs[i]);
    func->apply (*aggr, id);
  }
  func->finish();
  DComplex val = func->getDComplex();
  if (!near (val, expVal, 1.e-10)) {
    foundError = True;
    cout << str << ": found value " << val << "; expected "
         << expVal << endl;
  }
}

void doBool()
{
  // Define a Vector with values.
  // Use odd length (so median behaves fine).
  Vector<Bool> vecb(9);
  vecb = False; vecb[3] = True;  vecb[4] = True;
  // Define two records containing part of the vector.
  // The aggregate functions will evaluate all reocrds, thus full vector.
  for (int i=0; i<2; ++i) {
    vector<Record> recs(2);
    if (i == 0) {
      recs[0].define ("fld", vecb(Slice(0,5)));
      recs[1].define ("fld", vecb(Slice(5,4)));
    } else {
      recs[0].define ("fld", vecb(Slice(0,2)));
      recs[1].define ("fld", vecb(Slice(2,7)));
    }
    // Form the expression node from the record field.
    TableExprNode expr = makeRecordExpr (recs[0], "fld");
    // Create and check aggregation expressions.
    check (TableExprNode::newFunctionNode(TableExprFuncNode::gallFUNC, expr),
           recs, allTrue(vecb), "all");
    check (TableExprNode::newFunctionNode(TableExprFuncNode::ganyFUNC, expr),
           recs, anyTrue(vecb), "any");
    check (TableExprNode::newFunctionNode(TableExprFuncNode::gntrueFUNC, expr),
           recs, Int(ntrue(vecb)), "ntrue");
    check (TableExprNode::newFunctionNode(TableExprFuncNode::gnfalseFUNC, expr),
           recs, Int(nfalse(vecb)), "nfalse");
  }
}

void doInt()
{
  // Define a Vector with values.
  // Use odd length (so median behaves fine).
  Vector<Int> veci(9);
  indgen(veci); veci[1]=-4; veci[6] = 20;
  Vector<Double> vecd(9);
  indgen(vecd); vecd[1]=-4; vecd[6] = 20;
  // Define two records containing part of the vector.
  // The aggregate functions will evaluate all reocrds, thus full vector.
  vector<Record> recs(2);
  recs[0].define ("fld", veci(Slice(0,4)));
  recs[1].define ("fld", veci(Slice(4,5)));
  // Form the expression node from the record field.
  TableExprNode expr = makeRecordExpr (recs[0], "fld");
  // Create and check aggregation expressions.
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gminFUNC, expr),
         recs, min(veci), "minInt");
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gmaxFUNC, expr),
         recs, max(veci), "maxInt");
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gsumFUNC, expr),
         recs, sum(veci), "sumInt");
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gproductFUNC, expr),
         recs, product(veci), "productInt");
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gsumsqrFUNC, expr),
         recs, sum(veci*veci), "sumsqrInt");
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gmeanFUNC, expr),
         recs, mean(vecd), "meanInt");
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gvarianceFUNC, expr),
         recs, variance(vecd), "varianceInt");
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gstddevFUNC, expr),
         recs, stddev(vecd), "stddevInt");
  check (TableExprNode::newFunctionNode(TableExprFuncNode::grmsFUNC, expr),
         recs, rms(vecd), "rmsInt");
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gmedianFUNC, expr),
         recs, median(vecd), "medianInt");
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gfractileFUNC,
                                        expr, 0.3),
         recs, fractile(vecd, 0.3), "fractileInt");
}

void doDouble()
{
  // Define a Vector with values.
  // Use odd length (so median behaves fine).
  Vector<Double> vecd(40);
  indgen(vecd); vecd[1]=-40; vecd[6] = 20;
  Vector<Double> vec2;   //# test empty array
  // Define two records containing part of the vector.
  // The aggregate functions will evaluate all reocrds, thus full vector.
  vector<Record> recs(5);
  recs[0].define ("fld", vecd(Slice(0,13)));
  recs[1].define ("fld", vecd(Slice(13,3)));
  recs[2].define ("fld", vecd(Slice(16,10)));
  recs[3].define ("fld", vec2);
  recs[4].define ("fld", vecd(Slice(26,14)));
  // Form the expression node from the record field.
  TableExprNode expr = makeRecordExpr (recs[0], "fld");
  // Create and check aggregation expressions.
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gminFUNC, expr),
         recs, min(vecd), "minDouble");
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gmaxFUNC, expr),
         recs, max(vecd), "maxDouble");
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gsumFUNC, expr),
         recs, sum(vecd), "sumDouble");
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gproductFUNC, expr),
         recs, product(vecd), "productDouble");
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gsumsqrFUNC, expr),
         recs, sum(vecd*vecd), "sumsqrDouble");
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gmeanFUNC, expr),
         recs, mean(vecd), "meanDouble");
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gvarianceFUNC, expr),
         recs, variance(vecd), "varianceDouble");
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gstddevFUNC, expr),
         recs, stddev(vecd), "stddevDouble");
  check (TableExprNode::newFunctionNode(TableExprFuncNode::grmsFUNC, expr),
         recs, rms(vecd), "rmsDouble");
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gmedianFUNC, expr),
         recs, median(vecd), "medianDouble");
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gfractileFUNC,
                                        expr, 0.65),
         recs, fractile(vecd, 0.65), "fractileDouble");
}

void doDComplex()
{
  // Define a Vector with values.
  // Use odd length (so median behaves fine).
  Vector<DComplex> vecd(40);
  indgen(vecd, DComplex(0.1,0.2), DComplex(-0.015,0.025));
  Vector<DComplex> vec2;   //# test empty array
  // Define two records containing part of the vector.
  // The aggregate functions will evaluate all reocrds, thus full vector.
  vector<Record> recs(5);
  recs[0].define ("fld", vecd(Slice(0,22)));
  recs[1].define ("fld", vecd(Slice(22,3)));
  recs[2].define ("fld", vecd(Slice(25,1)));
  recs[3].define ("fld", vec2);
  recs[4].define ("fld", vecd(Slice(26,14)));
  // Form the expression node from the record field.
  TableExprNode expr = makeRecordExpr (recs[0], "fld");
  // Create and check aggregation expressions.
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gsumFUNC, expr),
         recs, sum(vecd), "sumDComplex");
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gproductFUNC, expr),
         recs, product(vecd), "productDComplex");
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gsumsqrFUNC, expr),
         recs, sum(vecd*vecd), "sumsqrDComplex");
  check (TableExprNode::newFunctionNode(TableExprFuncNode::gmeanFUNC, expr),
         recs, mean(vecd), "meanDComplex");
}


int main()
{
  try {
    doBool();
    doInt();
    doDouble();
    doDComplex();
  } catch (std::exception& x) {
    cout << "Unexpected exception: " << x.what() << endl;
    return 1;
  } catch (...) {
    cout << "Unexpected unknown exception" << endl;
    return 1;
  }
  if (foundError) {
    cout << "Some unexpected results were found" << endl;
    return 1;
  }
  cout << "OK" << endl;
  return 0;
}
