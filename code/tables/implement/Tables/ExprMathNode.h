//# ExprMathNode.h: Nodes representing scalar mathematical operators in table select expression tree
//# Copyright (C) 1994,1995,1996,1997,1999
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

#if !defined(AIPS_EXPRMATHNODE_H)
#define AIPS_EXPRMATHNODE_H

//# Includes
#include <aips/aips.h>
#include <aips/Tables/ExprNodeRep.h>


//# This file defines classes derived from TableExprNode representing
//# the data type and operator in a table expression.
//#
//# Data types Bool, Double, DComplex and String are used.
//# Char, uChar, Short, uShort, Int, uInt and float are converted 
//# to Double, and Complex to DComplex.
//# Binary operators +, -, *, /, ==, >=, >, <, <= and != are recognized.
//# Also &&, ||, parentheses and unary +, - and ! are recognized.



// <summary>
// Addition in table select expression tree
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="">
// </reviewed>

// <prerequisite>
//# Classes you should understand before using this one.
//   <li> TableExprNode
//   <li> TableExprNodeRep
// </prerequisite>

// <synopsis> 
// This class represents an addition in a table select expression tree.
// Strings can also be added (ie. concatenated).
// Numeric data types will be promoted if possible, so for instance
// an addition of Int and Complex is possible.
// </synopsis> 

class TableExprNodePlusDouble : public TableExprNodeBinary
{
public:
    TableExprNodePlusDouble (const TableExprNodeRep&);
    ~TableExprNodePlusDouble();
    Double   getDouble   (uInt rownr);
    DComplex getDComplex (uInt rownr);
};

class TableExprNodePlusDComplex : public TableExprNodeBinary
{
public:
    TableExprNodePlusDComplex (const TableExprNodeRep&);
    ~TableExprNodePlusDComplex();
    DComplex getDComplex (uInt rownr);
};

class TableExprNodePlusString : public TableExprNodeBinary
{
public:
    TableExprNodePlusString (const TableExprNodeRep&);
    ~TableExprNodePlusString();
    String getString (uInt rownr);
};

class TableExprNodePlusDate : public TableExprNodeBinary
{
public:
    TableExprNodePlusDate (const TableExprNodeRep&);
    ~TableExprNodePlusDate();
    Double getDouble (uInt rownr);
    MVTime getDate (uInt rownr);
};


// <summary>
// Subtraction in table select expression tree
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="">
// </reviewed>

// <prerequisite>
//# Classes you should understand before using this one.
//   <li> TableExprNode
//   <li> TableExprNodeRep
// </prerequisite>

// <synopsis> 
// This class represents a subtraction in a table select expression tree.
// Numeric data types will be promoted if possible, so for instance
// a subtraction of Int and Complex is possible.
// </synopsis> 

class TableExprNodeMinusDouble : public TableExprNodeBinary
{
public:
    TableExprNodeMinusDouble (const TableExprNodeRep&);
    ~TableExprNodeMinusDouble();
    Double   getDouble   (uInt rownr);
    DComplex getDComplex (uInt rownr);
};

class TableExprNodeMinusDComplex : public TableExprNodeBinary
{
public:
    TableExprNodeMinusDComplex (const TableExprNodeRep&);
    ~TableExprNodeMinusDComplex();
    DComplex getDComplex (uInt rownr);
};

class TableExprNodeMinusDate : public TableExprNodeBinary
{
public:
    TableExprNodeMinusDate (const TableExprNodeRep&);
    ~TableExprNodeMinusDate();
    MVTime getDate   (uInt rownr);
    Double getDouble (uInt rownr);
};


// <summary>
// Multiplication in table select expression tree
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="">
// </reviewed>

// <prerequisite>
//# Classes you should understand before using this one.
//   <li> TableExprNode
//   <li> TableExprNodeRep
// </prerequisite>

// <synopsis> 
// This class represents a multiplication in a table select expression tree.
// Numeric data types will be promoted if possible, so for instance
// a multiplication of Int and Complex is possible.
// </synopsis> 

class TableExprNodeTimesDouble : public TableExprNodeBinary
{
public:
    TableExprNodeTimesDouble (const TableExprNodeRep&);
    ~TableExprNodeTimesDouble();
    Double   getDouble   (uInt rownr);
    DComplex getDComplex (uInt rownr);
};

class TableExprNodeTimesDComplex : public TableExprNodeBinary
{
public:
    TableExprNodeTimesDComplex (const TableExprNodeRep&);
    ~TableExprNodeTimesDComplex();
    DComplex getDComplex (uInt rownr);
};


// <summary>
// Division in table select expression tree
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="">
// </reviewed>

// <prerequisite>
//# Classes you should understand before using this one.
//   <li> TableExprNode
//   <li> TableExprNodeRep
// </prerequisite>

// <synopsis> 
// This class represents a division in a table select expression tree.
// Numeric data types will be promoted if possible, so for instance
// a division of Int and Complex is possible.
// </synopsis> 

class TableExprNodeDivideDouble : public TableExprNodeBinary
{
public:
    TableExprNodeDivideDouble (const TableExprNodeRep&);
    ~TableExprNodeDivideDouble();
    Double   getDouble   (uInt rownr);
    DComplex getDComplex (uInt rownr);
};

class TableExprNodeDivideDComplex : public TableExprNodeBinary
{
public:
    TableExprNodeDivideDComplex (const TableExprNodeRep&);
    ~TableExprNodeDivideDComplex();
    DComplex getDComplex (uInt rownr);
};


// <summary>
// Modulo in table select expression tree
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="">
// </reviewed>

// <prerequisite>
//# Classes you should understand before using this one.
//   <li> TableExprNode
//   <li> TableExprNodeRep
// </prerequisite>

// <synopsis> 
// This class represents a modulo operation in a table select expression tree.
// It is only possible for datatype Double.
// </synopsis> 

class TableExprNodeModuloDouble : public TableExprNodeBinary
{
public:
    TableExprNodeModuloDouble (const TableExprNodeRep&);
    ~TableExprNodeModuloDouble();
    Double   getDouble   (uInt rownr);
    DComplex getDComplex (uInt rownr);
};



// <summary>
// Unary minus in table select expression tree
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="">
// </reviewed>

// <prerequisite>
//# Classes you should understand before using this one.
//   <li> TableExprNode
// </prerequisite>

// <synopsis> 
// This class represents a unary minus in a table select expression tree.
// This is defined for numeric data types only.
// </synopsis> 

class TableExprNodeMIN : public TableExprNodeBinary
{
public:
    TableExprNodeMIN (const TableExprNodeRep&);
    ~TableExprNodeMIN();
    Double   getDouble   (uInt rownr);
    DComplex getDComplex (uInt rownr);
};



#endif
