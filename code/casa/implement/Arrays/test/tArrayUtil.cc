//# tArrayUtil.cc: Test program for functions in ArrayUtil.h
//# Copyright (C) 1995,1996
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

#include <aips/Arrays/ArrayUtil.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/ArrayError.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Arrays/Vector.h>

// <summary>
// Test of functions in ArrayUtil.h.
// </summary>

// This program tests the various functions in ArrayUtil.h.
// When an argument is given to the program, it will not invoke any
// function resulting in an exception. This mode can be used to do
// proper detection of memory leaks using tools like TestCenter.


Bool testVectorToString (Bool)
{
    Bool ok = True;

    Vector<String> vec1 = stringToVector ("");
    if (vec1.nelements() != 0) {
	cout << "Empty string should result in vector length 0" << endl;
	ok = False;
    }

    Vector<String> vec2 = stringToVector ("abc");
    if (vec2.nelements() != 1  ||  vec2(0) != "abc") {
	cout << "<abc> should result in vector length 1" << endl;
	ok = False;
    }

    Vector<String> vec3 = stringToVector (",");
    if (vec3.nelements() != 2  ||  vec3(0) != ""  ||  vec3(1) != "") {
	cout << "<,> should result in vector length 2" << endl;
	ok = False;
    }

    Vector<String> vec4 = stringToVector ("abc,defg,,h");
    if (vec4.nelements() != 4  ||  vec4(0) != "abc"  ||  vec4(1) != "defg"
    ||  vec4(2) != ""  ||  vec4(3) != "h") {
	cout << "<abc,defg,,h> should result in vector length 4" << endl;
	ok = False;
    }

    Vector<String> vec5 = stringToVector (",abc,defg,");
    if (vec5.nelements() != 4  ||  vec5(0) != ""  ||  vec5(1) != "abc"
    ||  vec5(2) != "defg"  ||  vec5(3) != "") {
	cout << "<,abc,defg,> should result in vector length 4" << endl;
	ok = False;
    }

    return ok;
}


Bool testConcatenateArray (Bool doExcp)
{
    Bool ok = True;
    Matrix<Int> matrix1 (3,4);
    Matrix<Int> matrix2 (3,5);
    Matrix<Int> matrix3 (4,4);
    Vector<Int> vector1 (4);
    Vector<Int> vector2 (6);
    indgen (matrix1.ac());
    indgen (matrix2.ac(), Int(matrix1.nelements()));
    indgen (matrix3.ac());
    indgen (vector1.ac());
    indgen (vector2.ac(), Int(vector1.nelements()));
    Matrix<Int> matrixConc = concatenateArray (matrix1.ac(), matrix2.ac());
    if (matrixConc.shape() != IPosition(2,3,9)) {
	cout << "Error in shape of concatenated matrices" << endl;
	ok = False;
    }
    uInt i, j;
    Int value = 0;
    for (j=0; j<9; j++) {
	for (i=0; i<3; i++) {
	    if (matrixConc(i,j) != value++) {
		cout << "Error in matrix on " << i << "," << j << endl;
		ok = False;
	    }
	}
    }

    Vector<Int> vectorConc = concatenateArray (vector1.ac(), vector2.ac());
    if (vectorConc.shape() != IPosition(1,10)) {
	cout << "Error in shape of concatenated vectors" << endl;
	ok = False;
    }
    value = 0;
    for (i=0; i<10; i++) {
	if (vectorConc(i) != value++) {
	    cout << "Error in vector on " << i << endl;
	    ok = False;
	}
    }
    
    if (doExcp) {
	try {
	    concatenateArray (matrix1.ac(), matrix2.ac());
	} catch (ArrayConformanceError x) {
	} end_try;
	try {
	    concatenateArray (matrix1.ac(), vector1.ac());
	} catch (ArrayConformanceError x) {
	} end_try;
    }

    return ok;
}


main (int argc)
{
    Bool ok = True;
    try {
	if (! testVectorToString (ToBool (argc < 2))) {
	    ok = False;
	}
	if (! testConcatenateArray(ToBool (argc < 2))) {
	    ok = False;
	}
    } catch (AipsError x) {
	cout << "Caught an exception: " << x.getMesg() << endl;
	ok = False;
    } end_try;
    if (!ok) {
	return 1;
    }
    cout << "OK" << endl;
    return 0;               // successfully executed
}
