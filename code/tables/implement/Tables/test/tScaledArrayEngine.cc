//# tScaledArrayEngine.cc: Test program for class ScaledArrayEngine
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
#include <aips/Tables/ScaledArrayEngine.h>
#include <aips/Tables/ArrayColumn.h>
#include <aips/Tables/ScalarColumn.h>
#include <aips/Arrays/Cube.h>
#include <aips/Lattices/IPosition.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Lattices/Slicer.h>
#include <aips/Lattices/Slice.h>
#include <aips/Tables/TableError.h>

// <summary> Test program for class ScaledArrayEngine </summary>

// This program tests the virtual column engine ScaledArrayEngine.
// The results are written to stdout. The script executing this program,
// compares the results with the reference output file.

void a();
void b();

main () {
    try {
	a();
	b();
    } catch (AipsError x) {
	cout << "Caught an exception: " << x.getMesg() << endl;
	return 1;
    } end_try;
    return 0;                           // exit with success status
}

// First build a description.
void a() {
    // First register the virtual column engine.
    ScaledArrayEngine<double,Int>::registerClass();
    ScaledArrayEngine<float,uChar>::registerClass();

    // Build the table description.
    TableDesc td("", "1", TableDesc::Scratch);
    td.comment() = "A test of class TableDesc";
    td.addColumn (ArrayColumnDesc<Int> ("target1"));
    td.addColumn (ArrayColumnDesc<double> ("source1"));
    td.addColumn (ArrayColumnDesc<uChar> ("target2"));
    td.addColumn (ArrayColumnDesc<float> ("source2","",
					  IPosition(3,2,3,4),
					  ColumnDesc::Direct));
    td.addColumn (ArrayColumnDesc<Int> ("target3", "",
					IPosition(3,2,3,4),
					ColumnDesc::Direct));
    td.addColumn (ArrayColumnDesc<double> ("source3", "",
					   IPosition(3,2,3,4),
					   ColumnDesc::Direct));
    td.addColumn (ScalarColumnDesc<double> ("scale3"));

    // Now create a new table from the description.
    SetupNewTable newtab("tScaledArrayEngine_tmp.data", td, Table::New);
    // Create the virtual column engine with the scale factors
    // and bind the columns to them.
    ScaledArrayEngine<double,Int> engine1("source1", "target1", 2.0, 4.0);
    ScaledArrayEngine<float,uChar> engine2("source2", "target2", 6.0, 2.0);
    ScaledArrayEngine<double,Int> engine3("source3", "target3", "scale3");
    newtab.bindColumn ("source1", engine1);
    newtab.bindColumn ("source2", engine2);
    newtab.bindColumn ("source3", engine3);
    Table tab(newtab, 10);

    // Fill the table via the virtual columns.
    ArrayColumn<double> source1 (tab, "source1");
    ArrayColumn<float> source2 (tab, "source2");
    ArrayColumn<double> source3 (tab, "source3");
    ScalarColumn<double> scale3 (tab,"scale3");

    Cube<double> arrd(IPosition(3,2,3,4));
    Cube<float> arrf(IPosition(3,2,3,4));
    uInt i;
    i=2;
    for (uInt i2=0; i2<4; i2++)
	for (uInt i1=0; i1<3; i1++)
	    for (uInt i0=0; i0<2; i0++) {
		arrd(i0,i1,i2) = i;
		arrf(i0,i1,i2) = i;
		i += 6;
	    }
    for (i=0; i<10; i++) {
	scale3.put (i, 1 + i%3);
	source1.put (i, arrd);
	source2.put (i, arrf);
	source3.put (i, arrd.ac() + (double)4);
	arrd.ac() += (double)(6*arrd.nelements());
	arrf.ac() += (float)(6*arrd.nelements());
    }

    //# Do an erronous thing.
    SetupNewTable newtab2("tScaledArrayEngine_tmp.dat2", td, Table::Scratch);
    newtab2.bindColumn ("source2", engine1);
///    try {
///	Table tab2(newtab2, 10);                // bound to incorrect column
///    } catch (AipsError x) {
///	cout << x.getMesg() << endl;
///    } end_try;
}

