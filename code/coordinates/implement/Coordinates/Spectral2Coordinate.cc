//# Spectral2Coordinate.cc: this defines Measures related SpectralCoordinate functions
//# Copyright (C) 1997,1998,1999,2000,2001,2002
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

#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/Vector.h>
#include <trial/Coordinates/CoordinateUtil.h>
#include <aips/Logging/LogIO.h>
#include <aips/Mathematics/Math.h>
#include <aips/Measures/VelocityMachine.h>
#include <aips/Measures/MeasConvert.h>
#include <aips/Measures/MFrequency.h>
#include <aips/Measures/MDoppler.h>
#include <aips/Quanta/MVFrequency.h>
#include <aips/Quanta/Quantum.h>
#include <aips/Quanta/Unit.h>
#include <aips/Utilities/String.h>


SpectralCoordinate::SpectralCoordinate(MFrequency::Types freqType,
                                       MDoppler::Types velType,
                                       const Vector<Double>& velocities,
                                       const String& velUnit,
                                       Double restFrequency )
: Coordinate(),
  type_p(freqType),
  restfreqs_p(0),
  restfreqIdx_p(0),
  pConversionMachineTo_p(0),
  pConversionMachineFrom_p(0),
  pVelocityMachine_p(0),
  prefVelType_p(MDoppler::RADIO),
  unit_p("Hz")
{
   restfreqs_p.resize(1);
   restfreqs_p(0) = restFrequency;

// Convert to frequency 

   makeVelocityMachine (velUnit, velType, String("Hz"), freqType, restFrequency);
   Quantum<Vector<Double> > frequencies = pVelocityMachine_p->makeFrequency(velocities);
  
// Construct

   Vector<Double> channels(velocities.nelements());
   indgen(channels);
   worker_p = TabularCoordinate(channels, frequencies.getValue(), "Hz", "Frequency");
}                                      
 

Bool SpectralCoordinate::toWorld(MFrequency& world, 
				 Double pixel) const
{
    static MVFrequency world_tmp;
    if (toWorld(world_tmp, pixel)) {
       world.set(world_tmp, MFrequency::Ref(type_p));
       return True;
    }
    return False;
}

Bool SpectralCoordinate::toWorld(MVFrequency& world, 
				 Double pixel) const
{
    Double world_tmp;
    static Quantum<Double> q_tmp;
//
    if (toWorld(world_tmp, pixel)) {
       q_tmp.setValue(world_tmp);
       q_tmp.setUnit(Unit(worldAxisUnits()(0)));
       world = MVFrequency(q_tmp);
       return True;
    }
    return False;
}

Bool SpectralCoordinate::toPixel(Double& pixel,
                                 const MFrequency& world) const
{
    return toPixel(pixel, world.getValue());
}
 
Bool SpectralCoordinate::toPixel(Double& pixel,
                                 const MVFrequency& world) const
{
   Double world_tmp;
   world_tmp = world.get(Unit(worldAxisUnits()(0))).getValue();
   return toPixel(pixel, world_tmp);
}
 
Bool SpectralCoordinate::pixelToVelocity (Quantum<Double>& velocity, Double pixel)
{
   Double world;
   if (!toWorld(world, pixel)) return False;
   return frequencyToVelocity (velocity, world);
}

Bool SpectralCoordinate::pixelToVelocity (Double& velocity, Double pixel)
{
   Double world;
   if (!toWorld(world, pixel)) return False;
   velocity = pVelocityMachine_p->makeVelocity(world).getValue();
//
   return True;
}

Bool SpectralCoordinate::pixelToVelocity (Vector<Double>& velocity, const Vector<Double>& pixel)
{
   velocity.resize(pixel.nelements());

// Perhaps its faster to make a vector of world and do them 
// all in one go in the machine ?  Haven't tested it.

   Double world;
   for (uInt i=0; i<pixel.nelements(); i++) {
      if (!toWorld(world, pixel(i))) return False;
      velocity(i) = pVelocityMachine_p->makeVelocity(world).getValue();
   }
//
   return True;
}

Bool SpectralCoordinate::frequencyToVelocity (Quantum<Double>& velocity, Double frequency)
{
   velocity = pVelocityMachine_p->makeVelocity(frequency);
   MVFrequency mvf(frequency);
//
   return True;
}


Bool SpectralCoordinate::frequencyToVelocity (Double& velocity, Double frequency)
{
   static Quantum<Double> t;
   t = pVelocityMachine_p->makeVelocity(frequency);
   velocity = t.getValue();
   return True;
}

Bool SpectralCoordinate::frequencyToVelocity (Vector<Double>& velocity, const Vector<Double>& frequency)

{
   velocity.resize(frequency.nelements());
   velocity = pVelocityMachine_p->makeVelocity(frequency).getValue();
//
   return True;
}

