//# NewMSSource.cc: The NewMeasurementSet SOURCE Table
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

#include <aips/MeasurementSets/NewMSSource.h>
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

NewMSSource::NewMSSource():hasBeenDestroyed_p(True) { }

NewMSSource::NewMSSource(const String &tableName, TableOption option) 
    : NewMSTable<PredefinedColumns,
      PredefinedKeywords>(tableName, option),hasBeenDestroyed_p(False)
{
    // verify that the now opened table is valid
    if (! validate(this->tableDesc()))
	throw (AipsError("NewMSSource(String &, TableOption) - "
			 "table is not a valid NewMSSource"));
}

NewMSSource::NewMSSource(const String& tableName, const String &tableDescName,
			       TableOption option)
    : NewMSTable<PredefinedColumns,
      PredefinedKeywords>(tableName, tableDescName,option),
      hasBeenDestroyed_p(False)
{
    // verify that the now opened table is valid
    if (! validate(this->tableDesc()))
	throw (AipsError("NewMSSource(String &, String &, TableOption) - "
			 "table is not a valid NewMSSource"));
}

NewMSSource::NewMSSource(SetupNewTable &newTab, uInt nrrow,
			       Bool initialize)
    : NewMSTable<PredefinedColumns,
      PredefinedKeywords>(newTab, nrrow, initialize), 
      hasBeenDestroyed_p(False)
{
    // verify that the now opened table is valid
    if (! validate(this->tableDesc()))
	throw (AipsError("NewMSSource(SetupNewTable &, uInt, Bool) - "
			 "table is not a valid NewMSSource"));
}

NewMSSource::NewMSSource(const Table &table)
    : NewMSTable<PredefinedColumns,
      PredefinedKeywords>(table), hasBeenDestroyed_p(False)
{
    // verify that the now opened table is valid
    if (! validate(this->tableDesc()))
	throw (AipsError("NewMSSource(const Table &) - "
			 "table is not a valid NewMSSource"));
}

NewMSSource::NewMSSource(const NewMSSource &other)
    : NewMSTable<PredefinedColumns,
      PredefinedKeywords>(other), 
      hasBeenDestroyed_p(False)
{
    // verify that other is valid
    if (&other != this) 
	if (! validate(this->tableDesc()))
	    throw (AipsError("NewMSSource(const NewMSSource &) - "
			     "table is not a valid NewMSSource"));
}

NewMSSource::~NewMSSource()
{
// check to make sure that this NewMSSource is still valid
    if (!hasBeenDestroyed_p &&  !validate()) {
	hasBeenDestroyed_p = True;
	// the table is otherwise OK, so ensure that it is written if necessary
	this->flush();
	// now we can thrown an exception
	throw (AipsError("~NewMSSource() - "
			 "Table written is not a valid NewMSSource"));
    }
    // if we get to here, let nature take its course
    // this should not be necessary, but do it for insurance anyway
    hasBeenDestroyed_p = True;
}


NewMSSource& NewMSSource::operator=(const NewMSSource &other)
{
    if (&other != this) {
	NewMSTable<PredefinedColumns,
	PredefinedKeywords>::operator=(other);
	hasBeenDestroyed_p=other.hasBeenDestroyed_p;
    }
    return *this;
}

void NewMSSource::init()
{
    if (! columnMap_p.ndefined()) {
	// the PredefinedColumns
	// CALIBRATION_GROUP 
	colMapDef(CALIBRATION_GROUP, "CALIBRATION_GROUP", TpInt,
		  "Number of grouping for calibration purpose.","","");
	// CODE
	colMapDef(CODE, "CODE", TpString,
		  "Special characteristics of source, "
		  "e.g. Bandpass calibrator","","");
	// DIRECTION 
	colMapDef(DIRECTION, "DIRECTION", TpArrayDouble,
		  "Direction (e.g. RA, DEC).","rad","Direction");
	// INTERVAL
	colMapDef(INTERVAL, "INTERVAL", TpDouble,
		  "Interval of time for which this set of parameters "
		  "is accurate","s","");
	// NAME
	colMapDef(NAME, "NAME", TpString,
		  "Name of source as given during observations","","");
	// NUM_LINES
	colMapDef(NUM_LINES, "NUM_LINES", TpInt,
		  "Number of spectral lines","","");
	// POSITION
	colMapDef(POSITION, "POSITION", TpArrayDouble,
		  "Position (e.g. for solar system objects",
		  "m","Position");
	// PROPER_MOTION
	colMapDef(PROPER_MOTION, "PROPER_MOTION", TpArrayDouble,
		  "Proper motion","rad/s","");
	// PULSAR_ID
	colMapDef(PULSAR_ID, "PULSAR_ID", TpInt,
		  "Pulsar Id, pointer to pulsar table","","");
	// REST_FREQUENCY
	colMapDef(REST_FREQUENCY, "REST_FREQUENCY", TpArrayDouble,
		  "Line rest frequency","Hz","Frequency");
	// SOURCE_ID
	colMapDef(SOURCE_ID, "SOURCE_ID", TpInt,
		  "Source id","","");
	// SOURCE_MODEL
	colMapDef(SOURCE_MODEL, "SOURCE_MODEL", TpRecord,
		  "Component Source Model","","");
	// SPECTRAL_WINDOW_ID
	colMapDef(SPECTRAL_WINDOW_ID,"SPECTRAL_WINDOW_ID",TpInt,
		  "ID for this spectral window setup","","");
	// SYSVEL
	colMapDef(SYSVEL, "SYSVEL", TpArrayDouble,
		  "Systemic velocity at reference","m/s","Radialvelocity");
	// TIME
	colMapDef(TIME, "TIME", TpDouble,
		  "Midpoint of time for which this set of parameters "
		  "is accurate.","s","Epoch");
	// TRANSITION
	colMapDef(TRANSITION, "TRANSITION", TpArrayString,
		  "Line Transition name","","");
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
	shape(0)=2;
	addColumnToDesc(requiredTD, DIRECTION, shape, option);
	addColumnToDesc(requiredTD, PROPER_MOTION, shape, option);
	// Now define all other columns (duplicates are skipped)
	for (i = UNDEFINED_COLUMN+1; 
	     i <= NUMBER_REQUIRED_COLUMNS; i++) {
	    addColumnToDesc(requiredTD, PredefinedColumns(i));
	}
	requiredTD_p=new TableDesc(requiredTD);
    }
}

	
NewMSSource NewMSSource::referenceCopy(const String& newTableName, 
				 const Block<String>& writableColumns) const
{
    return NewMSSource(NewMSTable<PredefinedColumns,PredefinedKeywords>::
		     referenceCopy(newTableName,writableColumns));
}
