//# LatticeExprNode.cc:  this defines LatticeExprNode.cc
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

#include <trial/Lattices/LatticeExprNode.h>
#include <trial/Lattices/LatticeExpr.h>
#include <trial/Lattices/Lattice.h>
#include <trial/Lattices/PixelRegion.h>
#include <trial/Lattices/LELLattice.h>
#include <trial/Lattices/LELConvert.h>
#include <trial/Lattices/LELBinary.h>
#include <trial/Lattices/LELUnary.h>
#include <trial/Lattices/LELFunction.h>
#include <aips/Arrays/Array.h>
#include <aips/Containers/Block.h>
#include <aips/Utilities/COWPtr.h>
#include <aips/Utilities/CountedPtr.h>
#include <aips/Utilities/Assert.h>
#include <aips/Exceptions/Error.h> 
#include <iostream.h>



// Default constructor
LatticeExprNode::LatticeExprNode()
: donePrepare_p (False)
{
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode::default constructor; pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}

// Destructor
LatticeExprNode::~LatticeExprNode()
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode::destructor; pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}


LatticeExprNode::LatticeExprNode(const CountedPtr<LELInterface<Float> >& pExpr)
: donePrepare_p (False),
  dtype_p       (TpFloat),
  pAttr_p       (&pExpr->getAttribute()),
  pExprFloat_p  (pExpr)
{
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: constructor (CountedPtr<LELInterface<T>>&); pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}

LatticeExprNode::LatticeExprNode(const CountedPtr<LELInterface<Double> >& pExpr)
: donePrepare_p (False),
  dtype_p       (TpDouble),
  pAttr_p       (&pExpr->getAttribute()),
  pExprDouble_p (pExpr)
{
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: constructor (CountedPtr<LELInterface<T>>&); pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}

LatticeExprNode::LatticeExprNode
                             (const CountedPtr<LELInterface<Complex> >& pExpr)
: donePrepare_p  (False),
  dtype_p        (TpComplex),
  pAttr_p        (&pExpr->getAttribute()),
  pExprComplex_p (pExpr)
{
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: constructor (CountedPtr<LELInterface<T>>&); pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}

LatticeExprNode::LatticeExprNode
                             (const CountedPtr<LELInterface<DComplex> >& pExpr)
: donePrepare_p   (False),
  dtype_p         (TpDComplex),
  pAttr_p         (&pExpr->getAttribute()),
  pExprDComplex_p (pExpr)
{
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: constructor (CountedPtr<LELInterface<T>>&); pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}

LatticeExprNode::LatticeExprNode(const CountedPtr<LELInterface<Bool> >& pExpr)
: donePrepare_p (False),
  dtype_p       (TpBool),
  pAttr_p       (&pExpr->getAttribute()),
  pExprBool_p   (pExpr)
{
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: constructor (CountedPtr<LELInterface<T>>&); pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}


LatticeExprNode::LatticeExprNode(LELInterface<Float>* pExpr)
: donePrepare_p (False),
  dtype_p       (TpFloat),
  pAttr_p       (&pExpr->getAttribute()),
  pExprFloat_p  (pExpr)
{
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: constructor (LELInterface<T>*); pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}

LatticeExprNode::LatticeExprNode(LELInterface<Double>* pExpr)
: donePrepare_p (False),
  dtype_p       (TpDouble),
  pAttr_p       (&pExpr->getAttribute()),
  pExprDouble_p (pExpr)
{
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: constructor (LELInterface<T>*); pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}

LatticeExprNode::LatticeExprNode(LELInterface<Complex>* pExpr)
: donePrepare_p  (False),
  dtype_p        (TpComplex),
  pAttr_p        (&pExpr->getAttribute()),
  pExprComplex_p (pExpr)
{
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: constructor (LELInterface<T>*); pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}

LatticeExprNode::LatticeExprNode(LELInterface<DComplex>* pExpr)
: donePrepare_p   (False),
  dtype_p         (TpDComplex),
  pAttr_p         (&pExpr->getAttribute()),
  pExprDComplex_p (pExpr)
{
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: constructor (LELInterface<T>*); pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}

LatticeExprNode::LatticeExprNode(LELInterface<Bool>* pExpr)
: donePrepare_p (False),
  dtype_p       (TpBool),
  pAttr_p       (&pExpr->getAttribute()),
  pExprBool_p   (pExpr)
{
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: constructor (LELInterface<T>*); pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}


