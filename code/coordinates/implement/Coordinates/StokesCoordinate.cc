//# StokesCoordinate.cc: this defines StokesCoordinate which shoe-horns Stokes axes into a Coordinate
//# Copyright (C) 1997,1998,1999,2000
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

#include <trial/Coordinates/StokesCoordinate.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/Matrix.h>

#include <aips/Containers/Record.h>
#include <aips/Utilities/Assert.h>
#include <aips/Utilities/LinearSearch.h>
#include <aips/Mathematics/Math.h>

#include <strstream.h>

StokesCoordinate::StokesCoordinate(const Vector<Int> &whichStokes)
    : Coordinate(),
      values_p(whichStokes.nelements()), crval_p(0), crpix_p(0), matrix_p(1),
      cdelt_p(1), name_p("Stokes"), unit_p("")
{
    AlwaysAssert(whichStokes.nelements()>0, AipsError);
//
    Block<Bool> alreadyUsed(Stokes::NumberOfTypes);
    if (whichStokes.nelements() > 0) {
	crval_p = whichStokes(0);
    }
    whichStokes.toBlock(values_p);
    // Make sure the stokes occur at most once
    alreadyUsed = False;
    for (uInt i=0; i<values_p.nelements(); i++) {
	AlwaysAssert(Stokes::type(values_p[i]) != Stokes::Undefined, AipsError);
	AlwaysAssert(!alreadyUsed[whichStokes(i)], AipsError);
	alreadyUsed[whichStokes(i)] = True;
    }
}

StokesCoordinate::StokesCoordinate(const StokesCoordinate &other)
: Coordinate(other),
  values_p(other.values_p),
  crval_p(other.crval_p),
  crpix_p(other.crpix_p),
  matrix_p(other.matrix_p),
  cdelt_p(other.cdelt_p),
  name_p(other.name_p),
  unit_p(other.unit_p)
{  
}

StokesCoordinate &StokesCoordinate::operator=(const StokesCoordinate &other)
{
    if (this != &other) {
        Coordinate::operator=(other);
	values_p = other.values_p;
	crval_p = other.crval_p;
	crpix_p = other.crpix_p;
	matrix_p = other.matrix_p;
	cdelt_p = other.cdelt_p;
	name_p = other.name_p;
	unit_p = other.unit_p;
    }

    return *this;
}

StokesCoordinate::~StokesCoordinate()
{
    // Nothing
}

Coordinate::Type StokesCoordinate::type() const
{
    return Coordinate::STOKES;
}

String StokesCoordinate::showType() const
{
    return String("Stokes");
}

uInt StokesCoordinate::nPixelAxes() const
{
    return 1;
}

uInt StokesCoordinate::nWorldAxes() const
{
    return 1;
}

Bool StokesCoordinate::toWorld(Stokes::StokesTypes &stokes, Int pixel) const
{
// Avoid going to the Vector version for efficiency reasons.

    Double index = floor((pixel - crpix_p)*cdelt_p*matrix_p + 0.5);
    Double nValues = Double(values_p.nelements());
    if (index >= 0.0 && index < nValues) {
	stokes = Stokes::type(values_p[Int(index)]);
	return True;
    } else {
	ostrstream os;
	os << "Index value of " << index << " is out of range [0.." <<
	    values_p.nelements()-1 << "]";
	set_error(os);
	return False;
    }
}

Bool StokesCoordinate::toPixel(Int &pixel, Stokes::StokesTypes stokes) const
{
    Bool found = False;
    uInt index;
    for (index=0; index<values_p.nelements(); index++) {
	if (values_p[index] == Int(stokes)) {
	    found = True;
	    break;
	}
    }
    if (!found) {
	ostrstream os;
        os << "Stokes value " << Stokes::name(stokes) <<
	    " is not found on any axis position";
	set_error(os);
	return False;
    }
    
    pixel = Int(floor((Double(index))/cdelt_p/matrix_p + 
		      crpix_p + 0.5));
    return True;
}

