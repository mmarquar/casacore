//# SkyComponent.cc:  this defines SkyComponent
//# Copyright (C) 1996,1997,1998,1999
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

#include <trial/ComponentModels/SkyComponent.h>
#include <trial/ComponentModels/ComponentShape.h>
#include <trial/ComponentModels/Flux.h>
#include <trial/ComponentModels/SkyCompRep.h>
#include <trial/ComponentModels/SpectralModel.h>
#include <aips/Arrays/Vector.h>
#include <aips/Containers/RecordInterface.h>
#include <aips/Exceptions/Error.h>
#include <aips/Logging/LogIO.h>
#include <aips/Logging/LogOrigin.h>
#include <aips/Measures/MDirection.h>
#include <aips/Measures/MFrequency.h>
#include <aips/Quanta/MVAngle.h>
#include <aips/Utilities/Assert.h>
#include <aips/Utilities/String.h>

SkyComponent::SkyComponent()
  :itsCompPtr(new SkyCompRep) 
{
  DebugAssert(ok(), AipsError);
}

SkyComponent::SkyComponent(const ComponentType::Shape& shape)   
  :itsCompPtr(new SkyCompRep(shape))
{
  DebugAssert(ok(), AipsError);
}

SkyComponent::SkyComponent(const ComponentType::Shape& shape,
			   const ComponentType::SpectralShape& spectralModel) 
  :itsCompPtr(new SkyCompRep(shape, spectralModel))
{
  DebugAssert(ok(), AipsError);
}

SkyComponent::SkyComponent(const Flux<Double>& flux,
			   const ComponentShape& shape, 
			   const SpectralModel& spectrum)
  :itsCompPtr(new SkyCompRep(flux, shape, spectrum))
{
  DebugAssert(ok(), AipsError);
}

SkyComponent::SkyComponent(const SkyComponent& other) 
  :itsCompPtr(other.itsCompPtr)
{ 
  DebugAssert(ok(), AipsError);
}

SkyComponent::~SkyComponent() {
  DebugAssert(ok(), AipsError);
}

SkyComponent& SkyComponent::operator=(const SkyComponent& other) {
  if (this != &other)
    itsCompPtr = other.itsCompPtr;
  DebugAssert(ok(), AipsError);
  return *this;
}

Flux<Double>& SkyComponent::flux() {
  DebugAssert(ok(), AipsError);
  return itsCompPtr->flux();
}

const Flux<Double>& SkyComponent::flux() const {
  DebugAssert(ok(), AipsError);
  return itsCompPtr->flux();
}

const ComponentShape& SkyComponent::shape() const {
  DebugAssert(ok(), AipsError);
  return itsCompPtr->shape();
}

ComponentShape& SkyComponent::shape() {
  DebugAssert(ok(), AipsError);
  return itsCompPtr->shape();
}

void SkyComponent::setShape(const ComponentShape& newShape) {
  DebugAssert(ok(), AipsError);
  itsCompPtr->setShape(newShape);
}

const SpectralModel& SkyComponent::spectrum() const {
  DebugAssert(ok(), AipsError);
  return itsCompPtr->spectrum();
}

SpectralModel& SkyComponent::spectrum() {
  DebugAssert(ok(), AipsError);
  return itsCompPtr->spectrum();
}

void SkyComponent::setSpectrum(const SpectralModel& newSpectrum) {
  DebugAssert(ok(), AipsError);
  itsCompPtr->setSpectrum(newSpectrum);
}

String& SkyComponent::label() {
  DebugAssert(ok(), AipsError);
  return itsCompPtr->label();
}

const String& SkyComponent::label() const {
  DebugAssert(ok(), AipsError);
  return itsCompPtr->label();
}

Flux<Double> SkyComponent::sample(const MDirection& direction, 
			      const MVAngle& pixelSize, 
			      const MFrequency& centerFrequency) const {
  DebugAssert(ok(), AipsError);
  return itsCompPtr->sample(direction, pixelSize, centerFrequency);
}

void SkyComponent::sample(Matrix<Flux<Double> >& samples,
			  const Vector<MVDirection>& directions, 
			  const MeasRef<MDirection>& dirRef, 
			  const MVAngle& pixelSize, 
			  const Vector<MVFrequency>& frequencies,
			  const MeasRef<MFrequency>& freqRef) const {
  DebugAssert(ok(), AipsError);
  itsCompPtr->sample(samples, directions, dirRef, pixelSize, 
		     frequencies, freqRef);
}

Flux<Double> SkyComponent::visibility(const Vector<Double>& uvw,
 				      const Double& frequency) const {
  DebugAssert(ok(), AipsError);
  return itsCompPtr->visibility(uvw, frequency);
}

Bool SkyComponent::fromRecord(String& errorMessage, 
 			      const RecordInterface& record) {
  DebugAssert(ok(), AipsError);
  return itsCompPtr->fromRecord(errorMessage, record);
}

Bool SkyComponent::toRecord(String& errorMessage,
 			    RecordInterface& record) const {
  DebugAssert(ok(), AipsError);
  return itsCompPtr->toRecord(errorMessage, record);
}

Bool SkyComponent::fromRecord(String& error, const GlishRecord& in) {
  return itsCompPtr->SkyCompBase::fromRecord(error, in);
}

Bool SkyComponent::toRecord(String& error, GlishRecord& out) const {
  return itsCompPtr->SkyCompBase::toRecord(error, out);
}

SkyComponent SkyComponent::copy() const {
  DebugAssert(ok(), AipsError);
  SkyComponent newComp(flux().copy(), shape(), spectrum());
  newComp.label() = label();
  return newComp;
}

Bool SkyComponent::ok() const {
  if (itsCompPtr.null()) {
    LogIO logErr(LogOrigin("SkyComponent", "ok()"));
    logErr << LogIO::SEVERE << "Internal pointer is not pointing to anything"
           << LogIO::POST;
    return False;
  }
  if (!itsCompPtr->ok()) {
    LogIO logErr(LogOrigin("SkyComponent", "ok()"));
    logErr << LogIO::SEVERE << "Component representation is not ok"
           << LogIO::POST;
    return False;
  }
  return True;
}
// Local Variables: 
// compile-command: "gmake SkyComponent"
// End: 
