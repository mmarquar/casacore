//# LQNonLinearFitLM.h: Solve non-linear fit using Levenberg-Marquardt method.
//# Copyright (C) 1995,1999,2000,2001,2002
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

#if !defined(AIPS_LQNONLINEARFITLM_H)
#define AIPS_LQNONLINEARFITLM_H

//# Includes
#include <aips/aips.h>
#include <trial/Fitting/NonLinearFit.h>

//# Forward declarations

//
// <summary>
// Solve non-linear fit with Levenberg-Marquardt method.
// </summary>
//
// <reviewed reviewer="" date="" tests="tLQNonLinearFitLM.cc" demos="">
// </reviewed>
//
// <prerequisite>
// <ol>
//   <li> <linkto class="LQNonLinearFit">LQNonLinearFit</linkto>
//   <li> <linkto module="Fitting">Fitting</linkto>
// </ol>
// </prerequisite>
//
// <etymology>
// This class uses the Levenberg-Marquardt method to solve the non-linear
// least-squares fit problem hence LQNonLinearFitLM
// </etymology>
//
// <synopsis>
// See the <linkto class=LQNonLinearFit>LQNonLinearFit</linkto> class for a
// general description.
//
// This class is derived from the general LQNonLinearFit class. It does
// a non-linear least-squares fit using the Levenberg-Marquardt method.
//
// See Numerical Recipes for more information
// on the Levenberg-Marquardt method.
// 
// <templating arg=T>
// The following data types can be used to instantiate the LQNonLinearFit 
// templated class:
// <li> Float
// <li> Double
// <li> Complex
// <li> DComplex   
// </templating>
// </synopsis>
//
// <motivation>
// Levenberg-Marquardt method is a standard method for non-linear
// least-squares fits.  It works well in practice over a wide range of
// problems.
// </motivation>
// 
// <example>
// </example>

template<class T> class LQNonLinearFitLM : public LQNonLinearFit<T> {
public:
  //# Constructors
  // Create a fitter: the normal way to generate a fitter object. Necessary
  // data will be deduced from the Functional provided with
  // <src>setFunction()</src>.
  // Optionally, a fitter with SVD behaviour
  explicit LQNonLinearFitLM(Bool svd=False);
  // Create a fitter for complex data with non-standard interpretation
  // of the complex values
  // (see <linkto module=Fitting>Fitting</linkto> module). Additionally
  // the svd switch can be set.
  explicit LQNonLinearFitLM(LSQ::normType type, Bool svd=False);
  // Copy constructor (deep copy)
  LQNonLinearFitLM(const LQNonLinearFitLM &other);
  // Assignment (deep copy)
  LQNonLinearFitLM &operator=(const LQNonLinearFitLM &other);
  
  // Destructor
  virtual ~LQNonLinearFitLM();

 protected:
  //# Member functions
  // Generalised fitter
  virtual Bool fitIt
    (Vector<typename FunctionTraits<T>::BaseType> &sol, 
     const Array<typename FunctionTraits<T>::BaseType> &x, 
     const Vector<typename FunctionTraits<T>::BaseType> &y,
     const Vector<typename FunctionTraits<T>::BaseType> *const sigma,
     const Vector<Bool> *const mask=0);
  
 private:
  //# Data
  // The parameter that makes this the Levenberg-Marquardt method.  
  Double lamda_p;
  // The current fit state
  Double fitit_p;

};

#endif
