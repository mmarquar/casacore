//# SpectralCoordinate.cc: this defines SpectralCoordinate
//# Copyright (C) 1997,1998,1999,2000,2001
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
//#
//# $Id$

#include <trial/Coordinates/SpectralCoordinate.h>
//
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/ArrayUtil.h>
#include <aips/Utilities/Assert.h>
#include <aips/Containers/Record.h>
#include <aips/Functionals/Interpolate1D.h>
#include <aips/Functionals/ScalarSampledFunctional.h>
#include <aips/Mathematics/Math.h>
#include <aips/Measures/MFrequency.h>
#include <aips/Measures/VelocityMachine.h>
#include <aips/Quanta/Quantum.h>
#include <aips/Containers/RecordInterface.h>
#include <aips/Logging/LogIO.h>
#include <aips/Logging/LogOrigin.h>
#include <trial/FITS/FITSUtil.h>

#include <aips/strstream.h>


SpectralCoordinate::SpectralCoordinate()
: Coordinate(),
  type_p(MFrequency::TOPO), 
  restfreqs_p(0),
  restfreqIdx_p(0),
  worker_p(0.0,1.0,0.0,"Hz", "Frequency"),
  pVelocityMachine_p(0),
  prefVelType_p(MDoppler::RADIO),
  prefUnit_p("")
{
   restfreqs_p.resize(1);
   restfreqs_p(0) = 0.0;
   makeVelocityMachine (String("km/s"), prefVelType_p, 
                        worker_p.worldAxisUnits()(0),
                        type_p, restfreqs_p(restfreqIdx_p));
}

SpectralCoordinate::SpectralCoordinate(MFrequency::Types type,
				       Double f0, Double inc, Double refPix,
				       Double restFrequency)
: Coordinate(),
  type_p(type), 
  restfreqs_p(0),
  restfreqIdx_p(0),
  worker_p(f0, inc, refPix, "Hz", "Frequency"),
  pVelocityMachine_p(0),
  prefVelType_p(MDoppler::RADIO),
  prefUnit_p("")
{
   AlwaysAssert(restFrequency>=0.0, AipsError);
   restfreqs_p.resize(1);
   restfreqs_p(0) = max(0.0, restFrequency);
//
   makeVelocityMachine (String("km/s"), prefVelType_p, 
                        worker_p.worldAxisUnits()(0),   
                        type_p, restfreqs_p(restfreqIdx_p));
}


SpectralCoordinate::SpectralCoordinate(MFrequency::Types type, 
                                       const Quantum<Double>& f0,
                                       const Quantum<Double>& inc,
                                       Double refPix, 
                                       const Quantum<Double>& restFrequency)
: Coordinate(),
  type_p(type),
  restfreqs_p(0),
  restfreqIdx_p(0),
  pVelocityMachine_p(0),
  prefVelType_p(MDoppler::RADIO),
  prefUnit_p("")
{
   Unit hz("Hz");
   if (!f0.isConform(hz)) {
      throw(AipsError("Unit of reference frequency is not consistent with Hz"));
   }
   if (!inc.isConform(hz)) {
      throw(AipsError("Unit of frequency increment is not consistent with Hz"));
   }
   if (!restFrequency.isConform(hz)) {
      throw(AipsError("Unit of rest frequency is not consistent with Hz"));
   }
//
   AlwaysAssert(restFrequency.getValue(hz)>=0.0, AipsError);
   restfreqs_p.resize(1);
   restfreqs_p(0) = max(0.0, restFrequency.getValue(hz));
//
   worker_p = TabularCoordinate(f0.getValue(hz), inc.getValue(hz),
                                refPix, "Hz", "Frequency");
//
   makeVelocityMachine (String("km/s"), prefVelType_p, 
                        worker_p.worldAxisUnits()(0),   
                        type_p, restfreqs_p(restfreqIdx_p));
}


SpectralCoordinate::SpectralCoordinate(MFrequency::Types type, 
                                       const Vector<Double> &freqs,
                                       Double restFrequency)
