//# LinearCoordinate.cc: this defines LinearCoordinate
//# Copyright (C) 1997,1998,1999,2000,2001,2003
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

#include <trial/Coordinates/LinearCoordinate.h>

#include <aips/Exceptions/Error.h>
#include <aips/Utilities/Assert.h>
#include <aips/Utilities/LinearSearch.h>
#include <aips/Utilities/Regex.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Containers/Record.h>
#include <aips/Mathematics/Math.h>
#include <aips/Quanta/Quantum.h>

#include <aips/sstream.h>

LinearCoordinate::LinearCoordinate(uInt naxis)
: Coordinate(),
  transform_p(naxis), 
  names_p(naxis), 
  units_p(naxis), 
  prefUnits_p(naxis), 
  crval_p(naxis)
{
    crval_p.set(0.0);
    setDefaultWorldMixRanges();
}

LinearCoordinate::LinearCoordinate(const Vector<String>& names,
				   const Vector<String>& units,
				   const Vector<Double>& refVal,
				   const Vector<Double>& inc,
				   const Matrix<Double>& pc,
				   const Vector<Double>& refPix)
: Coordinate(),
  transform_p(refPix, inc, pc), 
  names_p(names.nelements()),
  units_p(names.nelements()), 
  prefUnits_p(names.nelements()), 
  crval_p(names.nelements())
{
    uInt naxis = names.nelements();
    names_p = names;
    units_p = units;
    AlwaysAssert(units.nelements() == naxis &&
		 refVal.nelements() == naxis &&
		 inc.nelements() == naxis &&
		 pc.nrow() == naxis &&
		 pc.ncolumn() == naxis &&
		 refPix.nelements() == naxis, AipsError);
    for (uInt i=0; i<naxis; i++) {
	crval_p[i] = refVal(i);
    }
//
    setDefaultWorldMixRanges();
}


LinearCoordinate::LinearCoordinate(const Vector<String>& names,
                                   const Vector<Quantum<Double> >& refVal,
                                   const Vector<Quantum<Double> >& inc,   
                                   const Matrix<Double>& pc,
                                   const Vector<Double>& refPix)
: Coordinate(),
  names_p(names.nelements()),
  units_p(names.nelements()), 
  prefUnits_p(names.nelements()), 
  crval_p(names.nelements())
{
// Check dimensions

    const uInt n = names.nelements();
    AlwaysAssert(refVal.nelements() == n &&
		 inc.nelements() == n &&
		 pc.nrow() == n &&
		 pc.ncolumn() == n &&
		 refPix.nelements() == n, AipsError);
//

    Vector<Double> cdelt(n);
//
    for (uInt i=0; i<n; i++) {
       if (refVal(i).isConform(inc(i))) {

// Assign 

          names_p(i) = names(i);
          units_p(i) = refVal(i).getUnit();
  	  crval_p[i] = refVal(i).getValue();

// Convert inc to units of refVal

          cdelt(i) = inc(i).getValue(Unit(units_p(i)));
       } else {
          throw (AipsError("Units of reference value and increment inconsistent"));
       }
    }
//
    transform_p = LinearXform(refPix, cdelt, pc);
//
    setDefaultWorldMixRanges();
}

LinearCoordinate::LinearCoordinate(const LinearCoordinate &other)
: Coordinate(other),
  transform_p(other.transform_p), 
  names_p(other.names_p.copy()),
  units_p(other.units_p.copy()), 
  prefUnits_p(other.prefUnits_p.copy()), 
  worldMin_p(other.worldMin_p.copy()),
  worldMax_p(other.worldMax_p.copy()),
  crval_p(other.crval_p)
{
    // Nothing
}

LinearCoordinate &LinearCoordinate::operator=(const LinearCoordinate &other)
{
    if (this != &other) {
        Coordinate::operator=(other);
//
	uInt naxis = other.nWorldAxes();
	names_p.resize(naxis); 
        names_p = other.names_p;
	units_p.resize(naxis); 
        units_p = other.units_p;
	prefUnits_p.resize(naxis); 
        prefUnits_p = other.prefUnits_p;
	crval_p = other.crval_p;
	transform_p = other.transform_p;
        worldMin_p.resize(naxis);
        worldMin_p = other.worldMin_p;
        worldMax_p.resize(naxis);
        worldMax_p = other.worldMax_p;
    }
    return *this;
}

