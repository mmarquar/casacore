//# <ClassFileName.h>: this defines <ClassName>, which ...
//# Copyright (C) 1997
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

#include <wcslib/cel.h>
#include <wcslib/proj.h>

#include <trial/Coordinates/DirectionCoordinate.h>
#include <aips/Mathematics/Constants.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Utilities/Assert.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Containers/Record.h>

// Helper functions to help us interface to WCS

static void make_celprm_and_prjprm(celprm *&celprm_p, prjprm *&prjprm_p,
           Projection::Type proj, Double refLong, Double refLat, 
           const Vector<Double> &parameters, Double longPole, Double latPole)
{
    celprm_p = new celprm;
    if (! celprm_p) {
        throw(AllocError("static ::make_celprm_and_prjprm()", sizeof(celprm)));
    }
    celprm_p->flag = 0;
    celprm_p->ref[0] = refLong;
    celprm_p->ref[1] = refLat;
    celprm_p->ref[2] = longPole;
    celprm_p->ref[3] = latPole;

    prjprm_p = new prjprm;
    if (! prjprm_p) {
        delete celprm_p;
        celprm_p = 0;
        throw(AllocError("static ::make_celprm_and_prjprm()", sizeof(prjprm)));
    }
    prjprm_p->flag = 0;
    prjprm_p->r0 = 0.0;
    objset(prjprm_p->p, double(0.0), uInt(10));
    uInt nRequiredParameters = Projection::nParameters(proj);
    AlwaysAssert(parameters.nelements() >= nRequiredParameters, AipsError);
    int startAt = ((proj == Projection::ZPN) ? 0 : 1);
    for (uInt i=0; i < nRequiredParameters; i++) {
        prjprm_p->p[i + startAt] = parameters(i);
    }
    String name = Projection::name(proj);
    const char *nameptr = name.chars();
    int errnum = celset(nameptr, celprm_p, prjprm_p);

    if (errnum != 0) {
        String errmsg = "wcs celset_error: ";
        errmsg += celset_errmsg[errnum];
        throw(AipsError(errmsg));
    }
}

static void copy_celprm_and_prjprm(celprm *&tocel, prjprm *&toprj, Projection::Type fromproj,
				   const celprm *fromcel, const prjprm *fromprj)
{
    AlwaysAssert(fromcel != 0 && fromprj != 0, AipsError);
    if (tocel == 0) tocel = new celprm;
    if (! tocel) {
        throw(AllocError("static ::copy_celprm_and_prjprm()", sizeof(celprm)));
    }
    toprj = new prjprm;
    if (! toprj) {
        delete tocel;
        tocel = 0;
        throw(AllocError("static ::copy_celprm_and_prjprm()", sizeof(prjprm)));
    }

    *tocel = *fromcel;
    *toprj = *fromprj;
}

DirectionCoordinate::DirectionCoordinate(MDirection::Types directionType,
 			const Projection &projection,
			Double refLong, Double refLat,
			Double incLong, Double incLat,
			const Matrix<Double> &xform,
 			Double refX, Double refY)
  : type_p(directionType), projection_p(projection),
    celprm_p(0), prjprm_p(0), linear_p(1),
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

    make_celprm_and_prjprm(celprm_p, prjprm_p, projection.type(),
			   crval(0), crval(1), projection.parameters(), 
			   999.0, 999.0);
}

DirectionCoordinate::DirectionCoordinate(const DirectionCoordinate &other)
    : type_p(other.type_p), projection_p(other.projection_p),
      celprm_p(0), prjprm_p(0), linear_p(other.linear_p),
      names_p(other.names_p.copy()), units_p(other.units_p.copy())
{
    to_degrees_p[0] = other.to_degrees_p[0];
    to_degrees_p[1] = other.to_degrees_p[1];
    copy_celprm_and_prjprm(celprm_p, prjprm_p, projection_p.type(),
			   other.celprm_p, other.prjprm_p);
}


DirectionCoordinate &DirectionCoordinate::operator=(const DirectionCoordinate &other)
{
    if (this != &other) {
	type_p = other.type_p;
	projection_p = other.projection_p;
	copy_celprm_and_prjprm(celprm_p, prjprm_p, projection_p.type(),
			       other.celprm_p, other.prjprm_p);
	linear_p = other.linear_p;
	names_p = other.names_p;
	units_p = other.units_p;
	to_degrees_p[0] = other.to_degrees_p[0];
	to_degrees_p[1] = other.to_degrees_p[1];
    }
    return *this;
}

DirectionCoordinate::~DirectionCoordinate()
{
    delete celprm_p; 
    celprm_p = 0;
    delete prjprm_p; 
    prjprm_p = 0;
}

Coordinate::Type DirectionCoordinate::type() const
{
    return Coordinate::DIRECTION;
}

uInt DirectionCoordinate::nPixelAxes() const
{
    return 2;
}

uInt DirectionCoordinate::nWorldAxes() const
{
    return 2;
}

// Move out of func for exception emulation
    static String errorMsg;
Bool DirectionCoordinate::toWorld(Vector<Double> &world,
				  const Vector<Double> &pixel) const
{
    AlwaysAssert(world.nelements() == 2 &&
		 pixel.nelements() == 2, AipsError);

    Bool ok = linear_p.reverse(world, pixel, errorMsg);
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
	    errorMsg = "wcslib celrev error: ";
	    errorMsg += celrev_errmsg[errnum];
	}
    }

    if (!ok) {
	set_error(errorMsg);
    }

    toOther(world);
    return ok;
}

    static Vector<Double> tmpstatic(2);