LatticeExprNode::LatticeExprNode (Int constant) 
: donePrepare_p (False),
  dtype_p       (TpFloat),
  pExprFloat_p  (new LELUnaryConst<Float> (constant))
{ 
   pAttr_p = &pExprFloat_p->getAttribute();

#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: Unary constructor (T); pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}

LatticeExprNode::LatticeExprNode (Float constant) 
: donePrepare_p (False),
  dtype_p       (TpFloat),
  pExprFloat_p  (new LELUnaryConst<Float> (constant))
{ 
   pAttr_p = &pExprFloat_p->getAttribute();

#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: Unary constructor (T); pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}

LatticeExprNode::LatticeExprNode (Double constant) 
: donePrepare_p (False),
  dtype_p       (TpDouble),
  pExprDouble_p (new LELUnaryConst<Double> (constant))
{ 
   pAttr_p = &pExprDouble_p->getAttribute();

#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: Unary constructor (T); pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}

LatticeExprNode::LatticeExprNode (const Complex& constant) 
: donePrepare_p  (False),
  dtype_p        (TpComplex),
  pExprComplex_p (new LELUnaryConst<Complex> (constant))
{ 
   pAttr_p = &pExprComplex_p->getAttribute();

#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: Unary constructor (T); pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}

LatticeExprNode::LatticeExprNode (const DComplex& constant) 
: donePrepare_p   (False),
  dtype_p         (TpDComplex),
  pExprDComplex_p (new LELUnaryConst<DComplex> (constant))
{ 
   pAttr_p = &pExprDComplex_p->getAttribute();

#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: Unary constructor (T); pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}

LatticeExprNode::LatticeExprNode (Bool constant) 
: donePrepare_p (False),
  dtype_p       (TpBool),
  pExprBool_p   (new LELUnaryConst<Bool> (constant))
{ 
   pAttr_p = &pExprBool_p->getAttribute();

#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: Unary constructor (T); pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}


LatticeExprNode::LatticeExprNode (const Lattice<Float>& lattice) 
: donePrepare_p (False),
  dtype_p       (TpFloat),
  pExprFloat_p  (new LELLattice<Float> (lattice))
{
   pAttr_p = &pExprFloat_p->getAttribute();

#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: constructor (const Lattice<T>>&); pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}

LatticeExprNode::LatticeExprNode (const Lattice<Double>& lattice) 
: donePrepare_p (False),
  dtype_p       (TpDouble),
  pExprDouble_p (new LELLattice<Double> (lattice))
{
   pAttr_p = &pExprDouble_p->getAttribute();

#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: constructor (const Lattice<T>>&); pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}

LatticeExprNode::LatticeExprNode (const Lattice<Complex>& lattice) 
: donePrepare_p  (False),
  dtype_p        (TpComplex),
  pExprComplex_p (new LELLattice<Complex> (lattice))
{
   pAttr_p = &pExprComplex_p->getAttribute();

#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: constructor (const Lattice<T>>&); pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}

LatticeExprNode::LatticeExprNode (const Lattice<DComplex>& lattice) 
: donePrepare_p   (False),
  dtype_p         (TpDComplex),
  pExprDComplex_p (new LELLattice<DComplex> (lattice))
{
   pAttr_p = &pExprDComplex_p->getAttribute();

#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: constructor (const Lattice<T>>&); pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}

LatticeExprNode::LatticeExprNode (const Lattice<Bool>& lattice) 
: donePrepare_p (False),
  dtype_p       (TpBool),
  pExprBool_p   (new LELLattice<Bool> (lattice))
{
   pAttr_p = &pExprBool_p->getAttribute();

#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: constructor (Lattice<T>); pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}


LatticeExprNode::LatticeExprNode(const LatticeExprNode& other)
: donePrepare_p   (other.donePrepare_p),
  dtype_p         (other.dtype_p),
  pAttr_p         (other.pAttr_p),
  pExprFloat_p    (other.pExprFloat_p),
  pExprDouble_p   (other.pExprDouble_p),
  pExprComplex_p  (other.pExprComplex_p),
  pExprDComplex_p (other.pExprDComplex_p),
  pExprBool_p     (other.pExprBool_p)
{
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: copy constructor (LatticeExprNode); pExpr_p.nrefs() = "
	<< pExprDouble_p.nrefs() << endl;
#endif
}

// Assignment operator  
LatticeExprNode& LatticeExprNode::operator=(const LatticeExprNode& other)
{
   if (this != &other) {
      donePrepare_p   = other.donePrepare_p;
      dtype_p         = other.dtype_p;
      pAttr_p         = other.pAttr_p;
      pExprFloat_p    = other.pExprFloat_p;
      pExprDouble_p   = other.pExprDouble_p;
      pExprComplex_p  = other.pExprComplex_p;
      pExprDComplex_p = other.pExprDComplex_p;
      pExprBool_p     = other.pExprBool_p;
   }
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: assignment operator (LatticeExprNode&); pExpr_p.nrefs() = " 
	<< pExprDouble_p.nrefs() << endl;
#endif
   return *this;
}


void LatticeExprNode::replaceScalarExpr()
{
   switch (dataType()) {
   case TpFloat:
      LELInterface<Float>::replaceScalarExpr (pExprFloat_p);
      pAttr_p = &pExprFloat_p->getAttribute();
      break;
   case TpDouble:
      LELInterface<Double>::replaceScalarExpr (pExprDouble_p);
      pAttr_p = &pExprDouble_p->getAttribute();
      break;
   case TpComplex:
      LELInterface<Complex>::replaceScalarExpr (pExprComplex_p);
      pAttr_p = &pExprComplex_p->getAttribute();
      break;
   case TpDComplex:
      LELInterface<DComplex>::replaceScalarExpr (pExprDComplex_p);
      pAttr_p = &pExprDComplex_p->getAttribute();
      break;
   case TpBool:
      LELInterface<Bool>::replaceScalarExpr (pExprBool_p);
      pAttr_p = &pExprBool_p->getAttribute();
      break;
   default:
      throw (AipsError ("LatticeExpr::replaceScalarExpr - unknown data type"));
   }
}


void LatticeExprNode::eval (Array<Float>& result,
			    const PixelRegion& region) const
{
   DebugAssert (dataType() == TpFloat, AipsError);
   if (!donePrepare_p) {

// If first time, try to do optimization.

      LatticeExprNode* This = (LatticeExprNode*)this;
      LELInterface<Float>::replaceScalarExpr (This->pExprFloat_p);
      This->donePrepare_p = True;
   }
   if (isScalar()) {
      result = pExprFloat_p->getScalar();
   } else {
      Array<Float> temp(result.shape());
      pExprFloat_p->eval(temp, region);
      result = temp;
   }
}

void LatticeExprNode::eval (Array<Double>& result,
			    const PixelRegion& region) const
{
   DebugAssert (dataType() == TpDouble, AipsError);
   if (!donePrepare_p) {

// If first time, try to do optimization.

      LatticeExprNode* This = (LatticeExprNode*)this;
      LELInterface<Double>::replaceScalarExpr (This->pExprDouble_p);
      This->donePrepare_p = True;
   }
   if (isScalar()) {
      result = pExprDouble_p->getScalar();
   } else {
      Array<Double> temp(result.shape());
      pExprDouble_p->eval(temp, region);
      result = temp;
   }
}

void LatticeExprNode::eval (Array<Complex>& result,
			    const PixelRegion& region) const
{
   DebugAssert (dataType() == TpComplex, AipsError);
   if (!donePrepare_p) {

// If first time, try to do optimization.

      LatticeExprNode* This = (LatticeExprNode*)this;
      LELInterface<Complex>::replaceScalarExpr (This->pExprComplex_p);
      This->donePrepare_p = True;
   }
   if (isScalar()) {
      result = pExprComplex_p->getScalar();
   } else {
      Array<Complex> temp(result.shape());
      pExprComplex_p->eval(temp, region);
      result = temp;
   }
}

void LatticeExprNode::eval (Array<DComplex>& result,
			    const PixelRegion& region) const
{
   DebugAssert (dataType() == TpDComplex, AipsError);
   if (!donePrepare_p) {

// If first time, try to do optimization.

      LatticeExprNode* This = (LatticeExprNode*)this;
      LELInterface<DComplex>::replaceScalarExpr (This->pExprDComplex_p);
      This->donePrepare_p = True;
   }
   if (isScalar()) {
      result = pExprDComplex_p->getScalar();
   } else {
      Array<DComplex> temp(result.shape());
      pExprDComplex_p->eval(temp, region);
      result = temp;
   }
}

void LatticeExprNode::eval (Array<Bool>& result,
			    const PixelRegion& region) const
{
   DebugAssert (dataType() == TpBool, AipsError);
   if (!donePrepare_p) {

// If first time, try to do optimization.

      LatticeExprNode* This = (LatticeExprNode*)this;
      LELInterface<Bool>::replaceScalarExpr (This->pExprBool_p);
      This->donePrepare_p = True;
   }
   if (isScalar()) {
      result = pExprBool_p->getScalar();
   } else {
      Array<Bool> temp(result.shape());
      pExprBool_p->eval(temp, region);
      result = temp;
   }
}


void LatticeExprNode::eval (Float& result) const
{
   DebugAssert (dataType() == TpFloat, AipsError);
   result = pExprFloat_p->getScalar();
}

void LatticeExprNode::eval (Double& result) const
{
   DebugAssert (dataType() == TpDouble, AipsError);
   result = pExprDouble_p->getScalar();
}

void LatticeExprNode::eval (Complex& result) const
{
   DebugAssert (dataType() == TpComplex, AipsError);
   result = pExprComplex_p->getScalar();
}

void LatticeExprNode::eval (DComplex& result) const
{
   DebugAssert (dataType() == TpDComplex, AipsError);
   result = pExprDComplex_p->getScalar();
}

void LatticeExprNode::eval (Bool& result) const
{
   DebugAssert (dataType() == TpBool, AipsError);
   result = pExprBool_p->getScalar();
}

Float LatticeExprNode::getFloat() const
{
   DebugAssert (dataType() == TpFloat, AipsError);
   return pExprFloat_p->getScalar();
}

Double LatticeExprNode::getDouble() const
{
   DebugAssert (dataType() == TpDouble, AipsError);
   return pExprDouble_p->getScalar();
}

Complex LatticeExprNode::getComplex() const
{
   DebugAssert (dataType() == TpComplex, AipsError);
   return pExprComplex_p->getScalar();
}

DComplex LatticeExprNode::getDComplex() const
{
   DebugAssert (dataType() == TpDComplex, AipsError);
   return pExprDComplex_p->getScalar();
}

Bool LatticeExprNode::getBool() const
{
   DebugAssert (dataType() == TpBool, AipsError);
   return pExprBool_p->getScalar();
}



LatticeExprNode::operator LatticeExpr<Float>()
{
   return LatticeExpr<Float> (LatticeExprNode(makeFloat()), 0);
}

LatticeExprNode::operator LatticeExpr<Double>()
{
   return LatticeExpr<Double> (LatticeExprNode(makeDouble()), 0);
}

LatticeExprNode::operator LatticeExpr<Complex>()
{
   return LatticeExpr<Complex> (LatticeExprNode(makeComplex()), 0);
}

LatticeExprNode::operator LatticeExpr<DComplex>()
{
   return LatticeExpr<DComplex> (LatticeExprNode(makeDComplex()), 0);
}

LatticeExprNode::operator LatticeExpr<Bool>()
{
   AlwaysAssert (dataType() == TpBool, AipsError);
   return LatticeExpr<Bool> (*this, 0);
}


LatticeExprNode operator+(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: unary operator +" << endl;
#endif
   AlwaysAssert (expr.dataType() != TpBool, AipsError);
   return expr;
}

LatticeExprNode operator-(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: Unary operator -" << endl;
#endif
   AlwaysAssert (expr.dataType() != TpBool, AipsError);
   return LatticeExprNode::newNumUnary (LELUnaryEnums::MINUS, expr);
}



LatticeExprNode toFloat(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function float" << endl;
#endif
   return expr.makeFloat();
}

LatticeExprNode toDouble(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function double" << endl;
#endif
   return expr.makeDouble();
}

LatticeExprNode toComplex(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function complex" << endl;
#endif
   return expr.makeComplex();
}

LatticeExprNode toDComplex(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function dcomplex" << endl;
#endif
   return expr.makeDComplex();
}


LatticeExprNode sin(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function sin" << endl;
#endif
   return LatticeExprNode::newNumFunc1D (LELFunctionEnums::SIN, expr);
}

LatticeExprNode sinh(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function sinh" << endl;
#endif
   return LatticeExprNode::newNumFunc1D (LELFunctionEnums::SINH, expr);
}

LatticeExprNode asin(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function asin" << endl;
#endif
   return LatticeExprNode::newRealFunc1D (LELFunctionEnums::ASIN, expr);
}

LatticeExprNode cos(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function cos" << endl;
#endif
   return LatticeExprNode::newNumFunc1D (LELFunctionEnums::COS, expr);
}

LatticeExprNode cosh(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function cosh" << endl;
#endif
   return LatticeExprNode::newNumFunc1D (LELFunctionEnums::COSH, expr);
}

LatticeExprNode acos(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function acos" << endl;
#endif
   return LatticeExprNode::newRealFunc1D (LELFunctionEnums::ACOS, expr);
}

LatticeExprNode tan(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function tan" << endl;
#endif
   return LatticeExprNode::newRealFunc1D (LELFunctionEnums::TAN, expr);
}

LatticeExprNode tanh(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function tanh" << endl;
#endif
   return LatticeExprNode::newRealFunc1D (LELFunctionEnums::TANH, expr);
}

LatticeExprNode atan(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function atan" << endl;
#endif
   return LatticeExprNode::newRealFunc1D (LELFunctionEnums::ATAN, expr);
}

LatticeExprNode exp(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function exp" << endl;
#endif
   return LatticeExprNode::newNumFunc1D (LELFunctionEnums::EXP, expr);
}

LatticeExprNode log(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function log" << endl;
#endif
   return LatticeExprNode::newNumFunc1D (LELFunctionEnums::LOG, expr);
}

LatticeExprNode log10(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function log10" << endl;
#endif
   return LatticeExprNode::newNumFunc1D (LELFunctionEnums::LOG10, expr);
}

LatticeExprNode sqrt(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function sqrt" << endl;
#endif
   return LatticeExprNode::newNumFunc1D (LELFunctionEnums::SQRT, expr);
}

LatticeExprNode ceil(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function ceil" << endl;
#endif
   return LatticeExprNode::newRealFunc1D (LELFunctionEnums::CEIL, expr);
}

LatticeExprNode floor(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function floor" << endl;
#endif
   return LatticeExprNode::newRealFunc1D (LELFunctionEnums::FLOOR, expr);
}

LatticeExprNode abs(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function abs" << endl;
#endif
   return LatticeExprNode::newNumFunc1D (LELFunctionEnums::ABS, expr);
}

LatticeExprNode min(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function min" << endl;
#endif
   return LatticeExprNode::newNumFunc1D (LELFunctionEnums::MIN1D, expr);
}

LatticeExprNode max(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function max" << endl;
#endif
   return LatticeExprNode::newNumFunc1D (LELFunctionEnums::MAX1D, expr);
}

LatticeExprNode mean(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function mean" << endl;
#endif
   return LatticeExprNode::newNumFunc1D (LELFunctionEnums::MEAN1D, expr);
}

LatticeExprNode sum(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function sum" << endl;
#endif
   return LatticeExprNode::newNumFunc1D (LELFunctionEnums::SUM, expr);
}

LatticeExprNode nelements(const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function nelements" << endl;
#endif
   Block<LatticeExprNode> arg(1, expr);
   return new LELFunctionDouble (LELFunctionEnums::NELEM, arg);
}


LatticeExprNode atan2 (const LatticeExprNode& left,
		       const LatticeExprNode& right)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 2d function atan2" << endl;
#endif
   return LatticeExprNode::newNumFunc2D (LELFunctionEnums::ATAN2, left, right);
}

LatticeExprNode pow (const LatticeExprNode& left,
                     const LatticeExprNode& right)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 2d function pow" << endl;
#endif
   return LatticeExprNode::newNumFunc2D (LELFunctionEnums::POW, left, right);
}

LatticeExprNode fmod (const LatticeExprNode& left,
                      const LatticeExprNode& right)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 2d function fmod" << endl;
#endif
   return LatticeExprNode::newNumFunc2D (LELFunctionEnums::FMOD, left, right);
}

LatticeExprNode min (const LatticeExprNode& left,
                     const LatticeExprNode& right)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 2d function min" << endl;
#endif
   return LatticeExprNode::newNumFunc2D (LELFunctionEnums::MIN, left, right);
}

LatticeExprNode max (const LatticeExprNode& left,
                     const LatticeExprNode& right)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 2d function max" << endl;
#endif
   return LatticeExprNode::newNumFunc2D (LELFunctionEnums::MAX, left, right);
}


LatticeExprNode operator+ (const LatticeExprNode& left,
                           const LatticeExprNode& right)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: binary operator +" << endl;
#endif
   return LatticeExprNode::newNumBinary (LELBinaryEnums::ADD, left, right);
}

LatticeExprNode operator- (const LatticeExprNode& left,
                           const LatticeExprNode& right)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode::binary  operator -" << endl;
#endif
   return LatticeExprNode::newNumBinary (LELBinaryEnums::SUBTRACT, left, right);
}

LatticeExprNode operator* (const LatticeExprNode& left,
                           const LatticeExprNode& right)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: binary operator *" << endl;
#endif
   return LatticeExprNode::newNumBinary (LELBinaryEnums::MULTIPLY, left, right);
}

LatticeExprNode operator/ (const LatticeExprNode& left,
                           const LatticeExprNode& right)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: binary operator /" << endl;
#endif
   return LatticeExprNode::newNumBinary (LELBinaryEnums::DIVIDE, left, right);
}


LatticeExprNode operator== (const LatticeExprNode& left,
			    const LatticeExprNode& right)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: binary operator ==" << endl;
#endif
   return LatticeExprNode::newBinaryCmp (LELBinaryEnums::EQ, left, right);
}

LatticeExprNode operator> (const LatticeExprNode& left,
			   const LatticeExprNode& right)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: binary operator >" << endl;
#endif
   AlwaysAssert (left.dataType() != TpBool  &&  right.dataType != TpBool,
		 AipsError);
   return LatticeExprNode::newBinaryCmp (LELBinaryEnums::GT, left, right);
}

