//# DirectionCoordinate.cc: this defines the DirectionCoordinate class
//# Copyright (C) 1997,1998,1999,2000,2001,2002,2003,2004
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

#include <wcslib/wcs.h>
#include <wcslib/cel.h>
#include <wcslib/prj.h>
#include <wcslib/cylfix.h>

#include <trial/Coordinates/DirectionCoordinate.h>
#include <trial/Coordinates/LinearCoordinate.h>
#include <trial/Coordinates/Coordinate.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Arrays/ArrayAccessor.h>
#include <aips/Containers/Record.h>
#include <aips/Logging/LogIO.h>
#include <aips/Logging/LogOrigin.h>
#include <aips/Mathematics/Constants.h>
#include <aips/Measures/MeasConvert.h>
#include <aips/Quanta/MVAngle.h>
#include <aips/Quanta/Quantum.h>
#include <aips/Quanta/Unit.h>
#include <aips/Quanta/RotMatrix.h>
#include <aips/Quanta/Euler.h>
#include <aips/Utilities/Assert.h>
#include <aips/Utilities/LinearSearch.h>
#include <aips/Utilities/String.h>

#include <aips/iomanip.h>  
#include <aips/sstream.h>


DirectionCoordinate::DirectionCoordinate()
: Coordinate(),
  type_p(MDirection::J2000), 
  conversionType_p(type_p), 
  projection_p(Projection(Projection::CAR)),
  names_p(2), 
  units_p(2), 
  worldMin_p(0),
  worldMax_p(0),
  pConversionMachineTo_p(0),
  pConversionMachineFrom_p(0)
{
    Matrix<Double> xform(2,2); 
    xform = 0.0;
    xform.diagonal() = 1.0;
    makeDirectionCoordinate (0.0, 0.0, 1.0, 1.0,
                             xform, 0.0, 0.0, 999.0, 999.0);
    setDefaultWorldMixRanges();
    setRotationMatrix();
}

DirectionCoordinate::DirectionCoordinate(MDirection::Types directionType,
                                         const Projection &projection,
                                         Double refLong, Double refLat,
                                         Double incLong, Double incLat,
                                         const Matrix<Double> &xform,
                                         Double refX, Double refY,
                                         Double longPole, Double latPole)
: Coordinate(),
  type_p(directionType), 
  conversionType_p(type_p), 
  projection_p(projection),
  names_p(axisNames(directionType).copy()),
  units_p(2),
  worldMin_p(0),
  worldMax_p(0),
  pConversionMachineTo_p(0),
  pConversionMachineFrom_p(0)
{
    makeDirectionCoordinate (refLong, refLat, incLong, incLat,
                             xform, refX, refY, longPole, latPole);
    setDefaultWorldMixRanges();
    setRotationMatrix();
}

DirectionCoordinate::DirectionCoordinate(MDirection::Types directionType,
                                         const Projection &projection,  
                                         const Quantum<Double>& refLong,
                                         const Quantum<Double>& refLat,  
                                         const Quantum<Double>& incLong,
                                         const Quantum<Double>& incLat,
                                         const Matrix<Double> &xform,
                                         Double refX, Double refY,
                                         const Quantum<Double>& longPole,
                                         const Quantum<Double>& latPole)
: Coordinate(),
  type_p(directionType), 
  conversionType_p(type_p), 
  projection_p(projection),
  names_p(axisNames(directionType).copy()),
  units_p(2),
  worldMin_p(0),
  worldMax_p(0),
  pConversionMachineTo_p(0),
  pConversionMachineFrom_p(0)
{
   Unit rad("rad");
//
   if (!refLong.isConform(rad)) {
      throw(AipsError("Specified longitude is not angular"));
   }
   if (!refLat.isConform(rad)) {
      throw(AipsError("Specified latitude is not angular"));
   }
   if (!incLong.isConform(rad)) {
      throw(AipsError("Specified longitude increment is not angular"));
   }
   if (!incLat.isConform(rad)) {
      throw(AipsError("Specified latitude increment is not angular"));
   }
//
   Double lon = refLong.getValue(rad);
   Double lat = refLat.getValue(rad);
   Double dlon = incLong.getValue(rad);
   Double dlat = incLat.getValue(rad);
//
   Double dLongPole = longPole.getValue();
   Double dLatPole = latPole.getValue();
//
// Any value >=999 is taken as the default, regardless of units
//
   if (dLongPole < 999.0) {                 
      dLongPole = longPole.getValue(rad);
   } else {
      dLongPole = 999.0;
   }
   if (dLatPole < 999.0) {
      dLatPole = latPole.getValue(rad);
   } else {
      dLatPole = 999.0;
   }
//
   makeDirectionCoordinate (lon, lat, dlon, dlat, xform, refX, refY,
                            dLongPole, dLatPole);
   setDefaultWorldMixRanges();
   setRotationMatrix();
}


DirectionCoordinate::DirectionCoordinate(const DirectionCoordinate &other)
: Coordinate(other),
  pConversionMachineTo_p(0),
  pConversionMachineFrom_p(0)
{
   operator=(other);
}


DirectionCoordinate &DirectionCoordinate::operator=(const DirectionCoordinate &other)
{
    if (this != &other) {

// Copy most private members

        Coordinate::operator=(other);
//
	type_p = other.type_p;
	conversionType_p = other.conversionType_p;
	projection_p = other.projection_p;
//
	names_p = other.names_p;
	units_p = other.units_p;
        worldMin_p = other.worldMin_p;
        worldMax_p = other.worldMax_p;
	to_degrees_p = other.to_degrees_p.copy();
	to_radians_p = other.to_radians_p.copy();
        rot_p = other.rot_p;

// Copy WCS structure

       int err = wcscopy (1, &(other.wcs_p), &wcs_p);
       if (err != 0) {
          String errmsg = "wcs wcscopy_error: ";
          errmsg += wcscopy_errmsg[err];
          throw(AipsError(errmsg));
       } 
       err = wcsset(&wcs_p);
       if (err != 0) { 
           String errmsg = "wcs wcsset_error: ";
           errmsg += wcsset_errmsg[err];
           throw(AipsError(errmsg));
       }

// Machines

        if (pConversionMachineTo_p) {
           delete pConversionMachineTo_p;
           pConversionMachineTo_p = 0;
        }
        if (pConversionMachineFrom_p) {
           delete pConversionMachineFrom_p;
           pConversionMachineFrom_p = 0;
        }
        makeConversionMachines();
    }
    return *this;
}

DirectionCoordinate::~DirectionCoordinate()
{
    wcsfree(&wcs_p);
//
    if (pConversionMachineTo_p) {
       delete pConversionMachineTo_p;
       pConversionMachineTo_p = 0;
    }
    if (pConversionMachineFrom_p) {
       delete pConversionMachineFrom_p;
       pConversionMachineFrom_p = 0;
    }
}

Coordinate::Type DirectionCoordinate::type() const
{
    return Coordinate::DIRECTION;
}

String DirectionCoordinate::showType() const
{
    return String("Direction");
}


uInt DirectionCoordinate::nPixelAxes() const
{
    return 2;
}

uInt DirectionCoordinate::nWorldAxes() const
{
    return 2;
}

void DirectionCoordinate::setReferenceConversion (MDirection::Types type)
{
// See if something to do

   if (conversionType_p==type) return;

// If conversion type the same as the native type, just 
// remove the machines

   conversionType_p = type;
   if (pConversionMachineTo_p) {
      delete pConversionMachineTo_p;      
      pConversionMachineTo_p = 0;
   }
   if (pConversionMachineFrom_p) {
      delete pConversionMachineFrom_p;
      pConversionMachineFrom_p = 0;
   }
   if (conversionType_p == type_p) return;

// Now make new machines as needed

   makeConversionMachines();
}

