//# SDAntennaFiller.cc: an ANTENNA filler for SDFITS data  
//# Copyright (C) 2000
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

//# Includes
#include <trial/MeasurementSets/SDAntennaHandler.h>

#include <aips/Tables/ColumnsIndex.h>
#include <aips/MeasurementSets/NewMeasurementSet.h>
#include <aips/MeasurementSets/NewMSAntennaColumns.h>
#include <aips/MeasurementSets/NewMSAntenna.h>
#include <aips/Containers/Record.h>
#include <aips/Arrays/Vector.h>
#include <aips/Quanta/Quantum.h>
#include <aips/Mathematics/Constants.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Tables/ScalarColumn.h>
#include <aips/Tables/ArrayColumn.h>
#include <aips/TableMeasures/ScalarMeasColumn.h>
#include <aips/Utilities/Assert.h>
#include <aips/Exceptions/Error.h>
#include <aips/Measures/MeasTable.h>
#include <aips/Tables/TableDesc.h>
#include <aips/Arrays/ArrayUtil.h>

SDAntennaHandler::SDAntennaHandler() 
    : index_p(0), msAnt_p(0), msAntCols_p(0), rownr_p(-1),
      siteLongFldNum_p(-1), siteLatFldNum_p(-1), siteElevFldNum_p(-1)
{;}

SDAntennaHandler::SDAntennaHandler(NewMeasurementSet &ms, Vector<Bool> &handledCols,
				   const Record &row) 
    : index_p(0), msAnt_p(0), msAntCols_p(0), rownr_p(-1),
      siteLongFldNum_p(-1), siteLatFldNum_p(-1), siteElevFldNum_p(-1)
{
    initAll(ms, handledCols, row);
}

SDAntennaHandler::SDAntennaHandler(const SDAntennaHandler &other) 
    : index_p(0), msAnt_p(0), msAntCols_p(0), rownr_p(-1),
      siteLongFldNum_p(-1), siteLatFldNum_p(-1), siteElevFldNum_p(-1)
{
    *this = other;
}

SDAntennaHandler &SDAntennaHandler::operator=(const SDAntennaHandler &other)
{
    if (this != &other) {
	clearAll();
	index_p = new ColumnsIndex(*(other.index_p));
	AlwaysAssert(index_p, AipsError);
	// need to avoid the assignment operator here because we want
	// this to point to the field in index_p, not in other.index_p
	nameKey_p.attachToRecord(index_p->accessKey(),
				 NewMSAntenna::columnName(NewMSAntenna::NAME));
	msAnt_p = new NewMSAntenna(*(other.msAnt_p));
	AlwaysAssert(msAnt_p, AipsError);
	msAntCols_p = new NewMSAntennaColumns(*msAnt_p);
	AlwaysAssert(msAntCols_p, AipsError);
	
	rownr_p = other.rownr_p;
	
	// this should point to the same field as that in other
	telescopField_p = other.telescopField_p;
	
	siteLongFldNum_p = other.siteLongFldNum_p;
	siteLatFldNum_p = other.siteLatFldNum_p;
	siteElevFldNum_p = other.siteElevFldNum_p;
	mountField_p = other.mountField_p;
	msNameField_p = other.msNameField_p;
	stationField_p = other.stationField_p;
	orbitIdField_p = other.orbitIdField_p;
	phasedArrayIdField_p = other.phasedArrayIdField_p;
	dishDiameterField_p = other.dishDiameterField_p;
	offsetField_p = other.offsetField_p;
	positionField_p = other.positionField_p;
    }
    return *this;
}

void SDAntennaHandler::attach(NewMeasurementSet &ms, Vector<Bool> &handledCols, const Record &row)
{
    clearAll();
    initAll(ms, handledCols, row);
}

void SDAntennaHandler::resetRow(const Record &row) 
{
    clearRow();
    Vector<Bool> dummyHandled;
    initRow(dummyHandled, row);
}

