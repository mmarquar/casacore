//# CoordinateSystem.h: Interconvert pixel and image coordinates.
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

#if !defined(AIPS_COORDINATE_SYSTEM_H)
#define AIPS_COORDINATE_SYSTEM_H

#include <aips/aips.h>
#include <trial/Coordinates/Coordinate.h>
#include <trial/Coordinates/ObsInfo.h>

#include <aips/Containers/Block.h>

class DirectionCoordinate;
class LinearCoordinate;
class SpectralCoordinate;
class StokesCoordinate;
class TabularCoordinate;
class IPosition;

// <summary>
// Interconvert pixel and image coordinates.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=Coordinate>Coordinate</linkto>
// </prerequisite>

// <synopsis>
// CoordinateSystem is the normal interface to coordinate systems,
// typically attached to an 
// <linkto class=ImageInterface>ImageInterface</linkto>, however the
// coordinate system can be manipulated on its own. CoordinateSystem
// is in turn composed from various classes derived from the base class
// <linkto class=Coordinate>Coordinate</linkto>.
// <p>
// The fundamental operations available to the user of a 
// CoordinateSystem are:
// <ol>
//   <li> Transform a world (physical) coordinate system to a pixel coordinate 
//        or vice versa viathe methods toWorld and toPixel.
//   <li> Compose a coordinate system from one or more independent groups,
//        typically the sky-plane transformation will be one group, and the
//        spectral axis will be another group. Each group consists of a linear
//        transformation (in FITS terms, apply <src>CRPIX, PC, CDELT</src>)
//        to turn the pixel coordinates into relative physical coordinates, 
//        followed by a (possibly) nonlinear projection to world coordinates 
//        (i.e. apply <src>CTYPE and CRVAL</src>), typically a sky projection
//        or a frequency to velocity conversion. Note that an arbitrary rotation
//        or linear transformation can be applied by changing the
//        matrix.
//   <li> Transpose the world and/or pixel axes.
//   <li> One or more pixel or world axes may be removed. You are encouraged to
//        leave all the world axes if you remove a pixel axis.
//        Removing a world axis also removes the corresponding pixel axis.
//   <li> Calculate the coordinate system that results from a subimage
//        operation.
// </ol>
//
// Note that all the knowledge to do with removing and transposing axes is
// maintained by the CoordinateSystem.  The individual Coordinates, of which it
// is made, know nothing about this.
// <p>
// Although the CoordinateSystem exists in the absence of an image, the usual
// time you will find one is attached to an object derived from ImageInterface 
// such as PagedImage. When you do so, the physical (or pixel) axes in the image
// map one to one with the pixel axes contained in the CoordinateSystem.
// It cannot be any other way as when you create a PagedImage, it is checked
// that there are equal numbers of image and CoordinateSystem pixel axes.
// It is up to the creator of the PagedImage to make sure that they are
// in the correct order.
// <p>
// However, the CoordinateSystem may have more world axes than pixel axes
// because it is possible to remove a pixel axis but not its associated
// world axis (for example for a moment image).   Now, if you use
// the CoordinateSystem functions 
// referencePixel and referenceValue, you will find the vector of reference
// values will have more values than the vector of reference pixels,
// if a pixel axis has been removed but not the world axis.  You 
// must use the ancilliary functions provided
// to find out what is where.   
// <p>
// Let's consider an example where a CoordinateSystem consisted of
// a DirectionCoordinate and a SpectralCoordinate.  Let us say that
// the first two pixel axes of the image associate (roughly of course
// because lines of constant RA and DEC are not parallel with
// the pixel coordinates) with the DirectionCoordinate (RA and DEC say) 
// and the third pixel axis is the SpectralCoordinate.
// Now imagine we collapse the image along the second pixel axis (roughly,
// the DEC axis).  For the output image, we remove the second pixel axis
// from the CoordinateSystem, but leave the world axis intact.  This enables
// us to still be able to make coordinate conversions for the first (roughly RA)
// pixel axis.  Thus, CoordinateSystem::referenceValue would return a Vector of
// length 3 (for RA, DEC and spectral), but CoordinateSystem::referencePixel
// would return a vector length 2 (for RA and spectral).  
// <p>
// Now this CoordinateSystem has two coordinates, a DirectionCoordinate and
// a SpectralCoordinate, and let us state that that is the order in which
// they exist in the CoordinateSystem (you can change them about if you wish);
// they are coordinates number 0 and 1. The DirectionCoordinate has two axes
// (RA and DEC) and the SpectralCoordinate has one axis. Only the
// CoordinateSystem knows about removed axes, the DirectionCoordinate
// itself is ignorant that it has been bisected. If you want to find
// out what axis in the Coordinate system is where, you can use
// the functions findPixelAxis or findWorldAxis.
// If we asked the former to find pixel axis 0, it would tell us that the
// Coordinate number was 0 (the DirectionCoordinate) and that the axis in
// that coordinate was 0 (the first axis in a DirectionCoordinate
// is always longitude, the second always latitude).  If we asked it to find
// pixel axis 1, it would tell us that the Coordinate number was 1
// (the SpectralCoordinate) and that the axis in that coordinate was 0
// (there is only one axis in a SpectralCoordinate). If we asked for
// pixelAxis 2 that would generate an error because our squashed image
// only has 2 pixel axes. Now, if we asked findWorldAxis similar questions,
// it would tell us that worldAxis 0 in the CoordinateSystem can be found in
// coordinate 0 (the DirectionCoordinate) in axis 0 of that DirectionCoordinate.
// Similarly, worldAxis 1 in the CoordinateSystem (which has not been removed)
// is in coordinate 0 (the DirectionCoordinate) in axis 1 of that 
// Finally, worldAxis 2 in the CoordinateSystem is in coordinate 1 
// (the SpectralCoordinate) in axis 0 of that SpectralCoordinate.
// <p>
// Other handy functions are pixelAxes and worldAxes.
// These list the pixel and world axes in
// the CoordinateSystem for the specified coordinate. Thus, if we asked
// pixelAxes to find the pixel axes for coordinate 0 (the  DirectionCoordinate)
// in the CoordinateSystem it would return a vector [0, -1]  indicating
// the second axis of  the DirectionCoordinate has been removed.  However, 
// the worldAxes function would return [0,1] as no world axis has been removed.
// Similarly, if operated on coordinate 1 (the SpectralCoordinate), pixelAxes
// would return [1] and worldAxes would return [2].
//
// Because you can transpose the CoordinateSystem about, you should NEVER ASSUME
// ANYTHING except that the pixel axes of the CoordinateSystem map to the pixel
// axes of the image when you first construct the image.
// </synopsis>