Bool DirectionCoordinate::toWorld(Vector<Double> &world,
 				  const Vector<Double> &pixel) const
{
    DebugAssert(pixel.nelements() == 2, AipsError);
    world.resize(2);
//   
    Bool delPixel, delWorld;
    const double* pixelStore = pixel.getStorage(delPixel);
    double* worldStore = world.getStorage(delWorld);
//
    double imgCrd[2];
    double phi;
    double theta;
    int stat;
    int iret = wcsp2s (&wcs_p, 1, 2, pixelStore, imgCrd, &phi, &theta, worldStore, &stat);
    pixel.freeStorage(pixelStore, delPixel);
    world.putStorage(worldStore, delWorld);
//
    if (iret!=0) {  
       String errorMsg = String("wcslib wcsp2s error: ") + wcsp2s_errmsg[iret];
       set_error(errorMsg);
       return False;
    }

// To correct angular units

    toOther(world);

// Convert to specified conversion reference type
  
    convertTo(world);  

    return True;
}
  


Bool DirectionCoordinate::toPixel(Vector<Double> &pixel,
				  const Vector<Double> &world) const
{
    static Vector<Double> world_tmp;
//
    DebugAssert(world.nelements() == nWorldAxes(), AipsError);
    pixel.resize(2);
       
// Convert from specified conversion reference type

    world_tmp.resize(nWorldAxes());
    world_tmp[0] = world[0];
    world_tmp[1] = world[1];
    convertFrom(world_tmp);
    
// To degrees for wcs

    double worldIn[2];
    worldIn[0] = world_tmp[0] * to_degrees_p[0];
    worldIn[1] = world_tmp[1] * to_degrees_p[1];
//
    Bool delPixel;
    double* pixelStore = pixel.getStorage(delPixel);
    double imgCrd[2];
    double phi;
    double theta;
    int stat;
    int iret = wcss2p (&wcs_p, 1, 2, worldIn, &phi, &theta, imgCrd, pixelStore, &stat);
    pixel.putStorage(pixelStore, delPixel);
    if (iret!=0) {
       String errorMsg = String("wcslib wcss2p error: ") + wcss2p_errmsg[iret];
       set_error(errorMsg);
       return False;
    }
//
    return True;
}



Bool DirectionCoordinate::toMix(Vector<Double>& worldOut,
                                Vector<Double>& pixelOut,
                                const Vector<Double>& worldIn,
                                const Vector<Double>& pixelIn,
                                const Vector<Bool>& worldAxes,
                                const Vector<Bool>& pixelAxes,
                                const Vector<Double>& worldMin,
                                const Vector<Double>& worldMax) const
{
   Bool useConversionType = False;
         
// Temporaries
       
   static Vector<Double> in_tmp;
   static Vector<Double> out_tmp;
//
   const uInt nWorld = worldAxes.nelements();
   const uInt nPixel = pixelAxes.nelements();
   DebugAssert(nWorld==nWorldAxes(), AipsError);
   DebugAssert(nPixel==nPixelAxes(), AipsError);
   DebugAssert(worldIn.nelements()==nWorld, AipsError);
   DebugAssert(pixelIn.nelements()==nPixel, AipsError);
   DebugAssert(worldMin.nelements()==nWorld, AipsError);
   DebugAssert(worldMax.nelements()==nWorld, AipsError);
//
   for (uInt i=0; i<nPixel; i++) {
      if (pixelAxes(i) && worldAxes(i)) {
         set_error("DirectionCoordinate::toMix - duplicate pixel/world axes");
         return False;
      }
      if (!pixelAxes(i) && !worldAxes(i)) {
         set_error("DirectionCoordinate::toMix - each coordinate must be either pixel or world");
         return False;
      }
   }
//
   worldOut.resize(nWorldAxes());
   pixelOut.resize(nPixelAxes());
//
   if (pixelAxes[0] && pixelAxes[1]) {
//
// pixel->world
//
      if (!toWorld(worldOut, pixelIn)) return False;
      pixelOut[0] = pixelIn[0];
      pixelOut[1] = pixelIn[1];
   } else if (worldAxes[0] && worldAxes[1]) {
//
// world->pixel
//
      if (!toPixel(pixelOut, worldIn)) return False;
      worldOut[0] = worldIn[0];
      worldOut[1] = worldIn[1];
   } else if (pixelAxes[0] && worldAxes[1]) {
//
// pixel,world->world,pixel
//
      in_tmp.resize(2);
      out_tmp.resize(2);
//
      in_tmp[0] = pixelIn[0];
      in_tmp[1] = worldIn[1];
      if (!toMix2(out_tmp, in_tmp, worldMin,
                  worldMax, False)) {
         return False;
      }
//
      pixelOut[0] = in_tmp[0];
      pixelOut[1] = out_tmp[1];
      worldOut[0] = out_tmp[0];
      worldOut[1] = in_tmp[1];
//
      if (useConversionType) { 
         convertTo(worldOut);  
      }
   } else if (worldAxes[0] && pixelAxes[1]) {
// 
// world,pixel->pixel,world
//
      in_tmp.resize(2); 
      out_tmp.resize(2);
//
      in_tmp[0] = worldIn[0];
      in_tmp[1] = pixelIn[1];
      if (!toMix2(out_tmp, in_tmp, worldMin,
                  worldMax, True)) {
         return False;
      }
//
      pixelOut[0] = out_tmp[0];
      pixelOut[1] = in_tmp[1]; 
      worldOut[0] = in_tmp[0];
      worldOut[1] = out_tmp[1];
//
      if (useConversionType) { 
         convertTo(worldOut);
      }
   }
   return True;
}

 

Bool DirectionCoordinate::toWorldMany (Matrix<Double>& world,
                                       const Matrix<Double>& pixel,
                                       Vector<Bool>& failures) const
{ 
    uInt nCoord = pixel.ncolumn();
    uInt nPixel = nPixelAxes();
    AlwaysAssert(pixel.nrow()==nPixel, AipsError);
    world.resize(pixel.shape());
    failures.resize(nCoord);
//
    Bool deleteWorld, deletePixel;
    Double* pWorld = world.getStorage(deleteWorld);
    const Double* pPixel = pixel.getStorage(deletePixel);
// 
    Bool deleteImgCrd, deletePhi, deleteTheta, deleteStat;
    Matrix<Double> imgCrd(nPixel,nCoord);
    Vector<Double> phi(nCoord);
    Vector<Double> theta(nCoord);
    Vector<Int> stat(nCoord);

// Convert with wcs to degrees

    Double* pImgCrd = imgCrd.getStorage(deleteImgCrd);    
    Double* pPhi = phi.getStorage(deletePhi);    
    Double* pTheta = theta.getStorage(deleteTheta);    
    Int* pStat = stat.getStorage(deleteStat);    
//
    int nC = nCoord;
    int iret = wcsp2s (&wcs_p, nC, 2, pPixel, pImgCrd, pPhi, pTheta, pWorld, pStat);
    for (uInt i=0; i<nCoord; i++) {
       failures[i] = (stat[i]!=0);
    }
//
    pixel.freeStorage(pPixel, deletePixel);
    world.putStorage(pWorld, deleteWorld);
    imgCrd.putStorage(pImgCrd, deleteImgCrd);
    phi.putStorage(pPhi, deletePhi);
    theta.putStorage(pTheta, deleteTheta);
    stat.putStorage(pStat, deleteStat);
//
    if (iret!=0) {
        String errorMsg= "wcs wcsp2s_error: ";
        errorMsg += wcsp2s_errmsg[iret];
        set_error(errorMsg);
        return False;
    }

// Convert to correct angular units from degrees

    Vector<Double> r0(world.row(0));
    r0 /= to_degrees_p[0];
    Vector<Double> r1(world.row(1));
    r1 /= to_degrees_p[1];

// Convert to specified conversion reference type
  
    convertToMany(world);  
//
    return True;

}     