Bool StokesCoordinate::toWorld(Vector<Double> &world, 
			       const Vector<Double> &pixel) const
{
    AlwaysAssert(pixel.nelements()==1, AipsError);
    if (world.nelements()!=1) world.resize(1);
    Double index = floor((pixel(0) - crpix_p)*cdelt_p*matrix_p + 0.5);
    Double nValues = Double(values_p.nelements()) - 1.0;
    if (index < 0.0 || index > nValues) {
	ostrstream os;
	os << "Index value of " << index << " is out of range [0.." <<
	    values_p.nelements()-1 << "]";
	set_error(os);
	return False;
    }
    world(0) = values_p[Int(index)];

    return True;
}

Bool StokesCoordinate::toPixel(Vector<Double> &pixel, 
    	                       const Vector<Double> &world) const
{
    AlwaysAssert(world.nelements()==1, AipsError);
    if (pixel.nelements()!=1) pixel.resize(1);
//
    // First find the index;
    Bool found = False;
    uInt index;
    for (index=0; index<values_p.nelements(); index++) {
	found = ::near(world(0), Double(values_p[index]));
	if (found) {
	    break;
	}
    }
    if (!found) {
	ostrstream os;
	os << "Stokes value " << world(0) << 
	    " is not found on any axis position";
	set_error(os);
	return False;
    }

    // OK, we found it, compute pixel position
    pixel(0) = (Double(index))/cdelt_p/matrix_p + crpix_p;
    
    return True;
}

Vector<Int> StokesCoordinate::stokes() const
{
    return Vector<Int>(values_p);
}


Vector<String> StokesCoordinate::worldAxisNames() const
{
    Vector<String> names(1);
    names = name_p;
    return names;
}

Vector<String> StokesCoordinate::worldAxisUnits() const
{
    Vector<String> units(1);
    units = unit_p;
    return units;
}

Vector<Double> StokesCoordinate::referencePixel() const
{
    Vector<Double> crpix(1);
    crpix = crpix_p;
    return crpix;
}

Matrix<Double> StokesCoordinate::linearTransform() const
{
    Matrix<Double> matrix(1,1);
    matrix(0,0) = matrix_p;
    return matrix;
}

Vector<Double> StokesCoordinate::increment() const
{
    Vector<Double> cdelt(1);
    cdelt = cdelt_p;
    return cdelt;
}

Vector<Double> StokesCoordinate::referenceValue() const
{
    Vector<Double> crval(1);
    crval = crval_p;
    return crval;
}

Bool StokesCoordinate::setWorldAxisNames(const Vector<String> &names)
{
    Bool ok = ToBool(names.nelements()==1);
    if (!ok) {
       set_error ("names vector must be of length 1");
    } else {
       name_p = names(0);
    }
    return ok;
}

Bool StokesCoordinate::setWorldAxisUnits(const Vector<String> &units)
{
    Bool ok = ToBool(units.nelements()==1);
    if (!ok) {
       set_error ("units vector must be of length 1");
    } else {
       if (units(0) == "") return True;
    }
    return ok;
}


Bool StokesCoordinate::setReferencePixel(const Vector<Double> &refPix)
{
    Bool ok = ToBool(refPix.nelements()==1);
    if (!ok) {
       set_error ("reference pixel vector must be of length 1");
    } else {
       crpix_p = refPix(0);
    }
    return ok;
}

Bool StokesCoordinate::setLinearTransform(const Matrix<Double> &xform)
{
    Bool ok = ToBool(xform.nelements()==1);
    if (!ok) {
       set_error ("linear transform matrix must be of length 1");
    } else {
       matrix_p = xform(0,0);
    }
    return ok;
}

Bool StokesCoordinate::setIncrement(const Vector<Double> &inc) 
{
    AlwaysAssert(inc.nelements() == 1, AipsError);
    cdelt_p = inc(0);
    return True;
}

Bool StokesCoordinate::setReferenceValue(const Vector<Double> &refval)
{
    Bool ok = ToBool(refval.nelements()==1);
    if (!ok) {
       set_error ("reference value vector must be of length 1");
    } else {
       crval_p = refval(0);
    }
    return ok;
}


Bool StokesCoordinate::near(const Coordinate& other,
                            Double tol) const
{
   Vector<Int> excludeAxes;
   return near(other, excludeAxes, tol);
}

