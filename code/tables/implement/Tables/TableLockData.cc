//# TableLockData.cc: Class to hold table lock data
//# Copyright (C) 1997
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


#include <aips/Tables/TableLockData.h>
#include <aips/Tables/TableError.h>
#include <aips/Logging/LogIO.h>
#include <unistd.h>


TableLockData::TableLockData (const TableLock& lockOptions,
			      TableLockData::ReleaseCallBack* releaseCallBack,
			      void* releaseParentObject)
: TableLock          (lockOptions),
  itsLock            (0),
  itsReadLocked      (False),
  itsWriteLocked     (False),
  itsReleaseCallBack (releaseCallBack),
  itsReleaseParent   (releaseParentObject)
{}

TableLockData::~TableLockData()
{
    delete itsLock;
}


void TableLockData::makeLock (const String& name, Bool create, Bool write)
{
    //# Create lock file object only when not created yet.
    //# It is acceptable that no lock file exists for a readonly table
    //# (to be able to read older tables).
    if (itsLock == 0) {
	itsLock = new LockFile (name + "/table.lock", interval(), create,
				True, False);
    }
    //# Acquire a lock when permanent locking is in use.
    if (isPermanent()) {
	uInt nattempts = 1;
	if (option() == PermanentLockingWait) {
	    nattempts = 0;                          // wait
	}
	if (! itsLock->acquire (write, nattempts)) {
	    throw (TableError ("Permanent lock on table " + name +
			       " could not be acquired (" +
			       itsLock->lastMessage() + ")"));
	}
	itsReadLocked = True;
	if (write) {
	    itsWriteLocked = True;
	}
    }
}

Bool TableLockData::acquire (MemoryIO* info, Bool write, uInt nattempts)
{
    //# Try to acquire a lock.
    //# Show a message when we have to wait for a long time.
    //# Start with n attempts, show a message and continue thereafter.
    uInt n = 30;
    if (nattempts > 0  &&  nattempts < n) {
	n = nattempts;
    }
    Bool status = itsLock->doAcquire (info, write, n);
    if (!status  &&  n != nattempts) {
	String s = "read";
	if (write) {
	    s = "write";
	}
	LogIO os;
	os << "Process " << uInt(getpid()) << ": waiting for "
	   << s << "-lock on file " << itsLock->name();
	os.post();
	if (nattempts > 0) {
	    nattempts -= n;
	}
	status = itsLock->doAcquire (info, write, nattempts);
	if (status) {
	    os << "Process " << uInt(getpid()) << ": acquired "
	       << s << "-lock on file " << itsLock->name();
	    os.post();
	}else{
	    if (nattempts > 0) {
		os << "Process " << uInt(getpid()) << ": gave up acquiring "
		   << s << "-lock on file " << itsLock->name()
		   << " after " << nattempts << " seconds";
		os.post();
	    }
	}
    }
    //# Throw exception when error while we had to wait forever.
    if (!status) {
	if (nattempts == 0) {
	    throw (TableError ("Error (" + itsLock->lastMessage() +
			       ") when acquiring lock on " + itsLock->name()));
	}
    }else{
	itsReadLocked = True;
	if (write) {
	    itsWriteLocked = True;
	}
    }
    return status;
}

void TableLockData::release (Bool always)
{
    //# Only release if not permanently locked.
    if (always  ||  !isPermanent()) {
	MemoryIO* memIO = 0;
	if (itsWriteLocked) {
	    if (itsReleaseCallBack != 0) {
		memIO = itsReleaseCallBack (itsReleaseParent, always);
	    }
	}
	if (! itsLock->doRelease (memIO)) {
	    throw (TableError ("Error (" + itsLock->lastMessage() +
			       ") when releasing lock on " + itsLock->name()));
	}
	itsReadLocked  = False;
	itsWriteLocked = False;
    }
}
