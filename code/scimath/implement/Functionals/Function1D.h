//# NQFunction1D.h: Numerical functional interface class for 1 dimension
//# Copyright (C) 2001,2002
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

#if !defined(AIPS_NQFUNCTION1D_H)
#define AIPS_NQFUNCTION1D_H

//# Includes
#include <aips/aips.h>
#include <aips/Functionals/Function.h>

//# Forward declarations

// <summary> Numerical functional interface class for 1 dimension
// </summary>

// <use visibility=export>

// <reviewed reviewer="tcornwel" date="1996/02/22" tests="tNQGaussian1D"
//	 demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class="Function">Function</linkto>
// </prerequisite>
//
// <synopsis>
// A <src>Function1D</src> is used for classes which map a
// scalar or n-dimensional Vector of type <src>T</src> into a <src>T</src>.
// The object also has one parameter which can be masked
// if necessary, and be used in the <src>Fitting</src> module, and, implicitly,
// in the <linkto class=AutoDiff>AutoDiff</linkto> differentiation module.
//
// The only method implemented in <src>Function1D</src> is the
// <src>ndim()</src> method. The rest is inhereted from
// <linkto class="Function">Function</linkto>.
// </synopsis>

// <example>
// See <linkto class=Function>Function</linkto>.
// </example>
//
// <templating arg=T>
//    <li> Besides the requirements set by the
//    <linkto class="Functional">Functional</linkto> base class, it must be
//    possible to form a <src>Vector<T></src>.
// </templating>


template<class T, class U=T> class NQFunction1D : public Function<T,U> {

 public:
  //# Typedefs
  typedef const T* FunctionArg;

  //# Constructors
  // Constructors for FunctionParam
  // <group>
  NQFunction1D() : Function<T,U>() {};
  explicit NQFunction1D(const uInt n) : Function<T,U>(n) {};
  explicit NQFunction1D(const Vector<T> &in) : Function<T,U>(in) {};
  NQFunction1D(const FunctionParam<T> &other) : Function<T,U>(other) {};
  // </group>

  // Destructor
  virtual ~NQFunction1D() {};
  
  // Returns the number of dimensions of function
  virtual uInt ndim() const { return 1; }

};

#endif
