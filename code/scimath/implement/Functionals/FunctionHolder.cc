//# FunctionHolder.cc: A holder for Functions to enable record conversions
//# Copyright (C) 2002
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
#include <trial/Functionals/FunctionHolder.h>
#include <aips/Exceptions.h>
#include <aips/Containers/RecordInterface.h>
#include <aips/Containers/RecordFieldId.h>
#include <aips/Containers/Record.h>
#include <aips/Arrays/Vector.h>
#include <aips/Utilities/String.h>
#include <aips/Functionals/Gaussian1D.h>
#include <aips/Functionals/Gaussian2D.h>
#include <trial/Functionals/Gaussian3D.h>
#include <aips/Functionals/GaussianND.h>
#include <aips/Functionals/Polynomial.h>
#include <trial/Functionals/EvenPolynomial.h>
#include <trial/Functionals/OddPolynomial.h>
#include <aips/Functionals/HyperPlane.h>
#include <aips/Functionals/Sinusoid1D.h>
#include <aips/Functionals/Chebyshev.h>
#include <aips/Functionals/CombiFunction.h>
#include <aips/Functionals/CompoundFunction.h>
#include <aips/Quanta/MUString.h>
#include <aips/Mathematics/AutoDiff.h>
#include <aips/Mathematics/AutoDiffMath.h>
#include <aips/strstream.h>

//# Constructors
template <class T>
FunctionHolder<T>::FunctionHolder() 
  : hold_p(), nam_p(N_Types), isFilled(False) {}

template <class T>
FunctionHolder<T>::FunctionHolder(const Function<T> &in) 
  : hold_p(in.clone()), nam_p(N_Types), isFilled(False) {}

template <class T>
FunctionHolder<T>::FunctionHolder(const FunctionHolder<T> &other)
  : hold_p(), nam_p(N_Types), isFilled(False) {
  if (other.hold_p.ptr()) hold_p.set(other.hold_p.ptr()->clone());
}

//# Destructor
template <class T>
FunctionHolder<T>::~FunctionHolder() {}

//# Operators
template <class T>
FunctionHolder<T> &FunctionHolder<T>::
operator=(const FunctionHolder<T> &other) {
  if (this != &other) {
    if (other.hold_p.ptr()) {
      hold_p.set(other.hold_p.ptr()->clone());
    } else {
      hold_p.clear();
    };
  };
  return *this;
}

//# Member Functions
template <class T>
Bool FunctionHolder<T>::isEmpty() const {
  return (!hold_p.ptr());
}

template <class T>
const Vector<String> &FunctionHolder<T>::names() const {
  init();
  return nam_p;
}

template <class T>
const Function<T> &FunctionHolder<T>::asFunction() const {
  if (!hold_p.ptr()) {
    throw(AipsError("Empty FunctionHolder argument for asFunction"));
  };
  return *hold_p.ptr();
}

template <class T>
Bool FunctionHolder<T>::addFunction(const Function<T> &fnc) {
  if (nf_p == COMBINE) {
    dynamic_cast<CombiFunction<T> &>(*hold_p.ptr()).addFunction(fnc);
  } else if (nf_p == COMPOUND) {
    dynamic_cast<CompoundFunction<T> &>(*hold_p.ptr()).addFunction(fnc);
  } else return False;
  return True;
} 

template <class T>
const FunctionHolder<T>::Types FunctionHolder<T>::type() const {
  if (!hold_p.ptr()) {
    throw(AipsError("Empty FunctionHolder argument for type()"));
  };
  return nf_p;
}

template <class T>
void FunctionHolder<T>::init() const {
  static FunctionHolder<T>::FuncStat fnc[N_Types] = {
    { String("gaussian1d"), GAUSSIAN1D,
      False},
    { String("gaussian2d"), GAUSSIAN2D,
      False},
    { String("gaussian3d"), GAUSSIAN3D,
      False},
    { String("gaussianNd"), GAUSSIANND,
      True},
    { String("hyperplane"), HYPERPLANE,
      True},
    { String("polynomial"), POLYNOMIAL,
      True},
    { String("evenpolynomial"), EVENPOLYNOMIAL,
      True},
    { String("oddpolynomial"), ODDPOLYNOMIAL,
      True},
    { String("sinusoid1d"), SINUSOID1D,
      False},
    { String("chebyshev"), CHEBYSHEV,
      True},
    { String("combine"), COMBINE,
      False},
    { String("compound"), COMPOUND,
      False}
  };
  if (!isFilled) {
    isFilled = True;
    for (uInt i=0; i<N_Types; ++i) {
      nam_p[i] = fnc[i].nam;
      if (i != static_cast<uInt>(fnc[i].tp)) {
	throw(AipsError("Lists in FunctionHolder incorrect order"));
      };
    };
  };
}