: Coordinate(),
  type_p(type), 
  restfreqs_p(0),
  restfreqIdx_p(0),
  pVelocityMachine_p(0),
  prefVelType_p(MDoppler::RADIO),
  prefUnit_p("")
{
   AlwaysAssert(restFrequency>=0.0, AipsError);
   restfreqs_p.resize(1);
   restfreqs_p(0) = max(0.0, restFrequency);
//
   Vector<Double> channels(freqs.nelements());
   indgen(channels);
   worker_p = TabularCoordinate(channels, freqs, "Hz", "Frequency");
//
   makeVelocityMachine (String("km/s"), prefVelType_p, 
                        worker_p.worldAxisUnits()(0),   
                        type_p, restfreqs_p(restfreqIdx_p));
}


SpectralCoordinate::SpectralCoordinate(MFrequency::Types type, 
                                       const Quantum<Vector<Double> >& freqs,
                                       const Quantum<Double>& restFrequency)
: Coordinate(),
  type_p(type),
  restfreqs_p(0),
  restfreqIdx_p(0),
  pVelocityMachine_p(0),
  prefVelType_p(MDoppler::RADIO),
  prefUnit_p("")
{
   Unit hz("Hz");
   if (!freqs.isConform(hz)) {
      throw(AipsError("Unit of frequencies is not consistent with Hz"));
   }
   if (!restFrequency.isConform(hz)) {
      throw(AipsError("Unit of rest frequency is not consistent with Hz"));
   }
//
   AlwaysAssert(restFrequency.getValue(hz)>=0.0, AipsError);
   restfreqs_p.resize(1);
   restfreqs_p(0) = max(0.0, restFrequency.getValue(hz));
//
   Vector<Double> freqs2 = freqs.getValue(hz);
   Vector<Double> channels(freqs2.nelements());
   indgen(channels);
   worker_p = TabularCoordinate(channels, freqs2, "Hz", "Frequency");
//
   makeVelocityMachine (String("km/s"), prefVelType_p,
                        worker_p.worldAxisUnits()(0),
                        type_p, restfreqs_p(restfreqIdx_p));
}


SpectralCoordinate::SpectralCoordinate(const SpectralCoordinate &other)
: Coordinate(other),
  type_p(other.type_p),
  restfreqs_p(other.restfreqs_p.copy()),
  restfreqIdx_p(other.restfreqIdx_p),
  worker_p(other.worker_p),
  pVelocityMachine_p(0),
  prefVelType_p(other.prefVelType_p),
  prefUnit_p(other.prefUnit_p)
{
   deleteVelocityMachine();
   pVelocityMachine_p = new VelocityMachine(*(other.pVelocityMachine_p));
}

SpectralCoordinate &SpectralCoordinate::operator=(const SpectralCoordinate &other)
{
    if (this != &other) {
        Coordinate::operator=(other);
	type_p = other.type_p;
//
        restfreqs_p.resize(0);
        restfreqs_p = other.restfreqs_p;
//
	restfreqIdx_p = other.restfreqIdx_p;
	worker_p = other.worker_p;
//
        deleteVelocityMachine();
        pVelocityMachine_p = new VelocityMachine(*(other.pVelocityMachine_p));
//
        prefVelType_p = other.prefVelType_p;
        prefUnit_p = other.prefUnit_p;
    }
    return *this;
}

SpectralCoordinate::~SpectralCoordinate()
{
   deleteVelocityMachine();
}  


Coordinate::Type SpectralCoordinate::type() const
{
    return Coordinate::SPECTRAL;
}

String SpectralCoordinate::showType() const
{
    return String("Spectral");
}

uInt SpectralCoordinate::nPixelAxes() const
{
    return 1;
}

uInt SpectralCoordinate::nWorldAxes() const
{
    return 1;
}

Bool SpectralCoordinate::toWorld(Vector<Double> &world, 
				 const Vector<Double> &pixel) const
{
    return worker_p.toWorld(world, pixel);
}

Bool SpectralCoordinate::toPixel(Vector<Double> &pixel, 
				 const Vector<Double> &world) const
{
    return worker_p.toPixel(pixel, world);
}


