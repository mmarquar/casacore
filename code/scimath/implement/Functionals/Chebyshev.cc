//# NQChebyshev.cc  a function class that defines a NQChebyshev polynomial
//# Copyright (C) 2000,2001
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
#include <aips/Functionals/NQChebyshev.h>

//# Constructors

//# Operators
template <class T>
T NQChebyshev<T>::eval(typename NQFunction1D<T>::FunctionArg x) const {
  T xp = x[0];
  // handle out-of-interval values
  if (xp < minx_p || xp > maxx_p) {
    switch (mode_p) {

    case CONSTANT:
      return def_p;

    case ZEROTH:
      return param_p[0];

    case CYCLIC: {
      T period = (maxx_p-minx_p);
      while (xp < minx_p) xp += period;
      while (xp > maxx_p) xp -= period;
    }
    break;

    case EDGE: {
      T tmp(0);
      if (xp<minx_p) {
	for (uInt i=0; i<nparameters(); i+=2) tmp += param_p[i];
	for (uInt i=1; i<nparameters(); i+=2) tmp -= param_p[i];
      } else {
	for (uInt i=0; i<nparameters(); ++i) tmp += param_p[i];
      };
      return tmp;
    }
    break;

    default:
      break;
    }
  };

  T yi1=T(0);
  T yi2=T(0);
  T tmp;

  // map Chebeshev range [minx_p, maxx_p] into [-1, 1]
  xp = (T(2)*xp-minx_p-maxx_p)/(maxx_p-minx_p);
  // evaluate using Clenshaw recursion relation
  for (Int i=nparameters()-1; i>0; i--) {
    tmp = T(2)*xp*yi1 - yi2 + param_p[i];
    yi2 = yi1;
    yi1 = tmp;
  };
  return xp*yi1 - yi2 + param_p[0];
}

template <class T>
NQChebyshev<T> NQChebyshev<T>::derivative() const {
    Vector<T> ce(nparameters());
    ce = parameters().getParameters();
    derivativeCoeffs(ce, minx_p, maxx_p);
    return NQChebyshev<T>(ce, minx_p, maxx_p, mode_p, T(0));
}

//# Member functions
