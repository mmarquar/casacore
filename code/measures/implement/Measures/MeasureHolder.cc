//# MeasureHolder.cc: A holder for Measures to enable record conversions
//# Copyright (C) 1998
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
#include <aips/Measures/MeasureHolder.h>
#include <aips/Quanta/QuantumHolder.h>
#include <aips/Exceptions.h>
#include <aips/Quanta/Quantum.h>
#include <aips/Measures/MDirection.h>
#include <aips/Measures/MDoppler.h>
#include <aips/Measures/MEpoch.h>
#include <aips/Measures/MFrequency.h>
#include <aips/Measures/MPosition.h>
#include <aips/Measures/MRadialVelocity.h>
#include <aips/Measures/MBaseline.h>
#include <aips/Measures/Muvw.h>
#include <aips/Measures/MEarthMagnetic.h>
#include <aips/Containers/RecordInterface.h>
#include <aips/Containers/RecordFieldId.h>
#include <aips/Containers/Record.h>
#include <aips/Arrays/Vector.h>
#include <aips/Utilities/String.h>
#include <aips/Logging/LogIO.h>
#include <aips/Logging/LogOrigin.h>

//# Constructors
MeasureHolder::MeasureHolder() : hold_p() {};

MeasureHolder::MeasureHolder(const Measure &in) : hold_p(in.clone()) {}

MeasureHolder::MeasureHolder(const MeasureHolder &other) : hold_p() {
	if (other.hold_p.ptr()) hold_p.set(other.hold_p.ptr()->clone());
      }

//# Destructor
MeasureHolder::~MeasureHolder() {}

//# Operators
MeasureHolder &MeasureHolder::operator=(const MeasureHolder &other) {
  if (this != &other) hold_p.set(other.hold_p.ptr()->clone());
  return *this;
}

//# Member Functions
Bool MeasureHolder::isEmpty() const {
  return ToBool(!hold_p.ptr());
}

Bool MeasureHolder::isMeasure() const {
  return ToBool(hold_p.ptr());
}

Bool MeasureHolder::isMDirection() const {
  return ToBool(hold_p.ptr() && hold_p.ptr()->type() == MDirection::myType());
}

Bool MeasureHolder::isMDoppler() const {
  return ToBool(hold_p.ptr() && hold_p.ptr()->type() == MDoppler::myType());
}

Bool MeasureHolder::isMEpoch() const {
  return ToBool(hold_p.ptr() && hold_p.ptr()->type() == MEpoch::myType());
}

Bool MeasureHolder::isMFrequency() const {
  return ToBool(hold_p.ptr() && hold_p.ptr()->type() == MFrequency::myType());
}

Bool MeasureHolder::isMPosition() const {
  return ToBool(hold_p.ptr() && hold_p.ptr()->type() == MPosition::myType());
}

Bool MeasureHolder::isMRadialVelocity() const {
  return ToBool(hold_p.ptr() && hold_p.ptr()->type() == MRadialVelocity::myType());
}

Bool MeasureHolder::isMBaseline() const {
  return ToBool(hold_p.ptr() && hold_p.ptr()->type() == MBaseline::myType());
}

Bool MeasureHolder::isMuvw() const {
  return ToBool(hold_p.ptr() && hold_p.ptr()->type() == Muvw::myType());
}

Bool MeasureHolder::isMEarthMagnetic() const {
  return ToBool(hold_p.ptr() && hold_p.ptr()->type() == MEarthMagnetic::myType());
}

const Measure &MeasureHolder::asMeasure() const {
  if (!hold_p.ptr()) {
    throw(AipsError("Empty MeasureHolder argument for asMeasure"));
  };
  return *hold_p.ptr();
}

const MDirection &MeasureHolder::asMDirection() const {
  if (!hold_p.ptr() || !isMDirection()) {
    throw(AipsError("Empty or wrong MeasureHolder for asMDirection"));
  };
  return (const MDirection &) *hold_p.ptr();
}

