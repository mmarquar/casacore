//# tTableDesc.cc: Test program for the TableDesc class
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

#include <tTableDesc.h>
#include <aips/Tables/TableDesc.h>
#include <aips/Tables/ScaColDesc.h>
#include <aips/Tables/ArrColDesc.h>
#include <aips/Tables/SubTabDesc.h>
#include <aips/Tables/DataManager.h>
#include <aips/Tables/TableRecord.h>
#include <aips/Lattices/IPosition.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/Array.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Arrays/ArrayIO.h>
#include <aips/Utilities/Assert.h>
#include <iostream.h>

// <summary> Test program for the TableDesc class </summary>

// This program tests the class TableDesc and related classes.
// The results are written to stdout. The script executing this program,
// compares the results with the reference output file.


// Remove eventually - needed for g++
typedef Vector<String> forgnugpptTableDesc;

// First build a description.
void a (Bool doExcp)
{
    // Add Scalar/ArrayColumnDesc<ExampleDesc> to column type map.
    ScalarColumnDesc<ExampleDesc> tmp0(ColumnDesc::registerMap);
    ArrayColumnDesc<ExampleDesc>  tmp1(ColumnDesc::registerMap);

    // First build the description of a subtable.
    // Do it in separate scope to destruct it (thus to write it).
    {
	TableDesc subtd("tTableDesc_tmp_sub", "1", TableDesc::New);
	subtd.keywordSet().define ("subint", Int(10));
	subtd.addColumn (ScalarColumnDesc<double> ("ra"));
	subtd.addColumn (ScalarColumnDesc<double> ("dec"));
    }
    TableDesc subtd("tTableDesc_tmp_sub", "1", TableDesc::Update);
    // Now build the main table description.
    uInt i;
    ColumnDesc cd,cd2;
    Vector<double> arr(4);
    for (i=0; i<4; i++) {
	arr(i) = i;
    }
    TableDesc td("tTableDesc_tmp", "1", TableDesc::New);
    td.comment() = "A test of class TableDesc";
    td.keywordSet().define ("ra", float(3.14));
    td.keywordSet().define ("equinox", double(1950));
    td.keywordSet().define ("aa", Int(1));

    td.addColumn (ScalarColumnDesc<Int> ("ab","Comment for column ab"));
    if (doExcp) {
	try {
	    td.addColumn (ScalarColumnDesc<Int> ("ab"));   // already exists
	} catch (AipsError x) {
	    cout << x.getMesg() << endl;
	} end_try;
    }
    td.addColumn (ScalarColumnDesc<Int> ("ac"));
    td.rwColumnDesc("ac").keywordSet().define ("scale", Complex(0,0));
    td.rwColumnDesc("ac").keywordSet().define ("unit", "");
    td.addColumn (ScalarColumnDesc<uInt> ("ad","comment for ad"));
    td.rwColumnDesc("ac").keywordSet().define ("unit", "DEG");
    td.addColumn (ScalarColumnDesc<ExampleDesc> ("ae"));
    td.addColumn (ArrayColumnDesc<ExampleDesc> ("arr0"));
    if (doExcp) {
	try {
	    td.addColumn (ScalarColumnDesc<ExampleDesc>
			                     ("af", ColumnDesc::Undefined));
    	} catch (AipsError x) {
	    cout << x.getMesg() << endl;     // undefined given
	} end_try;
    }

    td.addColumn (ArrayColumnDesc<Complex> ("Arr1","comment for Arr1",0));
    td.addColumn (ArrayColumnDesc<Int> ("A2r1","comment for Arr1",3));
    if (doExcp) {
	try {
	    td.addColumn (ArrayColumnDesc<uInt> ("Arr3","comment for Arr1",
						 IPosition(2,3,4)));
	} catch (AipsError x) {
	    cout << x.getMesg() << endl;     // shape for non-fixedShape array
	} end_try;
    }
    ArrayColumnDesc<uInt> coldes("Arr3","comment for Arr1",
				 IPosition(2,3,4), ColumnDesc::Direct);
    Matrix<uInt> defmat(3,4);
    defmat = 0;
    Matrix<uInt> defmat2(4,4);
    td.addColumn (coldes);

    // Set the shape of some columns.
    if (doExcp) {
	try {
	    td.rwColumnDesc("ab").setNdim (2);
	} catch (AipsError x) {
	    cout << x.getMesg() << endl;     // column is a scalar
	} end_try;
	try {
	    td.rwColumnDesc("ab").setShape (IPosition());
	} catch (AipsError x) {
	    cout << x.getMesg() << endl;     // column is a scalar
	} end_try;
	try {
	    td.rwColumnDesc("Arr3").setNdim (2);
	} catch (AipsError x) {
	    cout << x.getMesg() << endl;     // ndim already defined
	} end_try;
	try {
	    td.rwColumnDesc("Arr3").setShape (IPosition());
	} catch (AipsError x) {
	    cout << x.getMesg() << endl;     // shape already defined
	} end_try;
    }
    td.addColumn (SubTableDesc("sub1", "subtable by name",
                  "tTableDesc_tmp_sub"));
    td.addColumn (SubTableDesc("sub2", "subtable copy", subtd));
    td.addColumn (SubTableDesc("sub3", "subtable pointer", &subtd));
    td.addColumn (SubTableDesc("sub4", "subtable by name",
                  "tTableDesc_tmp_sub1"), "sub4Renamed");
    td.show();
    cout << td.keywordSet().isDefined("aa") << td.isColumn("ab")
	 << td.keywordSet().isDefined("ab") << td.isColumn("aa") << endl;

    // Not all subtables are known.
    cout << "checkSubTableDesc: ";
    if (doExcp) {
	try {
	    td.checkSubTableDesc();
	} catch (AipsError x) {
	    cout << x.getMesg() << endl;     // subtable sub1 does not exist
	} end_try;
    }

    // Add another column and keyword to the subtable to test
    // if they get reflected in the subtable. They should for sub3,
    // but not in sub1 and sub2.
    subtd.addColumn (ScalarColumnDesc<float> ("Equinox"));
    subtd.keywordSet().define ("CoordSys", "equatorial");
    td["sub1"].tableDesc()->show();     // should have 2 columns
    td["sub3"].tableDesc()->show();     // should have 3 columns
    td["sub2"].tableDesc()->show();     // should have 2 columns

    // Test isDisjoint, etc..
    TableDesc tdx(td);
    const ColumnDescSet& set1 = td["sub1"].tableDesc()->columnDescSet();
    const ColumnDescSet& set2 = td["sub2"].tableDesc()->columnDescSet();
    const ColumnDescSet& set3 = td["sub3"].tableDesc()->columnDescSet();
    Bool equalDataTypes;
    AlwaysAssertExit (set2.isDisjoint (tdx.columnDescSet()));
    tdx.addColumn (ScalarColumnDesc<float> ("ra"));
    AlwaysAssertExit (! set2.isDisjoint (tdx.columnDescSet()));
    AlwaysAssertExit (! set2.isSubset (tdx.columnDescSet(), equalDataTypes));
    tdx.addColumn (ScalarColumnDesc<double> ("dec"));
    AlwaysAssertExit (set2.isSubset (tdx.columnDescSet(), equalDataTypes));
    AlwaysAssertExit (! equalDataTypes);
    
    AlwaysAssertExit (! set1.isDisjoint (set2));
    AlwaysAssertExit (set1.isEqual (set2, equalDataTypes));
    AlwaysAssertExit (equalDataTypes);
    AlwaysAssertExit (set1.isSubset (set2, equalDataTypes));
    AlwaysAssertExit (set1.isSuperset (set2, equalDataTypes));
    AlwaysAssertExit (! set1.isStrictSubset (set2, equalDataTypes));
    AlwaysAssertExit (! set1.isStrictSuperset (set2, equalDataTypes));
    AlwaysAssertExit (! set3.isEqual (set2, equalDataTypes));
    AlwaysAssertExit (! set3.isSubset (set2, equalDataTypes));
    AlwaysAssertExit (set3.isSuperset (set2, equalDataTypes));
    AlwaysAssertExit (! set3.isStrictSubset (set2, equalDataTypes));
    AlwaysAssertExit (set3.isStrictSuperset (set2, equalDataTypes));
}