template <class T>
Bool FunctionHolder<T>::fromRecord(String &error, const RecordInterface &in) {
  hold_p.clear();
  Function<T> *fn(0);
  if (!getRecord(error, fn, in)) {
    delete fn; fn = 0;
    return False;
  };
  hold_p.set(fn);
  return True;
}

template <class T>
template <class U>
Bool FunctionHolder<T>::getRecord(String &error, Function<U> *&fn,
				  const RecordInterface &in) {
  if (in.isDefined(String("type")) &&
      in.isDefined(String("order")) &&
      in.type(in.idToNumber(RecordFieldId("order"))) == TpInt &&
      (in.type(in.idToNumber(RecordFieldId("type"))) == TpString ||
       (in.type(in.idToNumber(RecordFieldId("type"))) == TpInt &&
	in.isDefined(String("ndim")) &&
	in.isDefined(String("npar")) &&
	in.isDefined(String("params")) &&
	in.type(in.idToNumber(RecordFieldId("ndim"))) == TpInt &&
	in.type(in.idToNumber(RecordFieldId("npar"))) == TpInt &&
	in.type(in.idToNumber(RecordFieldId("params"))) ==
	TpArrayDouble))) {
    if (!getType(error, fn, in)) return False;
    if ((nf_p == COMBINE || nf_p == COMPOUND) &&
	in.isDefined(String("nfunc")) &&
	in.isDefined(String("funcs")) &&
	in.type(in.idToNumber(RecordFieldId("nfunc"))) == TpInt &&
	in.type(in.idToNumber(RecordFieldId("funcs"))) == TpRecord) {
      Int nfunc;
      in.get(RecordFieldId("nfunc"), nfunc);
      Record fnsrec = in.asRecord(RecordFieldId("funcs"));
      for (Int i=0; i<nfunc; ++i) {
	Record fnr = fnsrec.asRecord(i);
	FunctionHolder<T> fnch;
	Function<U> *fnc(0);
	if (!fnch.getRecord(error, fnc, fnr)) {
	  delete fnc; fnc = 0;
	  return False;
	};
	if (nf_p == COMBINE) {
	  dynamic_cast<CombiFunction<U> *>(fn)->
	    addFunction(*fnc);
	} else {
	  dynamic_cast<CompoundFunction<U> *>(fn)->
	    addFunction(*fnc);
	};
	delete fnc; fnc = 0;
      };
    };
    if (in.isDefined(String("params")) &&
	in.type(in.idToNumber(RecordFieldId("params"))) == TpArrayDouble) {
      Vector<T> params;
      in.get(RecordFieldId("params"), params);
      setParameters(fn, params);
    };
    return True;
  };
  error += String("Illegal Function record in "
		  "FunctionHolder<T>::fromRecord\n");
  return False;
}

template <class T>
Bool FunctionHolder<T>::fromString(String &error,
				   const String &in) {
  order_p = -1;
  Int nf;
  init();
  nf = MUString::minimaxNC(in, nam_p);
  nf_p = static_cast<FunctionHolder<T>::Types>(nf);
  Function<T> *fn(0);
  if (getType(error, fn)) {
    hold_p.set(fn);
    return True;
  };
  delete fn; fn = 0;
  return False;
}

