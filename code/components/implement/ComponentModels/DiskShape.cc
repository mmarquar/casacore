//# DiskShape.cc:
//# Copyright (C) 1998,1999
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

#include <trial/ComponentModels/DiskShape.h>
#include <trial/ComponentModels/Flux.h>
#include <aips/Arrays/Vector.h>
#include <aips/Containers/Record.h>
#include <aips/Containers/RecordFieldId.h>
#include <aips/Containers/RecordInterface.h>
#include <aips/Exceptions/Error.h>
#include <aips/Logging/LogIO.h>
#include <aips/Logging/LogOrigin.h>
#include <aips/Mathematics/Constants.h>
#include <aips/Mathematics/Math.h>
#include <aips/Measures/MCDirection.h>
#include <aips/Quanta/MVAngle.h>
#include <aips/Measures/MeasConvert.h>
#include <aips/Quanta/Quantum.h>
#include <aips/Quanta/QuantumHolder.h>
#include <aips/Utilities/Assert.h>
#include <aips/Utilities/DataType.h>
#include <aips/Utilities/String.h>

DiskShape::DiskShape()
  :ComponentShape(),
   itsMajValue(Quantity(1,"'").getValue("rad")),
   itsMinValue(Quantity(1,"'").getValue("rad")),
   itsPaValue(Quantity(0,"deg").getValue("rad")),
   itsHeight(1.0/(C::pi*itsMajValue*itsMinValue)),
   itsMajUnit("arcmin"),
   itsMinUnit("arcmin"),
   itsPaUnit("deg")
{
  DebugAssert(ok(), AipsError);
}

DiskShape::DiskShape(const MDirection& direction, 
		     const Quantum<Double>& majorAxis,
		     const Quantum<Double>& minorAxis,
		     const Quantum<Double>& positionAngle)
  :ComponentShape(),
   itsMajValue(majorAxis.getValue("rad")),
   itsMinValue(minorAxis.getValue("rad")),
   itsPaValue(positionAngle.getValue("rad")),
   itsHeight(1.0/(C::pi*itsMajValue*itsMinValue)),
   itsMajUnit(majorAxis.getFullUnit()),
   itsMinUnit(minorAxis.getFullUnit()),
   itsPaUnit(positionAngle.getFullUnit())
{
  DebugAssert(ok(), AipsError);
}

DiskShape::DiskShape(const MDirection& direction,
		     const Quantum<Double>& width,
		     const Double axialRatio,
		     const Quantum<Double>& positionAngle) 
  :ComponentShape(),
   itsMajValue(width.getValue("rad")),
   itsMinValue(itsMajValue*axialRatio),
   itsPaValue(positionAngle.getValue("rad")),
   itsHeight(1.0/(C::pi*itsMajValue*itsMinValue)),
   itsMajUnit(width.getFullUnit()),
   itsMinUnit(width.getFullUnit()),
   itsPaUnit(positionAngle.getFullUnit())

{
  DebugAssert(ok(), AipsError);
}

DiskShape::DiskShape(const DiskShape& other) 
  :ComponentShape(),
   itsMajValue(other.itsMajValue),
   itsMinValue(other.itsMinValue),
   itsPaValue(other.itsPaValue),
   itsHeight(other.itsHeight),
   itsMajUnit(other.itsMajUnit),
   itsMinUnit(other.itsMinUnit),
   itsPaUnit(other.itsPaUnit)
{
  DebugAssert(ok(), AipsError);
}

DiskShape::~DiskShape() {
  DebugAssert(ok(), AipsError);
}

DiskShape& DiskShape::operator=(const DiskShape& other) {
  if (this != &other) {
    ComponentShape::operator=(other);
    itsMajValue = other.itsMajValue;
    itsMinValue = other.itsMinValue;
    itsPaValue = other.itsPaValue;
    itsHeight = other.itsHeight;
    itsMajUnit = other.itsMajUnit;
    itsMinUnit = other.itsMinUnit;
    itsPaUnit = other.itsPaUnit;
  }
  DebugAssert(ok(), AipsError);
  return *this;
}

ComponentType::Shape DiskShape::type() const {
  DebugAssert(ok(), AipsError);
  return ComponentType::DISK;
}

