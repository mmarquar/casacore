//# ClassFileName.cc:  this defines ClassName, which ...
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

#include <trial/ComponentModels/ConstantSpectrum.h>
#include <aips/Exceptions/Error.h>
#include <aips/Containers/RecordInterface.h>
#include <aips/Utilities/Assert.h>
#include <aips/Utilities/String.h>

ConstantSpectrum::ConstantSpectrum()
  :itsRefFreq()
{
  DebugAssert(ok(), AipsError);
}

ConstantSpectrum::ConstantSpectrum(const ConstantSpectrum & other) 
  :itsRefFreq(other.itsRefFreq)
{
  DebugAssert(ok(), AipsError);
}

ConstantSpectrum::~ConstantSpectrum() {
  DebugAssert(ok(), AipsError);
}

ConstantSpectrum & ConstantSpectrum::operator=(const ConstantSpectrum & other) {
  if (this != &other) {
    itsRefFreq = other.itsRefFreq;
  }
  DebugAssert(ok(), AipsError);
  return *this;
}

ComponentType::SpectralShape ConstantSpectrum::spectralShape() const {
  DebugAssert(ok(), AipsError);
  return ComponentType::CONSTANT_SPECTRUM;
}

const MFrequency & ConstantSpectrum::refFrequency() const {
  DebugAssert(ok(), AipsError);
  return itsRefFreq;
}

void ConstantSpectrum::setRefFrequency(const MFrequency & newRefFreq) {
  itsRefFreq = newRefFreq;
  DebugAssert(ok(), AipsError);
}

void ConstantSpectrum::sample(Flux<Double> & flux, 
			      const MFrequency & centerFreq) const {
  DebugAssert(ok(), AipsError);
  // Use centerFreq for something to suppress a compiler warning
  if (&centerFreq == 0) {
  }
  // Use flux for something to suppress a compiler warning
  if (&flux == 0) {
  }
}

SpectralModel * ConstantSpectrum::cloneSpectrum() const {
  DebugAssert(ok(), AipsError);
  SpectralModel * tmpPtr = new ConstantSpectrum(*this);
  AlwaysAssert(tmpPtr != 0, AipsError);
  return tmpPtr;
}

uInt ConstantSpectrum::nSpectralParameters() const {
  DebugAssert(ok(), AipsError);
  return 0;
}

void ConstantSpectrum::
setSpectralParameters(const Vector<Double> & newSpectralParms) {
  DebugAssert(newSpectralParms.nelements() == nSpectralParameters(),AipsError);
  DebugAssert(ok(), AipsError);
}

void ConstantSpectrum::spectralParameters(Vector<Double> & spectralParms) const {
  DebugAssert(ok(), AipsError);
  DebugAssert(spectralParms.nelements() == nSpectralParameters(),AipsError);
}

Bool ConstantSpectrum::fromRecord(String & errorMessage, 
				  const RecordInterface & record) {
  // Use errorMessage for something to suppress a compiler warning
  if (&errorMessage == 0) {
  }
  // Use record for something to suppress a compiler warning
  if (&record == 0) {
  }
  return True;
}

Bool ConstantSpectrum::toRecord(String & errorMessage,
				RecordInterface & record) const {
  // Use errorMessage for something to suppress a compiler warning
  if (&errorMessage == 0) {
  }
  record.define("type", ComponentType::name(spectralShape()));
  return True;
}

Bool ConstantSpectrum::ok() const {
  return True;
}
// Local Variables: 
// compile-command: "gmake OPTLIB=1 ConstantSpectrum"
// End:
