//# NQGaussian2DParam.cc: Parameter handling for 2 dimensional Gaussian class
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
#include <aips/Functionals/NQGaussian2DParam.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/Vector.h>
#include <aips/Exceptions/Error.h>
#include <aips/Utilities/Assert.h>
#include <aips/Mathematics/Constants.h>
#include <aips/Mathematics/Math.h>

//# Statics
///template<class T>
///const T NQGaussian2DParam<T>::fwhm2int = T(1.0)/sqrt(log(T(16.0)));

//# Constructors
template<class T>
NQGaussian2DParam<T>::NQGaussian2DParam() :
  Function<T>(6),
  fwhm2int(T(1.0)/sqrt(log(T(16.0)))),
  theSpa(T(0.0)), theCpa(T(1.0)) {
  param_p[HEIGHT] = T(1.0);
  param_p[XCENTER] = T(0.0);
  param_p[YCENTER] = T(0.0);
  param_p[YWIDTH] = T(1.0);
  param_p[RATIO] = T(1.0);
  param_p[PANGLE] = T(0.0);
  theXwidth = T(1.0);
}

template<class T>
NQGaussian2DParam<T>::NQGaussian2DParam(const T &height, const T &xCenter,
				    const T &yCenter,
				    const T &majorAxis, const T &axialRatio,
				    const T &pa) :
  Function<T>(6),
  fwhm2int(T(1.0)/sqrt(log(T(16.0)))) {
  param_p[HEIGHT] = height;
  param_p[XCENTER] = xCenter;
  param_p[YCENTER] = yCenter;
  param_p[YWIDTH] = majorAxis;
  param_p[RATIO] = T(1.0);
  param_p[PANGLE] = T(0.0);
  theXwidth = T(0.0);
  setMajorAxis(majorAxis);
  setAxialRatio(axialRatio);
  setPA(pa);
}

template<class T>
NQGaussian2DParam<T>::NQGaussian2DParam(const T &height, const Vector<T> &center, 
				    const Vector<T> &width, const T &pa) :
  Function<T>(6),
  fwhm2int(T(1.0)/sqrt(log(T(16.0)))) {
  param_p[HEIGHT] = height;
  param_p[YWIDTH] = T(0.0);
  theXwidth = T(0.0);
  setCenter(center);
  setWidth(width);
  setPA(pa);
}

template<class T>
NQGaussian2DParam<T>::NQGaussian2DParam(const NQGaussian2DParam<T> &other) :
  Function<T>(other),
  fwhm2int(T(1.0)/sqrt(log(T(16.0)))) {
  theXwidth = other.theXwidth;
  theSpa = other.theSpa;
  theCpa = other.theCpa;
}

template<class T>
NQGaussian2DParam<T>::~NQGaussian2DParam() {}

//# Operators
template<class T>
NQGaussian2DParam<T> &
NQGaussian2DParam<T>::operator=(const NQGaussian2DParam<T> &other) {
  if (this != &other) {
    fwhm2int = other.fwhm2int;
    Function<T>::operator=(other);
    theXwidth = other.theXwidth;
    theSpa = other.theSpa;
    theCpa = other.theCpa;
  };
  return *this;
}

//# Member functions
template<class T>
T NQGaussian2DParam<T>::flux() const {
  return param_p[HEIGHT]*abs(param_p[YWIDTH]*theXwidth*
			     fwhm2int*fwhm2int*T(C::pi));
}

template<class T>
void NQGaussian2DParam<T>::setFlux(const T &flux) {
  param_p[HEIGHT] = flux/abs(param_p[YWIDTH]*theXwidth*T(C::pi))/
    fwhm2int/fwhm2int;
}

template<class T>
Vector<T> NQGaussian2DParam<T>::center() const {
  Vector<T> center(2);
  center(0) = param_p[XCENTER];
  center(1) = param_p[YCENTER];
  return center;
}

template<class T>
void NQGaussian2DParam<T>::setCenter(const Vector<T> &center) {
  DebugAssert(center.nelements() == 2, AipsError);
  param_p[XCENTER] = center(0);
  param_p[YCENTER] = center(1);
}