void SDAntennaHandler::fill(const Record &row)
{
    // don't bother unless there is something there
    if (msAnt_p) {
	if (telescopField_p.isAttached()) {
	    // fill the key with the telescope value
	    *nameKey_p = *telescopField_p;
	} else {
	    // use ANTENNA_NAME only if TELESCOP field is not present
	    if (msNameField_p.isAttached()) {
		*nameKey_p = *msNameField_p;
	    } else {
		// just use an empty string as the key
		*nameKey_p = "";
	    }
	}
	if (mountField_p.isAttached()) {
	    *mountKey_p = *mountField_p;
	} 
	if (stationField_p.isAttached()) {
	    *stationKey_p = *stationField_p;
	} 
	if (dishDiameterField_p.isAttached()) {
	    *dishDiameterKey_p = *dishDiameterField_p;
	}
	if (phasedIdKey_p.isAttached()) {
	    *phasedIdKey_p = *phasedArrayIdField_p;
	}
	if (orbitIdKey_p.isAttached()) {
	    *orbitIdKey_p = *orbitIdField_p;
	}
	Vector<uInt> foundRows = index_p->getRowNumbers();
	Bool found = False;
	MPosition pos;
	Vector<Double> offset(3,0.0);
	if (siteLongFldNum_p >= 0) {
	    // construct an MPosition from these values
	    Double siteLong = row.asDouble(siteLongFldNum_p);
	    Double siteLat = row.asDouble(siteLatFldNum_p);
	    Double siteElev = row.asDouble(siteElevFldNum_p);
	    pos = MPosition(Quantity(siteLong,"m"),
			    Quantity(siteLat,"deg"),
			    Quantity(siteElev,"deg"),
			    MPosition::WGS84);
	} else {
	    // SITE* keywords take precendence over ARRAY_POSITION 
	    if (positionField_p.isAttached()) {
		// we write out this column as ITRF with all values in meters
		pos = MPosition(MVPosition(Quantum<Vector<Double> >(*positionField_p,"m")),
				MPosition::ITRF);
	    } else {
		// if this returns False, pos will still be set at its unset value (0,0,0)
		MeasTable::Observatory(pos,*nameKey_p);
	    }
	}
	if (offsetField_p.isAttached()) {
	    offset = *offsetField_p;
	}
	if (foundRows.nelements() > 0) {
	    // we have at least 1 candidate
	    uInt whichOne = 0;
	    // if there are no positions, stop and use the first one
	    if (siteLongFldNum_p < 0) {
		found = True;
	    } else {
		Vector<Double> thisSite, thatSite;
		thisSite = pos.getAngle().getValue();
		while (!found && whichOne<foundRows.nelements()) {
		    thatSite = msAntCols_p->positionMeas()(foundRows(whichOne)).getAngle().getValue();
		    if (allEQ(thisSite,thatSite) && 
			allEQ(offset,msAntCols_p->offset()(foundRows(whichOne)))) {
			found = True;
		    }
		    if (!found) whichOne++;
		}
	    }
	    if (found) {
		rownr_p = foundRows(whichOne);
	    }
	}
	if (!found) {
	    // we need to add one
	    rownr_p = msAnt_p->nrow();
	    msAnt_p->addRow();
	    if (dishDiameterKey_p.isAttached()) {
		msAntCols_p->dishDiameter().put(rownr_p,*dishDiameterKey_p);
	    } else {
		msAntCols_p->dishDiameter().put(rownr_p,0.0);
	    }
	    msAntCols_p->flagRow().put(rownr_p,False);
	    if (mountKey_p.isAttached()) {
		msAntCols_p->mount().put(rownr_p,*mountKey_p);
	    } else {
		msAntCols_p->mount().put(rownr_p,"");
	    }
	    msAntCols_p->name().put(rownr_p, *nameKey_p);
	    msAntCols_p->offset().put(uInt(rownr_p),offset);
	    msAntCols_p->positionMeas().put(rownr_p,pos);
	    if (stationKey_p.isAttached()) {
		msAntCols_p->station().put(rownr_p,*stationKey_p);
	    } else {
		// for want of something better to put in here ...
		msAntCols_p->station().put(rownr_p,*nameKey_p);
	    }
	    if (orbitIdField_p.isAttached()) {
		if (*orbitIdField_p >= 0 && !orbitIdKey_p.isAttached()) {
		    // apparently this is actually used, set the index, add the column
		    addOrbitIdColumn();
		}
		if (orbitIdKey_p.isAttached()) {
		    // this means that the column is also available
		    msAntCols_p->orbitId().put(rownr_p,*orbitIdField_p);
		}
	    }
	    if (phasedArrayIdField_p.isAttached()) {
		if (*phasedArrayIdField_p >= 0 && !phasedIdKey_p.isAttached()) {
		    // apparently this is actually used, set the index, add the column
		    addPhasedArrayIdColumn();
		}
		if (phasedIdKey_p.isAttached()) {
		    // this means that the column is also available
		    msAntCols_p->phasedArrayId().put(rownr_p,*phasedArrayIdField_p);
		}
	    }
	}
	name_p = *nameKey_p;
	position_p = msAntCols_p->positionMeas()(rownr_p);
    }
}