Bool DirectionCoordinate::toPixel(Vector<Double> &pixel,
				  const Vector<Double> &world) const
{

    AlwaysAssert(world.nelements() == 2 &&
		 pixel.nelements() == 2, AipsError);

    tmpstatic(0) = world(0); tmpstatic(1) = world(1);
    toDegrees(tmpstatic);

    double x, y, phi, theta;
    double lng = tmpstatic(0), lat = tmpstatic(1);
    int errnum = celfwd(pcodes[projection_p.type()], lng, lat,
			celprm_p, &phi, &theta, prjprm_p, &x, &y);
    tmpstatic(0) = x; tmpstatic(1) = y;
    Bool ok = ToBool(errnum == 0);
    if (ok) {
	ok = linear_p.forward(tmpstatic, pixel, errorMsg);
    } else {
        errorMsg = "wcslib celfwd error: ";
        errorMsg += celfwd_errmsg[errnum];
    }

    if (! ok) {
	set_error(errorMsg);
    }

    return ok;
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
    names_p = names;
    return True;
}

Bool DirectionCoordinate::setWorldAxisUnits(const Vector<String> &units, 
					  Bool adjust)

{
    Bool ok = True;
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

    if (ok) {
	ok = ToBool(units.nelements() == 2);
	if (ok) {
	    units_p = units;
	} else {
	    set_error("Two units must be provided!");
	}
    }

    return ok;
}


Bool DirectionCoordinate::setReferencePixel(const Vector<Double> &refPix)
{
    linear_p.crpix(refPix);
    return True;
}

Bool DirectionCoordinate::setLinearTransform(const Matrix<Double> &xform)
{
    linear_p.pc(xform);
    return True;
}

Bool DirectionCoordinate::setIncrement(const Vector<Double> &inc)
{
    Bool ok = ToBool(inc.nelements() == 2);
    if (! ok) {
	set_error("increment must be a 2-vector");
    } else {
	Vector<Double> tmp(inc.copy());
	toDegrees(tmp);
	linear_p.cdelt(tmp);
    }
    return ok;
}

Bool DirectionCoordinate::setReferenceValue(const Vector<Double> &refval)
{
    Bool ok = ToBool(refval.nelements() == 2);
    if (! ok) {
	set_error("Reference value must be a 2-vector");
    } else {
	Vector<Double> tmp(refval.copy());
	toDegrees(tmp);
	celprm_p->flag = 0;
	celprm_p->ref[0] = tmp(0);
	celprm_p->ref[1] = tmp(1);
    }
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
	    names(2) = "DEC";
	    break;
	case MDirection::AZEL:
	    names(0) = "AZ";
	    names(2) = "EL";
	    break;
	default:
	    names = "????";
	}
    } else {
	names(0) = "Right Ascension";
	names(1) = "Declination";
	switch(type) {
	case MDirection::J2000: names.ac() += String(" (J2000)"); break;
	case MDirection::JMEAN: names.ac() += String(" (mean (J))"); break;
	case MDirection::APP: names.ac() += String(" (apparent)"); break;
	case MDirection::B1950: names.ac() += String(" (B1950)"); break;
	case MDirection::BMEAN: names.ac() += String(" (mean (B))"); break;
	case MDirection::BTRUE: names.ac() += String(" (true (B))"); break;
	case MDirection::GALACTIC:
	    names(0) = "Galactic Longitude";
	    names(1) = "Galactic Latitude";
	    break;
	case MDirection::HADEC:
	    names(0) = "Hour Angle";
	    names(2) = "Declination";
	    break;
	case MDirection::AZEL:
	    names(0) = "Azimuth";
	    names(2) = "Elevation";
	    break;
	default:
	    names = "Unknown";
	}
    }
    return names;
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

Bool DirectionCoordinate::save(RecordInterface &container,
			    const String &fieldName) const
{
    Bool ok = ToBool(!container.isDefined(fieldName));
    if (ok) {
	Record subrec;
	Projection proj = projection();
	String system = "unknown";
	switch (type_p) {
	case MDirection::J2000: system = "J2000"; break;
	case MDirection::JMEAN: system = "JMEAN"; break;
	case MDirection::JTRUE: system = "JTRUE"; break;
	case MDirection::APP: system = "APP"; break;
	case MDirection::B1950: system = "B1950"; break;
	case MDirection::BMEAN: system = "BMEAN"; break;
	case MDirection::BTRUE: system = "BTRUE"; break;
	case MDirection::GALACTIC: system = "GALACTIC"; break;
	case MDirection::HADEC: system = "HADEC"; break;
	case MDirection::AZEL: system = "AZEL"; break;
	}
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

DirectionCoordinate *DirectionCoordinate::restore(const RecordInterface &container,
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
    if (system == "J2000") {
	sys = MDirection::J2000;
    } else if (system == "JMEAN") {
	sys = MDirection::JMEAN;
    } else if (system == "JTRUE") {
	sys = MDirection::JTRUE;
    } else if (system == "APP") {
	sys = MDirection::APP;
    } else if (system == "B1950") {
	sys = MDirection::B1950;
    } else if (system == "BMEAN") {
	sys = MDirection::BMEAN;
    } else if (system == "BTRUE") {
	sys = MDirection::BTRUE;
    } else if (system == "GALACTIC") {
	sys = MDirection::GALACTIC;
    } else if (system == "HADEC") {
	sys = MDirection::HADEC;
    } else if (system == "AZEL") {
	sys = MDirection::AZEL;
    } else {
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
