//# tImageExprGram.cc: Test program for image expression parser
//# Copyright (C) 1998,1999
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

#include <trial/Images/ImageExprParse.h>
#include <trial/Images/ImageExpr.h>
#include <trial/Images/ImageRegion.h>
#include <trial/Lattices/PagedArray.h>
#include <trial/Lattices/ArrayLattice.h>
#include <trial/Lattices/LCBox.h>
#include <trial/Lattices/LCPixelSet.h>
#include <aips/Tables/Table.h>
#include <aips/Arrays/Array.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Arrays/IPosition.h>
#include <aips/Containers/Block.h>
#include <aips/Glish/GlishRecord.h>
#include <aips/Mathematics/Constants.h>
#include <aips/Logging/LogIO.h>
#include <aips/Inputs/Input.h>
#include <aips/Exceptions/Error.h>
#include <iostream.h>


//# This functions simulates the same function in DOImage2.cc.
String substituteOID (Block<LatticeExprNode>& nodes,
		      String& exprName,
		      const String& expr)
{
  nodes.resize (0, False, True);
  exprName = expr;
  return expr;
}
void makeRegionBlock (PtrBlock<const ImageRegion*>& regions,
		      const GlishRecord&,
		      LogIO& os)
{
   for (uInt j=0; j<regions.nelements(); j++) {
      delete regions[j];
   }
   regions.resize (0, True, True);
}

//# This function is a copy of the expr function in DOImage2.cc, which
//# failed mysteriously on sneffels (RedHat Linux 5.2) in April 1999.
void doExpr (const String& expr, const GlishRecord& regions)
{
  LogIO os(LogOrigin("image", "expr(const String& expr)", WHERE));

// Get LatticeExprNode (tree) from parser
// Convert the GlishRecord containing regions to a
// PtrBlock<const ImageRegion*>.

  if (expr.empty()) {
    os << "You must specify an expression" << LogIO::EXCEPTION;
  }
  Block<LatticeExprNode> temps;
  String exprName;
  String newexpr = substituteOID (temps, exprName, expr);
  PtrBlock<const ImageRegion*> tempRegs;
  makeRegionBlock (tempRegs, regions, os);
  LatticeExprNode node = ImageExprParse::command (newexpr, temps, tempRegs);
  
// Delete the ImageRegions (by using an empty GlishRecord).
  
  makeRegionBlock (tempRegs, GlishRecord(), os);
  
// Make the ImageExpr object.  It will throw an exception if there
// are no true coordinates
  
  LatticeExpr<Float> latEx(node); 
  ImageInterface<Float>* pImage = new ImageExpr<Float>(latEx, exprName);
  if (pImage==0) {
    os << "Failed to create PagedImage" << LogIO::EXCEPTION;
  }
}