Bool DirectionCoordinate::toPixelMany (Matrix<Double>& pixel,
                                       const Matrix<Double>& world,
                                       Vector<Bool>& failures) const
{
    uInt nCoord = world.ncolumn();
    uInt nWorld = nWorldAxes();
    AlwaysAssert(world.nrow()==nWorld, AipsError);
    pixel.resize(world.shape());
    failures.resize(nCoord);

// Copy input as we have to convert it to all sorts of things

    Matrix<Double> world2(world.copy());    

// Convert from specified conversion reference type

    convertFromMany (world2);

// Convert to degrees for wcs

    Vector<Double> r0(world2.row(0));
    r0 *= to_degrees_p[0];
    Vector<Double> r1(world2.row(1));
    r1 *= to_degrees_p[1];

// Convert with wcs

    Bool deleteWorld, deletePixel;
    Double* pPixel = pixel.getStorage(deletePixel);
    const Double* pWorld = world2.getStorage(deleteWorld);
//
    Bool deleteImgCrd, deletePhi, deleteTheta, deleteStat;
    Matrix<Double> imgCrd(nWorld,nCoord);
    Vector<Double> phi(nCoord);
    Vector<Double> theta(nCoord);
    Vector<Int> stat(nCoord);
//
    Double* pImgCrd = imgCrd.getStorage(deleteImgCrd);    
    Double* pPhi = phi.getStorage(deletePhi);    
    Double* pTheta = theta.getStorage(deleteTheta);    
    Int* pStat = stat.getStorage(deleteStat);    
//
    const int nC = nCoord;
    int iret = wcss2p (&wcs_p, nC, 2, pWorld, pPhi, pTheta, pImgCrd, pPixel, pStat);
    for (uInt i=0; i<nCoord; i++) {
       failures[i] = (stat[i]!=0);
    }
//  
    world2.freeStorage(pWorld, deleteWorld);
    pixel.putStorage(pPixel, deletePixel);
    imgCrd.putStorage(pImgCrd, deleteImgCrd);
    phi.putStorage(pPhi, deletePhi);
    theta.putStorage(pTheta, deleteTheta);
    stat.putStorage(pStat, deleteStat);
//
    if (iret!=0) {
        String errorMsg= "wcs wcss2p_error: ";
        errorMsg += wcss2p_errmsg[iret];
        set_error(errorMsg);
        return False;
    }
    return True;
}
  
 


MDirection::Types DirectionCoordinate::directionType() const
{
    return type_p;
}

Projection DirectionCoordinate::projection() const
{
    return projection_p;
}

Vector<String> DirectionCoordinate::worldAxisNames() const
{
    return names_p;
}

Vector<String> DirectionCoordinate::worldAxisUnits() const
{
    return units_p;
}

Vector<Double> DirectionCoordinate::referenceValue() const
{
    Vector<Double> crval(2);
    crval[0] = wcs_p.crval[0];
    crval[1] = wcs_p.crval[1];                    // degrees
    toOther(crval);
    return crval;
}

Vector<Double> DirectionCoordinate::increment() const
{
    Vector<Double> cdelt(2);
    cdelt[0] = wcs_p.cdelt[0];
    cdelt[1] = wcs_p.cdelt[1];            // degrees
    toOther(cdelt);
    return cdelt;
}

Matrix<Double> DirectionCoordinate::linearTransform() const
{
   Matrix<Double> tmp;
   pcToXform (tmp, wcs_p);
   return tmp;
}


Vector<Double> DirectionCoordinate::referencePixel() const
{
    Vector<Double> crpix(2);
    crpix[0] = wcs_p.crpix[0];
    crpix[1] = wcs_p.crpix[1]; 
    return crpix;
}

Bool DirectionCoordinate::setWorldAxisNames(const Vector<String> &names)
{
    if (!(names.nelements()==nWorldAxes())) {
       set_error("names vector must be of length 2");
       return False;
    }
//
    names_p = names;
    return True;
}

Bool DirectionCoordinate::setWorldAxisUnits(const Vector<String> &units)
{
    if (!(units.nelements()==nWorldAxes())) {
       set_error("units vector must be of length 2");
       return False;
    }
//
    String error;
    Vector<Double> factor;
    Bool ok = find_scale_factor(error, factor, units, worldAxisUnits());
    if (ok) {
      to_degrees_p[0] /= factor[0];
      to_degrees_p[1] /= factor[1];
//
      to_radians_p[0] = to_degrees_p[0] * C::degree;
      to_radians_p[1] = to_degrees_p[1] * C::degree;
    } else {
      set_error(error);
    }
    if (ok) {
       units_p = units;
       worldMin_p *= factor;
       worldMax_p *= factor;
    }
    return ok;
}


Bool DirectionCoordinate::setReferencePixel(const Vector<Double> &refPix)
{
    if (!(refPix.nelements()==nPixelAxes())) {
       set_error("reference pixels vector must be of length 2");
       return False;
    }
//
    wcs_p.crpix[0] = refPix[0];
    wcs_p.crpix[1] = refPix[1];
//   
    int errnum = wcsset(&wcs_p);
    if (errnum != 0) {
        String errmsg = "wcs wcsset_error: ";
        errmsg += wcsset_errmsg[errnum];
        throw(AipsError(errmsg));
    }
//
    return True;
}

Bool DirectionCoordinate::setLinearTransform(const Matrix<Double> &xform)
{
    Bool ok = (xform.nrow() == nWorldAxes() && 
               xform.ncolumn() == nWorldAxes());
    if (!ok) {
       set_error("linear transform matrix has wrong shape");
       return False;
    }

// Set PC cards
  
    xFormToPC (wcs_p, xform);
//
    int errnum = wcsset(&wcs_p);
    if (errnum != 0) {
        String errmsg = "wcs wcsset_error: ";
        errmsg += wcsset_errmsg[errnum];
        throw(AipsError(errmsg));
    }
//      
    return True;
}

Bool DirectionCoordinate::setIncrement(const Vector<Double> &inc)
{
    Bool ok = (inc.nelements()==nWorldAxes());
    if (!ok) {
       set_error("Two increments must be provided!");
       return False;
    }
//
    Vector<Double> tmp(inc.copy());
    toDegrees(tmp);
    wcs_p.cdelt[0] = tmp[0];  
    wcs_p.cdelt[1] = tmp[1];
//
    int errnum = wcsset(&wcs_p); 
    if (errnum != 0) {
        String errmsg = "wcs wcsset_error: ";
        errmsg += wcsset_errmsg[errnum];
        throw(AipsError(errmsg));
    }
//
    return ok;
}

Bool DirectionCoordinate::setReferenceValue(const Vector<Double> &refval)
{
    Bool ok = (refval.nelements()==nWorldAxes());
    if (!ok) {
       set_error("Two ref. values must be provided!");
       return False;
    }
//
    Vector<Double> tmp(refval.copy());
    toDegrees(tmp);
//
    wcs_p.crval[0] = tmp[0];
    wcs_p.crval[1] = tmp[1];
//
    int errnum = wcsset(&wcs_p);
    if (errnum != 0) {
        String errmsg = "wcs wcsset_error: ";
        errmsg += wcsset_errmsg[errnum];
        throw(AipsError(errmsg));
    }
            
// Update offset coordinate rotation matrix
            
    setRotationMatrix();
//
    return ok;
}

Vector<String> DirectionCoordinate::axisNames(MDirection::Types type, 
					      Bool FITSName)
{
    Vector<String> names(2);
    if (FITSName) {
	switch(type) {
	case MDirection::J2000:
	case MDirection::JMEAN:
	case MDirection::APP:
	case MDirection::B1950:
	case MDirection::BMEAN:
	case MDirection::BTRUE:
	    names[0] = "RA";
	    names[1] = "DEC";
	    break;
	case MDirection::GALACTIC:
	    names[0] = "GLON";
	    names[1] = "GLAT";
	    break;
        case MDirection::SUPERGAL:
            names[0] = "SLON";
            names[1] = "SLAT";
            break;
        case MDirection::ECLIPTIC:
        case MDirection::MECLIPTIC:
        case MDirection::TECLIPTIC:
            names[0] = "ELON"; 
            names[1] = "ELAT"; 
            break;
	case MDirection::HADEC:
	    names[0] = "RA";           // HA not recognized by WCS
	    names[1] = "DEC";
	    break;
/*
// WCS does not recognize AZEL

	case MDirection::AZEL:
        case MDirection::AZELSW:   
	    names[0] = "AZ";
	    names[1] = "EL";
	    break;
*/
	default:
            names[0] = "??LN"; 
            names[1] = "??LT";
	}
    } else {
	switch(type) {
	case MDirection::J2000: // These are all RA/DEC systems
	case MDirection::JMEAN:
	case MDirection::APP:
	case MDirection::B1950:
	case MDirection::BMEAN:
	case MDirection::BTRUE:
	    names[0] = "Right Ascension";
	    names[1] = "Declination";
	    break;
	case MDirection::GALACTIC:
        case MDirection::SUPERGAL:
        case MDirection::ECLIPTIC:
        case MDirection::MECLIPTIC:
        case MDirection::TECLIPTIC:
 	    names[0] = "Longitude";
	    names[1] = "Latitude";
	    break;
	case MDirection::HADEC:
	    names[0] = "Hour Angle";
	    names[1] = "Declination";
	    break;
/*
// WCS does not recognize AZEL

	case MDirection::AZEL:
        case MDirection::AZELSW:
	    names[0] = "Azimuth";
	    names[1] = "Elevation";
	    break;
*/
	default:
            names[0] = "Longitude";
            names[1] = "Latitude";  
	}
    }
    return names;
}


