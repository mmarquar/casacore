//# tMaskArrMath0.cc: Test program for MaskedArrays mathematical operations.
//# Copyright (C) 1993,1994,1995,1996
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
#include <aips/Utilities/String.h>
#include <aips/Mathematics/Complex.h>
#include <aips/Mathematics/Math.h>
#include <aips/Utilities/Assert.h>

#include <aips/Lattices/IPosition.h>
#include <aips/Arrays/Array.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Arrays/ArrayIO.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Arrays/Cube.h>
#include <aips/Arrays/ArrayError.h>
#include <aips/Arrays/MaskedArray.h>
#include <aips/Arrays/MaskArrMath.h>


main()
{
    try {
        {
            cout << endl << "Testing MaskArrMath0." << endl;

// Math

            {

            Vector<Int> f(10), g(10), h(10);
            Vector<Bool> b(10);

            indgen (f.ac());
            cout << endl << "f=indgen(f) = " << endl;
            cout << f.ac() << endl;


            {

                b = (f.ac()<3);
                cout << endl << "b=(f<3) = " << endl;
                cout << b.ac() << endl;

                MaskedArray<Int> m(h,b);

                h = 0;
                indgen (m);
                cout << endl << "h=0; indgen( m(h,b) ); h= " << endl;
                cout << h << endl;

            }

            {

                cout << endl << "((f>3) && (f<7)) = " << endl;
                cout << ((f.ac()>3) && (f.ac()<7)) << endl;

             //#MaskedArray<Int> m( h( ((f>3) && (f<7)) ) );

                h = 0;
             //#indgen (m);
                indgen ( h( ((f.ac()>3) && (f.ac()<7)) ), 10 );
             //#cout << endl << "h=0; indgen( m( h(f<3) ), 10 ); h= " << endl;
                cout << endl << "h=0; indgen( h( ((f>3) && (f<7)) ), 10 ); h= " << endl;
                cout << h.ac() << endl;

            }

            {

                b = (f.ac()>2);
                cout << endl << "b=(f>2) = " << endl;
                cout << b.ac() << endl;

                MaskedArray<Int> m(h,b);

                h = 1;
                m += 5;
                cout << endl << "h=1; m(h,b) += 5; h= " << endl;
                cout << h << endl;

            }

            {

                b = (f.ac()>2);
                cout << endl << "b=(f>2) = " << endl;
                cout << b.ac() << endl;

                h = 1;
                h(b) += 5;
                cout << endl << "h=1; h(b) += 5; h= " << endl;
                cout << h << endl;

            }

            {

                b = (f.ac()>2);
                cout << endl << "b=(f>2) = " << endl;
                cout << b.ac() << endl;

                MaskedArray<Int> m(h,b);

                h = -1;
                m += f.ac();
                cout << endl << "h=-1; m(h,b) += f; h= " << endl;
                cout << h << endl;

            }

            {

                b = (f.ac()>2);
                cout << endl << "b=(f>2) = " << endl;
                cout << b.ac() << endl;

                h = -1;
                h(b) += f.ac();
                cout << endl << "h=-1; h(b) += f; h= " << endl;
                cout << h << endl;

            }

            {

                b = (f.ac()>4);
                cout << endl << "b=(f>4) = " << endl;
                cout << b.ac() << endl;

                MaskedArray<Int> m(f,b);

                indgen (h.ac());
                cout << endl << "h=indgen(h) = " << endl;
                cout << h << endl;

                h.ac() += m;
                cout << endl << "h += m(f,b); h= " << endl;
                cout << h << endl;

            }

            {

                b = (f.ac()>3);
                cout << endl << "b=(f>3) = " << endl;
                cout << b.ac() << endl;

                Vector<Bool> c(10);
                c = (f.ac()<8);
                cout << endl << "c=(f<8) = " << endl;
                cout << c.ac() << endl;

                MaskedArray<Int> m(h,b), n(f,c);

                h = -1;
                m += n;
                cout << endl << "h=-1; m(h,b) += n(f,c); h= " << endl;
                cout << h << endl;

            }

            {

                b = (f.ac()>3);
                cout << endl << "b=(f>3) = " << endl;
                cout << b.ac() << endl;

                Vector<Bool> c(10);
                c = (f.ac()<8);
                cout << endl << "c=(f<8) = " << endl;
                cout << c.ac() << endl;

                MaskedArray<Int> n(f,c);

                h = -1;
                h(b) += n;
                cout << endl << "h=-1; h(b) += n(f,c); h= " << endl;
                cout << h << endl;

            }


            }

// End Math

            cout << endl << "OK" << endl;
        }
    } catch (AipsError x) {
        cout << "\nCaught an exception: " << x.getMesg() << endl;
    } end_try;

    cout << "OK" << endl;
    return 0;
}