Bool SpectralCoordinate::frequencyToVelocity (Quantum<Double>& velocity, const MFrequency& frequency)
{
   return frequencyToVelocity(velocity, frequency.getValue());
}

Bool SpectralCoordinate::frequencyToVelocity (Quantum<Double>& velocity, const MVFrequency& frequency)
{
   velocity = pVelocityMachine_p->operator()(frequency);
   return True;
}

Bool SpectralCoordinate::velocityToPixel (Double& pixel, Double velocity)
{
   Double frequency;
   if (!velocityToFrequency(frequency, velocity)) return False;
   return toPixel(pixel, frequency);
}

Bool SpectralCoordinate::velocityToPixel (Vector<Double>& pixel, const Vector<Double>& velocity)
{
   pixel.resize(velocity.nelements());
   Double frequency, pix;
   for (uInt i=0; i<velocity.nelements(); i++) {
      if (!velocityToFrequency(frequency, velocity(i))) return False;
      if (!toPixel(pix, frequency)) return False;
      pixel(i) = pix;
   }
//
   return True;
}

Bool SpectralCoordinate::velocityToFrequency (Double& frequency, Double velocity)
{
   frequency = pVelocityMachine_p->makeFrequency (velocity).getValue();
   return True;
}

Bool SpectralCoordinate::velocityToFrequency (Vector<Double>& frequency, 
                                              const Vector<Double>& velocity)
{
   frequency.resize(velocity.nelements());
   for (uInt i=0; i<velocity.nelements(); i++) {
      frequency(i) = pVelocityMachine_p->makeFrequency (velocity(i)).getValue();
   }
//
   return True;
}

void SpectralCoordinate::makeVelocityMachine (const String& velUnit, 
                                              MDoppler::Types velType,
                                              const String& freqUnit, 
                                              MFrequency::Types freqType,
                                              Double restFreq)
{
   Unit fU(freqUnit);
   Quantum<Double> rF(restFreq, fU);
   pVelocityMachine_p = new VelocityMachine(MFrequency::Ref(freqType), fU,
                                            MVFrequency(rF), MDoppler::Ref(velType), 
                                            Unit(velUnit));
}


void SpectralCoordinate::updateVelocityMachine (const String& velUnit, 
                                                MDoppler::Types velType)
{
   if (pVelocityMachine_p->getDopplerUnits().getName() != velUnit) {
      pVelocityMachine_p->set(Unit(velUnit));
   }
   if (MDoppler::castType(pVelocityMachine_p->getDopplerReference().getType()) != velType) {
      pVelocityMachine_p->set(MDoppler::Ref(velType));
   }
}

void SpectralCoordinate::reinitializeVelocityMachine ()
{
   Unit fU(worldAxisUnits()(0));
   Quantum<Double> rF(restfreqs_p(restfreqIdx_p), fU);
//
   deleteVelocityMachine();
   pVelocityMachine_p = new VelocityMachine(MFrequency::Ref(type_p), fU,
                                            MVFrequency(rF), MDoppler::Ref(MDoppler::RADIO), 
                                            Unit(String("km/s")));
}


void SpectralCoordinate::makeConversionMachines (const MEpoch& epoch, const MPosition& position,
                                                 const MDirection& direction)
{
   LogIO os(LogOrigin("SpectralCoordinate", "makeConversionMachines"));
//
   pConversionMachineTo_p = new MFrequency::Convert();
   Bool madeItTo = CoordinateUtil::makeFrequencyMachine (os, *pConversionMachineTo_p,
                                                         type_p, conversionType_p,
                                                         direction, direction, 
                                                         epoch, epoch,
                                                         position, position);
   pConversionMachineFrom_p = new MFrequency::Convert();
   Bool madeItFrom = CoordinateUtil::makeFrequencyMachine (os, *pConversionMachineFrom_p,
                                                           conversionType_p, type_p,
                                                           direction, direction,
                                                           epoch, epoch,
                                                           position, position);
// If the machines are noOps, delete them
   
   if (pConversionMachineTo_p->isNOP() && pConversionMachineFrom_p->isNOP()) {
      delete pConversionMachineTo_p;
      delete pConversionMachineFrom_p;
      pConversionMachineTo_p = 0;
      pConversionMachineFrom_p = 0;
   }

}

void SpectralCoordinate::convertTo (Vector<Double>& world) const
{

// SC always has world vector of length 1

   if (pConversionMachineTo_p) {
      world(0)  = (*pConversionMachineTo_p)(world(0)).get(unit_p).getValue();
   }
}

void SpectralCoordinate::convertFrom (Vector<Double>& world) const
{

// SC always has world vector of length 1

   if (pConversionMachineFrom_p) {
      world(0) = (*pConversionMachineFrom_p)(world(0)).get(unit_p).getValue();
   }
}

