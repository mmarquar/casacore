//# tversion.cc: "test" the version information.
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
//#
//# $Id$

#include <aips/aips.h>

#include <aips/version.h>
#include <aips/Utilities/Assert.h>
#include <iostream.h>
// Cure some problems seen on dec alpha - something is defining macros
// major and minor
#if defined(major)
#undef major
#endif
#if defined(minor)
#undef minor
#endif

int main()
{
    VersionInfo::report(cout);

    // All we can really test is that the major number hasn't gotten
    // smaller than it was on the date the "test" program was written
    // 10/14/96
    AlwaysAssertExit(VersionInfo::major() >= 7);

    cout << endl << "Recapitulation (should be consistent with above)" << endl;
    cout << "Major " << VersionInfo::major() << endl;
    cout << "Minor " << VersionInfo::minor() << endl;
    cout << "Patch " << VersionInfo::patch() << endl;
    cout << "Date  " << VersionInfo::date() << endl;
    cout << "Extra info " << VersionInfo::info() << endl;

    return 0;
}

