//# Aipsrc.h: Class to read the aipsrc general resource files
//# Copyright (C) 1995,1996,1997
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

#if !defined(AIPS_AIPSRC_H)
#define AIPS_AIPSRC_H

#if defined(_AIX)
#pragma implementation ("Aipsrc.cc")
#endif

#include <aips/aips.h>
#include <aips/Utilities/String.h>
#include <aips/Containers/Block.h>

//# Forward declarations
template <class T> class Vector;
template <class T> class AipsrcValue;
template <class T> class AipsrcVector;
class Aipsrc;

//# Typedefs
typedef class AipsrcValue<Double> AipsrcDouble;
typedef class AipsrcValue<Int> AipsrcInt;
typedef class AipsrcValue<Bool> AipsrcBool;
typedef class Aipsrc AipsrcString;
typedef class AipsrcVector<Double> AipsrcVDouble;
typedef class AipsrcVector<Int> AipsrcVInt;
typedef class AipsrcVector<Bool> AipsrcVBool;
typedef class AipsrcVector<String> AipsrcVString;

// <summary> Class to read the aipsrc general resource files </summary>

// <use visibility=export>

// <reviewed reviewer="wyoung" date="1996/11/25" tests="tAipsrc" demos="">
// </reviewed>

// <prerequisite>
//  <li> None
// </prerequisite>
//
// <etymology>
// A class for getting values from the aipsrc files
// </etymology>
//
// <synopsis>
// The static Aipsrc class can get information from the aipsrc resource files.
// It has the same functionality as getrc (in use for aips++ scripts).<br>
// In addition it acts as a central clearing house between system and
// software.<br>
// The format of a line in a resource file is:
// <srcblock>
//	# Line starting with an # in column 1 is a comment (as is an empty line)
//	keyword:   value
//	keyword:   value
// </srcblock>
// The keyword (starting at first non-blank) 
// consists in general of keyword fields separated by periods:  
//<srcblock>
//	printer.ps.page
//	measures.precession.d_interval
// 	measures.nutation.d_interval
// </srcblock>
// and, by preference, in lower case (but
// search is case sensitive) with an <src>_</src> as word-parts separator. <br>
// The keyword and value are separated by a <src>:</src>. The value is the string
// between the first non-whitespace character after the separator and the end of
// the line. Interpretation of the string is in general the program's 
// responsibility, but special <src>find()</src> calls (see below) exists to 
// aid.<br>
// Any part of the keyword string can be replaced by a wildcard <src>*</src>
// to indicate all values with that structure (e.g.
// <src>*.d_interval</src> would indicate in the example above both the
// precession and the nutation <src>d_interval</src>.<br>
// A match between a keyword to be found and a keyword in the resource files
// will be the first match (taking wildcards into account) encountered in the
// search through the resource files. The resource files searched are (in the
// given order):
// <srcblock>
//   ~/.aipsrc
//   $AIPSROOT/.aipsrc
//   $AIPSHOST/aipsrc
//   $AIPSSITE/aipsrc
//   $AIPSARCH/aipsrc
// </srcblock> 
// It is not an error for any of the aipsrc files to be absent or empty.
// However, it is an error if either <em>HOME</em> or <em>AIPSPATH</em> has
// not been set: an exception will occur.<br>
// The basic interaction with the class is with the static keyword match function
// <srcblock>Bool Aipsrc::find(String &result, const String &keyword)
// </srcblock>
// A set of 
// <srcblock>Bool AipsrcValue::find(Type &result, const String &keyword, ...)
// </srcblock>
// are available to interpret the string value found.
// (see <linkto class="AipsrcValue">AipsrcValue</linkto>).<br>
// All the <src>find</src>
// functions have the ability to set a default if there is no match,
// while also unit conversion is possible.<br>
// The Bool return indicates if the keyword was found, and, in the case of the
// interpretative finds, if an 'important' format error was found (e.g.
// '+12a' will be accepted as a Double, with a result of '12', since the
// standard double conversion in <src>>></src> will produce this result.) 
// <note role=caution> The search keyword (unlike the file keyword) has no
// wildcards. The real name should, of course, be looked for.</note>
// To aid in other places, the following (static) methods are available
// to get the requested information (derived from <src>HOME</src> and
// <src>AIPSPATH</src>, computer system information and/or aipsrc keywords):
//  <ul>
//   <li> const String &Aipsrc::aipsRoot()
//   <li> const String &Aipsrc::aipsArch()
//   <li> const String &Aipsrc::aipsSite()
//   <li> const String &Aipsrc::aipsHost()
//   <li> const String &Aipsrc::aipsHome() -- <src>~/aips++</src>
//  </ul>
// Other, numeric, system information can be found in
// <linkto class=AipsrcData>AipsrcData</linkt>.<br>
//
// Given an AIPSPATH of 
// <srcblock>/epp/aips++ sun4sol_gnu epping norma</srcblock>
// aipsSite will return
// <srcblock>/epp/aips++/sun4sol_gnu/epping</srcblock>.
//
// The basic find above reacts with the aipsrc files available. If regular 
// access is necessary (e.g. a lot of routines have to check independently a
// certain integration time limit, keywords can be <em>registered</em> to
// enable:
// <ul>
//   <li> fast access with integer code, rather than string
//   <li> ability to set values from programs if no aipsrc information given
//		(a dynamic default)
//   <li> (future) option to update the <src>$HOME/.aipsrc</src> keyword list 
// </ul>
// <note role=tip> The registered value is never equal to zero, hence a zero
// value can be used to chcek if registration done. Also, registering the
// same keyword twice is safe, and will produce the same value.</note>
// </synopsis>
//
// <example>
// <srcblock>
//  String printerPage;		// result of keyword find
//  if(!Aipsrc::find(printerPage, "printer.ps.page")) {	// look for keyword match
//    printerPage = "notSet";
//  };
// </srcblock>
// A more convenient way of accomplishing the same result is:
// <srcblock>
//    Aipsrc::find(printerPage, "printer.ps.page", "notSet");
// </srcblock>
// Here the final argument is the default to use if the keyword is not found
// at all.<br>
// If you often want to know, dynamically, the current 'printer.ps.page'
// value, you could do something like:
// <srcblock>
//	static uInt pp = Aipsrc::registerRC("printer.ps.page", "noSet");
//	String printerPage = get(pp);
// // Processing, and maybe somewhere else:
//	set(pp, "nowSet");
// // ...
//	printerPage = get(pp);
// </srcblock>
// </example>
//
// <motivation>
// Programs need a way to interact with the aipsrc files.
// </motivation>
//
// <thrown>
//    <li>AipsError if the environment variables HOME and/or AIPSPATH not set.
// </thrown>
//
// <todo asof="1997/08/07">
// </todo>

