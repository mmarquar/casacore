//# tNQHyperPlane.cc: Test the NQHyperPlane class
//# Copyright (C) 2001,2002
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

#include <aips/Functionals/HyperPlane.h>

#include <aips/Mathematics/AutoDiff.h>
#include <aips/Mathematics/AutoDiffA.h>
#include <aips/Mathematics/AutoDiffIO.h>
#include <aips/Mathematics/AutoDiffMath.h>
#include <aips/Mathematics/Constants.h>
#include <aips/Mathematics/Math.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Utilities/Assert.h>

#include <aips/iostream.h>

int main() {

  try {
    // Construct an m dimensional hyper plane which has m+1 coefficients.  By 
    // default, the coefficients are initialized to zero.
    //NQHyperPlane(uInt m);
    NQHyperPlane<Double> hyper(3);
    
    // Make this object a copy of other.
    //NQHyperPlane(const NQHyperPlane<Type> &other);
    NQHyperPlane<Double> comb2(hyper);
    
    // Make this object a copy of other.
    //NQHyperPlane<Type> &operator=(const NQHyperPlane<Type> &other);
    comb2 = hyper;
    
    // Return the total number of coefficients, which is the dimension of the
    // hyper plane plus one.
    AlwaysAssertExit(hyper.nparameters() == 4 && 
      hyper.nparameters() == comb2.nparameters());
    
    Vector<Double> v(4);
    
    // Set the value of a coefficient. 
    // Get the value of a coefficient.  
    // f(x,y,z) = 10x + 11y + 12*z + 13
    for (uInt i=0; i<4; i++) {
      hyper[i] = i+10;
      AlwaysAssertExit(hyper[i] == Double(i+10));
      v(i) = i+10;
    };
    
    // Set all coefficients at once. 
    // Get all the values of coefficients at once.  
    hyper.parameters().setParameters(v);
    AlwaysAssertExit(allEQ(hyper.parameters().getParameters(), v));
    
    // Evaluate the function at <src>v</src>. 
    // f(x,y,z) = 10x + 11y + 12*z + 13
    AlwaysAssertExit((hyper(v) - Double(378)) < 1.e-6);
    
    // Returns the dimension of functions in the linear hyper
    // uInt order() const;
    AlwaysAssertExit(hyper.order() == 3);
    
    // Set coefficients
    for (uInt i=0; i<hyper.nparameters()-2; i++) { 
      hyper.mask(i) = False;
      AlwaysAssertExit(!hyper.mask(i));
    };
    
    AlwaysAssertExit(hyper.parameters().nMaskedParameters() == 2);
    
    for (uInt i=0; i<hyper.parameters().nMaskedParameters(); i++) { 
      AlwaysAssertExit(hyper.parameters().getMaskedParameters()[i] ==
		       Double(12+i));
    };
  
    // test specialized AutoDiff 
    // f(x,y,z) = 10x + 11y + 12*z + 13
    Vector<AutoDiffA<Double> > v6(4);
    NQHyperPlane<AutoDiff<Double> > s5(3);
    for (uInt i=0; i<4; i++) {
      s5[i] = AutoDiff<Double>(i+10,4,i);
      AlwaysAssertExit(s5[i] == Double(i+10));
      v[i] = i+10;
      v6(i) = i+10;
    };
    Double y50 = s5(v).value();
    Vector<Double> y51;
    y51 = s5(v).derivatives();
    cout << "AutoDiff:  " << s5(v) << endl;
    
    // Generic AutoDiff
    NQHyperPlane<AutoDiffA<Double> > s6(3);
    for (uInt i=0; i<4; i++) {
      s6[i] = AutoDiffA<Double>(i+10,4,i);
      AlwaysAssertExit(s6[i].value() == Double(i+10));
      v6(i) = i+10;
    };
    Double y60 = s6(v6).value();
    Vector<Double> y61;
    y61 = s6(v6).derivatives();
    cout << "AutoDiffA: " << s6(v6) << endl;
    AlwaysAssertExit(near(y60, y50) &&
		     near(y61(0), y51[0]) &&
		     near(y61(1), y51[1]) &&
		     near(y61(2), y51[2]) &&
		     near(y61(3), y51[3]));
    
  } catch (AipsError x) {
    cout << "Exception : " << x.getMesg() << endl;
  } 

  cout << "OK" << endl;
  return 0;
}