void SDAntennaHandler::clearAll()
{
    delete index_p;
    index_p = 0;

    delete msAnt_p;
    msAnt_p = 0;

    delete msAntCols_p;
    msAntCols_p = 0;

    clearRow();
}

void SDAntennaHandler::clearRow()
{
    telescopField_p.detach();
    mountField_p.detach();
    msNameField_p.detach();
    stationField_p.detach();
    siteLongFldNum_p = siteLatFldNum_p = siteElevFldNum_p = -1;
    rownr_p = -1;
}

void SDAntennaHandler::initAll(NewMeasurementSet &ms, Vector<Bool> &handledCols, const Record &row)
{
    msAnt_p = new NewMSAntenna(ms.antenna());
    AlwaysAssert(msAnt_p, AipsError);

    msAntCols_p = new NewMSAntennaColumns(*msAnt_p);
    AlwaysAssert(msAntCols_p, AipsError);

    initRow(handledCols, row);

    // index on NAME column, but when you get that index, it
    // might be a number of rows and so it will be necessary to check
    // the position at each row before a true match is found
    // Optionally index on other columns as necessary
    String indxStr = NewMSAntenna::columnName(NewMSAntenna::NAME);
    
    if (mountField_p.isAttached()) {
	indxStr += ",";
	indxStr += NewMSAntenna::columnName(NewMSAntenna::MOUNT);
    } 
    if (stationField_p.isAttached()) {
	indxStr += ",";
	indxStr += NewMSAntenna::columnName(NewMSAntenna::STATION);
    }
    if (dishDiameterField_p.isAttached()) {
	indxStr += ",";
	indxStr += NewMSAntenna::columnName(NewMSAntenna::DISH_DIAMETER);
    }
    // ORBIT_ID and PHASED_ARRAY_ID are dealt with later, if necessary
    index_p = new ColumnsIndex(*msAnt_p, stringToVector(indxStr));
    AlwaysAssert(index_p, AipsError);
    

    nameKey_p.attachToRecord(index_p->accessKey(), 
			     NewMSAntenna::columnName(NewMSAntenna::NAME));
    if (stationField_p.isAttached()) {
	stationKey_p.attachToRecord(index_p->accessKey(),
				    NewMSAntenna::columnName(NewMSAntenna::STATION));
    }
    if (mountField_p.isAttached()) {
	mountKey_p.attachToRecord(index_p->accessKey(),
				  NewMSAntenna::columnName(NewMSAntenna::MOUNT));
    }
    if (dishDiameterField_p.isAttached()) {
	dishDiameterKey_p.attachToRecord(index_p->accessKey(),
					 NewMSAntenna::columnName(NewMSAntenna::DISH_DIAMETER));
    }
    // orbit_id and phased_array_id columns are dealt with elsewhere
}

