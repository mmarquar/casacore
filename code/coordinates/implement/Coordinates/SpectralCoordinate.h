//# SpectralCoordinate.h: Interconvert between pixel and frequency.
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

#if !defined(AIPS_SPECTRAL_COORDINATE_H)
#define AIPS_SPECTRAL_COORDINATE_H

#include <aips/aips.h>
#include <trial/Coordinates/Coordinate.h>
#include <trial/Coordinates/TabularCoordinate.h>
#include <aips/Measures/MFrequency.h>

class LogIO;
template<class T> class Quantum;

// <summary>
// Interconvert pixel and frequency values.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=Coordinate>Coordinate</linkto>
//   <li> <linkto class=MFrequency>MFrequency</linkto>,
//        <linkto class=MDoppler>MDoppler</linkto> and
//        <linkto class=VelocityMachine>VelocityMachine</linkto> 
//        classes if you want radial velocities.
// </prerequisite>
//
// <synopsis>
// This class performs the mapping from pixel to frequency. 
// An MFrequency may be obtained if you are interested in radial velocities.
// </synopsis>
//

// <note role=caution>
// All pixels coordinates are zero relative.
// </note>
//
// <example>
// See the example in <linkto module=Coordinates>Coordinates.h</linkto>
// and tSpectralCoordinate.cc
// </example>
//
// <todo asof="1997/08/15">
//   <li> Allow other than linear interpolations for frequency lookup.
// </todo>

class SpectralCoordinate : public Coordinate
{
public:
    // Default constructor.    It is equivalent to doing
    // SpectralCoordinate(MFrequency::TOPO, 0.0, 1.0, 0.0)
    SpectralCoordinate();

    // Create a linear frequency axis.
    // f0 is the frequency of the reference pixel, inc is the pixel increment,
    // refChan is the reference pixel. You can
    // optionally store the rest frequency for later use in calculating radial
    // velocities.
    //
    // Frequencies and increments initially in Hz. This may be changed later
    // with the method setWorldAxisUnits.
    SpectralCoordinate(MFrequency::Types type, Double f0, Double inc, 
		       Double refChan, Double restFrequency = 0.0);

    // Construct a SpectralCoordinate with the specified frequencies. 
    // This axis can be non linear; the increments and related 
    // functions return the <src>average</src> values
    // (calculated from the first and last pixels frequencies).
    //
    // A linear interpolation/extrapolation is used for channels which are
    // not supplied. The reference pixel is chosen to be 0.
    // The frequencies must increase or decrease monotonically (otherwise
    // the toPixel lookup would not be possible).
    SpectralCoordinate(MFrequency::Types type, const Vector<Double> &freqs,
		       Double restFrequency = 0.0);
    
    // Copy constructor (copy semantics).
    SpectralCoordinate(const SpectralCoordinate &other);

    // Assignment (copy semantics).
    SpectralCoordinate &operator=(const SpectralCoordinate &other);

    virtual ~SpectralCoordinate();

    // Always returns Coordinate::SPECTRAL.
    virtual Coordinate::Type type() const;

    // Always returns "Spectral"
    virtual String showType() const;

    // Always returns 1.
    // <group>
    virtual uInt nPixelAxes() const;
    virtual uInt nWorldAxes() const;
    // </group>

  // Convert a pixel position to a world position or vice versa. Returns True
  // if the conversion succeeds, otherwise it returns False and
  // <src>errorMessage()</src> contains an error message.  The input vectors
  // must be of length one and the output vectors are resized if they are not
  // already of length one.
  // <group>
  virtual Bool toWorld(Vector<Double> &world, 
		       const Vector<Double> &pixel) const;
  virtual Bool toPixel(Vector<Double> &pixel, 
		       const Vector<Double> &world) const;
  Bool toWorld(Double& world, const Double& pixel) const;
  Bool toPixel(Double& pixel, const Double& world) const;
  // </group>

    // Turn a pixel (channel number) into an MFrequency and vice versa. Usually you will do
    // this for calculating velocities or converting frequencies from one frame
    // to another.
    // <group>
    Bool toWorld(MFrequency &world,
		 Double pixel) const;
    Bool toPixel(Double& pixel, const MFrequency &world) const;
    // </group>

    // Retrieve/set the rest frequency in the current units.
    // <group>
    Double restFrequency() const;
    Bool setRestFrequency(Double newFrequency);
    // </group>
  
    // Retrieve/set the frequency system.  Note that setting the
    // frequency system just changes the internal value of the
    // frequency system, it does not cause any recomputation
    // or cause the result of <src>toWorld</src> to change.
    // <group>
    MFrequency::Types frequencySystem() const;
    void  setFrequencySystem(MFrequency::Types type);
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

    // Set the value of the requested attributed. Note that these just
    // change the internal values, they do not cause any recomputation.
    // <group>
    virtual Bool setWorldAxisNames(const Vector<String> &names);
    virtual Bool setReferencePixel(const Vector<Double> &refPix);
    virtual Bool setLinearTransform(const Matrix<Double> &xform);
    virtual Bool setIncrement(const Vector<Double> &inc) ;
    virtual Bool setReferenceValue(const Vector<Double> &refval);
    // </group>

    // Get the table, i.e. the pixel and world values. The length of these
    // Vectors will be zero if this axis is pure linear (i.e. if the
    // channel and frequencies are related through an increment and offset).
    // <group>
    Vector<Double> pixelValues() const;
    Vector<Double> worldValues() const;
    // </group>

    // ctype, crval, crpix, and cdelt must already be created. Other header
    // words are created as needed.  Use <src>oneRelative=True</src> to
    // convert zero-relative SpectralCoordinate pixel coordinates to 
    // one-relative FITS coordinates, and vice-versa.
    void toFITS(RecordInterface &header, uInt whichAxis, 
		LogIO &logger, Bool oneRelative=True,
		Bool preferVelocity=True, Bool opticalVelDef=True) const;
    static Bool fromFITS(SpectralCoordinate &out, String &error,
			 const RecordInterface &header, 
			 uInt whichAxis,
			 LogIO &logger, Bool oneRelative=True);

    // Set the unit. If adjust is True, the unit must be compatible with
    // frequency.
    virtual Bool setWorldAxisUnits(const Vector<String> &units,
				   Bool adjust = True);

    // Comparison function. Any private Double data members are compared
    // with the specified fractional tolerance.  Don't compare on the specified 
    // axes in the Coordinate.  If the comparison returns False, 
    // <src>errorMessage()</src> contains a message about why.
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

    // A null pointer means that the restoration did not succeed - probably 
    // because fieldName doesn't exist or doesn't contain a coordinate system.
    static SpectralCoordinate *restore(const RecordInterface &container,
				   const String &fieldName);

    // Make a copy of ourself using new. The caller is responsible for calling
    // delete.
    virtual Coordinate *clone() const;
private:
    MFrequency::Types type_p;
    Double restfreq_p;
    TabularCoordinate worker_p;

    mutable Vector<Double> world_tmp_p;
    mutable Vector<Double> pixel_tmp_p;

    // Check format type
    void checkFormat(Coordinate::formatType& format,         
                     const Bool absolute) const;



};

#endif
