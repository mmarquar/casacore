//# tMaskArrIO.cc: Test program for MaskedArray IO
//# Copyright (C) 1994,1995,1996,1998
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

//# If AIPS_DEBUG is not set, the Assert's won't be called.
#if !defined(AIPS_DEBUG)
#define AIPS_DEBUG
#endif

//# For extra debugging
#if !defined(AIPS_ARRAY_INDEX_CHECK)
#define AIPS_ARRAY_INDEX_CHECK
#endif

#include <iostream.h>

#include <aips/aips.h>
#include <aips/Arrays/MaskedArray.h>
#include <aips/Arrays/MaskArrMath.h>
#include <aips/Arrays/MaskArrIO.h>
#include <aips/Arrays/Array.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/ArrayIO.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Arrays/Cube.h>
#include <aips/Arrays/ArrayError.h>

int main ()
{
    try {
        cout << endl << "Testing MaskedArray IO." << endl;

        {
//          Vector<Int> a(10,-1);
            Vector<Int> a(10);
            cout << endl;
            for (Int orig0=0; orig0 < 5; orig0++) {
                indgen (a.ac(), orig0);
                cout << "a(a<7)= "
                     << endl
                     << a(a.ac()<7);
                cout << endl;
            }
            cout << endl;
        }

        {
//          Matrix<Int> a(10u,3u,-1, -2);
            Matrix<Int> a(10u,3u);
            cout << endl;
            for (Int orig0=0; orig0 < 5; orig0++) {
                indgen (a.ac(), orig0);
                cout << "a(a<7)= "
                     << endl
                     << a(a.ac()<7);
                cout << endl;
            }
            cout << endl;
        }

        {
//          Cube<Int> a(10,3,4,-1,-2,-3);
            Cube<Int> a(10,3,4);
            cout << endl;
            for (Int orig0=0; orig0 < 5; orig0++) {
                indgen (a.ac(), orig0);
                cout << "a(a<7)= "
                     << endl
                     << a(a.ac()<7);
                cout << endl;
            }
            cout << endl;
        }

    } catch (AipsError x) {
        cout << "\nCaught an exception: " << x.getMesg() << endl;
    } end_try;

    cout << endl << "OK" << endl;
    return 0;
}
