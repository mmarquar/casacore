//# ReadAsciiTable.cc: Filling a table from an Ascii file
//# Copyright (C) 1993,1994,1995,1996,1997,1999,2000,2001
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

#include <aips/Tables/ReadAsciiTable.h>
#include <aips/Tables/TableDesc.h>
#include <aips/Tables/ScaColDesc.h>
#include <aips/Tables/Table.h>
#include <aips/Tables/TableRecord.h>
#include <aips/Tables/TableColumn.h>
#include <aips/Tables/SetupNewTab.h>
#include <aips/Arrays/Vector.h>
#include <aips/Utilities/String.h>
#include <aips/Utilities/Regex.h>
#include <aips/Exceptions/Error.h>

#include <aips/Logging/LogIO.h>
#include <aips/Logging/LogOrigin.h>

#include <aips/stdio.h>
#include <aips/string.h>
#include <aips/iostream.h>
#include <aips/fstream.h>             // needed for file IO
#include <aips/strstream.h>           // needed for internal IO



//# Helper function 
//# It gets the next value from a line and stores it in result.
//# It updates at and returns the length of the value retrieved.
//# Quotes around strings are removed
//# -1 is returned if no more values are found.
Int readAsciiTableGetNext (const Char* string, Int strlen, Char* result,
			   Int& at)
{
    Int i = 0;
    Bool found  = False;
    Bool quoted = False;
    Char ihave;

    if (string[at] == '\0') {
        return -1;
    }
    for (; at<strlen; at++) {
	ihave = string[at];
	if (ihave == '\'' || ihave == '\"') {
	    if (quoted) {
		quoted = False;
		continue;
	    }else{
		quoted = True;
		found  = True;
		continue;
	    }
	}
	if (ihave == '\0') {
	    result[i] = '\0';
	    return (found  ?  i : -1);
	}
	if (quoted) {
	    result[i++] = ihave;
	    continue;
	}
	if (ihave == '\t') {
	    ihave = ' ';
	}
	if (ihave == ' '  &&  found) {
	    result[i] = '\0';
	    return i;
	}
	if (ihave != ' ') {
	    found = True;
	}
	if (found) {
	    result[i++] = ihave; 
	}
    } 
    return -1;
}



void getTypesAsciiTable (const Char* in, Int leng,
			 Char* string1, Char* string2)
{
    Int at = 0;
    Int i = 0;
    //# When constructing str in the while loop (in the else branch),
    //# a compiler bug appeared on RH systems.
    //# Therefore assignment is used instead.
    String str;
    while (readAsciiTableGetNext (in, leng, string2, at) >= 0) {
        if (string2[0] == '\0') {
	    string1[0] = 'A';
	} else {
	    str = string2;
	    if (str.matches (RXint)) {
	        string1[0] = 'I';
	    } else if (str.matches (RXdouble)) {
	        string1[0] = 'D';
	    } else {
	        string1[0] = 'A';
	    }
	}
	string1[1] = ' ';
	string1 += 2;
	char name[16];
	i++;
	sprintf (name, " Column%i", i);
	strcpy (string2, name);
	string2 += strlen(name);
    }
    string1[0] = '\0';
    string2[0] = '\0';
}



//# Convert a string to a Bool
Bool toBoolAsciiTable (const String& str)
{
    if (str.length() == 0  ||  str == "0"  ||  str[0] == 'F'
    ||  str[0] == 'f'  || str[0] == 'N'  || str[0] == 'n') {
        return False;
    }
    return True;
}



