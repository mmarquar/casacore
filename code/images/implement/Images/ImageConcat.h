//# ImageConcat.h: concatenate images along an axis
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

#if !defined(AIPS_IMAGECONCAT_H)
#define AIPS_IMAGECONCAT_H


#include <aips/aips.h>
#include <trial/Lattices/LatticeConcat.h>
#include <trial/Images/ImageInterface.h>
#include <aips/Tables/TableRecord.h>
class Unit;
class CoordinateSystem;

template <class T> class ImageSummary;
template <class T> class MaskedLattice;
template <class T> class Vector;


// <summary>
// Concatenates images along a specified axis
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=LatticeConcat>LatticeConcat</linkto> 
//   <li> <linkto class=ImageInterface>ImageInterface</linkto> 
// </prerequisite>

// <etymology>
// This is a class designed to concatenate images along a specified axis
// </etymology>

// <synopsis>
// This is a class designed to concatenate images along a specified
// axis. This means you can join them together.  E.g.,
// join images of shape  [10,20,30] and [10,20,40] into a lattice
// of shape [10,20,70].
//
// The ImageConcat object does not copy the input images, it
// just references them.  You can use the Lattice<T>::copyData(Lattice<T>)
// function to fill an output image with the concatenated input images
//
// If you use the putSlice function, be aware that it will change the underlying
// images if they are writable.
// If you use the putMaskSlice function, be aware that it will change the underlying
// images if their masks are writable.
//
// You can also concatenate  a lattice to an image.  
// </synopsis>
//
// <example>
// <srcblock>
//      IPosition shape(2, 10, 20);
//      PagedImage<Float> im1(shape, CoordinateUtil::defaultCoords2D(),
//                            "tImageConcat_tmp1.img");
//      im1.set(1.0);
//      PagedImage<Float> im2(shape, CoordinateUtil::defaultCoords2D(),
//                            "tImageConcat_tmp2.img");
//      im2.set(2.0);
//
//// Make concatenator for axis 0
//
//      ImageConcat<Float> concat(0);
//
//// Relax coordinate constraints
//
//      concat.setImage(im1, True);
//      concat.setImage(im2, True);
//
//// Make output image  and mask (if required, which it will be in this case)
//
//      PagedImage<Float> im3(concat.shape(), CoordinateUtil::defaultCoords2D(),
//                            "tImageConcat_tmp3.img");
//
//// Copy to output
//
//      im3.copyData(concat);
//
// </srcblock>
// See tImageConcat.cc for more examples.
// </example>


// <motivation>
// Image concatentation is a useful enduser requirement.  
// </motivation>

// <todo asof="1999/10/23">
//   <li> Offer the ability to increase the dimensionality of
//        the output image
// </todo>