LinearCoordinate::~LinearCoordinate()
{}



Coordinate::Type LinearCoordinate::type() const
{
    return Coordinate::LINEAR;
}

String LinearCoordinate::showType() const
{
    return String("Linear");
}

uInt LinearCoordinate::nPixelAxes() const
{
    return names_p.nelements();
}

uInt LinearCoordinate::nWorldAxes() const
{
    return nPixelAxes();
}

Bool LinearCoordinate::toWorld(Vector<Double> &world, 
			       const Vector<Double> &pixel) const
{
   static String errorMsg;
   uInt n = nPixelAxes();             // nWorldAxes == nPixelAxes 
   AlwaysAssert(pixel.nelements()==n, AipsError);
   world.resize(n);
//
   if (!transform_p.reverse(world, pixel, errorMsg)) {
      set_error(errorMsg);
      return False;
   } else {
      for (uInt i=0; i<n; i++) {
         world(i) += crval_p[i];
      }
   }
   return True;
}

Bool LinearCoordinate::toPixel(Vector<Double> &pixel, 
			       const Vector<Double> &world) const
{
   static String errorMsg;
   static Vector<Double> offset;
   uInt n = nPixelAxes();             // nWorldAxes == nPixelAxes 
   AlwaysAssert(world.nelements()==n, AipsError);
   pixel.resize(n);
//
   offset.resize(n);
   for (uInt i=0; i<n; i++) {
      offset(i) = world(i) - crval_p[i];
   }
   if (!transform_p.forward(pixel, offset, errorMsg)) {
      set_error(errorMsg);
      return False;
   }
   return True;
}

Vector<String> LinearCoordinate::worldAxisNames() const
{
    return names_p;
}

Vector<String> LinearCoordinate::worldAxisUnits() const
{
    return units_p;
}

Vector<Double> LinearCoordinate::referenceValue() const
{
    return Vector<Double>(crval_p);
}

Vector<Double> LinearCoordinate::increment() const
{
    return transform_p.cdelt();
}

Matrix<Double> LinearCoordinate::linearTransform() const
{
    return transform_p.pc();
}

Vector<Double> LinearCoordinate::referencePixel() const
{
    return transform_p.crpix();
}

Bool LinearCoordinate::setWorldAxisNames(const Vector<String> &names)
{
    Bool ok = (names.nelements() == nWorldAxes());
    if (!ok) {
       set_error("names vector has the wrong size");
    } else {
       names_p = names;
    }

    return ok;
}

Bool LinearCoordinate::setWorldAxisUnits(const Vector<String> &units)
{
    Vector<Double> d1 = increment();
    Bool ok = Coordinate::setWorldAxisUnits(units);
    if (ok) {
	ok = (units.nelements() == nWorldAxes());
	if (ok) {
	    units_p = units;

// The factor is hidden in Coordinate...
           
            Vector<Double> d2 = increment();
            worldMin_p *= d2 / d1;
            worldMax_p *= d2 / d1;
	}
    }
    return ok;
}


Bool LinearCoordinate::overwriteWorldAxisUnits(const Vector<String> &units)
{
   Bool ok = units.nelements() == nWorldAxes();
   if (ok) {
      units_p = units;
   } else {
      set_error ("units vector has the wrong size");
   }
   return ok;
}

Bool LinearCoordinate::setPreferredWorldAxisUnits (const Vector<String>& units)
{
    if (!Coordinate::setPreferredWorldAxisUnits(units)) return False;
//
    prefUnits_p = units;
    return True;
}

Vector<String> LinearCoordinate::preferredWorldAxisUnits () const
{
   return prefUnits_p;
}

Bool LinearCoordinate::setReferencePixel(const Vector<Double> &refPix)
{
    Bool ok = (refPix.nelements() == nWorldAxes());
    if (! ok) {
	set_error("reference pixel vector has the wrong size");
    } else {
	transform_p.crpix(refPix);
    }

    return ok;
}

Bool LinearCoordinate::setLinearTransform(const Matrix<Double> &pc)
{
    Bool ok = (pc.nrow() == nWorldAxes() && 
		     pc.ncolumn() == nWorldAxes() );
    if (! ok) {
	set_error("Transform matrix has the wrong size");
    } else {
	transform_p.pc(pc);
    }

    return ok;
}