Bool SpectralCoordinate::toWorld(Double& world, const Double& pixel) const
{
  return worker_p.toWorld(world, pixel);
}

Bool SpectralCoordinate::toPixel(Double& pixel, const Double& world) const
{
  return worker_p.toPixel(pixel, world);
}


Vector<String> SpectralCoordinate::worldAxisNames() const
{
    return worker_p.worldAxisNames();
}

Vector<String> SpectralCoordinate::worldAxisUnits() const
{
    return worker_p.worldAxisUnits();
}

Vector<Double> SpectralCoordinate::referencePixel() const
{
    return worker_p.referencePixel();
}

Matrix<Double> SpectralCoordinate::linearTransform() const
{
    return worker_p.linearTransform();
}

Vector<Double> SpectralCoordinate::increment() const
{
    return worker_p.increment();
}

Vector<Double> SpectralCoordinate::referenceValue() const
{
    return worker_p.referenceValue();
}

Bool SpectralCoordinate::setWorldAxisNames(const Vector<String> &names)
{
    return worker_p.setWorldAxisNames(names);
}

Bool SpectralCoordinate::setWorldAxisUnits(const Vector<String> &units)
{
    Double before = increment()(0);
    Bool ok = worker_p.setWorldAxisUnits(units);
    if (ok) {
	Double after = increment()(0);
        restfreqs_p *= after / before;
//
        pVelocityMachine_p->set(Unit(units(0)));
    }
//
    return ok;
}

Bool SpectralCoordinate::setPreferredWorldAxisUnits (const Vector<String>& units)
{
   if (units.nelements() != nWorldAxes()) {
      set_error("Wrong number of elements in preferred units vector");
      return False;
   }
//
   static const Unit unitsHZ(String("Hz"));
   static const Unit unitsKMS(String("km/s"));
//
   if (units(0).empty()) {
//
   } else {
      Unit unit0(units(0));
      if (unit0!=unitsHZ && unit0!=unitsKMS) {
         set_error("Unit must be empty or consistent with Hz or km/s");
         return False;
      }
   }
//
   prefUnit_p = units(0);
//
   return True;
}


Vector<String> SpectralCoordinate::preferredWorldAxisUnits () const
{
   Vector<String> t(1);
   t(0) = prefUnit_p;
   return t;
}

Bool SpectralCoordinate::setReferencePixel(const Vector<Double> &refPix)
{
    return worker_p.setReferencePixel(refPix);
}

Bool SpectralCoordinate::setLinearTransform(const Matrix<Double> &xform)
{
    return worker_p.setLinearTransform(xform);
}

Bool SpectralCoordinate::setIncrement(const Vector<Double> &inc) 
{
    return worker_p.setIncrement(inc);
}

Bool SpectralCoordinate::setReferenceValue(const Vector<Double> &refval)
{
    return worker_p.setReferenceValue(refval);
}

Double SpectralCoordinate::restFrequency() const
{
    return restfreqs_p(restfreqIdx_p);
}

Vector<Double> SpectralCoordinate::pixelValues() const
{
    return worker_p.pixelValues();
}

Vector<Double> SpectralCoordinate::worldValues() const
{
    return worker_p.worldValues();
}

MFrequency::Types SpectralCoordinate::frequencySystem() const
{
    return type_p;
}

void  SpectralCoordinate::setFrequencySystem(MFrequency::Types type)
{
    type_p = type;
//
    deleteVelocityMachine();
    makeVelocityMachine (String("km/s"), prefVelType_p, 
                         worker_p.worldAxisUnits()(0),
                         type_p, restfreqs_p(restfreqIdx_p));
}


Bool SpectralCoordinate::setRestFrequency(Double newFrequency, Bool append)
{
    newFrequency = max(0.0, newFrequency);
    if (append) {
       uInt n = restfreqs_p.nelements();
       restfreqs_p.resize(n+1, True);
       restfreqs_p(n) = newFrequency;
       restfreqIdx_p = n;
    } else {
       restfreqs_p(restfreqIdx_p) = newFrequency;
    }
//
    Quantum<Double> rf(restfreqs_p(restfreqIdx_p), worldAxisUnits()(0));
    pVelocityMachine_p->set(MVFrequency(rf));
//
    return True;
}