// Remove some keywords/columns.
// Do some tests of the options for the constructor.
void b (Bool doExcp) {
    TableDesc td("tTableDesc_tmp", TableDesc::Update);
    cout << td.columnNames().ac() << endl;
    cout << (td.columnDesc("ab") == td.columnDesc("ac"));
    cout << (td.columnDesc("ad") == td.columnDesc("ae"));
    cout << (td.columnDesc("ab") == td.columnDesc("Arr1"));
    cout << (td.columnDesc("Arr1") == td.columnDesc("Arr3"));
    cout << (td.columnDesc("Arr3") == td.columnDesc("ad"));
    cout << (td.columnDesc("A2r1") == td.columnDesc("ab"));
    cout << (td.columnDesc("sub1") != td.columnDesc("sub2"));
    cout << (td.columnDesc("sub1") != td.columnDesc("ab"));
    cout << endl;
    td.show();
    td["sub1"].tableDesc()->show();     // should have 3 columns
    td["sub2"].tableDesc()->show();     // should have 2 columns
    td["sub3"].tableDesc()->show();     // should have 3 columns
    cout << "endshow" << endl;

    // Set the shape of Arr1.
    td.rwColumnDesc("Arr1").setNdim (2);
    if (doExcp) {
	try {
	    td.rwColumnDesc("Arr1").setShape (IPosition(3,4,5,6));
	} catch (AipsError x) {
	    cout << x.getMesg() << endl;        // shape mimatches ndim
	} end_try;
    }
    td.rwColumnDesc("Arr1").setShape (IPosition(2,4,5));

    if (doExcp) {
	try {
	    td["Arr1"].tableDesc();              // no subtable
	} catch (AipsError x) {
	    cout << x.getMesg() << endl;
	} end_try;
    }

    // Define another descr. and add it to the first descr.
    TableDesc tdscr("TabSub",TableDesc::Scratch);
    tdscr.keywordSet().define ("key1", Int(0));
    ScalarColumnDesc<String> colaDesc ("cola");
    colaDesc.setMaxLength (32);
    tdscr.addColumn (colaDesc);
    td.addColumn (SubTableDesc("colsub","colsub comment",tdscr));
    tdscr.keywordSet().define ("key2", Int(0));
    tdscr.show();
    cout<<endl;
    TableDesc tda(td,"OtherName","O2",TableDesc::Scratch);   // copy the descr.
    tda.show();
    tda.add (tdscr, False);
    tda.keywordSet().removeField ("ra");
    tda.removeColumn ("sub2");
    ColumnDesc& cd = tda.rwColumnDesc("ac");
    cd.keywordSet().removeField ("scale");
    td.show();
    tda.show();

    if (doExcp) {
	try {
	    cout << "Tryerr update" << endl;
	    TableDesc td1("",TableDesc::Update);
	} catch (AipsError x) {
	    cout << x.getMesg() << endl;
	} end_try;
    }
}

