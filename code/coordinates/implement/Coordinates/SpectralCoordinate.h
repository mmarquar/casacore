//# SpectralCoordinate.h: Map a channel number to frequency.
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

#if !defined(AIPS_SPECTRAL_COORDINATE_H)
#define AIPS_SPECTRAL_COORDINATE_H

#include <aips/aips.h>
#include <trial/Coordinates/Coordinate.h>
#include <aips/Measures/MFrequency.h>

// <summary>
// Map a channel number to frequency.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=Coordinate>Coordinate</linkto>
//   <li> <linkto class=MFrequency>MFrequency</linkto> and
//        <linkto class=MDoppler>MDoppler</linkto> classes if you want
//        radial velocities.
// </prerequisite>
//
// <synopsis>
// This class performs the mapping from channel number to frequency. An MFrequency
// may be obtained if you are interested in radial velocities.
// </synopsis>
//
// <example>
// See the example in <linkto module=Coordinates>Coordinates.h</linkto>.
// </example>
//
// <todo asof="1997/01/15">
//   <li> Allow non-regularly gridded axes?
//   <li> Do velocity calculations directly for the user rather than going
//        through the measures system?
// </todo>

class SpectralCoordinate : public Coordinate
{
public:
    // f0 is the frequncy of th reference channel, inc is the channel increment,
    // refChan is the (0-relative) reference channel (often 0). You can optionally
    // store the rest frequency for later use in calculating radial velocities.
    //
    // Frequencies and increments initially in Hz. This may be changed later
    // with setWorldAxisUnits().
    SpectralCoordinate(MFrequency::Types type, Double f0, Double inc, 
		       Double refChan, Double restFrequency = 0.0);

    // Overwrite this SpectralCoordinate with other (copy semantics).
    // <group>
    SpectralCoordinate(const SpectralCoordinate &other);
    SpectralCoordinate &operator=(const SpectralCoordinate &other);
    // </group>

    virtual ~SpectralCoordinate();

    // Always returns Coordinate::SPECTRAL.
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

    // Turn a pixel (channel number) into an MFrequency. Usually you will do
    // this for calculating velocities or converting frequencies from one frame
    // to another.
    Bool toWorld(MFrequency &world,
		 Double pixel) const;

    // Retrieve/set the rest frequency in the cuurent units.
    // <group>
    Double restFrequency() const;
    Bool setRestFrequency(Double newFrequency);
    // </group>

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
    virtual Bool setReferenceValue(const Vector<Double> &refval);
    // </group>

    // Set the unit. If adjust is True, the unit must be compatible with
    // frequency.
    virtual Bool setWorldAxisUnits(const Vector<String> &units,
				   Bool adjust = True);

    // Save ourself into the supplised record using the supplied field name.
    // The field must not exist, otherwise <src>False</src> is returned.
    virtual Bool save(RecordInterface &container,
		    const String &fieldName) const;

    // A null pointer means that the restoration did not succeed - probably 
    // because fieldName doesn't exist or doesn't contain a coordinate system.
    static SpectralCoordinate *restore(const RecordInterface &container,
				   const String &fieldName);

    // Make a copy of ourself using new. The caller is responsible for calling
    // delete.
    virtual Coordinate *clone() const;
private:
    MFrequency::Types type_p;
    Double crval_p, cdelt_p, crpix_p;
    String unit_p;
    String name_p;
    Double matrix_p;
    Double restfreq_p;

    // Undefined and inaccessible
    SpectralCoordinate();
};


#endif


