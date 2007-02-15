//# EvenPolynomialParam.h: Parameter handling for even polynomials
//# Copyright (C) 2002,2005
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

#ifndef SCIMATH_EVENPOLYNOMIALPARAM_H
#define SCIMATH_EVENPOLYNOMIALPARAM_H

//# Includes
#include <casa/aips.h>
#include <scimath/Functionals/Function1D.h>
#include <casa/Utilities/Assert.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//# Forward declarations
template<class T> class Vector;

// <summary>  Parameter handling for even polynomials
// </summary>

// <reviewed reviewer="tcornwel" date="1996/02/22" tests="tSpecialPolynomial"
// demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class="FunctionParam">FunctionParam</linkto> class
//   <li> <linkto class=Function1D>Function1D</linkto>
// </prerequisite>

// <etymology> 
// A 1-dimensional EvenPolynomial's parameters.
// </etymology>
//
// <synopsis> 
// An <src>EvenPolynomial</src> is described by a set of coefficients;
// its fundamental operation is evaluating itself at some "x".
// The number of coefficients is the order of the polynomial divided
// by two, plus one.
//
// Since the <src>EvenPolynomial</src> is a <src>Function</src>,
// the derivatives can be obtained as well. 
//
// The parameter interface (see 
// <linkto class="FunctionParam">FunctionParam</linkto> class), 
// is used to provide an interface to the
// <linkto module="Fitting">Fitting</linkto> classes. 
//
// This class is in general used implicitly by the <src>EvenPolynomial</src>
// class only.
// </synopsis> 
//
// <example>
// <srcblock>
//  EvenPolynomial<Float> pf(3);  // Second order polynomial - coeffs 0 by default
//  pf.setCoefficient(0, 1.0);
//  pf[1] = 2.0;		// 2x^2 + 1x^0
//  pf(2); // == 8
// </srcblock>
// </example>

// <templating arg=T>
//  <li> T should have standard numerical operators. Current
//	implementation only tested for real types (and their AutoDiffs).
// </templating>

// <thrown>
//    <li> Assertion in debug mode if attempt is made to address incorrect
//		coefficients
// </thrown>

// <todo asof="2002/02/26">
//   <li> Nothing I know of
// </todo>

template<class T> class EvenPolynomialParam: public Function1D<T>
{
public:
  //# Constructors
  // Constructs a zero'th order polynomial, with a coeficcient of 0.0.
  EvenPolynomialParam();
  
  // Makes a polynomial of the given order, with all coeficcients set to
  // zero. 
  explicit EvenPolynomialParam(uInt order);
  
  // Make this a copy of other (deep copy).
  // <group>
  EvenPolynomialParam(const EvenPolynomialParam<T> &other);
  template <class W>
    EvenPolynomialParam(const EvenPolynomialParam<W> &other) :
    Function1D<T>(other) {}
  EvenPolynomialParam<T> &operator=(const EvenPolynomialParam<T> &other);
  // </group>
  
  // Destructor
  ~EvenPolynomialParam();

  //# Operators  
  // Comparisons.  
  // EvenPolynomials are equal if they are the same order
  // <group>
  Bool operator==(const EvenPolynomialParam<T> &other) const {
    return (param_p == other.param_p); };
  Bool operator!=(const EvenPolynomialParam<T> &other) const {
    return (param_p != other.param_p); };
  // </group>

  //# Member functions
  // Give name of function
  virtual const String &name() const { static String x("evenpolynomial");
    return x; };

  // What is the order of the polynomial, i.e. maximum exponent of "x".
  uInt order() const { return 2*param_p.nelements() - 2; };
  
  // What is the <em>which</em>'th coefficient of the polynomial. For an nth
  // degree polynomial, <em>which</em> varies between zero and n/2.
  T coefficient(uInt which) const {
    DebugAssert(which<=order(), AipsError); return param_p[which]; };
  
  // Return all the coefficients as a vector.
  const Vector<T> &coefficients() const;

  // Set the <em>which</em>'th coefficient to <em>value</em>. 
  void setCoefficient(uInt which, const T value) {
    DebugAssert(which<=order(), AipsError); param_p[which] = value; };
  
  // Set all the coefficients at once, throw away all existing coefficients.
  void setCoefficients(const Vector<T> &coefficients);

  //# Make members of parent classes known.
protected:
  using Function1D<T>::param_p;
public:
  using Function1D<T>::nparameters;
};


} //# NAMESPACE CASA - END

#ifndef AIPS_NO_TEMPLATE_SRC
#include <scimath/Functionals/EvenPolynomialParam.tcc>
#endif //# AIPS_NO_TEMPLATE_SRC
#endif