// <note role=caution>
// All pixels coordinates are zero relative.
// </note>

// <example>
// See the example in <linkto module=Coordinates>Coordinates.h</linkto>
// and tCoordinateSystem.cc
// </example>

// <motivation>
// Coordinate systems for images.
// </motivation>

// <todo asof="1997/01/13">
//   <li> Undelete individual removed axes.
//   <li> Non-integral pixel shifts/decimations in subimage operations?
//   <li> Copy-on-write for efficiency?
//   <li> Check if the classes are thread safe in general
// </todo>


class CoordinateSystem : public Coordinate
{
public:
    // Default constructor.  This is an empty CoordinateSystem.
    CoordinateSystem();

    // Copying constructor (copy semantics)
    CoordinateSystem(const CoordinateSystem &other);

    // Assignment (copy semantics).
    CoordinateSystem &operator=(const CoordinateSystem &other);

    // Destructor
    virtual ~CoordinateSystem();

    // Add another coordinate to this coordinate system. This addition is done
    // by copying, so that if coord changes the change is NOT
    // reflected in the coordinate system.
    void addCoordinate(const Coordinate &coord);

    // Transpose the coordinate system so that world axis 0 is
    // newWorldOrder(0) and so on for all the other axes.
    // newPixelOrder works similarly. Normally you will give the
    // same transformation vector for both the world and pixel transformations,
    // however this is not required.
    void transpose(const Vector<Int> &newWorldOrder,
                   const Vector<Int> &newPixelOrder);

    // Find the world axis mapping to the supplied CoordinateSystem
    // from the current coordinate system. <src>False</src> is 
    // returned if either the supplied or current coordinate system, 
    // has no world axes (and a message recoverable with function
    // errorMessage indicating why).  Otherwise <src>True</src> is returned.
    // worldAxisMap(i) is the location of world axis <src>i</src> (from the
    // supplied coordinate system, cSys, in the current coordinate system.
    // worldAxisTranspose(i) is the location of world axis 
    // <src>i</src> (from the current coordinate system) in the supplied 
    // coordinate system, cSys.  The output vectors
    // are resized appropriately by this function.  A value of  -1 
    // in either vector means that it could not be found in the other
    // coordinate system.  The vector <src>refCange</src> says
    // if the types are the same, is there a reference type change
    // (e.g. TOPO versus LSR for the SpectralCoordinate, 
    // or J2000 versus GALACTIC for DirectionCoordinate). Thus
    // if refChange(i) is True, it means world axis i in the
    // current coordinate system was matched, but has a different
    // reference type
    Bool worldMap (Vector<Int>& worldAxisMap,
		   Vector<Int>& worldAxisTranspose,
		   Vector<Bool>& refChange,
		   const CoordinateSystem& cSys) const;

