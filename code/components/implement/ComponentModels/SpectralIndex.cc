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

#include <trial/ComponentModels/SpectralIndex.h>
#include <trial/ComponentModels/Flux.h>
#include <trial/Measures/DOmeasures.h>
#include <trial/Tasking/MeasureParameterAccessor.h>
#include <aips/Containers/RecordInterface.h>
#include <aips/Containers/Record.h>
#include <aips/Exceptions/Error.h>
#include <aips/Glish/GlishRecord.h>
#include <aips/Mathematics/Constants.h>
#include <aips/Mathematics/Math.h>
#include <aips/Measures/MCFrequency.h>
#include <aips/Measures/MVFrequency.h>
#include <aips/Measures/MeasConvert.h>
#include <aips/Utilities/Assert.h>
#include <aips/Utilities/String.h>

#ifdef __GNUG__
typedef MeasConvert<MFrequency,MVFrequency,MCFrequency> 
  gpp_measconvert_mfrequency_mvfrequency_mcfrequency;
#endif

SpectralIndex::SpectralIndex()
  :itsRefFreq(),
   itsIndex(4, 0.0),
   itsRefFrame((MFrequency::Types) itsRefFreq.getRef().getType()),
   itsNu0(itsRefFreq.getValue().getValue()),
   itsIonly(True),
   itsIindex(itsIndex(0)),
   itsQindex(itsIndex(1)),
   itsUindex(itsIndex(2)),
   itsVindex(itsIndex(3))     
{
  DebugAssert(ok(), AipsError);
}

SpectralIndex::SpectralIndex(const MFrequency & refFreq, Double exponent)
  :itsRefFreq(refFreq),
   itsIndex(4, 0.0),
   itsRefFrame((MFrequency::Types) itsRefFreq.getRef().getType()),
   itsNu0(itsRefFreq.getValue().getValue()),
   itsIonly(True),
   itsIindex(itsIndex(0)),
   itsQindex(itsIndex(1)),
   itsUindex(itsIndex(2)),
   itsVindex(itsIndex(3))     
{
  itsIindex = exponent;
  DebugAssert(ok(), AipsError);
}

SpectralIndex::SpectralIndex(const MFrequency & refFreq,
			     const Vector<Double> & exponents)
  :itsRefFreq(refFreq),
   itsIndex(exponents),
   itsRefFrame((MFrequency::Types) itsRefFreq.getRef().getType()),
   itsNu0(itsRefFreq.getValue().getValue()),
   itsIonly(False),
   itsIindex(itsIndex(0)),
   itsQindex(itsIndex(1)),
   itsUindex(itsIndex(2)),
   itsVindex(itsIndex(3))     
{
  DebugAssert(exponents.nelements() == 4, AipsError);
  if (nearAbs(exponents(1), 0.0) && 
      nearAbs(exponents(2), 0.0) && 
      nearAbs(exponents(3), 0.0)) {
    itsIonly = True;
  }
  DebugAssert(ok(), AipsError);
}

SpectralIndex::SpectralIndex(const SpectralIndex & other) 
  :itsRefFreq(other.itsRefFreq),
   itsIndex(other.itsIndex.copy()),
   itsRefFrame(other.itsRefFrame),
   itsNu0(other.itsNu0),
   itsIonly(other.itsIonly),
   itsIindex(itsIndex(0)),
   itsQindex(itsIndex(1)),
   itsUindex(itsIndex(2)),
   itsVindex(itsIndex(3))     
{
  DebugAssert(ok(), AipsError);
}

SpectralIndex::~SpectralIndex() {
  DebugAssert(ok(), AipsError);
}

SpectralIndex & SpectralIndex::operator=(const SpectralIndex & other) {
  if (this != &other) {
    itsRefFreq = other.itsRefFreq;
    itsIndex = other.itsIndex;
    itsRefFrame = other.itsRefFrame;
    itsNu0 = other.itsNu0;
    itsIonly = other.itsIonly;
    itsIindex = itsIndex(0);
    itsQindex = itsIndex(1);
    itsUindex = itsIndex(2);
    itsVindex = itsIndex(3);
  }
  DebugAssert(ok(), AipsError);
  return *this;
}

ComponentType::SpectralShape SpectralIndex::spectralShape() const {
  DebugAssert(ok(), AipsError);
  return ComponentType::SPECTRAL_INDEX;
}

const MFrequency & SpectralIndex::refFrequency() const {
  DebugAssert(ok(), AipsError);
  return itsRefFreq;
}