LatticeExprNode operator>= (const LatticeExprNode& left,
			    const LatticeExprNode& right)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: binary operator >=" << endl;
#endif
   AlwaysAssert (left.dataType() != TpBool  &&  right.dataType != TpBool,
		 AipsError);
   return LatticeExprNode::newBinaryCmp (LELBinaryEnums::GE, left, right);
}

LatticeExprNode operator< (const LatticeExprNode& left,
			   const LatticeExprNode& right)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: binary operator <" << endl;
#endif
   AlwaysAssert (left.dataType() != TpBool  &&  right.dataType != TpBool,
		 AipsError);
   return LatticeExprNode::newBinaryCmp (LELBinaryEnums::GT, right, left);
}

LatticeExprNode operator<= (const LatticeExprNode& left,
			    const LatticeExprNode& right)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: binary operator <=" << endl;
#endif
   AlwaysAssert (left.dataType() != TpBool  &&  right.dataType != TpBool,
		 AipsError);
   return LatticeExprNode::newBinaryCmp (LELBinaryEnums::GE, right, left);
}

LatticeExprNode operator!= (const LatticeExprNode& left,
			    const LatticeExprNode& right)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: binary operator !=" << endl;
#endif
   return LatticeExprNode::newBinaryCmp (LELBinaryEnums::NE, left, right);
}