Bool LinearCoordinate::setIncrement(const Vector<Double> &inc)
{
    Bool ok = (inc.nelements() == nWorldAxes());
    if (! ok) {
	set_error("increment vector has the wrong size");
    } else {
	transform_p.cdelt(inc);
    }

    return ok;
}

Bool LinearCoordinate::setReferenceValue(const Vector<Double> &refval)
{
    Bool ok = (refval.nelements() == nWorldAxes());
    if (! ok) {
	set_error("reference value vector has the wrong size");
    } else {
	refval.toBlock(crval_p);
    }

    return ok;
}


Bool LinearCoordinate::near(const Coordinate& other,
                            Double tol) const
{
   Vector<Int> excludeAxes;
   return near(other, excludeAxes, tol);
}


Bool LinearCoordinate::near(const Coordinate& other,
                            const Vector<Int>& excludeAxes,
                            Double tol) const
{
   if (other.type() != this->type()) {
      set_error("Comparison is not with another LinearCoordinate");
      return False;
   }

   const LinearCoordinate& lCoord = dynamic_cast<const LinearCoordinate&>(other);


// Check descriptor vector lengths

   if (names_p.nelements() != lCoord.names_p.nelements()) {
      set_error("The LinearCoordinates have differing numbers of world axis names");
      return False;
   }
   if (units_p.nelements() != lCoord.units_p.nelements()) {
      set_error("The LinearCoordinates have differing numbers of axis units");
      return False;
   }
   if (crval_p.nelements() != lCoord.crval_p.nelements()) {
      set_error("The LinearCoordinates have differing numbers of reference values");
      return False;
   }


// Number of pixel and world axes is the same for a LinearCoordinate
// Add an assertion check should this change.  Other code in LC has 
// checked that all the vectors we are comparing have the same 
// length as nPixelAxes()

   AlwaysAssert(nPixelAxes()==nWorldAxes(), AipsError);
   Vector<Bool> exclude(nPixelAxes());
   exclude = False;
   Bool found;
   uInt j = 0;
   uInt i;
   for (i=0; i<nPixelAxes(); i++) {
      if (linearSearch(found, excludeAxes, Int(i), excludeAxes.nelements()) >= 0)
        exclude(j++) = True;
    }

// Check the descriptors

   ostringstream oss;
   for (i=0; i<names_p.nelements(); i++) {
      if (!exclude(i)) {
//
// the to/from FITS header conversion will convert linear axis
// names to upper case.  So to prevent that reflection failing,
// test on upper case only.  Also we need to strip off
// trialing white space (FITS length will be 8 chars)
//
         {
           String x1 = names_p(i);
           x1.upcase();
           String x2 = lCoord.names_p(i);
           x2.upcase();
//
           Int i1 = x1.index(RXwhite,0);
           if (i1==-1) i1 = x1.length();
           Int i2 = x2.index(RXwhite,0);
           if (i2==-1) i2 = x2.length();
//
           String y1 = String(x1.before(i1));
           String y2 = String(x2.before(i2));
//
           if (y1 != y2) {
              oss << "The LinearCoordinates have differing axis names for axis "
                  << i;
              set_error(String(oss));
              return False;
           }
        }
      }
   }
   for (i=0; i<units_p.nelements(); i++) {
      if (!exclude(i)) {

// This is bad.  After reading from FITS, the units are upper case.  
// So we cannot distinguish between a unit which is the same letter
// but case different if its been throgh FITS...

         {
           String x1 = units_p(i);
           x1.upcase();
           String x2 = lCoord.units_p(i);
           x2.upcase();
//
           Int i1 = x1.index(RXwhite,0);
           if (i1==-1) i1 = x1.length();
           Int i2 = x2.index(RXwhite,0);
           if (i2==-1) i2 = x2.length();
//
           String y1 = String(x1.before(i1));
           String y2 = String(x2.before(i2));
           if (y1 != y2) {
             oss << "The LinearCoordinates have differing axis units for axis "
                 << i;
             set_error(String(oss));
             return False;
           }
        }
      }
   }
   for (i=0; i<crval_p.nelements(); i++) {
      if (!exclude(i)) {
         if (!::near(crval_p[i],lCoord.crval_p[i],tol)) {
            oss << "The LinearCoordinates have differing reference values for axis "
                << i;
            set_error(String(oss));
            return False;
         }
      }
   }


// Check the linear transform

   if (!transform_p.near(lCoord.transform_p,excludeAxes,tol)) {
      set_error("The LinearCoordinates have differing linear transformation matrices");
      return False;
   }

   return True;
}


