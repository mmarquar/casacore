//# tNQSinusoid1D: Test the NQSinusoid1D class
//# Copyright (C) 2001
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

#include <aips/Functionals/NQSinusoid1D.h>

#include <aips/Mathematics/AutoDiff.h>
#include <aips/Mathematics/AutoDiffA.h>
#include <aips/Mathematics/AutoDiffIO.h>
#include <aips/Mathematics/AutoDiffMath.h>
#include <aips/Mathematics/Constants.h>
#include <aips/Mathematics/Math.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Utilities/Assert.h>

#include <aips/iostream.h>

int main() {
  //     NQSinusoid1D();
  NQSinusoid1D<Double> null;
  AlwaysAssertExit(null.amplitude()==1.0 && 
		   null.period() == 1.0 && null.x0()==0.0);
  // use nearAbs because one value is 0.0, which always
  // causes near() to return False as per the documentation
  AlwaysAssertExit(nearAbs(null(0.25), 0.0) && near(null(0.0), 1.0));
  
  //     NQSinusoid1D(const T& h, const T& c, const T& w);
  //     T amplitude() const
  //     void setAmplitude(const T & amplitude)
  //     T period() const
  //     setPeriod(const T & period)
  //     T x0() const;
  //     void setX0(const T & x0);
  //     virtual Type getAvailableParam(uInt which) const;
  NQSinusoid1D<Double> s1(4.0, 6.0, 8.0);
  AlwaysAssertExit(s1.amplitude()==4.0 && s1.period() == 6.0 &&
		   s1.x0()==8.0);
  const NQSinusoid1D<Double> &cs1 = s1;
  AlwaysAssertExit(cs1.amplitude()==4.0 && cs1.period() == 6.0 &&
		   cs1.x0()==8.0);
  s1.setAmplitude(2.0);
  s1.setPeriod(3.0);
  s1.setX0(4.0);
  AlwaysAssertExit(s1[NQSinusoid1D<Double>::X0] == 4.0 &&
		   s1[NQSinusoid1D<Double>::PERIOD] == 3.0 &&
		   s1[NQSinusoid1D<Double>::AMPLITUDE] == 2.0);
  //     T operator()(const T &x) const;
  AlwaysAssertExit(near(s1(7.0), 2.0));
  Vector<Double> xvec(1);
  xvec = 4.5;
  AlwaysAssertExit(near(s1(xvec(0)), 1.0));
  xvec = 5.125;
  AlwaysAssertExit(near(s1(xvec(0)), -2.0/sqrt(2.)));
  
  // test specialized AutoDiff 
  NQSinusoid1D<AutoDiff<Double> > s5;
  s5.setAmplitude(AutoDiff<Double>(2.0, 3, 0));
  s5.setPeriod(AutoDiff<Double>(3.0, 3, 1));
  s5.setX0(AutoDiff<Double>(4.0, 3, 2));
  Double y50 = s5(4.5).value();
  Vector<Double> y51;
  y51 = s5(4.5).derivatives();
  cout << "AutoDiff:  " << s5(4.5) << endl;
  Double y1 = C::_2pi * 0.5/3.0;
  AlwaysAssertExit(near(y50, 1.0) &&
  		   near(y51(0), cos(y1)) &&
  		   near(y51(1), 2.0/3.0*y1*sin(y1)) &&
  		   near(y51(2), 2.0/3.0*C::_2pi*sin(y1)));

  // Generic AutoDiff
  NQSinusoid1D<AutoDiffA<Double> > s6;
  s6.setAmplitude(AutoDiffA<Double>(2.0, 3, 0));
  s6.setPeriod(AutoDiffA<Double>(3.0, 3, 1));
  s6.setX0(AutoDiffA<Double>(4.0, 3, 2));
  Double y60 = s6(AutoDiffA<Double>(4.5)).value();
  Vector<Double> y61;
  y61 = s6(4.5).derivatives();
  cout << "AutoDiffA: " << s6(4.5) << endl;
  AlwaysAssertExit(near(y60, 1.0) &&
		   near(y61(0), cos(y1)) &&
		   near(y61(1), 2.0/3.0*y1*sin(y1)) &&
		   near(y61(2), 2.0/3.0*C::_2pi*sin(y1)));
   
  //   NQSinusoid1D(const NQSinusoid1D &other);
  //   NQSinusoid1D<T> &operator=(const NQSinusoid1D<T> &other);
  //   virtual uInt nAvailableParams() const;
  //   virtual void setAvailableParam(uInt which, const Type &value);
  //   virtual Type getAvailableParam(uInt which) const;
  //   virtual void setAvailableParamMask(uInt which, const Bool mask);
  //   virtual Bool getAvailableParamMask(uInt which) const;
  NQSinusoid1D<Double> s2(s1);
  NQSinusoid1D<Double> s3; s3 = s2;
  AlwaysAssertExit(s1.nparameters() == 3);
  Vector<Double> parms = s1.parameters().getParameters();
  AlwaysAssertExit(parms(0) == 2.0 && parms(1) == 3.0 && parms(2) == 4.0);
  AlwaysAssertExit(allEQ(parms, s2.parameters().getParameters()) &&
  		   allEQ(parms, s3.parameters().getParameters()));
  s1.mask(NQSinusoid1D<Double>::PERIOD) = False;
  AlwaysAssertExit(s1.parameters().nMaskedParameters() == 2);
  Vector<Double> parms2 = s1.parameters().getMaskedParameters();
  AlwaysAssertExit(parms2(0) == 2.0 && parms2(1) == 4.0);
  s1.mask(NQSinusoid1D<Double>::PERIOD) = True;
  s1[0] = 1.0; 
  s1[1] = 2.0; 
  s1[2] = 3.0; 
  AlwaysAssertExit(s1.amplitude()==1.0 && s1.period() == 2.0 &&
		   s1.x0()==3.0);
  parms = 11.0;
  s1.parameters().setParameters(parms);
  AlwaysAssertExit(allEQ(s1.parameters().getParameters(), 11.0));
  
  //   virtual Function1D<T,T> *cloneFunction1D() const;
  //   ~NQSinusoid1D();
  Function<Double> *s4ptr = s1.clone();

    AlwaysAssertExit(allEQ(s4ptr->parameters().getParameters(), 11.0));
    delete s4ptr;
  
  cout << "OK" << endl;
  return 0;
}
