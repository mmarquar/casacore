//# NQSinusoid1D.h: A one dimensional NQSinusoid class
//# Copyright (C) 1997,2001,2002
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

#if !defined(AIPS_NQSINUSOID1D_H)
#define AIPS_NQSINUSOID1D_H

//# Includes
#include <aips/aips.h>
#include <aips/Functionals/Sinusoid1DParam.h>
#include <aips/Functionals/Function1D.h>
#include <aips/Mathematics/AutoDiff.h>
#include <aips/Mathematics/AutoDiffMath.h>

//# Forward declarations

// <summary> A one dimensional NQSinusoid class.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="tNQSinusoid1D" 
// demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class="NQSinusoid1DParam">NQSinusoid1DParam</linkto>
//   <li> <linkto class="Function">Function</linkto>
// </prerequisite>

// <etymology> 
// A NQSinusoid1D functional is designed for calculating a
// NQSinusoid in one dimension. 
// </etymology>

// <synopsis> 
// A <src>NQSinusoid1D</src> is described by an amplitude, a period,
// and a location of a peak. Its fundamental operation is evaluating itself
// at some <src>x</src>. The
// parameters (amplitude, period, and x0) may be changed at run time. 
//
// The functional form is <src> A*cos(2*pi(x-x0)/P) </src>
//
// The parameter interface (see 
// <linkto class="NQSinusoid1DParam">NQSinusoid1DParam</linkto> class), 
// is used to provide an interface to the
// <linkto module="Fitting"> Fitting </linkto> classes. 
//
// There are 3 parameters that are used to describe the NQSinusoid:
// <ol>
// <li> The amplitude of the NQSinusoid. This is the value 
//      returned using the <src> amplitude </src> member function.
// <li> The period of the NQSinusoid in the x direction. This is 
//      the value returned using the <src> period </src> member function.
//	The period is expressed in full cycles.
// <li> The location of a peak of the NQSinusoid (i.e. where
// <src>x=pi+k.2pi</src>)
// </ol>
//
// An enumeration for the <src>AMPLITUDE</src>, <src>PERIOD</src> and
// <src>X0</src> parameter index is provided, enabling the setting
// and reading of parameters with the <src>[]</src> operator. The 
// <src>mask()</src> methods can be used to check and set the parameter masks.
//
// </synopsis>

// <example>
// <srcblock>
//    NQSinusoid<Double> sf(5.0, 25.0, 7);
//    sf(25);            // = -4.911
//    sf.setAmplitude(1.0);
//    sf[PERIOD] = 2.0;                
//    sf.setX0(0.0);
//    sf(0.5);             // = 1.0
// </srcblock>
// </example>

// <templating arg=T>
//  <li> T should have standard numerical operators and cos() function. Current
//	implementation only tested for real types.
//  <li> To obtain derivatives, the derivatives should be defined.
// </templating>

// <thrown>
//    <li> AipsError if incorrect parameter number specified.
//    <li> Assertion in debug mode if operator(Vector<>) with empty Vector
// </thrown>

template<class T> class NQSinusoid1D : public NQSinusoid1DParam<T> {
 public:
  //# Enumerations
  
  //# Constructors
  // Constructs the NQSinusoids, Defaults:
  //  amplitude=1, period==1, x0=0. I.e. a cosinusoid with <src>cos(x)</src>.
  // <note role=warning> Could not use default arguments
  // that worked both with gcc and IRIX </note>
  // <group>
  NQSinusoid1D() : NQSinusoid1DParam<T>() {};
  explicit NQSinusoid1D(const T &amplitude) :
    NQSinusoid1DParam<T>(amplitude) {};
  NQSinusoid1D(const T &amplitude, const T &period) :
    NQSinusoid1DParam<T>(amplitude, period) {};
  NQSinusoid1D(const T &amplitude, const T &period, const T &x0) :
    NQSinusoid1DParam<T>(amplitude, period, x0) {};
  // </group>

  // Copy constructor (deep copy)
  NQSinusoid1D(const NQSinusoid1D &other) : NQSinusoid1DParam<T>(other) {};

  // Copy assignment (deep copy)
  NQSinusoid1D<T> &operator=(const NQSinusoid1D<T> &other) {
    NQSinusoid1DParam<T>::operator=(other); return *this; };
    
  // Destructor
  virtual ~NQSinusoid1D() {};

  //# Operators    
  // Evaluate the NQSinusoid at <src>x</src>.
  // If a vector is used as the argument only its first element is used.
  // <group>
  virtual T eval(typename NQFunction1D<T>::FunctionArg x) const;
  // </group>
    
  //# Member functions
  // Return a copy of this object from the heap. The caller is responsible 
  // for deleting this pointer. 
  // <group>
  virtual Function<T> *clone() const { return new NQSinusoid1D<T>(*this); };
  // </group>

};

#define NQSinusoid1D_PS NQSinusoid1D
// <summary> Partial specialization of NQSinusoid1D for <src>AutoDiff</src>
// </summary>

// <synopsis>
// <note role=warning> The name <src>NQSinusoid1D_PS</src> is only for cxx2html
// documentation problems. Use <src>NQSinusoid1D</src> in your code.</note>
// </synopsis>

template <class T> class NQSinusoid1D_PS<AutoDiff<T> > :
public NQSinusoid1DParam<AutoDiff<T> > {
 public:
  //# Constructors
  // Constructs one dimensional NQSinusoids.
  // <group>
  NQSinusoid1D_PS() : NQSinusoid1DParam<AutoDiff<T> >() {};
  explicit NQSinusoid1D_PS(const AutoDiff<T> &amplitude) :
    NQSinusoid1DParam<AutoDiff<T> >(amplitude) {};
  NQSinusoid1D_PS(const AutoDiff<T> &amplitude, const AutoDiff<T> &period) :
    NQSinusoid1DParam<AutoDiff<T> >(amplitude, period) {};
  NQSinusoid1D_PS(const AutoDiff<T> &amplitude, const AutoDiff<T> &period,
		  const AutoDiff<T> &x0) :
    NQSinusoid1DParam<AutoDiff<T> >(amplitude, period, x0) {};
  // </group>

  // Copy constructor (deep copy)
  NQSinusoid1D_PS(const NQSinusoid1D_PS &other) :
    NQSinusoid1DParam<AutoDiff<T> >(other) {};

  // Copy assignment (deep copy)
  NQSinusoid1D_PS<AutoDiff<T> > &
    operator=(const NQSinusoid1D_PS<AutoDiff<T> > &other) {
    NQSinusoid1DParam<AutoDiff<T> >::operator=(other); return *this; };
    
  // Destructor
  virtual ~NQSinusoid1D_PS() {};
    
  //# Operators    
  // Evaluate the NQSinusoid at <src>x</src>.
  // <group>
  virtual AutoDiff<T>
    eval(typename Function<AutoDiff<T> >::FunctionArg x) const;
  // </group>
    
  //# Member functions
  // Return a copy of this object from the heap. The caller is responsible 
  // for deleting this pointer.
  // <group>
  virtual Function<AutoDiff<T> > *clone() const {
    return new NQSinusoid1D<AutoDiff<T> >(*this); };
  // </group>

};

#undef NQSinusoid1D_PS

#endif
