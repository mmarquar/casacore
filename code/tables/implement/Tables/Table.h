//# Table.h: Main interface classes to tables
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

#if !defined(AIPS_TABLE_H)
#define AIPS_TABLE_H

#if defined(_AIX)
#pragma implementation ("Table.cc")
#endif 

//# Includes
#include <aips/aips.h>
#include <aips/Tables/BaseTable.h>
#include <aips/Utilities/DataType.h>
#include <aips/Utilities/Sort.h>

//# Forward Declarations
class SetupNewTable;
class TableDesc;
class ColumnDesc;
class TableRecord;
class TableExprNode;
class DataManager;
class IPosition;
template<class T> class Vector;
template<class T> class Block;
template<class T> class PtrBlock;


// <summary>
// Main interface class to a read/write table
// </summary>

// <use visibility=export>

// <reviewed reviewer="TPPR" date="08.11.94" tests="none">
// </reviewed>

// <prerequisite>
//# Classes you should understand before using this one.
//   <li> SetupNewTable
//   <li> TableDesc
//   <li> TableColumn 
//   <li> ScalarColumn
//   <li> ArrayColumn
// </prerequisite>

// <synopsis>
// Class Table can be used to create a new table or to access an existing
// table in read/write or readonly mode.
//
// To access the data in a Table, objects have to be created
// to access the columns. These objects are TableColumn,
// ScalarColumn<T> and ArrayColumn<T>, which can be created
// via their constructors.
// Furthermore the Table has a TableRecord object for holding keywords
// which can be read or written using the appropriate functions.
//
// To open an existing table, a simple Table constructor can be used.
// The possible construct options are:
// <ul>
//   <li> Old            readonly table (default option)
//   <li> Update         update existing table
//   <li> Delete         delete table
// </ul>
// Creating a new table requires more work, because columns have
// to be bound to storage managers or virtual column engines.
// Class SetupNewTable is needed for this purpose. The Tables module
// documentation explains in more detail how to create a table.
//
// Other Table objects can be created from a Table using
// the select, project and sort functions. In that way a subset
// of the table can be created and it can be read/written in the same
// way as a normal Table. However, writing has the effect that the
// underlying table gets written.
// </synopsis>

// <example>
// <srcblock>
// // Open a table to be updated.
// Table myTable ("theTable", Table::Update);
// // Write the column containing the scalar RA.
// ScalarColumn<double> raColumn(myTable, "RA");
// uInt nrrow = myTable.nrow();
// for (uInt i=0; i<nrrow; i++) {
//    raColumn.put (i, i+10);    // Put value i+10 into row i
// }
// </srcblock>
// </example>

// <motivation>
// Table is the envelope for the underlying counted referenced
// classes derived from BaseTable. In this way no pointers have
// to be used to get polymorphism.
// </motivation>

// <todo asof="$DATE:$">
//# A List of bugs, limitations, extensions or planned refinements.
//   <li> add, remove, rename columns.
//   <li> virtual concatenation of tables (if still necessary).
//   <li> maybe an isAttached function.
// </todo>


class Table : public Cleanup
{
friend class ROTableColumn;
friend class BaseTable;
friend class PlainTable;
friend class RefTable;
friend class TableIterator;
friend class RODataManAccessor;
friend class TableExprNode;

public:
    enum TableOption {
	// existing table
	Old=1,
	// create table
	New,
	// create table (may not exist)
	NewNoReplace,
	// new table, which gets marked for delete
	Scratch,
	// update existing table
	Update,
	// delete table
	Delete
    };

    // Create a null Table object (i.e. no table is attached yet).
    // The sole purpose of this constructor is to allow construction
    // of an array of Table objects.
    // The assignment operator can be used to make a null object
    // reference a column.
    // Note that sort functions, etc. will cause a segmentation fault
    // when operating on a null object. It was felt it was too expensive
    // to test on null over and over again. The user should use the isNull
    // or throwIfNull function in case of doubt.
    Table();

    // Create a table object for an existing writable table.
    // The only options allowed are Old, Update, and Delete.
    // <group>
    Table (const String& tableName, TableOption = Table::Old);
    // Check if the table description has the given name.
    Table (const String& tableName, const String& tableDescName,
	   TableOption = Table::Old);
    // </group>