void DirectionCoordinate::checkFormat(Coordinate::formatType& format,
                                      Bool absolute) const
{   
   MDirection::GlobalTypes gtype = MDirection::globalType(type_p);

   if (format == Coordinate::DEFAULT) {
      if (gtype == MDirection::GRADEC || gtype == MDirection::GHADEC) {
         if (absolute) {
            format = Coordinate::TIME;
         } else {
            format = Coordinate::SCIENTIFIC;
         }
      } else if (gtype == MDirection::GLONGLAT) {
         format = Coordinate::FIXED;
      } else if (gtype == MDirection::GAZEL) {
         format = Coordinate::FIXED;
      } else {
         format = Coordinate::SCIENTIFIC;
      }
   }
}



void DirectionCoordinate::getPrecision (Int& precision,
                                        Coordinate::formatType& format,
                                        Bool absolute,
                                        Int defPrecScientific,
                                        Int defPrecFixed,
                                        Int defPrecTime) const
{
// Fill in DEFAULT

   checkFormat(format, absolute);

// Set precisions depending upon requested format type and
// desire to see absolute or offset coordinate

   if (format == Coordinate::SCIENTIFIC) {
      if (defPrecScientific >= 0) {
         precision = defPrecScientific;
      } else {
         precision = 6;
      }      
   } else if (format == Coordinate::FIXED) {
      if (defPrecFixed >= 0) {
         precision = defPrecFixed;
      } else {
         precision = 6;
      }
   } else if (format == Coordinate::TIME) {
      if (defPrecTime >= 0) {
         precision = defPrecTime;
      } else {
         precision = 3;
      }
   }
}


String DirectionCoordinate::format(String& units,
                                   Coordinate::formatType format,
                                   Double worldValue,
                                   uInt worldAxis,
                                   Bool isAbsolute,
                                   Bool showAsAbsolute,
                                   Int precision)
{
   DebugAssert(worldAxis< nWorldAxes(), AipsError);
   DebugAssert(nWorldAxes()==2, AipsError);

// Convert given world value to absolute or relative as needed
   
   static Vector<Double> world;
   world.resize(nWorldAxes());
//
   if (showAsAbsolute) {
      if (!isAbsolute) {
         world = 0.0;
         world(worldAxis) = worldValue;
         makeWorldAbsolute(world);
         worldValue = world(worldAxis);
      }
   } else {
      if (isAbsolute) {
         world = referenceValue();
         world(worldAxis) = worldValue;
         makeWorldRelative(world);
         worldValue = world(worldAxis);
      }
   } 
     
// Fill in DEFAULT format

   Coordinate::formatType form = format;

// Check format.  If showAsAbsolute==F, TIME formatting is switched off

   checkFormat(form, showAsAbsolute);

// Set default precision if needed

   Int prec = precision;
   if (prec < 0) getPrecision(prec, form, showAsAbsolute, -1, -1, -1);

// Set global DirectionCoordinate type

   MDirection::GlobalTypes gtype = MDirection::globalType(type_p);

// Check units

   static const Unit unitRAD("rad");
   String nativeUnit = worldAxisUnits()(worldAxis);
   if (units.empty()) {
//            leave empty telling formatLong/Lat to make up something nice
   } else {
      Unit unitCurrent(units);
      if (unitCurrent != unitRAD) {
         throw (AipsError("Specified unit is invalid"));
      }
   }
   
// Convert value to radians first so we can make an MVAngle from it

   Double worldValue2;
   worldValue2 = C::degree * worldValue * to_degrees_p[worldAxis];
   MVAngle mVA(worldValue2);

// We need to put the longitudes into the correct convention
// All latitudes better be -90 -> 90 and we don't need to fiddle them
// If the units are empty, the formatters will make something up

   String string;
   if (worldAxis==0) {
      string = formatLongitude (units, mVA, gtype, showAsAbsolute, form, prec);
   } else {
      string = formatLatitude (units, mVA, showAsAbsolute, form, prec);
   }

// Only return actual units if not TIME formatting

   if (form==Coordinate::TIME) units = String("");
//
   return string;
}


String DirectionCoordinate::formatLongitude (String& units, MVAngle& mVA,
                                             MDirection::GlobalTypes gtype,
                                             Bool absolute, 
                                             Coordinate::formatType form,
                                             Int prec) const
{
   ostringstream oss;         
   MVAngle mVA2(mVA);

// Time formatting we can do straight away

   if (form==Coordinate::TIME) {

// Format with mVA2. TIME formatting really only makes
// sense for RA and HA, but we do something with the others
// TIME formatting is only possible for absolute=T

      prec += 6;
      if (gtype == MDirection::GRADEC) {
         oss << mVA2.string(MVAngle::TIME,prec);
      } else if (gtype==MDirection::GHADEC) {
         oss << mVA2.string(MVAngle::TIME+MVAngle::DIG2,prec);
      } else if (gtype==MDirection::GAZEL) {
         oss << mVA2.string(MVAngle::ANGLE,prec);
      } else if (gtype==MDirection::GLONGLAT) {
         oss << mVA2.string(MVAngle::ANGLE,prec);
      }
//
      // oss << ends;
      return String(oss);
   }

// Continue on with other format types

   Double value = mVA2.get().getValue();     // Radians
   Bool emptyUnits = units.empty();
//
   if (gtype==MDirection::GRADEC){ 
      if (absolute) mVA2 = mVA(0.0);                   // 0->2pi (0->360)
      if (emptyUnits) {
         value = mVA2.get(Unit("deg")).getValue();
         units = "deg";
      } else {
         value = mVA2.get(Unit(units)).getValue();
      }
   } else if (gtype==MDirection::GHADEC) {
      if (absolute) mVA2 = mVA();                      // -pi->pi (-180->180)
      if (emptyUnits) {
         value = mVA2.get().getValue() * 24.0 / C::_2pi;
         units = "h";
      } else {
         value = mVA2.get(Unit(units)).getValue();
      }
   } else if (gtype==MDirection::GAZEL) {
      if (absolute) mVA2 = mVA(0.0);                   // 0->2pi (0->360)
      if (emptyUnits) {
         value = mVA2.get(Unit("deg")).getValue();
         units = "deg";
      } else {
         value = mVA2.get(Unit(units)).getValue();
      }
   } else if (gtype==MDirection::GLONGLAT) {
      if (type_p==MDirection::ECLIPTIC ||
          type_p==MDirection::MECLIPTIC ||
          type_p==MDirection::TECLIPTIC) {
         if (absolute) mVA2 = mVA();                  // -pi->pi (-180->180)
      } else {
         if (absolute) mVA2 = mVA(0.0);               // 0->2pi (0->360)
      }
      if (emptyUnits) {
         value = mVA2.get(Unit("deg")).getValue();
         units = "deg";
      } else {
         value = mVA2.get(Unit(units)).getValue();
      }
   } else {
      if (absolute) mVA2 = mVA(0.0);                  // 0->2pi (0->360)
      value = mVA2.get(Unit(units)).getValue();
   }

// For relative coordinates, use arcsec if user didnt specify

   if (!absolute) {
      if (emptyUnits&& units=="deg") {
         value *= 3600.0;
         units = "arcsec";
      }
   }
//
   if (form==Coordinate::SCIENTIFIC) {
      oss.setf(ios::scientific, ios::floatfield);
      oss.precision(prec);
      oss << value;
   } else if (form==Coordinate::FIXED) {
      oss.setf(ios::fixed, ios::floatfield);
      oss.precision(prec);
      oss << value;
   }
//
   // oss << ends;
   return String(oss);
}

