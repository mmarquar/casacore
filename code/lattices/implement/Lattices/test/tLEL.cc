//# tLEL.cc:  Tests the LEL* classes directly
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

#include <trial/Lattices/LatticeExpr.h>
#include <trial/Lattices/LELAttribute.h>
#include <trial/Lattices/LELBinary.h>
#include <trial/Lattices/LELConvert.h>
#include <trial/Lattices/LELFunction.h>
#include <trial/Lattices/LELLattice.h>
#include <trial/Lattices/LELUnary.h>

#include <trial/Lattices/ArrayLattice.h>
#include <trial/Lattices/PixelBox.h>

#include <aips/Arrays/Array.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Exceptions/Error.h>
#include <aips/Inputs/Input.h>
#include <aips/Lattices/IPosition.h>
#include <aips/Mathematics/Constants.h>
#include <aips/Mathematics/Complex.h>

#include <iostream.h>

Bool checkFloat (LELInterface<Float>& expr, 
                 const Float Result,
                 const String name,
                 const IPosition shape,
                 const Bool shouldBeScalar,
                 const Bool supress);

Bool checkDouble (LELInterface<Double>& expr, 
                 const Double Result,
                 const String name,
                 const IPosition shape,
                 const Bool shouldBeScalar,
                 const Bool supress);

Bool checkComplex (LELInterface<Complex>& expr, 
                 const Complex Result,
                 const String name,
                 const IPosition shape,
                 const Bool shouldBeScalar,
                 const Bool supress);

Bool checkDComplex (LELInterface<DComplex>& expr, 
                 const DComplex Result,
                 const String name,
                 const IPosition shape,
                 const Bool shouldBeScalar,
                 const Bool supress);

Bool checkBool (LELInterface<Bool>& expr, 
                 const Bool Result,
                 const String name,
                 const IPosition shape,
                 const Bool shouldBeScalar,
                 const Bool supress);