    // Make a table object for a new table, which can thereafter be used
    // for reading and writing.
    // If there are unbound columns, default storage managers an/ord virtual
    // column engines will be created and bound to those columns.
    // Create the table with the given nr of rows. If a storage manager
    // is used which does not allow addition of rows, the number of rows
    // in the table must already be given here.
    // Optionally the rows can be initialized with the default
    // values as defined in the column descriptions.
    Table (SetupNewTable&, uInt nrrow = 0, Bool initialize = False);

    //# Virtually concatenate some tables.
    //# All tables must have the same description.
//#//    Table (const Block<Table>&);

    // Copy constructor (reference semantics).
    Table (const Table&);

    // The destructor flushes (i.e. writes) the table if it is opened
    // for output and not marked for delete.
    // It will flush if the destructor is called due to an exception,
    // because the Table object may not be correct.
    // Of course, in that case the flush function could be called explicitly.
    ~Table();

    //*display 8
    // This function is used by the exception handling mechanism we have
    // defined. It merely calls the destructor without flushing the data.
    // When real exceptions are available it will be unnecessary.
    void cleanup();

    // Assignment (reference semantics).
    Table& operator= (const Table&);

    // Try to reopen the table for read/write access.
    // An exception is thrown if the table is not writable.
    // Nothing is done if the table is already open for read/write.
    void reopenRW();

    // Flush the table, i.e. write it to disk.
    // Nothing will be done if the table is not writable.
    // At any time a flush can be executed, even when the table is marked
    // for delete.
    // When the table is marked for delete, the destructor will remove
    // files written by intermediate flushes.
    // Note that if necessary the destructor will do an implicit flush,
    // unless it is executed due to an exception.
    void flush();

    // Test if the object is null, i.e. does not reference a table yet.
    // This is the case if the default constructor is used.
    Bool isNull() const
	{ return (baseTabPtr_p == 0  ?  True : False); }

    // Throw an exception if the object is null, i.e.
    // if function isNull() is True.
    void throwIfNull() const;

    // Test if the given data type is native to the table system.
    // If not, a virtual column engine is needed to store data with that type.
    // With the function DataType::whatType it can be used in a templated
    // function like:
    // <srcblock>
    //     if (Table::isNativeDataType (whatType((T*)0))) {
    // </srcblock>
    static Bool isNativeDataType (DataType dtype);

    // Make the table file name.
    static String fileName (const String& tableName);

    // Test if a table with the given name exists and is readable.
    static Bool isReadable (const String& tableName);

    // Return the layout of a table (i.e. description and #rows).
    // This function has the advantage that only the minimal amount of
    // information required is read from the table, thus it is much
    // faster than a normal table open.
    // <br> The number of rows is returned. The description of the table
    // is stored in desc (its contents will be overwritten).
    // <br> An exception is thrown if the table does not exist.
    static uInt getLayout (TableDesc& desc, const String& tableName);

    // Get the table info of the table with the given name.
    // An empty object is returned when the table is unknown.
    static TableInfo tableInfo (const String& tableName);

    // Test if a table with the given name exists and is writable.
    static Bool isWritable (const String& tableName);

    // Test if this table is opended as writable.
    Bool isWritable() const;

    // Test if the given column is writable.
    // <group>
    Bool isColumnWritable (const String& columnName) const;
    Bool isColumnWritable (uInt columnIndex) const;
    // </group>

    // Test if the given column is stored (otherwise it is virtual).
    // <group>
    Bool isColumnStored (const String& columnName) const;
    Bool isColumnStored (uInt columnIndex) const;
    // </group>

    // Get access to the table keyword set.
    // <group>
    const TableRecord& keywordSet() const;
    TableRecord& keywordSet();
    // </group>

    // Get access to the TableInfo object.
    // <group>
    const TableInfo& tableInfo() const;
    TableInfo& tableInfo();
    // </group>

    // Write the TableInfo object.
    // Usually this is not necessary, because it is done automatically
    // when the table gets written (by table destructor or flush function).
    // This function is only useful when the table info has to be written
    // before the table gets written (e.g. when another process reads
    // the table while it gets filled).
    void flushTableInfo() const;