template <class T>
Bool FunctionHolder<T>::toRecord(String &error, RecordInterface &out) const {
  if (hold_p.ptr() && putType(error, out)) {
    out.define(RecordFieldId("ndim"),
	       static_cast<Int>(hold_p.ptr()->ndim()));
    out.define(RecordFieldId("npar"),
	       static_cast<Int>(hold_p.ptr()->nparameters()));
    out.define(RecordFieldId("params"),
    	       hold_p.ptr()->parameters().getParameters());
    if (nf_p == COMBINE || nf_p == COMPOUND) {
      Int x(0);
      if (nf_p == COMBINE) {
	x = dynamic_cast<const CombiFunction<T> *>
	  (hold_p.ptr())->nFunctions();
      } else {
	x = dynamic_cast<const CompoundFunction<T> *>
	  (hold_p.ptr())->nFunctions();
      };
      out.define("nfunc", x);
      Record func;
      for (Int i=0; i<x; ++i) {
	Record fnc;
	if (nf_p == COMBINE) {
	  FunctionHolder<T> fn(dynamic_cast<const CombiFunction<T> *>
			    (hold_p.ptr())->function(i));
	  if (!fn.toRecord(error, fnc)) return False;
	} else {
	  FunctionHolder<T> fn(dynamic_cast<const CompoundFunction<T> *>
			    (hold_p.ptr())->function(i));
	  if (!fn.toRecord(error, fnc)) return False;
	};
	ostrstream oss;
	oss << "__*" << i;
	func.defineRecord(String(oss), fnc);
      };
      out.defineRecord("funcs", func);
    };
    return True;
  };
  error += String("No Function specified in FunctionHolder::toRecord\n");
  return False;
}

template <class T>
const String &FunctionHolder<T>::ident() const {
  static String myid = "fnc";
  return myid;
}

template <class T>
Bool FunctionHolder<T>::putType(String &error, RecordInterface &out) const {
  order_p = -1;
  if (dynamic_cast<const Gaussian1D<T> *>(hold_p.ptr())) {
    nf_p = GAUSSIAN1D;
  } else if (dynamic_cast<const Gaussian2D<T> *>(hold_p.ptr())) {
    nf_p = GAUSSIAN2D;
  } else if (dynamic_cast<const Gaussian3D<T> *>(hold_p.ptr())) {
    nf_p = GAUSSIAN3D;
  } else if (dynamic_cast<const GaussianND<T> *>(hold_p.ptr())) {
    nf_p = GAUSSIANND;
    order_p = Int(-3.0+sqrt(1+8*hold_p.ptr()->nparameters())+0.1)/2;
  } else if (dynamic_cast<const HyperPlane<T> *>(hold_p.ptr())) {
    nf_p = HYPERPLANE;
    order_p = hold_p.ptr()->nparameters()-1;
  } else if (dynamic_cast<const Polynomial<T> *>(hold_p.ptr())) {
    nf_p = POLYNOMIAL;
    order_p = hold_p.ptr()->nparameters()-1;
  } else if (dynamic_cast<const EvenPolynomial<T> *>(hold_p.ptr())) {
    nf_p = EVENPOLYNOMIAL;
    order_p = 2*hold_p.ptr()->nparameters()-1;
  } else if (dynamic_cast<const OddPolynomial<T> *>(hold_p.ptr())) {
    nf_p = ODDPOLYNOMIAL;
    order_p = 2*hold_p.ptr()->nparameters()-1;
  } else if (dynamic_cast<const Sinusoid1D<T> *>(hold_p.ptr())) {
    nf_p = SINUSOID1D;
  } else if (dynamic_cast<const Chebyshev<T> *>(hold_p.ptr())) {
    nf_p = CHEBYSHEV;
    order_p = hold_p.ptr()->nparameters()-1;
  } else if (dynamic_cast<const CombiFunction<T> *>(hold_p.ptr())) {
    nf_p = COMBINE;
  } else if (dynamic_cast<const CompoundFunction<T> *>(hold_p.ptr())) {
    nf_p = COMPOUND;
  } else {
    error += String("Unknown functional in FunctionHolder::putType()\n");
    return False;
  };
  out.define(RecordFieldId("type"), nf_p);
  out.define(RecordFieldId("order"), order_p);
  return True;
}

template <class T>
template <class U>
Bool FunctionHolder<T>::getType(String &error, Function<U> *&fn,
				const RecordInterface &in) {
  in.get(RecordFieldId("order"), order_p);
  Int nf;
  if (in.type(in.idToNumber(RecordFieldId("type"))) == TpString) {
    String tp;
    in.get(RecordFieldId("type"), tp);
    init();
    nf = MUString::minimaxNC(tp, nam_p);
  } else {
    in.get(RecordFieldId("type"), nf);
  };
  nf_p = static_cast<FunctionHolder<T>::Types>(nf);
  return getType(error, fn);
}

