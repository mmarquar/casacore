//# DirectionCoordinate.h: this defines the DirectionCoordinate class
//# Copyright (C) 1997,1998,1999
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
#include <wcslib/proj.h>

#include <trial/Coordinates/DirectionCoordinate.h>

#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Containers/Record.h>
#include <aips/Logging/LogIO.h>
#include <aips/Mathematics/Constants.h>
#include <aips/Quanta/MVAngle.h>
#include <aips/Quanta/Quantum.h>
#include <aips/Quanta/Unit.h>
#include <aips/Utilities/Assert.h>
#include <aips/Utilities/LinearSearch.h>
#include <aips/Utilities/String.h>

#include <iomanip.h>  
#include <strstream.h>


DirectionCoordinate::DirectionCoordinate()
: Coordinate(),
  type_p(MDirection::J2000), projection_p(Projection(Projection::CAR)),
  celprm_p(0), prjprm_p(0), wcs_p(0), linear_p(1),
  names_p(2), units_p(2)
{
    // Initially we are in radians
    to_degrees_p[0] = 180.0 / C::pi;
    to_degrees_p[1] = to_degrees_p[0];
    units_p = "rad";

    Vector<Double> crval(2), cdelt(2), crpix(2);
    crval = 0.0;
    cdelt = 1.0;
    crpix = 0.0;
    Matrix<Double> xform(2,2); 
    xform = 0.0;
    xform.diagonal() = 1.0;

    toDegrees(crval);
    toDegrees(cdelt);
    linear_p = LinearXform(crpix, cdelt, xform);
    make_celprm_and_prjprm(celprm_p, prjprm_p, wcs_p, c_ctype_p, c_crval_p,
                           projection_p, type_p, crval(0), crval(1), 
			   999.0, 999.0);
}

DirectionCoordinate::DirectionCoordinate(MDirection::Types directionType,
                                         const Projection &projection,
                                         Double refLong, Double refLat,
                                         Double incLong, Double incLat,
                                         const Matrix<Double> &xform,
                                         Double refX, Double refY)
: Coordinate(),
  type_p(directionType), projection_p(projection),
  celprm_p(0), prjprm_p(0), wcs_p(0), linear_p(1),
  names_p(axisNames(directionType).copy()),
  units_p(2)
{
    // Initially we are in radians
    to_degrees_p[0] = 180.0 / C::pi;
    to_degrees_p[1] = to_degrees_p[0];
    units_p = "rad";

    Vector<Double> crval(2), cdelt(2), crpix(2);
    crval(0) = refLong; crval(1) = refLat;
    cdelt(0) = incLong; cdelt(1) = incLat;
    crpix(0) = refX;    crpix(1) = refY;

    toDegrees(crval);
    toDegrees(cdelt);
    linear_p = LinearXform(crpix, cdelt, xform);

    make_celprm_and_prjprm(celprm_p, prjprm_p, wcs_p, c_ctype_p, 
                           c_crval_p, projection_p, type_p, 
                           crval(0), crval(1),  999.0, 999.0);
}

DirectionCoordinate::DirectionCoordinate(const DirectionCoordinate &other)
: Coordinate(other),
  type_p(other.type_p), projection_p(other.projection_p),
  celprm_p(0), prjprm_p(0), wcs_p(0), linear_p(other.linear_p),
  names_p(other.names_p.copy()), units_p(other.units_p.copy())
{
    to_degrees_p[0] = other.to_degrees_p[0];
    to_degrees_p[1] = other.to_degrees_p[1];
    copy_celprm_and_prjprm(celprm_p, prjprm_p, wcs_p, c_ctype_p, 
                           c_crval_p, other.celprm_p, other.prjprm_p,
                           other.wcs_p, other.c_ctype_p, other.c_crval_p);
}


DirectionCoordinate &DirectionCoordinate::operator=(const DirectionCoordinate &other)
{
    if (this != &other) {
        Coordinate::operator=(other);
//
	type_p = other.type_p;
	projection_p = other.projection_p;
//
	DirectionCoordinate::copy_celprm_and_prjprm(celprm_p, prjprm_p, wcs_p, 
                               c_ctype_p, c_crval_p,
			       other.celprm_p, other.prjprm_p,
                               other.wcs_p, 
			       other.c_ctype_p, other.c_crval_p);
	linear_p = other.linear_p;
	names_p = other.names_p.copy();
	units_p = other.units_p.copy();
	to_degrees_p[0] = other.to_degrees_p[0];
	to_degrees_p[1] = other.to_degrees_p[1];
    }
    return *this;
}

