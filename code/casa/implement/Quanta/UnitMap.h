//# UnitMap.h: defines the UnitMap class containing standard unit definitions
//# Copyright (C) 1994,1995,1996,1997,1998,1999
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

#if !defined(AIPS_UNITMAP_H)
#define AIPS_UNITMAP_H


//# Includes
#include <aips/aips.h>
#include <aips/Mathematics/Constants.h>
#include <aips/Containers/SimOrdMap.h>
#include <aips/Utilities/String.h>
#include <aips/Quanta/UnitDim.h>
#include <aips/Quanta/UnitVal.h>
#include <aips/Quanta/UnitName.h>

//# Forward Declarations

//* Constants
// <note role=warning>
// SUN compiler does not accept non-simple default arguments
// </note>
// IAU definition of Gaussian grav. constant for calculating IAU units
const Double IAU_k=0.01720209895;
// Number of FITS units recognised
const uInt N_FITS = 19;

// <summary>
// contains all simple known physical units
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="tUnit">
//
// <prerequisite>
// You should have at least a preliminary understanding of these classes:
//   <li> <linkto class=Unit>Unit</linkto>
// </prerequisite>
//
// <etymology>
// Based on Units and the Aips++ container classes called 'Map'
// </etymology>
//
// <synopsis> 
// Physical units are strings consisting of one or more names of known
// basic units, separated by '.' or ' ' (for multiplication) or '/' (for
// division). Each name can optionally be preceded by a standard decimal 
// prefix, and/or followed by an (optionally signed) exponent.
// Example:
//	km/s/(Mpc.s)2  is identical to km.s-1.Mpc-2.s-2
//
// See the <linkto class="Unit">Unit</linkto> class for more details.
//
// The UnitMap class contains the known standard basic units, and any
// other basic unit defined by the user of the Unit related classes.
// The known units are divided into 5 different groups:
// <ol>
//   <li> Defining units: 	m, kg, s, A, K, cd, mol, rad, sr
//   <li> SI units:		including a.o. Jy, AU etc)
//   <li> Customary units:	e.g. lb, hp, ly etc
//   <li> User defined units:	defined by user (e.g. beam, KPH, KM)
//   <li> Cached units:	cached unit strings for speed in operations
// </ol>
// The full list of known units can be viewed by running the tUnit test
// program.
//
// Information about the contents of the unit maps can be obtained by
// the Bool functions (False if not present):
// <ul>
//   <li> UnitMap::getPref("string", UnitName &)	prefix
//   <li> UnitMap::getUnit("string", UnitName &)	search user,
//		customary, SI (in that order)
//   <li> UnitMap::getCache("string", UnitVal &)	search cache
// </ul>
//
// The standard units can be viewed by the following commands, which
// output to cout:
// <ul>
//   <li> UnitMap::list()		all prefixes and SI, Cust and User units
//   <li> UnitMap::listCache()		current cache contents
//   <li> UnitMap::listPref()		all prefixes
//   <li> UnitMap::listDef()		all defining units
//   <li> UnitMap::listSI()		all SI Units
//   <li> UnitMap::listCust()		all customary units
//   <li> UnitMap::listUser()		all user defined units
// </ul>
//
// Units can be defined in the user list by:
// <note> The cache will be cleared if a user defined unit is overwritten,
// to make sure no old value will be used. </note>
// <srcblock>
// UnitMap::putUser("tag", UnitVal(factor,"unit"), "full name (optional)");
//    or:
// UnitMap::putUser(UnitName);
// </srcblock>
// A special set of 'units' used in FITS datasets can be added by the command
// <srcblock>
//	UnitMap::addFITS();
// </srcblock>
// This set can be cleared from the user table by:
// <srcblock>
//	UnitMap::clearFITS();
// </srcblock>
// Note that Unitmap keeps track of the inclusion of the FITS inclusion,
// making multiple calls inexpensive. The list of current FITS units can
// be viewed by running the tUnit program, or looking at the FITSunit
// table.
//
// The cache can be cleared by:
// <srcblock>
// UnitMap::clearCache();
// </srcblock>
// </synopsis> 
//
// <example>
// Check for legal prefix:
// <srcblock>
//	UnitName myUnit;
//	if (UnitMap::getPref("k", myUnit)) { cout << "k has value " << myUnit;}
// </srcblock>
// Define a value for the unit 'beam':
// <srcblock>
// UnitMap::putUser("beam",UnitVal(C::pi * 0.1, "\"_2"),"telescope beam");
// </srcblock>
// List current cache:
// <srcblock>
// UnitMap::listCache();
// </srcblock>
// </example>
//
// <motivation>
// Standard list available to try to enhance use of SI and related units
// </motivation>
//
// <todo asof="941110">
//   <li> Some inlining (did not work first go)
// </todo>