void SpectralCoordinate::setRestFrequencies(const Vector<Double>& restFrequencies,
                                            uInt which, Bool append)
{
   for (uInt i=0; i<restFrequencies.nelements(); i++) {
      AlwaysAssert(restFrequencies(i)>=0.0, AipsError);
   }
//
   if (append) {
      Vector<Double> tmp = concatenateArray (restfreqs_p, restFrequencies);
      restfreqs_p.resize(0);      
      restfreqs_p = tmp;
   } else {
      restfreqs_p.resize(0);
      restfreqs_p = restFrequencies;
   }
//
   AlwaysAssert(which>=0 && which<restfreqs_p.nelements(), AipsError);
   selectRestFrequency(which);
}

void SpectralCoordinate::selectRestFrequency(Double restFrequency)
{
   AlwaysAssert(restFrequency >= 0.0, AipsError);
   uInt which = 0;
   Double d, diff = 1.0e99;
   for (uInt i=0; i<restfreqs_p.nelements(); i++) {
      d = abs(restfreqs_p(i) - restFrequency);
      if (d < diff) {
         which = i;
         diff = d;
      }
   }
//
   selectRestFrequency(which);
}

void SpectralCoordinate::selectRestFrequency(uInt which)
{
   AlwaysAssert(which>=0 && which<restfreqs_p.nelements(), AipsError)
//
   restfreqIdx_p = which;
   Quantum<Double> rf(restfreqs_p(restfreqIdx_p), worldAxisUnits()(0));
   pVelocityMachine_p->set(MVFrequency(rf));
}


Bool SpectralCoordinate::near(const Coordinate& other,
                              Double tol) const
{
   Vector<Int> excludeAxes;
   return near(other, excludeAxes, tol);
}


Bool SpectralCoordinate::near(const Coordinate& other,
                              const Vector<Int>& excludeAxes,
                              Double tol) const
{
   if (this->type() != other.type()) {
      set_error("Comparison is not with another SpectralCoordinate");
      return False;
   }
//
   const SpectralCoordinate& sCoord = dynamic_cast<const SpectralCoordinate&>(other);
//
   if (type_p != sCoord.frequencySystem()) {
      set_error("The SpectralCoordinates have differing frequency systems");
      return False;
   }
//
   if (!::near(restFrequency(), sCoord.restFrequency(), tol)) {
      set_error("The SpectralCoordinates have differing active rest frequencies");
      return False;
   }

// Perhaps we shouldn't check the lists of rest frequencies. 
// Does it really matter ?

   const Vector<Double>& rfs = sCoord.restFrequencies();
   if (restfreqs_p.nelements() != rfs.nelements()) {
      set_error("The SpectralCoordinates have differing numbers of rest frequencies");
      return False;
   }
//
   for (uInt i=0; i<restfreqs_p.nelements(); i++) {
      if (!::near(restfreqs_p(i),rfs(i),tol)) {
         set_error("The SpectralCoordinates have differing lists of rest frequencies");
         return False;
      }
   }
//
   if (prefVelType_p != sCoord.preferredVelocityType()) {
      set_error("The SpectralCoordinates have differing preferred velocity types");
      return False;
   }

// Leave it to TabularCoordinate to report errors

   const TabularCoordinate& tmp = sCoord.worker_p;
   Bool ok = worker_p.near(tmp, excludeAxes, tol);
   if (!ok) set_error(worker_p.errorMessage());
//
   return ok;  
}