DirectionCoordinate::~DirectionCoordinate()
{
    delete celprm_p; 
    celprm_p = 0;
//
    delete prjprm_p; 
    prjprm_p = 0;
//
    delete wcs_p;
    wcs_p = 0;
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

Bool DirectionCoordinate::toWorld(Vector<Double> &world,
				  const Vector<Double> &pixel) const
{
    if (world.nelements()!=2) world.resize(2);
    AlwaysAssert(pixel.nelements() == 2, AipsError);

    Bool ok = linear_p.reverse(world, pixel, errorMsg_p);
    if (ok) {
	double lng, lat, phi, theta;
	double x = world(0), y=world(1); // world contains linear xformed numbers
	int errnum = celrev(pcodes[projection_p.type()],
			    x, y, prjprm_p, &phi, &theta,
			    celprm_p, &lng, &lat);
	ok = ToBool((errnum == 0));
	if (ok) {
	    world(0) = lng;
	    world(1) = lat;
	} else {
	    errorMsg_p = "wcslib celrev error: ";
	    errorMsg_p += celrev_errmsg[errnum];
	}
    }

    if (!ok) {
	set_error(errorMsg_p);
    }

    toOther(world);
    return ok;
}

Bool DirectionCoordinate::toPixel(Vector<Double> &pixel,
				  const Vector<Double> &world) const
{
    if (pixel.nelements()!=2) pixel.resize(2);
    AlwaysAssert(world.nelements() == nWorldAxes(), AipsError);
    if (world_tmp_p.nelements()!=nWorldAxes()) world_tmp_p.resize(nWorldAxes());
//
    world_tmp_p(0) = world(0); 
    world_tmp_p(1) = world(1);
    toDegrees(world_tmp_p);

    double x, y, phi, theta;
    double lng = world_tmp_p(0);
    double lat = world_tmp_p(1);
    int errnum = celfwd(pcodes[projection_p.type()], lng, lat,
			celprm_p, &phi, &theta, prjprm_p, &x, &y);
//
    world_tmp_p(0) = x; 
    world_tmp_p(1) = y;
    Bool ok = ToBool(errnum == 0);
    if (ok) {
	ok = linear_p.forward(world_tmp_p, pixel, errorMsg_p);
    } else {
        errorMsg_p = "wcslib celfwd error: ";
        errorMsg_p += celfwd_errmsg[errnum];
    }

    if (! ok) {
	set_error(errorMsg_p);
    }

    return ok;
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
   const uInt nWorld = worldAxes.nelements();
   const uInt nPixel = pixelAxes.nelements();
   AlwaysAssert(nWorld==nWorldAxes(), AipsError);
   AlwaysAssert(nPixel==nPixelAxes(), AipsError);
   AlwaysAssert(worldIn.nelements()==nWorld, AipsError);
   AlwaysAssert(pixelIn.nelements()==nPixel, AipsError);
   AlwaysAssert(worldMin.nelements()==nWorld, AipsError);
   AlwaysAssert(worldMax.nelements()==nWorld, AipsError);
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
   if (worldOut.nelements()!=nWorldAxes()) worldOut.resize(nWorldAxes());
   if (pixelOut.nelements()!=nPixelAxes()) pixelOut.resize(nPixelAxes());
//
   if (pixelAxes(0) && pixelAxes(1)) {
//
// pixel->world
//
      if (!toWorld(worldOut, pixelIn)) return False;
      pixelOut(0) = pixelIn(0);
      pixelOut(1) = pixelIn(1);
   } else if (worldAxes(0) && worldAxes(1)) {
//
// world->pixel
//
      if (!toPixel(pixelOut, worldIn)) return False;
      worldOut(0) = worldIn(0);
      worldOut(1) = worldIn(1);
   } else if (pixelAxes(0) && worldAxes(1)) {
//
// pixel,world->world,pixel
//
      if (in_tmp_p.nelements()!=2) in_tmp_p.resize(2);
      if (out_tmp_p.nelements()!=2) out_tmp_p.resize(2);
//
      in_tmp_p(0) = pixelIn(0);
      in_tmp_p(1) = worldIn(1);
      if (!toMix2(out_tmp_p, in_tmp_p, worldMin, worldMax, False)) return False;
//
      pixelOut(0) = in_tmp_p(0);
      pixelOut(1) = out_tmp_p(1);
      worldOut(0) = out_tmp_p(0);
      worldOut(1) = in_tmp_p(1);  
   } else if (worldAxes(0) && pixelAxes(1)) {
//
// world,pixel->pixel,world
//
      if (in_tmp_p.nelements()!=2) in_tmp_p.resize(2);
      if (out_tmp_p.nelements()!=2) out_tmp_p.resize(2);
//
      in_tmp_p(0) = worldIn(0);
      in_tmp_p(1) = pixelIn(1);
      if (!toMix2(out_tmp_p, in_tmp_p, worldMin, worldMax, True)) return False;
//
      pixelOut(0) = out_tmp_p(0);
      pixelOut(1) = in_tmp_p(1);
      worldOut(0) = in_tmp_p(0);
      worldOut(1) = out_tmp_p(1);
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
    return names_p.copy();
}

Vector<String> DirectionCoordinate::worldAxisUnits() const
{
    return units_p.copy();
}

Vector<Double> DirectionCoordinate::referenceValue() const
{
    Vector<Double> crval(2);
    crval(0) = celprm_p->ref[0];
    crval(1) = celprm_p->ref[1];
    toOther(crval);
    return crval;
}

Vector<Double> DirectionCoordinate::increment() const
{
    Vector<Double> cdelt(linear_p.cdelt().copy());
    toOther(cdelt);
    return cdelt;
}

Matrix<Double> DirectionCoordinate::linearTransform() const
{
    return linear_p.pc();
}

Vector<Double> DirectionCoordinate::referencePixel() const
{
    return linear_p.crpix();
}

Bool DirectionCoordinate::setWorldAxisNames(const Vector<String> &names)
{
    Bool ok = ToBool(names.nelements()==nWorldAxes());
    if (!ok) {
       set_error("names vector must be of length 2");
       return False;
    }
//
    names_p = names;
    return True;
}

Bool DirectionCoordinate::setWorldAxisUnits(const Vector<String> &units, 
					  Bool adjust)
{
    Bool ok = ToBool(units.nelements()==nWorldAxes());
    if (!ok) {
       set_error("units vector must be of length 2");
       return False;
    }
//
    if (adjust) {
	String error;
	Vector<Double> factor;
	ok = find_scale_factor(error, factor, units, worldAxisUnits());
	if (ok) {
	    to_degrees_p[0] /= factor(0);
	    to_degrees_p[1] /= factor(1);
	} else {
	    set_error(error);
	}
    }
    if (ok) units_p = units;

    return ok;
}


Bool DirectionCoordinate::setReferencePixel(const Vector<Double> &refPix)
{
    Bool ok = ToBool(refPix.nelements()==nPixelAxes());
    if (!ok) {
       set_error("reference pixels vector must be of length 2");
       return False;
    }
//
    linear_p.crpix(refPix);
    return True;
}

Bool DirectionCoordinate::setLinearTransform(const Matrix<Double> &xform)
{
    Bool ok = ToBool(xform.nrow() == nWorldAxes() && 
                     xform.ncolumn() == nWorldAxes());
    if (!ok) {
       set_error("linear transform matrix has wrong shape");
       return False;
    }
//
    linear_p.pc(xform);
    return True;
}

Bool DirectionCoordinate::setIncrement(const Vector<Double> &inc)
{
    Bool ok = ToBool(inc.nelements()==nWorldAxes());
    if (!ok) {
       set_error("Two increments must be provided!");
       return False;
    }
//
    Vector<Double> tmp(inc.copy());
    toDegrees(tmp);
    linear_p.cdelt(tmp);

    return ok;
}

Bool DirectionCoordinate::setReferenceValue(const Vector<Double> &refval)
{
    Bool ok = ToBool(refval.nelements()==nWorldAxes());
    if (!ok) {
       set_error("Two ref. values must be provided!");
       return False;
    }
//
    Vector<Double> tmp(refval.copy());
    toDegrees(tmp);
    celprm_p->flag  = 0;
    celprm_p->ref[0] = tmp(0);
    celprm_p->ref[1] = tmp(1);

    String name = Projection::name(projection().type());
    const char *nameptr = name.chars();
    int errnum = celset(nameptr, celprm_p, prjprm_p);
    ok = ToBool(errnum==0);
    if (!ok) {
       String errmsg = "wcs celset_error: ";
       errmsg += celset_errmsg[errnum];
    }
//
// Fill in the silly wcs private members I have to cart about
// for toMix caching
//
    c_crval_p[0] = tmp(0);
    c_crval_p[1] = tmp(1);
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
	    names(0) = "RA";
	    names(1) = "DEC";
	    break;
	case MDirection::GALACTIC:
	    names(0) = "GLON";
	    names(1) = "GLAT";
	    break;
	case MDirection::HADEC:
	    names(0) = "HA";
	    names(1) = "DEC";
	    break;
	case MDirection::AZEL:
	    names(0) = "AZ";
	    names(1) = "EL";
	    break;
	default:
	    names = "????";
	}
    } else {
	switch(type) {
	case MDirection::J2000: // These are all RA/DEC systems
	case MDirection::JMEAN:
	case MDirection::APP:
	case MDirection::B1950:
	case MDirection::BMEAN:
	case MDirection::BTRUE:
	    names(0) = "Right Ascension";
	    names(1) = "Declination";
	    break;
	case MDirection::GALACTIC:
	    names(0) = "Galactic Longitude";
	    names(1) = "Galactic Latitude";
	    break;
	case MDirection::HADEC:
	    names(0) = "Hour Angle";
	    names(1) = "Declination";
	    break;
	case MDirection::AZEL:
	    names(0) = "Azimuth";
	    names(1) = "Elevation";
	    break;
	default:
	    names = "Unknown";
	}
    }
    return names;
}