    // Remove a world or pixel axis. When its value is required for forward or
    // backwards transformations, use <src>replacement</src>
    // <br>
    // When a world axis is removed, the corresponding pixel axis is removed
    // too, because it makes no sense having a pixel axis without world
    // coordinates.
    // <br>
    // Removing a pixel axis without removing the corresponding world axis
    // is, however, possible and meaningful. It can be used when e.g. a
    // frequency plane is taken from a cube. The plane has 2 pixel axes, but
    // the 3rd world axis can still describe the frequency coordinate.
    // <group>
    Bool removeWorldAxis(uInt axis, Double replacement);
    Bool removePixelAxis(uInt axis, Double replacement);
    // </group>

    // Return a coordinate system appropriate for a shift of origin
    // (the shift is subtracted from the reference pixel)
    // and change of increment (the increments are multipled
    // by the factor). Both vectors should be of length nPixelAxes(). 
    CoordinateSystem subImage(const Vector<Int> &originShift,
			      const Vector<Int> &incrFac) const;

    // Untranspose and undelete all axes. Does not undo the effects of
    // subimaging.
    void restoreOriginal();

    // Returns the number of coordinates that this coordinate system contains.
    // The order might be unrelated to the axis order through the results of
    // transposing and removing axes.
    uInt nCoordinates() const;

    // For a given coordinate say where its world and coordinate axes are in
    // this coordinate system. The position in the returned Vector is its
    // axis number in the coordinate, and its value is the axis
    // number in the coordinate system. If the value is less than zero the axis
    // has been removed from this coordinate system.
    //  <group>
    Vector<Int> worldAxes(uInt whichCoord) const;
    Vector<Int> pixelAxes(uInt whichCoord) const;
    // </group> 

    // Return the type of the given coordinate.
    Coordinate::Type type(uInt whichCoordinate) const;

    // Returns the type of the given coordinate as a string
    String showType(uInt whichCoordinate) const;

    // Return the given coordinate as a reference to the base
    // class object
    const Coordinate &coordinate(uInt which) const;

    // Return the given coordinate.
    // Throws an exception if retrieved as the wrong type.
    // <group>
    const LinearCoordinate &linearCoordinate(uInt which) const;
    const DirectionCoordinate &directionCoordinate(uInt which) const;
    const SpectralCoordinate &spectralCoordinate(uInt which) const;
    const StokesCoordinate &stokesCoordinate(uInt which) const;
    const TabularCoordinate &tabularCoordinate(uInt which) const;
    // </group>

    // Replace one coordinate with another. The mapping of the coordinate axes
    // to the coordinate system axes is unchanged, therefore the number of world
    // and pixel axes must not be changed. You can change the type of the
    // coordinate however. For example, replace a SpectralCoordinate with a 1-D
    // Linearcoordinate.
    void replaceCoordinate(const Coordinate &newCoordinate, uInt whichCoordinate);

    // Find the coordinate number that corresponds to the given type.
    // Since there might be more than one coordinate of a given type you
    // can call this multiple times setting <src>afterCoord</src> to
    // the last value found. Returns -1 if a coordinate of the desired
    // type is not found.
    Int findCoordinate(Coordinate::Type type, Int afterCoord = -1) const;

    // Given an axis number (pixel or world) in the coordinate system,
    // find the corresponding coordinate number and axis in that coordinate. 
    // The returned values are set to -1 if the axis does not exist
    // <group>
    void findWorldAxis(Int &coordinate, Int &axisInCoordinate, 
		       uInt axisInCoordinateSystem) const;
    void findPixelAxis(Int &coordinate, Int &axisInCoordinate, 
		       uInt axisInCoordinateSystem) const;
    // </group>

    // Find the world axis for the given pixel axis in a coordinate system
    // Returns -1 if the world axis is unavailable (e.g. if it has been
    // removed).  
    Int pixelAxisToWorldAxis(uInt pixelAxis) const;

