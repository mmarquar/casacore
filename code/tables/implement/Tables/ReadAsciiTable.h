//# AsciiFileIO.h: Filling a table from an Ascii file
//# Copyright (C) 1993,1994,1995
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

#if !defined (AIPS_ASCIIFILEIO_H)
#define AIPS_ASCIIFILEIO_H

#include <aips/aips.h>

#if defined(_AIX)
#pragma implementation ("AsciiFileIO.cc")
#endif

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
// separated by whitespace (one line per table row).
// The following two lines define the columns in the table:
// <ol>
//   <li> The first line contains the names of the variables in each column.
//        These names may be enclosed in quotes (either single or double).     
//   <li> The second line contains the data types of each column.
//        Valid types are:
//      <ul>
//        <li>            R     for Real data
//        <li>            D     for Double Precision data
//        <li>            I     for Integer data
//        <li>            X     for Complex data (Real followed by Imaginary)
//        <li>            Z     for Complex data (Amplitude then Phase)
//        <li>            A     for ASCII data (must be enclosed in quotes
//                              if it contains one or more blanks)
//      </ul>
// </ol>
//
// There are two forms of the readAsciiTable function:
// <ol>
//  <li> The simplest form has one input file. The first two lines
//       define the columns as described above. The remaining lines contain
//       the data. For example:
//       <srcblock>
//        COLI   COLF   COLD       COLX        COLZ       COLS
//         I      R      D          X           Z          A
//        1      1.1    1.11       1.12 1.13   1.14 1.15  Str1
//        10     11     12         13   14     15   16    String17
//       </srcblock>
//       results in a table with 6 columns and 2 rows.
//  <li> The other form has two input files allowing the definition of
//       table keywords as well. The second input file contains the
//       column data. The first input file contains the keywords (if any)
//       and the column definitions.
//       The keywords in the first file, if there are any, must be enclosed
//       between a line that contains ".keywords" and a line that contains
//       ".endkeywords". Between these two lines each line should contain
//       the following:
//       <ul>
//        <li> The keyword name, e.g., ANYKEY
//        <li> The datatype of the keyword (cf. list of valid types above)
//        <li> The value or values for the keyword (the keyword may contain a
//             scalar or a vector of values).  e.g., 3.14159  21.78945
//      </ul>
//      For example:
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
//       COLI   COLF   COLD       COLX        COLZ       COLS
//        I      R      D          X           Z          A
//      </srcblock>
//      defines a table with 12 keywords (of which 6 contain vector values)
//      and 6 columns. The number of rows is determined by the number of
//      lines in the second input file.
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


// Create a table with name as given by tableName. The layout of the
// table is defined in the first 2 lines of the input file. The remaining
// lines in the input file contain the data.
//
// When the tableDescName is not blank, the table description will
// be stored in a table description file with the given name.
void readAsciiTable (const Char* filein, const Char* tableDescName,
		     const Char* tableName);

// This form reads TWO Ascii files. The first file may contain 
// keywords and their values as well as the two lines described above for
// the names and type of variables.  The second file is intended for data only.
//
// When the tableDescName is not blank, the table description will
// be stored in a table description file with the given name.
void readAsciiTable (const Char* headerFile, const Char* dataFfile, 
		     const Char* tableDescName, const Char* tablename);

// </group>


#endif
