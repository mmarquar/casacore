//# MSArrayParse.cc: Classes to hold results from scan grammar parser
//# Copyright (C) 1994,1995,1997,1998,1999,2000,2001,2003
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

#include <ms/MeasurementSets/MSArrayParse.h>
#include <ms/MeasurementSets/MSSelectionError.h>
#include <ms/MeasurementSets/MSMainColumns.h>

namespace casa { //# NAMESPACE CASA - BEGIN

  MSArrayParse* MSArrayParse::thisMSSParser = 0x0; // Global pointer to the parser object
  TableExprNode* MSArrayParse::node_p = 0x0;
  Vector<Int> MSArrayParse::idList;
  
  //# Constructor
  MSArrayParse::MSArrayParse ()
    : MSParse(), colName(MS::columnName(MS::ARRAY_ID)), maxArrays_p(1000)
  {
  }
  
  //# Constructor with given ms name.
  MSArrayParse::MSArrayParse (const MeasurementSet* ms)
    : MSParse(ms, "Array"), colName(MS::columnName(MS::ARRAY_ID)), maxArrays_p(1000)
  {
    if(node_p) delete node_p;
    node_p = new TableExprNode();
    idList.resize(0);
  }
  
  const void MSArrayParse::appendToIDList(const Vector<Int>& v)
  {
    Int currentSize = idList.nelements();
    Int n = v.nelements() + currentSize;
    Int j=0;

    idList.resize(n, True);
    for(Int i=currentSize;i<n;i++) idList[i] = v[j++];
  }

  const TableExprNode *MSArrayParse::selectRangeGTAndLT(const Int& n0,const Int& n1)
  {
    TableExprNode condition = TableExprNode( (ms()->col(colName) > n0) &&
					     (ms()->col(colName) < n1));
    if ((n0 < 0) || (n1 < 0) || (n1 <= n0))
      {
	ostringstream os;
	os << "Array Expression: Malformed range bounds " << n0 << " (lower bound) and " << n1 << " (upper bound)";
	throw(MSSelectionArrayParseError(os.str()));
      }
    Vector<Int> tmp(n1-n0-1);
    Int j=n0+1;
    for(uInt i=0;i<tmp.nelements();i++) tmp[i]=j++;
    appendToIDList(tmp);

    if (node_p->isNull())
      *node_p = condition;
    else
      *node_p = *node_p || condition;
    
    return node_p;
  }
  
  const TableExprNode *MSArrayParse::selectRangeGEAndLE(const Int& n0,const Int& n1)
  {
    TableExprNode condition = TableExprNode( (ms()->col(colName) >= n0) &&
					     (ms()->col(colName) <= n1));
    if ((n0 < 0) || (n1 < 0) || (n1 <= n0))
      {
	ostringstream os;
	os << "Array Expression: Malformed range bounds " << n0 << " (lower bound) and " << n1 << " (upper bound)";
	throw(MSSelectionArrayParseError(os.str()));
      }
    Vector<Int> tmp(n1-n0+1);
    Int j=n0;
    for(uInt i=0;i<tmp.nelements();i++) tmp[i]=j++;
    appendToIDList(tmp);

    if (node_p->isNull())
      *node_p = condition;
    else
      *node_p = *node_p || condition;
    
    return node_p;
  }
  
  const TableExprNode *MSArrayParse::selectArrayIds(const Vector<Int>& scanids)
  {
    TableExprNode condition = TableExprNode(ms()->col(colName).in(scanids));
    
    appendToIDList(scanids);

    if(node_p->isNull())
      *node_p = condition;
    else
      *node_p = *node_p || condition;
    
    return node_p;
  }
  
  const TableExprNode *MSArrayParse::selectArrayIdsGT(const Vector<Int>& scanids)
  {
    TableExprNode condition = TableExprNode(ms()->col(colName) > scanids[0]);
    
    Int n=maxArrays_p-scanids[0]+1,j;
    Vector<Int> tmp(n);
    j=scanids[0]+1;
    for(Int i=0;i<n;i++) tmp[i]=j++;
    appendToIDList(tmp);

    if(node_p->isNull())
      *node_p = condition;
    else
      *node_p = *node_p || condition;
    
    return node_p;
  }
  
  const TableExprNode *MSArrayParse::selectArrayIdsLT(const Vector<Int>& scanids)
  {
    TableExprNode condition = TableExprNode(ms()->col(colName) < scanids[0]);
    Vector<Int> tmp(scanids[0]);
    for(Int i=0;i<scanids[0];i++) tmp[i] = i;
    appendToIDList(tmp);

    if(node_p->isNull())
      *node_p = condition;
    else
      *node_p = *node_p || condition;
    
    return node_p;
  }

  const TableExprNode *MSArrayParse::selectArrayIdsGTEQ(const Vector<Int>& scanids)
  {
    TableExprNode condition = TableExprNode(ms()->col(colName) >= scanids[0]);
    
    Int n=maxArrays_p-scanids[0]+1,j;
    Vector<Int> tmp(n);
    j=scanids[0];
    for(Int i=0;i<n;i++) tmp[i]=j++;
    appendToIDList(tmp);

    if(node_p->isNull())
      *node_p = condition;
    else
      *node_p = *node_p || condition;
    
    return node_p;
  }
  
  const TableExprNode *MSArrayParse::selectArrayIdsLTEQ(const Vector<Int>& scanids)
  {
    TableExprNode condition = TableExprNode(ms()->col(colName) <= scanids[0]);
    Vector<Int> tmp(scanids[0]+1);
    for(Int i=0;i<=scanids[0];i++) tmp[i] = i;
    appendToIDList(tmp);

    if(node_p->isNull())
      *node_p = condition;
    else
      *node_p = *node_p || condition;
    
    return node_p;
  }
  
  const TableExprNode* MSArrayParse::node()
  {
    return node_p;
  }
  
} //# NAMESPACE CASA - END
