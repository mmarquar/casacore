//# MVRadialVelocity.cc: Internal value for MRadialvelocity
//# Copyright (C) 1996,1997
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
typedef Quantum<Double> gpp_MVRadialVelocity_bug1;
#endif
#include <aips/Exceptions/Error.h>
#include <aips/Utilities/Assert.h>
#include <aips/RTTI/Register.h>
#include <aips/Measures/MVRadialVelocity.h>
#include <aips/Mathematics/Math.h>

// MVRadialVelocity class

//# Constructors
MVRadialVelocity::MVRadialVelocity() : 
  val(0.0){}

MVRadialVelocity::MVRadialVelocity(Double d) : 
  val(d){}

MVRadialVelocity::MVRadialVelocity(const MVRadialVelocity &other) :
  val(other.val) {}

MVRadialVelocity::MVRadialVelocity(const Quantity &other) {
  val = other.getValue() * makeF(other.getFullUnit());
}

MVRadialVelocity::MVRadialVelocity(const Quantum<Vector<Double> > &other) {
  Vector<Double> tmp;
  tmp = other.getValue();
  uInt i = tmp.nelements();
  if (i == 0) {
    val = 0.0;
  } else if (i == 1) {
    val = tmp(0) * makeF(other.getFullUnit());
  } else {
    throw (AipsError("Illegal vector length in MVRadialVelocity constructor"));
  };
}

MVRadialVelocity::MVRadialVelocity(const Vector<Double> &other) {
  uInt i = other.nelements();
  if (i == 0) {
    val = 0.0;
  } else if (i == 1) {
    val = other(0);
  } else {
    throw (AipsError("Illegal vector length in MVRadialVelocity constructor"));
  };
}

MVRadialVelocity::MVRadialVelocity(const Vector<Quantity> &other) {
  uInt i = other.nelements();
  if (i == 0) {
    val = 0.0;
  } else if (i == 1) {
    val = other(0).getValue() * makeF(other(0).getFullUnit());
  } else {
    throw (AipsError("Illegal vector length in MVRadialVelocity constructor"));
  };
}

MVRadialVelocity &MVRadialVelocity::operator=(const MVRadialVelocity &other) {
  if (this != &other) {
    val = other.val;
  }
  return *this;
}

// Destructor
MVRadialVelocity::~MVRadialVelocity() {}

// Operators
MVRadialVelocity::operator Double() const {
  return val;
}

MVRadialVelocity &MVRadialVelocity::operator+=(const MVRadialVelocity &other) {
  val += other.val;
  return *this;
}

MVRadialVelocity &MVRadialVelocity::operator-=(const MVRadialVelocity &other) {
  val -= other.val;
  return *this;
}

Bool MVRadialVelocity::operator==(const MVRadialVelocity &other) const {
  return ToBool(val == other.val);
}

Bool MVRadialVelocity::operator!=(const MVRadialVelocity &other) const {
  return ToBool(val != other.val);
}

Bool MVRadialVelocity::near(const MVRadialVelocity &other, Double tol) const {
  return ::near(val, other.val, tol);
}

Bool MVRadialVelocity::nearAbs(const MVRadialVelocity &other, Double tol) const {
  return ::nearAbs(val, other.val, tol);
}

// Member functions

uInt MVRadialVelocity::type() const {
  return Register((MVRadialVelocity *)0);
}

void MVRadialVelocity::assert(const MeasValue &in) {
  if (in.type() != Register((MVRadialVelocity *)0)) {
    throw(AipsError("Illegal MeasValue type argument: MVRadialVelocity"));
  };
}

void MVRadialVelocity::print(ostream &os) const {
  os << val;
}

MeasValue *MVRadialVelocity::clone() const {
  return (new MVRadialVelocity(*this));
}

Double MVRadialVelocity::getValue() const {
  return val;
}

Quantity MVRadialVelocity::get() const {
  return Quantity(val,"m/s");
}

Quantity MVRadialVelocity::get(const Unit &unit) const {
  return Quantity(val/makeF(unit), unit);
}

Vector<Double> MVRadialVelocity::getVector() const {
  Vector<Double> x(1);
  x(0) = val;
  return x;
}

void MVRadialVelocity::putVector(const Vector<Double> &in) {
  if (in.nelements() < 1) {
    val = 0.0;
  } else {
    val = in(0);
  };
}

Double MVRadialVelocity::makeF(const Unit &dt) const{
  static Bool needInit = True;
  static UnitVal Velocity;
  if (needInit) {
    needInit = False;
    Velocity = UnitVal::LENGTH/UnitVal::TIME;
  };
  Quantity(1.0,dt).assert(Velocity);
  return (dt.getValue().getFac());
}
