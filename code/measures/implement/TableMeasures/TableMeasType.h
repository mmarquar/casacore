//# TableMeasType.h: Encapsulates the Measures type in the TableMeasDesc.
//# Copyright (C) 1999
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

#if !defined(AIPS_TABLEMEASTYPE_H)
#define AIPS_TABLEMEASTYPE_H


//# Includes
#include <aips/aips.h>
#include <aips/Measures/MeasureHolder.h>

//# Forward Declarations
class String;
class Table;
class RecordInterface;

// <summary>
// Definition of a Measure column in a Table.  
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="tTableMeasures.cc">
// </reviewed>

// <prerequisite>
//# Classes you should understand before using this one.
//   <li> <linkto module=Measures>Measures</linkto>
//   <li> <linkto module=Tables>Tables</linkto>
// </prerequisite>

// <synopsis>
// </synopsis>

// <example>
// </example>

// <motivation>
// Creating the required keyword for the definition of a Measure
// in a Table is somewhat complicated. This class assists in that
// process.
// </motivation>
//
// <thrown>
//    <li>
// </thrown>
//
//# <todo asof="$DATE:$">
//# A List of bugs, limitations, extensions or planned refinements.
//# </todo>


class TableMeasType
{
public:
  TableMeasType();

  // Construct from the given type of measure.
  explicit TableMeasType (const Measure&);

  // Copy constructor (copy semantics).
  TableMeasType (const TableMeasType& that);

  ~TableMeasType();
    
  // Assignment operator (copy semantics)
  TableMeasType& operator= (const TableMeasType& that);
    
  // Returns the descriptor's measure type as a String.
  const String& type() const;

  // Translates the refCode for the descriptors measure type.    
  const String& refType (uInt refCode) const;
    
  // Returns the reference code for this object given a string.  Throws
  // an exception if the refString is invalid for this object.
  uInt refCode (const String& refString) const;

  // Creates a record from the MeasureHolder.
  void toRecord (RecordInterface& rec);
    
private:
  Int itsNtypes;			//# number of refcodes/strings
  const String* itsStypes;		//# refcode strings
  const uInt*   itsTyps;		//# refcodes
  MeasureHolder itsMeasHolder;	        //# Holds the measure
};


#endif