void SpectralIndex::setRefFrequency(const MFrequency & newRefFrequency) {
  itsRefFreq = newRefFrequency;
  itsRefFrame = (MFrequency::Types) itsRefFreq.getRef().getType();
  itsNu0 = itsRefFreq.getValue().getValue();
  DebugAssert(ok(), AipsError);
}

const Double & SpectralIndex::index(const Stokes::StokesTypes which) const {
  DebugAssert(ok(), AipsError);
  switch (which) {
  case Stokes::I: 
    return itsIindex;
    break;
  case Stokes::Q:
    return itsQindex;
    break;
  case Stokes::U:
    return itsUindex;
    break;
  case Stokes::V:
    return itsVindex;
    break;
  default:
    throw(AipsError(String("SpectralIndex::index(Stokes::StokesTypes) - ") + 
		    String("Can only provide the spectral index for ")+ 
		    String("Stokes I,Q,U or V polarisations")));
  };
  return itsIindex;
}

void SpectralIndex::setIndex(const Double & newIndex, 
			     const Stokes::StokesTypes which) {
  switch (which) {
  case Stokes::I: 
    itsIindex = newIndex;
    break;
  case Stokes::Q:
    itsQindex = newIndex;
    break;
  case Stokes::U:
    itsUindex = newIndex;
    break;
  case Stokes::V:
    itsVindex = newIndex;
    break;
  default:
    throw(AipsError(String("SpectralIndex::setIndex") +
		    String("(Double,Stokes::StokesTypes) - ") +
		    String("Can only set the spectral index for ")+ 
		    String("Stokes I,Q,U or V polarisations")));
  };
  DebugAssert(ok(), AipsError);
}

const Vector<Double> & SpectralIndex::indices() const {
  DebugAssert(ok(), AipsError);
  return itsIndex;
}

void SpectralIndex::setIndices(const Vector<Double> & newIndices) {
  DebugAssert(newIndices.nelements() == 4, AipsError);
  itsIndex = newIndices;
  DebugAssert(ok(), AipsError);
}

void SpectralIndex::sample(Flux<Double> & scaledFlux, 
			   const MFrequency & centerFreq) const {
  DebugAssert(ok(), AipsError);
  DebugAssert(!nearAbs(itsNu0, 0.0, C::dbl_epsilon), AipsError);
  Double nu = centerFreq.getValue().getValue();
  if ((MFrequency::Types) centerFreq.getRef().getType() != itsRefFrame) {
    nu = MFrequency::Convert(centerFreq, itsRefFrame)().getValue().getValue();
  }
  if (!near(nu, itsNu0, C::dbl_epsilon)) {
    scaledFlux.convertPol(ComponentType::STOKES);
    const Double freqFactor = nu/itsNu0;
    const Double Iscale = pow(freqFactor, itsIindex);
    if (itsIonly) {
      scaledFlux.scaleValue(Iscale);
    } else {
      const Double Qscale = pow(freqFactor, itsQindex);
      const Double Uscale = pow(freqFactor, itsUindex);
      const Double Vscale = pow(freqFactor, itsVindex);
      scaledFlux.scaleValue(Iscale, Qscale, Uscale, Vscale);
    }
  }
}

SpectralModel * SpectralIndex::cloneSpectrum() const {
  DebugAssert(ok(), AipsError);
  SpectralModel * tmpPtr = new SpectralIndex(*this);
  AlwaysAssert(tmpPtr != 0, AipsError);
  return tmpPtr;
}

uInt SpectralIndex::nSpectralParameters() const {
  DebugAssert(ok(), AipsError);
  return 4;
}

void SpectralIndex::
setSpectralParameters(const Vector<Double> & newSpectralParms) {
  DebugAssert(newSpectralParms.nelements() == nSpectralParameters(),AipsError);
  itsIindex = newSpectralParms(0);
  itsQindex = newSpectralParms(1);
  itsUindex = newSpectralParms(2);
  itsVindex = newSpectralParms(3);
  DebugAssert(ok(), AipsError);
}

void SpectralIndex::spectralParameters(Vector<Double> & spectralParms) const {
  DebugAssert(ok(), AipsError);
  DebugAssert(spectralParms.nelements() == nSpectralParameters(),AipsError);
  spectralParms(0) = itsIindex;
  spectralParms(1) = itsQindex;
  spectralParms(2) = itsUindex;
  spectralParms(3) = itsVindex;
}