LatticeExprNode operator&& (const LatticeExprNode& left,
			    const LatticeExprNode& right)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: binary operator &&" << endl;
#endif
   AlwaysAssert (left.dataType() == TpBool  &&  right.dataType == TpBool,
		 AipsError);
   return new LELBinaryBool(LELBinaryEnums::AND, left.pExprBool_p,
			    right.pExprBool_p);
}

LatticeExprNode operator|| (const LatticeExprNode& left,
			    const LatticeExprNode& right)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: binary operator ||" << endl;
#endif
   AlwaysAssert (left.dataType() == TpBool  &&  right.dataType == TpBool,
		 AipsError);
   return new LELBinaryBool(LELBinaryEnums::OR, left.pExprBool_p,
			    right.pExprBool_p);
}

LatticeExprNode operator! (const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: unary operator !" << endl;
#endif
   AlwaysAssert (expr.dataType() == TpBool, AipsError);
   return new LELUnaryBool(LELUnaryEnums::NOT, expr.pExprBool_p);
}

LatticeExprNode all (const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function all" << endl;
#endif
   AlwaysAssert (expr.dataType() == TpBool, AipsError);
   Block<LatticeExprNode> arg(1, expr);
   return new LELFunctionBool(LELFunctionEnums::ALL, arg);
}

