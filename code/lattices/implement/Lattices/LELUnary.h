//# LELUnary.h:  LELUnary.h
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

#if !defined(AIPS_LELUNARY_H)
#define AIPS_LELUNARY_H


//# Includes
#include <trial/Lattices/LELInterface.h>
#include <trial/Lattices/LELUnaryEnums.h>

//# Forward Declarations
template <class T> class Array;
class PixelRegion;


// <summary> This LEL class handles scalar (unary) constants </summary>
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
//   <li> <linkto class="LELUnaryEnums"> LELUnaryEnums</linkto>
// </prerequisite>
//
// <etymology>
//  This derived LEL letter class handles scalar (unary) constants
// </etymology>
//
// <synopsis>
// This LEL letter class is derived from LELInterface.  It
// is used to construct LEL objects that represent scalars
// constants.   They can be of type Float,Double,Complex,DComplex
// and Bool.  
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
// ArrayLattice<Float> y(shape);
// ArrayLattice<Float> z(shape); 
// y.copyData(x+2.0);                 // y = x + 2.0
// z.copyData(True);                  // z = True
// </srcblock>
// </example>
//
// <motivation>
// Constants are a basic mathematical expression. 
// </motivation>
//
// <todo asof="1998/01/20">
// </todo>
 

template <class T> class LELUnaryConst : public LELInterface<T>
{
public: 
   
// Constructor takes a scalar.  
   LELUnaryConst(const T val);

// Destructor does nothing
  ~LELUnaryConst();

// Evaluate the expression.
// This throws an exception, since only a scalar can be returned.
   virtual void eval (Array<T>& result,
                      const PixelRegion& region) const;

// Evaluate the scalar expression (get the constant)
   virtual T getScalar() const;

// Do further preparations (e.g. optimization) on the expression.
   virtual void prepare();

// Get class name
   virtual String className() const;    

private:
   T val_p;
};



// <summary> This LEL class handles numerical unary operators </summary>
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
//   <li> <linkto class="LELUnaryEnums"> LELUnaryEnums</linkto>
// </prerequisite>
//
// <etymology>
//  This derived LEL letter class handles numerical unary 
//  operators 
// </etymology>
//
// <synopsis>
// This LEL letter class is derived from LELInterface.  It
// is used to construct LEL objects that apply numerical unary
// operators to Lattice expressions.  They operate on numerical
// Lattice (Float,Double,Complex,DComplex) expressions and return the 
// same numerical type. The available C++ operators  
// are  <src>+,-</src> with  equivalents in the enum 
// of PLUS and MINUS.
//
// A description of the implementation details of the LEL classes can
// be found in <a href="../../../notes/216/216.html">Note 216</a>
//
// </synopsis> 
//
// <example>
// Examples are not very useful as the user would never use 
// these classes directly.  Look in LatticeExprNode.cc to see 
// how it invokes these classes.  An example of how the user
// would indirectly use this class (through the envelope) is:
// <srcblock>
// IPosition shape(2,5,10);
// ArrayLattice<Float> x(shape); x.set(1.0);
// ArrayLattice<Float> y(shape); 
// y.copyData(-x);                 // y = -x
// </srcblock>
// </example>
//
// <motivation>
// Numerical unary operations are a basic mathematical expression. 
// </motivation>
//
// <todo asof="1998/01/20">
// </todo>
 
template <class T> class LELUnary : public LELInterface<T>
{
public: 
   
// Constructor takes operation and expression
// to be operated upon
   LELUnary(const LELUnaryEnums::Operation op, 
	    const CountedPtr<LELInterface<T> >& pExpr);

// Destructor does nothing
  ~LELUnary();

// Recursively evaluate the expression.
   virtual void eval (Array<T>& result,
                      const PixelRegion& region) const;

// Recursively evaluate the scalar expression.
   virtual T getScalar() const;

// Do further preparations (e.g. optimization) on the expression.
   virtual void prepare();

// Get class name
   virtual String className() const;    

private:
   LELUnaryEnums::Operation op_p;
   CountedPtr<LELInterface<T> > pExpr_p;
};




// <summary> This LEL class handles logical unary operators </summary>
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
//   <li> <linkto class="LELUnaryEnums"> LELUnaryEnums</linkto>
// </prerequisite>
//
// <etymology>
//  This derived LEL letter class handles logical unary 
//  operators 
// </etymology>
//
// <synopsis>
// This LEL letter class is derived from LELInterface.  It
// is used to construct LEL objects that apply logical unary
// operators to Lattice expressions.  They operate on Bool
// Lattice expressions only and return a Bool.
// The available C++ operator is <src>!</src> with  the equivalent
// in the enum  of NOT.
//
// A description of the implementation details of the LEL classes can
// be found in <a href="../../../notes/216/216.html">Note 216</a>
//
// </synopsis> 
//
// <example>
// Examples are not very useful as the user would never use 
// these classes directly.  Look in LatticeExprNode.cc to see 
// how it invokes these classes.  An example of how the user
// would indirectly use this class (through the envelope) is:
// <srcblock>
// IPosition shape(2,5,10);
// ArrayLattice<Bool> x(shape); x.set(True);
// ArrayLattice<Bool> y(shape); 
// y.copyData(!x);                 // y = !x
// </srcblock>
// </example>
//
// <motivation>
// Logical unary operations are a basic mathematical expression. 
// </motivation>
//
// <todo asof="1998/01/20">
// </todo>


class LELUnaryBool : public LELInterface<Bool>
{
public: 
   
// Constructor takes operation and expression
// to be operated upon
   LELUnaryBool(const LELUnaryEnums::Operation op, 
		const CountedPtr<LELInterface<Bool> >& pExpr);

// Destructor does nothing
  ~LELUnaryBool();

// Recursively evaluate the expression.
   virtual void eval (Array<Bool>& result,
                      const PixelRegion& region) const;

// Recursively evaluate the scalar expression.
   virtual Bool getScalar() const;

// Do further preparations (e.g. optimization) on the expression.
   virtual void prepare();

// Get class name
   virtual String className() const;    

private:
   LELUnaryEnums::Operation op_p;
   CountedPtr<LELInterface<Bool> > pExpr_p;
};



#endif