Bool SpectralCoordinate::save(RecordInterface &container,
			    const String &fieldName) const
{
    Bool ok = (!container.isDefined(fieldName));
    if (ok) {
	Record subrec;
	String system = "unknown";
	switch (type_p) {
	case MFrequency::REST: system = "REST"; break;
	case MFrequency::LSRD: system = "LSRD"; break;
	case MFrequency::LSRK: system = "LSRK"; break;
	case MFrequency::BARY: system = "BARY"; break;
	case MFrequency::GEO:  system = "GEO";  break;
	case MFrequency::TOPO: system = "TOPO"; break;
	case MFrequency::GALACTO: system = "GALACTO"; break;
	case MFrequency::N_Types: // Fallthrough
	default:
	    AlwaysAssert(0, AipsError); // NOTREACHED
	}
	subrec.define("system", system);
	subrec.define("restfreq", restFrequency());
        subrec.define("restfreqs", restFrequencies());
        subrec.define("prefVelType", Int(prefVelType_p));
        subrec.define("preferredunits", preferredWorldAxisUnits());
	ok = (worker_p.save(subrec, "tabular"));
//
	container.defineRecord(fieldName, subrec);
    }
    return ok;
}

SpectralCoordinate *SpectralCoordinate::restore(const RecordInterface &container,
                                                const String &fieldName)
{
    if (! container.isDefined(fieldName)) {
	return 0;
    }

    Record subrec(container.asRecord(fieldName));
    
    // We should probably do more type-checking as well as checking
    // for existence of the fields.
    if (! subrec.isDefined("system")) {
	return 0;
    }


    String system;
    subrec.get("system", system);
    MFrequency::Types sys;

    if (system == "LSR") {

// LSR is perpetuated in old images but is now deprecated in Measures
// So we must still read old ones not handled by MFrequency::getType

      sys = MFrequency::LSRK;
    } else {
      if (!MFrequency::getType(sys, system)) return 0;
    }
//
    if (!subrec.isDefined("restfreq")) {
	return 0;
    }
    Double restfreq;
    subrec.get("restfreq", restfreq);
//
    if (!subrec.isDefined("tabular")) {
	return 0;
    }
    TabularCoordinate* tabular = TabularCoordinate::restore(subrec, "tabular");
    if (tabular == 0) {
	return 0;
    }
//
    SpectralCoordinate* retval = new SpectralCoordinate;
    if (retval == 0) {
	return 0;
    }
    retval->worker_p = *tabular;
    delete tabular;
    retval->type_p = sys;
//
    if (subrec.isDefined("prefVelType")) {                 // optional
       MDoppler::Types type = 
          static_cast<MDoppler::Types>(subrec.asInt("prefVelType"));
       retval->setPreferredVelocityType(type);
    }
//
    if (subrec.isDefined("preferredunits")) {              // optional
       Vector<String> prefUnits;
       subrec.get("preferredunits", prefUnits);
       retval->setPreferredWorldAxisUnits(prefUnits);
    }
//
    if (subrec.isDefined("restfreqs")) {                   // optional
       Vector<Double> restFreqs;
       subrec.get("restfreqs", restFreqs);

// Old images might have a negative restfreq. Don't propagate that

        for (uInt i=0; i<restFreqs.nelements(); i++) {
           restFreqs(i) = max(0.0,restFreqs(i));
        }
//
       retval->setRestFrequencies(restFreqs, 0, False);
       retval->selectRestFrequency(restfreq);
    } else {
       retval->setRestFrequency(restfreq, False);
    }

// This is subtle.  The velocity machine is made empty because the default
// constructor was used.  Therefore we must recreate it with the
// correct internals

    retval->reinitializeVelocityMachine();
//
    return retval;
}

Coordinate *SpectralCoordinate::clone() const
{
    return new SpectralCoordinate(*this);
}


