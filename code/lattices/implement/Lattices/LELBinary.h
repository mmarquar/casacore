//# LELBinary.h:  LELBinary.h
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

#if !defined(AIPS_LELBINARY_H)
#define AIPS_LELBINARY_H


//# Includes
#include <trial/Lattices/LELInterface.h>
#include <trial/Lattices/LELBinaryEnums.h>

//# Forward Declarations
template <class T> class Array;
class PixelRegion;


// <summary> This LEL class handles numerical binary operators </summary>
//
// <use visibility=local>
//
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
//
// <prerequisite>
//   <li> <linkto class="Lattice"> Lattice</linkto>
//   <li> <linkto class="LatticeExpr"> LatticeExpr</linkto>
//   <li> <linkto class="LatticeExprNode"> LatticeExprNode</linkto>
//   <li> <linkto class="LELInterface"> LELInterface</linkto>
//   <li> <linkto class="LELBinaryEnums"> LELBinaryEnums</linkto>
// </prerequisite>
//
// <etymology>
//  This derived LEL letter class handles numerical binary 
//  operators 
// </etymology>
//
// <synopsis>
// This LEL letter class is derived from LELInterface.  It
// is used to construct LEL objects that apply numerical binary
// operators to Lattice expressions.  They operate on numerical
// Lattices and return a numerical Lattice. The
// available C++ operators  are  <src>+,-,*,/</src> with 
// equivalents in the enum of ADD, SUBTRACT, MULTIPLY, and DIVIDE.
//
// A description of the implementation details of the LEL classes can
// be found in <a href="../../../notes/216/216.html">Note 216</a>
//
// </synopsis> 
//
// <example>
// Examples are not very useful as the user would never use 
// these classes directly.  Look in LatticeExprNode.cc to see 
// how it invokes these classes.  Examples of how the user
// would indirectly use this class (through the envelope) are:
// <srcblock>
// IPosition shape(2,5,10);
// ArrayLattice<Float> x(shape); x.set(1.0);
// ArrayLattice<Float> y(shape); y.set(2.0);
// ArrayLattice<Float> z(shape); 
// z.copyData(x+y);                 // z = x + y;
// z.copyData(x-y);                 // z = x - y;
// z.copyData(x*y);                 // z = x * y;
// z.copyData(x/y);                 // z = x / y;
// </srcblock>
// </example>
//
// <motivation>
// Numerical binary operations are a basic mathematical expression. 
// </motivation>
//
// <todo asof="1998/01/20">
// </todo>
 

template <class T> class LELBinary : public LELInterface<T>
{
public: 
   
// Constructor takes operation and left and right expressions
// to be operated upon
   LELBinary(const LELBinaryEnums::Operation op, 
	     const CountedPtr<LELInterface<T> >& pLeftExpr,
	     const CountedPtr<LELInterface<T> >& pRightExpr);

// Destructor 
  ~LELBinary();

// Recursively evaluate the expression 
   virtual void eval (Array<T>& result,
                      const PixelRegion& region) const;

// Recursively efvaluate the scalar expression 
   virtual T getScalar() const;

// Do further preparations (e.g. optimization) on the expression.
   virtual void prepare();

// Get class name
   virtual String className() const;    

private:
   LELBinaryEnums::Operation op_p;
   CountedPtr<LELInterface<T> > pLeftExpr_p;
   CountedPtr<LELInterface<T> > pRightExpr_p;
};




// <summary> This LEL class handles relational binary numerical operators </summary>
//
// <use visibility=local>
//
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
//
// <prerequisite>
//   <li> <linkto class="Lattice"> Lattice</linkto>
//   <li> <linkto class="LatticeExpr"> LatticeExpr</linkto>
//   <li> <linkto class="LatticeExprNode"> LatticeExprNode</linkto>
//   <li> <linkto class="LELInterface"> LELInterface</linkto>
//   <li> <linkto class="LELBinaryEnums"> LELBinaryEnums</linkto>
// </prerequisite>
//
// <etymology>
//  This derived LEL letter class handles relational numerical binary 
//  operators 
// </etymology>
//
// <synopsis>
// This LEL letter class is derived from LELInterface.  It
// is used to construct LEL objects that apply relational numerical 
// binary operators to Lattice expressions.  They operate on numerical
// Lattices and result in a Bool Lattice.  The available C++ operators
// are  <src>==,!=>,>=,<,<=,</src> with equivalents in the enum of 
// EQ, NE, GT, GE, LT, and LE
//
// A description of the implementation details of the LEL classes can
// be found in <a href="../../../notes/216/216.html">Note 216</a>
//
// </synopsis> 
//
// <example>
// Examples are not very useful as the user would never use 
// these classes directly.  Look in LatticeExprNode.cc to see 
// how it invokes these classes.  Examples of how the user
// would indirectly use this class (through the envelope) are:
// <srcblock>
// IPosition shape(2,5,10);
// ArrayLattice<Float> x(shape); x.set(0.0);
// ArrayLattice<Float> y(shape); y.set(1.0);
// ArrayLattice<Bool> z(shape); z.set(False);
// z.copyData(x==y);                // z = x == y;
// z.copyData(x!=y);                // z = x != y;
// z.copyData(x>y);                 // z = x > y;
// z.copyData(x>=y);                // z = x >= y;
// z.copyData(x<y);                 // z = x < y;
// z.copyData(x<=y);                // z = x <= y;
// </srcblock>
// </example>
//
// <motivation>
// Numerical relational binary operations are a basic mathematical expression. 
// </motivation>
//
// <todo asof="1998/01/20">
// </todo>
 