void b()
{
    // Read back the table.
    Table tab("tScaledArrayEngine_tmp.data");
    ROArrayColumn<double> source1 (tab, "source1");
    ROArrayColumn<float> source2 (tab, "source2");
    ROArrayColumn<double> source3 (tab, "source3");
    ROArrayColumn<Int> target1 (tab, "target1");
    ROArrayColumn<uChar> target2 (tab, "target2");
    ROArrayColumn<Int> target3 (tab, "target3");
    Cube<Int> arri1(IPosition(3,2,3,4));
    Cube<Int> arri3(IPosition(3,2,3,4));
    Cube<Int> arrvali(IPosition(3,2,3,4));
    Cube<uChar> arrc2(IPosition(3,2,3,4));
    Cube<uChar> arrvalc(IPosition(3,2,3,4));
    Cube<double> arrd1(IPosition(3,2,3,4));
    Cube<double> arrd3(IPosition(3,2,3,4));
    Cube<double> arrvald(IPosition(3,2,3,4));
    Cube<float> arrf2(IPosition(3,2,3,4));
    Cube<float> arrvalf(IPosition(3,2,3,4));
    Cube<double> arrvalslice(arrvald(Slice(0,1),Slice(0,1,2),Slice(0,2,2)));
    Slice tmp;
    Slicer nslice (tmp, tmp, tmp,  Slicer::endIsLength);
    Slicer nslice2(Slice(0,1), Slice(0,1,2), Slice(0,2,2),
		   Slicer::endIsLength);
    uInt i=0;
    for (uInt i2=0; i2<4; i2++)
	for (uInt i1=0; i1<3; i1++)
	    for (uInt i0=0; i0<2; i0++) {
		arrd1(i0,i1,i2) = 2 + 6*i;
		arrf2(i0,i1,i2) = 2 + 6*i;
		arrd3(i0,i1,i2) = 6 + 6*i;
		arri1(i0,i1,i2) = 3*i - 1;
		arrc2(i0,i1,i2) = i;
		arri3(i0,i1,i2) = 6 + 6*i;
		i++;
	    }
    for (i=0; i<10; i++) {
	cout << "get row " << i << endl;
	source1.get (i, arrvald);
	if (!allEQ (arrvald.ac(), arrd1.ac())) {
	    cout << "error in source1 in row " << i << endl;
	}
	target1.get (i, arrvali);
	if (!allEQ (arrvali.ac(), arri1.ac())) {
	    cout << "error in target1 in row " << i << endl;
	}
	source2.get (i, arrvalf);
	if (!allEQ (arrvalf.ac(), arrf2.ac())) {
	    cout << "error in source2 in row " << i << endl;
	}
	target2.get (i, arrvalc);
	if (!allEQ (arrvalc.ac(), arrc2.ac())) {
	    cout << "error in target2 in row " << i << endl;
	}
	source3.get (i, arrvald);
	if (!allEQ (arrvald.ac(), arrd3.ac())) {
	    cout << "error in source3 in row " << i << endl;
	}
	target3.get (i, arrvali);
	if (!allEQ (arrvali.ac(), arri3.ac()/(Int)(1+i%3))) {
	    cout << "error in target3 in row " << i << endl;
	}
	source1.getSlice (i, nslice, arrvald);
	if (!allEQ (arrvald.ac(), arrd1.ac())) {
	    cout << "error in source1 (entire slice) in row " << i << endl;
	}
	source1.getSlice (i, nslice2, arrvalslice);
	if (!allEQ (arrvald.ac(), arrd1.ac())) {
	    cout << "error in source1 (partial slice) in row " << i << endl;
	}
	arrd1.ac() += (double)(6*arrd1.nelements());
	arrf2.ac() += (float)(6*arrf2.nelements());
	arrd3.ac() += (double)(6*arrd3.nelements());
	arri1.ac() += (Int)(3*arri1.nelements());
	arrc2.ac() += (uChar)(arrc2.nelements());
	arri3.ac() += (Int)(6*arri3.nelements());
    }
}