void SpectralCoordinate::toFITS(RecordInterface &header, uInt whichAxis, 
		LogIO &logger, Bool oneRelative, 
                Bool preferVelocity,  Bool opticalVelDef) const
{
    const Double offset(1.0*Int(oneRelative == True));

    logger << LogOrigin("SpectralCoordinate", "toFITS", WHERE);

    // Verify that the required headers exist and are the right type
    AlwaysAssert(header.isDefined("ctype") && 
                 header.dataType("ctype") == TpArrayString &&
		 header.shape("ctype").nelements() == 1 &&
                 header.shape("ctype")(0) > Int(whichAxis), AipsError);
    AlwaysAssert(header.isDefined("crval") && 
                 header.dataType("crval") == TpArrayDouble &&
		 header.shape("crval").nelements() == 1 && 
                 header.shape("crval")(0) > Int(whichAxis), AipsError);
    AlwaysAssert(header.isDefined("crpix") && 
		 header.dataType("crpix") == TpArrayDouble &&
		 header.shape("crpix").nelements() == 1 &&
		 header.shape("crpix")(0) > Int(whichAxis), AipsError);
    AlwaysAssert(header.isDefined("cdelt") && 
		 header.dataType("cdelt") == TpArrayDouble &&
		 header.shape("cdelt").nelements() == 1 &&
		 header.shape("cdelt")(0) > Int(whichAxis), AipsError);

    Vector<String> ctype, cunit;
    Vector<Double> crval, cdelt, crpix;
    header.get("ctype", ctype);
    header.get("crval", crval);
    header.get("crpix", crpix);
    header.get("cdelt", cdelt);

    if (header.isDefined("cunit")) {
	AlwaysAssert(header.dataType("cunit") == TpArrayString &&
		     header.shape("cunit").nelements() == 1 &&
		     header.shape("cunit")(0) > Int(whichAxis), AipsError);
	header.get("cunit", cunit);
    }

// If we are from a table, report how non-linear we are. At some point
// we should worry about nonlinear frequency axes more (e.g. they might
// be regularly gridded in lambda or velocities).

    if (pixelValues().nelements() != 0) {
	Vector<Double> pixel = pixelValues();
	Vector<Double> world = worldValues();
	Double crpix, cdelt, crval;
	crpix = referencePixel()(0);
	cdelt = increment()(0);
	crval = referenceValue()(0);
	Double maxDeviation = 0.0;
	Vector<Double> tmpworld(1), tmppixel(1);
	for (uInt i=0; i<pixel.nelements(); i++) {
	    tmppixel(0) = pixel(i);
	    Bool ok = toWorld(tmpworld, tmppixel);
	    if (!ok) {
		logger << LogIO::SEVERE << "Error calculating deviations "
		    "from linear" << errorMessage() << LogIO::POST;
		break;
	    }
	    Double actual = tmpworld(0);
	    Double linear = crval + cdelt*(pixel(i) - crpix);
	    maxDeviation = max(abs(actual-linear), maxDeviation);
	    if (maxDeviation != 0.0) {
		logger << LogIO::SEVERE << "Error in linearizing frequency "
		    "axis for FITS is " << maxDeviation << " " <<
		    worldAxisUnits()(0) << LogIO::POST;
	    }
	}
    }

// Wacky capitalization to avoid running into other variables

    String Ctype;
    Double Crval, Cdelt, Crpix, Altrval, Altrpix;
    Int Velref;
    Bool HaveAlt;
    Double Restfreq = Quantity(restfreqs_p(restfreqIdx_p),  // Canonicalize
			       worldAxisUnits()(0)).getBaseValue();
    Double RefFreq = Quantity(referenceValue()(0), 
			      worldAxisUnits()(0)).getBaseValue();
    Double FreqInc = Quantity(increment()(0), 
			      worldAxisUnits()(0)).getBaseValue();
    MDoppler::Types VelPreference = opticalVelDef ? MDoppler::OPTICAL :
	MDoppler::RADIO;
    AlwaysAssert(FITSSpectralUtil::toFITSHeader(Ctype, Crval, Cdelt, Crpix, HaveAlt, Altrval,
						Altrpix, Velref, Restfreq, logger,
						RefFreq, referencePixel()(0) + offset,
  					        FreqInc, type_p, preferVelocity,
						VelPreference), AipsError);

    ctype(whichAxis) = Ctype;
    crval(whichAxis) = Crval;
    crpix(whichAxis) = Crpix;
    cdelt(whichAxis) = Cdelt;
    if (cunit.nelements() > 0) {
	if (Ctype.contains("VELO") || Ctype.contains("FELO")) {
	    cunit(whichAxis) = "M/S";
	} else if (Ctype.contains("FREQ")) {
	    cunit(whichAxis) = "HZ";
	} else {
	    AlwaysAssert(0, AipsError); // NOTREACHED
	}
    }

    if (Restfreq > 0) {
	header.define("restfreq", Restfreq);
	header.setComment("restfreq", "Rest Frequency (Hz)");
    }
    if (HaveAlt) {
	header.define("altrval", Altrval);
	header.setComment("altrval", "Alternate frequency reference value");
	header.define("altrpix", Altrpix);
	header.setComment("altrpix", "Alternate frequency reference pixel");
	header.define("velref", Velref);
	header.setComment("velref", "1 LSR, 2 HEL, 3 OBS, +256 Radio");
	// the following agree with the current usage in FITSSpectralUtil
	// which in turn follows from Greisen, Paper III.  On the other
	// hand, that usage as applied here, to VELREF, is unlikely to
	// be understood by other FITS readers.  Still, its better than
	// doing nothing for these rest frames until the convention in
	// Paper III or its successor is formally adopted.
	FITSKeywordUtil::addComment(header, 
          "AIPS++ non-standard usage: 4 LSD, 5 GEO, 6 SOU, 7 GAL");
    }

    // OK, put the primary header information back
    header.define("ctype", ctype);
    header.define("crval", crval);
    header.define("crpix", crpix);
    header.define("cdelt", cdelt);
    if (cunit.nelements() > 0) {
	header.define("cunit", cunit);
    }
}