const MDoppler &MeasureHolder::asMDoppler() const {
  if (!hold_p.ptr() || !isMDoppler()) {
    throw(AipsError("Empty or wrong MeasureHolder for asMDoppler"));
  };
  return (const MDoppler &) *hold_p.ptr();
}

const MEpoch &MeasureHolder::asMEpoch() const {
  if (!hold_p.ptr() || !isMEpoch()) {
    throw(AipsError("Empty or wrong MeasureHolder for asMEpoch"));
  };
  return (const MEpoch &) *hold_p.ptr();
}

const MFrequency &MeasureHolder::asMFrequency() const {
  if (!hold_p.ptr() || !isMFrequency()) {
    throw(AipsError("Empty or wrong MeasureHolder for asMFrequency"));
  };
  return (const MFrequency &) *hold_p.ptr();
}

const MPosition &MeasureHolder::asMPosition() const {
  if (!hold_p.ptr() || !isMPosition()) {
    throw(AipsError("Empty or wrong MeasureHolder for asMPosition"));
  };
  return (const MPosition &) *hold_p.ptr();
}

const MRadialVelocity &MeasureHolder::asMRadialVelocity() const {
  if (!hold_p.ptr() || !isMRadialVelocity()) {
    throw(AipsError("Empty or wrong MeasureHolder for asMRadialVelocity"));
  };
  return (const MRadialVelocity &) *hold_p.ptr();
}

const MEarthMagnetic &MeasureHolder::asMEarthMagnetic() const {
  if (!hold_p.ptr() || !isMEarthMagnetic()) {
    throw(AipsError("Empty or wrong MeasureHolder for asMEarthMagnetic"));
  };
  return (const MEarthMagnetic &) *hold_p.ptr();
}

const MBaseline &MeasureHolder::asMBaseline() const {
  if (!hold_p.ptr() || !isMBaseline()) {
    throw(AipsError("Empty or wrong MeasureHolder for asMBaseline"));
  };
  return (const MBaseline &) *hold_p.ptr();
}

const Muvw &MeasureHolder::asMuvw() const {
  if (!hold_p.ptr() || !isMuvw()) {
    throw(AipsError("Empty or wrong MeasureHolder for asMuvw"));
  };
  return (const Muvw &) *hold_p.ptr();
}

