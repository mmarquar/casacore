//# TabularCoordinate.cc: Table lookup 1-D coordinate, with interpolation
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

#include <trial/Coordinates/TabularCoordinate.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Utilities/Assert.h>
#include <aips/Functionals/Interpolate1D.h>
#include <aips/Functionals/ScalarSampledFunctional.h>
#include <aips/Containers/Record.h>
#include <aips/Logging/LogIO.h>
#include <aips/Logging/LogOrigin.h>
#include <trial/FITS/FITSUtil.h>

#if defined(__GNUG__)
typedef Interpolate1D<Double,Double> gpp_bug;
#endif

TabularCoordinate::TabularCoordinate()
  : crval_p(0), cdelt_p(1), crpix_p(0), matrix_p(1.0), unit_p(""),
    name_p("Tabular"), channel_corrector_p(0), channel_corrector_rev_p(0)
{
    // Nothing
}

TabularCoordinate::TabularCoordinate(Double refval, Double inc, Double refpix,
				     const String &unit, const String &axisName)
  : crval_p(refval), cdelt_p(inc), crpix_p(refpix), matrix_p(1.0), unit_p(unit),
    name_p(axisName), channel_corrector_p(0), channel_corrector_rev_p(0)
{
    // Nothing
}

TabularCoordinate::TabularCoordinate(const Vector<Double> &pixelValues,
				     const Vector<Double> &worldValues,
				     const String &unit, const String &axisName)
    : crval_p(0.0), cdelt_p(0.0), crpix_p(0.0), matrix_p(0.0), unit_p(unit), 
      name_p(axisName), channel_corrector_p(0), channel_corrector_rev_p(0)
{
    const uInt n = pixelValues.nelements();

    if (n <= 1 || n != worldValues.nelements()) {
	throw(AipsError("TabularCoordinate::TabularCoordinate - illegal table "
			"(length 0 or 1 or n(pixelvalues) != n(worldvalues)"));
    }
    if (pixelValues(n-1) - pixelValues(0) == 0) {
	throw(AipsError("TabularCoordinate::TabularCoordinate - illegal table "
			"first and last pixel values are the same"));
    }


    // Work out "global" crval etc.
    crval_p = worldValues(0);
    crpix_p = pixelValues(0);
    cdelt_p = (worldValues(n-1) - worldValues(0)) /
              (pixelValues(n-1) - pixelValues(0));
    matrix_p = 1.0;

    if (cdelt_p == 0.0) {
        throw(AipsError("TabularCoordinate - start and "
                        "end values in table must differ"));
    }

    Double signworld = ((worldValues(n-1) - worldValues(0))  > 0 ? 1.0 : -1.0);
    Double signpixel = ((pixelValues(n-1) - pixelValues(0))  > 0 ? 1.0 : -1.0);

    // Check that the pixel values and values monotonically increase or decrease
    // and if so, work out the difference between the actual supplied pixel and
    // the "average" pixel value.
    Vector<Double> averagePixel(n);
    for (uInt i=0; i<n; i++) {
	if (i>1) {
	    Double diffworld = signworld*(worldValues(i) - worldValues(i-1));
	    Double diffpixel = signpixel*(pixelValues(i) - pixelValues(i-1));
	    if (diffworld <= 0 || diffpixel <= 0) {
		throw(AipsError("TabularCoordinate - pixel and world values "
				"must increase or decrease monotonically"));
	    }
	}
	averagePixel(i) = (worldValues(i) - crval_p)/cdelt_p + crpix_p;
    }

    ScalarSampledFunctional<Double> in(pixelValues), avg(averagePixel);
    channel_corrector_p = 
	new Interpolate1D<Double,Double>(in, avg, True, True);
    channel_corrector_rev_p = 
	new Interpolate1D<Double,Double>(avg, in, True, True);
    AlwaysAssert(channel_corrector_p != 0 && channel_corrector_rev_p != 0,
		 AipsError);

    channel_corrector_p->setMethod(Interpolate1D<Double,Double>::linear);
    channel_corrector_rev_p->setMethod(Interpolate1D<Double,Double>::linear);
}

