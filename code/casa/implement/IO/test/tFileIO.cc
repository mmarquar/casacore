//# tFileIO.cc: Test program for performance of file IO
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

#include <aips/IO/RegularFileIO.h>
#include <aips/IO/FiledesIO.h>
#include <aips/OS/Timer.h>
#include <aips/Utilities/String.h>
#include <iostream.h>
#include <strstream.h>


main (int argc, char** argv)
{
    int nr = 100;
    if (argc > 1) {
	istrstream istr(argv[1]);
	istr >> nr;
    }
    int leng = 1024;
    if (argc > 2) {
	istrstream istr(argv[2]);
	istr >> leng;
    }
    int size = 0;
    if (argc > 3) {
	istrstream istr(argv[3]);
	istr >> size;
    }
    cout << "tFileIO  nrrec=" << nr << " reclength=" << leng
	 << " buffersize=" << size << endl;
    char* buf = new char[leng];
    int i;
    for (i=0; i<leng; i++) {
	buf[i] = 0;
    }
    RegularFileIO file1(RegularFile("tFileIO_tmp.data"), ByteIO::New, size);
    Timer timer;
    for (i=0; i<nr; i++) {
	file1.write (leng, buf);
    }
    timer.show ("RegularFileIO");
    delete [] buf;
    FiledesIO file2 (FiledesIO::create ("tFileIO_tmp.data"));
    timer.mark();
    for (i=0; i<nr; i++) {
	file2.write (leng, buf);
    }
    timer.show ("FiledesIO    ");
    delete [] buf;
    return 0;                           // exit with success status
}
