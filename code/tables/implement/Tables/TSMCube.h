//# TSMCube.h: Tiled hypercube in a table
//# Copyright (C) 1995,1996,1997
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

#if !defined(AIPS_TSMCUBE_H)
#define AIPS_TSMCUBE_H

#if defined(_AIX)
#pragma implementation ("TSMCube.cc")
#endif 

//# Includes
#include <aips/aips.h>
#include <aips/Tables/TSMShape.h>
#include <aips/Containers/Record.h>
#include <aips/Lattices/IPosition.h>
#include <aips/OS/Conversion.h>

//# Forward declarations
class TiledStMan;
class TSMFile;
class TSMColumn;
class BucketCache;
template<class T> class Block;
#if defined(__KCC)
#include <iosfwd.h>
#else
class ostream;
#endif


// <summary>
// Tiled hypercube in a table
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="">
// </reviewed>

// <prerequisite>
//# Classes you should understand before using this one.
//   <li> <linkto class=TiledStMan>TiledStMan</linkto>
//   <li> <linkto class=ROTiledStManAccessor>ROTiledStManAccessor</linkto>
//        for a discussion of the maximum cache size
//   <li> <linkto class=TSMFile>TSMFile</linkto>
//   <li> <linkto class=BucketCache>BucketCache</linkto>
// </prerequisite>

// <etymology>
// TSMCube represents a hypercube in the Tiled Storage Manager.
// </etymology>

// <synopsis>
// TSMCube defines a tiled hypercube. The data is stored in a TSMFile
// object and accessed using a BucketCache object. The hypercube can
// be extensible in its last dimension to support tables with a size
// which is not known in advance.
// <br>
// Normally hypercubes share the same TSMFile object, but extensible
// hypercubes have their own TSMFile object (to be extensible).
// When the hypercolumn has multiple data columns, their cells share the same
// tiles. Per tile data column A appears first, thereafter B, etc..
// <br>
// The data in the cache is held in external format and is converted
// when accessed. The alternative would be to hold it in the cache in
// local format and convert it when read/written from the file. It was
// felt that the latter approach would generate more needless conversions.
// <p>
// The possible id and coordinate values are stored in a Record
// object. They are written in the main hypercube AipsIO file.
// <p>
// TSMCube uses the maximum cache size set for a Tiled Storage manager.
// The description of class
// <linkto class=ROTiledStManAccessor>ROTiledStManAccessor</linkto>
// contains a discussion about the effect of setting the maximum cache size.
// </synopsis> 

// <motivation>
// TSMCube encapsulates all operations on a hypercube.
// </motivation>

//# <todo asof="$DATE:$">
//# A List of bugs, limitations, extensions or planned refinements.
//# </todo>


class TSMCube
{
public:
    // Construct an object with a still undefined hypercube.
    // It can be filled later with setShape.
    // It is used by TiledCellStMan which may know its hypercube shape
    // only when ArrayColumn::setShape is done.
    TSMCube (TiledStMan* stman, TSMFile* file);

    // Construct the hypercube using the given file with the given shape.
    // The record contains the id and possible coordinate values.
    TSMCube (TiledStMan* stman, TSMFile* file,
	     const IPosition& cubeShape,
	     const IPosition& tileShape,
	     const Record& values);

    // Reconstruct the hypercube by reading its data from the AipsIO stream.
    // It will link itself to the correct TSMFile. The TSMFile objects
    // must have been reconstructed in advance.
    TSMCube (TiledStMan* stman, AipsIO& ios);

    ~TSMCube();

    // Flush the data in the cache.
    void flushCache();

    // Clear the cache, so data will be reread.
    void clearCache();

    // Empty the cache.
    // It will flush the cache as needed and remove all buckets from it
    // resulting in a possibly large drop in memory used.
    void emptyCache();

    // Show the cache statistics.
    void showCacheStatistics (ostream& os) const;

    // Put the data of the object into the AipsIO stream.
    void putObject (AipsIO& ios);

    // Get the data of the object from the AipsIO stream.
    // It returns the data manager sequence number.
    uInt getObject (AipsIO& ios);

    // Is the hypercube extensible?
    Bool isExtensible() const;

    // Get the bucket size (which is the length of a tile in external format).
    uInt bucketSize() const;

    // Get the lenghth of a tile in local format.
    uInt localTileLength() const;

    // Try to size the cache that it can hold the tiles holding the
    // amount of pixels defined by start,end,stride.
    // When forceSmaller is True, the cache will be made smaller
    // when less space is needed.
    Bool sizeCache (const IPosition& start, const IPosition& end,
		    const IPosition& stride, Bool forceSmaller = False);

    // Set the hypercube shape.
    // This is only possible when the shape was not defined yet.
    void setShape (const IPosition& cubeShape, const IPosition& tileShape);

    // Get the shape of the hypercube.
    const IPosition& cubeShape() const;

    // Get the shape of the tiles.
    const IPosition& tileShape() const;

    // Get the shape of the data cells in the cube.
    IPosition cellShape() const;

    // Get the size of a coordinate (i.e. the number of values in it).
    // If not defined, it returns zero.
    uInt coordinateSize (const String& coordinateName) const;

    // Get the record containing the id and coordinate values.
    // It is used by TSMIdColumn and TSMCoordColumn.
    // <group>
    const Record& valueRecord() const;
    Record& rwValueRecord();
    // </group>

    // Test if the id values match.
    Bool matches (const PtrBlock<TSMColumn*> idColSet,
		  const Record& idValues);