template<class T> class LELBinaryCmp : public LELInterface<Bool>
{
public: 
   
// Constructor takes operation and left and right expressions
// to be operated upon. It can only handle the comparison operators.
   LELBinaryCmp(const LELBinaryEnums::Operation op, 
		const CountedPtr<LELInterface<T> >& pLeftExpr,
		const CountedPtr<LELInterface<T> >& pRightExpr);

// Destructor 
  ~LELBinaryCmp();

// Recursively evaluate the expression 
   virtual void eval (Array<Bool>& result,
                      const PixelRegion& region) const;

// Recursively evaluate the scalar expression 
   virtual Bool getScalar() const;

// Do further preparations (e.g. optimization) on the expression.
   virtual void prepare();

// Get class name
   virtual String className() const;    

private:
   LELBinaryEnums::Operation op_p;
   CountedPtr<LELInterface<T> > pLeftExpr_p;
   CountedPtr<LELInterface<T> > pRightExpr_p;
};




// <summary> This LEL class handles logical binary operators </summary>
//
// <use visibility=local>
//
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
//
// <prerequisite>
//   <li> <linkto class="Lattice"> Lattice</linkto>
//   <li> <linkto class="LatticeExpr"> LatticeExpr</linkto>
//   <li> <linkto class="LatticeExprNode"> LatticeExprNode</linkto>
//   <li> <linkto class="LELInterface"> LELInterface</linkto>
//   <li> <linkto class="LELBinaryEnums"> LELBinaryEnums</linkto>
// </prerequisite>

// <etymology>
//  This derived LEL letter class handles logical binary operators 
// </etymology>
//
// <synopsis>
// This LEL letter class is derived from LELInterface.  It
// is used to construct LEL objects that apply logical 
// binary operators to Lattice expressions.  They apply only
// to Boolean Lattices and result in Boolean Lattices.  The 
// available C++ operators are  <src>&&,||,==,!=</src> with 
// equivalents in the enum of  AND, OR, EQ, and NE
//
// A description of the implementation details of the LEL classes can
// be found in <a href="../../../notes/216/216.html">Note 216</a>
//
// </synopsis> 
//
// <example>
// Examples are not very useful as the user would never use 
// these classes directly.  Look in LatticeExprNode.cc to see 
// how it invokes these classes.  Examples of how the user
// would indirectly use this class (through the envelope) are:
// <srcblock>
// IPosition shape(2,5,10);
// ArrayLattice<Bool> x(shape); x.set(False);
// ArrayLattice<Bool> y(shape); y.set(True);
// ArrayLattice<Bool> z(shape); z.set(False);
// z.copyData(x&&y);                // z = x && y;
// z.copyData(x||y);                // z = x || y;
// z.copyData(x==y);                // z = x == y;
// z.copyData(x!=y);                // z = x != y;
// </srcblock>
// </example>
//
// <motivation>
// Logical binary operations are a basic mathematical expression. 
// </motivation>
//
// <todo asof="1998/01/20">
// </todo>
 

class LELBinaryBool : public LELInterface<Bool>
{
public: 
   
// Constructor takes operation and left and right expressions
// to be operated upon.
   LELBinaryBool(const LELBinaryEnums::Operation op, 
		 const CountedPtr<LELInterface<Bool> >& pLeftExpr,
		 const CountedPtr<LELInterface<Bool> >& pRightExpr);

// Destructor 
  ~LELBinaryBool();

// Recursively evaluate the expression 
   virtual void eval (Array<Bool>& result,
                      const PixelRegion& region) const;

// Recursively evaluate the scalar expression 
   virtual Bool getScalar() const;

// Do further preparations (e.g. optimization) on the expression.
   virtual void prepare();

// Get class name
   virtual String className() const;    

private:
   LELBinaryEnums::Operation op_p;
   CountedPtr<LELInterface<Bool> > pLeftExpr_p;
   CountedPtr<LELInterface<Bool> > pRightExpr_p;
};



#endif
