//# TiledFileAccess.h: Tiled access to an array in a file
//# Copyright (C) 2001
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

#if !defined(AIPS_TILEDFILEACCESS_H)
#define AIPS_TILEDFILEACCESS_H


//# Includes
#include <aips/Tables/TSMCube.h>

//# Forward Declarations
class TiledFileHelper;
class Slicer;


// <summary>
// Tiled access to an array in a file.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="tTiledFileAccess.cc">
// </reviewed>

// <prerequisite>
//# Classes you should understand before using this one.
//   <li> Description of Tiled Storage Manager in module file
//        <linkto module=Tables:TiledStMan>Tables.h</linkto>
//   <li> <linkto class=ROTiledStManAccessor>ROTiledStManAccessor</linkto>
//        for a discussion of the maximum cache size
// </prerequisite>

// <synopsis> 
// TiledFileAccess is a class that makes it possible to access
// an arbitrary array in a file using the tiled storage manager classes.
// It can handle arrays of any type supported by the tiled storage
// managers. The array can be in local or in canonical format.
// <p>
// See <linkto class=TiledStManAccessor>TiledStManAccessor</linkto>
// for a more detailed discussion.
// </synopsis> 

// <motivation>
// This class makes it possible to access an image in a FITS file.
// </motivation>

// <example>
// <srcblock>
//  // Define the object which also opens the file.
//  // The array starts at offset 2880.
//  TiledFileAccess<Float> tfa ("fits.file", 2880, IPosition(2,512,512),
//                              IPosition(2,512,1));
//  // Get all the data.
//  Array<Float> data = tfa.get (Slicer(IPosition(2,0,0), tfa.shape()));
// </srcblock>
// </example>

//# <todo asof="$DATE:$">
//# A List of bugs, limitations, extensions or planned refinements.
//# </todo>


template<class T> class TiledFileAccess
{
public:
  // Create a TiledFileAccess object.
  TiledFileAccess (const String& fileName, Int64 fileOffset,
		   const IPosition& shape, const IPosition& tileShape,
		   uInt maximumCacheSize=0, Bool writable=False,
		   Bool canonical=True);

  ~TiledFileAccess();

  // Is the file writable?
  Bool isWritable() const
    { return itsWritable; }

  // Get part of the array.
  // The Array object is resized if needed.
  // <group>
  Array<T> get (const Slicer& section);
  void get (Array<T>&, const Slicer& section);
  // </group>

  // Put part of the array.
  void put (const Array<T>&, const Slicer& section);

  // Flush the cache.
  void flush()
    { itsCube->flushCache(); }

  // Empty the cache.
  // It will flush the cache as needed and remove all buckets from it
  // resulting in a possibly large drop in memory used.
  // It'll also clear the <src>userSetCache_p</src> flag.
  void clearCache()
    { itsCube->emptyCache(); }

  // Show the cache statistics.
  void showCacheStatistics (ostream& os) const
    { itsCube->showCacheStatistics (os); }

  // Get the shape of the array.
  const IPosition& shape() const
    { return itsCube->cubeShape(); }

  // Get the shape of the tiles.
  const IPosition& tileShape() const
    { return itsCube->tileShape(); }

  // Set the maximum cache size (in bytes).
  // 0 means no maximum.
  void setMaximumCacheSize (uInt nbytes);

  // Get the maximum cache size (in bytes).
  uInt maximumCacheSize() const;

  // Get the current cache size (in buckets).
  uInt cacheSize() const
    { return itsCube->cacheSize(); }

  // Set the cache size using the given access pattern.
  // <group>
  void setCacheSize (const IPosition& sliceShape,
		     const IPosition& axisPath,
		     Bool forceSmaller=True)
    { itsCube->setCacheSize (sliceShape, IPosition(), IPosition(),
			     axisPath, forceSmaller, True); }
  void setCacheSize (const IPosition& sliceShape,
		     const IPosition& windowStart,
		     const IPosition& windowLength,
		     const IPosition& axisPath,
		     Bool forceSmaller=True)
    { itsCube->setCacheSize (sliceShape, windowStart, windowLength,
			     axisPath, forceSmaller, True); }
  // </group>

  // Set the cache size for accessing the data.
  // When the give cache size exceeds the maximum cache size with more
  // than 10%, the maximum cache size is used instead.
  // <br>When forceSmaller is False, the cache is not resized when the
  // new size is smaller.
  void setCacheSize (uInt nbuckets, Bool forceSmaller=True)
    { itsCube->setCacheSize (nbuckets, forceSmaller, True); }



private:
  // Forbid copy constructor and assignment.
  TiledFileAccess (const TiledFileAccess&);
  TiledFileAccess& operator= (const TiledFileAccess&);
  // </group>


  TSMCube*         itsCube;
  TiledFileHelper* itsTSM;
  uInt             itsLocalPixelSize;
  Bool             itsWritable;
};


#endif