    // Extend the last dimension of the cube with the given number.
    // The record can contain the coordinates of the elements added.
    void extend (uInt nr, const Record& coordValues,
		 const TSMColumn* lastCoordColumn);

    // Extend the coordinates vector for the given coordinate
    // to the given length with the given coordValues.
    // It will be initialized to zero if no coordValues are given.
    // If the coordinate vector does not exist yet, it will be created.
    void extendCoordinates (const Record& coordValues,
			    const String& coordName, uInt length);

    // Read or write a section in the cube.
    // It is assumed that the section buffer is long enough.
    void accessSection (const IPosition& start, const IPosition& end,
			char* section, uInt colnr,
			uInt localPixelSize, Bool writeFlag);

    // Read or write a section in a strided way.
    // It is assumed that the section buffer is long enough.
    void accessStrided (const IPosition& start, const IPosition& end,
			const IPosition& stride,
			char* section, uInt colnr,
			uInt localPixelSize, Bool writeFlag);

    // Set the cache size for the given slice and access path.
    void setCacheSize (const IPosition& sliceShape,
		       const IPosition& windowStart,
		       const IPosition& windowLength,
		       const IPosition& axisPath,
		       Bool forceSmaller);

    // Resize the cache object.
    // When forceSmaller is False, the cache will only be resized
    // when it grows.
    // When the given size exceeds the maximum size with more
    // than 10%, the maximum size will be used.
    // The cacheSize has to be given in buckets.
    void setCacheSize (uInt cacheSize, Bool forceSmaller);

    // Validate the cache size (in buckets).
    // This means it will return the given cache size if smaller
    // than the maximum cache size. Otherwise the maximum is returned.
    uInt validateCacheSize (uInt cacheSize) const;

private:
    // Forbid copy constructor.
    TSMCube (const TSMCube&);

    // Forbid assignment.
    TSMCube& operator= (const TSMCube&);

    // Initialize the various variables.
    void setup();

    // Adjust the tile shape to the hypercube shape.
    // A size of 0 gets set to 1.
    // A tile size > cube size gets set to the cube size.
    IPosition adjustTileShape (const IPosition& cubeShape,
			       const IPosition& tileShape) const;
    
    // Get the cache object.
    // This will construct the cache object if not present yet.
    BucketCache* getCache();

    // Construct the cache object (if not constructed yet).
    void makeCache();

    // Access a line in a more optimized way.
    void accessLine (char* section, uInt pixelOffset,
		     uInt localPixelSize,
		     Bool writeFlag, BucketCache* cachePtr,
		     const IPosition& startTile, uInt endTile,
		     const IPosition& startPixelInFirstTile,
		     uInt endPixelInLastTile,
		     uInt lineIndex);

    // Define the callback functions for the BucketCache.
    // <group>
    static char* readCallBack (void* owner, const char* external);
    static void writeCallBack (void* owner, char* external,
			       const char* local);
    static char* initCallBack (void* owner);
    static void deleteCallBack (void* owner, char* buffer);
    // </group>

    // Define the functions doing the actual read and write of the 
    // data in the tile and converting it to/from local format.
    // <group>
    char* TSMCube::readTile (const char* external);
    void writeTile (char* external, const char* local);
    // </group>


    //# Declare member variables.
    // Pointer to the parent storage manager.
    TiledStMan*     stmanPtr_p;
    // The values of the possible id and coordinate columns.
    Record          values_p;
    // Is the hypercube extensible?
    Bool            extensible_p;
    // Dimensionality of the hypercube.
    uInt            nrdim_p;
    // Number of tiles in the hypercube.
    uInt            nrTiles_p;
    // The shape of the hypercube.
    IPosition       cubeShape_p;
    // The shape of the tiles in the hypercube.
    IPosition       tileShape_p;
    // The number of tiles in each hypercube dimension.
    IPosition       tilesPerDim_p;
    // Precomputed tileShape information.
    TSMShape        expandedTileShape_p;
    // Precomputed tilesPerDim information.
    TSMShape        expandedTilesPerDim_p;
    // Number of tiles in all but last dimension (used when extending).
    uInt            nrTilesSubCube_p;
    // The tilesize in pixels.
    uInt            tileSize_p;
    // Pointer to the TSMFile object holding the data.
    TSMFile*        filePtr_p;
    // Offset in the TSMFile object where the data of this hypercube starts.
    uInt            fileOffset_p;
    // Offset for each data column in a tile (in external format).
    Block<uInt>     externalOffset_p;
    // Offset for each data column in a tile (in local format).
    Block<uInt>     localOffset_p;
    // The bucket size in bytes (is equal to tile size in bytes).
    uInt            bucketSize_p;
    // The tile size in bytes in local format.
    uInt            localTileLength_p;
    // The bucket cache.
    BucketCache*    cache_p;
};



inline BucketCache* TSMCube::getCache()
{
    if (cache_p == 0) {
	makeCache();
    }
    return cache_p;
}
inline uInt TSMCube::bucketSize() const
{ 
    return bucketSize_p;
}
inline uInt TSMCube::localTileLength() const
{ 
    return localTileLength_p;
}
inline const IPosition& TSMCube::cubeShape() const
{ 
    return cubeShape_p;
}
inline const IPosition& TSMCube::tileShape() const
{ 
    return tileShape_p;
}
inline const Record& TSMCube::valueRecord() const
{
    return values_p;
}
inline Record& TSMCube::rwValueRecord()
{
    return values_p;
}



#endif