String DirectionCoordinate::formatLatitude (String& units, MVAngle& mVA,
                                            Bool absolute, 
                                            Coordinate::formatType form,
                                            Int prec) const
{
   ostringstream oss;         
   MVAngle mVA2(mVA);

// TIME formatting we can do straight away

   if (form==Coordinate::TIME) {
      prec += 6;
      oss << mVA2.string(MVAngle::ANGLE+MVAngle::DIG2,prec);
   }

// Continue on with other format types

   Double value = mVA2.get().getValue();     // Radians
   Bool emptyUnits = units.empty();
//
   if (!emptyUnits) {
      value = mVA2.get(Unit(units)).getValue();
   } else {
      value = mVA2.get(Unit("deg")).getValue();
      units = "deg";
   }
//
   if (!absolute) {
      if (emptyUnits && units=="deg") {
         value *= 3600.0;
         units = "arcsec";
      }
   }
//
   if (form==Coordinate::SCIENTIFIC) {
      oss.setf(ios::scientific, ios::floatfield);
      oss.precision(prec);
      oss << value;
   } else if (form==Coordinate::FIXED) {
      oss.setf(ios::fixed, ios::floatfield);
      oss.precision(prec);
      oss << value;
   }
   // oss << ends;
   return String(oss);
}




Coordinate *DirectionCoordinate::clone() const
{
    return new DirectionCoordinate(*this);
}

void DirectionCoordinate::toDegrees(Vector<Double> &other) const
{
    other[0] *= to_degrees_p[0];
    other[1] *= to_degrees_p[1];
}


Bool DirectionCoordinate::near(const Coordinate& other, 
                               Double tol) const
{
   Vector<Int> excludeAxes;
   return near(other, excludeAxes, tol);
}



Bool DirectionCoordinate::near(const Coordinate& other, 
                               const Vector<Int>& excludeAxes,
                               Double tol) const

{
   if (this->type() != other.type()) {
      set_error("Comparison is not with another DirectionCoordinate");
      return False;
   }
//     
   const DirectionCoordinate& dCoord = dynamic_cast<const DirectionCoordinate&>(other);
   
// Projection

   if (!projection_p.near(dCoord.projection_p, tol)) {
      set_error("The DirectionCoordinates have differing projections");
      return False;
   }

// Type

   if (type_p != dCoord.type_p) {
      set_error("The DirectionCoordinates have differing types");
      return False;
   }

// Conversion type

   if (conversionType_p != dCoord.conversionType_p) {

// Should this be an error or a warning ?

      set_error("The DirectionCoordinates have differing conversion types");
      return False;
   }


// Number of pixel and world axes is the same for a DirectionCoordinate
 
   AlwaysAssert(nPixelAxes()==nWorldAxes(), AipsError);
   Vector<Bool> exclude(nPixelAxes());   
   exclude = False;
   Bool found;
   const uInt nExcl = excludeAxes.nelements();
   if (nExcl > 0) {
      for (uInt i=0; i<nPixelAxes(); i++) {
         if (linearSearch(found, excludeAxes, Int(i), nExcl) >= 0) exclude[i] = True;
      }
   }

// Check names 

   ostringstream oss;
   if (names_p.nelements() != dCoord.names_p.nelements()) {
      set_error("The DirectionCoordinates have differing numbers of world axis names");
      return False;
   }
   for (uInt i=0; i<names_p.nelements(); i++) {
      if (!exclude[i]) {
         if (names_p[i] != dCoord.names_p[i]) {
            oss << "The DirectionCoordinates have differing axis names for axis "
                << i;
            set_error(String(oss));
            return False;      
         }
      }
   }

// Check units

   if (units_p.nelements() != dCoord.units_p.nelements()) {
      set_error("The DirectionCoordinates have differing numbers of axis units");
      return False;
   }
   for (uInt i=0; i<units_p.nelements(); i++) {
      if (!exclude[i]) {
         if (units_p[i] != dCoord.units_p[i]) {
            oss << "The DirectionCoordinates have differing axis units for axis "
                << i;
            set_error(String(oss));
            return False;      
         }
      }
   }

// {lon,lat}poles

    if (!::near(Double(wcs_p.lonpole), Double(dCoord.wcs_p.lonpole))) {
       oss << "The DirectionCoordinates have differing lonpoles";
       set_error(String(oss));
       return False;      
    }
    if (!::near(Double(wcs_p.latpole), Double(dCoord.wcs_p.latpole))) {
       oss << "The DirectionCoordinates have differing latpoles";
       set_error(String(oss));
       return False;      
    }


// Check reference  value

   {
      const Vector<Double>& thisVal = referenceValue();
      const Vector<Double>& thatVal = dCoord.referenceValue();
      if (thisVal.nelements() != thatVal.nelements()) {
         set_error("The DirectionCoordinates have differing reference values");
         return False;
      }
      for (uInt i=0; i<thisVal.nelements(); i++) {
         if (!exclude[i]) {
            if (!::near(thisVal[i],thatVal[i])) {
               oss << "The DirectionCoordinates have differing reference values for axis "
                   << i;
               set_error(String(oss));
               return False;      
            }
         }
      }
   }

// Check LinearXform components

   LinearXform thisVal(referencePixel(), increment(), linearTransform());
   LinearXform thatVal(dCoord.referencePixel(), dCoord.increment(), dCoord.linearTransform());
   if (!(thisVal.near(thatVal, excludeAxes))) {
      oss << "The DirectionCoordinates have differing LinearXform components";
      set_error(String(oss));
      return False;      
   }

   return True;
}




Bool DirectionCoordinate::save(RecordInterface &container,
                               const String &fieldName) const
{
    Bool ok = (!container.isDefined(fieldName));
    if (ok) {
	Record subrec;
	Projection proj = projection();
	String system = MDirection::showType(type_p);
//
	subrec.define("system", system);
	subrec.define("projection", proj.name());
	subrec.define("projection_parameters", proj.parameters());
	subrec.define("crval", referenceValue());
	subrec.define("crpix", referencePixel());
	subrec.define("cdelt", increment());
	subrec.define("pc", linearTransform());
	subrec.define("axes", worldAxisNames());
	subrec.define("units", worldAxisUnits());
//
	String convSystem = MDirection::showType(conversionType_p);
	subrec.define("conversionSystem", convSystem); 
//
        subrec.define("longpole", wcs_p.lonpole);    // Always degrees
        subrec.define("latpole", wcs_p.latpole);     // Always degrees
//
	container.defineRecord(fieldName, subrec);
    }
    return ok;
}