    // Get access to the table description.
    // This can be used to get nr of columns, etc..
    // <note role=tip>
    // The keyword set in the table description is not the
    // same set as the keyword set in the table itself.
    // </note>
    const TableDesc& tableDesc() const;

    // Get the table name.
    const String& tableName() const;

    // Rename the table and all its subtables.
    // The following options can be given:
    // <dl>
    // <dt> Table::Update
    // <dd> A table with this name must already exists, which will be
    //      overwritten. When succesfully renamed, the table is unmarked
    //      for delete (if necessary).
    // <dt> Table::New
    // <dd> When a table with this name exists, it will be overwritten.
    //      When succesfully renamed, the table is unmarked
    //      for delete (if necessary).
    // <dt> Table::NewNoReplace
    // <dd> When a table with this name already exists, an exception
    //      is thrown. When succesfully renamed, the table
    //      is unmarked for delete (if necessary).
    // <dt> Table::Scratch
    // <dd> Same as Table::New, but followed by markForDelete().
    // </dl>
    void rename (const String& newName, TableOption);

    // Copy the table and all its subtables.
    // The following options can be given:
    // <dl>
    // <dt> Table::New
    // <dd> When a table with this name exists, it will be overwritten.
    // <dt> Table::NewNoReplace
    // <dd> When a table with this name already exists, an exception
    //      is thrown.
    // <dt> Table::Scratch
    // <dd> Same as Table::New, but followed by markForDelete().
    // </dl>
    void copy (const String& newName, TableOption) const;

    // Get the table option.
    int tableOption() const;

    // Mark the table for delete.
    // This means that the underlying table gets deleted when it is
    // actually destructed.
    void markForDelete();

    // Unmark the table for delete.
    // This means the underlying table does not get deleted when destructed.
    void unmarkForDelete();

    // Test if the table is marked for delete.
    Bool isMarkedForDelete() const;
    
    // Test if the table needs synchronization.
    // Synchronization means that the size of the table is synchronized.
    // This may be needed when the table is filled by an external proces,
    // e.g. the data acquisition system of a telescope.
    Bool needToSync() const;

    // Synchronize the table and return the number of rows.
    // Also a flag is set telling if more rows can be expected.
    // Synchronization means that the size of the table is synchronized.
    // This may be needed when the table is filled by an external proces,
    // e.g. when a data acquisition system of a telescope extends a table,
    // another process reading that table needs to call sync now and then
    // to update its number of rows.
    uInt sync (Bool& moreToExpect) const;
    
    // Get the number of rows. This is unsynchronized.
    uInt nrow() const;

    // Test if it is possible to add a row to this table.
    // It is possible if all storage managers used for the table
    // support it.
    Bool canAddRow() const;

    // Add one or more rows at the end of the table.
    // This will fail for tables not supporting addition of rows.
    // Optionally the rows can be initialized with the default
    // values as defined in the column descriptions.
    void addRow (uInt nrrow = 1, Bool initialize = False);

    // Test if it is possible to remove a row from this table.
    // It is possible if all storage managers used for the table
    // support it.
    Bool canRemoveRow() const;

    // Remove the given row(s).
    // The latter formm can be useful with the select and rowNumbers functions
    // to remove some selected rows from the table.
    // <br>It will fail for tables not supporting removal of rows.
    // <note role=warning>
    // The following code fragments do NOT have the same result:
    // <srcblock>
    //    tab.removeRow (10);      // remove row 10
    //    tab.removeRow (20);      // remove row 20, which was 21
    //    Vector<uInt> vec(2);
    //    vec(0) = 10;
    //    vec(1) = 20;
    //    tab.removeRow (vec);     // remove row 10 and 20
    // </srcblock>
    // because in the first fragment removing row 10 turns the former
    // row 21 into row 20.
    // </note>
    // <group>
    void removeRow (uInt rownr);
    void removeRow (const Vector<uInt>& rownrs);
    // </group>

