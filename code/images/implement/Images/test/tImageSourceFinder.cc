//# tImageSourceFinder.cc: test ImageSourceFinder class
//# Copyright (C) 1996,1997,1998,1999,2000
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
// 
//
#include <aips/aips.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Arrays/Vector.h>
#include <aips/Mathematics/Constants.h>
#include <aips/Mathematics/Math.h>
#include <aips/Exceptions/Error.h>
#include <aips/Logging.h>
#include <aips/Quanta/Quantum.h>
#include <aips/Quanta/Unit.h>
#include <aips/Utilities/String.h>
#include <trial/Images/TempImage.h>
#include <trial/Images/PagedImage.h>
#include <trial/Images/ComponentImager.h>
#include <trial/Images/ImageSourceFinder.h>
#include <trial/Coordinates/CoordinateSystem.h>
#include <trial/Coordinates/DirectionCoordinate.h>
#include <trial/ComponentModels/GaussianShape.h>
#include <trial/ComponentModels/ConstantSpectrum.h>
#include <trial/ComponentModels/Flux.h>
#include <trial/ComponentModels/SkyComponent.h>
#include <trial/ComponentModels/ComponentList.h>

#include <iostream.h>

SkyComponent makeSkyComponent (const Vector<Double>& pixel, 
                               const Flux<Double>& flux,
                               const Quantum<Double>& maj,
                               const Quantum<Double>& min,
                               const Quantum<Double>& pa,
                               const DirectionCoordinate& cSys);

Bool compareSkyComponent (const SkyComponent& in,
                          const SkyComponent& out);

void test1 (LogIO& os, ImageInterface<Float>& im, const DirectionCoordinate& dC);


main (int argc, char **argv)
{
try {


   LogOrigin or("tImageSourceFind", "main()", WHERE);
   LogIO os(or);
 
// Make Coordinate SYstem

   IPosition shape(2,100,100);
   Projection proj(Projection::SIN);
   Double refLong = 0.1;
   Double refLat = C::pi / 4.0;
   Double incLong = -C::pi / (180.0 * 3600.0);
   Double incLat = C::pi / (180.0 * 3600.0);
   Matrix<Double> xform(2,2);
   xform.set(0.);
   xform.diagonal() = 1.0;
   Double refX = shape(0) / 2.0;
   Double refY = shape(1) / 2.0;
   DirectionCoordinate dC(MDirection::J2000, proj, refLong, refLat, incLong, incLat, xform,
                          refX, refY);
   CoordinateSystem cSys;
   cSys.addCoordinate(dC);

// Make image

   TempImage<Float> im(shape, cSys);
   im.set(0.0);
//
   test1(os, im, dC);
//
}  catch (AipsError x) {
     cerr << "aipserror: error " << x.getMesg() << endl;
     exit(1);
} 

exit(0);
}


SkyComponent makeSkyComponent (const Vector<Double>& pixel, 
                               const Flux<Double>& flux,
                               const Quantum<Double>& maj,
                               const Quantum<Double>& min,
                               const Quantum<Double>& pa,
                               const DirectionCoordinate& dC)
{
    MDirection world;
    dC.toWorld(world, pixel);
    GaussianShape gs(world, maj, min, pa);
    ConstantSpectrum sp;
    SkyComponent sky(flux, gs, sp);
    return sky;
}

Bool compareSkyComponent (const SkyComponent& in,
                          const SkyComponent& out)
{
   const MDirection dIn =in.shape().refDirection();
   const MDirection dOut = out.shape().refDirection();
//
   Flux<Double> fIn0 = in.flux().copy();
   Quantum<Double> fIn = fIn0.value(Stokes::I, True);
//
   Flux<Double> fOut0 = out.flux().copy();
   Quantum<Double> fOut = fOut0.value(Stokes::I, True);
/*
   cerr << "In  = " << fIn << ", " << dIn.getAngle() << endl;
   cerr << "Out =" << fOut << ", " << dOut.getAngle() << endl << endl;
*/
//
   Vector<Double> vIn = dIn.getAngle().getValue();
   Vector<Double> vOut = dOut.getAngle().getValue();
   Bool ok = near(vIn(0),vOut(0)) &&
             near(vIn(1),vOut(1)) &&
             dIn.type()==dOut.type() &&
             out.shape().type()==ComponentType::POINT;
   return ok;
}


void test1 (LogIO& os, ImageInterface<Float>& im, const DirectionCoordinate& dC)
{

// Make component list from three sources

   ComponentList listIn;
   Quantum<Double> maj(10.0, Unit("arcsec"));
   Quantum<Double> min(5.0, Unit("arcsec"));
   Quantum<Double> pa(45.0, Unit("deg"));
   Vector<Double> pixel(2);
//
   pixel(0) = 20.0;
   pixel(1) = 20.0;
   Flux<Double> flux1(100.0);
   SkyComponent sky1 = makeSkyComponent (pixel, flux1, maj, min, pa, dC);
   listIn.add(sky1);
//
   pixel(0) = 60.0;
   pixel(1) = 60.0;
   Flux<Double> flux2(50.0);
   SkyComponent sky2 = makeSkyComponent (pixel, flux2, maj, min, pa, dC);
   listIn.add(sky2);
//
   pixel(0) = 30.0;
   pixel(1) = 60.0;
   Flux<Double> flux3(10.0);
   SkyComponent sky3 = makeSkyComponent (pixel, flux3, maj, min, pa, dC);
   listIn.add(sky3);
//
   im.setUnits(Unit("Jy/pixel"));
   ComponentImager::project(im, listIn);
//
   ImageSourceFinder<Float> sf(im);
   ComponentList listOut = sf.findPointSources(os, 5, 0.05, False);
//
   if (listOut.nelements()!=3) {
      os << "Found wrong number of sources" << LogIO::EXCEPTION;
   }

// Finder  will find in order of decreasing flux

   for (uInt i=0; i<3; i++) {
      if (!compareSkyComponent(listIn.component(i), listOut.component(i))) {
         os << "Component " << i << " is incorrect" << endl;
      }
   }
}

