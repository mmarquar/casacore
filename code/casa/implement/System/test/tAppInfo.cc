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

#include <aips/Tasking/AppInfo.h>
#include <aips/Utilities/Assert.h>

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


    AlwaysAssertExit(mem == AppInfo::memoryInMB() && mem >= 64);
    AlwaysAssertExit(available <= mem && available > 0 && 
		     (mem-available==5 || mem-available==6));
    AlwaysAssertExit(ncpu == AppInfo::nProcessors() && ncpu >= 1);
    AlwaysAssertExit(tz == AppInfo::timeZone() && tz >= -1.0 && tz <= 1.0);

    delete [] cp;

    return 0;
}
