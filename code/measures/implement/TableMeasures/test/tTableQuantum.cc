//# tTableQuantum.cc: test program for Quantum columns in TableMeasures module
//# Copyright (C) 1997,1998,1999
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

#include <trial/TableMeasures/TableQuantumDesc.h>
#include <trial/TableMeasures/ScalarQuantColumn.h>
#include <trial/TableMeasures/ArrayQuantColumn.h>
#include <aips/Quanta/Quantum.h>
#include <aips/Quanta/Unit.h>
#include <aips/Measures/MEpoch.h>
#include <aips/Mathematics/Complex.h>
#include <aips/Tables/ArrColDesc.h>
#include <aips/Tables/ScaColDesc.h>
#include <aips/Tables/SetupNewTab.h>
#include <aips/Tables/Table.h>
#include <aips/Tables/ArrayColumn.h>
#include <aips/Tables/ScalarColumn.h>
#include <aips/Tables/TableDesc.h>
#include <aips/Arrays/Array.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/ArrayIter.h>
#include <aips/Exceptions.h>
#include <iostream.h>


int main(int argc)
{
  try {
    Bool doExcep = ToBool(argc<2);

    cout << "Begin tTableQuantum.cc.\n";

    // Need a table to work with.
    TableDesc td("tTableQuantum_tmp", "1", TableDesc::New);
    td.comment() = "Created by tTableQuantum.cc";

    // This test uses two ScalarQuantum columns. They will be a
    // Quantum<Double> and a for Quantum<Complex> columns.  The Quantum<Double>
    // will have static "deg" units but the units for the Quantum<Complex>
    // column will be variable.  This requires an additional String column for
    // storing the units.
    ScalarColumnDesc<Double> scdQD("ScaQuantDouble",
	"A scalar column of Quantum<Double> with units 'deg'.");
    ScalarColumnDesc<Complex> scdQC("ScaQuantComplex",
	"A scalar column Quantum<Complex> with variable units.");
    ScalarColumnDesc<String> scdStr("varUnitsColumn",
	"Units columns for column ScaQuantComplex");

    // Also create ArrayQuantum columns for the test
    ArrayColumnDesc<Double> acdQD("ArrQuantDouble",
	"A Quantum<double> array column");
    ArrayColumnDesc<Double> acdQD3("ArrQuantDoubleNonVar",
	"A Quantum<double> array column");
    ArrayColumnDesc<Double> acdQD2("ArrQuantScaUnits",
	"A Quantum<double> array column");
    ArrayColumnDesc<String> acdStr("varArrUnitsColumn",
	"String column for array of units");
    ScalarColumnDesc<String> ascdStr("varArrScaUnitsColumn",
	"Scalar string column for variable units per row");

    ArrayColumnDesc<Double> bogusCol("BogusQuantCol",
	"an array column but won't be made a quantum column");

    // These must be added to the table descriptor
    cout << "Adding column descriptors to the table...\n";
    td.addColumn(scdQD);
    td.addColumn(scdQC);
    td.addColumn(acdQD);
    td.addColumn(acdQD2);
    td.addColumn(acdQD3);
    td.addColumn(scdStr);
    td.addColumn(acdStr);
    td.addColumn(ascdStr);
    td.addColumn(bogusCol);

    // Now create the Table Quantum Descriptors.  Three are used below but
    // a couple of dummy objects are created to test assignment and the copy
    // constructor.  The object we finally want is tqdSQD.
    TableQuantumDesc tqddummy(td, "ScaQuantDouble", Unit("deg"));
    // test copy constructor
    TableQuantumDesc tqddummy2 = tqddummy;
    // test empty unit
    TableQuantumDesc tqdSQD(td, "ScaQuantDouble");
    // test assignment
    tqdSQD = tqddummy2;

    TableQuantumDesc tqdSQC(td, "ScaQuantComplex", "varUnitsColumn");
    TableQuantumDesc tqdAQC(td, "ArrQuantDouble", "varArrUnitsColumn");
    TableQuantumDesc tqdAQC2(td, "ArrQuantScaUnits", "varArrScaUnitsColumn");
    TableQuantumDesc tqdAQC3(td, "ArrQuantDoubleNonVar", Unit("MHz"));

    // test the exceptions
    if (doExcep) {
      cout << "Testing TableQuantumDesc constructor exceptions...\n";
      try {
	// no such column
	TableQuantumDesc taexcep(td, "SillyName");
      } catch (AipsError x) {
	cout << "A no such column message should follow\n";
	cout << x.getMesg() << endl;
      } end_try;

      try {
	// variable unit's column doesn't exist.
	TableQuantumDesc taexcep(td, "ScaQuantComplex", "SillyName");
      } catch (AipsError x) {
	cout << "A no such unit's column message should follow\n";
	cout << x.getMesg() << endl;
      } end_try;

      try {
	// The vaiable unit's column exists but the units type isn't String
	ScalarColumnDesc<Int> eucol("testvarcolumn",
		  "variable units column with incorrect type");
	td.addColumn(eucol);
	TableQuantumDesc taexcep(td, "ScaQuantComplex", "testvarcolumn");
      } catch (AipsError x) {
	cout << "A message about an incorrect variable unit's type...\n";
	cout << x.getMesg() << endl;
      } end_try;
    }
    // ...and make them persistent.
    tqdSQD.write(td);
    tqdSQC.write(td);
    tqdAQC.write(td);
    tqdAQC2.write(td);
    tqdAQC3.write(td);

    cout << "Column's name is: " + tqdSQD.columnName() << endl;
    if (tqdSQD.isUnitVariable()) {
      cout << "Quantum column " + tqdSQD.columnName()
	   << " has variable units.\n";
      cout << "\tIts units are stored in String column '"
	                         + tqdSQD.unitColumnName() << "' \n";
    }
    cout << "Column's name is: " + tqdSQC.columnName() << endl;

    // create a table with 5 rows.
    SetupNewTable newtab("tTableQuantum_tmp.tab", td, Table::New);
    Table qtab(newtab, 5);

    {
      // Play with a null object first
      cout << "Creating a null ScaQuantumCol()\n";
      ScalarQuantColumn<Double> sq1Col;
      ScalarQuantColumn<Double> sqCol(sq1Col);

      cout << "Check if isNull and then throwIfNull\n";
      if (sqCol.isNull()) {
	if (doExcep) {
	  // test isnull exception
	  try {
	    sqCol.throwIfNull();
	  } catch (AipsError x) {
	    cout << "Catch an AipsError. Column is null...\n";
	    cout << x.getMesg() << endl;
	  } end_try;
	}
	cout << "Object says it is null...attach a column\n";
	sqCol.attach(qtab, "ScaQuantDouble");
	if (sqCol.isNull()) {
	  cout << "Apparantly still null...this isn't correct!\n";
	} else {
	  cout << "No longer null...good\n";
	}
	sqCol.throwIfNull();
      }

      // This should be a quantum column with fixed units
      if (sqCol.isUnitVariable()) {
	cout << "Columns units: " << sqCol.getUnits() << endl;
      }

      // put some quanta into the columns.
      Quantum<Double> q;
      for (uInt i=0; i<qtab.nrow(); i++) {
	q.setValue(i * 3.12);
	q.setUnit ("rad");
	sqCol.put(i, q);
      }
      ScalarQuantColumn<Double> sq2Col(sqCol);
      sq2Col.throwIfNull();
    }
    {
      // Could also read values from sqCol but instead a ROScalarQuantCol
      // is created here to do that.
      // test attach member for this first
      ROScalarQuantColumn<Double> rosq1Col;
      ROScalarQuantColumn<Double> rosqCol(rosq1Col);
      if (rosqCol.isNull()) {
	rosqCol.attach(qtab, "ScaQuantDouble");
      }
      rosqCol.throwIfNull();
      cout << "Column's quantum units are: " << rosqCol.getUnits() << endl;
      uInt i;
      for (i=0; i<qtab.nrow(); i++) {
	cout << "Quantum " << i << ": " << rosqCol(i) << endl;
      }

      // get them again but convert them to arcmin
      Quantum<Double> q;
      for (i=0; i<qtab.nrow(); i++) {
	cout << "Quantum arcmin " << i << ": "
	     << rosqCol(i, Unit("arcmin")) << endl;
      }

      // get them again but convert them to arcsec.
      rosqCol.attach(qtab, "ScaQuantDouble", "arcsec");
      for (i=0; i<qtab.nrow(); i++) {
	cout << "Quantum arcsec " << i << ": " << rosqCol(i) << endl;
      }
      for (i=0; i<qtab.nrow(); i++) {
	cout << "Quantum arcmin " << i << ": "
	     << rosqCol(i, "arcmin") << endl;
      }
      ROScalarQuantColumn<Double> rosq2Col(rosqCol);
      rosq2Col.throwIfNull();
    }
    {
      // Store a column of complex quantums with variable units.
      ScalarQuantColumn<Complex> sqCol(qtab, "ScaQuantComplex");
      if (sqCol.isUnitVariable()) {
	cout << "The units for ScaQuantComplex are variable.\n";
	cout << "getUnits() should produce an empty string: "
	     << sqCol.getUnits() << endl;
      } else {
	cout << "The units for ScaQuantComplex are not variable.\n";
	cout << "This is an error.\n";
      }
      Quantum<Complex> q(Complex(4., 0.21), "deg");
      sqCol.put(0, q);
      cout << q.get("m/s") << endl;
      q.convert("ms");
      sqCol.put(1, q);
      cout << q.get("m/s") << endl;
      q.convert("g");
      sqCol.put(2, q);
      cout << q.get("m/s") << endl;
      q.convert("Jy");
      sqCol.put(3, q);
      cout << q.get("m/s") << endl;
      q.convert("GHz");
      sqCol.put(4, q);	
      cout << q.get("m/s") << endl;
    }
    {
      // Lets have a look at them
      ROScalarQuantColumn<Complex> rosqCol(qtab, "ScaQuantComplex");
      uInt i;
      for (i=0; i<qtab.nrow(); i++) {
	cout << "Complex quantum (var unit) " << i << ": " << rosqCol(i)
	     << endl;
      }
      Quantum<Complex> q(1., "m/s");
      for (i=0; i<qtab.nrow(); i++) {
	// this get the units converted to the units in q
	cout << "Complex quantum (var unit) " << i << ": "
	     << rosqCol(i, q) << endl;
      }
    }

    cout << "\nFinished test of Quantum scalar column...........\n";
    cout << "\nStart test of Quantum array column...........\n";

    // Fill an array with quanta.
    IPosition shape(2, 3, 2);
    Array<Quantum<Double> > quantArr(shape);
    Bool deleteIt;
    Quantum<Double>* q_p = quantArr.getStorage(deleteIt);
    q_p->setValue(1.41212);
    q_p->setUnit("GHz");
    q_p++;
    q_p->setValue(1.4921);
    q_p->setUnit("MHz");
    q_p++;
    q_p->setValue(1.4111);
    q_p->setUnit("kHz");
    q_p++;
    q_p->setValue(1.4003);
    q_p->setUnit("Hz");
    q_p++;
    q_p->setValue(1.22);
    q_p->setUnit("GHz");
    q_p++;
    q_p->setValue(1.090909);
    q_p->setUnit("Hz");
    quantArr.putStorage(q_p, deleteIt);

    {
      // Now for array columns.  This set up a Quant Array column with
      // variable units where the units vary per array element.
      ArrayQuantColumn<Double> tmpCol;
      if (doExcep) {
	try {
	  tmpCol.throwIfNull();
	} catch (AipsError x) {
	  cout << "Catch an AipsError. Array column is null...\n";
	  cout << x.getMesg() << endl;
	} end_try;

	// test attaching a bogus quantum column
	try {
	  // create with a real column but not a quantum column
	  ArrayQuantColumn<Double> testCol(qtab, "BogusQuantCol");
	} catch (AipsError x) {
	  cout << "Exception should report not a quantum column...\n";
	  cout << x.getMesg() << endl;
	} end_try;
      }
      if (tmpCol.isNull()) {
	cout << "Array Quantum Column is initially null.\n";
	tmpCol.attach(qtab, "ArrQuantDouble");
      }
      // cover copy constructor
      ArrayQuantColumn<Double> aqCol = tmpCol;
      aqCol.throwIfNull();
      if (aqCol.isUnitVariable()) {
	cout << "Array quantum column: units are variable.\n";
      } else {
	cout << "Array quantum column units: " << aqCol.getUnits() << endl;
      }

      // put the quantum array in the column (having variable units).
      aqCol.put(0, quantArr);
    }
    {
      ROArrayQuantColumn<Double> roaqCol(qtab, "ArrQuantDouble");

      // test array conformance error exception on get()
      if (doExcep) {
	try {
	  Array<Quantum<Double> > badShapeArr(IPosition(2,2));
	  roaqCol.get(badShapeArr, 0, False);
	} catch (AipsError x) {
	  cout << "The following line should be a ";
	  cout << "Table array conformance error exception.\n";
	  cout << x.getMesg() << endl;
	} end_try;
      }
      {
	// This should succeed.
	Array<Quantum<Double> > badShapeArr(IPosition(2,2));
	roaqCol.get(badShapeArr, 0, True);
	cout << badShapeArr << endl;
      }

      cout << roaqCol(0) << endl;
      cout << roaqCol(0, "Hz") << endl;

      ROArrayQuantColumn<Double> roaqCol1(qtab, "ArrQuantDouble", "kHz");
      cout << roaqCol1(0) << endl;
      cout << roaqCol1(0, "Hz") << endl;
    }
    {
      // A second ArrayQuantColumn with variable units but in this case
      // the units only vary once per row as opposed to per array element
      // per row as in the example above.  This can be done because the
      // TableQuantDesc for the row specified a ScalarColumn as the
      // units column.
      // Could just construct the column completely but test attach member
      ArrayQuantColumn<Double> aqCol;
      aqCol.attach(qtab, "ArrQuantScaUnits");
      if (aqCol.isUnitVariable()) {
	cout << "Array quantum column: units are variable.\n";
      } else {
	cout << "Array quantum column units: " << aqCol.getUnits() << endl;
      }

      // cover putting an empty array (which should be OK)
      Array<Quantum<Double> > emptyArr;
      aqCol.put(0, emptyArr);

      // Put the quantum array in the column
      // Use unit "kHz" for the 2nd row.
      aqCol.put(0, quantArr);
      quantArr(IPosition(2,0,0)).setUnit ("kHz");
      aqCol.put(1, quantArr);
    }
    {
      // another way of creating the object
      ROArrayQuantColumn<Double> roaqCol(qtab, "ArrQuantScaUnits");
      cout << roaqCol(0) << endl;
      cout << roaqCol(0, "Hz") << endl;
      cout << roaqCol(1) << endl;
      cout << roaqCol(1, "Hz") << endl;

      Quantum<Double> q(0.21, "Hz");
      cout << roaqCol(0, q);
    }
    {
      // These should complete the coverage of the class
      // contructor
      ArrayQuantColumn<Double> aqc(qtab, "ArrQuantDouble");
      // copy constructor
      ArrayQuantColumn<Double> aqc1 = aqc;
      // attach
      ArrayQuantColumn<Double> aqc2;
      aqc2.attach(qtab, "ArrQuantDouble");

      // non-variable units column
      ArrayQuantColumn<Double> aqc3(qtab, "ArrQuantDoubleNonVar");
      aqc3.put(0, quantArr);

      ROArrayQuantColumn<Double> aqc4(qtab, "ArrQuantDoubleNonVar");
      cout << aqc4.getUnits() << endl;
      cout << aqc4(0) << endl;
    }

  } catch (AipsError x) {
    cout << "Unexpected exception: " << x.getMesg() << endl;
    exit(1);
  } end_try;

  // Try it with a readonly table.
  try {
    Table qtab ("tTableQuantum_tmp.tab");
    // Could also read values from sqCol but instead a ROScalarQuantCol
    // is created here to do that.
    // test attach member for this first
    ROScalarQuantColumn<Double> rosqCol;
    if (rosqCol.isNull()) {
      rosqCol.attach(qtab, "ScaQuantDouble");
    }
    rosqCol.throwIfNull();
    cout << "Column's quantum units are: " << rosqCol.getUnits() << endl;
    uInt i;
    for (i=0; i<qtab.nrow(); i++) {
      cout << "Quantum " << i << ": " << rosqCol(i) << endl;
    }
  } catch (AipsError x) {
    cout << "Unexpected exception: " << x.getMesg() << endl;
    exit(1);
  } end_try;

  cout << "\nExecution of tTableQuantum.cc ended normally.\n";
  exit(0);
}
