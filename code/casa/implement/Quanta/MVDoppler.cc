//# MVDoppler.cc: Internal value for MDoppler
//# Copyright (C) 1996,1997,1998
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
#ifdef __GNUG__
#include <aips/Measures/Quantum.h>
typedef Quantum<Double> gpp_mvdoppler_bug1;
#endif
#include <aips/Exceptions/Error.h>
#include <aips/Utilities/Assert.h>
#include <aips/RTTI/Register.h>
#include <aips/Measures/MVDoppler.h>
#include <aips/Mathematics/Math.h>

// MVDoppler class

//# Constructors
MVDoppler::MVDoppler() : 
  val(0.0){}

MVDoppler::MVDoppler(Double d) : 
  val(d){}

MVDoppler::MVDoppler(const MVDoppler &other) :
  val(other.val) {}

MVDoppler::MVDoppler(const Quantity &other) {
  val = makeD(other.getValue(), other.getFullUnit());
}

MVDoppler::MVDoppler(const Quantum<Vector<Double> > &other) {
  Vector<Double> tmp;
  tmp = other.getValue();
  uInt i = tmp.nelements();
  if (i == 0) {
    val = 0.0;
  } else if (i == 1) {
    val = makeD(tmp(0), other.getFullUnit());
  } else {
    throw (AipsError("Illegal vector length in MVDoppler constructor"));
  };
}

MVDoppler::MVDoppler(const Vector<Double> &other) {
  uInt i = other.nelements();
  if (i == 0) {
    val = 0.0;
  } else if (i == 1) {
    val = other(0);
  } else {
    throw (AipsError("Illegal vector length in MVDoppler constructor"));
  };
}

MVDoppler::MVDoppler(const Vector<Quantity> &other) {
  uInt i = other.nelements();
  if (i == 0) {
    val = 0.0;
  } else if (i == 1) {
    val = makeD(other(0).getValue(), other(0).getFullUnit());
  } else {
    throw (AipsError("Illegal vector length in MVDoppler constructor"));
  };
}

MVDoppler &MVDoppler::operator=(const MVDoppler &other) {
  if (this != &other) {
    val = other.val;
  }
  return *this;
}

// Destructor
MVDoppler::~MVDoppler() {}

// Operators
MVDoppler::operator Double() const {
  return val;
}

MVDoppler &MVDoppler::operator+=(const MVDoppler &other) {
  val += other.val;
  return *this;
}

MVDoppler &MVDoppler::operator-=(const MVDoppler &other) {
  val -= other.val;
  return *this;
}

Bool MVDoppler::operator==(const MVDoppler &other) const {
  return ToBool(val == other.val);
}

Bool MVDoppler::operator!=(const MVDoppler &other) const {
  return ToBool(val != other.val);
}

Bool MVDoppler::near(const MVDoppler &other, Double tol) const {
  return ::near(val, other.val, tol);
}

Bool MVDoppler::nearAbs(const MVDoppler &other, Double tol) const {
  return ::nearAbs(val, other.val, tol);
}

// Member functions

uInt MVDoppler::type() const {
  return Register((MVDoppler *)0);
}

void MVDoppler::assert(const MeasValue &in) {
  if (in.type() != Register((MVDoppler *)0)) {
    throw(AipsError("Illegal MeasValue type argument: MVDoppler"));
  };
}

void MVDoppler::print(ostream &os) const {
  os << val;
}

MeasValue *MVDoppler::clone() const {
  return (new MVDoppler(*this));
}

Double MVDoppler::getValue() const {
  return val;
}

Quantity MVDoppler::get() const {
  return Quantity(val*C::c,"m/s");
}

Quantity MVDoppler::get(const Unit &unit) const {
  return Quantity(makeD(val, unit, True), unit);
}

Vector<Double> MVDoppler::getVector() const {
  Vector<Double> x(1);
  x(0) = val;
  return x;
}

void MVDoppler::putVector(const Vector<Double> &in) {
  if (in.nelements() < 1) {
    val = 0.0;
  } else {
    val = in(0);
  };
}

Double MVDoppler::makeD(Double v, const Unit &dt, Bool rev) const{
  static Bool needInit = True;
  static UnitVal Velocity;
  static Double LVel;
  if (needInit) {
    needInit = False;
    Velocity = UnitVal::LENGTH/UnitVal::TIME;
    LVel = (QC::c).getBaseValue();
  };
  Double x;
  if (dt.getValue() == UnitVal::NODIM) {
    x = dt.getValue().getFac();
  } else {
    Quantity(1.0,dt).assert(Velocity);
    x = dt.getValue().getFac()/LVel;
  }
  if (rev) return (v/x);
  return (v*x);
}
