//# TableLocker.h: Class to hold a (user) lock on a table
//# Copyright (C) 1998
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

#if !defined(AIPS_TABLELOCKER_H)
#define AIPS_TABLELOCKER_H


//# Includes
#include <aips/Tables/Table.h>
#include <aips/Tables/TableLock.h>


// <summary>
// Class to hold a (user) lock on a table.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="tTableLockSync.cc">
// </reviewed>

// <prerequisite>
//# Classes you should understand before using this one.
//   <li> <linkto class=Table>Table</linkto>
//   <li> <linkto class=TableLock>TableLock</linkto>
// </prerequisite>

// <synopsis>
// Class TableLocker can be used to acquire a (user) lock on a table.
// The lock can be a read or write lock.
// The destructor releases the lock.
// <p>
// TableLocker simply uses the <src>lock</src> and <src>unlock</src>
// function of class Table.
// The advantage of TableLocker over these functions is that the
// destructor of TableLocker is called automatically by the system,
// so unlocking the table does not need to be done explicitly and
// cannot be forgotten. Especially in case of exception handling this
// can be quite an adavantage.
// <p>
// This class is meant to be used with the UserLocking option.
// It can, however, also be used with the other locking options.
// In case of PermanentLocking(Wait) it won't do anything at all.
// In case of AutoLocking it will acquire and release the lock when
// needed. However, it is possible that the system releases an
// auto lock before the TableLocker destructor is called.
// </synopsis>

// <example>
// <srcblock>
// // Open a table to be updated.
// Table myTable ("theTable", TableLock::UserLocking, Table::Update);
// // Start of some critical section requiring a lock.
// {
//     TableLocker lock1 (myTable);
//     ... write the data
// }
// // The TableLocker destructor invoked by } unlocked the table.
// </srcblock>
// </example>

// <motivation>
// TableLocker makes it easier to unlock a table.
// </motivation>

//# <todo asof="$DATE:$">
//# A List of bugs, limitations, extensions or planned refinements.
//# </todo>


class TableLocker : public Cleanup
{
public:
    // The constructor acquires a read or write lock on a table.
    // If the table was already locked, the destructor will
    // still unlock the table. This means that care has to be taken
    // that a lock is acquired at one place only.
    // <br>
    // The number of attempts (default = forever) can be specified when
    // acquiring the lock does not succeed immediately. When nattempts>1,
    // the system waits 1 second between each attempt, so nattempts
    // is more or less equal to a wait period in seconds.
    // An exception is thrown when the lock cannot be acquired.
    explicit TableLocker (Table& table,
			  TableLock::LockMode mode = TableLock::Write,
			  uInt nattempts = 0);

    // If locked, the destructor releases the lock and flushes the data.
    ~TableLocker();

    // This function is used by the AIPS++ exception handling mechanism.
    // It calls the destructor.
    void cleanup();

    // Has this process the read or write lock, thus can the table
    // be read or written safely?
    Bool hasLock (TableLock::LockMode mode = TableLock::Write) const;

private:
    // The copy constructor and assignment are not possible.
    // Note that only one lock can be held on a table, so copying a
    // TableLocker object imposes great difficulties which object should
    // release the lock.
    // It can be solved by turning TableLocker into a handle class
    // with a reference counted body class.
    // However, that will only be done when the need arises.
    // <group>
    TableLocker (const TableLocker&);
    TableLocker& operator= (const TableLocker&);
    // </group>

    //# Variables.
    Table itsTable;
};


inline Bool TableLocker::hasLock (TableLock::LockMode mode) const
{
    return itsTable.hasLock (mode);
}


#endif