// Do some more erronous constructions.
void c (Bool doExcp) {
    // The next 2 statements are outcommented, because they result
    // in a bus error with the g++ compiler for reasons not understood.
    // The error occurs at the very end of the program.
//#//    TableDesc td("tTableDesc_tmp");
//#//    td.show();
    TableDesc* td1 = 0;
    TableDesc* td2 = 0;
    TableDesc* td3 = 0;

    if (doExcp) {
	try {
	    cout << "Tryerr update not exist" << endl;
	    td1 = new TableDesc("tTableDescXX_tmp", TableDesc::Update);
	} catch (AipsError x) {
	    cout << x.getMesg() << endl;
	    td1 = 0;
	} end_try;

	try {
	    cout << "Tryerr old not exist" << endl;
	    td2 = new TableDesc("tTableDescXX_tmp", TableDesc::Old);
	} catch (AipsError x) {
	    cout << x.getMesg() << endl;
	    td2 = 0;
	} end_try;

	try {
	    cout << "Tryerr newnoreplace" << endl;
	    td3 = new TableDesc("tTableDesc_tmp", TableDesc::NewNoReplace);
	} catch (AipsError x) {
	    cout << x.getMesg() << endl;
	    td3 = 0;
	} end_try;
    }

    delete td1;
    delete td2;
    delete td3;
    cout << endl;
}

void d (Bool)
{
    // Create a new description.
    TableDesc td("tTableDesc_tmp1", TableDesc::New);

    // Add an Int column.
    ColumnDesc c1 = td.addColumn(ScalarColumnDesc<Int>("colint","comment"));

    // Define a keyword colint_key1 (=10) for that column.
    c1.keywordSet().define ("colint_key1", Int(10));

    // It can also be done the other way around.
    ScalarColumnDesc<Int> colint("colint2","comment2");
    colint.keywordSet().define ("colint_key1", Int(20));
    td.addColumn(colint);

    // Add a third column.
    c1 = td.addColumn (ScalarColumnDesc<Int> ("colint3","comment"));

    // Extend the comment.
    td.rwColumnDesc("colint").comment() += " addition";

    // Define a keyword for the table.
    td.keywordSet().define ("tab_key1", "this is a string");

    // Register engines.
    DataManager::registerCtor ("c1_engine",0);
    DataManager::registerCtor ("c2_engine",0);

    // Define a virtual column.
    td.addColumn(ScalarColumnDesc<Int>("c1", "c1-comment",
				       "c1_engine", ""));

    // Show name and comment of all column descriptions.
    for (uInt jj=0; jj<td.ncolumn(); jj++) {
	c1 = td[jj];
	cout << c1.name() << " " << c1.comment() << endl;
    }
    TableDesc tda(td, "namex", "v2.0.1", TableDesc::Scratch);
    tda.show();
}

void e (Bool)
{
    TableDesc td("tTableDesc_tmp1");
    td.show();
}


main (int argc)
{
    try {
	d (ToBool (argc<2));
	e (ToBool (argc<2));
	a (ToBool (argc<2));
	b (ToBool (argc<2));
	c (ToBool (argc<2));
    } catch (AipsError x) {
	cout << "Caught an exception: " << x.getMesg() << endl;
	return 1;
    } end_try;
    cout << "OK" << endl;
    return 0;                           // exit with success status
}