class UnitMap {
public:

//# Constructors
// Default constructor of maps
    UnitMap();

// Destructor
    ~UnitMap();

//# General member functions
// Check if a unit name is known, and return its value if True
// <group name="find">
// Get a prefix definition from key
    static Bool getPref(const String &s, UnitName &name);

// Get a cached definition
    static Bool getCache(const String &s, UnitVal &val);

// Get a standard unit definition (search order: User, Customary, SI)
    static Bool getUnit(const String &s, UnitName &name);
// </group>
// Save a definition of a full unit name in the cache (the cache will be
// cleared if getting too large (200 entries)
    static void putCache(const String &s, const UnitVal &val);

// Define a user standard unit
// <group name="define">
    static void putUser(const String &s, const UnitVal &val);
    static void putUser(const String &s, const UnitVal &val,
			const String &name);
    static void putUser(const UnitName &name);
// </group>
// Remove a user unit
// <group>
    static void removeUser(const String &name);
    static void removeUser(const UnitName &name);
// </group>

// Clear out the cache
    static void clearCache();

// Define FITS related unit names
    static void addFITS();

// Clear FITS related units from user list
    static void clearFITS();

// Translate a FITS unit to the proper units. Note that this is a translation
// of the string only, no conversion. Unknown FITS units are not translated.
// Hence any new definition of the FITS units will work ok
    static Unit fromFITS(const Unit &un);

// Translate to a FITS unit
    static Unit toFITS(const Unit &un);

// List some part of the standard unit lists on cout or stream
// <group name="list">
// List all known unit symbols
// <group>
    static void list(ostream &os);
    static void list();
  // </group>

// List all units in cache
  // <group>
    static void listCache(ostream &os);
    static void listCache();
  // </group>

// List all prefixes
  // <group>
    static void listPref(ostream &os);
    static void listPref();
  // </group>

// List all defining units
  // <group>
    static void listDef(ostream &os);
    static void listDef();
  // </group>

// List all SI units
  // <group>
    static void listSI(ostream &os);
    static void listSI();
  // </group>

// List all customary units
  // <group>
    static void listCust(ostream &os);
    static void listCust();
  // </group>

// List all user defined units
  // <group>
    static void listUser(ostream &os);
    static void listUser();
  // </group>
// </group>

  // Return the different maps
  // <group>
  static const SimpleOrderedMap<String, UnitName> &givePref();
  static const SimpleOrderedMap<String, UnitName> &giveDef();
  static const SimpleOrderedMap<String, UnitName> &giveSI();
  static const SimpleOrderedMap<String, UnitName> &giveCust();
  static const SimpleOrderedMap<String, UnitName> &giveUser();
  static const SimpleOrderedMap<String, UnitVal> &giveCache();
  // </group>
private:
//# Constructors
// Copy constructor (not implemented)
    UnitMap(const UnitMap &other);

//# Operators
// Copy assignment (not implemented)
    UnitMap &operator=(const UnitMap &other);

//# Data members

// Decimal prefix list
    static SimpleOrderedMap<String, UnitName> *mapPref;

// Defining SI unit list
    static SimpleOrderedMap<String, UnitName> *mapDef;

// SI unit list
    static SimpleOrderedMap<String, UnitName> *mapSI;

// Customary list
    static SimpleOrderedMap<String, UnitName> *mapCust;

// User defined unit list
    static SimpleOrderedMap<String, UnitName> *mapUser;

// Cached list
    static SimpleOrderedMap<String, UnitVal> *mapCache;

// FITS unit list inclusion
    static Bool doneFITS;

//# Functions
// Get the name of a FITS unit
    static Bool getNameFITS(UnitName *&name, uInt which);
// Get the belonging unit to a FITS unit
    static const String &getStringFITS(uInt which);
// Initialise the static map
    static void initUM();
};

//# Inline Implementations

#endif