    // Create a TableExprNode object for a column or for a keyword
    // in the table keyword set.
    // This can be used in selecting rows from a table using
    // <src>operator()</src> described below.
    // <group name=keycol>
    TableExprNode key (const String& keywordName, Bool isArray=False) const;
    TableExprNode col (const String& columnName, Bool isArray=False) const;
    TableExprNode keyCol (const String& name, Bool isArray=False) const;
    // </group>

    // Create a TableExprNode object for the rownumber function.
    // 'origin' Indicates which rownumber is the first.
    // C++ uses origin = 0 (default)
    // Glish and TaQL both use origin = 1
    TableExprNode nodeRownr (uInt origin=0) const;

    // Create a TableExprNode object for the rand function.
    TableExprNode nodeRandom () const;

    // Select rows from a table using an select expression consisting
    // of TableExprNode objects.
    // Basic TableExprNode objects can be created with the functions
    // <linkto file="Table.h#keycol">key</linkto> and especially
    // <linkto file="Table.h#keycol">col</linkto>.
    // Composite TableExprNode objects, representing an expression,
    // can be created by applying operations (like == and +)
    // to the basic ones. This is described in class
    // <linkto class="TableExprNode:description">TableExprNode</linkto>.
    // For example:
    // <srcblock>
    //    Table result = tab(tab.col("columnName") > 10);
    // </srcblock>
    // All rows for which the expression is true, will be selected and
    // "stored" in the result.
    // You need to include ExprNode.h for this purpose.
    Table operator() (const TableExprNode&) const;

    // Select rows using a vector of row numbers.
    // This can, for instance, be used to select the same rows as
    // were selected in another table (using the rowNumbers function).
    // <srcblock>
    //     Table result = thisTable (otherTable.rowNumbers());
    // </srcblock>
    Table operator() (const Vector<uInt>& rownrs) const;

    // Select rows using a mask block.
    // The length of the block must match the number of rows in the table.
    // If an element in the mask is True, the corresponding row will be
    // selected.
    Table operator() (const Block<Bool>& mask) const;

    // Project the given columns (i.e. select the columns).
    Table project (const Block<String>& columnNames) const;

    //# Virtually concatenate all tables in this column.
    //# The column cells must contain tables with the same description.
//#//    Table concatenate (const String& columnName) const;

    // Do logical operations on a table.
    // It can be used for row-selected or projected (i.e. column-selected)
    // tables. The tables involved must come from the same root table or
    // be the root table themselves.
    // <group>
    // Intersection with another table.
    Table operator& (const Table&) const;
    // Union with another table.
    Table operator| (const Table&) const;
    // Subtract another table.
    Table operator- (const Table&) const;
    // Xor with another table.
    Table operator^ (const Table&) const;
    // Take complement.
    Table operator! () const;
    // </group>

    // Sort a table on one or more columns of scalars.
    // Per column a compare function can be provided. By default
    // the standard compare function defined in Compare.h will be used.
    // Default sort order is ascending.
    // Default sorting algorithm is the heap sort.
    // <group>
    // Sort on one column.
    Table sort (const String& columnName,
		int = Sort::Ascending,
		int = Sort::HeapSort) const;
    // Sort on multiple columns. The principal column has to be the
    // first element in the Block of column names.
    Table sort (const Block<String>& columnNames,
		int = Sort::Ascending,
		int = Sort::HeapSort) const;
    // Sort on multiple columns. The principal column has to be the
    // first element in the Block of column names.
    // The order can be given per column.
    Table sort (const Block<String>& columnNames,
		const Block<Int>& sortOrders,
		int = Sort::HeapSort) const;
    // Sort on multiple columns. The principal column has to be the
    // first element in the Block of column names.
    // The order can be given per column.
    // Provide some special compare functions via a function pointer.
    // A zero function pointer means using the standard compare function
    // from class <linkto class="ObjCompare:description">ObjCompare</linkto>.
    Table sort (const Block<String>& columnNames,
		const PtrBlock<ObjCompareFunc*>& compareFunctionPointers,
		const Block<Int>& sortOrders,
		int = Sort::HeapSort) const;
    // </group>

