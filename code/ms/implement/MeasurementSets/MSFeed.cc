//# NewMSFeed.cc: The NewMeasurementSet FEED Table
//# Copyright (C) 1996,1998,1999,2000
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

#include <aips/MeasurementSets/NewMSFeed.h>
#include <aips/Utilities/String.h>
#include <aips/Tables/SetupNewTab.h>
#include <aips/Tables/TableDesc.h>
#include <aips/Tables/ColDescSet.h>
#include <aips/Tables/ScaColDesc.h>
#include <aips/Tables/ArrColDesc.h>
#include <aips/Tables/StManAipsIO.h>
#include <aips/Tables/ForwardCol.h>
#include <aips/Arrays/Vector.h>
#include <aips/Exceptions/Error.h>

NewMSFeed::NewMSFeed():hasBeenDestroyed_p(True) { }

NewMSFeed::NewMSFeed(const String &tableName, TableOption option) 
    : NewMSTable<PredefinedColumns,
      PredefinedKeywords>(tableName, option),hasBeenDestroyed_p(False)
{
    // verify that the now opened table is valid
    if (! validate(this->tableDesc()))
	throw (AipsError("NewMSFeed(String &, TableOption) - "
			 "table is not a valid NewMSFeed"));
}

NewMSFeed::NewMSFeed(const String& tableName, const String &tableDescName,
			       TableOption option)
    : NewMSTable<PredefinedColumns,
      PredefinedKeywords>(tableName, tableDescName,option),
      hasBeenDestroyed_p(False)
{
    // verify that the now opened table is valid
    if (! validate(this->tableDesc()))
	throw (AipsError("NewMSFeed(String &, String &, TableOption) - "
			 "table is not a valid NewMSFeed"));
}

NewMSFeed::NewMSFeed(SetupNewTable &newTab, uInt nrrow,
			       Bool initialize)
    : NewMSTable<PredefinedColumns,
      PredefinedKeywords>(newTab, nrrow, initialize), 
      hasBeenDestroyed_p(False)
{
    // verify that the now opened table is valid
    if (! validate(this->tableDesc()))
	throw (AipsError("NewMSFeed(SetupNewTable &, uInt, Bool) - "
			 "table is not a valid NewMSFeed"));
}

NewMSFeed::NewMSFeed(const Table &table)
    : NewMSTable<PredefinedColumns,
      PredefinedKeywords>(table), hasBeenDestroyed_p(False)
{
    // verify that the now opened table is valid
    if (! validate(this->tableDesc()))
	throw (AipsError("NewMSFeed(const Table &) - "
			 "table is not a valid NewMSFeed"));
}

NewMSFeed::NewMSFeed(const NewMSFeed &other)
    : NewMSTable<PredefinedColumns,
      PredefinedKeywords>(other), 
      hasBeenDestroyed_p(False)
{
    // verify that other is valid
    if (&other != this) 
	if (! validate(this->tableDesc()))
	    throw (AipsError("NewMSFeed(const NewMSFeed &) - "
			     "table is not a valid NewMSFeed"));
}

NewMSFeed::~NewMSFeed()
{
// check to make sure that this NewMSFeed is still valid
    if (!hasBeenDestroyed_p &&  !validate()) {
	hasBeenDestroyed_p = True;
	// the table is otherwise OK, so ensure that it is written if necessary
	this->flush();
	// now we can thrown an exception
	throw (AipsError("~NewMSFeed() - "
			 "Table written is not a valid NewMSFeed"));
    }
    // if we get to here, let nature take its course
    // this should not be necessary, but do it for insurance anyway
    hasBeenDestroyed_p = True;
}


NewMSFeed& NewMSFeed::operator=(const NewMSFeed &other)
{
    if (&other != this) {
	NewMSTable<PredefinedColumns,
	PredefinedKeywords>::operator=(other);
	hasBeenDestroyed_p=other.hasBeenDestroyed_p;
    }
    return *this;
}

