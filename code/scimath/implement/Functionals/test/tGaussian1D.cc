//# tGaussian1D: Test the Gaussian1D class
//# Copyright (C) 1995,1996,1997,1999,2001,2002
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

#include <scimath/Functionals/Gaussian1D.h>

#include <scimath/Mathematics/AutoDiff.h>
#include <scimath/Mathematics/AutoDiffA.h>
#include <scimath/Mathematics/AutoDiffIO.h>
#include <scimath/Mathematics/AutoDiffMath.h>
#include <casa/BasicSL/Constants.h>
#include <casa/BasicMath/Math.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Utilities/Assert.h>

#include <casa/iostream.h>

int main() {
  Gaussian1D<Double> null;
  AlwaysAssertExit(null.height() == 1.0 && 
		   null.center() == 0.0 && null.width() == 1.0);
  AlwaysAssertExit(near(null(0.5), 0.5) && near(null(0.0), 1.0));
  
  //     Gaussian1D(const T& h, const T& c, const T& w);
  //     T height() const
  //     void setHeight(const T & height)
  //     T flux() const;
  //     void setFlux(const T & flux);
  //     T center() const
  //     setCenter(const T & center)
  //     T width() const;
  //     void setWidth(const T & width);
  Gaussian1D<Double> gauss1(4.0, 6.0, 8.0);
  AlwaysAssertExit(gauss1.height() == 4.0 && gauss1.center() == 6.0 &&
		   gauss1.width() == 8.0);
  const Gaussian1D<Double> &cgauss1 = gauss1;
  AlwaysAssertExit(cgauss1.height() == 4.0 && cgauss1.center() == 6.0 &&
		   cgauss1.width() == 8.0);
  gauss1.setHeight(2.0);
  AlwaysAssertExit(near(gauss1.flux(), 2.0*8.0*sqrt(C::pi/log(16.0))));
  gauss1.setCenter(3.0);
  gauss1.setWidth(4.0);
  gauss1.setFlux(1.0);
  AlwaysAssertExit(gauss1[Gaussian1D<Double>::WIDTH] == 4.0 &&
		   gauss1[Gaussian1D<Double>::CENTER] == 3.0 &&
		   near(gauss1[Gaussian1D<Double>::HEIGHT] , 
			1.0/4.0/sqrt(C::pi/log(16.0))));
  gauss1[Gaussian1D<Double>::HEIGHT] = 2.0;
  // <<
  cout << "Function Parameters: " << gauss1 << endl;
  //     T operator()(const T &x) const;
  AlwaysAssertExit(near(gauss1(3.0), 2.0));
  Vector<Double> xvec(1);
  xvec = 5.0;
  cout << "Value at 5:      " << gauss1(xvec(0)) << endl;
  AlwaysAssertExit(near(gauss1(xvec(0)), 2.0 / 2.0));
  xvec = -1.0;
  AlwaysAssertExit(near(gauss1(xvec(0)), 2.0/2.0/2.0/2.0/2.0));
  
  // Test Auto differentiation - specialized
  Gaussian1D<AutoDiff<Double> > gauss5(AutoDiff<Double>(4.0),
					 AutoDiff<Double>(6.0),
					 AutoDiff<Double>(8.0));
  AlwaysAssertExit(gauss5.height() == 4.0 && gauss5.center() == 6.0 &&
		   gauss5.width() == 8.0);
  const Gaussian1D<AutoDiff<Double> > &cgauss5 = gauss5;
  AlwaysAssertExit(cgauss5.height() == 4.0 && cgauss5.center() == 6.0 &&
		   cgauss5.width() == 8.0);
  gauss5.setHeight(AutoDiff<Double>(2.0));
  AlwaysAssertExit(near(gauss5.flux(), 2.0*8.0*sqrt(C::pi/log(16.0))));
  gauss5.setCenter(AutoDiff<Double>(3.0,3,1));
  gauss5.setWidth(AutoDiff<Double>(4.0,3,2));
  gauss5.setFlux(AutoDiff<Double>(1.0,3,0));
  AlwaysAssertExit(gauss5[Gaussian1D<AutoDiff<Double> >::WIDTH] == 4.0 &&
		   gauss5[Gaussian1D<AutoDiff<Double> >::CENTER] == 3.0 &&
		   near(gauss5[Gaussian1D<AutoDiff<Double> >::HEIGHT] , 
			1.0/4.0/sqrt(C::pi/log(16.0))));
  gauss5.setHeight(AutoDiff<Double>(2.0,3,0));
  cout << "Specialized(3):  " << gauss5(3.0) << endl;
  cout << "Specialized(5):  " << gauss5(5.0) << endl;
  AlwaysAssertExit(near(gauss1(3.0), 2.0));
  
  // Test Auto differentiation
  Gaussian1D<AutoDiffA<Double> > gauss6(AutoDiffA<Double>(4.0),
					  AutoDiffA<Double>(6.0),
					  AutoDiffA<Double>(8.0));
  AlwaysAssertExit(gauss6.height() == 4.0 && gauss6.center() == 6.0 &&
		   gauss6.width() == 8.0);
  const Gaussian1D<AutoDiffA<Double> > &cgauss6 = gauss6;
  AlwaysAssertExit(cgauss6.height() == 4.0 && cgauss6.center() == 6.0 &&
		   cgauss6.width() == 8.0);
  gauss6.setHeight(AutoDiffA<Double>(2.0));
  AlwaysAssertExit(near(gauss6.flux(), 2.0*8.0*sqrt(C::pi/log(16.0))));
  gauss6.setCenter(AutoDiffA<Double>(3.0,3,1));
  gauss6.setWidth(AutoDiffA<Double>(4.0,3,2));
  gauss6.setFlux(AutoDiffA<Double>(1.0,3,0));
  AlwaysAssertExit(gauss6[Gaussian1D<AutoDiffA<Double> >::WIDTH] == 4.0 &&
		   gauss6[Gaussian1D<AutoDiffA<Double> >::CENTER] == 3.0 &&
		   near(gauss6[Gaussian1D<AutoDiffA<Double> >::HEIGHT] , 
			1.0/4.0/sqrt(C::pi/log(16.0))));
  gauss6.setHeight(AutoDiffA<Double>(2.0,3,0));
  cout << "Generic(3):      " << gauss6(AutoDiffA<Double>(3.0)) << endl;
  cout << "Generic(5):      " << gauss6(AutoDiffA<Double>(5.0)) << endl;
  cout << "Generic(1):      " << gauss6(AutoDiffA<Double>(1.0)) << endl;
  AlwaysAssertExit(near(gauss1(3.0), 2.0));
  
  //     Gaussian1D(const Gaussian1D &other);
  //     Gaussian1D<T> &operator=(const Gaussian1D<T> &other);
  //   virtual uInt nAvailableParams() const;
  //   virtual void setAvailableParam(uInt which, const Type &value);
  //   virtual Type getAvailableParam(uInt which) const;
  //   virtual void setAvailableParamMask(uInt which, const Bool mask);
  //   virtual Bool getAvailableParamMask(uInt which) const;
  Gaussian1D<Double> gauss2(gauss1);
  Gaussian1D<Double> gauss3; gauss3 = gauss2;
  AlwaysAssertExit(gauss1.nparameters() == 3);
  Vector<Double> parms = gauss1.parameters().getParameters();
  AlwaysAssertExit(parms(0) == 2.0 && parms(1) == 3.0 && parms(2) == 4.0);
  AlwaysAssertExit(allEQ(parms, gauss2.parameters().getParameters()) &&
		   allEQ(parms, gauss3.parameters().getParameters()));
  gauss1.mask(Gaussian1D<Double>::CENTER) = False;
  AlwaysAssertExit(gauss1.parameters().nMaskedParameters() == 2);
  Vector<Double> parms2 = gauss1.parameters().getMaskedParameters();
  AlwaysAssertExit(parms2(0) == 2.0 && parms2(1) == 4.0);
  gauss1.mask(Gaussian1D<Double>::CENTER) = True;
  gauss1[0] = 1.0; 
  gauss1[1] = 2.0; 
  gauss1[2] = 3.0;
  AlwaysAssertExit(gauss1.height() == 1.0 && gauss1.center() == 2.0 &&
		   gauss1.width() == 3.0);
  
  AlwaysAssertExit(near(gauss5(5.0).value(),
			gauss6(AutoDiffA<Double>(5.0)).value()) &&
		   allNear(gauss5(5.0).derivatives(),
			   gauss6(AutoDiffA<Double>(5.0)).derivatives(),
			   1e-13));
  parms = 11.0;
  gauss1.parameters().setParameters(parms);
  AlwaysAssertExit(allEQ(gauss1.parameters().getParameters(), 11.0));
  
  // clone();
  //     ~Gaussian1D();
  Function<Double> *gauss4ptr = gauss1.clone();
  AlwaysAssertExit(allEQ(gauss4ptr->parameters().getParameters(), 11.0));
  delete gauss4ptr;

  cout << "OK" << endl;
  return 0;
}