Bool StokesCoordinate::near(const Coordinate& other,
                            const Vector<Int>& excludeAxes,
                            Double tol) const
{
   if (other.type() != this->type()) {
      set_error("Comparison is not with another StokesCoordinate");
      return False;
   }

   const StokesCoordinate& sCoord = dynamic_cast<const StokesCoordinate&>(other);

// Check name and units

   if (name_p != sCoord.name_p) {
      set_error("The StokesCoordinates have differing world axis names");
      return False;
   }
   if (unit_p != sCoord.unit_p) {
      set_error("The StokesCoordinates have differing axis units");
      return False;
   }


// Number of pixel and world axes is the same for a StokesCoordinate
// and it always 1.   SO if excludeAxes contains "0" we are done.
// Add an assertion check should this change 
 
   AlwaysAssert(nPixelAxes()==nWorldAxes(), AipsError);
   AlwaysAssert(nPixelAxes()==1, AipsError);
   Bool found;
   if (linearSearch(found, excludeAxes, 0, excludeAxes.nelements()) >= 0)
      return True;


// The only other thing that really matters in the STokesCoordinate
// is the values along the axis.    Nothing else (e.g. crval_p etc)
// is ever actually used.

   for (Int i=0; i<Int(values_p.nelements()); i++) {
      if (!::near(values_p[i],sCoord.values_p[i],tol)) {
         set_error("The StokesCoordinates have different Stokes values on the axis");
         return False;
      }
   }
 
   return True;
}


Bool StokesCoordinate::save(RecordInterface &container,
			    const String &fieldName) const
{
    Bool ok = ToBool(!container.isDefined(fieldName));
    if (ok) {
	Record subrec;
	subrec.define("crval", referenceValue());
	subrec.define("crpix", referencePixel());
	subrec.define("cdelt", increment());
	subrec.define("pc", linearTransform());
	subrec.define("axes", worldAxisNames());
	// subrec.define("units", worldAxisUnits()); // it never makes sense to set the units

	Vector<String> stokes(values_p.nelements());
	for (uInt i=0; i<stokes.nelements(); i++) {
	    Int dummy = values_p[i];
	    stokes(i) = Stokes::name((Stokes::StokesTypes)dummy);
	}
	subrec.define("stokes", stokes);

	container.defineRecord(fieldName, subrec);
    }
    return ok;
}

StokesCoordinate *StokesCoordinate::restore(const RecordInterface &container,
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

    
// ignore units for stokes axes as there are none of sense.  The
// StokesCoordinate is not constructed with units
// and only accepts blank Strings in setWorldAxisUnits
//
//     if (!subrec.isDefined("units")) {
// 	return 0;
//     }
//     Vector<String> units;
//     subrec.get("units", units);

    if (!subrec.isDefined("stokes")) {
	return 0;
    }
    Vector<String> stokes;
    subrec.get("stokes", stokes);
    Vector<Int> istokes(stokes.nelements());
    for (uInt i=0; i<istokes.nelements(); i++) {
	istokes(i) = Stokes::type(stokes(i));
    }
    

    StokesCoordinate *retval = new StokesCoordinate(istokes);
    AlwaysAssert(retval, AipsError);

    // retval->setWorldAxisUnits(units); // It never makes sense to set the units
    retval->setWorldAxisNames(axes);
    retval-> setIncrement(cdelt);
    retval->setReferenceValue(crval);
    retval->setReferencePixel(crpix);

    AlwaysAssert(retval, AipsError);
							  
    return retval;
}

Coordinate *StokesCoordinate::clone() const
{
    return new StokesCoordinate(*this);
}

String StokesCoordinate::format(String& units,
                                Coordinate::formatType,
                                Double worldValue,
                                uInt worldAxis,
                                Bool, Int, Bool) const
{
   units = worldAxisUnits()(worldAxis);
//
// This is very hard work !
//
   Stokes::StokesTypes sType;
   Vector<Double> pixel(1), world(1);
   world(0) = worldValue;
   Bool ok = toPixel(pixel, world);
   Int pixel2 = Int(pixel(0)+0.5);
   ok = toWorld(sType, pixel2);
//
   return Stokes::name(sType);
}
