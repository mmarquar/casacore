//# tComplex.cc: This program tests the Complex class
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

//# Includes

#include <aips/Mathematics/Complex.h>

#include <assert.h>
#include <stdio.h>
#include <fstream.h>
#include <unistd.h>

main() {

  assert(square(IComplex(2,0)) == IComplex(4,0));
  assert(cube(IComplex(2,0)) == IComplex(8,0));
  assert(square(IComplex(0,2)) == IComplex(-4,0));
  assert(cube(IComplex(0,2)) == IComplex(0,-8));

  Complex f1(23.9,1.8),f2(9.2,8.2),f3(2.7,1.8),fo(237.561,0.9312),fi;
  IComplex i1(5,2),i2(33,6),i3(f1);
  DComplex d1,d2(f1.real(), f1.imag()),d3(0.921,7.812);
  char tmpname[L_tmpnam];
  fstream fio(tmpnam(tmpname),ios::out | ios::trunc);

  cout << "Initial value for complex: " << d1 << endl;
  cout << d1 << " :" << endl;
  cout << "    real part:      " << d1.real() << " == " << real(d1) << endl;
  cout << "    imaginary part: " << d1.imag() << " == " << imag(d1) << endl;
  cout << f1 << " == " << d2 << " ~= " << i3 << endl;
  cout << "-" << d3 << " == " << -d3 << endl;
  cout << "conj(" << d3 << ") == " << conj(d3) << endl;
  cout << "norm(" << d3 << ") == " << norm(d3) << endl;
  cout << "arg(" << d3 << ") == " << arg(d3) << endl;
  d1.real() = 18.9;
  d1.imag() = -2.31;
  cout << "fabs(" << d1 << ") == " << fabs(d1) << endl;
  fio << fo << endl;
  fio.close();
  fio.open(tmpname,ios::in);
  fio >> fi;
  fio.close();
  unlink(tmpname);
  cout << "out: " << fo << "           in: " << fi << endl;

  
  d1 = i1;
  cout << d1 << " == " << i1 << endl;

  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << f1 << " + " << f2 << " = " << f1 + f2 << endl;
  cout << f1 << " * " << f2 << " = " << f1 * f2 << endl;
  cout << f1 << " - " << f2 << " = " << f1 - f2 << endl;
  cout << f1 << " / " << f2 << " = " << f1 / f2 << endl;
  cout << endl;

  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << f1 << " + " << 7.561 << " = " << f1 + 7.561 << endl;
  cout << f1 << " * " << 7.561 << " = " << f1 * 7.561 << endl;
  cout << f1 << " - " << 7.561 << " = " << f1 - 7.561 << endl;
  cout << f1 << " / " << 7.561 << " = " << f1 / 7.561 << endl;
  cout << endl;

  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << f1 << " + " << 13 << " = " << f1 + 13 << endl;
  cout << f1 << " * " << 13 << " = " << f1 * 13 << endl;
  cout << f1 << " - " << 13 << " = " << f1 - 13 << endl;
  cout << f1 << " / " << 13 << " = " << f1 /  13 << endl;
  cout << endl;

  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << f1 << " + " << i1 << " = " << f1 + i1 << endl;
  cout << f1 << " * " << i1 << " = " << f1 * i1 << endl;
  cout << f1 << " - " << i1 << " = " << f1 - i1 << endl;
  cout << f1 << " / " << i1 << " = " << f1 / i1 << endl;
  cout << endl;

  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << i1 << " + " << f1 << " = " << i1 + f1 << endl;
  cout << i1 << " * " << f1 << " = " << i1 * f1 << endl;
  cout << i1 << " - " << f1 << " = " << i1 - f1 << endl;
  cout << i1 << " / " << f1 << " = " << i1 / f1 << endl;
  cout << endl;

  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << f1 << " +=  4 -> "; cout << (f1 += 4) << endl;
  cout << f1 << " *=  4 -> "; cout << (f1 *= 4) << endl;
  cout << f1 << " -=  4 -> "; cout << (f1 -= 4) << endl;
  cout << f1 << " /=  4 -> "; cout << (f1 /= 4) << endl;
  cout << endl;

  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << f1 << " +=  0.896 -> "; cout << (f1 += 0.896) << endl;
  cout << f1 << " *=  0.896 -> "; cout << (f1 *= 0.896) << endl;
  cout << f1 << " -=  0.896 -> "; cout << (f1 -= 0.896) << endl;
  cout << f1 << " /=  0.896 -> "; cout << (f1 /= 0.896) << endl;
  cout << endl;

  char simChar = 2;
  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << f1 << " +=  simChar -> "; cout << (f1 += simChar) << endl;
  cout << f1 << " *=  simChar -> "; cout << (f1 *= simChar) << endl;
  cout << f1 << " -=  simChar -> "; cout << (f1 -= simChar) << endl;
  cout << f1 << " /=  simChar -> "; cout << (f1 /= simChar) << endl;
  cout << endl;

  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << f1 << " +=  " << f3 << " -> "; cout << (f1 += f3) << endl;
  cout << f1 << " *=  " << f3 << " -> "; cout << (f1 *= f3) << endl;
  cout << f1 << " -=  " << f3 << " -> "; cout << (f1 -= f3) << endl;
  cout << f1 << " /=  " << f3 << " -> "; cout << (f1 /= f3) << endl;
  cout << endl;

  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << f1 << " +=  " << d3 << " -> "; cout << (f1 += d3) << endl;
  cout << f1 << " *=  " << d3 << " -> "; cout << (f1 *= d3) << endl;
  cout << f1 << " -=  " << d3 << " -> "; cout << (f1 -= d3) << endl;
  cout << f1 << " /=  " << d3 << " -> "; cout << (f1 /= d3) << endl;
  cout << endl;

  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << f1 << " +=  " << i1 << " -> "; cout << (f1 += i1) << endl;
  cout << f1 << " *=  " << i1 << " -> "; cout << (f1 *= i1) << endl;
  cout << f1 << " -=  " << i1 << " -> "; cout << (f1 -= i1) << endl;
  cout << f1 << " /=  " << i1 << " -> "; cout << (f1 /= i1) << endl;
  cout << endl;

  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << i1 << " +=  " << f1 << " -> "; cout << (i1 += f1) << endl;
  cout << i1 << " *=  " << f1 << " -> "; cout << (i1 *= f1) << endl;
  cout << i1 << " -=  " << f1 << " -> "; cout << (i1 -= f1) << endl;
  cout << i1 << " /=  " << f1 << " -> "; cout << (i1 /= f1) << endl;
  cout << endl;

  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << "sin(" << f1 << ") = " << sin(f1) << endl;
  cout << "cos(" << f1 << ") = " << cos(f1) << endl;
  cout << "sinh(" << f1 << ") = " << sinh(f1) << endl;
  cout << "cosh(" << f1 << ") = " << cosh(f1) << endl;
  cout << endl;

  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << "log(" << f1 << ") = " << log(f1) << endl;
  cout << "log10(" << f1 << ") = " << log10(f1) << endl;
  cout << "exp(" << f1 << ") = " << exp(f1) << endl;
  cout << endl;

  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << "pow(" << f1 << ",3) = " << pow(f1,3) << endl;
  cout << "pow(" << f1 << ",-8) = " << pow(f1,-8) << endl;
  cout << "pow(" << f1 << ",0.214) = " << pow(f1,0.214) << endl;
  cout << "pow(" << f1 << "," << i1 << " = " << pow(f1,i1) << endl;
  cout << "pow(" << f1 << "," << f2 << " = " << pow(f1,f2) << endl;
  cout << endl;

  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << "sqrt(" << f1 << ") = " << sqrt(f1) << endl;
  cout << "sqrt(" << i1 << ") = " << sqrt(i1) << endl;
  cout << endl;

  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << "real(" << i1 << ") = " << real(i1) << endl;
  cout << "imag(" << i1 << ") = " << imag(i1) << endl;
  cout << endl;

  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << f1 << " == " << f2 << " -> " << (f1 == f2) << endl;
  cout << f1 << " != " << f2 << " -> " << (f1 != f2) << endl;
  cout << f1 << " >= " << f2 << " -> " << (f1 >= f2) << endl;
  cout << f1 << " > " << f2 << " -> " << (f1 > f2) << endl;
  cout << f1 << " <= " << f2 << " -> " << (f1 <= f2) << endl;
  cout << f1 << " < " << f2 << " -> " << (f1 < f2) << endl;
  cout << endl;

  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << f2 << " == " << f1 << " -> " << (f2 == f1) << endl;
  cout << f2 << " != " << f1 << " -> " << (f2 != f1) << endl;
  cout << f2 << " >= " << f1 << " -> " << (f2 >= f1) << endl;
  cout << f2 << " > " << f1 << " -> " << (f2 > f1) << endl;
  cout << f2 << " <= " << f1 << " -> " << (f2 <= f1) << endl;
  cout << f2 << " < " << f1 << " -> " << (f2 < f1) << endl;
  cout << endl;

  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << f1 << " == " << f1 << " -> " << (f1 == f1) << endl;
  cout << f1 << " != " << f1 << " -> " << (f1 != f1) << endl;
  cout << f1 << " >= " << f1 << " -> " << (f1 >= f1) << endl;
  cout << f1 << " > " << f1 << " -> " << (f1 > f1) << endl;
  cout << f1 << " <= " << f1 << " -> " << (f1 <= f1) << endl;
  cout << f1 << " < " << f1 << " -> " << (f1 < f1) << endl;
  cout << endl;

  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << f1 << " == " << i1 << " -> " << (f1 == i1) << endl;
  cout << f1 << " != " << i1 << " -> " << (f1 != i1) << endl;
  cout << f1 << " >= " << i1 << " -> " << (f1 >= i1) << endl;
  cout << f1 << " > " << i1 << " -> " << (f1 > i1) << endl;
  cout << f1 << " <= " << i1 << " -> " << (f1 <= i1) << endl;
  cout << f1 << " < " << i1 << " -> " << (f1 < i1) << endl;


  f1.real() = 33.0;
  f1.imag() = 6.0;
  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << f1 << " == " << i2 << " -> " << (f1 == i2) << endl;
  cout << f1 << " != " << i2 << " -> " << (f1 != i2) << endl;
  cout << f1 << " >= " << i2 << " -> " << (f1 >= i2) << endl;
  cout << f1 << " > " << i2 << " -> " << (f1 > i2) << endl;
  cout << f1 << " <= " << i2 << " -> " << (f1 <= i2) << endl;
  cout << f1 << " < " << i2 << " -> " << (f1 < i2) << endl;

  f1 = 33.021;
  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << f1 << " == " << i2 << " -> " << (f1 == i2) << endl;
  cout << f1 << " != " << i2 << " -> " << (f1 != i2) << endl;
  cout << f1 << " >= " << i2 << " -> " << (f1 >= i2) << endl;
  cout << f1 << " > " << i2 << " -> " << (f1 > i2) << endl;
  cout << f1 << " <= " << i2 << " -> " << (f1 <= i2) << endl;
  cout << f1 << " < " << i2 << " -> " << (f1 < i2) << endl;

  f1 = 0;
  cout << "- - - - - - - - - - - - - - - - - - - -" << endl;
  cout << f1 << " == " << i2 << " -> " << (f1 == i2) << endl;
  cout << f1 << " != " << i2 << " -> " << (f1 != i2) << endl;
  cout << f1 << " >= " << i2 << " -> " << (f1 >= i2) << endl;
  cout << f1 << " > " << i2 << " -> " << (f1 > i2) << endl;
  cout << f1 << " <= " << i2 << " -> " << (f1 <= i2) << endl;
  cout << f1 << " < " << i2 << " -> " << (f1 < i2) << endl;

  assert(near(Complex(0,10000), Complex(0,10001), 1.01e-4));
  assert(!near(Complex(0,10000), Complex(0,10001), 0.99e-4));
  assert(!near(Complex(10000,0), Complex(0,10001), 1.01e-4));
  assert(nearAbs(Complex(0,10000), Complex(0,10001), 1.01));
  assert(!nearAbs(Complex(0,10000), Complex(0,10001), 0.99));
  assert(!nearAbs(Complex(10000,0), Complex(0,10001), 1.01));

  assert(allNear(Complex(0,10000), Complex(0,10001), 1.01e-4));
  assert(!allNear(Complex(0,10000), Complex(0,10001), 0.99e-4));
  assert(!allNear(Complex(10000,0), Complex(0,10001), 1.01e-4));
  assert(allNearAbs(Complex(0,10000), Complex(0,10001), 1.01));
  assert(!allNearAbs(Complex(0,10000), Complex(0,10001), 0.99));
  assert(!allNearAbs(Complex(10000,0), Complex(0,10001), 1.01));

  assert(near(DComplex(0,10000), DComplex(0,10001), 1.01e-4));
  assert(!near(DComplex(0,10000), DComplex(0,10001), 0.99e-4));
  assert(!near(DComplex(10000,0), DComplex(0,10001), 1.01e-4));
  assert(nearAbs(DComplex(0,10000), DComplex(0,10001), 1.01));
  assert(!nearAbs(DComplex(0,10000), DComplex(0,10001), 0.99));
  assert(!nearAbs(DComplex(10000,0), DComplex(0,10001), 1.01));

  assert(allNear(DComplex(0,10000), DComplex(0,10001), 1.01e-4));
  assert(!allNear(DComplex(0,10000), DComplex(0,10001), 0.99e-4));
  assert(!allNear(DComplex(10000,0), DComplex(0,10001), 1.01e-4));
  assert(allNearAbs(DComplex(0,10000), DComplex(0,10001), 1.01));
  assert(!allNearAbs(DComplex(0,10000), DComplex(0,10001), 0.99));
  assert(!allNearAbs(DComplex(10000,0), DComplex(0,10001), 1.01));

  Complex c1; DComplex c2;
  setNaN(c1); setNaN(c2);
  assert(isNaN(c1) && isNaN(c2));
  c1.real() = 0.0; c2.imag() = 0.0;
  assert(isNaN(c1) && isNaN(c2));
  c1 = Complex(0.0); c2 = DComplex(0.0);
  assert((!isNaN(c1)) && (!isNaN(c2)));

  {
      // Test min/max
      Complex c1(0,1), c2(2,0);
      Complex c3 = min(c1,c2);
      assert(near(c1,c3));
      Complex c4 = max(c1,c2);
      assert(near(c2,c4));
  }

  return(0);
}
