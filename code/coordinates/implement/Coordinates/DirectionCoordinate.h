//# DirectionCoordinate.h: Interconvert pixel positions and directions (e.g. RA/DEC)
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

#if !defined(AIPS_DIRECTION_COORDINATE_H)
#define AIPS_DIRECTION_COORDINATE_H

#include <aips/aips.h>
#include <trial/Coordinates/Coordinate.h>
#include <trial/Coordinates/LinearXform.h>
#include <trial/Coordinates/Projection.h>
#include <aips/Measures/MDirection.h>
#include <aips/Arrays/Vector.h>
#include <wcslib/wcs.h>

class celprm;
class prjprm;

// <summary>
// Interconvert pixel positions and directions (e.g. RA/DEC).
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=Coordinate>Coordinate</linkto> defines the fundamental
//        interface to coordinate conversions.
//   <li> <linkto class=MDirection>MDirection</linkto> defines the types of
//        directions (J2000 etc.) which are defined. The measures machinery
//        also implements "astronomical" conversions which are outside the
//        scope of these coordinates (for example, <src>J2000</src> to
//        <src>B1950</src>).
//   <li> <linkto class=Projection>Projection</linkto> defines the types of
//        celestial projections which are available.
// </prerequisite>
//
// <synopsis>
// This class implements pixel to world coordinate conversions. It is important
// to understand that this class implements only the geometric conversions
// (e.g. SIN projection). Astronomical conversions (RA/DEC <--> l,b)
// are the responsibliy of the <linkto module=Measures>Measures</linkto> module.
// Of course the <linkto class=MDirection>MDirection</linkto> object you can
// obtain from this class would be the prime input for that conversion.
//
// The actual computations are carried out with the WCS library.
// </synopsis>
//

// <note role=caution>
// All pixels coordinates are zero relative.
// </note>

// <example>
// See the example in <linkto module=Coordinates>Coordinates.h</linkto>
// and tDirectionCoordinate.cc
// </example>
//
// <motivation>
// Directions in the sky are fundamental to astronomy.
// </motivation>
//
// <todo asof="1997/01/13">
//   <li> Add "wcsmix" capability.
// </todo>

class DirectionCoordinate : public Coordinate
{
public:
    // The default constructor creates a J2000 direction coordinate with a
    // CARtesion projection with longitude,latitude 0,0 at pixel 0,0 and an
    // increment of +1 radian per pixel on both axes.
    DirectionCoordinate();

    // Define the direction coordinate transformation. refLong and refLat will
    // normally the the RA/DEC of the pixel described by refX/refY. incLat/incLong
    // are the increments per pixel (RA is usually negative), and the xform
    // matrix is usually the unit diagonal matrix unless you have a rotation or
    // some other linear transformation between the pixel and world axes.
    // 
    // Note that the units are RADIANS initially. You can change it to degrees
    // or something else with the setWorldAxisUnits method later if you want.
    DirectionCoordinate(MDirection::Types directionType,
 			const Projection &projection,
			Double refLong, Double refLat,
			Double incLong, Double incLat,
			const Matrix<Double> &xform,
 			Double refX, Double refY);

    // Copy constructor (copy semantics)
    DirectionCoordinate(const DirectionCoordinate &other);

    // Assignment (copy semantics).
    DirectionCoordinate &operator=(const DirectionCoordinate &other);

    // Destructor
    virtual ~DirectionCoordinate();

    // Return Coordinate::DIRECTION
    virtual Coordinate::Type type() const;

    // Returns "Direction"
    virtual String showType() const;

    // Always 2.
    // <group>
    virtual uInt nPixelAxes() const;
    virtual uInt nWorldAxes() const;
    // </group>

    // Convert a pixel position to a worl position or vice versa. Returns True
    // if the conversion succeeds, otherwise it returns False and method
    // errorMessage returns an error message.   The output 
    // vectors are appropriately resized.
    // <group>
    virtual Bool toWorld(Vector<Double> &world, 
			 const Vector<Double> &pixel) const;
    virtual Bool toPixel(Vector<Double> &pixel, 
			 const Vector<Double> &world) const;
    // </group>


