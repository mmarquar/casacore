//# PagedImage.h: reading, storing and manipulating astronomical images
//# Copyright (C) 1994,1995,1996,1997
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

#if !defined(AIPS_PAGEDIMAGE_H)
#define AIPS_PAGEDIMAGE_H

#if defined(_AIX)
#pragma implementation ("PagedImage.cc")
#endif 

#include <aips/aips.h>

#include <trial/Images/ImageInterface.h>
#include <trial/Lattices/PagedArray.h>
#include <aips/Tables/Table.h>
#include <aips/Utilities/DataType.h>
#include <aips/Tables/TableRecord.h>

#include <aips/Logging/LogIO.h>

//# predeclarations
class IPosition;
class LatticeNavigator;
class Slicer;
template <class T> class Array;
template <class T> class COWPtr;
template <class T> class RO_LatticeIterInterface;
template <class T> class LatticeIterInterface;
class ostream;
class String;

// <summary> read, store, and manipulate astronomical images </summary>
//
// <reviewed reviewer="" date="" tests="tPagedmage.cc" demos="dPagedImage.cc>
// </reviewed>
//
// <prerequisite>
// <list>
//   <item> CoordinateSystem
//   <item> ImageInterface
//   <item> Lattice
//   <item> LatticeIterator
//   <item> LatticeNavigator
// </list>
// </prerequisite>
//
// <etymology>
// The PagedImage name comes from its role as the Image class with paging 
// from persistant memory.  Users are thus invited to treat the 
// ArrayImage instances like AIPS++ Lattices  
// </etymology>
//
// <synopsis> 
// All AIPS++ Images are Lattices.  They may be treated like any other Lattice;
// getSlice(...), putSlice(...), LatticeIterator for iterating, etc...
// ArrayImages contain a map, a mask for that map, and coordinate 
// information.  This provides a Lattice interface for images and their 
// respective coordinates.  Additional functionality is defined by the 
// ImageInterface class. 
//
// You can use the global function <src>imagePixelType</src> to determine
// what the pixel type of an image is before you open the image if your
// code can work with Images of many possible types, or for error checking.
// </synopsis> 
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// The size of astronomical data can be very large.  The ability to fit an 
// entire image into random access memory cannot be guaranteed.  Paging from 
// disk pieces of the image appeared to be the way to deal with this problem.
// </motivation>
//
// <todo asof="1996/09/04">
//   <li> The CoordinateSystem::store() function returns a TableRecord.  That
// TableRecord should be stored in the same row as our image.  This will 
// allow ImageStack members to have their own coordinate frames.
//   <li> Create a proper table log sink, not just the null sink.
// </todo>

