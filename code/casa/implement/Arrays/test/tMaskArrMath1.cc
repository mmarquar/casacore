//# tMaskArrMath1.cc: Test program for MaskedArrays mathematical operations.
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
            cout << endl << "Testing MaskArrMath1." << endl;

// Math

            {

            Vector<Double> df(10), dg(10), dh(10);
            Vector<Bool> b(10);

            indgen (df.ac());
            cout << endl << "df=indgen(df) = " << endl;
            cout << df.ac() << endl;

            indgen (dg.ac(), 2.0);
            cout << endl << "dg=indgen(dg) = " << endl;
            cout << dg.ac() << endl;

            {
                cout << endl
                     << "Test cos (MaskedArray<Double>)"
                     << endl;
                dh = 2.0;
                dh = cos (df ((df.ac() > 2.5) && (df.ac() < 6.5)));
                cout << endl << "dh=2.0; "
                     << endl
                     << "dh = cos (df ((df > 2.5) && (df < 6.5)));"
                     << endl;
                cout << dh.ac() << endl;
                cout << endl << "df= " << endl << df.ac() << endl;
            }

            {
                cout << endl
                     << "Test atan2 (MaskedArray<Double>, Array<Double>)"
                     << endl;
                dh = 2.0;
                dh = atan2 (df ((df.ac() > 2.5) && (df.ac() < 6.5)), dg.ac());
                cout << endl << "dh=2.0; "
                     << endl
                     << " dh = atan2 (df ((df > 2.5) && (df < 6.5)), dg);"
                     << endl;
                cout << dh.ac() << endl;
            }

            {
                cout << endl
                     << "Test atan2 (Array<Double>, MaskedArray<Double>)"
                     << endl;
                dh = 2.0;
                dh = atan2 (dg.ac(), df ((df.ac() > 2.5) && (df.ac() < 6.5)));
                cout << endl << "dh=2.0; "
                     << endl
                     << " dh = atan2 (dg, df ((df > 2.5) && (df < 6.5)));"
                     << endl;
                cout << dh.ac() << endl;
            }

            {
                cout << endl
                     << "Test atan2 (MaskedArray<Double>, MaskedArray<Double>)"
                     << endl;
                dh = 2.0;
                dh = atan2 (dg ((df.ac() > 3.5) && (df.ac() < 7.5)),
                            df ((df.ac() > 2.5) && (df.ac() < 6.5)));
                cout << endl << "dh=2.0; "
                     << endl
                     << " dh = atan2 (dg ((df > 3.5) && (df < 7.5)),"
                     << endl
                     << "             df ((df > 2.5) && (df < 6.5)));"
                     << endl;
                cout << dh.ac() << endl;
            }

            {
                cout << endl
                     << "Test atan2 (MaskedArray<Double>, Double)"
                     << endl;
                dh = 2.0;
                dh = atan2 (df ((df.ac() > 2.5) && (df.ac() < 6.5)), 2.0);
                cout << endl << "dh=2.0; "
                     << endl
                     << " dh = atan2 (df ((df > 2.5) && (df < 6.5)), 2.0);"
                     << endl;
                cout << dh.ac() << endl;
            }

            {
                cout << endl
                     << "Test atan2 (Double, MaskedArray<Double>)"
                     << endl;
                dh = 2.0;
                dh = atan2 (2.0, df ((df.ac() > 2.5) && (df.ac() < 6.5)));
                cout << endl << "dh=2.0; "
                     << endl
                     << " dh = atan2 (2.0, df ((df > 2.5) && (df < 6.5)));"
                     << endl;
                cout << dh.ac() << endl;
            }

            {
                cout << endl
                     << "Test pow (MaskedArray<Double>, Array<Double>)"
                     << endl;
                dh = 2.0;
                dh = pow (df ((df.ac() > 2.5) && (df.ac() < 6.5)), dg.ac());
                cout << endl << "dh=2.0; "
                     << endl
                     << " dh = pow (df ((df > 2.5) && (df < 6.5)), dg);"
                     << endl;
                cout << dh.ac() << endl;
            }

            {
                cout << endl
                     << "Test pow (Array<Double>, MaskedArray<Double>)"
                     << endl;
                dh = 2.0;
                dh = pow (dg.ac(), df ((df.ac() > 2.5) && (df.ac() < 6.5)));
                cout << endl << "dh=2.0; "
                     << endl
                     << " dh = pow (dg, df ((df > 2.5) && (df < 6.5)));"
                     << endl;
                cout << dh.ac() << endl;
            }

            {
                cout << endl
                     << "Test pow (MaskedArray<Double>, MaskedArray<Double>)"
                     << endl;
                dh = 2.0;
                dh = pow (dg ((df.ac() > 3.5) && (df.ac() < 7.5)),
                            df ((df.ac() > 2.5) && (df.ac() < 6.5)));
                cout << endl << "dh=2.0; "
                     << endl
                     << " dh = pow (dg ((df > 3.5) && (df < 7.5)),"
                     << endl
                     << "             df ((df > 2.5) && (df < 6.5)));"
                     << endl;
                cout << dh.ac() << endl;
            }

            {
                cout << endl
                     << "Test pow (MaskedArray<Double>, Double)"
                     << endl;
                dh = 2.0;
                dh = pow (df ((df.ac() > 2.5) && (df.ac() < 6.5)), 2.0);
                cout << endl << "dh=2.0; "
                     << endl
                     << " dh = pow (df ((df > 2.5) && (df < 6.5)), 2.0);"
                     << endl;
                cout << dh.ac() << endl;
            }

            {
                cout << endl
                     << "Test sum (MaskedArray<Double>)"
                     << endl;
                Double result (-1.0);
                result = sum (df ((df.ac() > 2.5) && (df.ac() < 6.5)));
                cout << endl
                     << " result = sum (df ((df > 2.5) && (df < 6.5)));"
                     << endl;
                cout << result << endl;
            }

            {
                cout << endl
                     << "Test sumsquares (MaskedArray<Double>)"
                     << endl;
                Double result (-1.0);
                result = sumsquares (df ((df.ac() > 2.5) && (df.ac() < 6.5)));
                cout << endl
                     << " result = sumsquares (df ((df > 2.5) && (df < 6.5)));"
                     << endl;
                cout << result << endl;
            }

            {
                cout << endl
                     << "Test product (MaskedArray<Double>)"
                     << endl;
                Double result (-1.0);
                result = product (df ((df.ac() > 2.5) && (df.ac() < 6.5)));
                cout << endl
                     << " result = product (df ((df > 2.5) && (df < 6.5)));"
                     << endl;
                cout << result << endl;
            }

            {
                cout << endl
                     << "Test mean (MaskedArray<Double>)"
                     << endl;
                Double result (-1.0);
                result = mean (df ((df.ac() > 2.5) && (df.ac() < 6.5)));
                cout << endl
                     << " result = mean (df ((df > 2.5) && (df < 6.5)));"
                     << endl;
                cout << result << endl;
            }

            {
                cout << endl
                     << "Test variance (MaskedArray<Double>)"
                     << endl;
                Double result (-1.0);
                result = variance (df ((df.ac() > 2.5) && (df.ac() < 6.5)));
                cout << endl
                     << " result = variance (df ((df > 2.5) && (df < 6.5)));"
                     << endl;
                cout << result << endl;
            }

            {
                cout << endl
                     << "Test stddev (MaskedArray<Double>)"
                     << endl;
                Double result (-1.0);
                result = stddev (df ((df.ac() > 2.5) && (df.ac() < 6.5)));
                cout << endl
                     << " result = stddev (df ((df > 2.5) && (df < 6.5)));"
                     << endl;
                cout << result << endl;
            }

            {
                cout << endl
                     << "Test avdev (MaskedArray<Double>)"
                     << endl;
                Double result (-1.0);
                result = avdev (df ((df.ac() > 2.5) && (df.ac() < 6.5)));
                cout << endl
                     << " result = avdev (df ((df > 2.5) && (df < 6.5)));"
                     << endl;
                cout << result << endl;
            }

            {
                cout << endl
                     << "Test even median (MaskedArray<Double>, True)"
                     << endl;
                Double result (-1.0);
                result = median (df ((df.ac() > 2.5) && (df.ac() < 6.5)), True);
                cout << endl
                     << " result = median (df ((df > 2.5) && (df < 6.5)), True);"
                     << endl;
                cout << result << endl;
            }

            {
                cout << endl
                     << "Test even median (MaskedArray<Double>)"
                     << endl;
                Vector<Double> dfunsort(10);
                dfunsort = df;
                Double tmp;
                tmp = dfunsort (9);
                dfunsort (9) = dfunsort (5);
                dfunsort (5) = tmp;
                Double result (-1.0);
                result = median (dfunsort ((dfunsort.ac() > 2.5) &&
                                           (dfunsort.ac() < 6.5)));
                cout << endl
                     << " result = median (dfunsort ((dfunsort > 2.5) &&"
                     << endl
                     << "                            (dfunsort < 6.5)));"
                     << endl;
                cout << result << endl;
                cout << "dfunsort= " << endl << dfunsort.ac() << endl;
            }

            {
                cout << endl
                     << "Test odd median (MaskedArray<Double>, True)"
                     << endl;
                Double result (-1.0);
                result = median (df ((df.ac() > 2.5) && (df.ac() < 7.5)), True);
                cout << endl
                     << " result = median (df ((df > 2.5) && (df < 7.5)), True);"
                     << endl;
                cout << result << endl;
            }

            {
                cout << endl
                     << "Test odd  median (MaskedArray<Double>)"
                     << endl;
                Vector<Double> dfunsort(10);
                dfunsort = df;
                Double tmp;
                tmp = dfunsort (9);
                dfunsort (9) = dfunsort (5);
                dfunsort (5) = tmp;
                Double result (-1.0);
                result = median (dfunsort ((dfunsort.ac() > 2.5) &&
                                           (dfunsort.ac() < 7.5)));
                cout << endl
                     << " result = median (dfunsort ((dfunsort > 2.5) &&"
                     << endl
                     << "                            (dfunsort < 7.5)));"
                     << endl;
                cout << result << endl;
                cout << "dfunsort= " << endl << dfunsort.ac() << endl;
            }

            {
                cout << endl
                     << "Test square (MaskedArray<Double>)"
                     << endl;
                dh = 2.0;
                dh = square (df ((df.ac() > 2.5) && (df.ac() < 6.5)));
                cout << endl << "dh=2.0; "
                     << endl
                     << "dh = square (df ((df > 2.5) && (df < 6.5)));"
                     << endl;
                cout << dh.ac() << endl;
                cout << endl << "df= " << endl << df.ac() << endl;
            }

            {
                cout << endl
                     << "Test cube (MaskedArray<Double>)"
                     << endl;
                dh = 2.0;
                dh = cube (df ((df.ac() > 2.5) && (df.ac() < 6.5)));
                cout << endl << "dh=2.0; "
                     << endl
                     << "dh = cube (df ((df > 2.5) && (df < 6.5)));"
                     << endl;
                cout << dh.ac() << endl;
                cout << endl << "df= " << endl << df.ac() << endl;
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
