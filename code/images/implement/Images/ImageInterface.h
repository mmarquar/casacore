//# ImageInterface.h: a base class for astronomical images
//# Copyright (C) 1996,1997,1998,1999
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
//# $Id$

#if !defined(AIPS_IMAGEINTERFACE_H)
#define AIPS_IMAGEINTERFACE_H


//# Includes
#include <aips/aips.h>
#include <trial/Images/RegionHandler.h>
#include <trial/Images/ImageInfo.h>
#include <trial/Lattices/MaskedLattice.h>
#include <trial/Coordinates/CoordinateSystem.h>
#include <aips/Logging/LogIO.h>

//# Forward Declarations
template <class T> class LatticeIterInterface;
template <class T> class Vector;
template <class T> class COWPtr;
class ImageRegion;
class IPosition;
class TiledShape;
class LogIO;
class RecordInterface;
class Unit;


// <summary>
// A base class for astronomical images.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=Lattice>Lattices</linkto>
//   <li> <linkto class=CoordinateSystem>CoordinateSystem</linkto>
// </prerequisite>

// <etymology>
// The ImageInterface class name is derived from its role as the cookie cutter
// Interface base class for Images.  
// </etymology>

// <synopsis> 
// The ImageInterface class is an abstract base class. All Image classes
// should derive from this class to ensure functions which operate on Images
// will work for all Image derivations.
// 
// An Image is currently defined as an Array of pixels, a Boolean mask,
// defining which pixels are valid and coordinates to define the reference
// frame. The only concrete class currently derived from this Interface is
// PagedImage, which allows the image to be stored on disk, and only reads
// specified portions of the image into memory.  
// </synopsis>

// <example>
// As this is an abstract base class it is not possible to construct an
// instance of this object.  It can however be used as a function argument.<br>
// eg 1. (used in dImageInterface.cc)
// <srcblock>
// Float sumPixels(const ImageInterface<Float>& image){
//   uInt rowLength = image.shape()(0);
//   IPosition rowShape(image.ndim());
//   rowShape = 1; rowShape(0) = rowLength;
//   Float sumPix = 0;
//   RO_LatticeIterator<Float> iter(image, rowShape);
//   while(!iter.atEnd()){
//     sumPix += sum(iter.vectorCursor().ac());
//     iter++;
//   }
//   return sumPix;
// }
// </srcblock>
//
// The main purpose of this class is for programming objects, the following
// example is of how one would derive from ImageInterface: <br>
// eg 2.
// <srcblock>
// template <class T> class myNewImage : public ImageInterface<T>
// {
// public:
//   // default constructor
//   myNewImage();
//
//   // argumented constructor
//   myNewImage(...);
//
//   // destructor
//   ~myNewImage
//   
//   // the shape function is forced upon us by the Lattice base class
//   IPosition shape() const;
//   
//   // doGetSlice is another function required of all Lattice objects.
//   Bool doGetSlice(<Array<T>& buffer, const Slicer& section);
//
//  // etc...
// private:
//  // put the actual map data down here.
//  // etc...
// };
// </srcblock>
// </example>

// <motivation> 
// The use of abstract base classes to guide inheritance seemed appropriate
// for Images to ensure that CoordinateSystems and masking get handled
// uniformly.
// </motivation>

// <todo asof="1995/04/25">
//   <li> replace ImageCoordinates
// </todo>


