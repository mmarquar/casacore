//# ComponentShape.cc:
//# Copyright (C) 1998,1999,2000
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

#include <trial/ComponentModels/ComponentShape.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/IPosition.h>
#include <aips/Containers/Record.h>
#include <aips/Containers/RecordFieldId.h>
#include <aips/Containers/RecordInterface.h>
#include <trial/Coordinates/DirectionCoordinate.h>
#include <aips/Exceptions/Error.h>
#include <aips/Logging/LogIO.h>
#include <aips/Mathematics/Complex.h>
#include <aips/Measures/MeasureHolder.h>
#include <aips/Measures/MeasFrame.h>
#include <aips/Measures/MeasRef.h>
#include <aips/Quanta/Quantum.h>
#include <aips/Quanta/UnitVal.h>
#include <aips/Utilities/Assert.h>
#include <aips/Utilities/DataType.h>
#include <aips/Utilities/String.h>

ComponentShape::ComponentShape() 
  :itsDir(),
   itsDirErrLat(0, "rad"),
   itsDirErrLong(0, "rad")
{
  DebugAssert(ComponentShape::ok(), AipsError);
}

ComponentShape::ComponentShape(const MDirection& direction)
  :itsDir(direction),
   itsDirErrLat(0, "rad"),
   itsDirErrLong(0, "rad")
{
  DebugAssert(ComponentShape::ok(), AipsError);
}

ComponentShape::ComponentShape(const ComponentShape& other)
  :itsDir(other.itsDir),
   itsDirErrLat(other.itsDirErrLat),
   itsDirErrLong(other.itsDirErrLong)
{
  DebugAssert(ComponentShape::ok(), AipsError);
}


ComponentShape::~ComponentShape() {
}

const String& ComponentShape::ident() const {
  DebugAssert(ComponentShape::ok(), AipsError);
  static const String typeString = ComponentType::name(type());
  return typeString;
}

ComponentShape& ComponentShape::operator=(const ComponentShape& other) {
  if (this != &other) {
    itsDir = other.itsDir; 
    itsDirErrLat = other.itsDirErrLat;
    itsDirErrLong = other.itsDirErrLong;
  }
  DebugAssert(ComponentShape::ok(), AipsError);
  return *this;
}

void ComponentShape::setRefDirection(const MDirection& newRefDir) {
  itsDir = newRefDir;
  DebugAssert(ComponentShape::ok(), AipsError);
}

const MDirection& ComponentShape::refDirection() const {
  DebugAssert(ComponentShape::ok(), AipsError);
  return itsDir;
}

void 
ComponentShape::setRefDirectionError(const Quantum<Double>& newRefDirErrLat, 
				     const Quantum<Double>& newRefDirErrLong) {
  if (badError(newRefDirErrLat) || badError(newRefDirErrLong)) {
    LogIO logErr(LogOrigin("ComponentShape", "setRefDirectionError"));
    logErr << "The errors must be non-negative angular quantities."
	   << LogIO::EXCEPTION;
  }
  itsDirErrLat = newRefDirErrLat;
  itsDirErrLong = newRefDirErrLong;
  DebugAssert(ComponentShape::ok(), AipsError);
}

const Quantum<Double>& ComponentShape::refDirectionErrorLat() const {
  return itsDirErrLat;
}

const Quantum<Double>& ComponentShape::refDirectionErrorLong() const {
  return itsDirErrLong;
}

void ComponentShape::sample(Vector<Double>& scale, 
			    const Vector<MDirection::MVType>& directions, 
			    const MDirection::Ref& refFrame,
			    const MVAngle& pixelLatSize,
			    const MVAngle& pixelLongSize) const {
  DebugAssert(ComponentShape::ok(), AipsError);
  const uInt nSamples = directions.nelements();
  DebugAssert(scale.nelements() == nSamples, AipsError);

  for (uInt i = 0; i < nSamples; i++) {
    scale(i) = sample(MDirection(directions(i), refFrame), 
		      pixelLatSize, pixelLongSize);
  }
}

void ComponentShape::visibility(Vector<DComplex>& scale, 
				const Matrix<Double>& uvw,
				const Double& frequency) const {
  DebugAssert(ComponentShape::ok(), AipsError);
  const uInt nSamples = scale.nelements();
  DebugAssert(uvw.ncolumn() == nSamples, AipsError);
  DebugAssert(uvw.nrow() == 3, AipsError);

  for (uInt i = 0; i < nSamples; i++) {
    scale(i) = visibility(uvw.column(i), frequency);
  }
}