    // Get a vector of row numbers.
    // In case the table is a subset of the root table, this tells which
    // rows of the root table are part of the subset.
    // In case the table is the root table itself, the result is a vector
    // containing the row numbers 0 .. #rows-1.
    Vector<uInt> rowNumbers() const;

    // Add a column to the table.
    // The data manager used for the column depend on the function used.
    // Exceptions are thrown when the column already exist or when the
    // table is not writable.
    // <group>
    // Use the first appropriate existing storage manager.
    // When there is none, a data manager is created using the default
    // data manager in the column description.
    void addColumn (const ColumnDesc& columnDesc);
    // Use an existing data manager with the given name or type.
    // When the flag byName is True, a name is given, otherwise a type.
    // When a name is given, an exception is thrown if the data manager is
    // unknown or does not allow addition of columns.
    // When a type is given, a storage manager of the given type will be
    // created when there is no such data manager allowing addition of rows.
    void addColumn (const ColumnDesc& columnDesc,
		    const String& dataManager, Bool byName);
    // Use the given data manager (which is a new one).
    void addColumn (const ColumnDesc& columnDesc,
		    const DataManager& dataManager);
    // </group>

    // Add a bunch of columns using the given new data manager.
    // All columns and possible hypercolumn definitions in the given table
    // description will be copied and added to the table.
    // This can be used in case of specific data managers which need to
    // be created with more than one column (e.g. the tiled hypercube
    // storage managers).
    void addColumn (const TableDesc& tableDesc,
		    const DataManager& dataManager);

    // Test if a column can be removed.
    // It can if the column exists and if the data manager it is using
    // supports removal of columns.
    Bool canRemoveColumn (const String& columnName) const;

    // Remove a column.
    void removeColumn (const String& columnName);

    // Test if a column can be renamed.
    Bool canRenameColumn() const;

    // Rename a column.
    void renameColumn (const String& newName, const String& oldName);

    //*display 4
    // Write a table to AipsIO (for TypedKeywords<Table>).
    // This will only write the table name.
    friend AipsIO& operator<< (AipsIO&, const Table&);

    //*display 4
    // Read a table from AipsIO (for TypedKeywords<Table>).
    // This will read the table name and open the table as writable
    // if the table file is writable, otherwise as readonly.
    friend AipsIO& operator>> (AipsIO&, Table&);

    //*display 4
    // Read a table from AipsIO (for TableKeywords).
    // This will read the table name and open the table as writable
    // if the switch is set and if the table file is writable.
    // otherwise it is opened as readonly.
    void getTableKeyword (AipsIO&, Bool openWritable);

    // Write a table to ostream (for TypedKeywords<Table>).
    // This only shows its name and number of columns and rows.
    friend ostream& operator<< (ostream&, const Table&);

protected:
    BaseTable*  baseTabPtr_p;                 //# ptr to table representation
    //# The isCounted_p flag is normally true.
    //# Only for internally used Table objects (i.e. in the DataManager)
    //# this flag is False, otherwise a mutual dependency would exist.
    //# The DataManager has a Table object, which gets deleted by the
    //# DataManager destructor. The DataManager gets deleted by the
    //# PlainTable destructor, which gets called when the last Table
    //# object gets destructed. That would never be the case if this
    //# internally used Table object was counted.
    Bool        isCounted_p;

    // Construct a Table object from a BaseTable*.
    // By default the object gets counted.
    Table (BaseTable*, Bool countIt = True);

    // Open an existing table.
    void open (const String& nam, const String& type, int tableOption);

private:
    // Get the pointer to the underlying BaseTable.
    // This is needed for some friend classes.
    BaseTable* baseTablePtr() const;

    // Look in the cache if the table is already open.
    // If so, check if table option matches.
    BaseTable* lookCache (const String& name, int tableOption);

    // Find the data manager with the given name.
    DataManager* findDataManager (const String& datamanagerName) const;
};



inline void Table::reopenRW()
    { baseTabPtr_p->reopenRW(); }
inline void Table::flush()
    { baseTabPtr_p->flush(); }

inline Bool Table::isWritable() const
    { return baseTabPtr_p->isWritable(); }
inline Bool Table::isColumnWritable (const String& columnName) const
    { return baseTabPtr_p->isColumnWritable (columnName); }