void TabularCoordinate::clear_self()
{
    crval_p = cdelt_p = crpix_p = matrix_p = -999.0;
    unit_p = "UNSET";
    name_p = "UNSET";
    if (channel_corrector_p) {
	delete channel_corrector_p;
	delete channel_corrector_rev_p;
    }
    channel_corrector_p = channel_corrector_rev_p = 0;
}

void TabularCoordinate::copy(const TabularCoordinate &other)
{
    if (this == &other) {
	return; // short circuit
    }

    clear_self();
    crval_p = other.crval_p;
    cdelt_p = other.cdelt_p;
    crpix_p = other.crpix_p;
    unit_p = other.unit_p;
    name_p = other.name_p;
    matrix_p = other.matrix_p;
    if (other.channel_corrector_p != 0) {
	channel_corrector_p = 
	    new Interpolate1D<Double,Double>(*other.channel_corrector_p);
	channel_corrector_rev_p = 
	    new Interpolate1D<Double,Double>(*other.channel_corrector_rev_p);
	AlwaysAssert(channel_corrector_p != 0 &&
		     channel_corrector_rev_p != 0, AipsError);
    }
}

TabularCoordinate::TabularCoordinate(const TabularCoordinate &other)
    : crval_p(0.0), cdelt_p(0.0), crpix_p(0.0), matrix_p(0.0), unit_p("UNSET"),
      name_p("UNSET"), channel_corrector_p(0), channel_corrector_rev_p(0)
{
    copy(other);
}

TabularCoordinate &TabularCoordinate::operator=(const TabularCoordinate &other)
{
    if (this != &other) {
	copy(other);
    }
    return *this;
}

TabularCoordinate::~TabularCoordinate()
{
    clear_self();
}

Coordinate::Type TabularCoordinate::type() const
{
    return Coordinate::TABULAR;
}

uInt TabularCoordinate::nPixelAxes() const
{
    return 1;
}

uInt TabularCoordinate::nWorldAxes() const
{
    return 1;
}

Bool TabularCoordinate::toWorld(Double &world, Double pixel) const
{
    if (channel_corrector_p) {
	pixel = (*channel_corrector_p)(pixel);
    }
    world = crval_p + cdelt_p * matrix_p * (pixel - crpix_p);
    return True;
}

Bool TabularCoordinate::toPixel(Double &pixel, Double world) const
{
    pixel = (world - crval_p)/(cdelt_p * matrix_p) + crpix_p;
    if (channel_corrector_rev_p) {
	pixel = (*channel_corrector_rev_p)(pixel);
    }
    return True;
}

Bool TabularCoordinate::toWorld(Vector<Double> &world, 
		     const Vector<Double> &pixel) const
{
    if (world.nelements() != 1 || world.nelements() != pixel.nelements()) {
	throw(AipsError("TabularCoordinate: world and pixel vectors must be "
			"length 1"));
    }
    return toWorld(world(0), pixel(0));
}

Bool TabularCoordinate::toPixel(Vector<Double> &pixel, 
		     const Vector<Double> &world) const
{
    if (world.nelements() != 1 || world.nelements() != pixel.nelements()) {
	throw(AipsError("TabularCoordinate: world and pixel vectors must be "
			"length 1"));
    }
    return toPixel(pixel(0), world(0));
}

Vector<String> TabularCoordinate::worldAxisNames() const
{
    Vector<String> tmp(1);
    tmp(0) = name_p;
    return tmp;
}

Vector<String> TabularCoordinate::worldAxisUnits() const
{
    Vector<String> tmp(1);
    tmp(0) = unit_p;
    return tmp;
}