LatticeExprNode any (const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function any" << endl;
#endif
   AlwaysAssert (expr.dataType() == TpBool, AipsError);
   Block<LatticeExprNode> arg(1, expr);
   return new LELFunctionBool(LELFunctionEnums::ANY, arg);
}

LatticeExprNode ntrue (const LatticeExprNode& expr)
{ 
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function ntrue" << endl;
#endif
   AlwaysAssert (expr.dataType() == TpBool, AipsError);
   Block<LatticeExprNode> arg(1, expr);
   return new LELFunctionDouble(LELFunctionEnums::NTRUE, arg);
}

LatticeExprNode nfalse (const LatticeExprNode& expr)
{  
#if defined(AIPS_TRACE)
   cout << "LatticeExprNode:: 1d function nfalse" << endl;
#endif
   AlwaysAssert (expr.dataType() == TpBool, AipsError);
   Block<LatticeExprNode> arg(1, expr);
   return new LELFunctionDouble(LELFunctionEnums::NFALSE, arg);
}



LatticeExprNode LatticeExprNode::newNumUnary (LELUnaryEnums::Operation oper,
					      const LatticeExprNode& expr)
{
   switch (expr.dataType()) {
   case TpFloat:
      return new LELUnary<Float> (oper, expr.pExprFloat_p);
   case TpDouble:
      return new LELUnary<Double> (oper, expr.pExprDouble_p);
   case TpComplex:
      return new LELUnary<Complex> (oper, expr.pExprComplex_p);
   case TpDComplex:
      return new LELUnary<DComplex> (oper, expr.pExprDComplex_p);
   default:
      throw (AipsError
            ("LatticeExpr: Bool argument used in numerical unary operation"));
   }
   return LatticeExprNode();
}