//# Read a keyword set and add it to keysets.
void readAsciiTableHandleKeyset (Int lineSize, char* string1,
				 char* first, char* second,
				 TableRecord& keysets,
				 LogIO& logger,
				 const String& fileName,
				 ifstream& jFile,
				 Int& lineNumber)
{
  Float tempR; Short tempSH; Int tempI; Double tempD;
  Float temp1, temp2, temp3, temp4;
  Double temp1d, temp2d, temp3d, temp4d;
  TableRecord keyset;

  // Get the column name in case it is a column keywordset.
  String colName;
  Int atl = 0;
  readAsciiTableGetNext (string1, lineSize, first, atl); 
  Int d4 = readAsciiTableGetNext (string1, lineSize, second, atl); 
  if (d4 > 0) {
    colName = second;
  }
  while (True) {

// Read the next line(s)

    if (!jFile.getline(string1, lineSize)) {
      throw (AipsError ("No .endkeywords line in " + fileName));
    }
    lineNumber++;

// If we are at END of KEYWORDS read the next line to get NAMES OF COLUMNS
// or to get next keyword group.

    if (strncmp(string1, ".end", 4) == 0) {
      if (!jFile.getline(string1, lineSize)) {
	string1[0] = '\0';
      } else {
	lineNumber++;
      }
      break;
    }

// Read the first two fields of a KEYWORD line
    Int done3, done4, at3=0;
    done3 = readAsciiTableGetNext (string1, lineSize, first, at3); 
    done4 = readAsciiTableGetNext (string1, lineSize, second, at3); 
    if (done3<=0 || done4<=0) {
      throw (AipsError ("No keyword name and type in line " +
			String::toString(lineNumber)
			+ " in " + fileName));
    }
    String keyName = String(first);
    String keyType = String(second);
    keyType.upcase();
    if (keyset.isDefined (keyName)) {
      logger << LogIO::WARN <<
	"Keyword " << keyName << " skipped because defined twice in "
	     << fileName << LogIO::POST;
    } else {

// Count the number of values for this key

      Int savat3 = at3;
      Int nVals = 0;
      while (readAsciiTableGetNext (string1, lineSize,
				    first, at3) >= 0) {
	nVals++;
      }
      if (nVals == 0) {
	throw (AipsError ("No keyword value(s) in line " +
			  String::toString(lineNumber)
			  + " in " + fileName));
      }

// Read the keyword value(s).
      at3 = savat3;
      if (keyType == "S") {
	Vector<Short> vectShort(nVals); 
	for (Int i21=0; i21<nVals; i21++) {
	  done3 = readAsciiTableGetNext (string1, lineSize,
					 first, at3);
	  istrstream(first, done3) >> tempSH;
	  vectShort(i21) = tempSH;
	}
	if (nVals > 1) {
	  keyset.define (keyName, vectShort);
	} else {
	  keyset.define (keyName, vectShort(0));
	}
      } else if (keyType == "I") {
	Vector<Int> vectInt(nVals); 
	for (Int i21=0; i21<nVals; i21++) {
	  done3 = readAsciiTableGetNext (string1, lineSize,
					 first, at3);
	  istrstream(first, done3) >> tempI;
	  vectInt(i21) = tempI;
	}
	if (nVals > 1) {
	  keyset.define (keyName, vectInt);
	} else {
	  keyset.define (keyName, vectInt(0));
	}
      } else if (keyType == "R") {
	Vector<Float> vectFloat(nVals); 
	for (Int i20=0; i20<nVals; i20++) {
	  done3 = readAsciiTableGetNext (string1, lineSize,
					 first, at3);
	  istrstream(first, done3) >> tempR;
	  vectFloat(i20) = tempR;
	}
	if (nVals > 1) {
	  keyset.define (keyName, vectFloat);
	} else {
	  keyset.define (keyName, vectFloat(0));
	}
      } else if (keyType == "D") {
	Vector<Double> vectDbl(nVals); 
	for (Int i20=0; i20<nVals; i20++) {
	  done3 = readAsciiTableGetNext (string1, lineSize,
					 first, at3);
	  istrstream(first, done3) >> tempD;
	  vectDbl(i20) = tempD;
	}
	if (nVals > 1) {
	  keyset.define (keyName, vectDbl);
	} else {
	  keyset.define (keyName, vectDbl(0));
	}
      } else if (keyType == "A") {
	Vector<String> vectStr(nVals); 
	for (Int i20=0; i20<nVals; i20++) {
	  done3 = readAsciiTableGetNext (string1, lineSize,
					 first, at3);
	  vectStr(i20) = first;
	}
	if (nVals > 1) {
	  keyset.define (keyName, vectStr);
	} else {
	  keyset.define (keyName, vectStr(0));
	}
      } else if (keyType == "B") {
	Vector<Bool> vectStr(nVals); 
	for (Int i20=0; i20<nVals; i20++) {
	  done3 = readAsciiTableGetNext (string1, lineSize,
					 first, at3);
	  vectStr(i20) = toBoolAsciiTable (first);
	}
	if (nVals > 1) {
	  keyset.define (keyName, vectStr);
	} else {
	  keyset.define (keyName, vectStr(0));
	}
      } else if (keyType == "X") {
	if (nVals%2 != 0) {
	  throw (AipsError ("Complex keyword " + keyName +
			    " in " + fileName +
			    " must have even number of values"));
	}
	nVals /= 2;
	Vector<Complex> vectCX(nVals); 
	for (Int i20=0; i20<nVals; i20++) {
	  done3 = readAsciiTableGetNext (string1, lineSize,
					 first, at3);
	  done4 = readAsciiTableGetNext (string1, lineSize,
					 second, at3);
	  istrstream(first, done3) >> temp1;
	  istrstream(second, done4) >> temp2;
	  vectCX(i20) = Complex(temp1, temp2);
	}
	if (nVals > 1) {
	  keyset.define (keyName, vectCX);
	} else {
	  keyset.define (keyName, vectCX(0));
	}
      } else if (keyType == "DX") {
	if (nVals%2 != 0) {
	  throw (AipsError ("DComplex keyword " + keyName +
			    " in " + fileName +
			    " must have even number of values"));
	}
	nVals /= 2;
	Vector<DComplex> vectCX(nVals); 
	for (Int i20=0; i20<nVals; i20++) {
	  done3 = readAsciiTableGetNext (string1, lineSize,
					 first, at3);
	  done4 = readAsciiTableGetNext (string1, lineSize,
					 second, at3);
	  istrstream(first, done3) >> temp1d;
	  istrstream(second, done4) >> temp2d;
	  vectCX(i20) = DComplex(temp1d, temp2d);
	}
	if (nVals > 1) {
	  keyset.define (keyName, vectCX);
	} else {
	  keyset.define (keyName, vectCX(0));
	}
      } else if (keyType == "Z") {
	if (nVals%2 != 0) {
	  throw (AipsError ("Complex keyword " + keyName +
			    " in " + fileName +
			    " must have even number of values"));
	}
	nVals /= 2;
	Vector<Complex> vectCX(nVals); 
	for (Int i20=0; i20<nVals; i20++) {
	  done3 = readAsciiTableGetNext (string1, lineSize,
					 first, at3);
	  done4 = readAsciiTableGetNext (string1, lineSize,
					 second, at3);
	  istrstream(first, done3) >> temp1;
	  istrstream(second, done4) >> temp2;
	  temp2 *= 3.14159265/180.0; 
	  temp3 = temp1 * cos(temp2);
	  temp4 = temp1 * sin(temp2);
	  vectCX(i20) = Complex(temp3, temp4);
	}
	if (nVals > 1) {
	  keyset.define (keyName, vectCX);
	} else {
	  keyset.define (keyName, vectCX(0));
	}
      } else if (keyType == "DZ") {
	if (nVals%2 != 0) {
	  throw (AipsError ("DComplex keyword " + keyName +
			    " in " + fileName +
			    " must have even number of values"));
	}
	nVals /= 2;
	Vector<DComplex> vectCX(nVals); 
	for (Int i20=0; i20<nVals; i20++) {
	  done3 = readAsciiTableGetNext (string1, lineSize,
					 first, at3);
	  done4 = readAsciiTableGetNext (string1, lineSize,
					 second, at3);
	  istrstream(first, done3) >> temp1d;
	  istrstream(second, done4) >> temp2d;
	  temp2d *= 3.14159265/180.0; 
	  temp3d = temp1d * cos(temp2d);
	  temp4d = temp1d * sin(temp2d);
	  vectCX(i20) = DComplex(temp3d, temp4d);
	}
	if (nVals > 1) {
	  keyset.define (keyName, vectCX);
	} else {
	  keyset.define (keyName, vectCX(0));
	}
      }
    }
  }
  if (keysets.isDefined (colName)) {
    logger << LogIO::WARN
	   << "Keywordset of column " << colName
	   << " skipped because defined twice in "
	   << fileName << LogIO::POST;
  } else {
    keysets.defineRecord (colName, keyset);
  }
}