void SDAntennaHandler::initRow(Vector<Bool> &handledCols, const Record &row)
{
    AlwaysAssert(handledCols.nelements()==row.description().nfields(), AipsError);

    if (row.fieldNumber("TELESCOP") >= 0) {
	telescopField_p.attachToRecord(row, "TELESCOP");
	handledCols(row.fieldNumber("TELESCOP")) = True;
    }
    siteLongFldNum_p = row.fieldNumber("SITELONG");
    siteLatFldNum_p = row.fieldNumber("SITELAT");
    siteElevFldNum_p = row.fieldNumber("SITEELEV");

    // its all or nothing with these
    if (siteLongFldNum_p >= 0 && siteLatFldNum_p >= 0 && siteElevFldNum_p >= 0) {
	handledCols(siteLongFldNum_p) = True;
	handledCols(siteLatFldNum_p) = True;
	handledCols(siteElevFldNum_p) = True;
    } else {
	siteLongFldNum_p = siteLatFldNum_p = siteElevFldNum_p = -1;
    }

    if (row.fieldNumber("ANTENNA_MOUNT") >= 0) {
	mountField_p.attachToRecord(row, "ANTENNA_MOUNT");
	handledCols(row.fieldNumber("ANTENNA_MOUNT")) = True;
    }
    if (row.fieldNumber("ANTENNA_NAME") >= 0) {
	msNameField_p.attachToRecord(row, "ANTENNA_NAME");
	handledCols(row.fieldNumber("ANTENNA_NAME")) = True;
    }
    if (row.fieldNumber("ANTENNA_STATION") >= 0) {
	stationField_p.attachToRecord(row, "ANTENNA_STATION");
	handledCols(row.fieldNumber("ANTENNA_STATION")) = True;
    }
    if (row.fieldNumber("ANTENNA_DISH_DIAMETER") >= 0 &&
	row.dataType("ANTENNA_DISH_DIAMETER") == TpDouble) {
	dishDiameterField_p.attachToRecord(row, "ANTENNA_DISH_DIAMETER");
	handledCols(row.fieldNumber("ANTENNA_DISH_DIAMETER")) = True;
    }
    if (row.fieldNumber("ANTENNA_OFFSET") >= 0 &&
	row.dataType("ANTENNA_OFFSET") == TpArrayDouble) {
	offsetField_p.attachToRecord(row, "ANTENNA_OFFSET");
	handledCols(row.fieldNumber("ANTENNA_OFFSET")) = True;
    }
    if (row.fieldNumber("ANTENNA_ORBIT_ID") >= 0 &&
	row.dataType("ANTENNA_ORBIT_ID") == TpInt) {
	orbitIdField_p.attachToRecord(row, "ANTENNA_ORBIT_ID");
	handledCols(row.fieldNumber("ANTENNA_ORBIT_ID")) = True;
    }
    if (row.fieldNumber("ANTENNA_PHASED_ARRAY_ID") >= 0 &&
	row.dataType("ANTENNA_PHASED_ARRAY_ID") == TpInt) {
	phasedArrayIdField_p.attachToRecord(row, "ANTENNA_PHASED_ARRAY_ID");
	handledCols(row.fieldNumber("ANTENNA_PHASED_ARRAY_ID")) = True;
    }
    if (row.fieldNumber("ANTENNA_POSITION") >= 0 &&
	row.dataType("ANTENNA_POSITION") == TpArrayDouble) {
	positionField_p.attachToRecord(row, "ANTENNA_POSITION");
	handledCols(row.fieldNumber("ANTENNA_POSITION")) = True;
    }
   // row number isn't set until the following fill
    rownr_p = -1;
}