LatticeExprNode LatticeExprNode::newNumFunc1D (LELFunctionEnums::Function func,
					       const LatticeExprNode& expr)
{
   AlwaysAssert (expr.dataType() != TpBool, AipsError);
   switch (expr.dataType()) {
   case TpFloat:
      return new LELFunction1D<Float> (func, expr.pExprFloat_p);
   case TpDouble:
      return new LELFunction1D<Double> (func, expr.pExprDouble_p);
   case TpComplex:
      return new LELFunction1D<Complex> (func, expr.pExprComplex_p);
   case TpDComplex:
      return new LELFunction1D<DComplex> (func, expr.pExprDComplex_p);
   default:
      throw (AipsError
            ("LatticeExpr: Bool argument used in numerical function"));
   }
   return LatticeExprNode();
}


LatticeExprNode LatticeExprNode::newRealFunc1D (LELFunctionEnums::Function func,
						const LatticeExprNode& expr)
{
   AlwaysAssert (expr.dataType() != TpBool, AipsError);
   switch (expr.dataType()) {
   case TpFloat:
      return new LELFunction1D<Float> (func, expr.pExprFloat_p);
   case TpDouble:
      return new LELFunction1D<Double> (func, expr.pExprDouble_p);
   default:
      throw (AipsError
            ("LatticeExpr: Bool or complex argument used in real "
	     "numerical function"));
   }
   return LatticeExprNode();
}