template <class T>
template <class U>
Bool FunctionHolder<T>::getType(String &error, Function<U> *&fn) {
  if (nf_p<0 || nf_p >= N_Types) {
    error += "Unknown type in FunctionHolder::getType()\n";
    return False;
  };
  ///  hold_p.clear();
  switch (nf_p) {
    
  case GAUSSIAN1D:
    fn = (new Gaussian1D<U>);
    break;
    
  case GAUSSIAN2D:
    fn = (new Gaussian2D<U>);
    break;
        
  case GAUSSIAN3D:
    fn = (new Gaussian3D<U>);
    break;
    
  case GAUSSIANND:
    if (order_p >= 0) {
      fn = (new GaussianND<U>(order_p));
    } else fn = (new GaussianND<U>);
    break;
     
  case HYPERPLANE:
    if (order_p >= 0) {
      fn = (new HyperPlane<U>(order_p));
    } else fn = (new HyperPlane<U>);
    break;
   
  case POLYNOMIAL:
    if (order_p >= 0) {
      fn = (new Polynomial<U>(order_p));
    } else fn = (new Polynomial<U>);
    break;
    
  case EVENPOLYNOMIAL:
    if (order_p >= 0) {
      fn = (new EvenPolynomial<U>(order_p));
    } else fn = (new EvenPolynomial<U>);
    break;
   
  case ODDPOLYNOMIAL:
    if (order_p >= 0) {
      fn = (new OddPolynomial<U>(order_p));
    } else fn = (new OddPolynomial<U>);
    break;
    
  case SINUSOID1D:
    fn = (new Sinusoid1D<U>);
    break;
    
  case CHEBYSHEV:
    if (order_p >= 0) {
      fn = (new Chebyshev<U>(order_p));
    } else fn = (new Chebyshev<U>);
    break;
    
  case COMBINE:
    fn = (new CombiFunction<U>);
    break;
    
  case COMPOUND:
    fn = (new CompoundFunction<U>);
    break;
    
  default:
    error += "Unknown type in FunctionHolder::getType()\n";
    return False;
    break;
  }
  return True;
}

template <class T>
void FunctionHolder<T>::setParameters(Function<T> *&fn,
				      const Vector<T> &params) {
  for (uInt i=0; i<fn->nparameters(); ++i) (*fn)[i] = params[i];
}

template <class T>
void FunctionHolder<T>::setParameters(Function<AutoDiff<T> > *&fn,
				      const Vector<T> &params) {
  for (uInt i=0; i<fn->nparameters(); ++i) {
    (*fn)[i] = AutoDiff<T>(params[i], fn->nparameters(), i);
  };
}

//# Instantiations
template class FunctionHolder<Double>;
template Bool FunctionHolder<Double>::getType(String &,
					      Function<Double> *&,
					      const RecordInterface &);
template Bool FunctionHolder<Double>::getType(String &,
					      Function<Double> *&);
template Bool FunctionHolder<Double>::getRecord(String &,
						Function<Double> *&,
						const RecordInterface &);
template Bool FunctionHolder<Double>::getType(String &,
					      Function<AutoDiff<Double> > *&,
					      const RecordInterface &);
template Bool FunctionHolder<Double>::getType(String &,
					      Function<AutoDiff<Double> > *&);
template Bool FunctionHolder<Double>::getRecord(String &,
						Function<AutoDiff<Double> > *&,
						const RecordInterface &);
///template class FunctionHolder<Complex>;
///template Bool FunctionHolder<Complex>::getType(String &,
///                                              Function<Complex> *&,
///                                              const RecordInterface &);
///template Bool FunctionHolder<Complex>::getType(String &,
///                                              Function<Complex> *&);
///template Bool FunctionHolder<Complex>::getRecord(String &,
///                                                Function<Complex> *&,
///                                                const RecordInterface &);
///template Bool FunctionHolder<Complex>::getType(String &,
///                                              Function<AutoDiff<Complex> > *&,
///                                              const RecordInterface &);
///template Bool FunctionHolder<Complex>::getType(String &,
///                                              Function<AutoDiff<Complex> > *&);
///template Bool FunctionHolder<Complex>::getRecord(String &,
///                                                Function<AutoDiff<Complex> > *&,
///                                                const RecordInterface &);
