//# tMaskedArray.cc: Test program for MaskedArrays
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
#include <aips/Arrays/MaskArrIO.h>
#include <aips/Arrays/LogiCube.h>


main()
{
    try {
        {
            cout << endl << "Testing MaskedArrays." << endl;

            {

// Array
            Array<Int> x(IPosition(1,5)), y(IPosition(1,5));
            LogicalArray b(IPosition(1,5));

            Bool xOK = x.ok();

            uInt xNdim = x.ndim();

            uInt xNelements = x.nelements();

            IPosition xShape = x.shape();

            cout << " x=1;" << endl;
            x=1;

            cout << " indgen (y);" << endl;
            indgen (y);

            cout << endl << "x= " << endl;
            cout << x.ac() << endl;
            cout << endl << "y= " << endl;
            cout << y.ac() << endl;

            b = (x <= y);
            cout << endl << "b= (x <= y) = " << endl;
            cout << b << endl;

            Array<Int> z(IPosition(1,5));
            MaskedArray<Int> m(z, b);

            z = 0;
            cout << endl << "z= " << endl;
            cout << z << endl;

            m = 5;
            cout << endl << "z= " << endl;
            cout << z << endl;

            MaskedArray<Int> n(m);

            n = 6;
            cout << endl << "z= " << endl;
            cout << z << endl;

            z(b) = 7;
            cout << endl << "z= " << endl;
            cout << z << endl;

            z (x <= y) = 8;
            cout << endl << "z= " << endl;
            cout << z << endl;

            {
                Array<Int> yc (IPosition(1,24));
                indgen (yc);
                const Array<Int> ycc (yc);

                cout << "\n";

                cout << "ycc(ycc<18)=\n"
                     <<  ycc(ycc<18) << endl;

                cout << "(yc(yc<18))(yc>3)=\n"
                     <<  (yc(yc.ac()<18))(yc>3) << endl;

                         (yc(yc.ac()<18))(yc>3) = 8;

                cout << "(yc(yc<18))(yc>3) = 8; yc=\n"
                     <<  yc.ac() << endl;
            }

            {
                Array<Int> yc (IPosition(1,24));
                indgen (yc);

                cout << "\n";

                cout << "yc((yc<18)(yc>3))=\n"
                     <<  yc((yc.ac()<18)(yc>3)) << endl;

                         yc((yc.ac()<18)(yc>3)) = 8;

                cout << "yc((yc<18)(yc>3)) = 8; yc=\n"
                     << yc.ac() << endl;
            }

            {
                Array<Int> yc (IPosition(1,24));
                indgen (yc);

                cout << "\n";

                cout << "(yc(yc>7)) ((yc<18)(yc>3))=\n"
                     <<  (yc(yc>7)) ((yc.ac()<18)(yc>3)) << endl;

                         (yc(yc>7)) ((yc.ac()<18)(yc>3)) = 8;

                cout << "(yc(yc>7)) ((yc<18)(yc>3)) = 8; yc=\n"
                     << yc.ac() << endl;
            }

            {
                Array<Int> yc (IPosition(1,24));
                indgen (yc);
                const         Array<Int> ycc  (yc);
                const MaskedArray<Int> ycc1 (yc, yc.ac()<18);
                      MaskedArray<Int> ycc2 (yc, yc.ac()<18);
                const   MaskedArray<Int> ycc3 (yc, yc.ac()<18);
                        MaskedArray<Int> ycc4 (yc, yc.ac()<18);

                cout << "\n";

                cout << "ycc1(ycc>7)=\n"
                     <<  ycc1(ycc>7) << endl;

                cout << "ycc2(ycc>7)=\n"
                     <<  ycc2(ycc>7) << endl;

                cout << "ycc3(ycc>7)=\n"
                     <<  ycc3(ycc>7) << endl;

                cout << "ycc4(ycc>7)=\n"
                     <<  ycc4(ycc>7) << endl;

                cout << "yc(yc>7)=\n"
                     <<  yc(yc>7) << endl;

                cout << "ycc(ycc>7)=\n"
                     <<  ycc(ycc>7) << endl;

            }

// End Array

// Vector
            Vector<Int> w(5);
            indgen (w.ac(), 6);

            cout << endl << "w= " << endl;
            cout << w << endl;

            w = z(w.ac()<z);
            cout << endl << "w= z(w<z) = " << endl;
            cout << w << endl;

            {
                Vector<Int> yc (24);
                indgen (yc.ac());

                const Vector<Int> ycc (yc);

                cout << "\n";

                cout << "ycc(ycc<18)=\n"
                     <<  ycc(ycc.ac()<18) << endl;

                cout << "(yc(yc<18))(yc>3)=\n"
                     <<  (yc(yc.ac()<18))(yc.ac()>3) << endl;

                         (yc(yc.ac()<18))(yc.ac()>3) = 8;

                cout << "(yc(yc<18))(yc>3) = 8; yc=\n"
                     <<  yc.ac() << endl;
            }

            {
                Vector<Int> yc (24);
                indgen (yc.ac());

                cout << "\n";

                cout << "yc((yc<18)(yc>3))=\n"
                     <<  yc((yc.ac()<18)(yc.ac()>3)) << endl;

                         yc((yc.ac()<18)(yc.ac()>3)) = 8;

                cout << "yc((yc<18)(yc>3)) = 8; yc=\n"
                     << yc.ac() << endl;
            }

            {
                Vector<Int> yc (24);
                indgen (yc.ac());

                cout << "\n";

                cout << "(yc(yc>7)) ((yc<18)(yc>3))=\n"
                     <<  (yc(yc.ac()>7)) ((yc.ac()<18)(yc.ac()>3)) << endl;

                         (yc(yc.ac()>7)) ((yc.ac()<18)(yc.ac()>3)) = 8;

                cout << "(yc(yc>7)) ((yc<18)(yc>3)) = 8; yc=\n"
                     << yc.ac() << endl;
            }

// End Vector

// Matrix
            Matrix<Int> v8(5,3);
            Matrix<Int> v(5,3);
            v8 = 8;
            indgen (v.ac());

            cout << endl << "v8= " << endl;
            cout << v8.ac() << endl;

            cout << endl << "v= " << endl;
            cout << v.ac() << endl;

            v = v8(v.ac()<v8.ac());
            cout << endl << "v= v8(v<v8) = " << endl;
            cout << v.ac() << endl;

            v(v.ac()<9) = 9;
            cout << endl << "v(v<9)= 9 = " << endl;
            cout << v.ac() << endl;

            {
                Matrix<Int> yc (4,6);
                indgen (yc.ac());
                const Matrix<Int> ycc (yc);

                cout << "\n";

                cout << "ycc(ycc<18)=\n"
                     <<  ycc(ycc.ac()<18) << endl;

                cout << "(yc(yc<18))(yc>3)=\n"
                     <<  (yc(yc.ac()<18))(yc.ac()>3) << endl;

                         (yc(yc.ac()<18))(yc.ac()>3) = 8;

                cout << "(yc(yc<18))(yc>3) = 8; yc=\n"
                     <<  yc.ac() << endl;
            }

            {
                Matrix<Int> yc (4,6);
                indgen (yc.ac());

                cout << "\n";

                cout << "yc((yc<18)(yc>3))=\n"
                     <<  yc((yc.ac()<18)(yc.ac()>3)) << endl;

                         yc((yc.ac()<18)(yc.ac()>3)) = 8;

                cout << "yc((yc<18)(yc>3)) = 8; yc=\n"
                     << yc.ac() << endl;
            }

            {
                Matrix<Int> yc (4,6);
                indgen (yc.ac());

                cout << "\n";

                cout << "(yc(yc>7)) ((yc<18)(yc>3))=\n"
                     <<  (yc(yc.ac()>7)) ((yc.ac()<18)(yc.ac()>3)) << endl;

                         (yc(yc.ac()>7)) ((yc.ac()<18)(yc.ac()>3)) = 8;

                cout << "(yc(yc>7)) ((yc<18)(yc>3)) = 8; yc=\n"
                     << yc.ac() << endl;
            }

// End Matrix

// Cube
            Cube<Int> u15(5,3,2);
            Cube<Int> u(5,3,2);
            u15 = 15;
            indgen (u.ac());

            cout << endl << "u15= " << endl;
            cout << u15.ac() << endl;

            cout << endl << "u= " << endl;
            cout << u.ac() << endl;

            u = u15(u.ac()<u15.ac());
            cout << endl << "u= u15(u<u15) = " << endl;
            cout << u.ac() << endl;

            u(u.ac()<16) = 16;
            cout << endl << "u(u<16)= 16 = " << endl;
            cout << u.ac() << endl;

            {
                Cube<Int> yc (4,3,2);
                indgen (yc.ac());

                const Cube<Int> ycc (yc);

                cout << "\n";

                cout << "ycc(ycc<18)=\n"
                     <<  ycc(ycc.ac()<18) << endl;

                cout << "(yc(yc<18))(yc>3)=\n"
                     <<  (yc(yc.ac()<18))(yc.ac()>3) << endl;

                         (yc(yc.ac()<18))(yc.ac()>3) = 8;

                cout << "(yc(yc<18))(yc>3) = 8; yc=\n"
                     <<  yc.ac() << endl;
            }

            {
                Cube<Int> yc (4,3,2);
                indgen (yc.ac());

                cout << "\n";

                cout << "yc((yc<18)(yc>3))=\n"
                     <<  yc((yc.ac()<18)(yc.ac()>3)) << endl;

                         yc((yc.ac()<18)(yc.ac()>3)) = 8;

                cout << "yc((yc<18)(yc>3)) = 8; yc=\n"
                     << yc.ac() << endl;
            }

            {
                Cube<Int> yc (4,3,2);
                indgen (yc.ac());

                cout << "\n";

                cout << "(yc(yc>7)) ((yc<18)(yc>3))=\n"
                     <<  (yc(yc.ac()>7)) ((yc.ac()<18)(yc.ac()>3)) << endl;

                         (yc(yc.ac()>7)) ((yc.ac()<18)(yc.ac()>3)) = 8;

                cout << "(yc(yc>7)) ((yc<18)(yc>3)) = 8; yc=\n"
                     << yc.ac() << endl;
            }

// End Cube

// Compressed Array
            {

            Cube<Int> cu (5, 3, 2);
            indgen (cu.ac());

            LogicalCube lcu (5, 3, 2);
	    lcu = (cu.ac() > 10) && (cu.ac() <= 20);

            MaskedArray<Int> mcu (cu, lcu);

            cout << "\n";

            cout << "mcu=\n"
                 <<  mcu
                 << endl;

            {
                Vector<Int> vec (mcu.getCompressedArray());

                cout << "\n";

                cout << "Vector<Int> vec (mcu.getCompressedArray())=\n"
                     <<  vec
                     << endl;
            }

            {
                Matrix<Int> mat (mcu.getCompressedArray (IPosition (2,5,2)));

                cout << "\n";

       cout << "Matrix<Int> mat (mcu.getCompressedArray (IPosition (2,5,2)))=\n"
                     <<  mat.ac()
                     << endl;
            }

            {
                Matrix<Int> mat (5,2);

                mcu.getCompressedArray (mat);

                cout << "\n";

                cout << "mcu.getCompressedArray (mat).  mat=\n"
                     <<  mat.ac()
                     << endl;

                mcu.setCompressedArray (-mat.ac());

                cout << "mcu.setCompressedArray (-mat).  mcu=\n"
                     <<  mcu
                     << endl;
            }

            }

// End Compressed Array

// Readonly
            {
            Cube<Int> cu (5, 3, 2);
            indgen (cu.ac());

            LogicalCube lcu (5, 3, 2);
	    lcu = (cu.ac() > 10) && (cu.ac() <= 20);

            {
            MaskedArray<Int> mcu (cu, lcu);

            cout << "\n";

            cout << "mcu.isReadOnly()=\n"
                 <<  mcu.isReadOnly()
                 << endl;

            }

            {
            MaskedArray<Int> mcu (cu, lcu, False);

            cout << "\n";

            cout << "mcu.isReadOnly()=\n"
                 <<  mcu.isReadOnly()
                 << endl;

            }

            {
            MaskedArray<Int> mcu (cu, lcu, True);

            cout << "\n";

            cout << "mcu.isReadOnly()=\n"
                 <<  mcu.isReadOnly()
                 << endl;

            }

            {
            MaskedArray<Int> mcu (cu, lcu);
            mcu.setReadOnly();

            cout << "\n";

            cout << "mcu.isReadOnly()=\n"
                 <<  mcu.isReadOnly()
                 << endl;

            }

            {
            MaskedLogicalArray mlcu (lcu, lcu);
            MaskedArray<Int> mcu (cu, mlcu, True);

            cout << "\n";

            cout << "mcu.isReadOnly()=\n"
                 <<  mcu.isReadOnly()
                 << endl;

            }

            {
            MaskedArray<Int> mmcu (cu, lcu);
            MaskedArray<Int> mcu (mmcu, lcu, True);

            cout << "\n";

            cout << "mcu.isReadOnly()=\n"
                 <<  mcu.isReadOnly()
                 << endl;

            }

            {
            MaskedArray<Int> mmcu (cu, lcu);
            MaskedLogicalArray mlcu (lcu, lcu);
            MaskedArray<Int> mcu (mmcu, mlcu, True);

            cout << "\n";

            cout << "mcu.isReadOnly()=\n"
                 <<  mcu.isReadOnly()
                 << endl;

            }

            {
            MaskedArray<Int> mmcu (cu, lcu, True);
            MaskedArray<Int> mcu (mmcu, lcu, False);

            cout << "\n";

            cout << "mcu.isReadOnly()=\n"
                 <<  mcu.isReadOnly()
                 << endl;

            }

            {
            MaskedArray<Int> mmcu (cu, lcu, True);
            MaskedLogicalArray mlcu (lcu, lcu);
            MaskedArray<Int> mcu (mmcu, mlcu, False);

            cout << "\n";

            cout << "mcu.isReadOnly()=\n"
                 <<  mcu.isReadOnly()
                 << endl;

            }

            {
            const Array<Int> ccu (cu);

            cout << "\n";

            cout << "ccu(lcu).isReadOnly()=\n"
                 <<  ccu(lcu).isReadOnly()
                 << endl;

            }

            {
            MaskedArray<Int> mcu (cu, lcu, True);

            cout << "\n";

            cout << "mcu(lcu).isReadOnly()=\n"
                 <<  mcu(lcu).isReadOnly()
                 << endl;

            }

            {
            MaskedArray<Int> mmcu (cu, lcu, True);
            MaskedArray<Int> mcu (mmcu);

            cout << "\n";

            cout << "mcu.isReadOnly()=\n"
                 <<  mcu.isReadOnly()
                 << endl;

            }

            {
            MaskedArray<Int> mmcu (cu, lcu, True);
            MaskedArray<Int> mcu (mmcu, False);

            cout << "\n";

            cout << "mcu.isReadOnly()=\n"
                 <<  mcu.isReadOnly()
                 << endl;

            }

            {
            MaskedArray<Int> mmcu (cu, lcu, True);
            MaskedArray<Int> mcu (mmcu, True);

            cout << "\n";

            cout << "mcu.isReadOnly()=\n"
                 <<  mcu.isReadOnly()
                 << endl;

            }

            {
            MaskedArray<Int> mcu (cu, lcu, True);

            cout << "\n";

            cout << "mcu.copy().isReadOnly()=\n"
                 <<  mcu.copy().isReadOnly()
                 << endl;

            }

            {
            MaskedArray<Int> mcu (cu, lcu, True);

            cout << "\n";

            cout << "mcu.copy(False).isReadOnly()=\n"
                 <<  mcu.copy(False).isReadOnly()
                 << endl;

            }

            {
            MaskedArray<Int> mcu (cu, lcu);

            cout << "\n";

            cout << "mcu.copy(True).isReadOnly()=\n"
                 <<  mcu.copy(True).isReadOnly()
                 << endl;

            }

            {
            MaskedArray<Int> mcu (cu, lcu);

            cout << "\n";

            cout << "mcu.copy(False).isReadOnly()=\n"
                 <<  mcu.copy(False).isReadOnly()
                 << endl;

            }

            }

// End Readonly
            }

            cout << endl << "OK" << endl;
        }
    } catch (AipsError x) {
        cout << "\nCaught an exception: " << x.getMesg() << endl;
    } end_try;

    cout << "OK" << endl;
    return 0;
}
