//# LargeRegularFileIO.cc: Class for IO on a regular large file
//# Copyright (C) 1996,1997,1998,1999,2001
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

#include <aips/aips.h>
#include <aips/IO/LargeRegularFileIO.h>
#include <aips/IO/LargeIOFuncDef.h>
#include <aips/Utilities/String.h>
#include <aips/Utilities/Assert.h>
#include <aips/Exceptions/Error.h>
#include <aips/stdio.h>
#include <errno.h>                // needed for errno
#include <aips/string.h>               // needed for strerror


LargeRegularFileIO::LargeRegularFileIO (const RegularFile& regularFile,
					ByteIO::OpenOption option,
					uInt bufferSize)
: itsOption      (option),
  itsRegularFile (regularFile)
{
    const String& name = itsRegularFile.path().expandedName();
    // Open file as input and/or output.
    Bool writable = True;
    String stropt;
    switch (option) {
    case ByteIO::Old:
	writable = False;
	stropt = "rb";
	break;
    case ByteIO::New:
    case ByteIO::Scratch:
	stropt = "wb+";
	break;
    case ByteIO::NewNoReplace:
	if (regularFile.exists()) {
	    throw (AipsError ("LargeRegularFileIO: new file " + name +
			      " already exists"));
	}
	stropt = "wb+";
	break;
    case ByteIO::Append:
	stropt = "ab+";
	break;
    case ByteIO::Update:
    case ByteIO::Delete:
	stropt = "rb+";
	break;
    default:
	throw (AipsError ("LargeRegularFileIO: unknown open option"));
    }
    // Open the file.
    FILE* file = traceFOPEN ((char *)name.chars(), (char *)stropt.chars());
    if (file == 0) {
	throw (AipsError ("LargeRegularFileIO: "
			  "error in open or create of file " +
			  name + ": " + strerror(errno)));
    }
    attach (file, bufferSize, True, writable);
    fillSeekable();
}

LargeRegularFileIO::~LargeRegularFileIO()
{
    detach();
    if (itsOption == ByteIO::Scratch  ||  itsOption == ByteIO::Delete) {
	itsRegularFile.remove();
    }
}


void LargeRegularFileIO::reopenRW()
{
    if (isWritable()) {
	return;
    }
    // First try if the file can be opened as read/write.
    const String& name = itsRegularFile.path().expandedName();
    FILE* file = traceFOPEN ((char *)name.chars(), "rb+");
    if (file == 0) {
	throw (AipsError ("LargeRegularFileIO::reopenRW "
			  "not possible for file " +
			  name + ": " + strerror(errno)));
    }
    uInt bufsize = bufferSize();
    detach();
    attach (file, bufsize, True, True);
    // It can be reopened, so close and reopen.
    itsOption = ByteIO::Update;
}


String LargeRegularFileIO::fileName() const
{
    return itsRegularFile.path().expandedName();
}

void LargeRegularFileIO::flush()
{
    fflush (getFilePtr());
}

