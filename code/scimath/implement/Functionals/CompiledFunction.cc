//# CompiledFunction.cc:  Form a linear combination of Functions
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
#include <trial/Functionals/CompiledFunction.h>
#include <trial/Functionals/FuncExpression.h>
#include <aips/Functionals/FunctionTraits.h>
#include <aips/Mathematics/Constants.h>
///
#include <aips/Mathematics/Complex.h>
#include <aips/Utilities/String.h>
#include <aips/vector.h>

//# Constructors

//# Operators
template<class T>
T CompiledFunction<T>::eval(typename Function<T>::FunctionArg x) const {
  String error_p = "";
  T res(0);
  if (!functionPtr_p) {
    error_p = "No CompiledFunction specified";
    return res;
  };
  vector<T> exec_p;
  exec_p.resize(0);
  vector<Double>::const_iterator
    constp = functionPtr_p->getConst().begin();
  for (vector<FuncExprData::ExprOperator>::const_iterator
	 pos=functionPtr_p->getCode().begin();
       pos != functionPtr_p->getCode().end(); pos++) {
    T t(0);
    if (pos->narg == 2 ||
	(pos->code == FuncExprData::ATAN && pos->state.argcnt == 2)) {
      t = exec_p.back();
      exec_p.pop_back();
    };
    switch (pos->code) {
    case  FuncExprData::UNAMIN:
      exec_p.back() = -exec_p.back();
    case FuncExprData::UNAPLUS:
      break;
  
    case  FuncExprData::POW:
      exec_p.back() = pow(exec_p.back(), t);
      break;
    case  FuncExprData::GTE:
      exec_p.back() = exec_p.back() >= t ? T(1) : T(0);
      break;
    case  FuncExprData::LTE:
      exec_p.back() = exec_p.back() <= t ? T(1) : T(0);
      break;
    case  FuncExprData::EQ:
      exec_p.back() = exec_p.back() == t ? T(1) : T(0);
      break;
    case  FuncExprData::NEQ:
      exec_p.back() = exec_p.back() != t ? T(1) : T(0);
      break;
    case  FuncExprData::OR:
      exec_p.back() = (exec_p.back() != T(0) || t != T(0)) ? T(1) : T(0);
      break;
    case  FuncExprData::AND:
      exec_p.back() = (t*exec_p.back() != T(0)) ? T(1) : T(0);
      break;
    case  FuncExprData::ADD:
      exec_p.back() += t;
      break;
    case  FuncExprData::SUB:
      exec_p.back() -= t;
      break;
    case  FuncExprData::MUL:
      exec_p.back() *= t;
      break;
    case  FuncExprData::DIV:
      exec_p.back() /= t;
      break;

    case  FuncExprData::CONST:
      exec_p.push_back(T(*(constp++)));
      break;
    case FuncExprData::PARAM:
      exec_p.push_back(T(param_p[pos->narg]));
      break;
    case FuncExprData::ARG:
      exec_p.push_back(T(x[pos->narg]));
      break;

    case  FuncExprData::SIN:
      exec_p.back() = sin(exec_p.back());
      break;
    case  FuncExprData::COS:
      exec_p.back() = cos(exec_p.back());
      break;
    case  FuncExprData::ATAN:
      if (pos->state.argcnt == 1) {
	exec_p.back() = atan(exec_p.back());
	break;
      };
    case  FuncExprData::ATAN2:
      exec_p.back() = atan2(exec_p.back(), t);
      break;
    case  FuncExprData::ASIN:
      exec_p.back() = asin(exec_p.back());
      break;
    case  FuncExprData::ACOS:
      exec_p.back() = acos(exec_p.back());
      break;
    case  FuncExprData::EXP:
      exec_p.back() = exp(exec_p.back());
      break;
    case  FuncExprData::EXP2:
      exec_p.back() = exp(exec_p.back()*
			  static_cast<typename FunctionTraits<T>::BaseType>
			  (C::ln2));
      break;
    case  FuncExprData::EXP10:
      exec_p.back() = exp(exec_p.back()*
			  static_cast<typename FunctionTraits<T>::BaseType>
			  (C::ln10));
      break;
    case  FuncExprData::LOG:
      exec_p.back() = log(exec_p.back());
      break;
    case  FuncExprData::LOG2:
      exec_p.back() = log(exec_p.back())/
	static_cast<typename FunctionTraits<T>::BaseType>(C::ln2);
      break;
    case  FuncExprData::LOG10:
      exec_p.back() = log10(exec_p.back());
      break;
    case  FuncExprData::PI: {
      if (pos->state.argcnt == 0) {
	exec_p.push_back(T(static_cast<typename FunctionTraits<T>::BaseType>
			   (C::pi)));
      } else {
	exec_p.back() *= static_cast<typename FunctionTraits<T>::BaseType>
	  (C::pi);
      };
      break; }
    case  FuncExprData::EE: {
      if (pos->state.argcnt == 0) {
	exec_p.push_back(T(static_cast<typename FunctionTraits<T>::BaseType>
			   (C::e)));
      } else {
	exec_p.back() *= static_cast<typename FunctionTraits<T>::BaseType>
	  (C::e);
      };
      break; }
    case  FuncExprData::ABS:
      exec_p.back() = abs(exec_p.back());
      break;
      ///   case  FuncExprData::FLOOR:
      ///      exec_p.back() = floor(exec_p.back());
      ///      break;
      ///    case  FuncExprData::CEIL:
      ///      exec_p.back() = ceil(exec_p.back());
      ///      break;
      ///    case  FuncExprData::ROUND:
      ///      exec_p.back() = floor(exec_p.back()+T(0.5));
      ///      break;
      ///    case  FuncExprData::INT:
      ///      if (exec_p.back() < 0) exec_p.back() = floor(exec_p.back());
      ///      else exec_p.back() = ceil(exec_p.back());
      ///      break;
      ///    case  FuncExprData::FRACT:
      ///      if (exec_p.back() < 0) exec_p.back() -= ceil(exec_p.back());
      ///      else exec_p.back() -= floor(exec_p.back());
      ///      break;
    case  FuncExprData::SQRT:
      exec_p.back() = sqrt(exec_p.back());
      break;
    case  FuncExprData::REAL:
      break;
    case  FuncExprData::IMAG:
      exec_p.back() = T(0);
      break;
    case  FuncExprData::AMPL:
      break;
    case  FuncExprData::PHASE:
      exec_p.back() = T(0);
      break;
    default:
      error_p = String("Unknown execution code '") +
	pos->name + "': programming error";
      break;
    }
  };
  if (exec_p.size() != 1 && error_p.empty()) error_p = "No value returned";
  if (error_p.empty()) res = exec_p.back();

  return res;
}

//# Member functions

/// for now
#include <aips/Mathematics/AutoDiff.h>
#include <aips/Mathematics/AutoDiffMath.h>
template class vector<AutoDiff<Double> >;
AIPS_VECTOR_AUX_TEMPLATES(AutoDiff<Double>)
template class vector<AutoDiff<DComplex> >;
AIPS_VECTOR_AUX_TEMPLATES(AutoDiff<DComplex>)
template class vector<AutoDiff<Complex> >;
AIPS_VECTOR_AUX_TEMPLATES(AutoDiff<Complex>)
template class vector<DComplex>;
AIPS_VECTOR_AUX_TEMPLATES(DComplex)
template class vector<Complex>;
AIPS_VECTOR_AUX_TEMPLATES(Complex)