Bool SpectralCoordinate::fromFITS(SpectralCoordinate &out, String &,
				  const RecordInterface &header, 
				  uInt whichAxis, LogIO &logger,
				  Bool oneRelative)
{
    Int spectralAxis;
    Double referenceChannel, referenceFrequency, deltaFrequency;
    Vector<Double> frequencies;
    MFrequency::Types refFrame;
    MDoppler::Types velocityPreference;
    Double restFrequency;
//    
    Bool ok = FITSSpectralUtil::fromFITSHeader(spectralAxis,
					       referenceChannel,
					       referenceFrequency,
					       deltaFrequency,
					       frequencies,
					       refFrame,
					       velocityPreference,
					       restFrequency,
					       logger,
					       header,
					       'c',
					       oneRelative);
//
    restFrequency = max(0.0, restFrequency);
    if (ok && spectralAxis == Int(whichAxis)) {
	SpectralCoordinate tmp(refFrame, referenceFrequency, deltaFrequency, 
			       referenceChannel, restFrequency);
	out = tmp;
    } else if (ok && spectralAxis != Int(whichAxis)) {
	logger << LogOrigin("SpectralCoordinate", "fromFITS") << 
	    LogIO::SEVERE << "Disgreement about where the spectral axis is. " <<
	    spectralAxis << " vs. " << whichAxis << LogIO::POST;
	ok = False;
    }
//					       
    return ok;
}


Coordinate* SpectralCoordinate::makeFourierCoordinate (const Vector<Bool>& axes, 
                                                       const Vector<Int>& shape) const
//
// axes says which axes in the coordinate are to be transformed
// shape is the shape of the image for all axes in this coordinate
//
{   
   return worker_p.makeFourierCoordinate(axes, shape);
}

void SpectralCoordinate::deleteVelocityMachine ()
{
   delete pVelocityMachine_p;  
   pVelocityMachine_p = 0;
}


