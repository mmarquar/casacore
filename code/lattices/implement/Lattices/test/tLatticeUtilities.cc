//# tLatticeUtilities.cc:
//# Copyright (C) 1997,1998,1999,2000,2001
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
#include <aips/Exceptions/Error.h>
#include <aips/Arrays/IPosition.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Mathematics/Math.h>
#include <aips/Logging/LogIO.h>
#include <aips/Logging/LogOrigin.h>
#include <aips/Utilities/Assert.h>
#include <aips/Lattices/TempLattice.h>
#include <trial/Lattices/LatticeUtilities.h>
#include <trial/Lattices/SubLattice.h>
#include <trial/Lattices/LatticeExprNode.h>
#include <trial/Lattices/LatticeExpr.h>
#include <aips/Lattices/ArrayLattice.h>

#include <aips/iostream.h>

void doMinMax();
void doCollapse();
void doCopy();

int main()
{
  try {
    
// minMax

     doMinMax();

// Collapse

     doCollapse();

// Copy

     doCopy();

  } catch (AipsError x) {
    cout<< "FAIL"<< endl;
    cerr << x.getMesg() << endl;
    return 1;
  } 
  cout<< "OK"<< endl;
  return 0;
};




void doMinMax()
{
    cerr << "minMax " << endl;
    TempLattice<Float> lat(IPosition(3,512,512,10), 1.0);
    lat.set(0.0);
    lat.putAt( 1.0, IPosition(3, 10, 10, 0) );
    lat.putAt( -1.0, IPosition(3, 20, 20, 2) );
    lat.putAt( 1.0, IPosition(3, 500, 400, 3) );
    lat.putAt( -1.0, IPosition(3, 502, 490, 4) );
    lat.putAt( 2.0, IPosition(3, 400, 500, 5) );
    lat.putAt( -2.0, IPosition(3, 10, 400, 6) );
    lat.putAt( 3.0, IPosition(3, 400, 100, 7) );
    lat.putAt( -3.0, IPosition(3, 500, 100, 8) );

    Float lmin, lmax;
    IPosition lminPos(3, 0);
    IPosition lmaxPos(3, 0);

    minMax(lmin, lmax, lminPos, lmaxPos, lat);

    IPosition trueMaxPos = IPosition(3, 400, 100, 7);
    IPosition trueMinPos = IPosition(3, 500, 100, 8);
    AlwaysAssert(trueMaxPos == lmaxPos && lmax == 3.0, AipsError);
    AlwaysAssert(trueMinPos == lminPos && lmin == -3.0, AipsError);
}


void doCollapse ()
{
    cerr << "Collapse" << endl;
    IPosition shape(3, 10, 20, 30);
    ArrayLattice<Float> latIn(shape);
    latIn.set(1.0);
    Array<Float> data;
    Array<Bool> mask;
    IPosition axes(2); axes(0) = 1; axes(1) = 2;

// Unmasked input

    {       
       cerr << "  Unmasked" << endl;
       SubLattice<Float> mLatIn(latIn);
//
       LatticeUtilities::collapse (data, axes, mLatIn, True);
       AlwaysAssert(data.ndim()==1, AipsError);       
       AlwaysAssert(data.shape()(0)==shape(0), AipsError);
       AlwaysAssert(allNear(data, Float(1.0), 1.0e-6), AipsError);
//
       LatticeUtilities::collapse (data, mask, axes, mLatIn, True, True, True);
       AlwaysAssert(data.ndim()==1, AipsError);       
       AlwaysAssert(mask.ndim()==1, AipsError);
       AlwaysAssert(data.shape()(0)==shape(0), AipsError);
       AlwaysAssert(mask.shape()(0)==shape(0), AipsError);
       AlwaysAssert(allNear(data, Float(1.0), 1.0e-6), AipsError);
       AlwaysAssert(allEQ(mask, True), AipsError);
    }

// Masked Input

    {       
       cerr << "  Masked" << endl;
       SubLattice<Float> mLatIn(latIn);
//
       ArrayLattice<Bool> maskLat(shape);
       maskLat.set(True);       
       mLatIn.setPixelMask(maskLat, True);
//
       LatticeUtilities::collapse (data, axes, mLatIn, True);
       AlwaysAssert(data.ndim()==1, AipsError);       
       AlwaysAssert(data.shape()(0)==shape(0), AipsError);
       AlwaysAssert(allNear(data, Float(1.0), 1.0e-6), AipsError);
//
       LatticeUtilities::collapse (data, mask, axes, mLatIn, True, True, True);
       AlwaysAssert(data.ndim()==1, AipsError);       
       AlwaysAssert(mask.ndim()==1, AipsError);
       AlwaysAssert(data.shape()(0)==shape(0), AipsError);
       AlwaysAssert(mask.shape()(0)==shape(0), AipsError);
       AlwaysAssert(allNear(data, Float(1.0), 1.0e-6), AipsError);
       AlwaysAssert(allEQ(mask, True), AipsError);
    }
}



void doCopy ()
{
    cerr << "copyDataAndMask" << endl;
    LogIO os(LogOrigin("tLatticeUtilities", "doCopy", WHERE));
//
    IPosition shape(2, 5, 10);
    IPosition pos(2,0);
    ArrayLattice<Float> latIn(shape);
    latIn.set(1.0);
    SubLattice<Float> mLatIn(latIn, True);
    ArrayLattice<Bool> maskIn(shape);
    maskIn.set(True);
    mLatIn.setPixelMask(maskIn, True);

// Unmasked output

    {       
       cerr << "  Unmasked output" << endl;
//
       ArrayLattice<Float> latOut(shape);
       SubLattice<Float> mLatOut(latOut, True);
//
       LatticeUtilities::copyDataAndMask (os, mLatOut, mLatIn, False);
       AlwaysAssert(allNear(mLatOut.get(), Float(1.0), 1.0e-6), AipsError);
       AlwaysAssert(allEQ(mLatOut.getMask(), True), AipsError);
    }
//
    {       
       cerr << "  Masked output" << endl;
//
       ArrayLattice<Float> latOut(shape);
       SubLattice<Float> mLatOut(latOut, True);
       ArrayLattice<Bool> latMaskOut(shape);       
       latMaskOut.set(False);
       mLatOut.setPixelMask(latMaskOut, True);
//
       LatticeUtilities::copyDataAndMask (os, mLatOut, mLatIn, False);
       AlwaysAssert(allNear(mLatOut.get(), Float(1.0), 1.0e-6), AipsError);
       AlwaysAssert(allEQ(mLatOut.getMask(), True), AipsError);

// Now set one mask value to False so the output pixel should be zero


       Lattice<Bool>& pixelMaskIn = mLatIn.pixelMask();
       pixelMaskIn.set(True);
       pixelMaskIn.putAt(False,pos);
       LatticeUtilities::copyDataAndMask (os, mLatOut, mLatIn, True);
//
       {
          Array<Float> dataOut = mLatOut.get();
          Array<Bool> maskOut = mLatOut.getMask();
          AlwaysAssert(near(dataOut(pos), Float(0.0), 1.0e-6), AipsError);
          AlwaysAssert(maskOut(pos)==False, AipsError);
       }
   }
}