main (int argc, char *argv[])
{
 try {
    Input inp(1);
    inp.Version(" ");
    inp.Create("nx", "2", "Number of pixels along the x-axis", "int");
    inp.Create("ny", "2", "Number of pixels along the y-axis", "int");
    inp.Create("sup", "True", "Supress caught exception messages", "Bool");
    inp.ReadArguments(argc, argv);

    const uInt nx=inp.GetInt("nx");
    const uInt ny=inp.GetInt("ny");
    const Bool supress =inp.GetBool("sup");

//
// The use of these tiny ArrayLattices means this test program
// does not computationally stress the classes.  Here we just
// test them logically.  See other test programs for stress tests
//

    IPosition shape(2,nx,ny);

// Bool Lattices

    Array<Bool> BArr(shape);
    Bool BResult;
    ArrayLattice<Bool> aB(shape);
    ArrayLattice<Bool> bB(shape);
    ArrayLattice<Bool> cB(shape);
    Bool aBVal = True;
    aB.set(aBVal);
    Bool bBVal = False;
    bB.set(bBVal);
    Bool cBVal = True;
    cB.set(cBVal);


// FLoat Lattices

    Array<Float> FArr(shape);
    Float FResult;
    ArrayLattice<Float> aF(shape);
    ArrayLattice<Float> bF(shape);
    ArrayLattice<Float> cF(shape);
    Float aFVal = 0.0;
    aF.set(aFVal);
    Float bFVal = 2.0;
    bF.set(bFVal);
    Float cFVal = 3.0;
    cF.set(cFVal);


// Double Lattices

    Array<Double> DArr(shape);
    Double DResult;
    ArrayLattice<Double> aD(shape);
    ArrayLattice<Double> bD(shape);
    ArrayLattice<Double> cD(shape);
    Double aDVal = 0.0;
    aD.set(aDVal);
    Double bDVal = 2.0;
    bD.set(bDVal);
    Double cDVal = 3.0;
    cD.set(cDVal);


// Complex Lattices

    Array<Complex> CArr(shape);
    Complex CResult;
    ArrayLattice<Complex> aC(shape);
    ArrayLattice<Complex> bC(shape);
    ArrayLattice<Complex> cC(shape);
    Complex aCVal = Complex(0.0,0.0);
    aC.set(aCVal);
    Complex bCVal = Complex(2.0,2.0);
    bC.set(bCVal);
    Complex cCVal = Complex(3.0,3.0);
    cC.set(cCVal);


// DComplex Lattices

    Array<DComplex> DCArr(shape);
    DComplex DCResult;
    ArrayLattice<DComplex> aDC(shape);
    ArrayLattice<DComplex> bDC(shape);
    ArrayLattice<DComplex> cDC(shape);
    DComplex aDCVal = DComplex(0.0,0.0);
    aDC.set(aDCVal);
    DComplex bDCVal = DComplex(2.0,2.0);
    bDC.set(bDCVal);
    DComplex cDCVal = DComplex(3.0,3.0);
    cDC.set(cDCVal);


    IPosition origin(2,0,0);
    PixelBox region(origin, shape-1, shape);
    Bool ok = True;


//************************************************************************
// 
// LELAttribute; coverage 100%
//
  {
    cout << "LELAttribute" << endl;
    Bool isScalar1 = True;
    IPosition shape1 = IPosition();
    LELAttribute attr1(isScalar1, shape1);
    if (attr1.isScalar() != isScalar1) {
      cout << "   isScalar function failed" << endl;
      ok = False;
    }
    if (attr1.shape() != shape1) {
      cout << "   shape function failed" << endl;
      ok = False;
    }    
    Bool isScalar2 = False;
    IPosition shape2 = shape;
    LELAttribute attr2(isScalar2, shape2);

    LELAttribute attr3 = attr2;
    if (attr3.isScalar()!=attr2.isScalar() || attr3.shape() != attr2.shape()) {
      cout << "   Assignment failed" << endl;
      ok = False;
    }    

// Result of scalar and non-scalar is non-scalar

    LELAttribute attr4(attr1, attr2);
    if (attr4.isScalar() || attr4.shape() != attr2.shape()) {
      cout << "   double constructor failed" << endl;
      ok = False;
    }    
  }       
       

//************************************************************************
//
// LELLattice; 100% coverage
//
  {
    cout << endl << "LELLattice<Float> " << endl;
    LELLattice<Float> expr(bF);
    FResult = bFVal;
    if (!checkFloat (expr, FResult, String("LELLattice"), shape, False, supress)) ok = False;
  }
  {
    cout << "LELLattice<Double> " << endl;
    LELLattice<Double> expr(bD);
    DResult = bDVal;
    if (!checkDouble(expr, DResult, String("LELLattice"), shape, False, supress)) ok = False;
  }
  {
    cout << "LELLattice<Complex> " << endl;
    LELLattice<Complex> expr(bC);
    CResult = bCVal;
    if (!checkComplex(expr, CResult, String("LELLattice"), shape, False, supress)) ok = False;
  }
  {
    cout << "LELLattice<DComplex> " << endl;
    LELLattice<DComplex> expr(bDC);
    DCResult = bDCVal;
    if (!checkDComplex(expr, DCResult, String("LELLattice"), shape, False, supress)) ok = False;
  }
  {
    cout << "LELLattice<Bool> " << endl;
    LELLattice<Bool> expr(bB);
    BResult = bBVal;
    if (!checkBool(expr, BResult, String("LELLattice"), shape, False, supress)) ok = False;
  }

//************************************************************************
//
// LELUnaryConst
//
  {

    cout << endl << "LELUnaryConst<Float>" << endl;
    LELUnaryConst<Float> expr(aFVal);
    if (!checkFloat (expr, aFVal, String("LELUnaryConst"), shape, True, supress)) ok = False;

  }
  {
    cout << "LELUnaryConst<Double>" << endl;
    LELUnaryConst<Double> expr(aDVal);
    if (!checkDouble(expr, aDVal, String("LELUnaryConst"), shape, True, supress)) ok = False;
  }
  {
    cout << "LELUnaryConst<Complex>" << endl;
    
    LELUnaryConst<Complex> expr(aCVal);
    if (!checkComplex(expr, aCVal, String("LELUnaryConst"), shape, True, supress)) ok = False;
  }
  {
    cout << "LELUnaryConst<DComplex>" << endl;
    LELUnaryConst<DComplex> expr(aDCVal);
    if (!checkDComplex(expr, aDCVal, String("LELUnaryConst"), shape, True, supress)) ok = False;
  }
  {
    cout << "LELUnaryConst<Bool>" << endl;
    LELUnaryConst<Bool> expr(aBVal);
    if (!checkBool(expr, aBVal, String("LELUnaryConst"), shape, True, supress)) ok = False;
  }

//
//************************************************************************
//
// LELUnary; 100% coverage
//
   cout << endl << "LELUnary<Float>" << endl;
  {
    CountedPtr<LELInterface<Float> > pExpr = new LELLattice<Float>(bF);

// Note that operator+ is not actually implemented in LELUnary because it
// wouldn't do anything !  It is implemented in LatticeExprNode though

    cout << "   Operator -" << endl;     
    LELUnary<Float> expr(LELUnaryEnums::MINUS, pExpr);
    if (!checkFloat (expr, -bFVal, String("LELUnary"), shape, False, supress)) ok = False;
  }

   cout << "LELUnary<Double>" << endl;
  {

// Note that operator+ is not actually implemented in LELUnary because it
// wouldn't do anything !  It is implemented in LatticeExprNode though

    cout << "   Operator -" << endl;     
    CountedPtr<LELInterface<Double> > pExpr = new LELLattice<Double>(bD);
    LELUnary<Double> expr(LELUnaryEnums::MINUS, pExpr);
    if (!checkDouble(expr, -bDVal, String("LELUnary"), shape, False, supress)) ok = False;
  }


   cout << "LELUnary<Complex>" << endl;
  {

// Note that operator+ is not actually implemented in LELUnary because it
// wouldn't do anything !  It is implemented in LatticeExprNode though

    cout << "   Operator -" << endl;     
    CountedPtr<LELInterface<Complex> > pExpr = new LELLattice<Complex>(bC);
    LELUnary<Complex> expr(LELUnaryEnums::MINUS, pExpr);
    if (!checkComplex(expr, -bCVal, String("LELUnary"), shape, False, supress)) ok = False;
  }


   cout << "LELUnary<DComplex>" << endl;
  {

// Note that operator+ is not actually implemented in LELUnary because it
// wouldn't do anything !  It is implemented in LatticeExprNode though

    cout << "   Operator -" << endl;     
    CountedPtr<LELInterface<DComplex> > pExpr = new LELLattice<DComplex>(bDC);
    LELUnary<DComplex> expr(LELUnaryEnums::MINUS, pExpr);
    if (!checkDComplex(expr, -bDCVal, String("LELUnary"), shape, False, supress)) ok = False;
  }

//************************************************************************
//
// LELUnaryBool; 100% coverage
//
  {
    cout << endl << "LELUnaryBool" << endl;

    {
      cout << "   Operator !" << endl;     
      CountedPtr<LELInterface<Bool> > pExpr = new LELLattice<Bool>(aB);
      LELUnaryBool expr(LELUnaryEnums::NOT, pExpr);
      if (!checkBool(expr, ToBool(!aBVal), String("LELUnaryBool"), shape, False, supress)) ok = False;
    }
  }
//
//************************************************************************
//
// LELBinary<Float>; 100% coverage
//
  {
    cout << endl << "LELBinary<Float>" << endl;
    CountedPtr<LELInterface<Float> > pExprLeft = new LELLattice<Float>(bF);
    CountedPtr<LELInterface<Float> > pExprRight = new LELLattice<Float>(cF);

    {
    cout << "   Operator +" << endl;     
    LELBinary<Float> expr(LELBinaryEnums::ADD, pExprLeft, pExprRight);
    FResult = bFVal + cFVal;
    if (!checkFloat (expr, FResult, String("LELBinary"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Operator -" << endl;     
    LELBinary<Float> expr(LELBinaryEnums::SUBTRACT, pExprLeft, pExprRight);
    FResult = bFVal - cFVal;
    if (!checkFloat (expr, FResult, String("LELBinary"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Operator *" << endl;     
    LELBinary<Float> expr(LELBinaryEnums::MULTIPLY, pExprLeft, pExprRight);
    FResult = bFVal * cFVal;
    if (!checkFloat (expr, FResult, String("LELBinary"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Operator /" << endl;     
    LELBinary<Float> expr(LELBinaryEnums::DIVIDE, pExprLeft, pExprRight);
    FResult = bFVal / cFVal;
    if (!checkFloat (expr, FResult, String("LELBinary"), shape, False, supress)) ok = False;
    }
  }
//
//
//************************************************************************
//
// LELBinary<Double>; 100% coverage
//
  {
    cout << endl << "LELBinary<Double>" << endl;
    CountedPtr<LELInterface<Double> > pExprLeft = new LELLattice<Double>(bD);
    CountedPtr<LELInterface<Double> > pExprRight = new LELLattice<Double>(cD);

    {
    cout << "   Operator +" << endl;     
    LELBinary<Double> expr(LELBinaryEnums::ADD, pExprLeft, pExprRight);
    DResult = bDVal + cDVal;
    if (!checkDouble (expr, DResult, String("LELBinary"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Operator -" << endl;     
    LELBinary<Double> expr(LELBinaryEnums::SUBTRACT, pExprLeft, pExprRight);
    DResult = bDVal - cDVal;
    if (!checkDouble (expr, DResult, String("LELBinary"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Operator *" << endl;     
    LELBinary<Double> expr(LELBinaryEnums::MULTIPLY, pExprLeft, pExprRight);
    DResult = bDVal * cDVal;
    if (!checkDouble (expr, DResult, String("LELBinary"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Operator /" << endl;     
    LELBinary<Double> expr(LELBinaryEnums::DIVIDE, pExprLeft, pExprRight);
    DResult = bDVal / cDVal;
    if (!checkDouble (expr, DResult, String("LELBinary"), shape, False, supress)) ok = False;
    }
  }
//
//************************************************************************
//
// LELBinary<Complex>; 100% coverage
//
  {
    cout << endl << "LELBinary<Complex>" << endl;
    CountedPtr<LELInterface<Complex> > pExprLeft = new LELLattice<Complex>(bC);
    CountedPtr<LELInterface<Complex> > pExprRight = new LELLattice<Complex>(cC);

    {
    cout << "   Operator +" << endl;     
    LELBinary<Complex> expr(LELBinaryEnums::ADD, pExprLeft, pExprRight);
    CResult = bCVal + cCVal;
    if (!checkComplex (expr, CResult, String("LELBinary"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Operator -" << endl;     
    LELBinary<Complex> expr(LELBinaryEnums::SUBTRACT, pExprLeft, pExprRight);
    CResult = bCVal - cCVal;
    if (!checkComplex (expr, CResult, String("LELBinary"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Operator *" << endl;     
    LELBinary<Complex> expr(LELBinaryEnums::MULTIPLY, pExprLeft, pExprRight);
    CResult = bCVal * cCVal;
    if (!checkComplex (expr, CResult, String("LELBinary"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Operator /" << endl;     
    LELBinary<Complex> expr(LELBinaryEnums::DIVIDE, pExprLeft, pExprRight);
    CResult = bCVal / cCVal;
    if (!checkComplex (expr, CResult, String("LELBinary"), shape, False, supress)) ok = False;
    }
  }
//
//************************************************************************
//
// LELBinary<DComplex>; 100% coverage
//
  {
    cout << endl << "LELBinary<DComplex>" << endl;
    CountedPtr<LELInterface<DComplex> > pExprLeft = new LELLattice<DComplex>(bDC);
    CountedPtr<LELInterface<DComplex> > pExprRight = new LELLattice<DComplex>(cDC);

    {
    cout << "   Operator +" << endl;     
    LELBinary<DComplex> expr(LELBinaryEnums::ADD, pExprLeft, pExprRight);
    DCResult = bDCVal + cDCVal;
    if (!checkDComplex (expr, DCResult, String("LELBinary"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Operator -" << endl;     
    LELBinary<DComplex> expr(LELBinaryEnums::SUBTRACT, pExprLeft, pExprRight);
    DCResult = bDCVal - cDCVal;
    if (!checkDComplex (expr, DCResult, String("LELBinary"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Operator *" << endl;     
    LELBinary<DComplex> expr(LELBinaryEnums::MULTIPLY, pExprLeft, pExprRight);
    DCResult = bDCVal * cDCVal;
    if (!checkDComplex (expr, DCResult, String("LELBinary"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Operator /" << endl;     
    LELBinary<DComplex> expr(LELBinaryEnums::DIVIDE, pExprLeft, pExprRight);
    DCResult = bDCVal / cDCVal;
    if (!checkDComplex (expr, DCResult, String("LELBinary"), shape, False, supress)) ok = False;
    }
  }
//
//************************************************************************
//
// LELBinaryCmp<Float>; 100% coverage
//
  {
    cout << endl << "LELBinaryCmp<Float>" << endl;
    CountedPtr<LELInterface<Float> > pExprLeft = new LELLattice<Float>(bF);
    CountedPtr<LELInterface<Float> > pExprRight = new LELLattice<Float>(cF);

    {
    cout << "   Operator ==" << endl;     
    LELBinaryCmp<Float> expr(LELBinaryEnums::EQ, pExprLeft, pExprRight);
    BResult = ToBool(bFVal==cFVal);
    if (!checkBool(expr, BResult, String("LELBinaryCmp"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Operator !=" << endl;     
    LELBinaryCmp<Float> expr(LELBinaryEnums::NE, pExprLeft, pExprRight);
    BResult = ToBool(bFVal!=cFVal);
    if (!checkBool(expr, BResult, String("LELBinaryCmp"), shape, False, supress)) ok = False;
    }


    {
    cout << "   Operator >" << endl;     
    LELBinaryCmp<Float> expr(LELBinaryEnums::GT, pExprLeft, pExprRight);
    BResult = ToBool(bFVal>cFVal);
    if (!checkBool(expr, BResult, String("LELBinaryCmp"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Operator >=" << endl;     
    LELBinaryCmp<Float> expr(LELBinaryEnums::GE, pExprLeft, pExprRight);
    BResult = ToBool(bFVal>=cFVal);
    if (!checkBool(expr, BResult, String("LELBinaryCmp"), shape, False, supress)) ok = False;
    }
  }
//
//************************************************************************
//
// LELBinaryCmp<Double>; 100% coverage
//
  {
    cout << endl << "LELBinaryCmp<Double>" << endl;
    CountedPtr<LELInterface<Double> > pExprLeft = new LELLattice<Double>(bD);
    CountedPtr<LELInterface<Double> > pExprRight = new LELLattice<Double>(cD);

    {
    cout << "   Operator ==" << endl;     
    LELBinaryCmp<Double> expr(LELBinaryEnums::EQ, pExprLeft, pExprRight);
    BResult = ToBool(bDVal==cDVal);
    if (!checkBool(expr, BResult, String("LELBinaryCmp"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Operator !=" << endl;     
    LELBinaryCmp<Double> expr(LELBinaryEnums::NE, pExprLeft, pExprRight);
    BResult = ToBool(bDVal!=cDVal);
    if (!checkBool(expr, BResult, String("LELBinaryCmp"), shape, False, supress)) ok = False;
    }


    {
    cout << "   Operator >" << endl;     
    LELBinaryCmp<Double> expr(LELBinaryEnums::GT, pExprLeft, pExprRight);
    BResult = ToBool(bDVal>cDVal);
    if (!checkBool(expr, BResult, String("LELBinaryCmp"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Operator >=" << endl;     
    LELBinaryCmp<Double> expr(LELBinaryEnums::GE, pExprLeft, pExprRight);
    BResult = ToBool(bDVal>=cDVal);
    if (!checkBool(expr, BResult, String("LELBinaryCmp"), shape, False, supress)) ok = False;
    }
  }
//
//************************************************************************
//
// LELBinaryCmp<Complex>; 100% coverage
//
  {
    cout << endl << "LELBinaryCmp<Complex>" << endl;
    CountedPtr<LELInterface<Complex> > pExprLeft = new LELLattice<Complex>(bC);
    CountedPtr<LELInterface<Complex> > pExprRight = new LELLattice<Complex>(cC);

    {
    cout << "   Operator ==" << endl;     
    LELBinaryCmp<Complex> expr(LELBinaryEnums::EQ, pExprLeft, pExprRight);
    BResult = ToBool(bCVal==cCVal);
    if (!checkBool(expr, BResult, String("LELBinaryCmp"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Operator !=" << endl;     
    LELBinaryCmp<Complex> expr(LELBinaryEnums::NE, pExprLeft, pExprRight);
    BResult = ToBool(bCVal!=cCVal);
    if (!checkBool(expr, BResult, String("LELBinaryCmp"), shape, False, supress)) ok = False;
    }


    {
    cout << "   Operator >" << endl;     
    LELBinaryCmp<Complex> expr(LELBinaryEnums::GT, pExprLeft, pExprRight);
    BResult = ToBool(bCVal>cCVal);
    if (!checkBool(expr, BResult, String("LELBinaryCmp"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Operator >=" << endl;     
    LELBinaryCmp<Complex> expr(LELBinaryEnums::GE, pExprLeft, pExprRight);
    BResult = ToBool(bCVal>=cCVal);
    if (!checkBool(expr, BResult, String("LELBinaryCmp"), shape, False, supress)) ok = False;
    }
  }
//
//************************************************************************
//
// LELBinaryCmp<DComplex>; 100% coverage
//
  {
    cout << endl << "LELBinaryCmp<DComplex>" << endl;
    CountedPtr<LELInterface<DComplex> > pExprLeft = new LELLattice<DComplex>(bDC);
    CountedPtr<LELInterface<DComplex> > pExprRight = new LELLattice<DComplex>(cDC);

    {
    cout << "   Operator ==" << endl;     
    LELBinaryCmp<DComplex> expr(LELBinaryEnums::EQ, pExprLeft, pExprRight);
    BResult = ToBool(bDCVal==cDCVal);
    if (!checkBool(expr, BResult, String("LELBinaryCmp"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Operator !=" << endl;     
    LELBinaryCmp<DComplex> expr(LELBinaryEnums::NE, pExprLeft, pExprRight);
    BResult = ToBool(bDCVal!=cDCVal);
    if (!checkBool(expr, BResult, String("LELBinaryCmp"), shape, False, supress)) ok = False;
    }


    {
    cout << "   Operator >" << endl;     
    LELBinaryCmp<DComplex> expr(LELBinaryEnums::GT, pExprLeft, pExprRight);
    BResult = ToBool(bDCVal>cDCVal);
    if (!checkBool(expr, BResult, String("LELBinaryCmp"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Operator >=" << endl;     
    LELBinaryCmp<DComplex> expr(LELBinaryEnums::GE, pExprLeft, pExprRight);
    BResult = ToBool(bDCVal>=cDCVal);
    if (!checkBool(expr, BResult, String("LELBinaryCmp"), shape, False, supress)) ok = False;
    }
  }
//
//************************************************************************
//
// LELBinaryBool; 100% coverage
//
  {
    cout << endl << "LELBinaryBool" << endl;
    CountedPtr<LELInterface<Bool> > pExprLeft = new LELLattice<Bool>(bB);
    CountedPtr<LELInterface<Bool> > pExprRight = new LELLattice<Bool>(cB);

    {
    cout << "   Operator ==" << endl;     
    LELBinaryBool expr(LELBinaryEnums::EQ, pExprLeft, pExprRight);
    BResult = ToBool(bBVal==cBVal);
    if (!checkBool(expr, BResult, String("LELBinaryBool"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Operator !=" << endl;     
    LELBinaryBool expr(LELBinaryEnums::NE, pExprLeft, pExprRight);
    BResult = ToBool(bBVal!=cBVal);
    if (!checkBool(expr, BResult, String("LELBinaryBool"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Operator &&" << endl;     
    LELBinaryBool expr(LELBinaryEnums::AND, pExprLeft, pExprRight);
    BResult = ToBool(bBVal&&cBVal);
    if (!checkBool(expr, BResult, String("LELBinaryBool"), shape, False, supress)) ok = False;
    }
  }

//
//************************************************************************
//
// LELFunction1D<Float>; 100% coverage
//
  {
    cout << endl << "LELFunction1D<Float>" << endl;
    CountedPtr<LELInterface<Float> > pExpr = new LELLattice<Float>(bF);

    {
    cout << "   Function sin" << endl;     
    LELFunction1D<Float> expr(LELFunctionEnums::SIN, pExpr);
    FResult = sin(bFVal);
    if (!checkFloat (expr, FResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function sinh" << endl;     
    LELFunction1D<Float> expr(LELFunctionEnums::SINH, pExpr);
    FResult = sinh(bFVal);
    if (!checkFloat (expr, FResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function cos" << endl;     
    LELFunction1D<Float> expr(LELFunctionEnums::COS, pExpr);
    FResult = cos(bFVal);
    if (!checkFloat (expr, FResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function cosh" << endl;     
    LELFunction1D<Float> expr(LELFunctionEnums::COSH, pExpr);
    FResult = cosh(bFVal);
    if (!checkFloat (expr, FResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function exp" << endl;     
    LELFunction1D<Float> expr(LELFunctionEnums::EXP, pExpr);
    FResult = exp(bFVal);
    if (!checkFloat (expr, FResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function log" << endl;     
    LELFunction1D<Float> expr(LELFunctionEnums::LOG, pExpr);
    FResult = log(bFVal);
    if (!checkFloat (expr, FResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function log10" << endl;     
    LELFunction1D<Float> expr(LELFunctionEnums::LOG10, pExpr);
    FResult = log10(bFVal);
    if (!checkFloat (expr, FResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function sqrt" << endl;     
    LELFunction1D<Float> expr(LELFunctionEnums::SQRT, pExpr);
    FResult = sqrt(bFVal);
    if (!checkFloat (expr, FResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function min" << endl;     
    LELFunction1D<Float> expr(LELFunctionEnums::MIN1D, pExpr);
    bF.getSlice(FArr, IPosition(FArr.ndim(),0), 
                FArr.shape(), IPosition(FArr.ndim(),1));
    FResult = min(FArr);
    if (!checkFloat (expr, FResult, String("LELFunction1D"), shape, True, supress)) ok = False;
    }


    {
    cout << "   Function max" << endl;     
    LELFunction1D<Float> expr(LELFunctionEnums::MAX1D, pExpr);
    bF.getSlice(FArr, IPosition(FArr.ndim(),0), 
                FArr.shape(), IPosition(FArr.ndim(),1));
    FResult = max(FArr);
    if (!checkFloat (expr, FResult, String("LELFunction1D"), shape, True, supress)) ok = False;
    }

    {
    cout << "   Function mean" << endl;     
    LELFunction1D<Float> expr(LELFunctionEnums::MEAN1D, pExpr);
    bF.getSlice(FArr, IPosition(FArr.ndim(),0), 
                FArr.shape(), IPosition(FArr.ndim(),1));
    FResult = mean(FArr);
    if (!checkFloat (expr, FResult, String("LELFunction1D"), shape, True, supress)) ok = False;
    }

    {
    cout << "   Function sum" << endl;     
    LELFunction1D<Float> expr(LELFunctionEnums::SUM, pExpr);
    bF.getSlice(FArr, IPosition(FArr.ndim(),0), 
                FArr.shape(), IPosition(FArr.ndim(),1));
    FResult = sum(FArr);
    if (!checkFloat (expr, FResult, String("LELFunction1D"), shape, True, supress)) ok = False;
    }
  }

//
//
//************************************************************************
//
// LELFunction1D<Double>; 100% coverage
//
  {
    cout << endl << "LELFunction1D<Double>" << endl;
    CountedPtr<LELInterface<Double> > pExpr = new LELLattice<Double>(bD);

    {
    cout << "   Function sin" << endl;     
    LELFunction1D<Double> expr(LELFunctionEnums::SIN, pExpr);
    DResult = sin(bDVal);
    if (!checkDouble (expr, DResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function sinh" << endl;     
    LELFunction1D<Double> expr(LELFunctionEnums::SINH, pExpr);
    DResult = sinh(bDVal);
    if (!checkDouble (expr, DResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function cos" << endl;     
    LELFunction1D<Double> expr(LELFunctionEnums::COS, pExpr);
    DResult = cos(bDVal);
    if (!checkDouble (expr, DResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function cosh" << endl;     
    LELFunction1D<Double> expr(LELFunctionEnums::COSH, pExpr);
    DResult = cosh(bDVal);
    if (!checkDouble (expr, DResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function exp" << endl;     
    LELFunction1D<Double> expr(LELFunctionEnums::EXP, pExpr);
    DResult = exp(bDVal);
    if (!checkDouble (expr, DResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function log" << endl;     
    LELFunction1D<Double> expr(LELFunctionEnums::LOG, pExpr);
    DResult = log(bDVal);
    if (!checkDouble (expr, DResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function log10" << endl;     
    LELFunction1D<Double> expr(LELFunctionEnums::LOG10, pExpr);
    DResult = log10(bDVal);
    if (!checkDouble (expr, DResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function sqrt" << endl;     
    LELFunction1D<Double> expr(LELFunctionEnums::SQRT, pExpr);
    DResult = sqrt(bDVal);
    if (!checkDouble (expr, DResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function min" << endl;     
    LELFunction1D<Double> expr(LELFunctionEnums::MIN1D, pExpr);
    bD.getSlice(DArr, IPosition(DArr.ndim(),0), 
                DArr.shape(), IPosition(DArr.ndim(),1));
    DResult = min(DArr);
    if (!checkDouble (expr, DResult, String("LELFunction1D"), shape, True, supress)) ok = False;
    }


    {
    cout << "   Function max" << endl;     
    LELFunction1D<Double> expr(LELFunctionEnums::MAX1D, pExpr);
    bD.getSlice(DArr, IPosition(DArr.ndim(),0), 
                DArr.shape(), IPosition(DArr.ndim(),1));
    DResult = max(DArr);
    if (!checkDouble (expr, DResult, String("LELFunction1D"), shape, True, supress)) ok = False;
    }

    {
    cout << "   Function mean" << endl;     
    LELFunction1D<Double> expr(LELFunctionEnums::MEAN1D, pExpr);
    bD.getSlice(DArr, IPosition(DArr.ndim(),0), 
                DArr.shape(), IPosition(DArr.ndim(),1));
    DResult = mean(DArr);
    if (!checkDouble (expr, DResult, String("LELFunction1D"), shape, True, supress)) ok = False;
    }

    {
    cout << "   Function sum" << endl;     
    LELFunction1D<Double> expr(LELFunctionEnums::SUM, pExpr);
    bD.getSlice(DArr, IPosition(DArr.ndim(),0), 
                DArr.shape(), IPosition(DArr.ndim(),1));
    DResult = sum(DArr);
    if (!checkDouble (expr, DResult, String("LELFunction1D"), shape, True, supress)) ok = False;
    }
  }

//
//
//************************************************************************
//
// LELFunction1D<Complex>; 100% coverage
//
  {
    cout << endl << "LELFunction1D<Complex>" << endl;
    CountedPtr<LELInterface<Complex> > pExpr = new LELLattice<Complex>(bC);

    {
    cout << "   Function sin" << endl;     
    LELFunction1D<Complex> expr(LELFunctionEnums::SIN, pExpr);
    CResult = sin(bCVal);
    if (!checkComplex (expr, CResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function sinh" << endl;     
    LELFunction1D<Complex> expr(LELFunctionEnums::SINH, pExpr);
    CResult = sinh(bCVal);
    if (!checkComplex (expr, CResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function cos" << endl;     
    LELFunction1D<Complex> expr(LELFunctionEnums::COS, pExpr);
    CResult = cos(bCVal);
    if (!checkComplex (expr, CResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function cosh" << endl;     
    LELFunction1D<Complex> expr(LELFunctionEnums::COSH, pExpr);
    CResult = cosh(bCVal);
    if (!checkComplex (expr, CResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function exp" << endl;     
    LELFunction1D<Complex> expr(LELFunctionEnums::EXP, pExpr);
    CResult = exp(bCVal);
    if (!checkComplex (expr, CResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function log" << endl;     
    LELFunction1D<Complex> expr(LELFunctionEnums::LOG, pExpr);
    CResult = log(bCVal);
    if (!checkComplex (expr, CResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function log10" << endl;     
    LELFunction1D<Complex> expr(LELFunctionEnums::LOG10, pExpr);
    CResult = log10(bCVal);
    if (!checkComplex (expr, CResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function sqrt" << endl;     
    LELFunction1D<Complex> expr(LELFunctionEnums::SQRT, pExpr);
    CResult = sqrt(bCVal);
    if (!checkComplex (expr, CResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function min" << endl;     
    LELFunction1D<Complex> expr(LELFunctionEnums::MIN1D, pExpr);
    bC.getSlice(CArr, IPosition(CArr.ndim(),0), 
                CArr.shape(), IPosition(CArr.ndim(),1));
    CResult = min(CArr);
    if (!checkComplex (expr, CResult, String("LELFunction1D"), shape, True, supress)) ok = False;
    }


    {
    cout << "   Function max" << endl;     
    LELFunction1D<Complex> expr(LELFunctionEnums::MAX1D, pExpr);
    bC.getSlice(CArr, IPosition(CArr.ndim(),0), 
                CArr.shape(), IPosition(CArr.ndim(),1));
    CResult = max(CArr);
    if (!checkComplex (expr, CResult, String("LELFunction1D"), shape, True, supress)) ok = False;
    }
  }

//
//
//************************************************************************
//
// LELFunction1D<DComplex>; 100% coverage
//
  {
    cout << endl << "LELFunction1D<DComplex>" << endl;
    CountedPtr<LELInterface<DComplex> > pExpr = new LELLattice<DComplex>(bDC);

    {
    cout << "   Function sin" << endl;     
    LELFunction1D<DComplex> expr(LELFunctionEnums::SIN, pExpr);
    DCResult = sin(bDCVal);
    if (!checkDComplex (expr, DCResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function sinh" << endl;     
    LELFunction1D<DComplex> expr(LELFunctionEnums::SINH, pExpr);
    DCResult = sinh(bDCVal);
    if (!checkDComplex (expr, DCResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function cos" << endl;     
    LELFunction1D<DComplex> expr(LELFunctionEnums::COS, pExpr);
    DCResult = cos(bDCVal);
    if (!checkDComplex (expr, DCResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function cosh" << endl;     
    LELFunction1D<DComplex> expr(LELFunctionEnums::COSH, pExpr);
    DCResult = cosh(bDCVal);
    if (!checkDComplex (expr, DCResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function exp" << endl;     
    LELFunction1D<DComplex> expr(LELFunctionEnums::EXP, pExpr);
    DCResult = exp(bDCVal);
    if (!checkDComplex (expr, DCResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function log" << endl;     
    LELFunction1D<DComplex> expr(LELFunctionEnums::LOG, pExpr);
    DCResult = log(bDCVal);
    if (!checkDComplex (expr, DCResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function log10" << endl;     
    LELFunction1D<DComplex> expr(LELFunctionEnums::LOG10, pExpr);
    DCResult = log10(bDCVal);
    if (!checkDComplex (expr, DCResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function sqrt" << endl;     
    LELFunction1D<DComplex> expr(LELFunctionEnums::SQRT, pExpr);
    DCResult = sqrt(bDCVal);
    if (!checkDComplex (expr, DCResult, String("LELFunction1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function min" << endl;     
    LELFunction1D<DComplex> expr(LELFunctionEnums::MIN1D, pExpr);
    bDC.getSlice(DCArr, IPosition(DCArr.ndim(),0), 
                DCArr.shape(), IPosition(DCArr.ndim(),1));
    DCResult = min(DCArr);
    if (!checkDComplex (expr, DCResult, String("LELFunction1D"), shape, True, supress)) ok = False;
    }


    {
    cout << "   Function max" << endl;     
    LELFunction1D<DComplex> expr(LELFunctionEnums::MAX1D, pExpr);
    bDC.getSlice(DCArr, IPosition(DCArr.ndim(),0), 
                DCArr.shape(), IPosition(DCArr.ndim(),1));
    DCResult = max(DCArr);
    if (!checkDComplex (expr, DCResult, String("LELFunction1D"), shape, True, supress)) ok = False;
    }

  }

//
//************************************************************************
//
// LELFunctionReal1D<Float>; 100% coverage
//
  {
    cout << endl << "LELFunctionReal1D<Float>" << endl;
    CountedPtr<LELInterface<Float> > pExpr = new LELLattice<Float>(bF);

    {
    cout << "   Function asin" << endl;     
    LELFunctionReal1D<Float> expr(LELFunctionEnums::ASIN, pExpr);
    FResult = asin(bFVal);
    if (!checkFloat (expr, FResult, String("LELFunctionReal1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function acos" << endl;     
    LELFunctionReal1D<Float> expr(LELFunctionEnums::ACOS, pExpr);
    FResult = acos(bFVal);
    if (!checkFloat (expr, FResult, String("LELFunctionReal1D"), shape, False, supress)) ok = False;
    }


    {
    cout << "   Function tan" << endl;     
    LELFunctionReal1D<Float> expr(LELFunctionEnums::TAN, pExpr);
    FResult = tan(bFVal);    
    if (!checkFloat (expr, FResult, String("LELFunctionReal1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function tanh" << endl;     
    LELFunctionReal1D<Float> expr(LELFunctionEnums::TANH, pExpr);
    FResult = tanh(bFVal);
    if (!checkFloat (expr, FResult, String("LELFunctionReal1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function ceil" << endl;     
    LELFunctionReal1D<Float> expr(LELFunctionEnums::CEIL, pExpr);
    FResult = ceil(bFVal);
    if (!checkFloat (expr, FResult, String("LELFunctionReal1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function floor" << endl;     
    LELFunctionReal1D<Float> expr(LELFunctionEnums::FLOOR, pExpr);
    FResult = floor(bFVal);
    if (!checkFloat (expr, FResult, String("LELFunctionReal1D"), shape, False, supress)) ok = False;
    }
  }

//
//************************************************************************
//
// LELFunctionReal1D<Double>; 100% coverage
//
  {
    cout << endl << "LELFunctionReal1D<Double>" << endl;
    CountedPtr<LELInterface<Double> > pExpr = new LELLattice<Double>(bD);

    {
    cout << "   Function asin" << endl;     
    LELFunctionReal1D<Double> expr(LELFunctionEnums::ASIN, pExpr);
    DResult = asin(bDVal);
    if (!checkDouble (expr, DResult, String("LELFunctionReal1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function acos" << endl;     
    LELFunctionReal1D<Double> expr(LELFunctionEnums::ACOS, pExpr);
    DResult = acos(bDVal);
    if (!checkDouble (expr, DResult, String("LELFunctionReal1D"), shape, False, supress)) ok = False;
    }


    {
    cout << "   Function tan" << endl;     
    LELFunctionReal1D<Double> expr(LELFunctionEnums::TAN, pExpr);
    DResult = tan(bDVal);    
    if (!checkDouble (expr, DResult, String("LELFunctionReal1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function tanh" << endl;     
    LELFunctionReal1D<Double> expr(LELFunctionEnums::TANH, pExpr);
    DResult = tanh(bDVal);
    if (!checkDouble (expr, DResult, String("LELFunctionReal1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function ceil" << endl;     
    LELFunctionReal1D<Double> expr(LELFunctionEnums::CEIL, pExpr);
    DResult = ceil(bDVal);
    if (!checkDouble (expr, DResult, String("LELFunctionReal1D"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function floor" << endl;     
    LELFunctionReal1D<Double> expr(LELFunctionEnums::FLOOR, pExpr);
    DResult = floor(bDVal);
    if (!checkDouble (expr, DResult, String("LELFunctionReal1D"), shape, False, supress)) ok = False;
    }
  }
//
//************************************************************************
//
// LELFunctionFloat; 100% coverage
//
  {
    cout << endl << "LELFunctionFloat" << endl;

    Block<LatticeExprNode> arga(2);
    arga[0] = LatticeExprNode(bF);
    arga[1] = LatticeExprNode(cF);


    {
    cout << "   Function min" << endl;     
    LELFunctionFloat expr(LELFunctionEnums::MIN, arga);
    FResult = min(bFVal,cFVal);
    if (!checkFloat (expr, FResult, String("LELFunctionFloat"), shape, False, supress)) ok = False;
    }


    {
    cout << "   Function max" << endl;     
    LELFunctionFloat expr(LELFunctionEnums::MAX, arga);
    FResult = max(bFVal,cFVal);
    if (!checkFloat (expr, FResult, String("LELFunctionFloat"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function pow" << endl;     
    LELFunctionFloat expr(LELFunctionEnums::POW, arga);
    FResult = pow(bFVal,cFVal);
    if (!checkFloat (expr, FResult, String("LELFunctionFloat"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function atan2" << endl;     
    LELFunctionFloat expr(LELFunctionEnums::ATAN2, arga);
    FResult = atan2(bFVal,cFVal);
    if (!checkFloat (expr, FResult, String("LELFunctionFloat"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function fmod" << endl;     
    LELFunctionFloat expr(LELFunctionEnums::FMOD, arga);
    FResult = fmod(bFVal,cFVal);
    if (!checkFloat (expr, FResult, String("LELFunctionFloat"), shape, False, supress)) ok = False;
    }


    Block<LatticeExprNode> argb(1);
    argb[0] = LatticeExprNode(bC);

    {
    cout << "   Function abs" << endl;     
    LELFunctionFloat expr(LELFunctionEnums::ABS, argb);
    FResult = abs(bCVal);
    if (!checkFloat (expr, FResult, String("LELFunctionFloat"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function arg" << endl;     
    LELFunctionFloat expr(LELFunctionEnums::ARG, argb);
    FResult = Float(arg(bCVal));
    if (!checkFloat (expr, FResult, String("LELFunctionFloat"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function real" << endl;     
    LELFunctionFloat expr(LELFunctionEnums::REAL, argb);
    FResult = real(bCVal);
    if (!checkFloat (expr, FResult, String("LELFunctionFloat"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function imag" << endl;     
    LELFunctionFloat expr(LELFunctionEnums::IMAG, argb);
    FResult = imag(bCVal);
    if (!checkFloat (expr, FResult, String("LELFunctionFloat"), shape, False, supress)) ok = False;
    }
  }
//
//************************************************************************
//
// LELFunctionDouble; 100% coverage
//
  {
    cout << endl << "LELFunctionDouble" << endl;

    Block<LatticeExprNode> arga(2);
    arga[0] = LatticeExprNode(bD);
    arga[1] = LatticeExprNode(cD);

    {
    cout << "   Function min" << endl;     
    LELFunctionDouble expr(LELFunctionEnums::MIN, arga);
    DResult = min(bDVal,cDVal);
    if (!checkDouble(expr, DResult, String("LELFunctionDouble"), shape, False, supress)) ok = False;
    }


    {
    cout << "   Function max" << endl;     
    LELFunctionDouble expr(LELFunctionEnums::MAX, arga);
    DResult = max(bDVal,cDVal);
    if (!checkDouble(expr, DResult, String("LELFunctionDouble"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function pow" << endl;     
    LELFunctionDouble expr(LELFunctionEnums::POW, arga);
    DResult = pow(bDVal,cDVal);
    if (!checkDouble(expr, DResult, String("LELFunctionDouble"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function atan2" << endl;     
    LELFunctionDouble expr(LELFunctionEnums::ATAN2, arga);
    DResult = atan2(bDVal,cDVal);
    if (!checkDouble(expr, DResult, String("LELFunctionDouble"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function fmod" << endl;     
    LELFunctionDouble expr(LELFunctionEnums::FMOD, arga);
    DResult = fmod(bDVal,cDVal);
    if (!checkDouble(expr, DResult, String("LELFunctionDouble"), shape, False, supress)) ok = False;
    }


    Block<LatticeExprNode> argb(1);
    argb[0] = LatticeExprNode(bDC);

    {
    cout << "   Function abs" << endl;     
    DResult = abs(bDCVal);
    LELFunctionDouble expr(LELFunctionEnums::ABS, argb);
    if (!checkDouble(expr, DResult, String("LELFunctionDouble"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function arg" << endl;     
    LELFunctionDouble expr(LELFunctionEnums::ARG, argb);
    DResult = Double(arg(bDCVal));
    if (!checkDouble(expr, DResult, String("LELFunctionDouble"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function real" << endl;     
    LELFunctionDouble expr(LELFunctionEnums::REAL, argb);
    DResult = real(bDCVal);
    if (!checkDouble(expr, DResult, String("LELFunctionDouble"), shape, False, supress)) ok = False;
    }

    {
    cout << "   Function imag" << endl;     
    LELFunctionDouble expr(LELFunctionEnums::IMAG, argb);
    DResult = imag(bDCVal);
    if (!checkDouble(expr, DResult, String("LELFunctionDouble"), shape, False, supress)) ok = False;
    }


    Block<LatticeExprNode> argc(1);
    argc[0] = LatticeExprNode(bB);
    {
    cout << "   Function ntrue" << endl;     
    LELFunctionDouble expr(LELFunctionEnums::NTRUE, argc);
    if (bBVal) {
      DResult = shape.product();
    } else {
      DResult = 0.0;
    }
    if (!checkDouble(expr, DResult, String("LELFunctionDouble"), shape, True, supress)) ok = False;
    }


    {
    cout << "   Function nfalse" << endl;     
    LELFunctionDouble expr(LELFunctionEnums::NFALSE, argc);
    if (!bBVal) {
      DResult = shape.product();
    } else {
      DResult = 0.0;
    }
    if (!checkDouble(expr, DResult, String("LELFunctionDouble"), shape, True, supress)) ok = False;
    }

    {
    cout << "   Function nelements" << endl;     
    LELFunctionDouble expr(LELFunctionEnums::NELEM, argc);
    DResult = shape.product();
    if (!checkDouble(expr, DResult, String("LELFunctionDouble"), shape, True, supress)) ok = False;
    }
  }
//
//************************************************************************
//
// LELFunctionComplex; 100% coverage
//
  {
    cout << endl << "LELFunctionComplex" << endl;

    Block<LatticeExprNode> arga(2);
    arga[0] = LatticeExprNode(bC);
    arga[1] = LatticeExprNode(cC);


    {
    cout << "   Function pow" << endl;     
    LELFunctionComplex expr(LELFunctionEnums::POW, arga);
    CResult = pow(bCVal,cCVal);
    if (!checkComplex(expr, CResult, String("LELFunctionComplex"), shape, False, supress)) ok = False;
    }


    Block<LatticeExprNode> argb(1);
    argb[0] = LatticeExprNode(bC);

    {
    cout << "   Function conj" << endl;     
    LELFunctionComplex expr(LELFunctionEnums::CONJ, argb);
    CResult = conj(bCVal);
    if (!checkComplex(expr, CResult, String("LELFunctionComplex"), shape, False, supress)) ok = False;
    }

  }
//
//************************************************************************
//
// LELFunctionDComplex; 100% coverage
//
  {
    cout << endl << "LELFunctionDComplex" << endl;

    Block<LatticeExprNode> arga(2);
    arga[0] = LatticeExprNode(bDC);
    arga[1] = LatticeExprNode(cDC);


    {
    cout << "   Function pow" << endl;     
    LELFunctionDComplex expr(LELFunctionEnums::POW, arga);
    DCResult = pow(bDCVal,cDCVal);
    if (!checkDComplex(expr, DCResult, String("LELFunctionDComplex"), shape, False, supress)) ok = False;
    }


    Block<LatticeExprNode> argb(1);
    argb[0] = LatticeExprNode(bDC);

    {
    cout << "   Function conj" << endl;     
    LELFunctionDComplex expr(LELFunctionEnums::CONJ, argb);
    DCResult = conj(bDCVal);
    if (!checkDComplex(expr, DCResult, String("LELFunctionDComplex"), shape, False, supress)) ok = False;
    }
  }
//
//************************************************************************
//
// LELFunctionBool; 100% coverage
//
  {
    cout << endl << "LELFunctionBool" << endl;

    Block<LatticeExprNode> arga(1);
    arga[0] = LatticeExprNode(bB);


    {
    cout << "   Function all" << endl;     
    LELFunctionBool expr(LELFunctionEnums::ALL, arga);
    BResult = bBVal;
    if (!checkBool(expr, BResult, String("LELFunctionBool"), shape, True, supress)) ok = False;
    }

    {
    cout << "   Function any" << endl;     
    LELFunctionBool expr(LELFunctionEnums::ANY, arga);
    BResult = bBVal;
    if (!checkBool(expr, BResult, String("LELFunctionBool"), shape, True, supress)) ok = False;
    }
  }
//
//************************************************************************
//
// LELConvert; 100% coverage
//
  {

    {
    cout << endl << "LELConvert<Float,Double> " << endl;
    CountedPtr<LELInterface<Double> > pExpr = new LELLattice<Double>(bD);
    LELConvert<Float,Double> expr(pExpr);
    FResult = Float(bDVal);
    if (!checkFloat (expr, FResult, String("LELConvert"), shape, False, supress)) ok = False;
    }

    {
    cout << "LELConvert<Double,Float> " << endl;
    CountedPtr<LELInterface<Float> > pExpr = new LELLattice<Float>(bF);
    LELConvert<Double,Float> expr(pExpr);
    DResult = Double(bFVal);
    if (!checkDouble(expr, DResult, String("LELConvert"), shape, False, supress)) ok = False;
    }

    {
    cout << "LELConvert<Complex,DComplex> " << endl;
    CountedPtr<LELInterface<DComplex> > pExpr = new LELLattice<DComplex>(bDC);
    LELConvert<Complex,DComplex> expr(pExpr);
    CResult = bDCVal;
    if (!checkComplex(expr, CResult, String("LELConvert"), shape, False, supress)) ok = False;
    }

    {
    cout << "LELConvert<DComplex,Complex> " << endl;
    CountedPtr<LELInterface<Complex> > pExpr = new LELLattice<Complex>(bC);
    LELConvert<DComplex,Complex> expr(pExpr);
    DCResult = bCVal;
    if (!checkDComplex(expr, DCResult, String("LELConvert"), shape, False, supress)) ok = False;
    }


    {
    cout << "LELConvert<Complex,Float> " << endl;
    CountedPtr<LELInterface<Float> > pExpr = new LELLattice<Float>(bF);
    LELConvert<Complex,Float> expr(pExpr);
    CResult = Complex(bFVal,0.0);
    if (!checkComplex(expr, CResult, String("LELConvert"), shape, False, supress)) ok = False;
    }

    {
    cout << "LELConvert<Complex,Double> " << endl;
    CountedPtr<LELInterface<Double> > pExpr = new LELLattice<Double>(bD);
    LELConvert<Complex,Double> expr(pExpr);
    CResult = Complex(bDVal,0.0);
    if (!checkComplex(expr, CResult, String("LELConvert"), shape, False, supress)) ok = False;
    }

    {
    cout << "LELConvert<DComplex,Float> " << endl;
    CountedPtr<LELInterface<Float> > pExpr = new LELLattice<Float>(bF);
    LELConvert<DComplex,Float> expr(pExpr);
    DCResult = DComplex(bFVal,0.0);
    if (!checkDComplex(expr, DCResult, String("LELConvert"), shape, False, supress)) ok = False;
    }

    {
    cout << "LELConvert<DComplex,Double> " << endl;
    CountedPtr<LELInterface<Double> > pExpr = new LELLattice<Double>(bD);
    LELConvert<DComplex,Double> expr(pExpr);
    DCResult = DComplex(bDVal,0.0);
    if (!checkDComplex(expr, DCResult, String("LELConvert"), shape, False, supress)) ok = False;
    }
  }



  if (!ok) {
    cout << "not ok" << endl;
     exit(1);
  } else {
    cout << endl << "ok" << endl;
  }

 } catch (AipsError x) {
    cerr << "aipserror: error " << x.getMesg() << endl;
    exit(1);
 } end_try;
 
   exit(0);
}


Bool checkFloat (LELInterface<Float>& expr, 
                 const Float Result,
                 const String name,
                 const IPosition shape,
                 const Bool shouldBeScalar,
                 const Bool supress)
{
    Array<Float> Arr(shape);
    Bool ok = True;
    IPosition origin(2,0,0);
    PixelBox region(origin, shape-1, shape);


    if (expr.className() != name) {
       cout << "   Class name is wrong" << endl;
       ok = False;
    }

    if (shouldBeScalar) {
      if (!expr.isScalar()) {
         cout << "   Expression is not a scalar but should be" << endl;
         ok = False;
      }
      if (expr.shape() != IPosition()) {
         cout << "   Expression has wrong shape" << endl;
         ok = False;
      }
      if (expr.getScalar() != Result) {
         cout << "   Result should be " << Result << endl;
         cout << "   Result is        " << expr.getScalar() << endl;
         ok = False;
      }
      try {
        expr.eval(Arr, region);
      } catch (AipsError x) {
        if (!supress) cout << "      Caught expected exception; message is: " << x.getMesg() << endl;
      } end_try;
    } else {
      if (expr.isScalar()) {
         cout << "   Expression is a scalar but shouldn't be" << endl;
         ok = False;
      }
      if (expr.shape() != shape) {
         cout << "   Expression has wrong shape" << endl;
         ok = False;
      }
      expr.eval(Arr, region);
      if (!allEQ (Arr, Result)) {
         cout << "   Result should be " << Result << endl;
         cout << "   Result is        " << Arr.ac()(origin) << endl;
         ok = False;
      }
      try {
       expr.getScalar();
      } catch (AipsError x) {
       if (!supress)  cout << "      Caught expected exception; message is: " << x.getMesg() << endl;
      } end_try;
    }
    expr.prepare();
 
    return ok;
}



Bool checkDouble (LELInterface<Double>& expr, 
                 const Double Result,
                 const String name,
                 const IPosition shape,
                 const Bool shouldBeScalar,
                 const Bool supress)
{
    Array<Double> Arr(shape);
    Bool ok = True;
    IPosition origin(2,0,0);
    PixelBox region(origin, shape-1, shape);


    if (expr.className() != name) {
       cout << "   Class name is wrong" << endl;
       ok = False;
    }

    if (shouldBeScalar) {
      if (!expr.isScalar()) {
         cout << "   Expression is not a scalar but should be" << endl;
         ok = False;
      }
      if (expr.shape() != IPosition()) {
         cout << "   Expression has wrong shape" << endl;
         ok = False;
      }
      if (expr.getScalar() != Result) {
         cout << "   Result should be " << Result << endl;
         cout << "   Result is        " << expr.getScalar() << endl;
         ok = False;
      }
      try {
        expr.eval(Arr, region);
      } catch (AipsError x) {
       if (!supress)  cout << "      Caught expected exception; message is: " << x.getMesg() << endl;
      } end_try;
    } else {
      if (expr.isScalar()) {
         cout << "   Expression is a scalar but shouldn't be" << endl;
         ok = False;
      }
      if (expr.shape() != shape) {
         cout << "   Expression has wrong shape" << endl;
         ok = False;
      }
      expr.eval(Arr, region);
      if (!allEQ (Arr, Result)) {
         cout << "   Result should be " << Result << endl;
         cout << "   Result is        " << Arr.ac()(origin) << endl;
         ok = False;
      }
      try {
       expr.getScalar();
      } catch (AipsError x) {
       if (!supress)  cout << "      Caught expected exception; message is: " << x.getMesg() << endl;
      } end_try;
    }
    expr.prepare();
 
    return ok;
}



Bool checkComplex (LELInterface<Complex>& expr, 
                 const Complex Result,
                 const String name,
                 const IPosition shape,
                 const Bool shouldBeScalar,
                 const Bool supress)
{
    Array<Complex> Arr(shape);
    Bool ok = True;
    IPosition origin(2,0,0);
    PixelBox region(origin, shape-1, shape);


    if (expr.className() != name) {
       cout << "   Class name is wrong" << endl;
       ok = False;
    }

    if (shouldBeScalar) {
      if (!expr.isScalar()) {
         cout << "   Expression is not a scalar but should be" << endl;
         ok = False;
      }
      if (expr.shape() != IPosition()) {
         cout << "   Expression has wrong shape" << endl;
         ok = False;
      }
      if (expr.getScalar() != Result) {
         cout << "   Result should be " << Result << endl;
         cout << "   Result is        " << expr.getScalar() << endl;
         ok = False;
      }
      try {
        expr.eval(Arr, region);
      } catch (AipsError x) {
       if (!supress)  cout << "      Caught expected exception; message is: " << x.getMesg() << endl;
      } end_try;
    } else {
      if (expr.isScalar()) {
         cout << "   Expression is a scalar but shouldn't be" << endl;
         ok = False;
      }
      if (expr.shape() != shape) {
         cout << "   Expression has wrong shape" << endl;
         ok = False;
      }
      expr.eval(Arr, region);
      if (!allEQ (Arr, Result)) {
         cout << "   Result should be " << Result << endl;
         cout << "   Result is        " << Arr.ac()(origin) << endl;
         ok = False;
      }
      try {
       expr.getScalar();
      } catch (AipsError x) {
       if (!supress)  cout << "      Caught expected exception; message is: " << x.getMesg() << endl;
      } end_try;
    }
    expr.prepare();
 
    return ok;
}



Bool checkDComplex (LELInterface<DComplex>& expr, 
                 const DComplex Result,
                 const String name,
                 const IPosition shape,
                 const Bool shouldBeScalar,
                 const Bool supress)
{
    Array<DComplex> Arr(shape);
    Bool ok = True;
    IPosition origin(2,0,0);
    PixelBox region(origin, shape-1, shape);


    if (expr.className() != name) {
       cout << "   Class name is wrong" << endl;
       ok = False;
    }

    if (shouldBeScalar) {
      if (!expr.isScalar()) {
         cout << "   Expression is not a scalar but should be" << endl;
         ok = False;
      }
      if (expr.shape() != IPosition()) {
         cout << "   Expression has wrong shape" << endl;
         ok = False;
      }
      if (expr.getScalar() != Result) {
         cout << "   Result should be " << Result << endl;
         cout << "   Result is        " << expr.getScalar() << endl;
         ok = False;
      }
      try {
        expr.eval(Arr, region);
      } catch (AipsError x) {
       if (!supress)  cout << "      Caught expected exception; message is: " << x.getMesg() << endl;
      } end_try;
    } else {
      if (expr.isScalar()) {
         cout << "   Expression is a scalar but shouldn't be" << endl;
         ok = False;
      }
      if (expr.shape() != shape) {
         cout << "   Expression has wrong shape" << endl;
         ok = False;
      }
      expr.eval(Arr, region);
      if (!allEQ (Arr, Result)) {
         cout << "   Result should be " << Result << endl;
         cout << "   Result is        " << Arr.ac()(origin) << endl;
         ok = False;
      }
      try {
       expr.getScalar();
      } catch (AipsError x) {
       if (!supress)  cout << "      Caught expected exception; message is: " << x.getMesg() << endl;
      } end_try;
    }
    expr.prepare();
 
    return ok;
}



Bool checkBool (LELInterface<Bool>& expr, 
                 const Bool Result,
                 const String name,
                 const IPosition shape,
                 const Bool shouldBeScalar,
                 const Bool supress)
{
    Array<Bool> Arr(shape);
    Bool ok = True;
    IPosition origin(2,0,0);
    PixelBox region(origin, shape-1, shape);


    if (expr.className() != name) {
       cout << "   Class name is wrong" << endl;
       ok = False;
    }

    if (shouldBeScalar) {
      if (!expr.isScalar()) {
         cout << "   Expression is not a scalar but should be" << endl;
         ok = False;
      }
      if (expr.shape() != IPosition()) {
         cout << "   Expression has wrong shape" << endl;
         ok = False;
      }
      if (expr.getScalar() != Result) {
         cout << "   Result should be " << Result << endl;
         cout << "   Result is        " << expr.getScalar() << endl;
         ok = False;
      }
      try {
        expr.eval(Arr, region);
      } catch (AipsError x) {
       if (!supress)  cout << "      Caught expected exception; message is: " << x.getMesg() << endl;
      } end_try;
    } else {
      if (expr.isScalar()) {
         cout << "   Expression is a scalar but shouldn't be" << endl;
         ok = False;
      }
      if (expr.shape() != shape) {
         cout << "   Expression has wrong shape" << endl;
         ok = False;
      }
      expr.eval(Arr, region);
      if (!allEQ (Arr, Result)) {
         cout << "   Result should be " << Result << endl;
         cout << "   Result is        " << Arr.ac()(origin) << endl;
         ok = False;
      }
      try {
       expr.getScalar();
      } catch (AipsError x) {
       if (!supress)  cout << "      Caught expected exception; message is: " << x.getMesg() << endl;
      } end_try;
    }
    expr.prepare();
 
    return ok;
}