Bool ComponentShape::fromRecord(String& errorMessage,
				const RecordInterface& record) {
  ComponentType::Shape thisType = getType(errorMessage, record);
  if ( thisType != type()) {
    errorMessage += String("The 'type' field, in the shape record,") + 
      String(" is not the expected value of '") + 
      ComponentType::name(type()) + String("'\n");
  }
  const String dirString("direction");
  if (!record.isDefined(dirString)) {
    errorMessage += "The 'direction' field does not exist\n";
    return False;
  }
  const RecordFieldId direction(dirString);
  if (record.dataType(direction) != TpRecord) {
    if ((record.dataType(direction) == TpString) && 
	(record.shape(direction) == IPosition(1,1)) &&
	(record.asString(direction) == String("current"))) {
      return True;
    } else {
      errorMessage += "The 'direction' field must be a record\n";
      errorMessage += "or the string 'current' (to use the current value)\n";
      return False;
    }
  }
  const Record& dirRecord = record.asRecord(direction);
  MeasureHolder mh;
  if (!mh.fromRecord(errorMessage, dirRecord)) {
    errorMessage += "Could not parse the reference direction\n";
    return False;
  }
  if (!mh.isMDirection()) {
    errorMessage += "The reference direction is not a direction measure\n";
    return False;
  }
  setRefDirection(mh.asMDirection());
  DebugAssert(ComponentShape::ok(), AipsError);
  return True;
}

Bool ComponentShape::toRecord(String& errorMessage,
			      RecordInterface& record) const {
  DebugAssert(ComponentShape::ok(), AipsError);
  record.define(RecordFieldId("type"), ComponentType::name(type()));
  Record dirRecord;
  const MeasureHolder mh(refDirection());
  if (!mh.toRecord(errorMessage, dirRecord)) {
    errorMessage += "Could not convert the reference direction to a record\n";
    return False;
  }
  record.defineRecord(RecordFieldId("direction"), dirRecord);
  return True;
}

Bool ComponentShape::ok() const {
  if (ComponentShape::badError(itsDirErrLat)) {
    LogIO logErr(LogOrigin("ComponentShape", "ok()"));
    logErr << LogIO::SEVERE << "The latitude error is bad." << LogIO::POST;
    return False;
  }
  if (ComponentShape::badError(itsDirErrLong)) {
    LogIO logErr(LogOrigin("ComponentShape", "ok()"));
    logErr << LogIO::SEVERE << "The longitude error is bad." << LogIO::POST;
    return False;
  }
  return True;
}

ComponentType::Shape ComponentShape::getType(String& errorMessage,
					     const RecordInterface& record) {
  const String typeString("type");
  if (!record.isDefined(typeString)) {
    errorMessage += 
      String("The 'shape' record does not have a 'type' field.\n");
    return ComponentType::UNKNOWN_SHAPE;
  }
  const RecordFieldId type(typeString);
  if (record.dataType(type) != TpString) {
    errorMessage += String("The 'type' field, in the shape record,") + 
      String(" must be a String\n");
    return ComponentType::UNKNOWN_SHAPE;
  }      
  if (record.shape(type) != IPosition(1,1)) {
    errorMessage += String("The 'type' field, in the shape record,") + 
      String(" must have only 1 element\n");
    return ComponentType::UNKNOWN_SHAPE;
  }      
  const String& typeVal = record.asString(type);
  return ComponentType::shape(typeVal);
}

Vector<Double> ComponentShape::toPixel (const DirectionCoordinate& dirCoord) const
{
   Vector<Double> pixelCen(2);
   if (!dirCoord.toPixel(pixelCen, itsDir)) {
      LogIO os(LogOrigin("ComponentShape", "toPixel(...)"));
      os << "DirectionCoordinate conversion to pixel failed because "
         << dirCoord.errorMessage() << LogIO::EXCEPTION;
   }                                
   return pixelCen;
}


void ComponentShape::fromPixel (const Vector<Double>& parameters,
                                const DirectionCoordinate& dirCoord) 
{
   LogIO os(LogOrigin("ComponentShape", "fromPixel(...)"));
   if (parameters.nelements()!=2) {
      os << "You must give a vector of length 2" << LogIO::EXCEPTION;
   }
//
   if (!dirCoord.toWorld(itsDir, parameters)) {
      os << "DirectionCoordinate conversion to pixel failed because "
         << dirCoord.errorMessage() << LogIO::EXCEPTION;
   }                                
}


Bool ComponentShape::differentRefs(const MeasRef<MDirection>& ref1,
				   const MeasRef<MDirection>& ref2) {
  if (ref1.getType() != ref2.getType()) return True;
  //# The MeasRef<T>::getFrame function should really be const.
  const MeasFrame& frame1 = const_cast<MeasRef<MDirection>&>(ref1).getFrame();
  const MeasFrame& frame2 = const_cast<MeasRef<MDirection>&>(ref2).getFrame();
  if (frame1.empty() && frame2.empty()) return False;
  return frame1 == frame2;
  //# I should also check the offsets but I cannot see how to fish
  //# them out of the MeasRef<T> class
}

Bool ComponentShape::badError(const Quantum<Double>& quantum) {
  return !(quantum.check(UnitVal::ANGLE)) || (quantum.getValue() < 0.0);
}

// Local Variables: 
// compile-command: "gmake ComponentShape"
// End: 
