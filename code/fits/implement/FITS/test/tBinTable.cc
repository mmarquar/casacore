//# tBinTable.cc - this program tests the BinTable class
//# Copyright (C) 1995, 1996
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

//# Includes

#include <trial/FITS/BinTable.h>
#include <aips/FITS/fitsio.h>
#include <aips/Inputs/Input.h>
#include <aips/Exceptions/Error.h>
#include <aips/OS/File.h>

#include <iostream.h>
#include <stdio.h>

Int main(int argc, char **argv)
{
    try {
	Input inputs(1);

	inputs.Create("inputFile",
		      "",
		      "The input FITS file",
		      "String");
	inputs.Create("baseName",
		      "",
		      "The root name for all created files",
		      "String");
	inputs.Create("storageManager",
		      "miriad",
		      "The storage manager to use - miriad (RLE) or aipsio "
		      "(memory)",
		      "String");
	inputs.Create("sdfits",
		      "False",
		      "Interpret keywords as virtual columns as in the SD-FITS convention",
		      "Bool");

	inputs.ReadArguments(argc, argv);

	String inputFilename = inputs.GetString("inputFile");
	String baseName = inputs.GetString("baseName");
	String storageManagerType = inputs.GetString("storageManager");
	Bool sdfits = inputs.GetBool("sdfits");

	storageManagerType.downcase();

	Bool useMiriadSM;
	if (storageManagerType == "miriad") {
	    useMiriadSM = True;
	} else 	if (storageManagerType == "aipsio") {
	    useMiriadSM = False;
	} else {
	    cout << storageManagerType << " is not a valid storage manager" << 
	        endl;
	    return 0;
	}

	File inputFile(inputFilename);
	if (! inputFile.isReadable()) {
	    cout << inputFilename << " is not readable - exiting" << endl;
	    return 0;
	}

	Int tabCount = 0;
	// This allows for constructed names of the form baseName.table.xx
	char *tabName = new char[baseName.length() + 10];
	// construct the FITS input
	FitsInput infits(inputFilename, FITS::Disk);
	if (infits.err() != FitsIO::OK) {
	    cout << "Problem instantiating FITS input " << infits.err() << endl;
	    return 0;
	}

	while (!infits.eof()) {
	    switch (infits.hdutype()) {
	    case FITS::BinaryTableHDU:
	    {
		sprintf(tabName,"%s.table.%i",baseName.chars(),tabCount++);
		String tabNameString(tabName);
		cout << "BinaryTableHDU : " << tabNameString << " ... " ;
		BinaryTable bintab(infits, cout, useMiriadSM, sdfits);
		if (infits.err() != FitsIO::OK) {
		    cout << "Problem in infits while instantiating binary table " <<
			infits.err() << endl;
		    return 0;
		}
		Table tab = bintab.fullTable(tabNameString, Table::NewNoReplace,
					     useMiriadSM);
		if (infits.err() != FitsIO::OK) {
		    cout << "Problem in infits while making the table " <<
			infits.err() << endl;
		    return 0;
		}
		cout << "done." << endl;
	    }
	    break;
	    default:
		cout << "Unable to do anything but skip this hdutype : " << 
		    infits.hdutype() << endl;
		infits.skip_hdu();
		if (infits.err() != FitsIO::OK) {
		    cout << "Problem in infits while skipping the hdu" <<
			infits.err() << endl;
		    return 0;
		}
		break;
	    }
	}
	cout << "At end of file" << endl;
    } catch (AipsError x) {
	cout << "Exception from file : " << x.thrownFile() << endl;
	cout << "at line : " << x.thrownLine() << endl;
	cout << "Message : " << x.getMesg() << endl;
    } end_try;
    return 1;
}