    // Mixed pixel/world coordinate conversion.
    // worldIn and worldAxes are of length nWorldAxes.
    // pixelIn and pixelAxes are of length nPixelAxes.
    // worldAxes(i) = True specifies you have given a world
    // value in worldIn(i) to convert to pixel.
    // pixelAxes(i)=True specifies you have given a pixel 
    // value in pixelIn(i) to convert to world.
    // You cannot specify the same axis via worldAxes
    // and pixelAxes.
    // Values in pixelIn are converted to world and
    // put into worldOut in the appropriate worldAxis
    // location.  Values in worldIn are copied to
    // worldOut.   
    // Values in worldIn are converted to pixel and
    // put into pixelOut in the appropriate pixelAxis
    // location.  Values in pixelIn are copied to
    // pixelOut
    // worldMin and worldMax specify the range of the world
    // coordinate (in the world axis units of that world axis
    // in the coordinate system) being solved for in a mixed calculation
    // for each world axis. They are only actually needed for DirectionCoordinates
    // and for all other coordinates the relevant elements
    // can be undefined.   If you don't know, use -180 to 180
    // degrees for longitude, and -90 to 90 for latitude. 
    // Some mixed solutions can be degenerate, whereupon you
    // you must say which one you want.
    // Removed axes are handled (for example, a removed pixel
    // axis with remaining corresponding world axis will
    // correctly be converted to world using the replacement
    // value).
    // Returns True if the conversion succeeds, otherwise it returns False and
    // <src>errorMessage()</src> contains an error message. The output vectors
    // are resized.
    virtual Bool toMix(Vector<Double>& worldOut,
                       Vector<Double>& pixelOut,
                       const Vector<Double>& worldIn,
                       const Vector<Double>& pixelIn,
                       const Vector<Bool>& worldAxes,
                       const Vector<Bool>& pixelAxes,
                       const Vector<Double>& worldMin,
                       const Vector<Double>& worldMax) const; 

    // A convenient way to turn the world vector into an MDirection for further
    // processing in the Measures system.
    // 
    // We could improve the performance of this if it would be useful, however I
    // expect that normally you would just call this once to get a template
    // MDirection, and then call the vector versions. A similar toPixel is also
    // possible, but probably not needed.
    Bool toWorld(MDirection &world, const Vector<Double> &pixel) const;

    // Recover the requested attributed.
    // <group>
    MDirection::Types directionType() const;
    Projection projection() const;
    virtual Vector<String> worldAxisNames() const;
    virtual Vector<String> worldAxisUnits() const;
    virtual Vector<Double> referenceValue() const;
    virtual Vector<Double> increment() const;
    virtual Matrix<Double> linearTransform() const;
    virtual Vector<Double> referencePixel() const;
    // </group>

    // Set the value of the requested attribute.  Note that these just
    // change the internal values, they do not cause any recomputation.
    // <group>
    virtual Bool setWorldAxisNames(const Vector<String> &names);
    virtual Bool setReferencePixel(const Vector<Double> &refPix);
    virtual Bool setLinearTransform(const Matrix<Double> &xform);
    virtual Bool setIncrement(const Vector<Double> &inc);
    virtual Bool setReferenceValue(const Vector<Double> &refval);
    // </group>

    // Change the world axis units.
    // If adjust is True, the units must be compatible with
    // angle. The units are initially "rad" (radians).
    virtual Bool setWorldAxisUnits(const Vector<String> &units,
				   Bool adjust = True);

    // Return canonical axis names for the given MDirection type,
    // giving FITS names if desired.
    // BEG think this should be in the MDirection class, but WNB
    // disagrees. Leave it here for now.
    static Vector<String> axisNames(MDirection::Types type, 
				    Bool FITSName = False);

    // Comparison function. Any private Double data members are compared    
    // with the specified fractional tolerance.  Don't compare on the specified     
    // axes in the Coordinate.  If the comparison returns False,  method
    // errorMessage returns a message about why.
     // <group>
    Bool near(const Coordinate* pOther, 
              Double tol=1e-6) const;
    Bool near(const Coordinate* pOther, 
              const Vector<Int>& excludeAxes,
              Double tol=1e-6) const;
    // </group>


