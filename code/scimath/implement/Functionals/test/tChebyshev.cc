//# tNQChebyshev: test the NQChebyshev class
//# Copyright (C) 2000,2001,2002
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

#ifdef DEBUG 
#define DIAGNOSTICS
#endif

#include <aips/Functionals/NQChebyshev.h>
#include <aips/Functionals/NQPolynomial.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Mathematics/Math.h>
#include <aips/Mathematics/AutoDiffA.h>
#include <aips/Mathematics/AutoDiffMath.h>
#include <aips/Mathematics/AutoDiffIO.h>
#include <aips/Utilities/Assert.h>
#include <aips/iostream.h>

int main() {
    NQChebyshev<Double> cheb;

    Vector<Double> coeffs(4, 0);
    coeffs(3) = 2.0;
    cheb.setCoefficients(coeffs);

#ifdef DIAGNOSTICS
    cout << "NQChebyshev " << coeffs;
#endif
    cheb.chebyshevToPower(coeffs);
#ifdef DIAGNOSTICS
    cout << " maps to polynomials coeffs: " << coeffs << endl;
#endif
    AlwaysAssertExit(coeffs(0) == 0 && coeffs(1) == -6 && 
		     coeffs(2) == 0 && coeffs(3) ==  8   );
#ifdef DIAGNOSTICS
    cout << "And power series coeffs " << coeffs;
#endif
    cheb.powerToNQChebyshev(coeffs);
#ifdef DIAGNOSTICS
    cout << " maps to chebyshev coeffs: " << coeffs << endl;
    cout << "coeffs: " << coeffs << ", " << cheb << endl;
#endif
    AlwaysAssertExit(coeffs(0) == cheb.getCoefficient(0) && 
		     coeffs(1) == cheb.getCoefficient(1) && 
		     coeffs(2) == cheb.getCoefficient(2) && 
		     coeffs(3) == cheb.getCoefficient(3)   );

    coeffs = 2.0;
    coeffs(0) += 1.0;
    cheb.setCoefficients(coeffs);
#ifdef DIAGNOSTICS
    cout << "NQChebyshev " << coeffs;
#endif
    cheb.chebyshevToPower(coeffs);
#ifdef DIAGNOSTICS
    cout << " maps to power series coeffs: " << coeffs << endl;
#endif
    AlwaysAssertExit(coeffs(0) == 1 && coeffs(1) == -4 && 
		     coeffs(2) == 4 && coeffs(3) ==  8   );

    Double xmin = 0, xmax = 4, xp;
    cheb.setInterval(xmin, xmax);
    AlwaysAssertExit(xmin == cheb.getIntervalMin());
    AlwaysAssertExit(xmax == cheb.getIntervalMax());

    NQPolynomial<Double> poly(3);
    poly.setCoefficients(coeffs);
    NQChebyshev<Double> chebp = cheb.derivative();

#ifdef DIAGNOSTICS
    Vector<Double> dce;
    dce = chebp.getCoefficients();
    chebp.chebyshevToPower(dce);
    cout << "dcheb: " << dce << endl;
#endif
    NQPolynomial<Double> polyp = poly.derivative();
    polyp.setCoefficients(polyp.coefficients() * (2/(xmax-xmin)));
#ifdef DIAGNOSTICS
    cout << "dpoly: " << polyp.coefficients() << endl;
#endif

    for (Double x=xmin; x <= xmax; x += 0.1) {
	xp = (2*x-xmin-xmax)/(xmax-xmin);
	AlwaysAssertExit(nearAbs(cheb(x), poly(xp), 1.0e-14));
	AlwaysAssertExit(nearAbs(chebp(x), polyp(xp), 1.0e-14));
    }

    // Test auto differentiation wrt x
    NQChebyshev<AutoDiffA<Double> > chebad(cheb.nparameters());
    chebad.setInterval(cheb.getIntervalMin(), cheb.getIntervalMax());
    ///    for (uInt i=0; i<4; ++i) chebad[i] = AutoDiffA<Double>(cheb[i]);
    for (uInt i=0; i<4; ++i) chebad[i] = cheb[i];
    for (AutoDiffA<Double> x(xmin, 1, 0); x <= xmax; x += 0.1) {
      AlwaysAssertExit(nearAbs(chebp(x.value()), chebad(x).deriv(0), 1.0e-14));
    };

    // test out-of-interval modes
    AlwaysAssertExit(0 == cheb.getDefault());
    cheb.setDefault(5);
    AlwaysAssertExit(5 == cheb.getDefault());
    AlwaysAssertExit(cheb(xmin-1) == cheb.getDefault());
    cheb.setOutOfIntervalMode(NQChebyshev<Double>::EXTRAPOLATE);
    AlwaysAssertExit(cheb(xmin-0.2) != cheb.getDefault());
    xp = (2*(xmin-0.2)-xmin-xmax)/(xmax-xmin);
#ifdef DIAGNOSTICS
    cout << xmin-0.2 << ": cheb-poly=" << cheb(xmin-0.2)-poly(xp) << endl;
#endif
    AlwaysAssertExit(nearAbs(cheb(xmin-0.2), poly(xp), 1.0e-14));
    cheb.setOutOfIntervalMode(NQChebyshev<Double>::CYCLIC);
#ifdef DIAGNOSTICS
    cout << xmin-1.3 << ": cheb(-1.3)-cheb(2.7)=" 
	 << cheb(xmin-1.3)-cheb(xmin-1.3+(xmax-xmin)) << endl;
#endif
    AlwaysAssertExit(nearAbs(cheb(xmin-1.3), 
			     cheb(xmin-1.3+(xmax-xmin)), 1.0e-15));
#ifdef DIAGNOSTICS
    cout << xmax+1.3 << ": cheb(5.3)-cheb(1.3)=" 
	 << cheb(xmin+1.3)-cheb(xmin+1.3-(xmax-xmin)) << endl;
#endif
    AlwaysAssertExit(nearAbs(cheb(xmax+1.3),
			     cheb(xmax+1.3-(xmax-xmin)), 1.0e-15));
    cheb.setOutOfIntervalMode(NQChebyshev<Double>::ZEROTH);
    cheb.setCoefficient(0, 1);
    AlwaysAssertExit(cheb(xmax+1) == cheb.getCoefficient(0));

    cout << "OK" << endl;
    return 0;
}
