//# StManAipsIO.h: Storage manager for tables using AipsIO
//# Copyright (C) 1994,1995,1996
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

#if !defined(AIPS_STMANAIPSIO_H)
#define AIPS_STMANAIPSIO_H

#if defined(_AIX)
#pragma implementation ("StManAipsIO.cc")
#endif 

//# Includes
#include <aips/aips.h>
#include <aips/Tables/DataManager.h>
#include <aips/Tables/StManColumn.h>
#include <aips/Containers/Block.h>
#include <aips/Mathematics/Complex.h>
#include <aips/Lattices/IPosition.h>
#include <aips/Utilities/String.h>

//# Forward clarations
class AipsIO;
class StManAipsIO;


// <summary>
// AipsIO table column storage manager class
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="">
// </reviewed>

// <prerequisite>
//# Classes you should understand before using this one.
//   <li> StManColumn
// </prerequisite>

// <etymology>
// StManColumnAipsIO handles a column for an AipsIO storage manager.
// </etymology>

// <synopsis> 
// StManColumnAipsIO is used by StManAipsIO to handle the access to
// the data in a table column.
// It is an storage manager based on AipsIO. The entire column is
// kept in memory and only written when the storage manager closes.
// When the storage manager gets opened, the entire column gets
// read back.
// It fully supports addition and removal of rows.
//
// StManColumnAipsIO serves 2 purposes:
// <ol>
// <li> It handles a column containing scalar values.
// <li> It serves as a base class for StManArrayColumnAipsIO and
//        StManIndArrayColumnAipsIO. These classes handle arrays and
//        use StManColumnAipsIO to hold a pointer to the array in each row.
// </ol>
//
// StManColumnAipsIO does not hold a column as a consecutive array,
// because extending the column (i.e. adding rows) proofed be too
// expensive due to the repeated copying involved when creating a table
// (this method was used by the old table system).
// Instead it has a number of data blocks (extensions) indexed to by a
// super block. Accessing a row means finding the appropriate extension
// via a binary search. Because there is only 1 extension when a table is
// read back, the overhead in finding a row is small.
// </synopsis> 

// <motivation>
// StManColumnAipsIO handles the standard data types. The class
// is not templated, but a switch statement is used instead.
// Templates would cause too many instantiations.
// </motivation>

// <todo asof="$DATE:$">
//# A List of bugs, limitations, extensions or planned refinements.
// </todo>


class StManColumnAipsIO : public StManColumn
{
public:

    // Create a column of the given type.
    // It will maintain a pointer to its parent storage manager.
    StManColumnAipsIO (StManAipsIO* stMan, int dataType, Bool byPtr);

    // Frees up the storage.
    virtual ~StManColumnAipsIO();

    // Get a scalar value in the given row.
    // The buffer pointed to by dataPtr has to have the correct length
    // (which is guaranteed by the Scalar/ArrayColumn get function).
    // <group>
    void getBoolV     (uInt rownr, Bool* dataPtr);
    void getuCharV    (uInt rownr, uChar* dataPtr);
    void getShortV    (uInt rownr, Short* dataPtr);
    void getuShortV   (uInt rownr, uShort* dataPtr);
    void getIntV      (uInt rownr, Int* dataPtr);
    void getuIntV     (uInt rownr, uInt* dataPtr);
    void getfloatV    (uInt rownr, float* dataPtr);
    void getdoubleV   (uInt rownr, double* dataPtr);
    void getComplexV  (uInt rownr, Complex* dataPtr);
    void getDComplexV (uInt rownr, DComplex* dataPtr);
    void getStringV   (uInt rownr, String* dataPtr);
    // </group>

    // Put a scalar value into the given row.
    // The buffer pointed to by dataPtr has to have the correct length
    // (which is guaranteed by the Scalar/ArrayColumn put function).
    // <group>
    void putBoolV     (uInt rownr, const Bool* dataPtr);
    void putuCharV    (uInt rownr, const uChar* dataPtr);
    void putShortV    (uInt rownr, const Short* dataPtr);
    void putuShortV   (uInt rownr, const uShort* dataPtr);
    void putIntV      (uInt rownr, const Int* dataPtr);
    void putuIntV     (uInt rownr, const uInt* dataPtr);
    void putfloatV    (uInt rownr, const float* dataPtr);
    void putdoubleV   (uInt rownr, const double* dataPtr);
    void putComplexV  (uInt rownr, const Complex* dataPtr);
    void putDComplexV (uInt rownr, const DComplex* dataPtr);
    void putStringV   (uInt rownr, const String* dataPtr);
    // </group>

