//# NQSinusoid1D.cc: A one dimensional Sinusoid class
//# Copyright (C) 1997,2001
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

#include <aips/Functionals/NQSinusoid1D.h>
#include <aips/Mathematics/Constants.h>
#include <aips/Mathematics/Math.h>

//# Constructors

//# Operators
template<class T> 
T NQSinusoid1D<T>::eval(typename NQFunction1D<T>::FunctionArg x) const {
  return param_p[AMPLITUDE]*
    cos(T(C::_2pi)*(x[0] - param_p[X0])/param_p[PERIOD]);
}

//# Member functions
template<class T>
Function<typename FunctionTraits<T>::DiffType>
*NQSinusoid1D<T>::cloneAD() const {
  Function<typename FunctionTraits<T>::DiffType> *t =
    new NQSinusoid1D<typename FunctionTraits<T>::DiffType>();
  for (uInt i=0; i<nparameters(); ++i) {
    (*t)[i] = typename FunctionTraits<T>::DiffType(param_p[i]);
  };
  return t;
}

template<class T>
Function<typename FunctionTraits<T>::BaseType>
*NQSinusoid1D<T>::cloneBase() const {
  Function<typename FunctionTraits<T>::BaseType> *t =
    new NQSinusoid1D<typename FunctionTraits<T>::BaseType>();
  for (uInt i=0; i<nparameters(); ++i) {
    (*t)[i] = typename FunctionTraits<T>::DiffType(param_p[i]).value();
  };
  return t;
}