void DiskShape::setWidth(const Quantum<Double>& majorAxis,
			 const Quantum<Double>& minorAxis, 
			 const Quantum<Double>& positionAngle) {
  itsMajValue = majorAxis.getValue("rad");
  itsMinValue = minorAxis.getValue("rad");
  itsPaValue = positionAngle.getValue("rad");
  AlwaysAssert(itsMajValue > 0 && itsMinValue > 0 && itsMajValue >=itsMinValue,
	       AipsError);
  itsHeight = 1.0/(C::pi*itsMajValue*itsMinValue);
  itsMajUnit = majorAxis.getFullUnit();
  itsMinUnit = minorAxis.getFullUnit();
  itsPaUnit = positionAngle.getFullUnit();
  DebugAssert(ok(), AipsError);
}

void DiskShape::setWidth(const Quantum<Double>& majorAxis,
			 const Double axialRatio, 
			 const Quantum<Double>& positionAngle) {
  const Unit majUnit = majorAxis.getFullUnit();
  setWidth(majorAxis, 
	   Quantum<Double>(majorAxis.getValue(majUnit)*axialRatio, majUnit),
	   positionAngle);
  DebugAssert(ok(), AipsError);
}

void DiskShape::width(Quantum<Double>& majorAxis,
		      Quantum<Double>& minorAxis,
		      Quantum<Double>& positionAngle) const {
  DebugAssert(ok(), AipsError);
  const Unit rad("rad");
  majorAxis.setValue(itsMajValue);
  majorAxis.setUnit(rad);
  majorAxis.convert(itsMajUnit);
  minorAxis.setValue(itsMinValue);
  minorAxis.setUnit(rad);
  minorAxis.convert(itsMinUnit);
  positionAngle.setValue(itsPaValue);
  positionAngle.setUnit(rad);
  positionAngle.convert(itsPaUnit);
}

void DiskShape::width(Quantum<Double>& majorAxis, Double& axialRatio,
		      Quantum<Double>& positionAngle) const {
  DebugAssert(ok(), AipsError);
  const Unit rad("rad");
  majorAxis.setValue(itsMajValue);
  majorAxis.setUnit(rad);
  majorAxis.convert(itsMajUnit);
  axialRatio = itsMinValue/itsMajValue;
  positionAngle.setValue(itsPaValue);
  positionAngle.setUnit(rad);
  positionAngle.convert(itsPaUnit);
}

void DiskShape::majorAxis(Quantum<Double>& majorAxis) const {
  DebugAssert(ok(), AipsError);
  majorAxis.setValue(itsMajValue);
  majorAxis.setUnit("rad");
  majorAxis.convert(itsMajUnit);
}

Quantum<Double> DiskShape::majorAxis() const {
  DebugAssert(ok(), AipsError);
  Quantum<Double> retVal(itsMajValue, "rad");
  retVal.convert(itsMajUnit);
  return retVal;
}

void DiskShape::minorAxis(Quantum<Double>& minorAxis) const {
  DebugAssert(ok(), AipsError);
  minorAxis.setValue(itsMinValue);
  minorAxis.setUnit("rad");
  minorAxis.convert(itsMinUnit);
}

Quantum<Double> DiskShape::minorAxis() const {
  DebugAssert(ok(), AipsError);
  Quantum<Double> retVal(itsMinValue, "rad");
  retVal.convert(itsMinUnit);
  return retVal;
}

void DiskShape::axialRatio(Double& axialRatio) const {
  DebugAssert(ok(), AipsError);
  axialRatio = itsMinValue/itsMajValue;
}

Double DiskShape::axialRatio() const {
  DebugAssert(ok(), AipsError);
  return itsMinValue/itsMajValue;
}

void DiskShape::positionAngle(Quantum<Double>& positionAngle) const {
  DebugAssert(ok(), AipsError);
  positionAngle.setValue(itsPaValue);
  positionAngle.setUnit("rad");
  positionAngle.convert(itsPaUnit);
}

Quantum<Double> DiskShape::positionAngle() const {
  DebugAssert(ok(), AipsError);
  Quantum<Double> retVal(itsPaValue, "rad");
  retVal.convert(itsPaUnit);
  return retVal;
}

void DiskShape::sample(Flux<Double>& flux, const MDirection& direction, 
		       const MVAngle& pixelSize) const {
  DebugAssert(ok(), AipsError);
  Double separation;
  Double pa;
  MVDirection dirVal = direction.getValue();
  if ((MDirection::Types) direction.getRef().getType() != refDirFrame()) {
    const MVDirection dirVal = 
      MDirection::Convert(direction, refDirFrame())().getValue();
    separation = refDirValue().separation(dirVal);
    pa = refDirValue().positionAngle(dirVal) - itsPaValue;
  } else {
    const MVDirection& dirVal = direction.getValue();
    separation = refDirValue().separation(dirVal);
    pa = refDirValue().positionAngle(dirVal);
  }

  const Double x = abs(separation*cos(pa));
  const Double y = abs(separation*sin(pa));
  const Double majRad = itsMajValue/2.0; 
  const Double minRad = itsMinValue/2.0; 
  if ((x <= majRad) && 
      (y <= minRad) && 
      (y <= minRad * sqrt(0.25 - square(x/majRad)))) {
    const Double scale = itsHeight*square(pixelSize.radian());
    flux.scaleValue(scale, scale, scale, scale);
  } else {
    flux.setValue(0.0);
  }
}

