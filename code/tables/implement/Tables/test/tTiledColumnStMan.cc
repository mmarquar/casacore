//# tTiledColumnStMan.cc: Test program for the TiledColumnStMan classes
//# Copyright (C) 1994,1995,1996
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

#include <aips/Tables/TableDesc.h>
#include <aips/Tables/SetupNewTab.h>
#include <aips/Tables/Table.h>
#include <aips/Tables/ScaColDesc.h>
#include <aips/Tables/ArrColDesc.h>
#include <aips/Tables/ScalarColumn.h>
#include <aips/Tables/ArrayColumn.h>
#include <aips/Tables/TiledColumnStMan.h>
#include <aips/Tables/TiledStManAccessor.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Arrays/Cube.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Arrays/ArrayUtil.h>
#include <aips/Arrays/ArrayIter.h>
#include <aips/Lattices/Slicer.h>
#include <aips/Exceptions/Error.h>
#include <iostream.h>

// <summary>
// Test program for the TiledColumnStMan class.
// </summary>

// This program tests the class TiledColumnStMan and related classes.
// The results are written to stdout. The script executing this program,
// compares the results with the reference output file.

void writeFixed();
void readTable();

main () {
    try {
	writeFixed();
	readTable();
    } catch (AipsError x) {
	cout << "Caught an exception: " << x.getMesg() << endl;
	return 1;
    } end_try;
    return 0;                           // exit with success status
}

// First build a description.
void writeFixed()
{
    // Build the table description.
    TableDesc td ("", "1", TableDesc::Scratch);
    td.addColumn (ArrayColumnDesc<float>  ("Pol", IPosition(1,16),
					   ColumnDesc::FixedShape));
    td.addColumn (ArrayColumnDesc<float>  ("Freq", 1, ColumnDesc::FixedShape));
    td.addColumn (ScalarColumnDesc<float> ("Time"));
    td.addColumn (ArrayColumnDesc<float>  ("Data", 2, ColumnDesc::FixedShape));
    td.addColumn (ArrayColumnDesc<float>  ("Weight", IPosition(2,16,20),
					   ColumnDesc::FixedShape));
    td.defineHypercolumn ("TSMExample",
			  3,
			  stringToVector ("Data,Weight"),
			  stringToVector ("Pol,Freq,Time"));
    
    // Now create a new table from the description.
    SetupNewTable newtab("tTiledColumnStMan_tmp.data", td, Table::New);
    // Create a storage manager for it.
    TiledColumnStMan sm1 ("TSMExample", IPosition(2,5,6));
    newtab.setShapeColumn ("Freq", IPosition(1,20));
    newtab.setShapeColumn ("Data", IPosition(2,16,20));
    newtab.bindAll (sm1);
    Table table(newtab);

    Vector<float> freqValues(20);
    Vector<float> polValues(16);
    indgen (freqValues.ac(), float(200));
    indgen (polValues.ac(), float(300));
    float timeValue;
    timeValue = 34;
    ArrayColumn<float> freq (table, "Freq");
    ArrayColumn<float> pol (table, "Pol");
    ArrayColumn<float> data (table, "Data");
    ArrayColumn<float> weight (table, "Weight");
    ScalarColumn<float> time (table, "Time");
    Matrix<float> array(IPosition(2,16,20));
    Matrix<float> result(IPosition(2,16,20));
    uInt i;
    indgen (array.ac());
    for (i=0; i<51; i++) {
	table.addRow();
	data.put (i, array);
	weight.put (i, array.ac()+float(100));
	time.put (i, timeValue);
	array.ac() += float(200);
	timeValue += 5;
    }
    freq.put (0, freqValues);
    pol.put (0, polValues);
    timeValue = 34;
    indgen (array.ac());
    for (i=0; i<table.nrow(); i++) {
	data.get (i, result);
	if (! allEQ (array.ac(), result.ac())) {
	    cout << "mismatch in data row " << i << endl;
	}
	weight.get (i, result);
	if (! allEQ (array.ac() + float(100), result.ac())) {
	    cout << "mismatch in weight row " << i << endl;
	}
	if (! allEQ (freq(i), freqValues.ac())) {
	    cout << "mismatch in freq row " << i << endl;
	}
	if (! allEQ (pol(i), polValues.ac())) {
	    cout << "mismatch in pol row " << i << endl;
	}
	if (time(i) != timeValue) {
	    cout << "mismatch in time row " << i << endl;
	}
	array.ac() += float(200);
	timeValue += 5;
    }
    ROTiledStManAccessor accessor (table, "TSMExample");
    accessor.showCacheStatistics (cout);
}