template<class T>
Vector<T> NQGaussian2DParam<T>::width() const {
  Vector<T> width(2);
  width(0) = majorAxis();
  width(1) = minorAxis();
  return width;
}

template<class T>
void NQGaussian2DParam<T>::setWidth(const Vector<T> &width) {
  DebugAssert(width.nelements() == 2, AipsError);
  if (abs(width(0)) > minorAxis()) {
    setMajorAxis(width(0));
    setMinorAxis(width(1));
  } else {
    setMinorAxis(width(1));
    setMajorAxis(width(0));
  };
}

template<class T>
T NQGaussian2DParam<T>::majorAxis() const {
  return max(abs(param_p[YWIDTH]), abs(theXwidth));
}

template<class T>
void NQGaussian2DParam<T>::setMajorAxis(const T &width) {
  if (width <= T(0.0)) {
    throw(AipsError("NQGaussian2DParam<T>::setMajorAxis(const T &width)"
		    " - width must be positive"));
  };
  // The near function is necessary for Intel processors (and doesn't hurt for
  // other architectures) because of the extra precision that floating point
  // variables have when returned in floating point registers. See
  // http://aips2.nrao.edu/mail/aips2-lib/1101 for a discussion of this. The
  // near function was added here and in the setMinorAxis function to fix
  // defect AOCso00071
  const T minorWidth = minorAxis();
  if (width < minorWidth && !near(width, minorWidth)) {
    throw(AipsError("NQGaussian2DParam<T>::setMajorAxis(const T &width)"
		    " - major axis is smaller than minor axis"));
  };
  if (abs(theXwidth) > abs(param_p[YWIDTH])) theXwidth = width;
  else param_p[YWIDTH] = width;
  param_p[RATIO] = theXwidth/param_p[YWIDTH];
}

template<class T>
T NQGaussian2DParam<T>::minorAxis() const {
  return min(abs(param_p[YWIDTH]),abs(theXwidth));
}

template<class T>
void NQGaussian2DParam<T>::setMinorAxis(const T &width) {
  if (width <= T(0.0)) {
    throw(AipsError("NQGaussian2DParam<T>::setMinorAxis(const T &width)"
		    " - width must be positive"));
  };
  const T majorWidth = majorAxis();
  if (width > majorWidth && !near(width, majorWidth)) {
    throw(AipsError("NQGaussian2DParam<T>::setMinorAxis(const T &width)"
		    " - minor axis is greater than major axis"));
  };
  if (abs(theXwidth) <= abs(param_p[YWIDTH])) theXwidth = width;
  else param_p[YWIDTH] = width;
  param_p[RATIO] = theXwidth/param_p[YWIDTH];
}

template<class T>
T NQGaussian2DParam<T>::axialRatio() const {
  return minorAxis()/majorAxis();
}

template<class T>
void NQGaussian2DParam<T>::setAxialRatio(const T &axialRatio) {
  if (axialRatio <= T(0.0) || axialRatio > T(1.0)) {
    throw(AipsError("NQGaussian2DParam<T>::setAxialRatio(const T &axialRatio)"
		    " - axialRatio must be between (0,1]"));
  };
  setMinorAxis(axialRatio*majorAxis());
}

template<class T>
T NQGaussian2DParam<T>::PA() const {
  T pa;
  if (abs(param_p[YWIDTH]) >= abs(theXwidth)) pa = fmod(param_p[PANGLE],
							T(C::pi));
  else pa = fmod(param_p[PANGLE]+T(C::pi_2), T(C::pi));
  if (pa < T(0.0)) pa += T(C::pi);
  return pa;
}

template<class T>
void NQGaussian2DParam<T>::setPA(const T &pa) {
  if (abs(pa) > T(C::_2pi)) {
    throw(AipsError("NQGaussian2DParam<T>::setPA(const T &pa)"
		    " - PA must be in radians and between -2pi and 2pi"));
  };
  if (abs(param_p[YWIDTH]) >= abs(theXwidth)) param_p[PANGLE] = pa;
  else param_p[PANGLE] = pa - T(C::pi_2);
  theCpa = cos(param_p[PANGLE]);
  theSpa = sin(param_p[PANGLE]);
}
