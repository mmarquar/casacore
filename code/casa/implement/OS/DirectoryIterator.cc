//# DirectoryIterator.cc: Class to define a DirectoryIterator
//# Copyright (C) 1996
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


#include <aips/OS/DirectoryIterator.h>
#include <aips/OS/Path.h>
#include <aips/Exceptions/Error.h>

#include <errno.h>                // needed for errno
#include <string.h>               // needed for strerror


DirectoryIterator::DirectoryIterator()
: itsDirectory           (),
  itsExpression          (".*"),
  itsDirectoryEntry      (0),
  itsDirectoryDescriptor (0),
  itsEnd                 (False)
{
    init();
}

DirectoryIterator::DirectoryIterator (const Directory& dir)
: itsDirectory           (dir),
  itsExpression          (".*"),
  itsDirectoryEntry      (0),
  itsDirectoryDescriptor (0),
  itsEnd                 (False)
{
    init();
}

DirectoryIterator::DirectoryIterator (const Directory& dir,
				      const Regex& regExpression)
: itsDirectory           (dir),
  itsExpression          (regExpression),
  itsDirectoryEntry      (0),
  itsDirectoryDescriptor (0),
  itsEnd                 (False)
{
    init();
}

DirectoryIterator::DirectoryIterator (const DirectoryIterator& that)
: itsDirectory           (that.itsDirectory),
  itsExpression          (that.itsExpression),
  itsDirectoryEntry      (0),
  itsDirectoryDescriptor (0),
  itsEnd                 (False)
{
    init();
}

DirectoryIterator::~DirectoryIterator()
{
    // Free all the memory used by DirectoryIterator.
    closedir (itsDirectoryDescriptor);
}

DirectoryIterator& DirectoryIterator::operator= (const DirectoryIterator& that)
{
    if (this != &that) {
	closedir (itsDirectoryDescriptor);
	itsDirectory           = that.itsDirectory;
	itsExpression          = that.itsExpression;
	itsDirectoryDescriptor = 0;
	itsDirectoryEntry      = 0;
	itsEnd                 = False;
	init();
    }
    return *this;
}


void DirectoryIterator::init()
{
    // Set the private directory on the current working directory
    // Open the directory, if this is not possible throw an exception
    itsDirectoryDescriptor = opendir(itsDirectory.path().expandedName());
    if (itsDirectoryDescriptor == 0){
	throw (AipsError ("DirectoryIterator: error on directory " +
			  itsDirectory.path().expandedName() +
			  ": " + strerror(errno)));
    }
    // Set itsDirectoryEntry on the first entry.
    operator++();
}

void DirectoryIterator::operator++()
{
    if (itsEnd) {
	throw (AipsError ("DirectoryIterator++ past end on " +
			  itsDirectory.path().expandedName()));
    }
    // Read the entries until a match with the expression is found.
    // Skip . and ..
    String name;
    do {
	itsDirectoryEntry = readdir (itsDirectoryDescriptor);
	if (itsDirectoryEntry == 0){
	    itsEnd = True;
	    break;
	}
        name = itsDirectoryEntry->d_name;
    }
    while (name == "."  ||  name == ".."
       ||  name.matches (itsExpression) == 0);
}

void DirectoryIterator::operator++(int)
{
    operator++();
}


String DirectoryIterator::name() const
{
    if (itsEnd) {
	throw (AipsError ("DirectoryIterator::name past end on " +
			  itsDirectory.path().expandedName()));
    }
    return itsDirectoryEntry->d_name;
}

File DirectoryIterator::file() const
{
    return itsDirectory.path().expandedName() + "/" + name();
}

void DirectoryIterator::reset()
{
    // Reset the directory to the beginning of the stream
    // and get the first entry.
    rewinddir (itsDirectoryDescriptor);
    itsEnd = False;
    operator++();
}

Bool DirectoryIterator::pastEnd() const
{
    return ToBool (itsEnd == True);
}