String doReadAsciiTable (const String& headerfile, const String& filein, 
			 const String& tableproto, const String& tablename,
			 Bool autoHeader)
{
    const Int   lineSize = 32768;
          char  string1[lineSize], string2[lineSize], stringsav[lineSize];
          char  first[lineSize], second[lineSize];
    const Int   arraySize = 1000;
          String  nameOfColumn[arraySize];
          String  typeOfColumn[arraySize];
          String  keyName;

	  LogIO logger(LogOrigin("readAsciiTable", WHERE));

// Determine if header and data are in one file.
    Bool oneFile = (headerfile == filein);

// PART ONE
// Define the TABLE description, i.e. define its columns.
// Create the description as scratch if no name is given.

    TableDesc td (tableproto,
		  (tableproto.empty() ? TableDesc::Scratch : TableDesc::New));

    ifstream jFile;
    jFile.open(headerfile, ios::in);
    if (! jFile) {
        throw (AipsError ("Cannot open header file " + headerfile));
    }

// Read the first line. It will be KEYWORDS or NAMES OF COLUMNS

    if (!jFile.getline(string1, lineSize)) {
	throw (AipsError ("Cannot read first header line of " + headerfile));
    }

// If the first line shows that we have KEYWORDS read until the
// end of keywords while assembling the keywords.

    Int lineNumber = 1;
    TableRecord keysets;
    while (strncmp(string1, ".key", 4) == 0) {
        readAsciiTableHandleKeyset (lineSize, string1, first, second,
				    keysets, logger,
				    headerfile, jFile, lineNumber);
    }

// Okay, all keywords have been read.
// string1 contains the next line (if any).
// Read the column definition lines from header file (if needed).
// Determine the types if autoheader is given.

// Previous line should be NAMES OF COLUMNS; now get TYPE OF COLUMNS line
    if (!autoHeader) {
        if (string1[0] == '\0') {
	    throw (AipsError("No COLUMN NAMES line in " + headerfile));
	}
        if (!jFile.getline(string2, lineSize)) {
	    throw (AipsError("No COLUMN TYPES line in " + headerfile));
	}
    }

// Now open the actual data file (if not the same as header file).
// Read the first line if auto header.

    if (!oneFile) {
        jFile.close();
	jFile.open(filein, ios::in);
	if (autoHeader) {
	    if (!jFile.getline(string1, lineSize)) {
	        string1[0] = '\0';
	    }
	}
    }

    // Process the auto header.
    // Save string, because it'll be overwritten.
    stringsav[0] = '\0';
    if (autoHeader) {
        strcpy (stringsav, string1);
        getTypesAsciiTable (string1, lineSize, string2, first);
	strcpy (string1, first);
    }

// Break up the NAME OF COLUMNS line and the TYPE OF COLUMNS line
// Place the results in the two arrays.
// Also put in in a single string to be returned to the caller.

    String formStr;
    Int done1 = 0, done2 = 0, at1 = 0, at2 = 0, nrcol = 0;
    while (done1 >= 0) {
	done1 = readAsciiTableGetNext (string1, lineSize, first, at1);
	done2 = readAsciiTableGetNext (string2, lineSize, second, at2);
	if (done1>0 && done2>0) {
	    nameOfColumn[nrcol] = String(first);
	    typeOfColumn[nrcol] = String(second);
	    typeOfColumn[nrcol].upcase();
	    if (! formStr.empty()) {
	        formStr += ", ";
	    }
	    formStr += nameOfColumn[nrcol] + "=" + typeOfColumn[nrcol];
	    nrcol++;
	} else if (done1>=0 || done2>=0) {
	    throw (AipsError ("Mismatching COLUMN NAMES AND TYPES lines in "
			      + headerfile));
	}
    }

// Create the TABLE Columns for these variables

    for (Int i5=0; i5<nrcol; i5++) {
	if (typeOfColumn[i5] == "S")
	    td.addColumn (ScalarColumnDesc<Short> (nameOfColumn[i5]));
	if (typeOfColumn[i5] == "I")
	    td.addColumn (ScalarColumnDesc<Int> (nameOfColumn[i5]));
	if (typeOfColumn[i5] == "R")
	    td.addColumn (ScalarColumnDesc<Float> (nameOfColumn[i5]));
	if (typeOfColumn[i5] == "D")
	    td.addColumn (ScalarColumnDesc<Double> (nameOfColumn[i5]));
	if (typeOfColumn[i5] == "X")
	    td.addColumn (ScalarColumnDesc<Complex> (nameOfColumn[i5]));
	if (typeOfColumn[i5] == "Z")
	    td.addColumn (ScalarColumnDesc<Complex> (nameOfColumn[i5]));
	if (typeOfColumn[i5] == "DX")
	    td.addColumn (ScalarColumnDesc<DComplex> (nameOfColumn[i5]));
	if (typeOfColumn[i5] == "DZ")
	    td.addColumn (ScalarColumnDesc<DComplex> (nameOfColumn[i5]));
	if (typeOfColumn[i5] == "A")
	    td.addColumn (ScalarColumnDesc<String> (nameOfColumn[i5]));
	if (typeOfColumn[i5] == "B")
	    td.addColumn (ScalarColumnDesc<Bool> (nameOfColumn[i5]));
    }



// PART TWO
// The TableDesc has now been created.  Start filling in the Table.
// Use the default (AipsIO) storage manager.

    SetupNewTable newtab(tablename, td, Table::New);
    Table tab(newtab);
    Float tempR; Short tempSH; Int tempI; Double tempD;
    Float temp1, temp2, temp3, temp4;
    Double temp1d, temp2d, temp3d, temp4d;

// Write keywordsets.

    for (uInt i=0; i<keysets.nfields(); i++) {
        String colnm = keysets.name(i);
        if (colnm.empty()) {
	    tab.rwKeywordSet() = keysets.subRecord (i);
	} else {
	    if (!tab.tableDesc().isColumn (colnm)) {
	        logger << LogIO::WARN
		       << "Keywordset of column " << colnm
		       << " skipped because column is not defined in "
		       << headerfile << LogIO::POST;
	    } else {
	        TableColumn tabcol (tab, colnm);
		tabcol.rwKeywordSet() = keysets.subRecord (i);
	    }
	}
    }

    TableColumn* tabcol = new TableColumn[nrcol];
    if (tabcol == 0) {
	throw (AllocError ("readAsciiTable", nrcol));
    }
    for (Int i=0; i<nrcol; i++) {
	tabcol[i].reference (TableColumn (tab, nameOfColumn[i]));
    }
    uInt rownr = 0;

// OK, Now we have real data
// stringsav may contain the first data line.

    Bool cont = True;
    if (stringsav[0] == '\0') {
        cont = jFile.getline(string1, lineSize);
    } else {
        strcpy (string1, stringsav);
    }
    while (cont) {
	at1 = 0; 
	tab.addRow();
	for (Int i6=0; i6<nrcol; i6++) {
	    done1 = readAsciiTableGetNext (string1, lineSize, first, at1);
	    done2 = 1;
	    if (typeOfColumn[i6] == "X"  ||  typeOfColumn[i6] == "DX"
	    ||  typeOfColumn[i6] == "Z"  ||  typeOfColumn[i6] == "DZ") {
		done2 = readAsciiTableGetNext (string1, lineSize, second, at1);
	    }
	    if (done1<0  || done2<0) {
	        tab.flush();
	        throw (AipsError ("Confused about input in " + filein
				  + "\nLast text seen: " + string1
				  + "\nThis occurred at about row "
				  + String::toString(tab.nrow())
				  + " (excluding headers). "
				  + "The rest of the file is ignored."));
	    }
	    if (typeOfColumn[i6] == "S") {
		istrstream(first, done1) >> tempSH;
		tabcol[i6].putScalar (rownr, tempSH);
	    }
	    if (typeOfColumn[i6] == "I") {
		istrstream(first, done1) >> tempI;
		tabcol[i6].putScalar (rownr, tempI);
	    }
	    if (typeOfColumn[i6] == "R") {
		istrstream(first, done1) >> tempR;
		tabcol[i6].putScalar (rownr, tempR);
	    }
	    if (typeOfColumn[i6] == "D") {
		istrstream(first, done1) >> tempD;
		tabcol[i6].putScalar (rownr, tempD);
	    }
	    if (typeOfColumn[i6] == "X") {
		istrstream(first, done1) >> temp1;
		istrstream(second, done2) >> temp2;
		tabcol[i6].putScalar (rownr, Complex(temp1, temp2));
	    }
	    if (typeOfColumn[i6] == "DX") {
		istrstream(first, done1) >> temp1d;
		istrstream(second, done2) >> temp2d;
		tabcol[i6].putScalar (rownr, DComplex(temp1d, temp2d));
	    }
	    if (typeOfColumn[i6] == "Z") {
		istrstream(first, done1) >> temp1;
		istrstream(second, done2) >> temp2;
		temp2 *= 3.14159265/180.0; 
		temp3 = temp1 * cos(temp2);
		temp4 = temp1 * sin(temp2);
		tabcol[i6].putScalar (rownr, Complex(temp3, temp4));
	    }
	    if (typeOfColumn[i6] == "DZ") {
		istrstream(first, done1) >> temp1d;
		istrstream(second, done2) >> temp2d;
		temp2d *= 3.14159265/180.0; 
		temp3d = temp1d * cos(temp2d);
		temp4d = temp1d * sin(temp2d);
		tabcol[i6].putScalar (rownr, DComplex(temp3d, temp4d));
	    }
	    if (typeOfColumn[i6] == "A") {
		tabcol[i6].putScalar (rownr, String(first));
	    }
	    if (typeOfColumn[i6] == "B") {
		tabcol[i6].putScalar (rownr, toBoolAsciiTable(first));
	    }
	}
	rownr++;
        cont = jFile.getline(string1, lineSize);
    }

    delete [] tabcol;
    jFile.close();
    return formStr;
}


String readAsciiTable (const String& headerfile, const String& filein, 
		       const String& tableproto, const char* tablename)
{
  return doReadAsciiTable (headerfile, filein, tableproto, String(tablename),
			   False);
}

String readAsciiTable (const String& headerfile, const String& filein, 
		       const String& tableproto, const String& tablename)
{
  return doReadAsciiTable (headerfile, filein, tableproto, tablename, False);
}

String readAsciiTable (const String& filein, const String& tableproto,
		       const String& tablename, Bool autoHeader)
{
  return doReadAsciiTable (filein, filein, tableproto, tablename, autoHeader);
}
