//# SSMBase.h: Base class of the Standard Storage Manager
//# Copyright (C) 2000
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

#if !defined(AIPS_SSMBASE_H)
#define AIPS_SSMBASE_H


//# Includes
#include <aips/aips.h>
#include <aips/Tables/DataManager.h>
#include <aips/Containers/Block.h>

//# Forward declarations
class BucketCache;
class BucketFile;
class StManArrayFile;
class SSMIndex;
class SSMColumn;
class SSMStringHandler;

// <summary>
// Base class of the Standard Storage Manager
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="tStandardStMan.cc">
// </reviewed>

// <prerequisite>
//# Classes you should understand before using this one.
//   <li> <linkto class=StandardStMan>StandardStMan</linkto>
//   <li> <linkto class=SSMColumn>SSMColumn</linkto>
// </prerequisite>

// <etymology>
// SSMBase is the base class of the Standard Storage Manager.
// </etymology>

// <synopsis>
// The behaviour of this class is described in
// <linkto class="StandardStMan:description">StandardStMan</linkto>.

// <motivation>
// The public interface of SSMBase is quite large, because the other
// internal SSM classes need these functions. To have a class with a
// minimal interface for the normal user, class <src>StandardStMan</src>
// is derived from it.
// <br>StandardStMan needs an isA- instead of hasA-relation to be
// able to bind columns to it in class <linkto class=SetupNewTable>
// SetupNewTable</linkto>.
// </motivation>

// <todo asof="$DATE:$">
//# A List of bugs, limitations, extensions or planned refinements.
//   <li> Removed AipsIO argument from open and close.
//   <li> When only 1 bucket in use addcolumn can check if there's enough
//        room to fit the new column (so rearange the bucket) in the free
//        row space.
// </todo>


class SSMBase: public DataManager
{
public:
  // Create an Standard storage manager without a name.
  explicit SSMBase (uInt aBucketSize=131072,uInt aCacheSize=1);
  
  // Create an Standard storage manager with the given name.
  // The bucket size has to be given in bytes
  explicit SSMBase (const String& aDataManName,
		    uInt aBucketSize=131072,
		    uInt aCacheSize=1);
  
  ~SSMBase();
  
  // Clone this object.
  // It does not clone SSMColumn objects possibly used.
  // The caller has to delete the newly created object.
  virtual DataManager* clone() const;
  
  // Get the type name of the data manager (i.e. StandardStMan).
  virtual String dataManagerType() const;
  
  // Get the name given to the storage manager (in the constructor).
  virtual String dataManagerName() const;
  
  // Get the version of the class.
  uInt getVersion() const;
  
  // Set the cache size (in buckets).
  void setCacheSize (uInt aCacheSize);

  // Get the current cache size (in buckets).
  uInt getCacheSize() const;
  
  // Clear the cache used by this storage manager.
  // It will flush the cache as needed and remove all buckets from it.
  void clearCache();

  // Show the statistics of all caches used.
  void showCacheStatistics (ostream& anOs) const;

  // Show Statistics of all indices used
  void showIndexStatistics (ostream & anOs) const;

  // Show Statistics of the Base offsets/index etc
  void showBaseStatistics (ostream & anOs) const;

  // Get the bucket size (in bytes).
  uInt getBucketSize() const;
  
  // Have the data to be stored in canonical format?
  Bool asCanonical() const;
  
  // Get the size of an uInt in external format (can be canonical or local).
  uInt getUIntSize() const;
  
  // Get a unique column number for the column
  // (it is only unique for this storage manager).
  // This is used by SSMColumnIndArr to create a unique file name.
  uInt getUniqueNr();
  
  // Get the number of rows in this storage manager.
  uInt getNRow() const;
  
  // Can the storage manager add rows? (yes)
  virtual Bool canAddRow() const;
  
  // Can the storage manager delete rows? (yes)
  virtual Bool canRemoveRow() const;
  
  // Can the storage manager add columns? (not yet)
  virtual Bool canAddColumn() const;
  
  // Can the storage manager delete columns? (not yet)
  virtual Bool canRemoveColumn() const;
  
  // Make the object from the type name string.
  // This function gets registered in the DataManager "constructor" map.
  // The caller has to delete the object.
  static DataManager* makeObject (const String& aDataManType);
  
  // Get access to the given column.
  SSMColumn& getColumn (uInt aColNr);
  
  // Get access to the given Index.
  SSMIndex& getIndex (uInt anIdxNr);
  
  // Make the current bucket in the cache dirty (i.e. something has been
  // changed in it and it needs to be written when removed from the cache).
  // (used by SSMColumn::putValue).
  void setBucketDirty();
  
  // Open (if needed) the file for indirect arrays with the given mode.
  // Return a pointer to the object.
  StManArrayFile* openArrayFile (ByteIO::OpenOption anOpt);

  // find the dataptr for a given row/column
  char* find(uInt aRowNr,     uInt aColNr, 
	     uInt& aStartRow, uInt& anEndRow);

  // add a new bucket and get its bucketnumber
  uInt getNewBucket();

  // return a BucketNr
  char*  getBucket(const uInt aBucketNr);

  // remove a bucketnr
  void removeBucket(const uInt aBucketNr);

  // get rows per bucket for a given column
  uInt getRowsPerBucket(uInt aColumn) const;

  // return a pointer to the StringHandler
  SSMStringHandler* getStringHandler();