void DiskShape::multiSample(Vector<Double>& scale, 
			    const Vector<MVDirection>& directions, 
			    const MVAngle& pixelSize) const {
  DebugAssert(ok(), AipsError);
  const uInt nSamples = directions.nelements();
  if (scale.nelements() == 0) scale.resize(nSamples);
  DebugAssert(scale.nelements() == nSamples, AipsError);
  
  Double separation;
  Double pa;
  const Double pixArea = square(pixelSize.radian());
  const Double majRad = itsMajValue/2.0; 
  const Double minRad = itsMinValue/2.0; 
  for (uInt i = 0; i < nSamples; i++) {
    const MVDirection& dirVal = directions(i);
    separation = refDirValue().separation(dirVal);
    scale(i) = 0.0;
    if (separation <= majRad) {
      pa = refDirValue().positionAngle(dirVal);
      const Double x = abs(separation*cos(pa));
      const Double y = abs(separation*sin(pa));
      if ((x <= majRad) && 
       	  (y <= minRad) && 
	  (y <= minRad * sqrt(0.25 - square(x/majRad)))) {
	scale(i) = itsHeight*pixArea;
      }
    }
  }
}

void DiskShape::visibility(Flux<Double>& flux, const Vector<Double>& uvw,
			   const Double& frequency) const {
  DebugAssert(uvw.nelements() == 3, AipsError);
  DebugAssert(frequency > 0, AipsError);
  DebugAssert(ok(), AipsError);
  Double u = uvw(0);
  Double v = uvw(1);
  if (!nearAbs(itsPaValue, 0.0, C::dbl_min)) {
    // If this function becomes a computation bottleneck then spa & cpa can be
    // cached as can itsMinValue/itsMajValue. My tests show it is not a
    // bottleneck at the moment.
    const Double cpa = cos(itsPaValue);
    const Double spa = sin(itsPaValue);
    u = u * cpa - v * spa;
    v = uvw(0) * spa + v * cpa;
  }
  u *= itsMinValue;
  v *= itsMajValue;
  const Double r = hypot(u, v) * C::pi * frequency/C::c;
  const Double scale = 2.0 * j1(r)/r;
  flux.scaleValue(scale, scale, scale, scale);
}

ComponentShape* DiskShape::clone() const {
  DebugAssert(ok(), AipsError);
  ComponentShape* tmpPtr = new DiskShape(*this);
  AlwaysAssert(tmpPtr != 0, AipsError);
  return tmpPtr;
}

uInt DiskShape::nParameters() const {
  DebugAssert(ok(), AipsError);
  return 3;
}

void DiskShape::setParameters(const Vector<Double>& newParms) {
  AlwaysAssert(newParms.nelements() == nParameters(), AipsError);
  DebugAssert(newParms(0) >= newParms(1), AipsError);
  DebugAssert(abs(newParms(2)) <= C::_2pi, AipsError);
  itsMajValue = newParms(0);
  itsMinValue = newParms(1);
  itsPaValue = newParms(2);
  itsHeight = C::pi * itsMajValue * itsMinValue;
  DebugAssert(ok(), AipsError);
}

void DiskShape::parameters(Vector<Double>& compParms) const {
  AlwaysAssert(compParms.nelements() == nParameters(), AipsError);
  compParms(0) = itsMajValue;
  compParms(1) = itsMinValue;
  compParms(2) = itsPaValue;
  DebugAssert(ok(), AipsError);
}

