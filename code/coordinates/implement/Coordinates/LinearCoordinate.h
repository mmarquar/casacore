//# LinearCoordinate.h: Assume a general linear relation between pixel and world axes.
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

#if !defined(AIPS_LINEAR_COORDINATE_H)
#define AIPS_LINEAR_COORDINATE_H

#include <aips/aips.h>
#include <trial/Coordinates/Coordinate.h>
#include <trial/Coordinates/LinearXform.h>
#include <aips/Arrays/Vector.h>

// <summary>
// Interconvert between pixel and a linear world coordinate.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=Coordinate>Coordinate</linkto> defines the fundamental
//        interface to coordinate conversions.
// </prerequisite>
//
// <synopsis>
// The LinearCoordinate class ties pixel and world axes together through 
// a general linear transformation. 
//
// <srcblock>
// world = (cdelt * PC * (pixel - crpix)) + crval
// </srcblock>
// Where PC is an NxN matrix, pixel, crval, crpix and world are length N 
// vectors, and cdelt is an NxN diagonal matrix, represented as a length 
// N vector.
//
// The actual computations are carried out with the WCS library.
//
// The LinearCoordinate can contain several axes (similar to the way
// in which the DirectionCoordinate contains two axes, but they
// would not be coupled).
// </synopsis>
//
// <note role=caution>
// All pixels coordinates are zero relative.
// </note>
//
// <example>
// See the example in <linkto module=Coordinates>Coordinates.h</linkto>.
// </example>
//
// <motivation>
// This class is intended for use for axes which do not have specific coordinate
// types. A "time" axis would be a good example.
// </motivation>
//
// <todo asof="1997/01/14">
//   <li> Allow differing numbers of world and pixel axes. Requires a change in
//        WCS or use of a different library.
// </todo>

class LinearCoordinate : public Coordinate
{
public:
    // The default constructor make a coordinate for which pixel 
    // and world coordinate are equal.
    LinearCoordinate(uInt naxis = 1);

    // Construct the linear transformation.
    LinearCoordinate(const Vector<String> &names,
		     const Vector<String> &units,
		     const Vector<Double> &refVal,
		     const Vector<Double> &inc,
		     const Matrix<Double> &xform,
		     const Vector<Double> &refPix);

    // Copy constructor (copy semantics).
    LinearCoordinate(const LinearCoordinate &other);

    // Assignment  (copy semantics).
    LinearCoordinate &operator=(const LinearCoordinate &other);

    // Destructor
    virtual ~LinearCoordinate();

    // Returns Coordinate::LINEAR.
    virtual Coordinate::Type type() const;

    //Returns "Linear"
    virtual String showType() const;

    // Returns the number of pixel/world axes. The number of axes is arbitrary,
    // however the number or world and pixel axes must at present be the same.
    // <group>
    virtual uInt nPixelAxes() const;
    virtual uInt nWorldAxes() const;
    // </group>

    // Convert a pixel position to a worl position or vice versa. Returns True
    // if the conversion succeeds, otherwise it returns False and method
    // errorMessage returns an error message.  The output 
    // vectors are appropriately resized.
    // <group>
    virtual Bool toWorld(Vector<Double> &world, 
			 const Vector<Double> &pixel) const;
    virtual Bool toPixel(Vector<Double> &pixel, 
			 const Vector<Double> &world) const;
    // </group>

    // Return the requested attributes
    // <group>
    virtual Vector<String> worldAxisNames() const;
    virtual Vector<Double> referenceValue() const;
    virtual Vector<Double> increment() const;
    virtual Matrix<Double> linearTransform() const;
    virtual Vector<Double> referencePixel() const;
    virtual Vector<String> worldAxisUnits() const;
    // </group>

    // Set the value of the requested attributed. Note that these just
    // change the internal values, they do not cause any recomputation.
    // <group>
    virtual Bool setWorldAxisNames(const Vector<String> &names);
    virtual Bool setReferencePixel(const Vector<Double> &refPix);
    virtual Bool setLinearTransform(const Matrix<Double> &xform);
    virtual Bool setIncrement(const Vector<Double> &inc);
    virtual Bool setReferenceValue(const Vector<Double> &refval);
    // </group>

    // Set the world axis units. If adjust is True, 
    // the units must be compatible with
    // angle. The units are initially "rad" (radians).
    virtual Bool setWorldAxisUnits(const Vector<String> &units,
				   Bool adjust = True);

    // Comparison function. Any private Double data members are compared    
    // with the specified fractional tolerance.  Don't 
    // compare on the specified     
    // axes in the Coordinate.  If the comparison returns False, method
    // errorMessage contains a message about why.
    // <group>
    virtual Bool near(const Coordinate* pOther, 
                      Double tol=1e-6) const;
    virtual Bool near(const Coordinate* pOther, 
                      const Vector<Int>& excludeAxes,
                      Double tol=1e-6) const;
    // </group>

    // Find the Coordinate for when we Fourier Transform ourselves.  This pointer
    // must be deleted by the caller. Axes specifies which axes of the coordinate
    // you wish to transform.   Shape specifies the shape of the image
    // associated with all the axes of the coordinate.   Currently the
    // output reference pixel is always shape/2.
    virtual Coordinate* makeFourierCoordinate (const Vector<Bool>& axes,
                                               const Vector<Int>& shape) const;

    // Save ourself into the supplised record using the supplied field name.
    // The field must not exist, otherwise <src>False</src> is returned.
    virtual Bool save(RecordInterface &container,
		    const String &fieldName) const;

    // Restore the LinearCoordinate from a record.
    // A null pointer means that the restoration did not succeed - probably 
    // because fieldName doesn't exist or doesn't contain a coordinate system.
    static LinearCoordinate *restore(const RecordInterface &container,
				   const String &fieldName);

    // Make a copy of ourself using new. The caller is responsible for calling
    // delete.
    virtual Coordinate *clone() const;

private:
    // An interface to the WCSLIB linear transformation routines.
    LinearXform transform_p;

    // Names and units.
    // <group>
    Vector<String> names_p;
    Vector<String> units_p;
    // </group>

    // The reference value.
    Block<Double> crval_p;

    // Temporary
    mutable String errorMsg_p;
};

#endif