class Aipsrc {

public:
  //# Constructors

  //# Destructor

  //# Copy assignment

  //# Member functions
  // <thrown>
  // <li> AipsError if HOME or AIPSPATH environment variable not set
  // </thrown> 
  // The <src>find()</src> functions will, given a keyword, return the value
  // with a matched keyword found in the files. If no match found the
  // function will be False. The <src>findNoHome()</src> emulates the <src>-i</src>
  // switch of getrc by bypassing the <src>~/.aipsrc</src> file.
  // <group>
  static Bool find(String &value, const String &keyword);
  static Bool findNoHome(String &value, const String &keyword);
  // </group>

  // These finds check a (possible) value of the keyword against a list
  // of coded values provided, and return an index into the list (N if not
  // found). Matching is minimax, case insensitive. Always better to use
  // the one with default. return is False if no keyword or no match.
  // <group>
  static Bool find(uInt &value, const String &keyword,
		   Int Nname, const String tname[]);
  static Bool find(uInt &value, const String &keyword,
		   const Vector<String> &tname);
  // </group>
  // This find usually saves you some lines of code, since you can supply the
  // default you want to use when no such keyword is defined.
  // If the return value is False, the keyword was not found and the default
  // was used.
  // <group>
  static Bool find(String &value, const String &keyword, 
		   const String &deflt);
  static Bool findNoHome(String &value, const String &keyword,
			 const String &deflt);
  static Bool find(uInt &value, const String &keyword,
		   Int Nname, const String tname[], const String &deflt);
  static Bool find(uInt &value, const String &keyword,
		   const Vector<String> &tname, const String &deflt);
  // </group>

  // Functions to register keywords for later use in get() and set(). The
  // returned value is the index for get() and set().
  // <group>
  static uInt registerRC(const String &keyword,
			 const String &deflt);
  static uInt registerRC(const String &keyword,
			 Int Nname, const String tname[], const String &deflt);
  static uInt registerRC(const String &keyword,
			 const Vector<String> &tname, const String &deflt);
  // </group>

  // Gets are like find, but using registered integers rather than names.
  // <group>
  static const String &get(uInt keyword);
  // get for code
  static const uInt &get(uInt &code, uInt keyword);
  // </group>

  // Sets allow registered values to be set
  // <group>
  static void set(uInt keyword, const String &deflt);
  static void set(uInt keyword,
		  Int Nname, const String tname[], const String &deflt);
  static void set(uInt keyword,
		  const Vector<String> &tname, const String &deflt);
 // </group>

  // Returns the appropiate AIPS++ or system variable values
  // <group>
  static const String &aipsRoot();
  static const String &aipsArch();
  static const String &aipsSite();
  static const String &aipsHost();
  // Returns: <src>~/aips++</src>
  static const String &aipsHome();
  // </group>
  
  // The <src>reRead()</src> function, will reinitialise the static maps and read the
  // aipsrc files again. It could be useful in some interactive or multi-processor 
  // circumstances.
  static void reRead();
  
  // The following functions return the full lists of available data. They could
  // be useful for debugging purposes.
  // <group>
  static const Block<String> &values();
  static const Block<String> &patterns();
  // </group>
  
  // The following <src>show()</src> function, useful for debugging, outputs 
  // all keyword/value pairs found
  static void show(ostream &oStream);
  // Prints all info on cout
  static void show();

protected:
  // Actual find function
  static Bool find(String &value, const String &keyword,
		   uInt start);
  // The registration function
  static uInt registerRC(const String &keyword, Block<String> &nlst);
  
private:
  //# Data
  // Indicate files read
  static Bool          doInit;
  // List of values belonging to keywords found
  static Block<String> keywordValue;
  // List of patterns deducted from names
  static Block<String> keywordPattern;
  // The start of the non-home values
  static uInt fileEnd;
  // AIPSROOT
  static String root;
  // AIPSARCH
  static String arch;
  // AIPSSITE
  static String site;
  // AIPSHOST
  static String host;
  // AIPSHOME
  static String home;
  // HOME
  static String uhome;
  // Indicate above filled
  static Bool filled;
  // String register list
  // <group>
  static Block<String> strlst;
  static Block<String> nstrlst;
  static Block<uInt> codlst;
  static Block<String> ncodlst;
  // </group>

  //# General member functions
  // Read in the aipsrc files, returning the number of lines found
  // <group>
  static uInt parse();
  static uInt parse(String &fileList);
  // </group>
  
  // Locate the right keyword in the static maps
  static Bool matchKeyword(uInt &where, const String &keyword,
			   uInt start);
  // Fill in root, arch, site, host and home, and return requested nam
  static const String &fillAips(const String &nam);
};

#endif