    // Find the pixel axis for the given world axis in a coordinate system
    // Returns -1 if the pixel axis is unavailable (e.g. if it has been
    // removed). 
    Int worldAxisToPixelAxis(uInt worldAxis) const;

    // Returns <src>Coordinate::COORDSYS</src>
    virtual Coordinate::Type type() const;

    // Always returns "System"
    virtual String showType() const;

    // Sums the number of axes in the coordinates that the coordinate system
    // contains, allowing for removed axes.
    // <group>
    virtual uInt nPixelAxes() const;
    virtual uInt nWorldAxes() const;
    // </group>


    // Convert a pixel position to a world position or vice versa. Returns True
    // if the conversion succeeds, otherwise it returns False and
    // <src>errorMessage()</src> contains an error message. The output vectors
    // are resized.
    // <group>
    virtual Bool toWorld(Vector<Double> &world, 
			 const Vector<Double> &pixel) const;
    virtual Bool toPixel(Vector<Double> &pixel, 
			 const Vector<Double> &world) const;
    // </group>

    // This is provided as a convenience since it is a very commonly desired
    // operation through CoordinateSystem.  The output vector is resized.   
    Bool toWorld(Vector<Double> &world, const IPosition &pixel) const;

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

    // Return the requested attribute.
    // <group>
    virtual Vector<String> worldAxisNames() const;
    virtual Vector<Double> referencePixel() const;
    virtual Matrix<Double> linearTransform() const;
    virtual Vector<Double> increment() const;
    virtual Vector<Double> referenceValue() const;
    virtual Vector<String> worldAxisUnits() const;
    // </group>

    // Set the requested attribute.  Note that these just
    // change the internal values, they do not cause any recomputation.
    // <group>
    virtual Bool setWorldAxisNames(const Vector<String> &names);
    virtual Bool setReferencePixel(const Vector<Double> &refPix);
    virtual Bool setLinearTransform(const Matrix<Double> &xform);
    virtual Bool setIncrement(const Vector<Double> &inc);
    virtual Bool setReferenceValue(const Vector<Double> &refval);
    // </group>

    // Change the units. If adjust is True, adjust the increment and
    // reference value by the ratio of the old and new units. This implies that
    // the units must be known <linkto class=Unit>Unit</linkto> strings, and
    // that they must be compatible, e.g. they can't change from time to
    // length.
    virtual Bool setWorldAxisUnits(const Vector<String> &units,
				   Bool adjust = True);

    // Comparison function. Any private Double data members are compared
    // with the specified fractional tolerance.  Don't compare on the specified 
    // pixel axes in the CoordinateSystem.  If the comparison returns
    // False, errorMessage() contains a message about why.
    // <group>
    virtual Bool near(const Coordinate* pOther, Double tol=1e-6) const;
    virtual Bool near(const Coordinate* pOther, 
                      const Vector<Int>& excludePixelAxes,
                      Double tol=1e-6) const;
    // </group>

    // Format a world value with the common format interface (refer to the base 
    // class <linkto class=Coordinate>Coordinate</linkto> for more details on this 
    // interface).   For the specified worldAxis, the coordinate
    // number in the CoordinateSystem is found and the actual derived Coordinate
    // class object for that number is created.  The arguments to the formatting 
    // function are then passed on to the formatter for that Coordinate. So
    // refer to the other derived Coordinate classes for specifics on the
    // formatting.
    //
    // The world value should have the units currently
    // set in the state of the particular Coordinate.  The units of
    // the formatted number are returned in <src>units</src>.
    // You can also use the Quantum interface (see base class Coordinate).
    // The units can then be anything consistent with the particualr 
    // Coordinate units.
    //<group>
    virtual String format(String& units,
                          Coordinate::formatType format,
                          Double worldValue,
                          uInt worldAxis,
                          Bool absolute,
                          Int precision=-1,
                          Bool native=False) const;

    // Miscellaneous information related to an observation, for example the
    // observation date.
    // <group>
    ObsInfo obsInfo() const;
    void setObsInfo(const ObsInfo &obsinfo);
    // </group>

    // Find the CoordinateSystem (you can safely caste the pointer to a CoordinateSystem)
    // for when we Fourier Transform ourselves.  This pointer 
    // must be deleted by the caller. Axes specifies which pixel axes of the Coordinate
    // System you wish to transform.   Shape specifies the shape of the image
    // associated with all the axes of the CoordinateSystem.  Currently you have
    // no control over the reference pixel, it is always shape/2.
    virtual Coordinate* makeFourierCoordinate (const Vector<Bool>& axes,
                                               const Vector<Int>& shape) const;