Bool MeasureHolder::fromRecord(String &error,
			       const RecordInterface &in) {
  if (in.isDefined(String("type")) &&
      in.isDefined(String("refer")) &&
      in.type(in.idToNumber(RecordFieldId("type"))) == TpString &&
      in.type(in.idToNumber(RecordFieldId("refer"))) == TpString) {
    String tp;
    in.get(RecordFieldId("type"), tp);
    tp.downcase();
    hold_p.clear();
    if (tp == downcase(MDirection::showMe())) {
      hold_p.set(new MDirection());
    } else if (tp == downcase(MDoppler::showMe())) {
      hold_p.set(new MDoppler());
    } else if (tp == downcase(MEpoch::showMe())) {
      hold_p.set(new MEpoch());
    } else if (tp == downcase(MFrequency::showMe())) {
      hold_p.set(new MFrequency());
    } else if (tp == downcase(MPosition::showMe())) {
      hold_p.set(new MPosition());
    } else if (tp == downcase(MRadialVelocity::showMe())) {
      hold_p.set(new MRadialVelocity());
    } else if (tp == downcase(MBaseline::showMe())) {
      hold_p.set(new MBaseline());
    } else if (tp == downcase(Muvw::showMe())) {
      hold_p.set(new Muvw());
    } else if (tp == downcase(MEarthMagnetic::showMe())) {
      hold_p.set(new MEarthMagnetic());
    } else {
      error += String("Unknown Measure record in MeasureHolder::fromRecord\n");
      return False;
    };
    String rf;
    in.get(RecordFieldId("refer"), rf);
    if (!hold_p.ptr()->setRefString(rf)) {
      LogIO os(LogOrigin("MeasureHolder", 
			 String("fromRecord(String, const RecordInterface"),
			 WHERE));
      os << LogIO::WARN <<
	String("Illegal or unknown reference type '") +
	rf + "' for " + tp + " definition. DEFAULT (" + 
	hold_p.ptr()->getDefaultType() + ") assumed." <<
	LogIO::POST;
    };
    if (in.isDefined(String("offset")) &&
	in.type(in.idToNumber(RecordFieldId("offset"))) == TpRecord) {
      MeasureHolder x;
      if (!x.fromRecord(error, in.asRecord(RecordFieldId("offset")))) {
	return False;
      };
      if (!hold_p.ptr()->setOffset(x.asMeasure())) {
	error += String("Unmatched offset type in MeasureHolder::fromRecord\n");
	return False;
      };
    };
    QuantumHolder q0, q1, q2;
    Int n;
    if (in.isDefined(String("m0")) &&
	in.type(in.idToNumber(RecordFieldId("m0"))) == TpRecord) {
      if (!q0.fromRecord(error, in.asRecord(RecordFieldId("m0")))) {
	return False;
      };
      n = 1;
      if (in.isDefined(String("m1")) &&
	  in.type(in.idToNumber(RecordFieldId("m1"))) == TpRecord) {
	if (!q1.fromRecord(error, in.asRecord(RecordFieldId("m1")))) {
	  return False;
	};
	n = 2;
	if (in.isDefined(String("m2")) &&
	    in.type(in.idToNumber(RecordFieldId("m2"))) == TpRecord) {
	  if (!q2.fromRecord(error, in.asRecord(RecordFieldId("m2")))) {
	    return False;
	  };
	  n = 3;
	};
      };
    };
    Vector<Quantity> vq(n);
    if (n > 0) vq(0) = q0.asQuantity();
    if (n > 1) vq(1) = q1.asQuantity();
    if (n > 2) vq(2) = q2.asQuantity();
    if (!hold_p.ptr()->putValue(vq)) {
      error += String("Illegal quantity in MeasureHolder::fromRecord\n");
      return False;
    };
    return True;
  };
  error += String("Illegal Measure record in MeasureHolder::fromRecord\n");
  return False;
}

Bool MeasureHolder::toRecord(String &error, RecordInterface &out) const {
  if (hold_p.ptr()) {
    out.define(RecordFieldId("type"),
	       downcase(String(hold_p.ptr()->tellMe())));
    out.define(RecordFieldId("refer"), hold_p.ptr()->getRefString());
    const Measure *off = hold_p.ptr()->getRefPtr()->offset();
    if (off) {
      Record offs;
      if (!MeasureHolder(*off).toRecord(error, offs)) return False;
      out.defineRecord(RecordFieldId("offset"), offs);
    };
    Vector<Quantum<Double> > res = hold_p.ptr()->getData()->getRecordValue();
    Int n = res.nelements();
    Record val;
    if (n > 2) {
      if (!QuantumHolder(res(2)).toRecord(error, val)) return False;
      out.defineRecord(RecordFieldId("m2"), val);
    };
    if (n > 1) {
      if (!QuantumHolder(res(1)).toRecord(error, val)) return False;
      out.defineRecord(RecordFieldId("m1"), val);
    };
    if (n > 0) {
      if (!QuantumHolder(res(0)).toRecord(error, val)) return False;
      out.defineRecord(RecordFieldId("m0"), val);
    };
    res.resize(0);
    res = hold_p.ptr()->getData()->getXRecordValue();
    val = Record();
    n = res.nelements();
    if (n > 2) {
      if (!QuantumHolder(res(2)).toRecord(error, val)) return False;
      out.defineRecord(RecordFieldId("ev2"), val);
    };
    if (n > 1) {
      if (!QuantumHolder(res(1)).toRecord(error, val)) return False;
      out.defineRecord(RecordFieldId("ev1"), val);
    };
    if (n > 0) {
      if (!QuantumHolder(res(0)).toRecord(error, val)) return False;
      out.defineRecord(RecordFieldId("ev0"), val);
      };
    return True;
  };
  error += String("No Measure specified in MeasureHolder::toRecord\n");
  return False;
}

const String &MeasureHolder::ident() const {
  static String myid = "meas";
  return myid;
}
