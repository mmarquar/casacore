//# tTempImage.cc: Test program for the TempImage class
//# Copyright (C) 1998,1999
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or(at your option)
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
//# $Id$

#include <trial/Images/TempImage.h>
#include <trial/Images/ImageInfo.h>
#include <trial/Coordinates/CoordinateUtil.h>
#include <trial/Lattices/LatticeIterator.h>
#include <aips/Arrays/Array.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Arrays/IPosition.h>
#include <aips/Quanta/QLogical.h>
#include <aips/Utilities/COWPtr.h>
#include <aips/Utilities/Assert.h>
#include <aips/Exceptions/Error.h>
#include <iostream.h>


void doIt (TempImage<Int>& scratch)
{
    IPosition shape(3,1);    
    shape(2) = scratch.shape()(2);
    AlwaysAssertExit (scratch.isWritable());
    LatticeIterator<Int> li(scratch, shape);
    Int i = 0;
    for (li.reset(); !li.atEnd(); li++, i++) {
	li.woCursor() = i;
    }
    shape = scratch.shape();
    shape(2) = 1;
    COWPtr<Array<Int> > ptrM;
    scratch.getSlice(ptrM, IPosition(3,0), shape, IPosition(3,1), False);
    AlwaysAssert(ptrM->shape().isEqual(shape), AipsError);
    Array<Int> expectedResult(shape);
    indgen(expectedResult);
    AlwaysAssert(allEQ(*ptrM, expectedResult), AipsError);
    ptrM.rwRef() = 0;
    AlwaysAssert(allEQ(*ptrM, 0), AipsError);
    Slicer sl(IPosition(3,0,0,5), shape, IPosition(3,1));
    scratch.getSlice(ptrM, sl, False);
    AlwaysAssert(allEQ(*ptrM, expectedResult), AipsError);
    scratch.set(0);
    scratch.putAt (7, IPosition(3,7));
    AlwaysAssert(scratch.getAt(IPosition(3,0)) == 0, AipsError);
    AlwaysAssert(scratch.getAt(IPosition(3,7)) == 7, AipsError);
//
    scratch.setUnits (Unit("Jy"));
    AlwaysAssert(scratch.units() == Unit("Jy"), AipsError);;
//
    ImageInfo info = scratch.imageInfo();
    AlwaysAssert(info.restoringBeam().nelements()==0, AipsError);
    Quantum<Double> a1(10.0,Unit("arcsec"));
    Quantum<Double> a2(8.0,Unit("arcsec"));
    Quantum<Double> a3(-45.0,Unit("deg"));
    info.setRestoringBeam(a1, a2, a3);
    scratch.setImageInfo(info);
    info = scratch.imageInfo();
    AlwaysAssert(info.restoringBeam()(0)==a1, AipsError);
    AlwaysAssert(info.restoringBeam()(1)==a2, AipsError);
    AlwaysAssert(info.restoringBeam()(2)==a3, AipsError);
}

main()
{
    try {
	{
	    TempImage<Int> scratch(TiledShape(IPosition(3,64,64,257)),
				   CoordinateUtil::defaultCoords3D(),
				   1);
	    AlwaysAssertExit (scratch.isPaged());
	    doIt (scratch);
	}
	{
	    TempImage<Int> small(TiledShape(IPosition(3,64,64,16)),
				 CoordinateUtil::defaultCoords3D(),
				 1);
	    AlwaysAssertExit (small.ok());
	    AlwaysAssertExit (! small.isPaged());
	    doIt (small);
	}
    } catch (AipsError x) {
	cerr << x.getMesg() << endl;
	cout << "FAIL" << endl;
	return 1;
    } end_try;
    cout << "OK" << endl;
    return 0;
}