Vector<Double> TabularCoordinate::referencePixel() const
{
    Vector<Double> tmp(1);
    tmp(0) = crpix_p;
    return tmp;
}

Vector<Double> TabularCoordinate::referenceValue() const
{
    Vector<Double> tmp(1);
    tmp(0) = crval_p;
    return tmp;
}

Vector<Double> TabularCoordinate::increment() const
{
    Vector<Double> tmp(1);
    tmp(0) = cdelt_p;
    return tmp;
}

Matrix<Double> TabularCoordinate::linearTransform() const
{
    Matrix<Double> tmp(1,1);
    tmp(0,0) = matrix_p;
    return tmp;
}

Bool TabularCoordinate::setWorldAxisNames(const Vector<String> &names)
{
    AlwaysAssert(names.nelements() == 1, AipsError);
    name_p = names(0);
    return True;
}

Bool TabularCoordinate::setWorldAxisUnits(const Vector<String> &units,
					   Bool adjust)
{
    AlwaysAssert(units.nelements() == 1, AipsError);
    Bool ok = Coordinate::setWorldAxisUnits(units, adjust);
    if (ok) {
	unit_p = units(0);
    }
    return ok;
}


Bool TabularCoordinate::setReferencePixel(const Vector<Double> &refPix)
{
    AlwaysAssert(refPix.nelements() == 1, AipsError);
    crpix_p = refPix(0);
    return True;
}

Bool TabularCoordinate::setLinearTransform(const Matrix<Double> &xform)
{
    AlwaysAssert(xform.nelements() == 1, AipsError);
    matrix_p = xform(0,0);
    return True;
}

Bool TabularCoordinate::setIncrement(const Vector<Double> &inc) 
{
    AlwaysAssert(inc.nelements() == 1, AipsError);
    cdelt_p = inc(0);
    return True;
}

Bool TabularCoordinate::setReferenceValue(const Vector<Double> &refval)
{
    AlwaysAssert(refval.nelements() == 1, AipsError);
    crval_p = refval(0);
    return True;
}

Vector<Double> TabularCoordinate::pixelValues() const
{
    Vector<Double> pixels;
    if (channel_corrector_p) {
	pixels = channel_corrector_p->getX();
    }
    return pixels;
}

Vector<Double> TabularCoordinate::worldValues() const
{
    Vector<Double> tmp = pixelValues();
    const uInt n = tmp.nelements();
    for (uInt i=0; i<n; i++) {
	AlwaysAssert(toWorld(tmp(i), tmp(i)), AipsError);
    }
    return tmp;
}

Bool TabularCoordinate::save(RecordInterface &container,
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
	subrec.define("units", worldAxisUnits());
	if (channel_corrector_p) {
	    subrec.define("pixelvalues", pixelValues());
	    subrec.define("worldvalues", worldValues());
	} else {
	    Vector<Double> tmp;
	    subrec.define("pixelvalues", tmp);
	    subrec.define("worldvalues", tmp);
	}

	container.defineRecord(fieldName, subrec);
    }
    return ok;
}

TabularCoordinate *TabularCoordinate::restore(const RecordInterface &container,
			   const String &fieldName)
{
    if (! container.isDefined(fieldName)) {
	return 0;
    }

    Record subrec(container.asRecord(fieldName));
    
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

    if (!subrec.isDefined("pixelvalues") || !subrec.isDefined("worldvalues")) {
	return 0;
    }
    Vector<Double> world, pixels;
    subrec.get("pixelvalues", pixels);
    subrec.get("worldvalues", world);

    TabularCoordinate *retval = 0;
    if (pixels.nelements() > 0) {
	retval = new TabularCoordinate(pixels, world, units(0), axes(0));
    } else {
	retval = new TabularCoordinate(crval(0), cdelt(0), crpix(0), units(0),
				       axes(0));
    }

    return retval;
}

Coordinate *TabularCoordinate::clone() const
{
    return new TabularCoordinate(*this);
}
