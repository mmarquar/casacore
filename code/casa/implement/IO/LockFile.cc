//# LockFile.cc: Class to handle file locking
//# Copyright (C) 1997,1998,1999
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


#include <aips/IO/LockFile.h>
#include <aips/IO/FiledesIO.h>
#include <aips/IO/MemoryIO.h>
#include <aips/IO/CanonicalIO.h>
#include <aips/OS/RegularFile.h>
#include <aips/OS/Path.h>
#include <aips/OS/CanonicalConversion.h>
#include <aips/Utilities/Assert.h>
#include <aips/Exceptions/Error.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream.h>
#include <strstream.h>

// PABLO_IO is used to profile the IO performance of the AIPS++ (in
// particular to help us locate bottlenecks associated with parallel
// processing)
//
// Please see http://www-pablo.cs.uiuc.edu if you need more details
// You'll need to set PABLO_IO variable in the makedefs as well as the
// -I flag to find the header file and -L to resolve with the library.
//
#ifdef PABLO_IO
#include "IOTrace.h"
#else
#define traceFCLOSE fclose
#define traceFSEEK fseek
#define traceFREAD fread
#define traceFWRITE fwrite
#define traceWRITE write
#define trace2OPEN open
#define trace3OPEN open
#define traceLSEEK lseek
#define traceCLOSE close
#endif // PABLO_IO

//# canonical size of an Int (checked in constructor).
#define SIZEINT 4u
#define NRREQID 32u
#define SIZEREQID ((1 + 2*NRREQID) * SIZEINT)


LockFile::LockFile (const String& fileName, double inspectInterval,
		    Bool create, Bool setRequestFlag, Bool mustExist,
		    uInt seqnr)
: itsFileIO    (0),
  itsCanIO     (0),
  itsWritable  (True),
  itsAddToList (setRequestFlag),
  itsInterval  (inspectInterval),
  itsPid       (getpid()),
///  itsHostId    (gethostid()),     gethostid is not declared in unistd.h
  itsHostId    (0),
  itsReqId     (SIZEREQID/SIZEINT, (Int)0)
{
    AlwaysAssert (SIZEINT == CanonicalConversion::canonicalSize (static_cast<Int*>(0)),
		  AipsError);
    itsName = Path(fileName).expandedName();
    //# Create the file if it does not exist yet.
    //# When the flag is set, it is allowed that the file does not
    //# exist and cannot be created. In that case it is assumed that
    //# later on each locking request is successful (without doing actual
    //# locking).
    if (! create) {
	File f (itsName);
	if (! f.exists()) {
	    if (!f.canCreate()  &&  !mustExist) {
		return;
	    }
	    create = True;
	}
    }
    //# Open the lock file as read/write if it exists.
    //# If it did not succeed, open as readonly.
    int fd;
    if (!create) {
	fd = FiledesIO::open (itsName, True, False);
	if (fd == -1) {
	    fd = FiledesIO::open (itsName, False);
	    itsWritable  = False;
	    itsAddToList = False;
	}
    }else{
	//# Create a new file with world write access.
	//# Initialize the values in it.
	fd = FiledesIO::create (itsName, 0666);
	putReqId (fd);
    }
    //# Create FileLocker objects for this lock file.
    //# The first one is for read/write locks.
    //# The second one is to set the file to "in use".
    itsLocker    = FileLocker (fd, 2*seqnr, 1);
    itsUseLocker = FileLocker (fd, 2*seqnr+1, 1);
    itsFileIO = new FiledesIO (fd);
    itsCanIO  = new CanonicalIO (itsFileIO);
    // Set the file to in use by acquiring a read lock.
    itsUseLocker.acquire (FileLocker::Read, 1);
}

LockFile::~LockFile()
{
    delete itsCanIO;
    delete itsFileIO;
    int fd = itsLocker.fd();
    if (fd >= 0) {
	FiledesIO::close (fd);
    }
}

Bool LockFile::isMultiUsed()
{
    //# If a write lock cannot be obtained, the file is in use.
    return (ToBool (itsUseLocker.fd() >= 0
                &&  !itsUseLocker.canLock (FileLocker::Write)));
}

Bool LockFile::acquire (MemoryIO* info, FileLocker::LockType type,
			uInt nattempts)
{
    //# When no lock file, lock requests always succeed,
    //# but we cannot return any info.
    if (itsFileIO == 0) {
	if (info != 0) {
	    info->clear();
	}
	return True;
    }
    //# Try to set a lock without waiting.
    Bool succ = itsLocker.acquire (type, 1);
    Bool added = False;
    //# When unsuccessful and multiple attempts have to be done,
    //# add the process to the request list (if needed) and try to acquire.
    if (!succ  &&  nattempts != 1) {
	if (itsAddToList) {
	    addReqId();
	    added = True;
	}
	succ = itsLocker.acquire (type, nattempts);
    }
    //# Do not read info if we did not acquire the lock.
    if (!succ) {
	info = 0;
    }
    //# Read the info when needed.
    //# This also reads the request id's.
    //# If no info is needed, read req id's only when needed.
    //# Note that each IO-operation is quite expensive, so do as few
    //# IO's as possible.
    if (info != 0) {
	getInfo (*info);
    } else if (added) {
	getReqId();
    }
    //# Remove this id from the request list (if added).
    if (added) {
	removeReqId();
    }
    //# Reset the inspection time.
    itsLastTime.now();
    return succ;
}