template <class T> class ImageInterface: public MaskedLattice<T>
{
public: 
  ImageInterface();

  // Copy constructor (copy semantics).
  ImageInterface (const ImageInterface& other);

  ~ImageInterface();

  // Make a copy of the derived object (reference semantics).
  // <group>
  virtual MaskedLattice<T>* cloneML() const;
  virtual ImageInterface<T>* cloneII() const = 0;
  // </group>

  // Function which changes the shape of the image (N.B. the data is thrown 
  // away - the Image will be filled with nonsense afterwards)
  virtual void resize (const TiledShape& newShape) = 0;
  
  // Function which get and set the units associated with the image
  // pixels (i.e. the "brightness" unit). <src>setUnits()</src> returns
  // False if it cannot set the unit for some reason (e.g. the underlying
  // file is not writable).
  // <group>
  virtual Bool setUnits (const Unit& newUnits) = 0;
  virtual Unit units() const = 0;
  // </group>

  // Return the name of the current ImageInterface object. This will generally 
  // be a file name for images that have a persistent form.  Any path
  // before the actual file name can be optionally stripped off.
  virtual String name (const Bool stripPath=False) const = 0;

  // Functions to set or replace the coordinate information in the Image
  // Returns False on failure, e.g. if the number of axes do not match.
  //# NOTE. setCoordinateInfo should be pure virtual with a partial 
  //# implementation however SGI ntv will not generate it with -no_prelink.
  // <group>
  virtual Bool setCoordinateInfo(const CoordinateSystem& coords);
  const CoordinateSystem& coordinates() const;
  // </group>

  // Function to get a LELCoordinate object containing the coordinates.
  virtual LELCoordinates lelCoordinates() const;
  
  // Allow messages to be logged to this ImageInterface.
  // <group>
  LogIO& logSink() {return log_p;}
  const LogIO& logSink() const {return log_p;}
  // </group>
  
  // Add the TableLogSink from other to this one.
  // The default implementation in this class does nothing.
  // The derived PagedImage class implements it.
  virtual void mergeTableLogSink (const ImageInterface<T>& other);

  // Often we have miscellaneous information we want to attach to an image.
  // This is where it goes.  
  // <br>
  // Note that setMiscInfo REPLACES the information with the new information.
  // It can fail if, e.g., the underlying table is not writable.
  // <group>
  virtual const RecordInterface& miscInfo() const = 0;
  virtual Bool setMiscInfo (const RecordInterface& newInfo) = 0;
  // </group>

  // The ImageInfo object contains some miscellaneous information about the image
  // which unlike that stored in MiscInfo, has a standard list of things,
  // such as the restoring beam.
  //
  // Note that setImageInfo REPLACES the information with the new information.
  // It is up to the derived class to make the ImageInfo permanent.
  // <group>
  virtual ImageInfo imageInfo() const;
  virtual Bool setImageInfo(const ImageInfo& info);
  // </group>

  // Can the derived class handle region definition?
  // The default implementation returns False.
  virtual Bool canDefineRegion() const;

  // The "region/mask" functions are only implemented in PagedImage.
  // All other Image classes ignore these operations (they are no-op's).
  // See <linkto class=PagedImage>PagedImage</linkto> for a description
  // of the functions.
  // <group>
  virtual void defineRegion (const String& name, const ImageRegion& region,
			     RegionHandler::GroupType,
			     Bool overwrite = False);
  virtual Bool hasRegion (const String& regionName,
			  RegionHandler::GroupType = RegionHandler::Any) const;
  virtual ImageRegion* getImageRegionPtr
                            (const String& name,
			     RegionHandler::GroupType = RegionHandler::Any,
			     Bool throwIfUnknown = True) const;
  virtual void renameRegion (const String& newName,
			     const String& oldName,
			     RegionHandler::GroupType = RegionHandler::Any,
			     Bool overwrite = False);
  virtual void removeRegion (const String& name,
			     RegionHandler::GroupType = RegionHandler::Any,
			     Bool throwIfUnknown = True);
  virtual Vector<String> regionNames
                   (RegionHandler::GroupType = RegionHandler::Any) const;
  virtual void setDefaultMask (const String& regionName);
  virtual String getDefaultMask() const;
  // </group>

  // Get a region belonging to the image.
  // An exception is thrown if the region does not exist.
  ImageRegion getRegion (const String& regionName,
			 RegionHandler::GroupType = RegionHandler::Any) const;

  // Make a unique region name from the given root name, thus make it such
  // that the name is not already in use for a region or mask.
  // The root name is returned if it is already unique.
  // Otherwise a number is appended to the root name to make it unique.
  // The number starts at the given number and is incremented until the name
  // is unique.
  String makeUniqueRegionName (const String& rootName,
			       uInt startNumber = 1) const;

  // Check class invariants. 
  virtual Bool ok() const = 0;
  
  //<group>
  // Function to work around the g++ upcast bug
  ImageInterface<T>& ic() {return *this;}
  const ImageInterface<T>& ic() const {return *this;}
  //</group>
 
 
protected:
  // Assignment (copy semantics) is only useful for derived classes.
  ImageInterface& operator= (const ImageInterface& other);

  // It is the job of the derived class to make the coordinate system valid.
  CoordinateSystem coords_p;
  LogIO log_p;

  // It is the job of the derived class to make the ImageInfo valid
  ImageInfo imageInfo_p;

  Bool restoreImageInfo(const RecordInterface& rec);

};


#endif
