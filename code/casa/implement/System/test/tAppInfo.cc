//# tAppInfo.cc: Test the AppInfo class
//# Copyright (C) 1996,1997
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
//#
//# $Id$

#include <aips/Exceptions/Error.h>
#include <aips/Arrays/Vector.h>
#include <aips/Utilities/Assert.h>
#include <aips/Tasking/AppInfo.h>
#include <aips/Utilities/Assert.h>
#include <aips/Arrays/ArrayIO.cc>
#include <aips/Utilities/String.h>
#include <aips/Tasking/AipsrcVector.h>
#include <aips/Utilities/Regex.h>

#include <aips/OS/Memory.h>

int main()
{
    // Not much testing is possible!
    char *cp = new char[5*1024*1024]; // Eat some memory

    uInt mem  = AppInfo::memoryInMB();
    Int available = AppInfo::availableMemoryInMB();
    uInt ncpu = AppInfo::nProcessors();
    Double tz   = AppInfo::timeZone();

    cout << "Memory in MB            : " << mem << endl;
    cout << "Available memory in MB  : " << available;
    cout << " (should be 5+ MB less than memory)" << endl;
    cout << "# of processors         : " << ncpu << endl;
    cout << "Timezone offset (hours) : " << tz*24.0 << endl;
    delete [] cp;

    AlwaysAssertExit(mem == AppInfo::memoryInMB() && mem >= 64);
    // Some OS/library combinations might not be able to track memory
    // use.
    if (available != Int(mem)) {
	AlwaysAssertExit(available <= Int(mem) && available > 0 && 
			 (mem-available==5 || mem-available==6));
    }
    AlwaysAssertExit(ncpu == AppInfo::nProcessors() && ncpu >= 1);
    AlwaysAssertExit(tz == AppInfo::timeZone() && tz >= -1.0 && tz <= 1.0);

    { // Test the work directory stuff
	// First set the work directory list to a known state

	// We have to initialize the work directories first before
	// we register the directories we really want.
	Vector<String> tmp = AppInfo::workDirectories();
	tmp.resize(0);

	Vector<String> dirs(3);
	dirs(0) = "."; dirs(1) = "/tmp"; dirs(2) = "/doesnotexist";
	uInt index = AipsrcVector<String>::registerRC(
				      "user.directories.work", dirs);
	AipsrcVector<String>::set(index, dirs);
	cerr << "\n\n=====Expect a single WARN level log message\n" << endl;
	tmp = AppInfo::workDirectories();
	AlwaysAssertExit(tmp.nelements()==2 && tmp(0) == dirs(0) && 
			 tmp(1) == dirs(1));
	// OK, now only have valid directories so we don't get all kinds
	// of log WARNings.
	AipsrcVector<String>::set(index, tmp);
	tmp.resize(0);
	// Someday this test will fail!
	tmp = AppInfo::workDirectories(1000000);
	AlwaysAssertExit(tmp.nelements() == 0);

	// Check that we cycle through the valid directories
	tmp = AppInfo::workDirectories();
	String dir1 = AppInfo::workDirectory();
	String dir2 = AppInfo::workDirectory();
	AlwaysAssertExit(dir1 != dir2 &&
			 (dir1 == tmp(0) || dir2 == tmp(1)) &&
			 (dir2 == tmp(0) || dir2 == tmp(1)));


	String file = AppInfo::workFileName();
	AlwaysAssertExit(file.contains(dir1) || file.contains(dir2));
	AlwaysAssertExit(file.contains(Regex("/aipstmp_")));
	file = AppInfo::workFileName(0, "foo_");
	AlwaysAssertExit(file.contains(dir1) || file.contains(dir2));
	AlwaysAssertExit(file.contains(Regex("/foo_")));

	Bool caught = False;
	try {
	    cerr << "=====Expect a single SEVERE level message\n";
	    file = AppInfo::workFileName(1000000);
	} catch (AipsError x) {
	    caught = True;
	} end_try;
	cerr << "=====There should be no more messages\n\n";
	AlwaysAssertExit(caught);
    }

    cerr << "OK" << endl;
    return 0;
}