void readTable()
{
    Table table("tTiledColumnStMan_tmp.data");
    ROTiledStManAccessor accessor (table, "TSMExample");
    ROArrayColumn<float> freq (table, "Freq");
    ROArrayColumn<float> pol (table, "Pol");
    ROArrayColumn<float> data (table, "Data");
    ROArrayColumn<float> weight (table, "Weight");
    ROScalarColumn<float> time (table, "Time");
    Vector<float> freqValues(20);
    Vector<float> polValues(16);
    indgen (freqValues.ac(), float(200));
    indgen (polValues.ac(), float(300));
    float timeValue;
    timeValue = 34;
    Matrix<float> array(IPosition(2,16,20));
    Matrix<float> result(IPosition(2,16,20));
    uInt i;
    indgen (array.ac());
    for (i=0; i<table.nrow(); i++) {
	data.get (i, result);
	if (! allEQ (array.ac(), result.ac())) {
	    cout << "mismatch in data row " << i << endl;
	}
	weight.get (i, result);
	if (! allEQ (array.ac() + float(100), result.ac())) {
	    cout << "mismatch in weight row " << i << endl;
	}
	if (! allEQ (freq(i), freqValues.ac())) {
	    cout << "mismatch in freq row " << i << endl;
	}
	if (! allEQ (pol(i), polValues.ac())) {
	    cout << "mismatch in pol row " << i << endl;
	}
	if (time(i) != timeValue) {
	    cout << "mismatch in time row " << i << endl;
	}
	array.ac() += float(200);
	timeValue += 5;
    }
    accessor.showCacheStatistics (cout);
    accessor.clearCaches();
    cout << "get's have been done" << endl;

    // Get the entire column.
    // (In a separate scope to delete the big array at the end).
    {
	Array<float> result;
	data.getColumn (result);
	if (result.shape() != IPosition (3,16,20,51)) {
	    cout << "shape of getColumn result is incorrect" << endl;
	}else{
	    indgen (array.ac());
	    uInt i=0;
	    ArrayIterator<float> iter (result, 2);
	    while (! iter.pastEnd()) {
		if (! allEQ (iter.array(), array.ac())) {
		    cout << "mismatch in getColumn data row " << i << endl;
		}
		array.ac() += float(200);
		i++;
		iter.next();
	    }
	}
	accessor.showCacheStatistics (cout);
	accessor.clearCaches();
	cout << "getColumn has been done" << endl;
    }

    // Get slices in the entire column.
    {
	uInt i, j;
	Cube<float> array(1,1,51);
	for (i=0; i<51; i++) {
	    array(0,0,i) = 200*i;
	}
	Array<float> result;
	for (j=0; j<20; j++) {
	    for (i=0; i<16; i++) {
		data.getColumn (Slicer (IPosition(2,i,j)), result);
		if (! allEQ (result, array.ac())) {
		    cout << "mismatch in getColumnSlice " << i << "," << j
			 << endl;
		}
		array.ac() += float(1);
	    }
	}
	accessor.showCacheStatistics (cout);
	accessor.clearCaches();
	cout << "getColumnSlice's have been done" << endl;
    }

    // Get strided slices in the entire column.
    {
	uInt i, j;
	Cube<float> array(2,2,51);
	for (i=0; i<51; i++) {
	    array(0,0,i) = 200*i;
	    array(1,0,i) = 200*i + 16/2;
	    array(0,1,i) = 200*i + 16*20/2;
	    array(1,1,i) = 200*i + 16*20/2 + 16/2;
	}
	Array<float> result;
	for (j=0; j<20/2; j++) {
	    for (i=0; i<16/2; i++) {
		data.getColumn (Slicer (IPosition(2,i,j),
					IPosition(2,2,2),
					IPosition(2,16/2,20/2)),
				result);
		if (! allEQ (result, array.ac())) {
		    cout << "mismatch in getColumnSlice " << i << "," << j
			 << endl;
		}
		array.ac() += float(1);
	    }
	    array.ac() += float(8);
	}
	accessor.showCacheStatistics (cout);
	accessor.clearCaches();
	cout << "strided getColumnSlice's have been done" << endl;
    }

    // Get slices from each cell.
    {
	uInt i, j, k;
	Matrix<float> array(2,4);
	Array<float> result;
	for (k=0; k<table.nrow(); k++) {
	    for (j=0; j<4; j++) {
		for (i=0; i<2; i++) {
		    array(i,j) = float(i + 16*j + 200*k);
		}
	    }
	    for (j=0; j<20/4; j++) {
		for (i=0; i<16/2; i++) {
		    data.getSlice (k, 
				   Slicer (IPosition(2,i*2,j*4),
					   IPosition(2,2,4)),
				   result);
		    if (! allEQ (result, array.ac())) {
			cout << "mismatch in getSlice " << i << "," << j
			     << "," << k << endl;
		    }
		    array.ac() += float(2);
		}
		array.ac() += float((4-1) * 16);
	    }
	}
	accessor.showCacheStatistics (cout);
	accessor.clearCaches();
	cout << "getSlice's have been done" << endl;
    }

    // Get a strided slice from each cell.
    {
	uInt i, j, k;
	Matrix<float> array(8,5);
	Array<float> result;
	for (k=0; k<table.nrow(); k++) {
	    for (j=0; j<5; j++) {
		for (i=0; i<8; i++) {
		    array(i,j) = float(i*16/8 + 16*20/5*j + 200*k);
		}
	    }
	    for (j=0; j<20/5; j++) {
		for (i=0; i<16/8; i++) {
		    data.getSlice (k, 
				   Slicer (IPosition(2,i,j),
					   IPosition(2,8,5),
					   IPosition(2,16/8,20/5)),
				   result);
		    if (! allEQ (result, array.ac())) {
			cout << "mismatch in strided getSlice " << i << ","
			     << j << "," << k << endl;
		    }
		    array.ac() += float(1);
		}
		array.ac() += float(16 - 16/8);
	    }
	}
	accessor.showCacheStatistics (cout);
	accessor.clearCaches();
	cout << "getSlice's with strides have been done" << endl;
    }
}