inline Bool Table::isColumnWritable (uInt columnIndex) const
    { return baseTabPtr_p->isColumnWritable (columnIndex); }

inline Bool Table::isColumnStored (const String& columnName) const
    { return baseTabPtr_p->isColumnStored (columnName); }
inline Bool Table::isColumnStored (uInt columnIndex) const
    { return baseTabPtr_p->isColumnStored (columnIndex); }

inline void Table::rename (const String& newName, TableOption option)
    { baseTabPtr_p->rename (newName, option); }
inline void Table::copy (const String& newName, TableOption option) const
    { baseTabPtr_p->copy (newName, option); }
inline void Table::markForDelete()
    { baseTabPtr_p->markForDelete(); }
inline void Table::unmarkForDelete()
    { baseTabPtr_p->unmarkForDelete(); }
inline Bool Table::isMarkedForDelete() const
    { return baseTabPtr_p->isMarkedForDelete(); }

inline Bool Table::needToSync() const
    { return baseTabPtr_p->needToSync(); }
inline uInt Table::sync (Bool& moreToExpect) const
    { return baseTabPtr_p->sync (moreToExpect); }
inline uInt Table::nrow() const
    { return baseTabPtr_p->nrow(); }
inline BaseTable* Table::baseTablePtr() const
    { return baseTabPtr_p; }
inline const TableDesc& Table::tableDesc() const
    { return baseTabPtr_p->tableDesc(); }
inline const TableRecord& Table::keywordSet() const
    { return baseTabPtr_p->keywordSet(); }
inline TableRecord& Table::keywordSet()
    { return baseTabPtr_p->keywordSet(); }

inline TableInfo Table::tableInfo (const String& tableName)
    { return BaseTable::tableInfo (tableName); }
inline const TableInfo& Table::tableInfo() const
    { return baseTabPtr_p->tableInfo(); }
inline TableInfo& Table::tableInfo()
    { return baseTabPtr_p->tableInfo(); }
inline void Table::flushTableInfo() const
    { baseTabPtr_p->flushTableInfo(); }

inline const String& Table::tableName() const
    { return baseTabPtr_p->tableName(); }
inline int Table::tableOption() const
    { return baseTabPtr_p->tableOption(); }

inline Bool Table::canAddRow() const
    { return baseTabPtr_p->canAddRow(); }
inline Bool Table::canRemoveRow() const
    { return baseTabPtr_p->canRemoveRow(); }
inline Bool Table::canRemoveColumn (const String& columnName) const
    { return baseTabPtr_p->canRemoveColumn (columnName); }
inline Bool Table::canRenameColumn() const
    { return baseTabPtr_p->canRenameColumn(); }

inline void Table::addRow (uInt nrrow, Bool initialize)
    { baseTabPtr_p->addRow (nrrow, initialize); }
inline void Table::removeRow (uInt rownr)
    { baseTabPtr_p->removeRow (rownr); }
inline void Table::removeRow (const Vector<uInt>& rownrs)
    { baseTabPtr_p->removeRow (rownrs); }
inline void Table::addColumn (const ColumnDesc& columnDesc)
    { baseTabPtr_p->addColumn (columnDesc); }
inline void Table::addColumn (const ColumnDesc& columnDesc,
			      const String& dataManager, Bool byName)
    { baseTabPtr_p->addColumn (columnDesc, dataManager, byName); }
inline void Table::addColumn (const ColumnDesc& columnDesc,
			      const DataManager& dataManager)
    { baseTabPtr_p->addColumn (columnDesc, dataManager); }
inline void Table::addColumn (const TableDesc& tableDesc,
			      const DataManager& dataManager)
    { baseTabPtr_p->addColumn (tableDesc, dataManager); }
inline void Table::removeColumn (const String& columnName)
    { baseTabPtr_p->removeColumn (columnName); }
inline void Table::renameColumn (const String& newName, const String& oldName)
    { baseTabPtr_p->renameColumn (newName, oldName); }

inline DataManager* Table::findDataManager (const String& name) const
{
    return baseTabPtr_p->findDataManager (name);
}


#endif