void DirectionCoordinate::checkFormat(Coordinate::formatType& format,
                                      const Bool absolute) const
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
                                        const Bool absolute,
                                        const Int defPrecScientific,
                                        const Int defPrecFixed,
                                        const Int defPrecTime) const
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
                                   const Coordinate::formatType format,
                                   const Double worldValue,
                                   const uInt worldAxis,
                                   const Bool absolute,
                                   const Int precision) const
//
// Input
//   worldValue   must be radians
//
{
   AlwaysAssert(worldAxis < nWorldAxes(), AipsError);

// Fill in DEFAULT format

   Coordinate::formatType form = format;
   checkFormat(form, absolute);


// Set default precision if needed

   Int prec = precision;
   if (prec < 0) getPrecision(prec, form, absolute, -1, -1, -1);

// Set global DirectionCoordinate type

   MDirection::GlobalTypes gtype = MDirection::globalType(type_p);

// Format according to required format type and absolute/offset
// and type of DirectionCoordinate.  Endless bloody ifs. I don't
// like case statements !

   ostrstream oss;         
   MVAngle mVA(worldValue);
   units = " ";

   if (gtype == MDirection::GRADEC || gtype == MDirection::GHADEC) {
      if (form == Coordinate::SCIENTIFIC) {
         oss.setf(ios::scientific, ios::floatfield);
         oss.precision(prec);
         if (absolute) {
            oss << worldValue;
            units = "rad";
         } else {     
            oss << mVA.degree() * 3600;
            units = "arcsec";
         }
      } else if (form == Coordinate::FIXED) {
         oss.setf(ios::fixed, ios::floatfield);
         oss.precision(prec);
         if (absolute) {
            oss << worldValue;
            units = "rad";
         } else {     
            oss << mVA.degree() * 3600;
            units = "arcsec";
         }
      } else if (form == Coordinate::TIME) {
         prec += 6;
         if (absolute) {
            if (worldAxis == 0) {
               if (gtype == MDirection::GRADEC) {
                  oss << mVA.string(MVAngle::TIME,prec);
               } else {
                  oss << mVA.string(MVAngle::TIME+MVAngle::DIG2,prec);
               }
            } else {
               oss << mVA.string(MVAngle::DIG2,prec); 
            }
         } else {
            if (worldAxis == 0) {
               oss << mVA.string(MVAngle::TIME+MVAngle::DIG2,prec);
            } else {
               oss << mVA.string(MVAngle::DIG2,prec); 
            }
         }
      }
   } else if (gtype == MDirection::GLONGLAT) {
      if (form == Coordinate::SCIENTIFIC) {
         oss.setf(ios::scientific, ios::floatfield);
         oss.precision(prec);
         oss << mVA.degree();
         units = "deg";
      } else if (form == Coordinate::FIXED) {
         oss.setf(ios::fixed, ios::floatfield);
         oss.precision(prec);
         oss << mVA.degree();
         units = "deg";
      } else if (form == Coordinate::TIME) {
         prec += 6;
         oss << mVA.string(MVAngle::ANGLE,prec);
      }
   } else if (gtype == MDirection::GAZEL) {
      if (form == Coordinate::SCIENTIFIC) {
         oss.setf(ios::scientific, ios::floatfield);
         oss.precision(prec);
         oss << mVA.degree();
         units = "deg";
      } else if (form == Coordinate::FIXED) {
         oss.setf(ios::fixed, ios::floatfield);
         oss.precision(prec);
         oss << mVA.degree();
         units = "deg";
      } else if (form == Coordinate::TIME) {
         prec += 6;
         if (worldAxis == 0) {
            oss << mVA.string(MVAngle::ANGLE,prec);
         } else {
            oss << mVA.string(MVAngle::DIG2,prec);
         }
      }
   } else {

// Some protection against new MDirection::globalTypes

      if (form == Coordinate::SCIENTIFIC) {
         oss.setf(ios::scientific, ios::floatfield);
         oss.precision(prec);
         oss << worldValue;
         units = "rad";
      } else if (form == Coordinate::FIXED) {
         oss.setf(ios::fixed, ios::floatfield);
         oss.precision(prec);
         oss << worldValue;
         units = "rad";
      } else {

// Don't do TIME formatting because we don't know what
// we have here.

         oss.setf(ios::scientific, ios::floatfield);
         oss.precision(prec);
         oss << worldValue;
         units = "rad";
      }
   }
   oss << ends;
   String string(oss);

   return string;
}