main (int argc, char *argv[])
{
 Bool foundError = False;

 try {
    cout << ">>>" << endl;
    Input inp(1);
    inp.version("1.0");
    inp.create("nx", "10", "Number of pixels along the x-axis", "int");
    inp.create("ny", "10", "Number of pixels along the y-axis", "int");
    inp.readArguments(argc, argv);
    cout << "<<<" << endl;

    const uInt nx=inp.getInt("nx");
    const uInt ny=inp.getInt("ny");

    Double aVal = 0.0;
    Double bVal = 1.0;
    Double cVal = 2.0;
    Double dVal = 3.0;
    Double eVal = 4.0;
    Double fVal = 5.0;
    Double gVal = 6.0;
    Double hVal = 7.0;
    Bool aBoolVal = False;
    Bool bBoolVal = False;
    IPosition shape(2, nx, ny);
    ArrayLattice<Double> a(shape);
    ArrayLattice<Bool> aBool(shape);
    a.set(aVal);
    aBool.set(aBoolVal);
    {
	PagedArray<Double> b(shape, "b");
	PagedArray<Double> c(shape, "c");
	PagedArray<Double> d(shape, "d");
	PagedArray<Double> e(shape, "e");
	PagedArray<Double> f(shape, "f");
	PagedArray<Double> g(shape, "g");
	PagedArray<Double> h(shape, "h");
	PagedArray<Bool> bBool(shape, "bBool");
	
	b.set(bVal);
	c.set(cVal);
	d.set(dVal);
	e.set(eVal);
	f.set(fVal);
	g.set(gVal);
	h.set(hVal);
	bBool.set(bBoolVal);
    }

    Array<Double> aArr(shape);
    Array<Bool> aBoolArr(shape);

  {
    cout << endl;
    try {
      doExpr ("xxx", GlishRecord());
    } catch (AipsError x) {
      cout << x.getMesg() << endl;
    } end_try;
    try {
      LatticeExpr<Double> expr (ImageExprParse::command ("b/a1"));
    } catch (AipsError x) {
      cout << x.getMesg() << endl;
    } end_try;
    try {
      LatticeExpr<Double> expr (ImageExprParse::command ("a1/b"));
    } catch (AipsError x) {
      cout << x.getMesg() << endl;
    } end_try;
    try {
      LatticeExpr<Double> expr (ImageExprParse::command ("b/b*"));
    } catch (AipsError x) {
      cout << x.getMesg() << endl;
    } end_try;
    try {
      LatticeExpr<Double> expr (ImageExprParse::command ("min(b,b,b)"));
    } catch (AipsError x) {
      cout << x.getMesg() << endl;
    } end_try;
  }
  {
    cout << endl;
    cout << "Expr:  a = 1" << endl;
    LatticeExpr<Double> expr (ImageExprParse::command ("1.0"));
    a.copyData(expr);
    a.getSlice(aArr, IPosition(aArr.ndim(),0), 
	       shape, IPosition(aArr.ndim(),1));
    Double result = 1.0;
    if (! allEQ (aArr, result)) {
	cout << "Result should be " << result << endl;
	cout << "Result is " << aArr << endl;
	foundError = True;
    }
  }
  {
    cout << "Expr:  a = b" << endl;
    LatticeExpr<Double> expr(ImageExprParse::command ("b"));
    a.copyData(expr);
    a.getSlice(aArr, IPosition(aArr.ndim(),0), 
	       shape, IPosition(aArr.ndim(),1));
    Double result = bVal;
    if (! allEQ (aArr, result)) {
	cout << "Result should be " << result  << endl;
	cout << "Result is " << aArr << endl;
	foundError = True;
    }
  }
  {
    cout << "Expr:  a = sin(\\c)" << endl;
    LatticeExpr<Double> expr(ImageExprParse::command ("sin(\\c)"));
    a.copyData(expr);
    a.getSlice(aArr, IPosition(aArr.ndim(),0), 
	       shape, IPosition(aArr.ndim(),1));
    Double result = sin(cVal);
    if (! allEQ (aArr, sin(cVal))) {
	cout << "Result should be " << result << endl;
	cout << "Result is " << aArr << endl;
	foundError = True;
    }
  }

  {
    cout << "Expr:  a = 'c'+2" << endl;
    LatticeExpr<Double> expr(ImageExprParse::command ("'c'+2"));
    a.copyData(expr);
    a.getSlice(aArr, IPosition(aArr.ndim(),0), 
	       shape, IPosition(aArr.ndim(),1));
    Double result = cVal+2;
    if (! allEQ (aArr, result)) {
	cout << "Result should be " << result << endl;
	cout << "Result is " << aArr << endl;
	foundError = True;
    }
  }
  {
    cout << "Expr:  a = b+('c'+d)[region]     (using $n notation)" << endl;
    PagedArray<Double> b("b");
    PagedArray<Double> c("c");
    PagedArray<Double> d("d");
    Block<LatticeExprNode> temps(3);
    temps[0] = LatticeExprNode(b);
    temps[1] = LatticeExprNode(c);
    temps[2] = LatticeExprNode(d);
    PtrBlock<const ImageRegion*> regions(1);
    regions[0] = new ImageRegion(LCBox(shape));
    LatticeExpr<Double> expr(ImageExprParse::command ("$1+($2+$3)[$REGION#1]",
						      temps, regions));
    delete regions[0];
    a.copyData(expr);
    a.getSlice(aArr, IPosition(aArr.ndim(),0), 
	       shape, IPosition(aArr.ndim(),1));
    Double result = bVal + cVal + dVal;
    if (! allEQ (aArr, result)) {
	cout << "Result should be " << result << endl;
	cout << "Result is " << aArr << endl;
	foundError = True;
    }
  }
  {
    cout << "Expr:  a = nelements(b[$region]" << endl;
    Block<LatticeExprNode> temps(0);
    PtrBlock<const ImageRegion*> regions(1);
    Matrix<Bool> mask(shape-1);
    mask = False;
    mask(0,0) = True;
    regions[0] = new ImageRegion(LCPixelSet(mask,LCBox(IPosition(2,0),
						       shape-2, shape)));
    LatticeExpr<Double> expr(ImageExprParse::command ("nelements(b[$REGION#1])",
						      temps, regions));
    delete regions[0];
    a.copyData(expr);
    a.getSlice(aArr, IPosition(aArr.ndim(),0), 
	       shape, IPosition(aArr.ndim(),1));
    Double result = 1;
    if (! allEQ (aArr, result)) {
	cout << "Result should be " << result << endl;
	cout << "Result is " << aArr << endl;
	foundError = True;
    }
  }
  {
    cout << "Expr:  a = nelements(b[$region1 || $region2) - "
	    "length(b[$region1],1)" << endl;
    Block<LatticeExprNode> temps(0);
    PtrBlock<const ImageRegion*> regions(2);
    Matrix<Bool> mask1(shape-1);
    Matrix<Bool> mask2(shape-1);
    mask1 = False;
    mask2 = False;
    mask1(0,0) = True;
    mask2(shape-2) = True;
    regions[0] = new ImageRegion(LCPixelSet(mask1,LCBox(IPosition(2,0),
							shape-2, shape)));
    regions[1] = new ImageRegion(LCPixelSet(mask2,LCBox(IPosition(2,0),
							shape-2, shape)));
    LatticeExpr<Double> expr(ImageExprParse::command
			     ("nelements(b[$REGION#1 || $REGION#2]) - "
			      "length(b[$REGION#1],1)",
			      temps, regions));
    delete regions[0];
    delete regions[1];
    a.copyData(expr);
    a.getSlice(aArr, IPosition(aArr.ndim(),0), 
	       shape, IPosition(aArr.ndim(),1));
    Double result = 2 - (shape(0)-1);
    if (! allEQ (aArr, result)) {
	cout << "Result should be " << result << endl;
	cout << "Result is " << aArr << endl;
	foundError = True;
    }
  }
  {
    cout << "Expr:  a = 3.5*b + cos('c')-10/min('c',d)*-e*log(b)-pi()" << endl;

    LatticeExpr<Double> expr( ImageExprParse::command
               ("(3.5*b) + (cos('c')) - (10/min('c',d)*(-e)*log(b)) - (pi()) "));
    a.copyData(expr);
    a.getSlice(aArr, IPosition(aArr.ndim(),0), 
	       shape, IPosition(aArr.ndim(),1));
    Double result = 3.5*bVal + cos(cVal) -
	            10/min(cVal,dVal)*-eVal*log(bVal) - C::pi;
    if (! allNear (aArr, result, 1.0e-10)) {
	cout << "Result should be " << result << endl;
	cout << "Result is " << aArr << endl;
	foundError = True;
    }
  }
  {
    cout << "Expr:  a = (b+'c'-d/2.0*-b) + pi()" << endl;

    LatticeExpr<Double> expr(ImageExprParse::command ("(b+'c'-d/2.0*-b)+pi()"));
    a.copyData(expr);
    a.getSlice(aArr, IPosition(aArr.ndim(),0), 
	       shape, IPosition(aArr.ndim(),1));
    Double result = (bVal + cVal - dVal / 2*-bVal) + C::pi;
    if (! allEQ (aArr, result)) {
	cout << "Result should be " << result << endl;
	cout << "Result is " << aArr << endl;
	foundError = True;
    }
  }
  {
    cout << "Expr:  a = pow('c',d)" << endl;

    LatticeExpr<Double> expr(ImageExprParse::command ("pow('c',d)"));
    a.copyData(expr);
    a.getSlice(aArr, IPosition(aArr.ndim(),0), 
	       shape, IPosition(aArr.ndim(),1));
    Double result = pow(cVal,dVal);
    if (! allEQ (aArr, result)) {
	cout << "Result should be " << result << endl;
	cout << "Result is " << aArr << endl;
	foundError = True;
    }
  }
  {
    cout << "Expr:  a = fmod('c'*2,d)" << endl;

    LatticeExpr<Double> expr(ImageExprParse::command ("fmod('c'*2,d)"));
    a.copyData(expr);
    a.getSlice(aArr, IPosition(aArr.ndim(),0), 
	       shape, IPosition(aArr.ndim(),1));
    Double result = fmod(cVal*2,dVal);
    if (! allEQ (aArr, result)) {
	cout << "Result should be " << result << endl;
	cout << "Result is " << aArr << endl;
	foundError = True;
    }
  }
  {
    cout << "Expr:  a = pow('c',2.3)" << endl;

    LatticeExpr<Double> expr(ImageExprParse::command ("pow('c',2.3)"));
    a.copyData(expr);
    a.getSlice(aArr, IPosition(aArr.ndim(),0), 
	       shape, IPosition(aArr.ndim(),1));
    Double result = pow(cVal,2.3);
    if (! allEQ (aArr, result)) {
	cout << "Result should be " << result << endl;
	cout << "Result is " << aArr << endl;
	foundError = True;
    }
  }
  {
    cout << "Expr:  a = cos(b)" << endl;

    LatticeExpr<Double> expr(ImageExprParse::command ("cos(b)"));
    a.copyData(expr);
    a.getSlice(aArr, IPosition(aArr.ndim(),0), 
	       shape, IPosition(aArr.ndim(),1));
    Double result = cos(bVal);
    if (! allEQ (aArr, result)) {
	cout << "Result should be " << result << endl;
	cout << "Result is " << aArr << endl;
	foundError = True;
    }
  }
  {
    cout << "Expr:  a = cos(sin(b))" << endl;

    LatticeExpr<Double> expr (ImageExprParse::command ("cos(sin(b))"));
    a.copyData(expr);
    a.getSlice(aArr, IPosition(aArr.ndim(),0), 
	       shape, IPosition(aArr.ndim(),1));
    Double result = cos(sin(bVal));
    if (! allEQ (aArr, result)) {
	cout << "Result should be " << result << endl;
	cout << "Result is " << aArr << endl;
	foundError = True;
    }
  }
  {
    cout << "Expr:  a = ntrue(b>=1)" << endl;

    LatticeExpr<Double> expr(ImageExprParse::command ("ntrue(b>=1)"));
    a.copyData(expr);
    a.getSlice(aArr, IPosition(aArr.ndim(),0), 
	       shape, IPosition(aArr.ndim(),1));
    Double result = shape.product();
    if (! allEQ (aArr, result)) {
	cout << "Result should be " << result << endl;
	cout << "Result is " << aArr << endl;
	foundError = True;
    }
  }
  {
    cout << "Expr:  aBool = T||F" << endl;

    LatticeExpr<Bool> expr(ImageExprParse::command ("T||F"));
    aBool.copyData(expr);
    aBool.getSlice(aBoolArr, IPosition(aBoolArr.ndim(),0), 
		   shape, IPosition(aBoolArr.ndim(),1));
    Bool result = ToBool(True||False);
    if (! allEQ (aBoolArr, result)) {
	cout << "Result should be " << result << endl;
	cout << "Result is " << aBoolArr << endl;
	foundError = True;
    }
  }
  {
    cout << "Expr:  aBool = !bBool" << endl;

    LatticeExpr<Bool> expr(ImageExprParse::command ("!bBool"));
    aBool.copyData(expr);
    aBool.getSlice(aBoolArr, IPosition(aBoolArr.ndim(),0), 
		   shape, IPosition(aBoolArr.ndim(),1));
    Bool result = ToBool(!bBoolVal);
    if (! allEQ (aBoolArr, result)) {
	cout << "Result should be " << result << endl;
	cout << "Result is " << aBoolArr << endl;
	foundError = True;
    }
  }
  {
    cout << "Expr:  a = sum(e)/nelements(b) + min('c') + max('c', mean('c'+d))" << endl;

    LatticeExpr<Double> expr(ImageExprParse::command
	          ("sum(e)/nelements(b) + min('c') + max('c', mean('c'+d))"));
    a.copyData(expr);
    a.getSlice(aArr, IPosition(aArr.ndim(),0), 
	       shape, IPosition(aArr.ndim(),1));
    Double result = eVal+cVal+(cVal+dVal);
    if (! allEQ (aArr, result)) {
	cout << "Result should be " << result << endl;
	cout << "Result is " << aArr << endl;
	foundError = True;
    }
  }
  {
    cout << "Expr:  a = sum(e[bBool])" << endl;

    LatticeExpr<Double> expr(ImageExprParse::command
	          ("sum(e[bBool])"));
    a.copyData(expr);
    a.getSlice(aArr, IPosition(aArr.ndim(),0), 
	       shape, IPosition(aArr.ndim(),1));
    Double result = (bBoolVal  ?  aArr.nelements()*eVal : 0);
    if (! allEQ (aArr, result)) {
	cout << "Result should be " << result << endl;
	cout << "Result is " << aArr << endl;
	foundError = True;
    }
  }
  {
    cout << "Expr:  a = sum((e + sum(f[b>0]*c)/d)[!bBool])" << endl;

    LatticeExpr<Double> expr(ImageExprParse::command
	          ("sum((e + sum(f[b>0]*c)/d)[!bBool])"));
    a.copyData(expr);
    a.getSlice(aArr, IPosition(aArr.ndim(),0), 
	       shape, IPosition(aArr.ndim(),1));
    Double result = (bVal>0  ?  aArr.nelements()*fVal*cVal/dVal : 0);
    result = (bBoolVal  ?  0 : aArr.nelements()*(eVal+result));
    if (! allNear (aArr, result, 1e-10)) {
	cout << "Result should be " << result << endl;
	cout << "Result is " << aArr << endl;
	foundError = True;
    }
  }
  {
    {
	PagedArray<Double> ap (shape, "a");
	ap.copyData (a);
    }
    cout << "Expr:  a = min(a+10,5)" << endl;

    LatticeExpr<Double> expr(ImageExprParse::command ("min(a+10,5)"));
    a.copyData(expr);
    a.getSlice(aArr, IPosition(aArr.ndim(),0), 
	       shape, IPosition(aArr.ndim(),1));
    Double result = 5;
    if (! allEQ (aArr, result)) {
	cout << "Result should be " << result << endl;
	cout << "Result is " << aArr << endl;
	foundError = True;
    }
  }

  cout << endl;

 } catch (AipsError x) {
    cerr << "aipserror: error " << x.getMesg() << endl;
    foundError = True;
 } end_try;

 // Delete all created tables (if they exist).
 Table tab; 
 if (Table::isReadable("a")) tab =  Table ("a", Table::Delete);
 if (Table::isReadable("b")) tab =  Table ("b", Table::Delete);
 if (Table::isReadable("c")) tab =  Table ("c", Table::Delete);
 if (Table::isReadable("d")) tab =  Table ("d", Table::Delete);
 if (Table::isReadable("e")) tab =  Table ("e", Table::Delete);
 if (Table::isReadable("f")) tab =  Table ("f", Table::Delete);
 if (Table::isReadable("g")) tab =  Table ("g", Table::Delete);
 if (Table::isReadable("h")) tab =  Table ("h", Table::Delete);
 if (Table::isReadable("bBool")) tab =  Table ("bBool", Table::Delete);

  if (foundError) {
     return 1;
  }
 return 0;
}
