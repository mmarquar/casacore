//# ClassFileName.cc:  this defines ClassName, which ...
//# Copyright (C) 1997,1998
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

#include <aips/aips.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Arrays/Vector.h>
#include <aips/Exceptions/Error.h>
#include <aips/Exceptions/Excp.h>
#include <aips/Lattices/IPosition.h>
#include <aips/Mathematics/Complex.h>
#include <aips/Mathematics/Math.h>
#include <aips/Utilities/Assert.h>
#include <trial/Lattices/LatticeFFT.h>
#include <trial/Lattices/LatticeIterator.h>
#include <trial/Lattices/PagedArray.h>
#include <iostream.h>

Int main() {
  try {
    {
      const uInt nz = 3;
      const uInt ny = 8;
      const uInt nx = (ny+2)/2;
      const IPosition cShape(3,nx,ny,nz);
      const IPosition rShape(3,ny,ny,nz);
      PagedArray<Complex> cArr(cShape);
      PagedArray<Float> rArr(rShape);
      IPosition centre=cShape/2;
      { // test the fft2d function
	cArr.set(Complex(1,0));
 	LatticeFFT::cfft2d(cArr);
	uInt i;
	for (i = 0; i < nz; i++) {
	  centre(2) = i;
	  AlwaysAssert(near(cArr.getAt(centre), Complex(nx*ny,0), 1E-5),
		       AipsError);
	  cArr.putAt(Complex(0,0), centre);
	}
	
	RO_LatticeIterator<Complex> iter(cArr, 
				       cArr.niceCursorShape(cArr.maxPixels()));
	for (iter.reset(); !iter.atEnd(); iter++) {
	  AlwaysAssert(allNearAbs(iter.cursor(), Complex(0,0), 1E-5),
		       AipsError);
	}
	for (i = 0; i < nz; i++) {
	  centre(2) = i;
	  cArr.putAt(Complex(nx*ny,0), centre);
	}
	LatticeFFT::cfft2d(cArr, False);
	for (iter.reset(); !iter.atEnd(); iter++) {
	  AlwaysAssert(allNearAbs(iter.cursor(), Complex(1,0), 1E-5),
		       AipsError);
	}
      }
      { // test the complex->complex fft function
	cArr.set(Complex(1,0));
	LatticeFFT::cfft(cArr);
	centre(2) = nz/2;
 	AlwaysAssert(near(cArr.getAt(centre), Complex(nx*ny*nz,0), 1E-5),
 		     AipsError);
 	cArr.putAt(Complex(0,0), centre);
	const IPosition tileShape(cArr.niceCursorShape(cArr.maxPixels()));
	{
	  RO_LatticeIterator<Complex> iter(cArr, tileShape);
	  for (iter.reset(); !iter.atEnd(); iter++) {
	    AlwaysAssert(allNearAbs(iter.cursor(), Complex(0,0), 1E-5),
			 AipsError);
	  }
	}
	Vector<Bool> whichAxes(3, True);
	whichAxes(2) = False;
	cArr.putAt(Complex(nx*ny,0), centre);
	LatticeFFT::cfft(cArr, whichAxes, False);
	IPosition planeShape = tileShape;
	planeShape(2) = 1;
	{
	  RO_LatticeIterator<Complex> planeIter(cArr,planeShape);
	  Complex cValue;
	  for (planeIter.reset(); !planeIter.atEnd(); planeIter++) {
	    if (planeIter.position()(2) == centre(2))
	      cValue = Complex(1,0);
	    else
	      cValue = Complex(0,0);
	    AlwaysAssert(allNearAbs(planeIter.cursor(), cValue, 1E-5),
			 AipsError);
	  }
	}
      }
      { // test the real->complex fft function
	rArr.set(1.0);
 	LatticeFFT::rcfft(cArr, rArr);
	centre = cShape/2;
	centre(0) = 0;
	
	AlwaysAssert(near(cArr.getAt(centre), Complex(ny*ny*nz,0), 1E-5),
		       AipsError);
	cArr.putAt(Complex(0,0), centre);
	{
	  RO_LatticeIterator<Complex> iter(cArr, 
				       cArr.niceCursorShape(cArr.maxPixels()));
	  for (iter.reset(); !iter.atEnd(); iter++) {
	    AlwaysAssert(allNearAbs(iter.cursor(), Complex(0,0), 1E-5),
			 AipsError);
	  }
	}
	
	Vector<Bool> whichAxes(3, True);
	whichAxes(2) = False;
	LatticeFFT::rcfft(cArr, rArr, whichAxes, False);
	centre = 0;
	for (uInt i = 0; i < nz; i++) {
	  centre(2) = i;
	  AlwaysAssert(near(cArr.getAt(centre), Complex(ny*ny,0), 1E-5),
		       AipsError);
	  cArr.putAt(Complex(0,0), centre);
	}
	{
	  RO_LatticeIterator<Complex> iter(cArr, 
				       cArr.niceCursorShape(cArr.maxPixels()));
	  for (iter.reset(); !iter.atEnd(); iter++) {
	    AlwaysAssert(allNearAbs(iter.cursor(), Complex(0,0), 1E-5),
			 AipsError);
	  }
	}
      }
      { // test the complex->real fft function
	cArr.set(Complex(1,0));
 	LatticeFFT::crfft(rArr.lc(), cArr.lc());
	centre = rShape/2;
	AlwaysAssert(near(rArr.getAt(centre), 1.0f, 1E-5), AipsError);
	rArr.putAt(0.0f, centre);
	{
	  RO_LatticeIterator<Float> iter(rArr, 
				       rArr.niceCursorShape(rArr.maxPixels()));
	  for (iter.reset(); !iter.atEnd(); iter++) {
	    AlwaysAssert(allNearAbs(iter.cursor(), 0.0f, 1E-5), AipsError);
	  }
	}

	cArr.set(Complex(1,0));
	Vector<Bool> whichAxes(3, True);
	whichAxes(2) = False;
	LatticeFFT::crfft(rArr, cArr, whichAxes, False);
	centre = 0;
	for (uInt i = 0; i < nz; i++) {
	  centre(2) = i;
	  AlwaysAssert(near(rArr.getAt(centre), 1.0f, 1E-5), AipsError);
	  rArr.putAt(0.0f, centre);
	}
 	{
 	  RO_LatticeIterator<Float> iter(rArr, 
 				       rArr.niceCursorShape(rArr.maxPixels()));
 	  for (iter.reset(); !iter.atEnd(); iter++) {
 	    AlwaysAssert(allNearAbs(iter.cursor(), 0.0f, 1E-5), AipsError);
 	  }
 	}
      }
    }
    cout<< "OK"<< endl;
    return 0;
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    cout<< "FAIL"<< endl;
  } end_try;
}
//  	{
//  	  RO_LatticeIterator<Float> iter(rArr, rArr.shape());
//  	  cout << iter.cursor() << endl;
//  	}
// Local Variables: 
// compile-command: "gmake OPTLIB=1 tLatticeFFT"
// End: 