Coordinate *DirectionCoordinate::clone() const
{
    return new DirectionCoordinate(*this);
}

void DirectionCoordinate::toDegrees(Vector<Double> &other) const
{
    other(0) *= to_degrees_p[0];
    other(1) *= to_degrees_p[1];
}


Bool DirectionCoordinate::near(const Coordinate* pOther, 
                               Double tol) const
{
   Vector<Int> excludeAxes;
   return near(pOther, excludeAxes, tol);
}



Bool DirectionCoordinate::near(const Coordinate* pOther, 
                               const Vector<Int>& excludeAxes,
                               Double tol) const

{
   if (this->type() != pOther->type()) {
      set_error("Comparison is not with another DirectionCoordinate");
      return False;
   }
     
   DirectionCoordinate* dCoord = (DirectionCoordinate*)pOther;
   
   if (!projection_p.near(dCoord->projection_p, tol)) {
      set_error("The DirectionCoordinates have differing projections");
      return False;
   }
   if (type_p != dCoord->type_p) {
      set_error("The DirectionCoordinates have differing types");
      return False;
   }


// Number of pixel and world axes is the same for a DirectionCoordinate
// Add an assertion check should this change 
 
   AlwaysAssert(nPixelAxes()==nWorldAxes(), AipsError);
   Vector<Bool> exclude(nPixelAxes());   
   exclude = False;
   Bool found;
   const uInt nExcl = excludeAxes.nelements();
   if (nExcl > 0) {
      Int j = 0;
      for (uInt i=0; i<nPixelAxes(); i++) {
         if (linearSearch(found, excludeAxes, Int(i), nExcl) >= 0) 
           exclude(j++) = True;
      }
   }

// Check linear transformation

   if (!linear_p.near(dCoord->linear_p, excludeAxes, tol)) {
      set_error("The DirectionCoordinates have differing linear transformation matrices");
      return False;
   }


// Check names and units

   ostrstream oss;
   if (names_p.nelements() != dCoord->names_p.nelements()) {
      set_error("The DirectionCoordinates have differing numbers of world axis names");
      return False;
   }
   if (units_p.nelements() != dCoord->units_p.nelements()) {
      set_error("The DirectionCoordinates have differing numbers of axis units");
      return False;
   }
   for (uInt i=0; i<names_p.nelements(); i++) {
      if (!exclude(i)) {
         if (names_p(i) != dCoord->names_p(i)) {
            oss << "The DirectionCoordinates have differing axis names for axis "
                << i << ends;
            set_error(String(oss));
            return False;      
         }
      }
   }
   for (uInt i=0; i<units_p.nelements(); i++) {
      if (!exclude(i)) {
         if (units_p(i) != dCoord->units_p(i)) {
            oss << "The DirectionCoordinates have differing axis units for axis "
                << i << ends;
            set_error(String(oss));
            return False;      
         }
      }
   }

     
// WCS structures.  

   if (celprm_p->flag != dCoord->celprm_p->flag) {
      set_error("The DirectionCoordinates have differing WCS spherical coordinate structures");
      return False;
   }

// Items 0 and 2 are longitudes, items 1 and 3 are latitiudes
// So treat them as axes 0 and 1 ???

   for (uInt i=0; i<2; i++) {
     if (!exclude(i)) {
        if (!::near(celprm_p->ref[i],dCoord->celprm_p->ref[i],tol)) {
           set_error("The DirectionCoordinates have differing WCS spherical coordinate structures");
           return False;
        }
        if (!::near(celprm_p->ref[i+2],dCoord->celprm_p->ref[i+2],tol)) {
           set_error("The DirectionCoordinates have differing WCS spherical coordinate structures");
           return False;
        }
     }
   }

// No idea what to do here for exclusion axes !
// Don't test prjprm->flag as it is a transient flag 
// whose value may change depending on what computations you
// do when
// 

   if (prjprm_p->r0!=0.0 && dCoord->prjprm_p->r0!=0.0) {

// 0 is the start value, and the WCS routines will change
// it, so if either value is 0, it's ok

      if (prjprm_p->r0 != dCoord->prjprm_p->r0) {
         set_error("The DirectionCoordinates have differing WCS projection parameters");
         return False;
       }
   }
   for (uInt i=0; i<10; i++) {
      if (!::near(prjprm_p->p[i],dCoord->prjprm_p->p[i],tol)) {
         set_error("The DirectionCoordinates have differing WCS projection structures");
         return False;
      }
   }
   
                          
// Probably these arrays are implicitly the same if the units are the same
// but might as well make sure

   if (!exclude(0)) {
      if (!::near(to_degrees_p[0],dCoord->to_degrees_p[0],tol)) {
         set_error("The DirectionCoordinates have differing unit conversion vectors");
         return False;
      }
   }
   if (!exclude(1)) {
      if (!::near(to_degrees_p[1],dCoord->to_degrees_p[1],tol)) {
         set_error("The DirectionCoordinates have differing unit conversion vectors");
         return False;
      }
   }

// These things are needed only for toMix caching speedup

   for (uInt i=0; i<2; i++) {
      if (strcmp(c_ctype_p[i],dCoord->c_ctype_p[i])!=0) {
         set_error("The DirectionCoordinates have differing FITS ctypes");
         return False;
      }
      if (!::near(c_crval_p[i],dCoord->c_crval_p[i],tol)) {
         set_error("The DirectionCoordinates have differing FITS crvals");
         return False;
      }
   }
// 
// The flag variable may change with use, so don't test it.
//   Bool ok = wcs_p->flag==dCoord->wcs_p->flag &&
//
   Bool ok = strcmp(wcs_p->pcode,dCoord->wcs_p->pcode)==0 &&
        strcmp(wcs_p->lngtyp,dCoord->wcs_p->lngtyp)==0 &&
        strcmp(wcs_p->lattyp,dCoord->wcs_p->lattyp)==0 &&
        wcs_p->lng==dCoord->wcs_p->lng &&
        wcs_p->lat==dCoord->wcs_p->lat &&        
        wcs_p->cubeface==dCoord->wcs_p->cubeface;
   if (!ok) {
      set_error("The DirectionCoordinates have differing wcs structures");
      return False;
   }
   return True;
}




