//# NQCompoundParam.h: Parameters for sum of parameterized Functions
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
//#
//# $Id$

#if !defined(AIPS_NQCOMPOUNDPARAM_H)
#define AIPS_NQCOMPOUNDPARAM_H

#include <aips/aips.h>
#include <aips/Functionals/Function.h>
#include <aips/Containers/Block.h>

// <summary> Parameters for sum of parameterized Functions
// </summary>

// <use visibility=local>

// <reviewed reviewer="tcornwel" date="1996/02/22" tests="tNQCompoundFunction" 
// demos="">
// </reviewed>
//
// <prerequisite>
//   <li> <linkto class="Function">Function</linkto>
// </prerequisite>
//
// <synopsis>
// This class takes an arbitrary number of Function objects, and generates
// a new, single function object. The parameters of the compound object
// are the union of all the parameters in the input objects.
//
// When NQCompoundFunction is evaluated, the result is the sum of 
// all the individual function values.
//
// Note that any Function object (including another Compound object) can be
// part of a compound object. 
// </synopsis>
//
// <example>
// Suppose for some reason we wanted the sum of <src>x^2</src> plus a gaussian.
// We could form it as follows:
// <srcblock>
//    NQPolynomial<Float> x2(2);
//    x[2] = 1.0; 					 // x^2
//    NQGaussian1D<Float> gauss(1.0, 0.0, 1.0);          // e^{-x^2}
//    NQCompoundParam<Float> sum;                        // sum == 0.0
//    sum.addFunction(x2);                               // sum == x^2
//    sum.addFunction(gauss);                            // sum == x^2+e^{-x^2}
//    sum(2.0);                                          // == 4 + e^-4
//    NQCompoundParam[0] = 2.0;                          // sum ==2x^2+e^{-x^2}
//    sum(2.0);                                          // == 8 + e^-4
//    // Set the height of the gaussian
//    sum[parameterOffset[1] + NQGaussian1D<Float>::HEIGHT] = 2.5;
// </srcblock>
// </example>

// <templating arg=T>
//  <li> T should have standard numerical operators and exp() function. Current
//	implementation only tested for real types.
//  <li> To obtain derivatives, the derivatives should be defined.
// </templating>

// <thrown>
// <li> AipsError if dimensions of functions added different
// </thrown>

// <motivation>
// This class was created to allow a non-linear least squares fitter to fit a 
// (potentially) arbitrary number of functions (typically gaussians).
// </motivation>
//
// <todo asof="2001/10/22">
//   <li> Nothing I know of
// </todo>

template<class T> class NQCompoundParam : public Function<T> {
 public:
  //# Constructors
  // The default constructor -- no functions, no parameters, nothing, the
  // function operator returns a 0.
  NQCompoundParam();
  // Make this object a (deep) copy of other.
  // <group>
  NQCompoundParam(const NQCompoundParam<T> &other);
  NQCompoundParam<T> 
    &operator=(const NQCompoundParam<T> &other);
  // </group>
  
  ~NQCompoundParam();

  //# Operators
  // Manipulate the nth parameter (0-based) with no index check
  // <group>
  T &operator[](const uInt n);
  const T &operator[](const uInt n) const;
  // </group>
  
  //# Member functions
  
  // Add a function to the sum. All functions must have the same 
  // <src>ndim()</src> as the first one. Returns the (zero relative) number 
  // of the function just added.
  uInt addFunction(const Function<T> &newFunction);
  
  // Return the number of functions in the sum.
  uInt nFunctions() const { return functionPtr_p.nelements(); };
  
  // Return a reference to a specific Function.
  // <group>
  const Function<T> &function(uInt which) const {
    DebugAssert(nFunctions() > which, AipsError);
    return *(functionPtr_p[which]); };
  const Function<T> &function(uInt which) {
    DebugAssert(nFunctions() > which, AipsError);
    return *(functionPtr_p[which]); };
  // </group>
  // Get the offset in function parameterlist for function which
  const uInt parameterOffset(uInt which) const {
    DebugAssert(nFunctions() > which, AipsError); return paroff_p[which]; };
  // Get the function number belonging to parameter list element which
  const uInt parameterFunction(uInt which) const {
    DebugAssert(nparameters() > which, AipsError); return funpar_p[which]; };
  // Returns the dimension of functions in the linear combination
  virtual uInt ndim() const { return ndim_p; };
  // Manipulate the mask associated with the nth parameter
  // (e.g. to indicate whether the parameter should be adjusted or not).
  // Note no index check.
  // <group>
  Bool &mask(const uInt n);
  Bool mask(const uInt n) const;
  // </group>
  // Return the parameter interface
  // <group>
  const FunctionParam<T> &parameters() const;
  FunctionParam<T> &parameters();
  // </group>

private:
  //# Data
  // Number of dimensions of underlying functions
  uInt ndim_p;
  // Set if the main parameter list could have been updated
  mutable Bool updated_p;
  // Pointer to each added function
  PtrBlock<Function<T> *> functionPtr_p;
  // Index of offset for each function to its parameters in general list
  Block<uInt> paroff_p;
  // Index of function belonging to parameter
  Block<uInt> funpar_p;
  // Index of local parameter
  Block<uInt> locpar_p;

  //# Member functions
  // Copy the local parameters to/from general block
  // <group>
  void toParam_p() const;
  void fromParam_p() const;
  // </group>

};

#endif
