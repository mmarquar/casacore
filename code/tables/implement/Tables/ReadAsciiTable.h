//# ReadAsciiTable.h: Filling a table from an Ascii file
//# Copyright (C) 1993,1994,1995,1999,2001
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

#if !defined (AIPS_READASCIITABLE_H)
#define AIPS_READASCIITABLE_H

//# Includes
#include <aips/aips.h>
#include <aips/Utilities/String.h>


// <summary>
// Filling a table from an Ascii file.
// </summary>

// <use visibility=export>

// <prerequisites>
//  <li> <linkto class="Table:description">Table</linkto>
// <prerequisites>

// <synopsis>
// Global functions to fill a table from an Ascii file.
//
// The table columns are filled from a file containing the data values
// separated by a separator (optionally followed by whitespace). The
// default separator is a comma. Non-given values default to 0, False, or
// blank string (depending on data type). A value is not given between 2
// consecutive separators or if less values are given than needed.
// One line per table row should be given.
// The following two header lines define the columns in the table:
// <ol>
//   <li> The first line contains the names of the variables in each column.
//        These names may be enclosed in double quotes.
//   <li> The second line contains the data types of each column.
//        Valid types are:
//      <ul>
//        <li>            S     for Short Integer data
//        <li>            I     for Integer data
//        <li>            R     for Real data
//        <li>            D     for Double Precision data
//        <li>            X     for Complex data (Real, Imaginary)
//        <li>            DX    for Double Precision Complex data (R,I)
//        <li>            Z     for Complex data (Amplitude, Phase)
//        <li>            DZ    for Double Precision Complex data (A,P)
//        <li>            A     for ASCII data (must be enclosed in double
//                              quotes if it contains one or more blanks)
//      </ul>
// </ol>
// If the <src>autoHeader</src> argument is True, the column definition
// lines should not be given. It recognizes the types from the first data
// line. It gives the names 'column0', etc. to the columns.
// It can recognize integer, double, and string types.
// <p>
// There are two forms of the readAsciiTable function:
// <ol>
//  <li> The simplest form has two input files.
//       The second input file contains the column data.
//       The first input file contains the keywords (if any)
//       and the column definitions.
//       The keywords in the first file, if there are any, must be enclosed
//       between a line that starts with ".keywords" and a line that starts
//       with ".endkeywords". To define column keywords, .keywords should be
//       followed by whitespace and the column name. 
//       Between these two lines each line should contain the following:
//       <ul>
//        <li> The keyword name, e.g., ANYKEY
//        <li> The datatype of the keyword (cf. list of valid types above)
//        <li> The value or values for the keyword (the keyword may contain a
//             scalar or a vector of values).  e.g., 3.14159  21.78945
//      </ul>
//      After the keywords definitions, the two column definition lines
//      should follow (unless <src>autoHeader=True</src> is given).
//      <br>For example:
//      <srcblock>
//       .keywords
//       KEYI  I  10
//       KEYIV I  11 12 13 14
//       KEYF  R  1.2
//       KEYFV R  -3.2 0 5.6
//       KEYD  D  1.23456789
//       KEYDV D  1 2 3 4 5 6 7 8 9
//       KEYX  X  -1.5 -3
//       KEYXC X  0 1 2 3 4 5 6 7 8 9
//       KEYZ  Z  -3  -1.5
//       KEYZV Z  0 0.1 0.2 0.3 0.4 0.5
//       KEYS  A  "1 2 3 4 5"
//       KEYSV A  " 1 2 " "AAA" BBB bbb CCc C "@#$%^&*()"
//       .endkeywords
//       .keywords  COLDX
//       IKEYS   A "coldx ikey"
//       DKEYS   A "coldx dkey"
//       .endkeywords
//       COLI   COLF   COLD       COLX        COLZ       COLS
//        I      R      D          X           Z          A
//      </srcblock>
//      defines a table with 12 table keywords (of which 6 contain vector
//      values), 2 keywords for column COLDX, and and 6 columns.
//      The number of rows is determined by the number of
//      lines in the second input file.
//  <li> The other form is to combine the two files in one file.
//       In that case the data lines must be preceeded by the optional
//       keyword and column definitions (without an intermediate blank line).
// </ol>
// </synopsis>

// <example>
// <srcblock>
//   readAsciiTable ("file.in", "", "table.test");
// </srcblock>
// creates a table with name <src>table.test</src> from the text file
// <src>file.in</src>. The text file could look like:
// <srcblock>
//  COLI   COLF   COLD       COLX        COLZ       COLS
//   I      R      D          X           Z          A
//  1      1.1    1.11       1.12 1.13   1.14 1.15  Str1
//  10     11     12         13   14     15   16    String17
// </srcblock>
// resulting in a table with 6 columns and 2 rows.
// </example>

// <group name=readAsciiTable>


// Create a table with name as given by tableName.
// If autoHeader==True, the format is automatically derived from the
// first lines. It can recognize integer, double, and String types.
// The columns will be named column1, column2, etc..
// If autoHeader==False, the layout of the table has to be defined in
// the first 2 lines of the input file. The remaining lines in the
// input file contain the data.
//
// When the tableDescName is not blank, the table description will
// be stored in a table description file with the given name.
// <br>It returns a string containing the format of the columns in
// the form COL1=R, COL2=D, ...
String readAsciiTable (const String& filein, const String& tableDescName,
		       const String& tableName, Bool autoHeader = False,
		       Char separator = ' ');

// This form reads TWO Ascii files. The first file may contain 
// keywords and their values as well as the two lines described above for
// the names and type of variables.  The second file is intended for data only.
//
// When the tableDescName is not blank, the table description will
// be stored in a table description file with the given name.
// <br>It returns a string containing the format of the columns in
// the form COL1=R, COL2=D, ...
// <group>
String readAsciiTable (const String& headerFile, const String& dataFile, 
		       const String& tableDescName, const String& tablename,
		       Char separator = ' ');
String readAsciiTable (const String& headerFile, const String& dataFile, 
		       const String& tableDescName, const char* tablename,
		       Char separator = ' ');
// </group>

// </group>


#endif