    // Save ourself into the supplied record using the supplied field name.
    // The field must not exist, otherwise <src>False</src> is returned.
    virtual Bool save(RecordInterface &container,
		    const String &fieldName) const;

    // Restore the coordinate system from a record.
    // A null pointer means that the restoration did not succeed - probably 
    // because fieldName doesn't exist or doesn't contain a coordinate system.
    static CoordinateSystem *restore(const RecordInterface &container,
 				   const String &fieldName);

    // Make a copy of ourself using new. The caller is responsible for calling
    // delete.
    virtual Coordinate *clone() const;

    // Convert a CoordinateSystem to FITS, i.e. fill in ctype etc. In the record
    // the keywords are vectors, it is expected that the actual FITS code will
    // split them into scalars and upcase the names. Returns False if one of the
    // keywords is already taken.
    // 
    // If writeWCS is True, attempt to write the WCS convention (Greisen and
    // Calabretta "Representation of celestial coordinates in FITS"). This is
    // a DRAFT convention evolving rapidly. It is not recommended that you
    // write this convention in general.
    // Use <src>oneRelative=True</src> to convert zero-relative pixel coordinates to
    // one-relative FITS coordinates.
    //
    // prefix gives the prefix for the FITS keywords. E.g.,
    // if prefix="c" then crval, cdelt etc. 
    // if prefix="d" then drval, ddelt etc. 
    //# Much of the work in to/from fits should be moved to the individual
    //# classes.
    Bool toFITSHeader(RecordInterface &header, 
		      IPosition &shape,
		      Bool oneRelative, 
		      char prefix = 'c', Bool writeWCS=False,
		      Bool preferVelocity=True, 
		      Bool opticalVelocity=True) const;


    // Probably even if we return False we should set up the best linear
    // coordinate that we can.
    // Use oneRelative=True to convert one-relative FITS pixel coordinates to
    // zero-relative aips++ coordinates.
    //# cf comment in toFITS.
    static Bool fromFITSHeader(CoordinateSystem &coordsys, 
			       const RecordInterface &header,
                               const IPosition& shape,
			       Bool oneRelative,
			       char prefix = 'c');

private:
    // Where we store copies of the coordinates we are created with.
    PtrBlock<Coordinate *> coordinates_p;
    
    // For coordinate[i] axis[j], 
    //    world_maps_p[i][j], if >=0 gives the location in the
    //                        input vector that maps to this coord/axis,
    //                        <= means that the axis has been removed
    //    world_tmp_p[i] a temporary vector length coord[i]->nWorldAxes()
    //    replacement_values_p[i][j] value to use for this axis if removed
    PtrBlock<Block<Int> *>     world_maps_p;
    PtrBlock<Vector<Double> *> world_tmps_p;
    PtrBlock<Vector<Double> *> world_replacement_values_p;

    // Same meanings as for the world*'s above.
    PtrBlock<Block<Int> *>     pixel_maps_p;
    PtrBlock<Vector<Double> *> pixel_tmps_p;
    PtrBlock<Vector<Double> *> pixel_replacement_values_p;

    // These temporaries all needed for the toMix function
    PtrBlock<Vector<Bool> *> worldAxes_tmps_p;
    PtrBlock<Vector<Bool> *> pixelAxes_tmps_p;
    PtrBlock<Vector<Double> *> worldOut_tmps_p;
    PtrBlock<Vector<Double> *> pixelOut_tmps_p;
    PtrBlock<Vector<Double> *> worldMin_tmps_p;
    PtrBlock<Vector<Double> *> worldMax_tmps_p;

    // Miscellaneous information about the observation associated with this
    // Coordinate System.
    ObsInfo obsinfo_p;

    // Helper functions to group common code.
    Bool mapOne(Vector<Int>& worldAxisMap, 
                Vector<Int>& worldAxisTranspose, 
                Vector<Bool>& refChange,
                const CoordinateSystem& cSys,
                const CoordinateSystem& cSys2,
                const uInt coord, const uInt coord2) const;

    void copy(const CoordinateSystem &other);
    void clear();
    Bool checkAxesInThisCoordinate(const Vector<Bool>& axes, uInt which) const;

};

#endif