  // <group>
  // callbacks for BucketCache
  static char* readCallBack (void* anOwner, const char* aBucketStorage);
  static void writeCallBack (void* anOwner, char* aBucketStorage,
                             const char* aBucket);
  static void deleteCallBack (void*, char* aBucket);
  static char* initCallBack (void* anOwner);
  // </group>

private:
  // Copy constructor (only meant for clone function).
  SSMBase (const SSMBase& that);
  
  // Assignment cannot be used.
  SSMBase& operator= (const SSMBase& that);
  
  // (Re)create the index, file, and cache object.
  void recreate();
  
  // Flush and optionally fsync the data.
  // It returns a True status if it had to flush (i.e. if data have changed).
  virtual Bool flush (AipsIO&, Bool doFsync);
  
  // Let the storage manager create files as needed for a new table.
  // This allows a column with an indirect array to create its file.
  virtual void create (uInt aNrRows);
  
  // Open the storage manager file for an existing table, read in
  // the data, and let the SSMColumn objects read their data.
  virtual void open (uInt aRowNr, AipsIO&);
  
  // Resync the storage manager with the new file contents.
  // This is done by clearing the cache.
  virtual void resync (uInt aRowNr);
  
  // Reopen the storage manager files for read/write.
  virtual void reopenRW();
  
  // Let the storage manager initialize itself.
  // It is used by create and open.
  void init(Bool doMakeIndex);
  
  // get the number of indices available
  uInt getNrIndices() const;
  
  // Add rows to the storage manager.
  // Per column it extends number of rows
  virtual void addRow (uInt aNrRows);
  
  // Delete a row from all columns.
  virtual void removeRow (uInt aRowNr);
  
  // Do the final addition of a column.
  virtual void addColumn (DataManagerColumn*);
  
  // Remove a column from the data file
  virtual void removeColumn (DataManagerColumn*);
  
  // Create a column in the storage manager on behalf of a table column.
  // The caller has to delete the newly created object.
  // <group>
  // Create a scalar column.
  virtual DataManagerColumn* makeScalarColumn (const String& aName,
					       int aDataType,
					       const String& aDataTypeID);
  // Create a direct array column.
  virtual DataManagerColumn* makeDirArrColumn (const String& aName,
					       int aDataType,
					       const String& aDataTypeID);
  // Create an indirect array column.
  virtual DataManagerColumn* makeIndArrColumn (const String& aName,
					       int aDataType,
					       const String& aDataTypeID);
  // </group>
  
  // Get the cache object.
  // This will construct the cache object if not present yet.
  // The cache object will be deleted by the destructor.
  BucketCache& getCache();
  
  // Construct the cache object (if not constructed yet).
  void makeCache();
  
  // Read the index (at the end of the file).
  void readIndex();
  
  // fill the indexbuckets
  void fillIndexBuckets();

  // Write the index (at the end of the file).
  void writeIndex();
  
  //# Declare member variables.
  // Name of data manager.
  String       itsDataManName;
  // The version of the class.
  uInt         itsVersion;
  
  // The file containing the indirect arrays.
  StManArrayFile* itsIosFile;
  
  // The number of rows in the columns.
  uInt         itsNrRows;
  
  // Column offset
  Block <uInt> itsColumnOffset;

  // Row Index ID containing all the columns in a bucket
  Block <uInt> itsColIndexMap;

  // Will contain all indices
  PtrBlock<SSMIndex*>  itsPtrIndex;
  
  // The cache with the SSM buckets.
  BucketCache* itsCache;
  
  // The file containing all data.
  BucketFile*  itsFile;
  
  // String handler class
  SSMStringHandler* itsStringHandler;

  // The persistent cache size.
  uInt itsPersCacheSize;
  
  // The actual cache size.
  uInt itsCacheSize;
  
  // The initial number of buckets in the cache.
  uInt itsNrBuckets;

  // Nr of buckets needed for index.
  uInt itsNrIdxBuckets;

  // Number of the first index bucket
  Int itsFirstIdxBucket;

  // Number of the first String Bucket
  Int itsLastStringBucket;

  // length of index memoryblock
  uInt itsIndexLength;

  // The nr of free buckets.
  uInt itsFreeBucketsNr;
  
  // The first free bucket.
  Int itsFirstFreeBucket;
  
  // The bucket size.
  uInt itsBucketSize;
  
  // The assembly of all columns.
  PtrBlock<SSMColumn*> itsPtrColumn;
  
  // Has the data changed since the last flush?
  Bool isDataChanged;
  
  // The size of a uInt in external format (local or canonical).
  uInt itsUIntSize;
  
  // Have the data to be stored in canonical or local format.
  Bool isCanonical;
  
};


inline uInt SSMBase::getVersion() const
{
  return itsVersion;
}

inline uInt SSMBase::getNrIndices() const
{
  return itsPtrIndex.nelements();
}

inline uInt SSMBase::getCacheSize() const
{
  return itsCacheSize;
}

inline uInt SSMBase::getNRow() const
{
  return itsNrRows;
}

inline uInt SSMBase::getBucketSize() const
{
  return itsBucketSize;
}

inline uInt SSMBase::getUIntSize() const
{
  return itsUIntSize;
}

inline Bool SSMBase::asCanonical() const
{
  return isCanonical;
}


inline BucketCache& SSMBase::getCache()
{
  if (itsCache == 0) {
    makeCache();
  }
  return *itsCache;
}

inline SSMColumn& SSMBase::getColumn (uInt aColNr)
{
  return *(itsPtrColumn[aColNr]);
}

inline SSMIndex& SSMBase::getIndex (uInt anIdxNr)
{
  return *(itsPtrIndex[anIdxNr]);
}

inline SSMStringHandler* SSMBase::getStringHandler()
{
  return itsStringHandler;
}


#endif
