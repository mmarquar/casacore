//# tVSCEngine.cc: Test program for class VSCEngine
//# Copyright (C) 1994,1995,1996,1999,2000,2001
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

//# Define the variable to exclude the main from dVSCEngine.cc
#define DVSCENGINE_MAIN


//# Includes
#include <aips/Tables/test/dVSCEngine.h>
#include <aips/Tables/test/dVSCEngine.cc>
#include <aips/Tables/TableDesc.h>
#include <aips/Tables/SetupNewTab.h>
#include <aips/Tables/Table.h>
#include <aips/Tables/ScaColDesc.h>
#include <aips/Tables/ScalarColumn.h>
#include <aips/Arrays/IPosition.h>
#include <aips/Arrays/Vector.h>
#include <aips/Tables/TableError.h>


// <summary> Test program for class VSCEngine </summary>

// This program tests the virtual column engine VSCEngine.
// It is using the example class VSCExampleVSCEngine for that purpose.
// The results are written to stdout. The script executing this program,
// compares the results with the reference output file.

void a();
void b();

int main () {
    try {
	a();
	b();
    } catch (AipsError x) {
	cout << "Caught an exception: " << x.getMesg() << endl;
	return 1;
    } 
    return 0;                           // exit with success status
}

// First build a description.
void a() {
    // First register the virtual column engine.
    VSCExampleVSCEngine::registerClass();
    // Add ScalarColumnDesc<VSCExample> to column type map.
    ScalarColumnDesc<VSCExample> tmp(ColumnDesc::registerMap);

    // Build the table description.
    TableDesc td("", "1", TableDesc::Scratch);
    td.comment() = "A test of class TableDesc";
    td.addColumn (ScalarColumnDesc<Int>   ("x"));
    td.addColumn (ScalarColumnDesc<float> ("y"));
    td.addColumn (ScalarColumnDesc<VSCExample> ("colA"));

    // Now create a new table from the description.
    SetupNewTable newtab("tVSCEngine_tmp.data", td, Table::New);
    // Create the virtual column engine with the target columns x and y.
    VSCExampleVSCEngine engine ("colA", "x", "y");
    newtab.bindColumn ("colA", engine);
    Table tab(newtab, 10);

    // Fill the table via the virtual columns.
    ScalarColumn<VSCExample> colA (tab,"colA");
    uInt i;
    for (i=0; i<10; i++) {
	colA.put (i, VSCExample(i,i+1));
    }

    //# Do an erronous thing.
    SetupNewTable newtab2("tVSCEngine_tmp.dat2", td, Table::Scratch);
    newtab2.bindColumn ("x", engine);
///    try {
///	Table tab2(newtab2, 10);                // bound to incorrect column
///    } catch (AipsError x) {
///	cout << x.getMesg() << endl;
///    } 
}

void b()
{
    // Read back the table.
    Table tab("tVSCEngine_tmp.data");
    ROScalarColumn<Int> colx (tab, "x");
    ROScalarColumn<float> coly (tab, "y");
    ROScalarColumn<VSCExample> colA(tab, "colA");
    Int valx;
    float valy;
    VSCExample valA;
    Int i;
    for (i=0; i<10; i++) {
	cout << "get row " << i << endl;
	colx.get (i, valx);
	coly.get (i, valy);
	colA.get (i, valA);
	if (valx != i  ||  valy != i+1  ||  !(valA == VSCExample(i,i+1))) {
	    cout << "error: " << valx << " " << valy << " "
		 << valA.x() << " " << valA.y() << endl;
	}
    }
    Vector<VSCExample> vecA = colA.getColumn();
    for (i=0; i<10; i++) {
	if (!(vecA(i) == VSCExample(i,i+1))) {
	    cout << "error in vecA(" << i << "): "
		 << vecA(i).x() << " " << vecA(i).y() << endl;
	}
    }
}