Bool DirectionCoordinate::save(RecordInterface &container,
			    const String &fieldName) const
{
    Bool ok = ToBool(!container.isDefined(fieldName));
    if (ok) {
	Record subrec;
	Projection proj = projection();
	String system = MDirection::showType(type_p);

	subrec.define("system", system);
	subrec.define("projection", proj.name());
	subrec.define("projection_parameters", proj.parameters());
	subrec.define("crval", referenceValue());
	subrec.define("crpix", referencePixel());
	subrec.define("cdelt", increment());
	subrec.define("pc", linearTransform());
	subrec.define("axes", worldAxisNames());
	subrec.define("units", worldAxisUnits());

	container.defineRecord(fieldName, subrec);
    }
    return ok;
}

DirectionCoordinate *DirectionCoordinate::restore(const 
						  RecordInterface &container,
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
    MDirection::Types sys;
    MDirection::Ref mref;
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

    if (!subrec.isDefined("crval")) {
	return 0;
    }
    Vector<Double> crval;
    subrec.get("crval", crval);

    if (!subrec.isDefined("crpix")) {
	return 0;
    }
    Vector<Double> crpix;
    subrec.get("crpix", crpix);

    if (!subrec.isDefined("cdelt")) {
	return 0;
    }
    Vector<Double> cdelt;
    subrec.get("cdelt", cdelt);

    if (!subrec.isDefined("pc")) {
	return 0;
    }
    Matrix<Double> pc;
    subrec.get("pc", pc);

    
    if (!subrec.isDefined("axes")) {
	return 0;
    }
    Vector<String> axes;
    subrec.get("axes", axes);

    
    if (!subrec.isDefined("units")) {
	return 0;
    }
    Vector<String> units;
    subrec.get("units", units);

    DirectionCoordinate *retval = 
	new DirectionCoordinate(sys, proj, 0, 0, 1, 1, pc, 0, 0);
    AlwaysAssert(retval, AipsError);

    // We have to do the units first since they will change the
    // reference value and increment if we do them too late.
    retval->setWorldAxisUnits(units);
    retval->setWorldAxisNames(axes);
    retval-> setIncrement(cdelt);
    retval->setReferenceValue(crval);
    retval->setReferencePixel(crpix);
							  
    return retval;
}