void NewMSFeed::init()
{
    if (! columnMap_p.ndefined()) {
	// the PredefinedColumns
	// ANTENNA_ID
	colMapDef(ANTENNA_ID, "ANTENNA_ID", TpInt,
		  "ID of antenna in this array","","");
	// BEAM_ID
	colMapDef(BEAM_ID,"BEAM_ID",TpInt,
		  "Id for BEAM model","","");
	// BEAM_OFFSET
	colMapDef(BEAM_OFFSET,"BEAM_OFFSET",TpArrayDouble,
		  "Beam position offset (on sky but in antenna"
		  "reference frame)","rad","Direction");
	// FEED_ID
	colMapDef(FEED_ID,"FEED_ID",TpInt,
		  "Feed id","","");
	// FOCUS_LENGTH
	colMapDef(FOCUS_LENGTH,"FOCUS_LENGTH",TpDouble,
		  "Focus lenght","m","");
	// INTERVAL
	colMapDef(INTERVAL,"INTERVAL",TpDouble,
		  "Interval for which this set of parameters is accurate",
		  "s","");
	// NUM_RECEPTORS
	colMapDef(NUM_RECEPTORS,"NUM_RECEPTORS",TpInt,
		  "Number of receptors on this feed (probably 1 or 2)","","");
	// PHASED_FEED_ID
	colMapDef(PHASED_FEED_ID,"PHASED_FEED_ID",TpInt,
		  "index into PHASED_FEED table (ignore if<0)","","");
	// POL_RESPONSE
	colMapDef(POL_RESPONSE,"POL_RESPONSE",TpArrayComplex,
		  "D-matrix i.e. leakage between two receptors","","");
	// POLARIZATION_TYPE
	colMapDef(POLARIZATION_TYPE,"POLARIZATION_TYPE",TpArrayString,
		  "Type of polarization to which a given RECEPTOR responds",
		  "","");
	// POSITION
	colMapDef(POSITION,"POSITION",TpArrayDouble,
		  "Position of feed relative to feed reference position",
		  "m","Position");
	// RECEPTOR_ANGLE
	colMapDef(RECEPTOR_ANGLE,"RECEPTOR_ANGLE",TpArrayDouble,
		  "The reference angle for polarization","rad","");
	// SPECTRAL_WINDOW_ID
	colMapDef(SPECTRAL_WINDOW_ID,"SPECTRAL_WINDOW_ID",TpInt,
		  "ID for this spectral window setup","","");
	// TIME
	colMapDef(TIME,"TIME",TpDouble,
		  "Midpoint of time for which this set of "
		  "parameters is accurate","s","Epoch");

	// PredefinedKeywords

	// init requiredTableDesc
	TableDesc requiredTD;
	// all required keywords
	uInt i;
	for (i = UNDEFINED_KEYWORD+1;
	     i <= NUMBER_PREDEFINED_KEYWORDS; i++) {
	    addKeyToDesc(requiredTD, PredefinedKeywords(i));
	}
	
	// all required columns 
	// First define the columns with fixed size arrays
	IPosition shape(1,3);
	ColumnDesc::Option option=ColumnDesc::Direct;
	addColumnToDesc(requiredTD, POSITION, shape, option);
	// define the columns with known dimensionality
	addColumnToDesc(requiredTD, BEAM_OFFSET, 2);
	addColumnToDesc(requiredTD, POLARIZATION_TYPE, 1);
	addColumnToDesc(requiredTD, POL_RESPONSE, 2);
	addColumnToDesc(requiredTD, RECEPTOR_ANGLE, 1);
	// Now define all other columns (duplicates are skipped)
	for (i = UNDEFINED_COLUMN+1; 
	     i <= NUMBER_REQUIRED_COLUMNS; i++) {
	    addColumnToDesc(requiredTD, PredefinedColumns(i));
	}
	requiredTD_p=new TableDesc(requiredTD);
    }
}

NewMSFeed NewMSFeed::referenceCopy(const String& newTableName, 
			     const Block<String>& writableColumns) const
{
    return NewMSFeed(NewMSTable<PredefinedColumns,PredefinedKeywords>::referenceCopy
		  (newTableName,writableColumns));
}