void SDAntennaHandler::addPhasedArrayIdColumn()
{
    // if the index field is already attached, do nothing
    if (!phasedIdKey_p.isAttached() && index_p) {
	Vector<String> indexNames = index_p->columnNames();
	delete index_p;
	index_p = 0;
	delete msAntCols_p;
	msAntCols_p = 0;
	// we need to add a new column to the ANTENNA table
	TableDesc td;
	NewMSAntenna::addColumnToDesc(td,NewMSAntenna::PHASED_ARRAY_ID);
	msAnt_p->addColumn(td[0]);
	// remake the columns object
	msAntCols_p = new NewMSAntennaColumns(*msAnt_p);
	AlwaysAssert(msAntCols_p, AipsError);
	// and the index
	indexNames.resize(indexNames.nelements()+1, True);
	indexNames(indexNames.nelements()-1) = 
	    NewMSAntenna::columnName(NewMSAntenna::PHASED_ARRAY_ID);
	index_p = new ColumnsIndex(*msAnt_p, indexNames);
	AlwaysAssert(index_p, AipsError);	
	nameKey_p.attachToRecord(index_p->accessKey(), 
				 NewMSAntenna::columnName(NewMSAntenna::NAME));
	if (stationField_p.isAttached()) {
	    stationKey_p.attachToRecord(index_p->accessKey(),
					NewMSAntenna::columnName(NewMSAntenna::STATION));
	}
	if (mountField_p.isAttached()) {
	    mountKey_p.attachToRecord(index_p->accessKey(),
				      NewMSAntenna::columnName(NewMSAntenna::MOUNT));
	}
	if (dishDiameterField_p.isAttached()) {
	    dishDiameterKey_p.attachToRecord(index_p->accessKey(),
					     NewMSAntenna::columnName(NewMSAntenna::DISH_DIAMETER));
	}
	phasedIdKey_p.attachToRecord(index_p->accessKey(),
				     NewMSAntenna::columnName(NewMSAntenna::PHASED_ARRAY_ID));
	if (anyEQ(indexNames, NewMSAntenna::columnName(NewMSAntenna::ORBIT_ID))) {
	    orbitIdKey_p.attachToRecord(index_p->accessKey(),
					NewMSAntenna::columnName(NewMSAntenna::ORBIT_ID));
	}
    }
}

void SDAntennaHandler::addOrbitIdColumn()
{
    // if the index field is already attached, do nothing
    if (!orbitIdKey_p.isAttached() && index_p) {
	Vector<String> indexNames = index_p->columnNames();
	delete index_p;
	index_p = 0;
	delete msAntCols_p;
	msAntCols_p = 0;
	// we need to add a new column to the ANTENNA table
	TableDesc td;
	NewMSAntenna::addColumnToDesc(td,NewMSAntenna::ORBIT_ID);
	msAnt_p->addColumn(td[0]);
	// remake the columns object
	msAntCols_p = new NewMSAntennaColumns(*msAnt_p);
	AlwaysAssert(msAntCols_p, AipsError);
	// and the index
	indexNames.resize(indexNames.nelements()+1, True);
	indexNames(indexNames.nelements()-1) = NewMSAntenna::columnName(NewMSAntenna::ORBIT_ID);
	index_p = new ColumnsIndex(*msAnt_p, indexNames);
	AlwaysAssert(index_p, AipsError);	
	nameKey_p.attachToRecord(index_p->accessKey(), 
				 NewMSAntenna::columnName(NewMSAntenna::NAME));
	if (stationField_p.isAttached()) {
	    stationKey_p.attachToRecord(index_p->accessKey(),
					NewMSAntenna::columnName(NewMSAntenna::STATION));
	}
	if (mountField_p.isAttached()) {
	    mountKey_p.attachToRecord(index_p->accessKey(),
				      NewMSAntenna::columnName(NewMSAntenna::MOUNT));
	}
	if (dishDiameterField_p.isAttached()) {
	    dishDiameterKey_p.attachToRecord(index_p->accessKey(),
					     NewMSAntenna::columnName(NewMSAntenna::DISH_DIAMETER));
	}
	orbitIdKey_p.attachToRecord(index_p->accessKey(),
				    NewMSAntenna::columnName(NewMSAntenna::ORBIT_ID));
	if (anyEQ(indexNames, NewMSAntenna::columnName(NewMSAntenna::PHASED_ARRAY_ID))) {
	    phasedIdKey_p.attachToRecord(index_p->accessKey(),
					 NewMSAntenna::columnName(NewMSAntenna::PHASED_ARRAY_ID));
	}
    }
}