Bool DiskShape::fromRecord(String& errorMessage,
			   const RecordInterface& record) {
  if (!ComponentShape::fromRecord(errorMessage, record)) return False;
  Quantum<Double> majorAxis;
  {
    const String fieldString("majoraxis");
    if (!record.isDefined(fieldString)) {
      errorMessage += "The 'majoraxis' field does not exist\n";
      return False;
    }
    const RecordFieldId field(fieldString);
    if (!(record.dataType(field) == TpRecord || 
	  ((record.dataType(field) == TpString) && 
	   (record.shape(field) == IPosition(1,1))))) {
      errorMessage += "The 'majoraxis' field must be a record\n";
      errorMessage += "or a string (but not a vector of strings)\n";
      return False;
    }
    QuantumHolder qHolder;
    if (record.dataType(field) == TpString) {
      if (!qHolder.fromString(errorMessage, record.asString(field))) {
	errorMessage += "Problem parsing the 'majoraxis' string\n";
	return False;
      }
    } else if (!qHolder.fromRecord(errorMessage, record.asRecord(field))) {
      errorMessage += "Problem parsing the 'majoraxis' record\n";
      return False;
    }
    if (!(qHolder.isScalar() && qHolder.isReal())) {
      errorMessage += "The 'majoraxis' field is not a quantity\n";
      return False;
    }
    majorAxis = qHolder.asQuantumDouble();
    if (majorAxis.getFullUnit() != Unit("deg")) {
      errorMessage += "The 'majoraxis' field must have angular units\n";
      return False;
    }
  }
  Quantum<Double> minorAxis;
  {
    const String fieldString("minoraxis");
    if (!record.isDefined(fieldString)) {
      errorMessage += "The 'minoraxis' field does not exist\n";
      return False;
    }
    const RecordFieldId field(fieldString);
    if (!(record.dataType(field) == TpRecord || 
	  ((record.dataType(field) == TpString) && 
	   (record.shape(field) == IPosition(1,1))))) {
      errorMessage += "The 'minoraxis' field must be a record\n";
      errorMessage += "or a string (but not a vector of strings)\n";
      return False;
    }      
    QuantumHolder qHolder;
    if (record.dataType(field) == TpString) {
      if (!qHolder.fromString(errorMessage, record.asString(field))) {
	errorMessage += "Problem parsing the 'minoraxis' string\n";
	return False;
      }
    } else if (!qHolder.fromRecord(errorMessage, record.asRecord(field))) {
      errorMessage += "Problem parsing the 'minoraxis' record\n";
      return False;
    }
    if (!(qHolder.isScalar() && qHolder.isReal())) {
      errorMessage += "The 'minoraxis' field is not a quantity\n";
      return False;
    }
    minorAxis = qHolder.asQuantumDouble();
    if (minorAxis.getFullUnit() != Unit("deg")) {
      errorMessage += "The 'minoraxis' field must have angular units\n";
      return False;
    }
  }
  Quantum<Double> pa;
  {
    const String fieldString("positionangle");
    if (!record.isDefined(fieldString)) {
      errorMessage += "The 'positionangle' field does not exist\n";
      return False;
    }
    const RecordFieldId field(fieldString);
    if (!(record.dataType(field) == TpRecord || 
	  ((record.dataType(field) == TpString) && 
	   (record.shape(field) == IPosition(1,1))))) {
      errorMessage += "The 'positionangle' field must be a record\n";
      errorMessage += "or a string (but not a vector of strings)\n";
      return False;
    }      
    QuantumHolder qHolder;
    if (record.dataType(field) == TpString) {
      if (!qHolder.fromString(errorMessage, record.asString(field))) {
	errorMessage += "Problem parsing the 'positionangle' string\n";
	return False;
      }
    } else if (!qHolder.fromRecord(errorMessage, record.asRecord(field))) {
      errorMessage += "Problem parsing the 'positionangle' record\n";
      return False;
    }
    if (!(qHolder.isScalar() && qHolder.isReal())) {
      errorMessage += "The 'positionangle' field is not a quantity\n";
      return False;
    }
    pa = qHolder.asQuantumDouble();
    if (pa.getFullUnit() != Unit("deg")) {
      errorMessage += "The 'positionangle' field must have angular units\n";
      return False;
    }
  }
  const Unit rad("rad");
  if (majorAxis.getValue(rad) < minorAxis.getValue(rad)) {
    errorMessage += "The major axis cannot be smaller than the minor axis\n";
    return False;
  }
  setWidth(majorAxis, minorAxis, pa);
  DebugAssert(ok(), AipsError);
  return True;
}