Bool LinearCoordinate::save(RecordInterface &container,
			    const String &fieldName) const
{
    Bool ok = (!container.isDefined(fieldName));
    if (ok) {
	Record subrec;
	subrec.define("crval", referenceValue());
	subrec.define("crpix", referencePixel());
	subrec.define("cdelt", increment());
	subrec.define("pc", linearTransform());
	subrec.define("axes", worldAxisNames());
	subrec.define("units", worldAxisUnits());
	subrec.define("preferredunits", preferredWorldAxisUnits());

	container.defineRecord(fieldName, subrec);
    }
    return ok;
}

LinearCoordinate *LinearCoordinate::restore(const RecordInterface &container,
					   const String &fieldName)
{
    if (! container.isDefined(fieldName)) {
	return 0;
    }

    Record subrec(container.asRecord(fieldName));
    
    // We should probably do more type-checking as well as checking
    // for existence of the fields.
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

    LinearCoordinate *retval = 
	new LinearCoordinate(axes, units, crval, cdelt, pc, crpix);
    AlwaysAssert(retval, AipsError);
//
    if (subrec.isDefined("preferredunits")) {                // optional
       Vector<String> prefUnits;
       subrec.get("preferredunits", prefUnits);
       retval->setPreferredWorldAxisUnits(prefUnits);
    }
//							  
    return retval;
}



Coordinate *LinearCoordinate::clone() const
{
    return new LinearCoordinate(*this);
}




Coordinate* LinearCoordinate::makeFourierCoordinate (const Vector<Bool>& axes,
                                                     const Vector<Int>& shape) const
//        
// axes says which axes in the coordinate are to be transformed
// shape is the shape of the image for all axes in this coordinate 
//
{
   if (axes.nelements() != nPixelAxes()) {
      throw (AipsError("Invalid number of specified axes"));
   }
   uInt nT = 0;
   for (uInt i=0; i<nPixelAxes(); i++) if (axes(i)) nT++;
   if (nT==0) {
      throw (AipsError("You have not specified any axes to transform"));
   }
//
   if (shape.nelements() != nPixelAxes()) {
      throw (AipsError("Invalid number of elements in shape"));
   }

// Find the canonical input units that we should convert to.
// Find the Fourier coordinate names and units

   Vector<String> units(worldAxisUnits());
   Vector<String> unitsCanon(worldAxisUnits().copy());
   Vector<String> unitsOut = worldAxisUnits().copy();
   Vector<String> names(worldAxisNames());
   Vector<String> namesOut(worldAxisNames().copy());
   Vector<Double> crval(referenceValue());
   Vector<Double> crpix(referencePixel());
   Vector<Double> scale(nPixelAxes(), 1.0);
//
   for (uInt i=0; i<nPixelAxes(); i++) {
      if (axes(i)) {
         crval(i) = 0.0;
         fourierUnits(namesOut(i), unitsOut(i), unitsCanon(i), Coordinate::LINEAR, 
                      i, units(i), names(i));
         scale(i) = 1.0 / Double(shape(i));
         crpix(i) = Int(shape(i) / 2);
      }
   }

// Make a copy of ourselves so we can change the units.  Otherwise we
// could make this function non-const and then put it back

   LinearCoordinate lc = *this;
   if (!lc.setWorldAxisUnits(unitsCanon)) {
      throw(AipsError("Could not set world axis units"));
   }

// Now create the new LinearCoordinate, using the LinearXform class
// to invert the coordinate matrices

   const LinearXform& linear = lc.transform_p;
   const LinearXform linear2 = linear.fourierInvert(axes, crpix, scale);
//
   return new LinearCoordinate(namesOut, unitsOut, crval, linear2.cdelt(),
                               linear2.pc(), linear2.crpix());
}

Bool LinearCoordinate::setWorldMixRanges (const IPosition& shape)
{
   return Coordinate::setWorldMixRanges (worldMin_p, worldMax_p, shape);
}


void LinearCoordinate::setDefaultWorldMixRanges ()
{
   Coordinate::setDefaultWorldMixRanges (worldMin_p, worldMax_p);
}