    // Get scalars from the given row on with a maximum of nrmax values.
    // This can be used to get an entire column of scalars or to get
    // a part of a column (for a cache for example).
    // The buffer pointed to by dataPtr has to have the correct length
    // (which is guaranteed by the ScalarColumn get function).
    // <group>
    uInt getBlockBoolV     (uInt rownr, uInt nrmax, Bool* dataPtr);
    uInt getBlockuCharV    (uInt rownr, uInt nrmax, uChar* dataPtr);
    uInt getBlockShortV    (uInt rownr, uInt nrmax, Short* dataPtr);
    uInt getBlockuShortV   (uInt rownr, uInt nrmax, uShort* dataPtr);
    uInt getBlockIntV      (uInt rownr, uInt nrmax, Int* dataPtr);
    uInt getBlockuIntV     (uInt rownr, uInt nrmax, uInt* dataPtr);
    uInt getBlockfloatV    (uInt rownr, uInt nrmax, float* dataPtr);
    uInt getBlockdoubleV   (uInt rownr, uInt nrmax, double* dataPtr);
    uInt getBlockComplexV  (uInt rownr, uInt nrmax, Complex* dataPtr);
    uInt getBlockDComplexV (uInt rownr, uInt nrmax, DComplex* dataPtr);
    uInt getBlockStringV   (uInt rownr, uInt nrmax, String* dataPtr);
    // </group>

    // Put nrmax scalars from the given row on.
    // This can be used to put an entire column of scalars or to put
    // a part of a column (for a cache for example).
    // The buffer pointed to by dataPtr has to have the correct length
    // (which is guaranteed by the ScalarColumn put function).
    // <group>
    void putBlockBoolV     (uInt rownr, uInt nrmax, const Bool* dataPtr);
    void putBlockuCharV    (uInt rownr, uInt nrmax, const uChar* dataPtr);
    void putBlockShortV    (uInt rownr, uInt nrmax, const Short* dataPtr);
    void putBlockuShortV   (uInt rownr, uInt nrmax, const uShort* dataPtr);
    void putBlockIntV      (uInt rownr, uInt nrmax, const Int* dataPtr);
    void putBlockuIntV     (uInt rownr, uInt nrmax, const uInt* dataPtr);
    void putBlockfloatV    (uInt rownr, uInt nrmax, const float* dataPtr);
    void putBlockdoubleV   (uInt rownr, uInt nrmax, const double* dataPtr);
    void putBlockComplexV  (uInt rownr, uInt nrmax, const Complex* dataPtr);
    void putBlockDComplexV (uInt rownr, uInt nrmax, const DComplex* dataPtr);
    void putBlockStringV   (uInt rownr, uInt nrmax, const String* dataPtr);
    // </group>

    // Add (newNrrow-oldNrrow) rows to the column.
    virtual void addRow (uInt newNrrow, uInt oldNrrow);

    // Resize the data blocks.
    // This adds an extension when needed.
    void resize (uInt nrval);

    // Remove the given row.
    // If no rows remain in the extension, the extension is also removed.
    virtual void remove (uInt rownr);

    // Create the number of rows in a new table.
    // This is used when a table gets created.
    virtual void doCreate (uInt nrrow);

    // Write the column data into AipsIO.
    // It will successively write all extensions using putData.
    virtual void putFile (uInt nrval, AipsIO&);

    // Read the column data from AipsIO.
    // One extension gets allocated to hold all rows in the column.
    virtual void getFile (uInt nrval, AipsIO&);

    // Reopen the storage manager files for read/write.
    virtual void reopenRW();

    // Check if the class invariants still hold.
    virtual Bool ok() const;

protected:
    // The storage manager.
    StManAipsIO* stmanPtr_p;
    // The data type (for caching purposes).
    int dtype_p;
    // The data is indirectly accessed via a pointer (for the derived classes).
    Bool  byPtr_p;
    // The number of allocated rows in the column.
    uInt  nralloc_p;
    // The nr of extensions in use.
    uInt  nrext_p;
    // The assembly of all extensions (actually Block<T*>).
    Block<void*> data_p;
    // The cumulative nr of rows in all extensions.
    Block<uInt>  ncum_p;

    // Find the extension in which the row number is.
    uInt findExt (uInt rownr) const;

    // Get the next extension.
    // For the first iteration extnr should be zero.
    // It returns the number of values in it until the maximum is reached.
    // Zero means no more extensions.
    uInt nextExt (void*& ext, uInt& extnr, uInt nrmax) const;

    // Allocate an extension with the data type of the column.
    void* allocData (uInt nrval, Bool byPtr);

    // Delete all extensions.
    // Possible underlying data (as used by StManArrayColumnAipsIO)
    // will not be deleted and should have been deleted beforehand.
    void deleteAll();

    // Delete an extension.
    void deleteData (void* datap, Bool byPtr);

    // Remove an entry (i.e. a row) from an extension at the given index.
    // It will do this by shifting the rest (nrvalAfter elements)
    // one position to the left.
    void removeData (void* datap, uInt inx, uInt nrvalAfter);

    // Put the data (nrval elements) in an extension (starting at datap)
    // into AipsIO.
    virtual void putData (void* datap, uInt nrval, AipsIO&);

