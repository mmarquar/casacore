//# tFITS.cc: This program tests the simple FITS wrappers
//# Copyright (C) 1993,1994,1995,1999
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
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

//# Includes

#if !defined(AIPS_DEBUG)
#define AIPS_DEBUG
#endif

#include <unistd.h>
#include <aips/aips.h>
#include <aips/Utilities/Assert.h>
#include <aips/FITS/FITS.h>
#include <aips/Arrays/Array.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Lattices/IPosition.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Containers/OrderedMap.h>

main()
{
    const char *file = "test.fits";
    Matrix<Float> m(512,512), m2;

    // Create a wedge; rows in a matrix normally are columns in an image,
    // i.e. the result might be the transpose of what you expect.
    for (uInt i=0; i < 512; i++) {
	m.row(i) = float(i);
    }

    // Create the "optional" information
    OrderedMap<String, Double> mapout(0.0), mapin(0.0);
    String unitout, unitin;
    unitout = "Jy";
    Vector<String> namesout(2), namesin(2);
    namesout(0) = "X" ; namesout(1) = "Y";
    Vector<Float> refout(2), refin(2), locout(2), locin(2), deltaout(2),
	deltain(2);
    refout = 0.0f; locout = 1.0f; deltaout = 1.0f;
    mapout("hello") = 1.0;
    mapout("world") = 2.0;
    String objectin, objectout;
    objectout  = "Testing 1.2.3.";

    String message;


    cout << "Writing...." << endl;
    if (WriteFITS(file,m,message, unitout, &namesout, &refout,
		  &locout, &deltaout, &mapout, objectout) == False) {
	cout << "Write failed: " << message << endl;
	return 1;
    }
    Bool ok;

    cout << "Reading.... (will leave test.fits if program fails)" << endl;
    m2 = ReadFITS(file,ok,message, &unitin, &namesin, &refin, &locin, &deltain,
		  &mapin, &objectin);
    if (ok == False) {
	cout << "Read failed: " << message << endl;
	return 1;
    }
    // Could fail just for roundoff reasons, but likely ok
    AlwaysAssertExit(allEQ(m,m2));

    AlwaysAssertExit(unitout == unitin);
    AlwaysAssertExit(allEQ(namesout , namesin));
    AlwaysAssertExit(allEQ(refout , refin));
    AlwaysAssertExit(allEQ(locout , locin));
    AlwaysAssertExit(allEQ(deltaout , deltain));
    AlwaysAssertExit(mapin("HELLO") == 1.0);
    AlwaysAssertExit(mapin("WORLD") == 2.0);
    AlwaysAssertExit(objectout == objectin);

    unlink(file);
    cout << "OK\n";
    return 0;
}