LatticeExprNode LatticeExprNode::newNumFunc2D (LELFunctionEnums::Function func,
					       const LatticeExprNode& left,
					       const LatticeExprNode& right)
{
   DataType dtype = resultDataType (left.dataType(), right.dataType());
   Block<LatticeExprNode> arg(2);
   switch (dtype) {
   case TpFloat:
      arg[0] = left.makeFloat();
      arg[1] = right.makeFloat();
      return new LELFunctionFloat (func, arg);
   case TpDouble:
      arg[0] = left.makeDouble();
      arg[1] = right.makeDouble();
      return new LELFunctionDouble (func, arg);
   case TpComplex:
      arg[0] = left.makeComplex();
      arg[1] = right.makeComplex();
      return new LELFunctionComplex (func, arg);
   case TpDComplex:
      arg[0] = left.makeDComplex();
      arg[1] = right.makeDComplex();
      return new LELFunctionDComplex (func, arg);
   default:
      throw (AipsError
            ("LatticeExpr: Bool argument used in numerical function"));
   }
   return LatticeExprNode();
}


LatticeExprNode LatticeExprNode::newNumBinary (LELBinaryEnums::Operation oper,
					       const LatticeExprNode& left,
					       const LatticeExprNode& right)
{
   DataType dtype = resultDataType (left.dataType(), right.dataType());
   switch (dtype) {
   case TpFloat:
      return new LELBinary<Float> (oper, left.makeFloat(),
				   right.makeFloat());
   case TpDouble:
      return new LELBinary<Double> (oper, left.makeDouble(),
				    right.makeDouble());
   case TpComplex:
      return new LELBinary<Complex> (oper, left.makeComplex(),
				     right.makeComplex());
   case TpDComplex:
      return new LELBinary<DComplex> (oper, left.makeDComplex(),
				      right.makeDComplex());
   default:
      throw (AipsError
            ("LatticeExpr: Bool argument used in numerical binary operation"));
   }
   return LatticeExprNode();
}


