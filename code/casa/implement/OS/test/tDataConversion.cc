//# tDataConversion.cc: test program for data conversion classes
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


#include <aips/aips.h>
#include <aips/OS/DataConversion.h>
#include <aips/OS/CanonicalDataConversion.h>
#include <aips/OS/IBMDataConversion.h>
#include <aips/OS/VAXDataConversion.h>
#include <aips/OS/RawDataConversion.h>
#include <iostream.h>


// This program tests the data conversion classes.


void showConv (const DataConversion* conv)
{
    cout << "  Char externalSize = " << conv->externalSize ((Char*)0) << endl;
    cout << " uChar externalSize = " << conv->externalSize ((uChar*)0) << endl;
    cout << " Short externalSize = " << conv->externalSize ((Short*)0) << endl;
    cout << "uShort externalSize = " << conv->externalSize ((uShort*)0) <<endl;
    cout << "   Int externalSize = " << conv->externalSize ((Int*)0) << endl;
    cout << "  uInt externalSize = " << conv->externalSize ((uInt*)0) << endl;
    cout << "  Long externalSize = " << conv->externalSize ((Long*)0) << endl;
    cout << " uLong externalSize = " << conv->externalSize ((uLong*)0) << endl;
    cout << " Float externalSize = " << conv->externalSize ((Float*)0) << endl;
    cout << "Double externalSize = " << conv->externalSize ((Double*)0) <<endl;
    cout << "  Char canCopy = " << conv->canCopy ((Char*)0) << endl;
    cout << " uChar canCopy = " << conv->canCopy ((uChar*)0) << endl;
    cout << " Short canCopy = " << conv->canCopy ((Short*)0) << endl;
    cout << "uShort canCopy = " << conv->canCopy ((uShort*)0) << endl;
    cout << "   Int canCopy = " << conv->canCopy ((Int*)0) << endl;
    cout << "  uInt canCopy = " << conv->canCopy ((uInt*)0) << endl;
    cout << "  Long canCopy = " << conv->canCopy ((Long*)0) << endl;
    cout << " uLong canCopy = " << conv->canCopy ((uLong*)0) << endl;
    cout << " Float canCopy = " << conv->canCopy ((Float*)0) << endl;
    cout << "Double canCopy = " << conv->canCopy ((Double*)0) << endl;
}

main()
{
    cout << ">>>" << endl;
#if defined(LIITLE_ENDIAN)
    cout << "This is a little-endian machine" << endl;
#else
    cout << "This is a big-endian machine" << endl;
#endif
    DataConversion* d1 = new CanonicalDataConversion;
    cout << endl << "Canonical:" << endl;
    showConv (d1);
    DataConversion* d2 = new IBMDataConversion;
    cout << endl << "IBM:" << endl;
    showConv (d2);
    DataConversion* d3 = new VAXDataConversion;
    cout << endl << "VAX:" << endl;
    showConv (d3);
    DataConversion* d4 = new RawDataConversion;
    cout << endl << "Raw:" << endl;
    showConv (d4);
    cout << "<<<" << endl;
    delete d1;
    delete d2;
    delete d3;
    delete d4;
    cout << "OK" << endl;
    return 0;
}
