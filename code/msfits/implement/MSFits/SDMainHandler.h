//# SDMainFiller.h: fills the MAIN table for the SDFITS filler
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
//#
//# $Id$

#if !defined(AIPS_SDMAINHANDLER_H)
#define AIPS_SDMAINHANDLER_H

#include <aips/aips.h>
#include <aips/Utilities/DataType.h>
#include <aips/Containers/Record.h>
#include <aips/Containers/RecordField.h>

//# Forward Declarations
class NewMeasurementSet;
class NewMSMainColumns;
class MEpoch;
class MVTime;
class TiledDataStManAccessor;

template <class T> class Vector;
template <class T> class Matrix;

// <summary>
// </summary>

// <use visibility=local>   or   <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> SomeClass
//   <li> SomeOtherClass
//   <li> some concept
// </prerequisite>
//
// <etymology>
// </etymology>
//
// <synopsis>
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// </motivation>
//
// <templating arg=T>
//    <li>
//    <li>
// </templating>
//
// <thrown>
//    <li>
//    <li>
// </thrown>
//
// <todo asof="yyyy/mm/dd">
//   <li> add this feature
//   <li> fix this bug
//   <li> start discussion of this possible extension
// </todo>

class SDMainHandler
{
public:
    // default ctor is not attached to a MS and hence is useless until attached
    SDMainHandler();

    // attach this to a MS - mark fields in row as handled
    SDMainHandler(NewMeasurementSet &ms, Vector<Bool> &handledCols, const Record &row,
		  const String &dataHypercubeName);

    // copy ctor
    SDMainHandler(const SDMainHandler &other);

    ~SDMainHandler() {clearAll();}

    // assignment operator, uses copy semantics
    SDMainHandler &operator=(const SDMainHandler &other);

    // attach to a MS, mark fields in row as handled
    void attach(NewMeasurementSet &ms, Vector<Bool> &handledCols, const Record &row,
		const String &dataHypercubeName);

    // reset internals given indicated row, use the same MS
    void resetRow(const Record &row);
    
    // fill - a new row is always added
    void fill(const Record &row, const MEpoch &time, Int antennaId, Int feedId,
	      Int dataDescId, Int fieldId, const MVTime &exposure, 
	      Int observationId, const Matrix<Float> &floatData);
private:
    NewMeasurementSet *ms_p;
    NewMSMainColumns *msCols_p;

    TiledDataStManAccessor *dataAccessor_p;

    Record hyperDef_p;
    RecordFieldPtr<Int> hyperId_p;

    Int scanNumberId_p;
    DataType scanNumberType_p;

    // fields from sdfits2ms, independent of MS version number so far
    Int arrayIdId_p, sigmaId_p, flagRowId_p, intervalId_p, weightId_p, flagId_p, maxDataId_p;

    // cleanup everything
    void clearAll();

    // cleanup row-related stuff
    void clearRow();

    // initialize everything
    void initAll(NewMeasurementSet &ms, Vector<Bool> &handledCols, const Record &row,
		 const String &dataHypercubeName);

    // intialize the row related stuff
    void initRow(Vector<Bool> &handledCols, const Record &row);
};

#endif