String SpectralCoordinate::format(String& units,
                                  Coordinate::formatType format,
                                  Double worldValue,
                                  uInt worldAxis,
                                  Bool isAbsolute,
                                  Bool showAsAbsolute,
                                  Int precision)
{
   AlwaysAssert(worldAxis < nWorldAxes(), AipsError);
    
// Check format
                                   
   Coordinate::formatType form = format;
   checkFormat (form, showAsAbsolute);
                                   
// Set default precision
                                   
   Int prec = precision;
   if (prec < 0) getPrecision(prec, form, showAsAbsolute, -1, -1, -1);

// If units are empty used preferred unit.   
// If given units are not consistent with native units
// then see if they are velocity.  If so, convert to
// desired units.
  
   static const Unit unitsHZ(String("Hz"));      
   static const Unit unitsKMS(String("km/s"));      
   static Quantum<Double> qVel;
   static Quantum<Double> qFreq;
   static Vector<Double> world;
//
   String nativeUnit = worldAxisUnits()(worldAxis);
   String prefUnit;
   if (units.empty()) {
      prefUnit = preferredWorldAxisUnits()(worldAxis);
      if (prefUnit.empty()) {      
         units = nativeUnit;
      } else {
         units = prefUnit;
      }
   }
//
   String theString;
   Unit unit(units);
   if (unit != unitsHZ) {

// Requested unit is not consistent with Hz.  

      if (unit != unitsKMS) {
        throw(AipsError("Requested units are invalid for a SpectralCoordinate"));
      }

// Requested unit is consistent with km/s

      if (world.nelements()!=nWorldAxes()) world.resize(nWorldAxes());
      String tunits(units);

// We must convert to absolute first (regardless of how we want
// to see the value) as we are formatting in km/s

      if (!isAbsolute) {
         world = 0.0;
         world(worldAxis) = worldValue; 
         makeWorldAbsolute(world);
         worldValue = world(worldAxis);
      }
//
      updateVelocityMachine (tunits, prefVelType_p);
      if (showAsAbsolute) {
         if (!frequencyToVelocity (qVel, worldValue)) {
            theString = "Fail";
            return theString;
         }
         worldValue = qVel.getValue();
      } else {

// Find relative coordinate in km/s consistent units

         static Vector<Double> vel(2), freq2(2);
         freq2(0) = referenceValue()(worldAxis);
         freq2(1) = worldValue;
         if (!frequencyToVelocity(vel, freq2)) {
            theString = "Fail";
            return theString;
         }
         worldValue = vel(1) - vel(0);           // rel = abs - ref
      }
//
      ostrstream oss;
      if (form == Coordinate::SCIENTIFIC) {
         oss.setf(ios::scientific, ios::floatfield);
         oss.precision(prec);
         oss << worldValue;  
      } else if (form == Coordinate::FIXED) {
         oss.setf(ios::fixed, ios::floatfield);
         oss.precision(prec);
         oss << worldValue;
      }
      theString = String(oss);
   } else {
      theString = Coordinate::format(units, form, worldValue, worldAxis,
                                     isAbsolute, showAsAbsolute, precision);
   }
//
   return theString;
}
  

void SpectralCoordinate::checkFormat(Coordinate::formatType& format,
                                     const Bool ) const
{  
// Scientific or fixed formats only are allowed.
// Absolute or offset is irrelevant

   if (format == Coordinate::DEFAULT) {  
      format = Coordinate::SCIENTIFIC;
   } else {
      if (format != Coordinate::SCIENTIFIC &&
          format != Coordinate::FIXED) format = Coordinate::SCIENTIFIC;
   }
}

const Vector<Double>& SpectralCoordinate::restFrequencies() const
{
   return restfreqs_p;
}



String SpectralCoordinate::formatRestFrequencies () const
{  
   const Vector<Double>& rfs = restFrequencies();
   Double rf = restFrequency();
   String unit = worldAxisUnits()(0);
   const uInt n = rfs.nelements();
//
   if (n==0) return String("");
         
// It should never be that the active rest frequency is zero
// but there is more than one.  Zero is often used when making
// a continuum SpectralCoordinate where the restfreq is irrelevant
   
   ostrstream oss;
   if (rf > 0.0) {
      oss << "Rest frequency      : " << rf;
//
      if (n > 1) {
         oss << " [";
         uInt j = 0;
         for (uInt i=0; i<n; i++) {
            if (!::near(rfs(i), rf)) {
               if (j > 0) oss << ", ";
               oss << rfs(i);
               j++;
            }
         }
         oss << "]";
      }
//
      oss << " " << unit << ends;
   }   
//
   return String(oss);
}