template <class T> class PagedImage: public ImageInterface<T>
{
public: 
  // construct a new Image from an IPosition and coordinate information. Data
  // will be stored in the argument table, masking is created if True
  PagedImage(const IPosition &mapShape, const CoordinateSystem &coordinateInfo,
	     Table &table, Bool masking = False, uInt rowNumber = 0);
  
  // construct a new Image from an IPosition and coordinate information. Table
  // will be stored in the named file, masking is created if True.
  PagedImage(const IPosition &mapShape, const CoordinateSystem &coordinateInfo,
	     const String &nameOfNewFile, Bool masking = False, 
	     uInt rowNumber = 0);
  
  // reconstruct an image from a pre-existing file
  PagedImage(Table &table, uInt rowNumber = 0);
  
  // reconstruct an image from a pre-existing file
  PagedImage(const String &filename, uInt rowNumber = 0);
  
  // the copy constructor. (reference semantics)
  PagedImage(const PagedImage<T> &other);

  // destructor
  ~PagedImage();
  
  // assignment operator. (reference semantics)
  PagedImage<T> &operator=(const PagedImage<T> &other);
  
  // Function to change the name of the Table file on disk.
  // PagedImage is given a file name at construction time.  You may change
  // that name here.
  void rename(const String &newName);

  // returns the current Table name
  virtual String name() const;

  // function to change the row number where the PagedImage is stored
  // PagedImage is given a TableColumn row number at construction time.  
  // You may change the row number. <note>  This function is intended
  // for ImageStack use only. </note>
  void changeRowNumber(uInt rowNum);

  // returns the current TableColumn row number 
  uInt rowNumber() const;

  // return the shape of the image
  IPosition shape() const;

  // change the shape of the image (N.B. the data is thrown away)
  void resize(const IPosition &newShape);

  // functions which extract an array from the map.
  // <group>   
  Bool getSlice(COWPtr<Array<T> > &buffer, const IPosition &start, 
		const IPosition &shape, const IPosition &stride,
		Bool removeDegenerateAxes=False) const;
  
  Bool getSlice(COWPtr<Array<T> > &buffer, const Slicer &theSlice, 
		Bool removeDegenerateAxes=False) const;
  
  Bool getSlice(Array<T> &buffer, const IPosition &start, 
		const IPosition &shape, const IPosition &stride,
		Bool removeDegenerateAxes=False);
  
  Bool getSlice(Array<T> &buffer, const Slicer &theSlice, 
		Bool removeDegenerateAxes=False);
  // </group>
  
  // function to replace the values in the map with soureBuffer.
  void putSlice(const Array<T> &sourceBuffer, const IPosition &where,
		const IPosition &stride);

  // replace every element, x, of the lattice with the result  of f (x).
  // you must pass in the address of the function -- so the function
  // must be declared and defined in the scope of your program.  
  // both versions of apply require a function that accepts a single 
  // argument of type T (the Lattice template actual type) and returns
  // a result of the same type.  The first apply expects a function with
  // an argument passed by value; the second expects the argument to
  // be passed by const reference.  The first form ought to run faster
  // for the built-in types, which may be an issue for large Lattices
  // stored in memory, where disk access is not an issue.
  // <group>
  void apply(T (*function)(T));
  void apply(T (*function)(const T &));
  void apply(const Functional<T,T> &function);
  // </group>

  // function which returns True if the image has a mask, returns False 
  // otherwise.
  virtual Bool isMasked() const;

  // return the whole mask Lattice to allow iteration or Lattice functions.
  // <group>    
  const Lattice<Bool> &mask() const;
  Lattice<Bool> &mask();
  // </group>
  
  // addition operator. Masks are summed, i.e. if the pixel is 
  // masked in either image, it is masked in the final.
  PagedImage<T> &operator+=(const PagedImage<T> &other);

  // addition operator. Lattice.
  PagedImage<T> &operator+=(const Lattice<T> &other);

  // addition operator. Constant
  PagedImage<T> &operator+=(const T &val);

  // function to set the default value.  This value will be returned when 
  // access to a masked element is attempted.
  void setDefaultValue(const T &newValue);
  
  // function to return the value of the stored default value.
  const T &defaultValue() const;

  // return the table holding the data
  Table table();

  // Flushes the new coordinate system to disk if the table is writable.
  virtual Bool setCoordinateInfo(const CoordinateSystem &coords);

  // check for symmetry in data members
  virtual Bool ok() const;

  // These are the true implementations of the paran operator.
  // <note> Not for public use </note>
  // <group>
  T getAt(const IPosition &where) const;
  void putAt(const T &value, const IPosition &where);
  // </group>

  // Often we have miscellaneous information we want to attach to an image.
  // This is how it is done. Eventually we will want to register that some
  // of the information is to be destroyed if the image changes so that, e.g.
  // data max/min values can be removed if the image changes.
  //
  // Note that setMiscInfo REPLACES the information with the new information.
  // If can fail if, e.g., the underlying table is not writable.
  // <group>
  virtual const RecordInterface &miscInfo() const;
  virtual Bool setMiscInfo(const RecordInterface &newInfo);
  // </group>

  
  // these are the implementations of the letters for the envelope Iterator
  // class <note> Not for public use </note>
  //<group>
  RO_LatticeIterInterface<T> *makeIter(
				    const LatticeNavigator &navigator) const;
  RO_LatticeIterInterface<T> *makeIter(const IPosition &cursorShape) const;
  LatticeIterInterface<T> *makeIter(const LatticeNavigator &navigator);
  LatticeIterInterface<T> *makeIter(const IPosition &cursorShape);
  //</group>

  // Help the user pick a cursor for most efficient access if he only wants
  // pixel values and doesn't care about the order. Usually just use
  // <src>IPosition shape = pa.niceCursorShape(pa.maxPixels());</src>
  virtual uInt maxPixels() const;
  virtual IPosition niceCursorShape(uInt maxPixels) const;

  // Maximum size - not necessarily all used. In pixels.
  uInt maximumCacheSize() const;

  // Set the maximum (allowd) cache size as indicated.
  void setMaximumCacheSize(uInt howManyPixels);

  // Set the cache size as to "fit" the indicated path.
  void setCacheSizeFromPath(const IPosition& sliceShape,
			    const IPosition& windowStart,
			    const IPosition& windowLength,
			    const IPosition& axisPath);
  // Clears and frees up the caches, but the maximum allowd cache size is 
  // unchanged from when setCacheSize was called
  void clearCache() const;

  // Report on cache success
  void showCacheStatistics(ostream &os);
private:  
  // the default constructor -- useless.
  PagedImage();
  void restore_units();
  void save_units();
  void report_mask();
  void check_conformance(const Lattice<T> &other);
  void openTable(const String &filename);
  void openTable(const String &filename) const;
  void setTableType();

  Table table_p;
  PagedArray<T> map_p;
  PagedArray<Bool> *mask_p;
  T defaultvalue_p;
  LogIO sink_p;
};


//# A nasty - the column name is hard-coded into this function, needs to
//# be centralized somewhere.
// <linkfrom anchor="pixeltype" classes="PagedImage">
// <here>Global functions</here> for PagedImage.
// </linkfrom>
// Determine the pixel type in the PagedImage contained in
// <src>fileName</src>.  If the file doesn't appear to be a Table or cannot
// be opened, TpOther is returned.
// <group name="pixeltype")
    DataType imagePixelType(const String &fileName);
// </group>

#endif
