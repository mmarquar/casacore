//# RegularFile.cc: Manipulate and get information about regular files
//# Copyright (C) 1993,1994,1995,1996,1997,2001
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


#include <aips/Exceptions.h>
#include <aips/OS/RegularFile.h>
#include <aips/OS/SymLink.h>

#include <sys/stat.h>             // needed for stat
#include <sys/resource.h>         // needed for rlimit, etc.
#include <fcntl.h>                // needed for creat
#include <unistd.h>               // needed for unlink, etc.
#include <errno.h>                // needed for errno
#include <aips/string.h>               // needed for strerror
#include <aips/stdlib.h>               // needed for system
#if defined(__hpux__)
#include <limits.h>               // needed for MAX__UINT
#endif


RegularFile::RegularFile ()
: File()
{}

RegularFile::RegularFile (const Path& path)
: File(path)
{
    checkPath();
}

RegularFile::RegularFile (const String& path)
: File(path)
{
    checkPath();
}

RegularFile::RegularFile (const File& file)
: File(file)
{
    checkPath();
}
    
RegularFile::RegularFile (const RegularFile& that)
: File    (that),
  itsFile (that.itsFile)
{}

RegularFile::~RegularFile()
{}

RegularFile& RegularFile::operator= (const RegularFile& that)
{
    if (this != &that) {
	File::operator= (that);
	itsFile = that.itsFile;
    }
    return *this;
}

void RegularFile::checkPath()
{
    itsFile = *this;
    // If exists, check if it is a regular file.
    // If the file is a symlink, resolve the entire symlink chain.
    // Otherwise check if it can be created.
    if (exists()) {
	if (isSymLink()) {
	    itsFile = SymLink(*this).followSymLink();
	    // Error if no regular file and exists or cannot be created.
	    if (!itsFile.isRegular()) {
		if (itsFile.exists() || !itsFile.canCreate()) {
		    throw (AipsError ("RegularFile: " + path().expandedName()
				      + " is a symbolic link not"
				      " pointing to a valid regular file"));
		}
	    }
	} else if (!isRegular()) {
	    throw (AipsError ("RegularFile: " + path().expandedName() +
			      " exists, but is no regular file"));
	}
    } else {
	if (!canCreate()) {
	    throw (AipsError ("RegularFile: " + path().expandedName() +
			      " does not exist and cannot be created"));
	}
    }
}

void RegularFile::create (Bool overwrite) 
{
    // If overwrite is False the file will not be overwritten.
    if (exists()) {
	if (!itsFile.isRegular (False)) {
	    throw (AipsError ("RegularFile::create: " +
			      itsFile.path().expandedName() +
			      " already exists as a non-regular file"));
	}
	if (!overwrite) {
	    throw (AipsError ("RegularFile::create: " +
			      itsFile.path().expandedName() +
			      " already exists"));
	}
    }
    int fd = ::creat (itsFile.path().expandedName(), 0644);
    if (fd < 0) {
	throw (AipsError ("RegularFile::create error on " +
			  itsFile.path().expandedName() +
			  ": " + strerror(errno)));
    }
    ::close (fd);
}

void RegularFile::remove() 
{
    if (isSymLink()) {
	removeSymLinks();
    }    
    unlink (itsFile.path().expandedName());
}

void RegularFile::copy (const Path& target, Bool overwrite,
			Bool setUserWritePermission) const
{
    Path targetName(target);
    checkTarget (targetName, overwrite);
    // This function uses the system function cp.	    
    String call("cp ");
    call += itsFile.path().expandedName() + " " + targetName.expandedName();
    system (call);
    if (setUserWritePermission) {
	File result(targetName.expandedName());
	if (! result.isWritable()) {
	    result.setPermissions (result.readPermissions() | 0200);
	}
    }
}

void RegularFile::move (const Path& target, Bool overwrite)
{
    Path targetName(target);
    checkTarget (targetName, overwrite);
    // This function uses the system function mv.	    
    String call("mv ");
    call += itsFile.path().expandedName() + " " + targetName.expandedName();
    system (call);
}

uInt RegularFile::size() const
{
    // The struct buf is filled in by mylstat, and the size 
    // of the file is extracted from buf.
    struct stat buf;
    getstat (itsFile.path().expandedName(), &buf);
    return buf.st_size;
}
    
uInt RegularFile::maxSize()
{
    // Return the maximum size of a file  by using getrlimit.
    // HP RLIMIT_FSIZE is not defined on HPUX.
    // Return maxuint in that case.
#if defined(__hpux__)
    return UINT_MAX;
#else
    struct rlimit limit;
    getrlimit(RLIMIT_FSIZE, &limit);
    return limit.rlim_max;
#endif
}