Bool LockFile::release (const MemoryIO* info)
{
    //# When no lock file, lock requests are not really handled.
    if (itsFileIO == 0) {
	return True;
    }
    if (info != 0) {
	putInfo (*info);
    }
    return itsLocker.release();
}

Bool LockFile::inspect()
{
    //# When no lock file, lock requests are not really handled.
    if (itsFileIO == 0) {
	return False;
    }
    //# Only inspect if time interval has passed.
    if (itsInterval > 0  &&  itsLastTime.age() < itsInterval) {
	return False;
    }
    //# Get the number of request id's and reset the time.
    uInt nr = getNrReqId();
    itsLastTime.now();
    return ToBool (nr > 0);
}

void LockFile::getInfo (MemoryIO& info)
{
    // The lock file contains:
    // - the fixed length request list in the first bytes
    // - thereafter the length of the info (as a uInt)
    // - thereafter the entire info
    uChar buffer[2048];
    // Read the first part of the file.
    traceLSEEK (itsLocker.fd(), 0, SEEK_SET);
    uInt leng = read (itsLocker.fd(), buffer, sizeof(buffer));
    // Extract the request list from it.
    convReqId (buffer, leng);
    // Get the length of the info.
    uInt infoLeng = getInt (buffer, leng, SIZEREQID);
    // Clear the MemoryIO object.
    info.clear();
    if (infoLeng == 0) {
	return;
    }
    // Get the length of the info in the part already read.
    // (subtract length of request list and the info-length uInt)
    leng -= SIZEREQID+SIZEINT;
    if (leng > infoLeng) {
	leng = infoLeng;
    }
    info.seek (0);
    info.write (leng, buffer + SIZEREQID + SIZEINT);
    // Read the remaining info parts.
    if (infoLeng > leng) {
	infoLeng -= leng;
	uChar* buf = new uChar[infoLeng];
	read (itsLocker.fd(), buf, infoLeng);
	info.write (infoLeng, buf);
	delete [] buf;
    }
    info.seek (0);
}

void LockFile::putInfo (const MemoryIO& info) const
{
    uInt infoLeng = ((MemoryIO&)info).length();
    if (!itsWritable  ||  infoLeng == 0) {
	return;
    }
    uChar buffer[1024];
    uInt leng = CanonicalConversion::fromLocal (buffer, infoLeng);
    traceLSEEK (itsLocker.fd(), SIZEREQID, SEEK_SET);
    if (infoLeng > 1024 - leng) {
	traceWRITE (itsLocker.fd(), (Char *)buffer, leng);
	traceWRITE (itsLocker.fd(), (Char *)info.getBuffer(), infoLeng);
    }else{
	memcpy (buffer+leng, info.getBuffer(), infoLeng);
	traceWRITE (itsLocker.fd(), (Char *)buffer, leng+infoLeng);
    }
    fsync (itsLocker.fd());
}

Int LockFile::getNrReqId() const
{
    uChar buffer[8];
    traceLSEEK (itsLocker.fd(), 0, SEEK_SET);
    uInt leng = read (itsLocker.fd(), buffer, SIZEINT);
    return getInt (buffer, leng, 0);
}

Int LockFile::getInt (const uChar* buffer, uInt leng, uInt offset) const
{
    if (leng < offset + SIZEINT) {
	return 0;
    }
    Int value;
    CanonicalConversion::toLocal (value, buffer+offset);
    return value;
}

void LockFile::convReqId (const uChar* buffer, uInt leng)
{
    if (leng >= SIZEREQID) {
	CanonicalConversion::toLocal (itsReqId.storage(), buffer,
				      SIZEREQID/SIZEINT);
    }
}

void LockFile::addReqId()
{
    //# Add the id at the last free place in the block.
    //# If full, use last element. This is better than ignoring it,
    //# because in this way the last request is always known.
    uInt inx = itsReqId[0];
    if (inx == itsReqId.nelements()) {
	inx--;
    }else{
	itsReqId[0]++;
    }
    itsReqId[2*inx+1] = itsPid;
    itsReqId[2*inx+2] = itsHostId;
    putReqId (itsLocker.fd());
}

void LockFile::removeReqId()
{
    Int i;
    //# Remove the id and all previous id's from the block.
    //# In principle previous id's should not occur, but it
    //# can happen when a process with an outstanding request died.
    Int nr = itsReqId[0];
    for (i=0; i<nr; i++) {
	if (Int(itsPid) == itsReqId[2*i+1]
        &&  Int(itsHostId) == itsReqId[2*i+2]) {
	    break;
	}
    }
    if (i < nr) {
	nr -= i+1;
	objcopy (&itsReqId[1], &itsReqId[2*i+1], 2*nr);
	itsReqId[0] = nr;
	putReqId (itsLocker.fd());
    }
}

void LockFile::putReqId (int fd) const
{
    if (itsAddToList) {
	uChar buffer[SIZEREQID];
	uInt leng = CanonicalConversion::fromLocal (buffer,
						    itsReqId.storage(),
						    itsReqId.nelements());
	traceLSEEK (fd, 0, SEEK_SET);
	traceWRITE (fd, (Char *)buffer, leng);
	fsync (fd);
    }
}

void LockFile::getReqId()
{
    int fd = itsLocker.fd();
    uChar buffer[SIZEREQID];
    traceLSEEK (fd, 0, SEEK_SET);
    if (read (fd, buffer, SIZEREQID) > 0) {
	CanonicalConversion::fromLocal (buffer,
					itsReqId.storage(),
					itsReqId.nelements());
    }
}