    // Format a DirectionCoordinate coordinate world value nicely through the
    // common format interface.  Formatting types that are allowed are
    // Coordinate::SCIENTIFIC, Coordinate::FIXED, Coordinate::TIME
    // If you ask for format type Coordinate::DEFAULT then the
    // selected format depends upon what the value of the enum 
    // MDirection::GlobalTypes is for this DirectionCoordinate.
    // For example, if it is GRADEC or GHADEC you would
    // get Coordinate::TIME style formatting (DD:MM:SS.SS), otherwise
    // you would get Coordinate::FIXED formatting by default.
    //
    // axis says which axis in this Coordinate we are formatting.  
    // We have to know this because we may format, say, RA and DEC differently.  
    // For Coordinate::TIME style formatting, precision
    // refers to the places after the decimal in the SS field.
    //<group>
    virtual void getPrecision (Int& precision, 
                               Coordinate::formatType& format,
                               const Bool absolute, 
                               const Int defPrecScientific,
                               const Int defPrecFixed,
                               const Int defPrecTime) const;
    virtual String format(String& units,
                          const Coordinate::formatType format, 
                          const Double worldValue, 
                          const uInt axis, 
                          const Bool absolute,
                          const Int precision = -1) const;
    //</group>

    // Save ourself into the supplised record using the supplied field name.
    // The field must not exist, otherwise <src>False</src> is returned.
    virtual Bool save(RecordInterface &container,
		    const String &fieldName) const;

    // A null pointer means that the restoration did not succeed - probably 
    // because fieldName doesn't exist or doesn't contain a coordinate system.
    static DirectionCoordinate *restore(const RecordInterface &container,
				   const String &fieldName);

    // Make a copy of ourself using new. The caller is responsible for calling
    // delete.
    virtual Coordinate *clone() const;

private:
    // Direction type
    MDirection::Types type_p;

    // Projection parameters
    Projection projection_p;

    // WCS structures
    // <group>
    celprm* celprm_p;
    prjprm* prjprm_p;
    wcsprm* wcs_p;
    char c_ctype_p[2][9];
    double c_crval_p[2];
    // </group>

    // Performs the linear part of the transformation.
    LinearXform linear_p;

    // WCS computes in degrees - use this to convert back and forth between
    // degrees and the currently requested units.
    Double to_degrees_p[2];

    // Axis names.
    Vector<String> names_p;

    // Current units.
    Vector<String> units_p;

    // Temporaries
    mutable Vector<Double> world_tmp_p;
    mutable Vector<Double> in_tmp_p;
    mutable Vector<Double> out_tmp_p;
    mutable Quantum<Double> mix_quant_tmp;
    mutable String errorMsg_p;
    mutable int mixpix_p;
    mutable int mixcel_p;
    mutable double mix_vspan_p[2];
    mutable double mix_world_p[2];
    mutable double mix_pixcrd_p[2];
    mutable double mix_imgcrd_p[2];
    mutable double mix_vstep_p;
    mutable int mix_viter_p;
    mutable double mix_phi_p;
    mutable double mix_theta_p;

    // Interconvert between degrees and the current angular unit
    // <group>
    void toDegrees(Vector<Double> &other) const;
    void toOther(Vector<Double> &degrees) const;
    // </group>

    // Check formatting types
    void checkFormat(Coordinate::formatType& format,
                     const Bool absolute) const;

    // Mixed pixel/world coordinate conversion.  Vector in must
    // be length nWorldAxes (2).  Specify whether longitude
    // (in(0)) or latitude (in(1)) is the world coordinate . It is
    // assumed that the other value is the pixel coordinate.
    Bool toMix2(Vector<Double>& out, const Vector<Double>& in,
                const Vector<Double>& minWorld, const Vector<Double>& maxWorld,
                Bool longIsWorld) const;

    // Helper functions interfacing to WCS
    // <group>
    void make_celprm_and_prjprm(celprm* &pCelPrm, prjprm* &pPrjPrm, wcsprm* &pWcs,
                                   char c_ctype[2][9], double c_crval[2],
                                   const Projection& proj,
                                   MDirection::Types type,
                                   Double refLong, Double refLat,
                                   Double longPole, Double latPole) const;

    void copy_celprm_and_prjprm(celprm* &pToCel, prjprm* &pToPrj,
                                   wcsprm* &pToWcs,
                                   char toctype[2][9], double tocrval[2],
                                   const celprm *pFromCel, const prjprm *pFromPrj,
                                   const wcsprm *pFromWcs,
                                   const char fromctype[2][9], const double fromcrval[2]) const;
    // </group>

};

#endif