DirectionCoordinate* DirectionCoordinate::restore(const RecordInterface &container,
						  const String &fieldName)
{
    if (!container.isDefined(fieldName)) {
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
    MDirection::Types sys;
    Bool ok = MDirection::getType(sys, system);
    if (!ok) {
	return 0;
    }
    
    if (!subrec.isDefined("projection") || 
	!subrec.isDefined("projection_parameters")) {
	return 0;
    }
    String projname;
    subrec.get("projection", projname);
    Vector<Double> projparms;
    subrec.get("projection_parameters", projparms);
    Projection proj(Projection::type(projname), projparms);
//
    if (!subrec.isDefined("crval")) {
	return 0;
    }
    Vector<Double> crval;
    subrec.get("crval", crval);
//
    if (!subrec.isDefined("crpix")) {
	return 0;
    }
    Vector<Double> crpix;
    subrec.get("crpix", crpix);
//
    if (!subrec.isDefined("cdelt")) {
	return 0;
    }
    Vector<Double> cdelt;
    subrec.get("cdelt", cdelt);
//
    if (!subrec.isDefined("pc")) {
	return 0;
    }
    Matrix<Double> pc;
    subrec.get("pc", pc);
//
    Double longPole, latPole;
    longPole = latPole = 999.0;            // Optional
    if (subrec.isDefined("longpole")) {    
       subrec.get("longpole", longPole);   // Always degrees
       longPole *= C::degree;
    }
    if (subrec.isDefined("latpole")) {
       subrec.get("latpole", latPole);     // Always degrees
       latPole *= C::degree;
    }
//    
    if (!subrec.isDefined("axes")) {
	return 0;
    }
    Vector<String> axes;
    subrec.get("axes", axes);
//
    if (!subrec.isDefined("units")) {
	return 0;
    }
    Vector<String> units;
    subrec.get("units", units);

// Dummy DC which we overwrite.  Use refVal [0.0, 0.5] rather than [0.0, 0.0]
// to avoid divide by zero in Coordinate::make_Direction_FITS_ctype() function.
// See that function for further comments
// 

    Quantum<Double> refLong(crval[0], units[0]);
    Quantum<Double> refLat(crval[1], units[1]);
    Quantum<Double> incLong(cdelt[0], units[0]);
    Quantum<Double> incLat(cdelt[1], units[1]);
    DirectionCoordinate* retval = 
	new DirectionCoordinate(sys, proj, 
                                refLong.getValue(Unit("rad")), 
                                refLat.getValue(Unit("rad")), 
                                incLong.getValue(Unit("rad")), 
                                incLat.getValue(Unit("rad")), 
                                pc, crpix[0], crpix[1], 
                                longPole, latPole);
    AlwaysAssert(retval, AipsError);

// Set the actual units and names

    retval->setWorldAxisUnits(units);
    retval->setWorldAxisNames(axes);

// Set the conversion type if it exists

    if (subrec.isDefined("conversionSystem")) {
       String conversionSystem;
       subrec.get("conversionSystem", conversionSystem);
       MDirection::Types cSystem;
       Bool ok = MDirection::getType(cSystem, conversionSystem);
       if (ok) {
          retval->setReferenceConversion(cSystem);
       }
    }
//
    return retval;
}


void DirectionCoordinate::toOther(Vector<Double> &degrees) const
{
    degrees[0] /= to_degrees_p[0];
    degrees[1] /= to_degrees_p[1];
}

Bool DirectionCoordinate::toMix2(Vector<Double>& out,
                                 const Vector<Double>& in,
                                 const Vector<Double>& minWorld,
                                 const Vector<Double>& maxWorld,
                                 Bool longIsWorld) const
//
// vectors must be of length 2. no checking
//
{
//
// Temporaries  
//
    int mixpix;
    int mixcel;
    double mix_vspan[2];
    double mix_world[2];
    double mix_pixcrd[2];
    double mix_imgcrd[2];
    double mix_vstep;
    int mix_viter;
    double mix_phi, mix_theta;
    String errorMsg;
//
// Set input world/pixel arrays
//
    if (longIsWorld) {
       mixcel = 1;          // 1 or 2 (a code, not an index)
       mixpix = 1;          // index into pixcrd array
// 
       mix_world[wcs_p.lng] = in[0] * to_degrees_p[0];
       mix_pixcrd[mixpix] = in[1];
//
// Latitude span
//
       mix_vspan[0] = minWorld[1] * to_degrees_p[1];
       mix_vspan[1] = maxWorld[1] * to_degrees_p[1];
    } else {
       mixcel = 2;          // 1 or 2 (a code, not an index)
       mixpix = 0;          // index into pixcrd array
//
       mix_world[wcs_p.lat] = in[1] * to_degrees_p[1];
       mix_pixcrd[mixpix] = in[0];
//
// Longitude span
// 
       mix_vspan[0] = minWorld[0] * to_degrees_p[0];
       mix_vspan[1] = maxWorld[0] * to_degrees_p[0];
    }
//
// Do it
//
//    mix_vstep = 1.0;
    mix_vstep = 0.0;
    mix_viter = 5;
    int iret = wcsmix(&wcs_p, mixpix, mixcel, mix_vspan, mix_vstep, mix_viter,
                      mix_world, &mix_phi, &mix_theta, mix_imgcrd,
                      mix_pixcrd);
    if (iret!=0) {
        errorMsg= "wcs wcsmix_error: ";
        errorMsg += wcsmix_errmsg[iret];
        set_error(errorMsg);
        return False;
    }
//
// Fish out the results
//
    if (longIsWorld) {
       out[0] = mix_pixcrd[0];
       out[1] = mix_world[wcs_p.lat] / to_degrees_p[1];
    } else {
       out[0] = mix_world[wcs_p.lng] / to_degrees_p[0];
       out[1] = mix_pixcrd[1];
    }

// Phi and theta may be returned in a different interface somewhen
// Could assign these in toMix rather than pass them out
// but not very clear
  
//    phi = mix_phi / to_degrees_p[0];
//    theta = mix_theta / to_degrees_p[1];
//
    return True;
}
     



Coordinate* DirectionCoordinate::makeFourierCoordinate (const Vector<Bool>& axes,
                                                        const Vector<Int>& shape) const
//
// axes says which axes in the coordinate are to be transformed
// shape is the shape of the image for all axes in this coordinate
//
{
   AlwaysAssert(nPixelAxes()==2, AipsError);
   AlwaysAssert(nWorldAxes()==2, AipsError);
//
   if (axes.nelements() != 2) {
      throw (AipsError("Invalid number of specified axes"));
   }
   if (!axes[0] || !axes[1]) {
      throw (AipsError("You must specify both axes of the DirectionCoordinate to transform"));
   }
//
   if (shape.nelements() != 2) {
      throw (AipsError("Invalid number of elements in shape"));
   }

// Find names and units for Fourier coordinate and units to set 
// for this DirectionCoordinate 

   Vector<String> names(worldAxisNames());
   Vector<String> units(worldAxisUnits());
   Vector<String> unitsCanon(worldAxisUnits());
//
   Vector<String> namesOut(worldAxisNames().copy());
   Vector<String> unitsOut(worldAxisUnits().copy());
   fourierUnits (namesOut[0], unitsOut[0], unitsCanon[0], Coordinate::DIRECTION, 0,
                 units[0], names[0]);
   fourierUnits (namesOut[1], unitsOut[1], unitsCanon[1], Coordinate::DIRECTION, 1,
                 units[1], names[1]);

// Make a copy of ourselves and set the new units

   DirectionCoordinate dc = *this;
   if (!dc.setWorldAxisUnits(unitsCanon)) {
      throw(AipsError("Could not set world axis units"));
   }

// Create a LinearXform to do the inversion

   Vector<Double> cdelt(dc.increment().copy());
   toDegrees(cdelt);
   LinearXform linear(dc.referencePixel(), cdelt, dc.linearTransform());

// Now make the new output LinearCoordinate.  

   Vector<Double> crpix(2), scale(2), crval(2,0.0);
   crpix[0] = Int(shape[0] / 2);
   crpix[1] = Int(shape[1] / 2);
//
   scale[0] = dc.to_degrees_p[0] / Double(shape[0]);
   scale[1] = dc.to_degrees_p[1] / Double(shape[1]);
//
   const LinearXform linear2 = linear.fourierInvert(axes, crpix, scale);
//
   return new LinearCoordinate(namesOut, unitsOut, crval, linear2.cdelt(),
                               linear2.pc(), linear2.crpix());
}



void DirectionCoordinate::makeDirectionCoordinate(Double refLong, Double refLat,
                                                  Double incLong, Double incLat,
                                                  const Matrix<Double>& xform,
                                                  Double refX, Double refY,
                                                  Double longPole, Double latPole)
{
// Initially we are in radians
   
    to_degrees_p.resize(2);
    to_radians_p.resize(2);
// 
    to_degrees_p[0] = 1.0 / C::degree;
    to_degrees_p[1] = to_degrees_p[0];
    to_radians_p[0] = 1.0;
    to_radians_p[1] = 1.0;
    units_p = "rad";
//
    Double longPole2 = longPole;
    Double latPole2 = latPole;
    if (longPole < 999.0) {
       longPole2 = longPole * to_degrees_p[0];
    }
    if (latPole < 999.0) {
       latPole2 = latPole * to_degrees_p[1];
    }                                                  
//
    makeWCS(wcs_p, xform, projection_p, type_p,
            refX, refY, 
            refLong*to_degrees_p[0], refLat*to_degrees_p[1],
            incLong*to_degrees_p[0], incLat*to_degrees_p[1],
            longPole2, latPole2);
}




Bool DirectionCoordinate::cylindricalFix (Int shapeLong, Int shapeLat)
//
// Fix up Cylindrical parameters for when longitude outside of [-180,180] range
// This has to be in its own function because the image shape intrudes.
//   
{
   int naxis[2];
   naxis[0] = shapeLong;  
   naxis[1] = shapeLat;   
   
// Updates crval, crpix, and lonPole

   int ierr = cylfix (naxis, &wcs_p);
   
// ierr = 0 means successful update.
// ierr = -1 means no changed needed
   
   LogIO os(LogOrigin("DirectionCoordinate", "cylindricalFix", WHERE));
//
   if (ierr==-1) {
//      os << LogIO::NORMAL << "No cylindrical coordinate update was required" << LogIO::POST;
      return True;
   }
//
   if (ierr == 0) {

// Update internals of DirectionCoordinate

      Vector<Double> refVal(nWorldAxes());
      refVal[0] = wcs_p.crval[0];
      refVal[1] = wcs_p.crval[1];
      toOther(refVal);   
      setReferenceValue (refVal);
//    
      Vector<Double> refPix(nPixelAxes());
      refPix[0] = wcs_p.crpix[0];
      refPix[1] = wcs_p.crpix[1];
      setReferencePixel(refPix);
//
      os << LogIO::NORMAL << "A cylindrical coordinate update was required and applied" << LogIO::POST;
   } else {
      set_error(String("DirectionCoordinate::cylindricalFix - ") +
                String("Could not convert CYL header to [-180,180] longitude range"));
      return False;
   }
//
   return True;
}



Vector<Double> DirectionCoordinate::longLatPoles () const
{
    Vector<Double> x(4);
    x[0] = wcs_p.crval[0];          // refLong;
    x[1] = wcs_p.crval[1];          // refLat;
    x[2] = wcs_p.lonpole;           // longPole
    x[3] = wcs_p.latpole;           // latPole
    return x;
}

// These world abs/rel functions are independent of the conversion direction type.

void DirectionCoordinate::makeWorldRelative (Vector<Double>& world) const
{
    static MVDirection mv;
    DebugAssert(world.nelements()==2, AipsError);
//
    mv.setAngle(world[0]*to_radians_p[0], world[1]*to_radians_p[1]);
    mv *= rot_p;
//
    world[0] = cos(mv.getLat()) * mv.getLong() / to_radians_p[0];
    world[1] = mv.getLat()  / to_radians_p[1];
}

void DirectionCoordinate::makeWorldRelative (MDirection& world) const
{
    static MVDirection mv;
    mv = world.getValue() * rot_p;
    Double lon = mv.getLong();
    Double lat = mv.getLat();
    mv.setAngle(cos(lat)*lon, lat);
    world.set(mv);
}


void DirectionCoordinate::makeWorldAbsolute (Vector<Double>& world) const
{
    static MVDirection mv;
    DebugAssert(world.nelements()==2, AipsError);
//   
    Double lat = world[1]*to_radians_p[1];
    mv.setAngle(world[0]*to_radians_p[0]/cos(lat), lat);
    mv = rot_p * mv;
//
    world[0] = mv.getLong() / to_radians_p[0];
    world[1]= mv.getLat()  / to_radians_p[1];
}

void DirectionCoordinate::makeWorldAbsoluteRef (Vector<Double>& world,
                                                const Vector<Double>& refVal) const
{
    static MVDirection mv;
    DebugAssert(world.nelements()==2, AipsError);
    DebugAssert(refVal.nelements()==2, AipsError);
//   
    RotMatrix rot;
    setRotationMatrix(rot, refVal[0], refVal[1]);
//
    Double lat = world[1]*to_radians_p[1];
    mv.setAngle(world[0]*to_radians_p[0]/cos(lat), lat);
    mv = rot * mv;
//
    world[0] = mv.getLong() / to_radians_p[0];
    world[1]= mv.getLat()  / to_radians_p[1];
}

void DirectionCoordinate::makeWorldAbsolute (MDirection& world) const
{
    static MVDirection mv;
//
    Double lon = world.getValue().getLong();
    Double lat = world.getValue().getLat();
    mv.setAngle(lon/cos(lat), lat);
//
    world.set(rot_p * mv);
}



Bool DirectionCoordinate::setWorldMixRanges (const IPosition& shape)
{
   AlwaysAssert(nWorldAxes()==nPixelAxes(), AipsError);
   const uInt n = shape.nelements();
   if (n!=nPixelAxes()) {
      set_error("Shape must be of length nPixelAxes");
      return False;
   }
//
   worldMin_p.resize(2);
   worldMax_p.resize(2);
   const Vector<Double>& cdelt = increment();

// Find centre of image.
// If the shape is -1, it means its not known for this world
// axis (user may have removed a pixel axis in CoordinateSystem)
// Use reference pixel in this case

   Vector<Double> pixelIn(referencePixel().copy());
   Vector<Double> worldOut(nWorldAxes());
   for (uInt i=0; i<nPixelAxes(); i++) {
      if (shape(i) > 0) {
         pixelIn(i) = shape(i) / 2.0;
      }
   }
   if (!toWorld(worldOut, pixelIn)) return False;
//
   Vector<String> units = worldAxisUnits();
   Double cosdec = cos(worldOut[1] * to_radians_p[1]);
   Double fac = 1.0;

// If the shape is -1, it means its not known for this world
// axis (user may have removed a pixel axis in CoordinateSystem)
// They might have also have removed a pixel axis  but not a world axis.
// Really, in this case, the replacement world value should be used.


   Float frac = 0.5;
   Int n2 = 0;
   for (uInt i=0; i<2; i++) {
      fac = 1.0;
      if (i==0) fac = cosdec;

// Find number of pixels to offset from centre.
// Do something arbitrary if the shape is <= 1

      if (shape(i)<=1) {
         n2 = 10;
      } else  if (shape(i) > 0) { 
         n2 = (shape(i) + Int(frac*shape(i))) / 2;
      }
//
      worldMin_p(i) = worldOut(i) - abs(cdelt(i))*n2/fac;    
      worldMax_p(i) = worldOut(i) + abs(cdelt(i))*n2/fac;
//
      if (i==0) {
         worldMin_p(i) = putLongInPiRange (worldMin_p(i), units(i));
         worldMax_p(i) = putLongInPiRange (worldMax_p(i), units(i));
      }
   }
//

   return True;
}



void DirectionCoordinate::setWorldMixRanges (const Vector<Bool>& which,
                                             const Vector<Double>& world)
{
   AlwaysAssert(which.nelements()==nWorldAxes(), AipsError);
   AlwaysAssert(world.nelements()==nWorldAxes(), AipsError);
//
   worldMin_p.resize(2, True);
   worldMax_p.resize(2, True);
   const Vector<Double>& cdelt = increment();
   const Vector<String>& units = worldAxisUnits();
//
   Vector<Double> pixelIn(referencePixel().copy());
   Vector<Double> worldOut;
   toWorld(worldOut, pixelIn);
//
   Double cosdec = cos(worldOut[1] * to_radians_p[1]);
   Int n = 10;                                          // Arbitrary offset
   for (uInt i=0; i<nWorldAxes(); i++) {
      if (which(i)) {
         if (i==0) {
            worldMin_p(i) = world(i) - abs(cdelt(i))*n/cosdec;    
            worldMax_p(i) = world(i) + abs(cdelt(i))*n/cosdec;

// Put longitude  in -pi -> pi range

            worldMin_p(i) = putLongInPiRange (worldMin_p(i), units[0]);
            worldMax_p(i) = putLongInPiRange (worldMax_p(i), units[0]);
         } else {
            worldMin_p(i) = world(i) - abs(cdelt(i))*n;
            worldMin_p(i) = max(worldMin_p(i), -90.0/to_degrees_p[1]);
//
            worldMax_p(i) = world(i) + abs(cdelt(i))*n;
            worldMax_p(i) = min(worldMax_p(i),  90.0/to_degrees_p[1]);
         }
      }
   }
}



void DirectionCoordinate::setDefaultWorldMixRanges ()
{
   worldMin_p.resize(2);
   worldMax_p.resize(2);                     
   worldMin_p[0] = -180.0/to_degrees_p[0];     //long
   worldMax_p[0] =  180.0/to_degrees_p[0];
   worldMin_p[1] =  -90.0/to_degrees_p[1];     //lat
   worldMax_p[1] =   90.0/to_degrees_p[1];
}

void DirectionCoordinate::setRotationMatrix ()
{
   setRotationMatrix(rot_p, referenceValue()[0], referenceValue()[1]);
}


void DirectionCoordinate::setRotationMatrix (RotMatrix& rot, 
                                             Double lon, Double lat) const
// 
// Set rotation matrix for use in handling offset coordinates
//
{
    Double refLon = lon * to_radians_p[0];
    Double refLat = lat * to_radians_p[1];
//
    MVDirection refPos(refLon, refLat);
    Euler eul(refLat, 2u, -refLon, 3);
    RotMatrix rot2(eul);
    rot2.transpose();
//
    rot = rot2;
}

void DirectionCoordinate::makeConversionMachines ()
{
   if (type_p != conversionType_p) {
      MDirection::Ref oldType(type_p);
      MDirection::Ref newType(conversionType_p);
      pConversionMachineTo_p   = new MDirection::Convert(oldType, newType);
      pConversionMachineFrom_p = new MDirection::Convert(newType, oldType);
   }
}


void DirectionCoordinate::convertTo (Vector<Double>& world) const
{
   static MVDirection inMV;

// I can't set the machine to operate in the native units because
// the user can set them differently for lon and lat

   if (pConversionMachineTo_p) {
      inMV.setAngle(world[0]*to_radians_p[0], world[1]*to_radians_p[1]);
      world  = (*pConversionMachineTo_p)(inMV).getValue().get() / to_radians_p;
   }
}

void DirectionCoordinate::convertToMany (Matrix<Double>& world) const

{
// Make sure there is something to do !

    if (!pConversionMachineTo_p) return;
//
    AlwaysAssert(nWorldAxes()==world.nrow(), AipsError);
    Vector<Double> worldTmp(nWorldAxes());
    ArrayAccessor<Double, Axis<1> > jWorld(world);
    ArrayAccessor<Double, Axis<0> > iWorld;
//
    uInt k;
    for (jWorld.reset(); jWorld!=jWorld.end(); ++jWorld) {
       iWorld = jWorld;            // Partial assignment
       for (iWorld.reset(),k=0; iWorld!=iWorld.end(); ++iWorld,k++) {
          worldTmp[k] = *iWorld;
       }

// Convert this coordinate pair

       convertTo(worldTmp);

// Fill back into Matrix

       iWorld = jWorld;            // Partial assigment
       for (iWorld.reset(),k=0; iWorld!=iWorld.end(); ++iWorld,k++) {
           *iWorld = worldTmp[k];
       }
    }
}




void DirectionCoordinate::convertFrom (Vector<Double>& world) const
{
   static MVDirection inMV;

// I can't set the machine to operate in the native units because
// the user can set them differently for lon and lat

   if (pConversionMachineFrom_p) {
      inMV.setAngle(world[0]*to_radians_p[0], world[1]*to_radians_p[1]);
      world = (*pConversionMachineFrom_p)(inMV).getValue().get() / to_radians_p;
   }
}


void DirectionCoordinate::convertFromMany (Matrix<Double>& world) const
{

// Make sure there is something to do

    if (!pConversionMachineFrom_p) return;
//
    AlwaysAssert(nWorldAxes()==world.nrow(), AipsError);
    Vector<Double> worldTmp(nWorldAxes());
    ArrayAccessor<Double, Axis<1> > jWorld(world);
    ArrayAccessor<Double, Axis<0> > iWorld;
//
    uInt k;
    for (jWorld.reset(); jWorld!=jWorld.end(); ++jWorld) {
       iWorld = jWorld;            // Partial assignment
       for (iWorld.reset(),k=0; iWorld!=iWorld.end(); ++iWorld,k++) {
          worldTmp[k] = *iWorld;
       }

// Convert this coordinate pair

       convertFrom(worldTmp);

// Fill back into Matrix

       iWorld = jWorld;            // Partial assigment
       for (iWorld.reset(),k=0; iWorld!=iWorld.end(); ++iWorld,k++) {
           *iWorld = worldTmp[k];
       }
    }
}


Double DirectionCoordinate::putLongInPiRange (Double lon, const String& unit) const
{  
   Unit u(unit);
   Quantum<Double> q(lon, u);
   MVAngle mva(q);
   const MVAngle& mva2 = mva();
   Double t = mva2.get(u).getValue();
//   if (t < 0) t += 360.0/to_degrees_p[0];
   return t;
}


// Helper functions to help us interface to WCS

void DirectionCoordinate::makeWCS(wcsprm& wcs,  const Matrix<Double>& xform,
                                  const Projection& proj, MDirection::Types directionType,
                                  Double refPixLong, Double refPixLat,
                                  Double refLong, Double refLat,
                                  Double incLong, Double incLat,
                                  Double longPole, Double latPole)
{
    wcs.flag = -1;
    int iret = wcsini(1, 2, &wcs);
    if (iret != 0) {
        String errmsg = "wcs wcsini_error: ";
        errmsg += wcsini_errmsg[iret];
        throw(AipsError(errmsg));
    }

// Fill in PC matrix

    xFormToPC (wcs, xform);

// Now the rest

    wcs.crpix[0] = refPixLong;
    wcs.crpix[1] = refPixLat;
//
    wcs.cdelt[0] = incLong;
    wcs.cdelt[1] = incLat;
//
    wcs.crval[0] = refLong;
    wcs.crval[1] = refLat;
//
    wcs.lonpole = longPole;
    wcs.latpole = latPole;
        
// Construct FITS ctype vector   
     
    Bool isNCP = False;
    Vector<String> axisNames = DirectionCoordinate::axisNames(directionType, True);
    Vector<String> ctype = make_Direction_FITS_ctype(isNCP, proj, axisNames,
                                                     refLat*C::degree, False);
    strncpy (wcs.ctype[0], ctype[0].chars(), 9);
    strncpy (wcs.ctype[1], ctype[1].chars(), 9);
//
    String name = Projection::name(proj.type());
    const Vector<Double>& projParameters = proj.parameters();
    const uInt nProj = projParameters.nelements();
    uInt startAt = ((proj.type() == Projection::ZPN) ? 0 : 1);     // Only ZPN uses prjprm->p[0]
//
    wcs.npv = nProj;
    for (uInt i=0; i < nProj; i++) {
       wcs.pv[i].i = 2;                  // Latitude, 1-relative
       wcs.pv[i].m = i+startAt;
       wcs.pv[i].value = projParameters[i];
    }
    
// Fill in the wcs structure
  
    if (int iret = wcsset(&wcs)) {
        String errmsg = "wcs wcsset_error: ";
        errmsg += wcsset_errmsg[iret];
        throw(AipsError(errmsg));
    }
}

void DirectionCoordinate::pcToXform (Matrix<Double>& xform, const wcsprm& wcs) const
{
   xform.resize(2,2);
   uInt count = 0;
   for (uInt i=0; i<2; i++) {
     for (uInt j=0; j<2; j++) {
        xform(j,i) = wcs.pc[count];
        count++;
     }
   }
}

void DirectionCoordinate::xFormToPC (wcsprm& wcs, const Matrix<Double>& xform) const
{
   uInt count = 0;
   for (uInt i=0; i<2; i++) {
     for (uInt j=0; j<2; j++) {
        wcs.pc[count] = xform(j,i);
        count++;
     }
   }
}