void DirectionCoordinate::toOther(Vector<Double> &degrees) const
{
    degrees(0) /= to_degrees_p[0];
    degrees(1) /= to_degrees_p[1];
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
// Set input world/pixel arrays
//
    Unit degrees("deg");
    if (longIsWorld) {
       mixcel_p = 1;          // 1 or 2 (a code, not an index)
       mixpix_p = 1;          // index into pixcrd array
//
       mix_quant_tmp.setValue(in(0));
       mix_quant_tmp.setUnit(Unit(units_p(0)));
       mix_world_p[wcs_p->lng] = mix_quant_tmp.getValue(degrees);
       mix_pixcrd_p[mixpix_p] = in(1);
//
// Latitude span
//
       mix_quant_tmp.setUnit(Unit(units_p(1)));
       mix_quant_tmp.setValue(minWorld(1));
       mix_vspan_p[0] = mix_quant_tmp.getValue(degrees);
//
       mix_quant_tmp.setValue(maxWorld(1));
       mix_vspan_p[1] = mix_quant_tmp.getValue(degrees);
    } else {
       mixcel_p = 2;          // 1 or 2 (a code, not an index)
       mixpix_p = 0;          // index into pixcrd array
//
       mix_quant_tmp.setValue(in(1));
       mix_quant_tmp.setUnit(Unit(units_p(1)));
       mix_world_p[wcs_p->lat] = mix_quant_tmp.getValue(degrees);
       mix_pixcrd_p[mixpix_p] = in(0);
//
// Longitude span
//
       mix_quant_tmp.setUnit(Unit(units_p(0)));
       mix_quant_tmp.setValue(minWorld(0));
       mix_vspan_p[0] = mix_quant_tmp.getValue(degrees);
//
       mix_quant_tmp.setValue(maxWorld(0));
       mix_vspan_p[1] = mix_quant_tmp.getValue(degrees);
    }
//
// Do it
//
//cout << "doMix2: mixpix_p = " << mixpix_p << endl;
//cout << "doMix2: mixcel_p = " << mixcel_p << endl;
//cout << "doMix2: input world = " << mix_world_p[0] << ", " << mix_world_p[1] << endl;
//cout << "doMix2: input pixel = " << mix_pixcrd_p[0] << ", " << mix_pixcrd_p[1] << endl;
//cout << "doMix2: input vspan_p = " << mix_vspan_p[0] << ", " << mix_vspan_p[1] << endl;
//cout << "doMix2: c_crval= " << c_crval_p[0] << " " << c_crval_p[1] << endl;
//
    mix_vstep_p = 1.0;
    mix_viter_p = 2;
    int iret = wcsmix(c_ctype_p, wcs_p, mixpix_p, mixcel_p, mix_vspan_p, 
                      mix_vstep_p, mix_viter_p, 
                      mix_world_p, c_crval_p, celprm_p, &mix_phi_p, &mix_theta_p, 
                      prjprm_p, mix_imgcrd_p, linear_p.linprmWCS(), mix_pixcrd_p);
    if (iret!=0) {
        errorMsg_p= "wcs wcsmix_error: ";
        errorMsg_p += wcsmix_errmsg[iret];
        set_error(errorMsg_p);
        return False;
    }
//
// Fish out the results
//
//cout << "doMix2: output world = " << mix_world_p[0] << ", " << mix_world_p[1] << endl;
//cout << "doMix2: output pixel = " << mix_pixcrd_p[0] << ", " << mix_pixcrd_p[1] << endl;
//cout << "doMix2: phi, theta=" << phi << ", " << theta << endl;

    if (longIsWorld) {
       out(0) = mix_pixcrd_p[0];
       Double tmp1 = Double(mix_world_p[wcs_p->lat]);
       Quantum<Double> tmp(tmp1, Unit("deg"));
       out(1) = tmp.getValue(Unit(units_p(1)));
    } else {
       Double tmp1 = Double(mix_world_p[wcs_p->lng]);
       Quantum<Double> tmp(tmp1, Unit("deg"));
       out(0) = tmp.getValue(Unit(units_p(0)));
       out(1) = mix_pixcrd_p[1];
    } 
//
    return True;
}


