//# StokesCoordinate.h: Interconvert between pixel number and Stokes value.
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

#if !defined(AIPS_STOKES_COORDINATE_H)
#define AIPS_STOKES_COORDINATE_H

#include <aips/aips.h>
#include <trial/Coordinates/Coordinate.h>
#include <aips/Measures/Stokes.h>
#include <aips/Containers/Block.h>

// <summary>
// Interconvert between pixel number and Stokes value.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=Coordinate>Coordinate</linkto>
//   <li> <linkto class=Stokes>Stokes</linkto>
// </prerequisite>
//
// <synopsis>
// Although not really a "coordinate", an axis where pixel numbers are used
// for different Stokes values are in wide use.
// </synopsis>
//
// <example>
// See the example in <linkto module=Coordinates>Coordinates.h</linkto>.
// </example>
//
// <todo asof="1997/1/14">
//   <li> This could probably be generalized into an "enumeration axis" class.
// </todo>

class StokesCoordinate : public Coordinate
{
public:

    // The length of whichStokes is the length of the axis, and the values
    // define which stokes are in which axis value. Often the vector will be of
    // length 4 and will contain Stokes::I, Q, U and V, however any valid value
    // from the stokes enum may be used. The values may not repeat however, e.g.
    // only one axis position may contain "I".
    StokesCoordinate(const Vector<Int> &whichStokes);

    // Copy semantics.
    // <group>
    StokesCoordinate(const StokesCoordinate &other);
    StokesCoordinate &operator=(const StokesCoordinate &other);
    // </group>

    virtual ~StokesCoordinate();

    // Always returns Coordinates::STOKES
    virtual Coordinate::Type type() const;

    // Always returns 1.
    // <group>
    virtual uInt nPixelAxes() const;
    virtual uInt nWorldAxes() const;
    // </group>

    // Convert a pixel position to a worl position or vice versa. Returns True
    // if the conversion succeeds, otherwise it returns False and
    // <src>errorMessage()</src> contains an error message.
    // <group>
    virtual Bool toWorld(Vector<Double> &world, 
			 const Vector<Double> &pixel) const;
    virtual Bool toPixel(Vector<Double> &pixel, 
			 const Vector<Double> &world) const;
    // </group>

    // Interconvert between pixel number and stokes type.
    // <group>
    Bool toPixel(Int &pixel, Stokes::StokesTypes stokes) const;
    Bool toWorld(Stokes::StokesTypes &stokes, Int pixel) const;
    // </group>

    // Get all the stokes at once.
    Vector<Int> stokes() const;

    // Report the value of the requested attributed.
    // <group>
    virtual Vector<String> worldAxisNames() const;
    virtual Vector<Double> referencePixel() const;
    virtual Matrix<Double> linearTransform() const;
    virtual Vector<Double> increment() const;
    virtual Vector<Double> referenceValue() const;
    virtual Vector<String> worldAxisUnits() const;
    // </group>

    // Set the value of the requested attributed.
    // <group>
    virtual Bool setWorldAxisNames(const Vector<String> &names);
    virtual Bool setReferencePixel(const Vector<Double> &refPix);
    virtual Bool setLinearTransform(const Matrix<Double> &xform);
    virtual Bool setIncrement(const Vector<Double> &inc) ;
    virtual Bool setReferenceValue(const Vector<Double> &refval) ;
    // </group>

    // You can only set the units if adjust is False (or if the units are
    // the empty string).
    virtual Bool setWorldAxisUnits(const Vector<String> &units,
				   Bool adjust = True);

    // Save ourself into the supplised record using the supplied field name.
    // The field must not exist, otherwise <src>False</src> is returned.
    virtual Bool save(RecordInterface &container,
		    const String &fieldName) const;

    // A null pointer means that the restoration did not succeed - probably 
    // because fieldName doesn't exist or doesn't contain a coordinate system.
    static StokesCoordinate *restore(const RecordInterface &container,
				   const String &fieldName);

    // Make a copy of ourself using new. The caller is responsible for calling
    // delete.
    virtual Coordinate *clone() const;
private:
    Block<Int> values_p;
    // Keep these for subimaging purposes.
    Double crval_p, crpix_p, matrix_p, cdelt_p;
    String name_p;
    String unit_p;

    // Undefined and inaccessible
    StokesCoordinate();
};

#endif
