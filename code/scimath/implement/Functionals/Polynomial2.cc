//# NQPolynomial2.cc: One dimensional polynomial class specialized for AutoDiff
//# Copyright (C) 2001
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

//# Includes
#include <aips/Functionals/NQPolynomial.h>

//# Constructors

//# Operators
template<class T>
AutoDiff<T> NQPolynomial<AutoDiff<T> >::
eval(Function<AutoDiff<T> >::FunctionArg x) const {
  AutoDiff<T> tmp;
  for (uInt i=0; i<nparameters(); ++i) {
    if (param_p[i].nDerivatives() > 0) {
      tmp = param_p[i];
      break;
    };
  };
  for (uInt j=0; j<tmp.nDerivatives(); j++) tmp.deriv(j) = 0.0;
  // function value
  Int j = nparameters();
  tmp.value() = param_p[--j].value();
  while (--j >= 0) {
    tmp.value() *= x[0].value();
    tmp.value() += param_p[j].value();
  };
  // derivatives
  T dev(1);
  for (uInt i=0; i<nparameters(); ++i) {
    for (uInt j=0; j<param_p[i].nDerivatives(); j++) {
      tmp.deriv(j) += dev*param_p[i].deriv(j);
    };
    dev *= x[0].value();
  };
  return tmp;
}

//# Member functions