LatticeExprNode LatticeExprNode::newBinaryCmp (LELBinaryEnums::Operation oper,
					       const LatticeExprNode& left,
					       const LatticeExprNode& right)
{
   DataType dtype = resultDataType (left.dataType(), right.dataType());
   switch (dtype) {
   case TpFloat:
      return new LELBinaryCmp<Float> (oper, left.makeFloat(),
				      right.makeFloat());
   case TpDouble:
      return new LELBinaryCmp<Double> (oper, left.makeDouble(),
				       right.makeDouble());
   case TpComplex:
      return new LELBinaryCmp<Complex> (oper, left.makeComplex(),
					right.makeComplex());
   case TpDComplex:
      return new LELBinaryCmp<DComplex> (oper, left.makeDComplex(),
					 right.makeDComplex());
   case TpBool:
      if (oper != LELBinaryEnums::EQ  &&  oper != LELBinaryEnums::NE) {
	 throw (AipsError ("LatticeExpr: Bool data type cannot be used with "
			   ">, >=, <, and <= operator"));
      }
      return new LELBinaryBool (oper, left.pExprBool_p, right.pExprBool_p);
   default:
      throw (AipsError ("LatticeExpr: invalid data type used in comparison"));
   }
   return LatticeExprNode();
}


DataType LatticeExprNode::resultDataType (DataType left, DataType right)
{
    if (left == right) {
	return left;
    }
    if (left == TpBool  ||  right == TpBool) {
	throw (AipsError
                     ("LatticeExpr: Bool and numeric operands cannot mixed"));
    }
    if (left == TpDComplex  ||  right == TpDComplex) {
	return TpDComplex;
    }
    if (left == TpComplex  ||  right == TpComplex) {
	if (left == TpDouble  ||  right == TpDouble) {
	    return TpDComplex;
	}
	return TpComplex;
    }
    if (left == TpDouble  ||  right == TpDouble) {
	return TpDouble;
    }
    return TpFloat;
}


LELAttribute LatticeExprNode::checkArg (const Block<LatticeExprNode>& arg,
					const Block<Int>& argType,
					Bool expectArray)
{
    if (arg.nelements() != argType.nelements()) {
	throw (AipsError ("LatticeExpr: invalid number of function arguments"));
    }
    Bool      isScalar = True;
    IPosition shape;
    for (uInt i=0; i<arg.nelements(); i++) {
	if (arg[i].dataType() != argType[i]) {
	    throw (AipsError ("LatticeExpr: function argument has an "
			      "invalid data type"));
	}
	if (! arg[i].isScalar()) {
	    if (isScalar) {
		isScalar = False;
		shape    = arg[i].shape();
	    } else if (arg[i].shape() != shape) {
		throw (AipsError ("LatticeExpr: shapes of function arguments "
				  "mismatch"));
	    }
	}
    }
    if (expectArray  &&  shape.nelements() == 0) {
	throw (AipsError ("LatticeExpr: expected a lattice function argument"));
    }
    return LELAttribute (isScalar, shape);
}


CountedPtr<LELInterface<Float> > LatticeExprNode::makeFloat() const
{
    switch (dataType()) {
    case TpFloat:
	return pExprFloat_p;
    case TpDouble:
	return new LELConvert<Float,Double> (pExprDouble_p);
    default:
	throw (AipsError ("LatticeExpr: conversion to Float not possible"));
    }
    return 0;
}

CountedPtr<LELInterface<Double> > LatticeExprNode::makeDouble() const
{
    switch (dataType()) {
    case TpFloat:
	return new LELConvert<Double,Float> (pExprFloat_p);
    case TpDouble:
	return pExprDouble_p;
    default:
	throw (AipsError ("LatticeExpr: conversion to Double not possible"));
    }
    return 0;
}

CountedPtr<LELInterface<Complex> > LatticeExprNode::makeComplex() const
{
    switch (dataType()) {
    case TpFloat:
	return new LELConvert<Complex,Float> (pExprFloat_p);
    case TpDouble:
	return new LELConvert<Complex,Double> (pExprDouble_p);
    case TpComplex:
	return pExprComplex_p;
    case TpDComplex:
	return new LELConvert<Complex,DComplex> (pExprDComplex_p);
    default:
	throw (AipsError ("LatticeExpr: conversion to Complex not possible"));
    }
    return 0;
}

CountedPtr<LELInterface<DComplex> > LatticeExprNode::makeDComplex() const
{
    switch (dataType()) {
    case TpFloat:
	return new LELConvert<DComplex,Float> (pExprFloat_p);
    case TpDouble:
	return new LELConvert<DComplex,Double> (pExprDouble_p);
    case TpComplex:
	return new LELConvert<DComplex,Complex> (pExprComplex_p);
    case TpDComplex:
	return pExprDComplex_p;
    default:
	throw (AipsError ("LatticeExpr: conversion to DComplex not possible"));
    }
    return 0;
}