    // Get data (nrval elements) into an extension (starting at datap
    // plus the given index).
    virtual void getData (void* datap, uInt index, uInt nrval, AipsIO&,
			  uInt version);

    // Get the pointer for the given row.
    // This is for the derived classes like StManArrayColumnAipsIO.
    void* getArrayPtr (uInt rownr);

    // Put the pointer for the given row.
    // This is for the derived classes like StManArrayColumnAipsIO.
    void putArrayPtr (uInt rownr, void* dataPtr);

private:
    // Forbid copy constructor.
    StManColumnAipsIO (const StManColumnAipsIO&);

    // Forbid assignment.
    StManColumnAipsIO& operator= (const StManColumnAipsIO&);
};




// <summary>
// AipsIO table storage manager class
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="">
// </reviewed>

// <prerequisite>
//# Classes you should understand before using this one.
//   <li> DataManager
//   <li> StManColumnAipsIO
// </prerequisite>

// <etymology>
// StManAipsIO is the storage manager using AipsIO.
// </etymology>

// <synopsis> 
// StManAipsIO is a table storage manager based on AipsIO.
// It holds the data in the columns in memory and writes them to
// a file when the table gets closed. Only the data of indirect arrays
// are directly read/written from/to a file.
// It contains pointers to the underlying StManColumnAipsIO objects,
// which do the actual data handling.
//
// The AipsIO storage manager does fully support addition and removal
// of rows and columns.
//
// All data, except indirect columns, for this storage manager are kept
// in one file. The file name is the table name appended with
// .N_AipsIO, where N is the (unique) storage manager sequence number.
// Each column containing indirect arrays is stored in a separate file
// using class StManIndArrayColumnAipsIO. The name of such a file is
// the storage manager file name appended with _cM, where M is a unique
// column sequence number acquired using function uniqueNr().
// </synopsis> 

// <todo asof="$DATE:$">
//# A List of bugs, limitations, extensions or planned refinements.
// </todo>


class StManAipsIO : public DataManager
{
public:

    // Create an AipsIO storage manager.
    // Its name will be blank.
    StManAipsIO();

    // Create an AipsIO storage manager with the given name.
    // Its name can be used later in e.g. Table::addColumn to
    // add a column to this storage manager.
    StManAipsIO (const String& storageManagerName);

    ~StManAipsIO();

    // Clone this object.
    // It does not clone StManAipsIOColumn objects possibly used.
    DataManager* clone() const;

    // Get the type name of the data manager (i.e. StManAipsIO).
    String dataManagerType() const;

    // Get the name given to this storage manager.
    String dataManagerName() const;

    // Get a unique column number for the column
    // (it is only unique for this storage manager).
    // This is used by StManIndArrayColumnAipsIO to create a unique file name.
    uInt uniqueNr()
	{ return uniqnr_p++; }

    // Get the nr of rows in this storage manager.
    uInt nrow() const
	{ return nrrow_p; }

    // Does the storage manager allow to add rows? (yes)
    Bool canAddRow() const;

    // Does the storage manager allow to delete rows? (yes)
    Bool canRemoveRow() const;

    // Does the storage manager allow to add columns? (yes)
    Bool canAddColumn() const;

    // Does the storage manager allow to delete columns? (yes)
    Bool canRemoveColumn() const;

    // Make the object from the string.
    // This function gets registered in the DataManager "constructor" map.
    static DataManager* makeObject (const String& dataManagerType);


private:
    // Flush the data and close the storage manager.
    // All StManColumnAipsIO objects will write their data and close
    // themselves.
    void close (AipsIO&);

    // Let the storage manager create files as needed for a new table.
    // This allows a column with an indirect array to create its file.
    void create (uInt nrrow);

    // Open the storage manager file for an existing table and read in
    // the data and let the StManColumnAipsIO objects read their data.
    void open (uInt nrrow, AipsIO&);

    // Reopen the storage manager files for read/write.
    virtual void reopenRW();

    // Add rows to all columns.
    void addRow (uInt nrrow);

    // Delete a row from all columns.
    void removeRow (uInt rownr);

    // Create a column in the storage manager on behalf of a table column.
    // <group>
    // Create a scalar column.
    DataManagerColumn* makeScalarColumn (const String& name, int dataType,
					 const String& dataTypeID);
    // Create a direct array column.
    DataManagerColumn* makeDirArrColumn (const String& name, int dataType,
					 const String& dataTypeID);
    // Create an indirect array column.
    DataManagerColumn* makeIndArrColumn (const String& name, int dataType,
					 const String& dataTypeID);
    // </group>

    // Add a column.
    void addColumn (DataManagerColumn*);

    // Delete a column.
    void removeColumn (DataManagerColumn*);


    // Name given by user to this storage manager.
    String stmanName_p;
    // Unique nr for column in this storage manager.
    uInt   uniqnr_p;
    // The number of rows in the columns.
    uInt   nrrow_p;
    // The assembly of all columns.
    PtrBlock<StManColumnAipsIO*>  colSet_p;
};



#endif