// Helper functions to help us interface to WCS

void DirectionCoordinate::make_celprm_and_prjprm(celprm* &pCelPrm, prjprm* &pPrjPrm, wcsprm* &pWcs,  
                                                 char c_ctype[2][9], double c_crval[2],
                                                 const Projection& proj,
                                                 MDirection::Types directionType,
                                                 Double refLong, Double refLat,  
                                                 Double longPole, Double latPole) const
{
    pCelPrm = new celprm;
    if (! pCelPrm) {
        throw(AllocError("DirectionCoordinate::make_celprm_and_prjprm()", sizeof(celprm)));
    }
    pCelPrm->flag = 0;
    pCelPrm->ref[0] = refLong;
    pCelPrm->ref[1] = refLat;
    pCelPrm->ref[2] = longPole;
    pCelPrm->ref[3] = latPole;

    pPrjPrm = new prjprm;
    if (! pPrjPrm) {
        delete pCelPrm;
        pCelPrm = 0;
        throw(AllocError("static ::make_celprm_and_prjprm()", sizeof(prjprm)));
    }
    pPrjPrm->flag = 0;
    pPrjPrm->r0 = 0.0;
    objset(pPrjPrm->p, double(0.0), uInt(10));
    uInt nRequiredParameters = Projection::nParameters(proj.type());
    Vector<Double> projParameters = proj.parameters();
    AlwaysAssert(projParameters.nelements() >= nRequiredParameters, AipsError);
    int startAt = ((proj.type() == Projection::ZPN) ? 0 : 1);
    for (uInt i=0; i < nRequiredParameters; i++) {
        pPrjPrm->p[i + startAt] = projParameters(i);
    }
    String name = Projection::name(proj.type());
    const char *nameptr = name.chars();
    int errnum = celset(nameptr, pCelPrm, pPrjPrm);

    if (errnum != 0) {
        String errmsg = "wcs celset_error: ";
        errmsg += celset_errmsg[errnum];
        throw(AipsError(errmsg));
    }
// 
// All the things we make next are needed by the toMix2 function
// We don't want to make them every time this is called
//
    pWcs = new wcsprm;
    if (!pWcs) {
        throw(AllocError("static ::make_celprm_and_prjprm()", sizeof(wcsprm)));
    }
//
// Construct FITS ctype vector
//
    Vector<String> axisNames = DirectionCoordinate::axisNames(directionType, True);
    Vector<String> ctype = make_Direction_FITS_ctype(proj, axisNames, refLat, False);
    strncpy (c_ctype[0], ctype(0).chars(), 9);
    strncpy (c_ctype[1], ctype(1).chars(), 9);
//
    c_crval[0] = refLong;
    c_crval[1] = refLat;
//
// Fill in the wcs structure
//
    int iret = wcsset(2, c_ctype, pWcs);
    if (iret!=0) {
        String errorMsg = "wcs wcsset_error: ";
        errorMsg += wcsset_errmsg[iret];
        throw(AipsError(errorMsg));
    }
}