Bool DiskShape::toRecord(String& errorMessage,
			     RecordInterface& record) const {
  DebugAssert(ok(), AipsError);
  if (!ComponentShape::toRecord(errorMessage, record)) return False;
  {
    const QuantumHolder qHolder(majorAxis());
    Record qRecord;
    if (!qHolder.toRecord(errorMessage, qRecord)) {
      errorMessage += "Cannot convert the major axis to a record\n";
      return False;
    }
    record.defineRecord(RecordFieldId("majoraxis"), qRecord);
  }
  {
    const QuantumHolder qHolder(minorAxis());
    Record qRecord;
    if (!qHolder.toRecord(errorMessage, qRecord)) {
      errorMessage += "Cannot convert the minor axis to a record\n";
      return False;
    }
    record.defineRecord(RecordFieldId("minoraxis"), qRecord);
  }
  {
    const QuantumHolder qHolder(positionAngle());
    Record qRecord;
    if (!qHolder.toRecord(errorMessage, qRecord)) {
      errorMessage += "Cannot convert the position angle to a record\n";
      return False;
    }
    record.defineRecord(RecordFieldId("positionangle"), qRecord);
  }
  return True;
}

Bool DiskShape::convertUnit(String& errorMessage,
			    const RecordInterface& record) {
  const Unit deg("deg");
  {
    const String fieldString("majoraxis");
    if (!record.isDefined(fieldString)) {
      errorMessage += "The 'majoraxis' field does not exist\n";
      return False;
    }
    const RecordFieldId field(fieldString);
    if (!((record.dataType(field) == TpString) && 
	  (record.shape(field) == IPosition(1,1)))) {
      errorMessage += "The 'majoraxis' field must be a string\n";
      errorMessage += "(but not a vector of strings)\n";
      return False;
    }
    const Unit unit = Unit(record.asString(field));
    if (unit != deg) {
      errorMessage += 
	"Cannot convert the major axis width to a non angular unit";
      return False;
    }
    itsMajUnit = unit;
  }
  {
    const String fieldString("minoraxis");
    if (!record.isDefined(fieldString)) {
      errorMessage += "The 'minoraxis' field does not exist\n";
      return False;
    }
    const RecordFieldId field(fieldString);
    if (!((record.dataType(field) == TpString) && 
	  (record.shape(field) == IPosition(1,1)))) {
      errorMessage += "The 'minoraxis' field must be a string\n";
      errorMessage += "(but not a vector of strings)\n";
      return False;
    }
    const Unit unit = Unit(record.asString(field));
    if (unit != deg) {
      errorMessage += 
	"Cannot convert the minor axis width to a non angular unit";
      return False;
    }
    itsMinUnit = unit;
  }
  {
    const String fieldString("positionangle");
    if (!record.isDefined(fieldString)) {
      errorMessage += "The 'positionangle' field does not exist\n";
      return False;
    }
    const RecordFieldId field(fieldString);
    if (!((record.dataType(field) == TpString) && 
	  (record.shape(field) == IPosition(1,1)))) {
      errorMessage += "The 'positionangle' field must be a string\n";
      errorMessage += "(but not a vector of strings)\n";
      return False;
    }
    const Unit unit = Unit(record.asString(field));
    if (unit != deg) {
      errorMessage += 
	"Cannot convert the position angle to a non angular unit";
      return False;
    }
    itsPaUnit = unit;
  }
  DebugAssert(ok(), AipsError);
  return True;
}

Bool DiskShape::ok() const {
  // The LogIO class is only constructed if an error is detected for
  // performance reasons. Both function static and file static variables
  // where considered and rejected for this purpose.
  if (!ComponentShape::ok()) return False;
  if (!near(itsHeight, 1.0/C::pi*itsMajValue*itsMinValue, C::dbl_epsilon)) {
    LogIO logErr(LogOrigin("DiskCompRep", "ok()"));
    logErr << LogIO::SEVERE << "The disk shape does not have"
	   << " unit area"
           << LogIO::POST;
    return False;
  }
  const Unit deg("deg");
  if (itsMajUnit != deg) {
    LogIO logErr(LogOrigin("DiskCompRep", "ok()"));
    logErr << LogIO::SEVERE << "The major axis does not have angular units."
           << LogIO::POST;
    return False;
  }
  if (itsMinUnit != deg) {
    LogIO logErr(LogOrigin("DiskCompRep", "ok()"));
    logErr << LogIO::SEVERE << "The minor axis does not have angular units."
           << LogIO::POST;
    return False;
  }
  if (itsPaUnit != deg) {
    LogIO logErr(LogOrigin("DiskCompRep", "ok()"));
    logErr << LogIO::SEVERE <<"The position angle does not have angular units."
           << LogIO::POST;
    return False;
  }
  return True;
}

// Local Variables: 
// compile-command: "gmake OPTLIB=1 DiskShape"
// End: 
