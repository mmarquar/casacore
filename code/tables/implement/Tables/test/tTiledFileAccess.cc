//# tTiledFileAccess.h: Test program for class TiledFileAccess
//# Copyright (C) 2001,2002
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


#include <trial/Tables/TiledFileAccess.h>
#include <aips/Utilities/String.h>
#include <aips/Arrays/IPosition.h>
#include <aips/Exceptions/Error.h>
#include <aips/Arrays/Array.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Arrays/ArrayIO.h>
#include <aips/Arrays/Slicer.h>
#include <aips/IO/CanonicalIO.h>
#include <aips/IO/RawIO.h>
#include <aips/IO/RegularFileIO.h>
#include <aips/OS/RegularFile.h>
#include <aips/Utilities/Assert.h>
#include <aips/iostream.h>


int main()
{
  // Test for a Float array written in canonical format starting
  // at offset 0.
  {
    IPosition shape(2,16,32);
    Array<Float> arr(shape);
    indgen(arr);
    {
      Bool deleteIt;
      const Float* dataPtr = arr.getStorage (deleteIt);
      RegularFileIO fios(RegularFile("tTiledFileAccess_tmp.dat"), ByteIO::New);
      CanonicalIO ios (&fios);
      ios.write (shape.product(), dataPtr);
      arr.freeStorage (dataPtr, deleteIt);
    }
    try {
      TiledFileAccess tfa ("tTiledFileAccess_tmp.dat", 0, shape,
			   IPosition(2,16,1), TpFloat);
      AlwaysAssertExit (tfa.shape() == shape);
      AlwaysAssertExit (tfa.tileShape() == IPosition(2,16,1));
      AlwaysAssertExit (! tfa.isWritable());
      AlwaysAssertExit (tfa.maximumCacheSize() == 0);
      cout << tfa.cacheSize() << endl;
      tfa.setMaximumCacheSize (100000);
      AlwaysAssertExit (tfa.maximumCacheSize() == 100000);
      
      AlwaysAssertExit (allEQ (arr, tfa.getFloat (Slicer(IPosition(2,0,0),
							 shape))));
      tfa.showCacheStatistics (cout);
      cout << tfa.cacheSize() << endl;
    } catch (AipsError x) {
      cout << "Exception: " << x.getMesg() << endl;
      return 1;
    }
  }

  // Test for a Float array written in local format starting
  // at offset 1. The tile size is half the length of the first axis.
  {
    IPosition shape(2,32,16);
    Array<Float> arr(shape);
    indgen(arr);
    uInt off2;
    {
      Bool deleteIt;
      const Float* dataPtr = arr.getStorage (deleteIt);
      RegularFileIO fios(RegularFile("tTiledFileAccess_tmp.dat"), ByteIO::New);
      RawIO ios (&fios);
      uChar nr  = 0;
      off2 = ios.write (1, &nr);
      ios.write (shape.product(), dataPtr);
      arr.freeStorage (dataPtr, deleteIt);
    }
    try {
      // The array starts at offset off2.
      TiledFileAccess tfa ("tTiledFileAccess_tmp.dat", off2, shape,
			   IPosition(2,16,1), TpFloat, 100, False, False);
      AlwaysAssertExit (tfa.shape() == shape);
      AlwaysAssertExit (tfa.tileShape() == IPosition(2,16,1));
      AlwaysAssertExit (! tfa.isWritable());
      AlwaysAssertExit (tfa.maximumCacheSize() == 100);
      cout << tfa.cacheSize() << endl;
      tfa.setMaximumCacheSize (100000);
      AlwaysAssertExit (tfa.maximumCacheSize() == 100000);
      
      AlwaysAssertExit (allEQ (arr, tfa.getFloat (Slicer(IPosition(2,0,0),
							 shape))));
      tfa.showCacheStatistics (cout);
      cout << tfa.cacheSize() << endl;
    } catch (AipsError x) {
      cout << "Exception: " << x.getMesg() << endl;
      return 1;
    }
  }

  // Test for a DComplex array written in canonical and in local format.
  // Open it writable and update the values.
  // Check it after writing by iterating through the data.
  {
    IPosition shape(2,17,40);
    Array<DComplex> arr(shape);
    indgen(arr);
    uInt off2;
    {
      Bool deleteIt;
      const DComplex* dataPtr = arr.getStorage (deleteIt);
      RegularFileIO fios(RegularFile("tTiledFileAccess_tmp.dat"), ByteIO::New);
      CanonicalIO ios (&fios);
      off2 = ios.write (shape.product(), dataPtr);
      RawIO cios (&fios);
      cios.write (shape.product(), dataPtr);
      arr.freeStorage (dataPtr, deleteIt);
    }
    try {
      Slicer slicer (IPosition(2,0,0), shape);
      TiledFileAccess tfac ("tTiledFileAccess_tmp.dat", 0, shape,
			    IPosition(2,17,1), TpDComplex, 0, True);
      AlwaysAssertExit (allEQ (arr, tfac.getDComplex (slicer)));
      AlwaysAssertExit (tfac.shape() == shape);
      AlwaysAssertExit (tfac.tileShape() == IPosition(2,17,1));
      TiledFileAccess tfal ("tTiledFileAccess_tmp.dat", off2, shape,
			    IPosition(2,17,1), TpDComplex, 0, True, False);
      AlwaysAssertExit (allEQ (arr, tfal.getDComplex (slicer)));
      tfac.put (tfac.getDComplex(slicer) + DComplex(1,2), slicer);
      tfal.put (tfal.getDComplex(slicer) + DComplex(3,5), slicer);
    } catch (AipsError x) {
      cout << "Exception: " << x.getMesg() << endl;
      return 1;
    }
    try {
      TiledFileAccess tfac ("tTiledFileAccess_tmp.dat", 0, shape,
			    IPosition(2,17,1), TpDComplex, 0, True);
      AlwaysAssertExit (tfac.shape() == shape);
      AlwaysAssertExit (tfac.tileShape() == IPosition(2,17,1));
      TiledFileAccess tfal ("tTiledFileAccess_tmp.dat", off2, shape,
			    IPosition(2,17,1), TpDComplex, 0, True, False);
      IPosition st(2,0,0);
      IPosition end(2,15,0);
      IPosition leng(2,16,1);
      for (Int i=0; i<shape(0); i++) {
	st(1) = i;
	end(1) = i;
	AlwaysAssertExit (allEQ (arr(st,end) + DComplex(1,2),
				 tfac.getDComplex (Slicer(st,leng))));
	AlwaysAssertExit (allEQ (arr(st,end) + DComplex(3,5),
				 tfal.getDComplex (Slicer(st,leng))));
      }
      cout << end << endl;
    } catch (AipsError x) {
      cout << "Exception: " << x.getMesg() << endl;
      return 1;
    }
  }

  // Test for a Short array written in canonical format.
  // Read it also back as Float with a scale and offset.
  {
    IPosition shape(2,17,40);
    Array<Short> arrs(shape);
    Array<Float> arrf(shape);
    Float scale = 2;
    Float offset = -10;
    indgen(arrs);
    indgen(arrf, float(-10), float(2));
    {
      Bool deleteIt;
      const Short* dataPtr = arrs.getStorage (deleteIt);
      RegularFileIO fios(RegularFile("tTiledFileAccess_tmp.dat"), ByteIO::New);
      CanonicalIO ios (&fios);
      ios.write (shape.product(), dataPtr);
      arrs.freeStorage (dataPtr, deleteIt);
    }
    try {
      Slicer slicer (IPosition(2,0,0), shape);
      TiledFileAccess tfac ("tTiledFileAccess_tmp.dat", 0, shape,
			    IPosition(2,17,4), TpShort, 0, True);
      AlwaysAssertExit (allEQ (arrs, tfac.getShort (slicer)));
      AlwaysAssertExit (allEQ (arrf, tfac.getFloat (slicer, scale, offset,
						    short(-32768))));
      AlwaysAssertExit (tfac.shape() == shape);
      AlwaysAssertExit (tfac.tileShape() == IPosition(2,17,4));
    } catch (AipsError x) {
      cout << "Exception: " << x.getMesg() << endl;
      return 1;
    }
  }

  // Test the tileShape function in various ways.
  {
    try {
      cout << TiledFileAccess::makeTileShape (IPosition(2,17,40)) << endl;
      cout << TiledFileAccess::makeTileShape (IPosition(2,17,40), 17) << endl;
      cout << TiledFileAccess::makeTileShape (IPosition(2,17,40), 34) << endl;
      cout << TiledFileAccess::makeTileShape (IPosition(2,17,40), 33) << endl;
      cout << TiledFileAccess::makeTileShape (IPosition(2,17,40), 15) << endl;
      cout << TiledFileAccess::makeTileShape (IPosition(2,17,40), 3) << endl;
    } catch (AipsError x) {
      cout << "Exception: " << x.getMesg() << endl;
      return 1;
    }
  }
  return 0;
}