template <class T> class ImageConcat : public ImageInterface<T>
{
public:

// Constructor. Specify the pixel axis for concatenation
   ImageConcat (uInt axis);

// Default constructor, Sets the concatenation axis to 0
   ImageConcat ();

// Copy constructor (reference semantics)
   ImageConcat(const ImageConcat<T> &other);

// Destructor
   virtual ~ImageConcat ();

// Assignment operator (reference semantics)
   ImageConcat<T> &operator=(const ImageConcat<T> &other);

// Sets a new image into the list to be concatenated.  
// If relax is False, throws an exception if the images
// are not contiguous along the concatenation axis.
// If relax is True, it will create a non-regular TabularCoordinate
// for non-contiguous images if the coordinaets are monotonic.
// Otherwise, it just uses the coordinates of the image
   void setImage (ImageInterface<T>& image, Bool relax);

// Sets a new lattice into the list to be concatenated.  
// You can only concatenate a lattice with an image if
// you have first used setImage to set an image (this
// provides the CooridinateSystem information)
   void setLattice (MaskedLattice<T>& lattice);

// Return the number of images/lattices set so far
   uInt nimages() const {return latticeConcat_p.nlattices();};

// Returns the current concatenation axis (0 relative)
   uInt axis () const {return latticeConcat_p.axis();};

// Returns the number of dimensions of the *input* images/lattices
// Returns 0 if none yet set. 
   uInt imageDim() const {return latticeConcat_p.latticeDim();};

// Acquire or release locks.
// These functions operate on all of the underlying lattices
// <group>
  virtual Bool lock (FileLocker::LockType type, uInt nattempts) 
    {return latticeConcat_p.lock(type, nattempts);};
  virtual void unlock() {latticeConcat_p.unlock();};
  virtual Bool hasLock (FileLocker::LockType type) const
    {return latticeConcat_p.hasLock(type);};
// </group>

// Return the name of the current ImageInterface object.
// Since many images may be concatenated, there is no
// sensible name.  So returns the string "Concatenation :"
   virtual String name(const Bool stripPath=False) const {return String("Concatenation :");};

// Make a copy of the object (reference semantics).
   virtual ImageInterface<T>* cloneII() const {return new ImageConcat(*this);};

// Has the object really a mask?
   virtual Bool isMasked() const {return latticeConcat_p.isMasked();};
  
// Get the region used (always returns 0)
   virtual const LatticeRegion* getRegionPtr() const {return latticeConcat_p.getRegionPtr();};

// If all of the underlying lattices are writable returns True
   virtual Bool isWritable() const {return latticeConcat_p.isWritable();};
   
// Is the mask writable?
// If all of the undcontributing lattice masks are writable returns True
   virtual Bool isMaskWritable() const {return latticeConcat_p.isMaskWritable();};
  
// Return the shape of the concatenated image
   virtual IPosition shape() const {return latticeConcat_p.shape();};

  
// Return the best cursor shape.  This isn't very meaningful  for an ImageConcat
// Image since it isn't on disk !  But if you do copy it out, this is
// what you should use.  The maxPixels aregument is ignored.   
   virtual IPosition doNiceCursorShape (uInt maxPixels) const;

// Do the actual get of the data.
// The return value is always False, thus the buffer does not reference
// another array.  Generally the user should use function getSlice
   virtual Bool doGetSlice (Array<T>& buffer, const Slicer& section);
   
// Do the actual get of the mask data.
// The return value is always False, thus the buffer does not reference
// another array. Generally the user should use function getMaskSlice
   virtual Bool doGetMaskSlice (Array<Bool>& buffer, const Slicer& section);

// Do the actual put of the data into the Lattice.  This will change the underlying
// images (if they are writable) that were used to create the
// ImageConcat object. It throws an exception if not writable.
// Generally the user should use function putSlice
   virtual void doPutSlice (const Array<T>& sourceBuffer,
                            const IPosition& where,
                            const IPosition& stride);      

// Do the actual put of a section of the mask into the Lattice.   This will change
// the underlying images (if they are writable) that were used to create the
// ImageConcat object. It throws an exception if not writable.
// Generally the user should use function putMaskSlice
   virtual void doPutMaskSlice (const Array<Bool>& buffer,
                                const IPosition& where,
                                const IPosition& stride);

// Throws an excpetion as you cannot reshape an ImageConcat object
   virtual void resize(const TiledShape&);

// Function which get and set the units associated with the image
// pixels (i.e. the "brightness" unit). <src>units</src>
// returns the Unit of the first image set (or whatever you set
// with <src>setUnits</src> (which always returns True)
// <group>
  virtual Bool setUnits(const Unit& newUnits);
  virtual Unit units() const;
// </group>

// Functions to set or replace the coordinate information.  
// Although the ImageConcat is not writable, you can change the
// CoordinateSystem (as long as it is consistent with the old one)
  virtual Bool setCoordinateInfo(const CoordinateSystem& coords);

// Function to get a LELCoordinate object containing the coordinates.
  virtual LELCoordinates lelCoordinates() const;

// Often we have miscellaneous information we want to attach to an image.
// <src>setMiscInfo</src> throws an exception as ImageConcat is not
// writable.  <src>miscInfo</src> returns a record with all of the
// miscInfo records from each image in it, one per numbered field
// <group>
  virtual const RecordInterface &miscInfo() const;
  virtual Bool setMiscInfo(const RecordInterface &newInfo);
// </group>

// Check class invariants.
  virtual Bool ok() const;
  
// These are the implementations of the LatticeIterator letters.
// <note> not for public use </note>
  virtual LatticeIterInterface<T> *makeIter(const LatticeNavigator &navigator) const;


private:

   LatticeConcat<T> latticeConcat_p;
   Unit unit_p;
   Bool warnAxisNames_p, warnAxisUnits_p, warnImageUnits_p;
   Bool warnContig_p, warnRefPix_p, warnRefVal_p, warnInc_p;
   Bool isContig_p;
   mutable TableRecord rec_p;
   Vector<Bool> isImage_p;
//
   Double coordConvert(Int& worldAxis, LogIO& os,
                       const CoordinateSystem& cSys,
                       uInt axis, Double pixelCoord) const;
   void checkContiguous (Bool& isContig, Bool& warnContig, const IPosition& shape1,
                         const CoordinateSystem& cSys1,
                         const CoordinateSystem& cSys2,
                         LogIO& os, uInt axis, Bool relax);
   void checkCoordinates (Bool& warnRefPix, Bool& warnRefVal,
                          Bool& warnInc, LogIO& os,
                          const ImageSummary<T>& sum1,
                          const ImageSummary<T>& sum2,
                          uInt axis, Bool relax);
   Vector<Int> makeNewStokes(const Vector<Int>& stokes1,
                             const Vector<Int>& stokes2);
   void setCoordinates();

};
#endif