void DirectionCoordinate::copy_celprm_and_prjprm(celprm* &pToCel, prjprm* &pToPrj,
                                                 wcsprm* &pToWcs, 
                                                 char toctype[2][9], double tocrval[2],
                                                 const celprm *pFromCel, const prjprm *pFromPrj,
                                                 const wcsprm *pFromWcs, 
                                                 const char fromctype[2][9], const double fromcrval[2]) const
//
// This function is used by the copy constructor (for which the
// output pointers will be 0) and assignment, for which the
// output pointers will be non-zero
//
// Note that none of these structures have pointers in them. Therefore
// we can copy them easily.  SOme of the wcs strcutures have pointers
// in them and deep copies must be made.
//
{
    AlwaysAssert(pFromCel!=0 && pFromPrj!=0 && pFromWcs!=0, AipsError);
//
    if (pToCel==0) pToCel = new celprm;
    if (!pToCel) {
        throw(AllocError("static ::copy_celprm_and_prjprm()", sizeof(celprm)));
    }
    *pToCel = *pFromCel;
//
    if (pToPrj==0) pToPrj = new prjprm;
    if (!pToPrj) {
        delete pToCel;
        pToCel = 0;
        throw(AllocError("static ::copy_celprm_and_prjprm()", sizeof(prjprm)));
    }
    *pToPrj = *pFromPrj;
//
    if (pToWcs==0) pToWcs = new wcsprm;
    if (!pToWcs) {
        delete pToCel;
        pToCel = 0;
        delete pToPrj;
        pToPrj = 0;
        throw(AllocError("static ::copy_celprm_and_prjprm()", sizeof(wcsprm)));
    }
    *pToWcs = *pFromWcs;
//
    strncpy (toctype[0], fromctype[0], 9);
    strncpy (toctype[1], fromctype[1], 9);
    tocrval[0] = fromcrval[0];
    tocrval[1] = fromcrval[1];
}