Bool SpectralIndex::fromRecord(String & errorMessage, 
			       const RecordInterface & record) {
  {
    if (!record.isDefined(String("reference"))) {
      errorMessage += "\nThe 'spectrum' record must have an 'reference' field";
      return False;
    }
    GlishRecord gRecord;
    gRecord.fromRecord(record);
    MeasureParameterAccessor<MFrequency> mpa(String("reference"),
					     ParameterSet::In, &gRecord);
    if (!mpa.copyIn(errorMessage)) return False;
    setRefFrequency(mpa());
  }
  {
    if (!record.isDefined(String("index"))) {
      errorMessage += "\nThe 'spectrum' record must have an 'index' field";
      return False;
    }
    const RecordFieldId index("index");
    const IPosition shape(1,4);
    if (record.shape(index) != shape) {
      errorMessage += "\nThe 'index' field must be a vector with 4 elements";
      return False;
    }
    Vector<Double> indexVal(shape);
    try {
      indexVal = record.asArrayDouble(index);
    }
    catch (AipsError x) {
      errorMessage += "\nThe 'index' field must contain a vector of numbers";
      return False;
    } end_try;
    setIndex(indexVal(0), Stokes::I);
    setIndex(indexVal(1), Stokes::Q);
    setIndex(indexVal(2), Stokes::U);
    setIndex(indexVal(3), Stokes::V);
  }
  DebugAssert(ok(), AipsError);
  return True;
}

Bool SpectralIndex::toRecord(String & errorMessage,
			     RecordInterface & record) const {
  // Use errorMessage for something to suppress a compiler warning
  if (&errorMessage == 0) {
  }
  record.define("type", ComponentType::name(spectralShape()));
  {
    const GlishRecord gRecord = measures::toRecord(refFrequency());
    Record refFreqRec;
    gRecord.toRecord(refFreqRec);
    record.defineRecord("reference", refFreqRec);
  }
  Vector<Double> indicies(4);
  indicies(0) = index(Stokes::I);
  indicies(1) = index(Stokes::Q);
  indicies(2) = index(Stokes::U);
  indicies(3) = index(Stokes::V);
  record.define("index", indicies);
  return True;
}

Bool SpectralIndex::ok() const {
  return True;
}

// spectrum := [type = 'constant']
// spectrum := [type = 'spectralindex',
//              reference = MFrequency,
//              indicies = [1.0, .5, .4, 1],
//              stokes = ['I', 'Q', 'U', 'V'],
//             ]
// spectrum := [type = 'discrete',
//              reference = MFrequency,
//              frequencyoffset[1] = [value = 0, unit='Hz'],
//              frequencyoffset[2] = [value = 1, unit='MHz'],
//              scale[1] = [2, 1, 5, 4],
//              scale[2] = [3, 2, 3, 4],
//              stokes = ['I', 'Q', 'U', 'V'],
//             ]
// spectrum := [type = 'rotationmeasure',
//              reference = MFrequency,
//              value = 1.0,
//              unit = 'rad.m^-2',
//              *frequencyoffset = [value = 0, unit = 'Hz']
//             ]
// spectrum := [type = 'gaussian',
//              reference = MFrequency,
//              fwhm.value = 1.0,
//              fwhm.unit = 'Hz',
//              scale = [.5, 0, 0],
//              stokes = ['I', 'QU', 'V'],
//              *frequencyoffset = [value = 0, unit = 'Hz']
//              *fluxoffset = [1, 1, 1],
//             ]
// spectrum := [type = 'composite',
//              reference = MFrequency,
//              types[1] = 'gaussian'
//              types[2] = 'gaussian',
//              parameters[1].fwhm = [value=10, unit='Hz']
//              parameters[1].frequencyoffset = [value=200, unit='MHz'],
//              parameters[1].scale = [2, 1, .5],
//              parameters[1].fluxoffset = [1, 1, 1],
//              parameters[1].stokes = ['I', 'QU', 'V'],
//              parameters[2].fwhm = [value=20, unit='Hz']
//              parameters[2].frequencyoffset = [value=87, unit='MHz'],
//              parameters[2].scale = [.5],
//              parameters[2].fluxoffset = [1],
//              parameters[2].stokes = ['V']
//             ]
// spectrum := [type = 'composite',
//              reference = MFrequency,
//              types[1] = 'spectralindex'
//              types[2] = 'rotationmeasure',
//              parameters[1].indicies = [1.0]
//              parameters[1].stokes = ['IQUV'],
//              parameters[2] = [value = 10, unit = 'rad.m^-2']
//             ]
// Local Variables: 
// compile-command: "gmake OPTLIB=1 SpectralIndex"
// End: 
