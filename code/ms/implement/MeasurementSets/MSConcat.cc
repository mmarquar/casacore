//# MSConcat.cc: A class for concatenating MeasurementSets.
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

#include <trial/MeasurementSets/MSConcat.h>
#include <aips/Arrays/Vector.h>
#include <aips/Containers/Block.h>
#include <aips/Containers/Record.h>
#include <aips/Containers/RecordField.h>
#include <aips/Containers/RecordFieldId.h>
#include <aips/Exceptions/Error.h>
#include <aips/Logging/LogIO.h>
#include <aips/Logging/LogOrigin.h>
#include <aips/Mathematics/Math.h>
#include <aips/MeasurementSets/MSAntenna.h>
#include <aips/MeasurementSets/MSAntennaColumns.h>
#include <aips/MeasurementSets/MSDataDescColumns.h>
#include <aips/MeasurementSets/MSFeed.h>
#include <aips/MeasurementSets/MSField.h>
#include <aips/MeasurementSets/MSFieldColumns.h>
#include <aips/MeasurementSets/MSMainColumns.h>
#include <aips/MeasurementSets/MSPolColumns.h>
#include <aips/MeasurementSets/MSSpWindowColumns.h>
#include <aips/Measures/MDirection.h>
#include <aips/Measures/MFrequency.h>
#include <aips/Measures/MeasConvert.h>
#include <aips/TableMeasures/ScalarMeasColumn.h>
#include <aips/TableMeasures/ScalarQuantColumn.h>
#include <aips/Tables/ColumnsIndex.h>
#include <aips/Tables/ScalarColumn.h>
#include <aips/Tables/TableDesc.h>
#include <aips/Tables/TableRow.h>
#include <aips/Utilities/Assert.h>
#include <aips/Utilities/String.h>

MSConcat::MSConcat(MeasurementSet& ms):
  MSColumns(ms),
  itsMS(ms),
  itsFixedShape(isFixedShape(ms.tableDesc()))
{
}

IPosition MSConcat::isFixedShape(const TableDesc& td) {
  IPosition fixedShape(0);
  Bool isFixed = False;
  const Vector<String> hypercolumnNames=td.hypercolumnNames();
  const uInt nHyperCols = hypercolumnNames.nelements();
  Vector<String> dataColNames,coordColNames,idColNames;
  uInt hc = 0;
  while (isFixed == False && hc < nHyperCols) {
    td.hypercolumnDesc(hypercolumnNames(hc), dataColNames, coordColNames,
		       idColNames);
    const uInt nDataCol = dataColNames.nelements();
    uInt dc = 0;
    while (isFixed == False && dc < nDataCol) {
      const String& dataColName = dataColNames(dc);
      // The order of these if conditions is important as I am trying to get
      // the biggest possible fixed shape.
      if (dataColName == MS::columnName(MS::FLAG_CATEGORY) || 
	  dataColName == MS::columnName(MS::DATA) ||
	  dataColName == MS::columnName(MS::FLAG) || 
	  dataColName == MS::columnName(MS::SIGMA_SPECTRUM) ||
	  dataColName == MS::columnName(MS::WEIGHT_SPECTRUM) ||
	  dataColName == MS::columnName(MS::FLOAT_DATA) ||
	  dataColName == MS::columnName(MS::CORRECTED_DATA) || 
	  dataColName == MS::columnName(MS::MODEL_DATA) || 
	  dataColName == MS::columnName(MS::LAG_DATA) ||
	  dataColName == MS::columnName(MS::SIGMA) || 
	  dataColName == MS::columnName(MS::WEIGHT) || 
	  dataColName == MS::columnName(MS::IMAGING_WEIGHT) || 
	  dataColName == MS::columnName(MS::VIDEO_POINT)) {
	const ColumnDesc& colDesc = td.columnDesc(dataColNames(dc));
	isFixed = colDesc.isFixedShape();
	if (isFixed) fixedShape = colDesc.shape();
      }
      dc++;
    }
    hc++;
    dataColNames.resize(0);
    coordColNames.resize(0);
    idColNames.resize(0);
  }
  return fixedShape;
}

void MSConcat::concatenate(const MeasurementSet& otherMS)
{
  LogIO log(LogOrigin("MSConcat", "concatenate"));
  log << "Appending " << otherMS.tableName() 
      << " to " << itsMS.tableName() << endl;
  const ROMSMainColumns otherMainCols(otherMS);
  const ROMSPolarizationColumns otherPolCols(otherMS.polarization());
  const ROMSSpWindowColumns otherSpwCols(otherMS.spectralWindow());
  const ROMSDataDescColumns otherDDCols(otherMS.dataDescription());
  if (otherMS.nrow() > 0) {
    if (itsFixedShape.nelements() > 0) {
      const uInt nShapes = otherDDCols.nrow();
      for (uInt s = 0; s < nShapes; s++) {
	checkShape(getShape(otherDDCols, otherSpwCols, otherPolCols, s));
      }
    }
    checkCategories(otherMainCols);
  }
  uInt oldRows = itsMS.antenna().nrow();
  const Block<uInt> newAntIndices = 
    copyAntennaAndFeed(otherMS.antenna(), otherMS.feed());
  {
    const uInt addedRows = itsMS.antenna().nrow() - oldRows;
    const uInt matchedRows = otherMS.antenna().nrow() - addedRows;
    log << "Added " << addedRows 
	<< " rows and matched " << matchedRows 
	<< " from the antenna subtable" << endl;
  }
  oldRows = itsMS.field().nrow();
  const Block<uInt> newFldIndices = copyField(otherMS.field());
  {
    const uInt addedRows = itsMS.field().nrow() - oldRows;
    const uInt matchedRows = otherMS.field().nrow() - addedRows;
    log << "Added " << addedRows 
	<< " rows and matched " << matchedRows 
	<< " from the field subtable" << endl;
  }
  oldRows = itsMS.dataDescription().nrow();
  const Block<uInt> newDDIndices = copySpwAndPol(otherMS.spectralWindow(),
						 otherMS.polarization(),
						 otherMS.dataDescription());
  {
    const uInt addedRows = itsMS.dataDescription().nrow() - oldRows;
    const uInt matchedRows = otherMS.dataDescription().nrow() - addedRows;
    log << "Added " << addedRows 
	<< " rows and matched " << matchedRows 
	<< " from the data description subtable" << endl;
  }
}

void MSConcat::checkShape(const IPosition& otherShape) const 
{
  const uInt nAxes = min(itsFixedShape.nelements(), otherShape.nelements());
  DebugAssert(nAxes > 0 && nAxes < 4, AipsError);
  if (nAxes > 1 && itsFixedShape(1) != otherShape(1)) {
    throw(AipsError(String("MSConcat::checkShapes\n") + 
		    String("cannot concatenate this measurement set as ") +
		    String("it has a different number of channels\n") +
		    String("and this cannot be changed")));
  }
  if (itsFixedShape(0) != otherShape(0)) {
    throw(AipsError(String("MSConcat::checkShapes\n") + 
		    String("cannot concatenate this measurement set as ") +
		    String("it has a different number of correlations\n") +
		    String("and this cannot be changed")));
  }
}

IPosition MSConcat::getShape(const ROMSDataDescColumns& ddCols, 
			     const ROMSSpWindowColumns& spwCols, 
			     const ROMSPolarizationColumns& polCols, 
			     uInt whichShape) {
  DebugAssert(whichShape < ddCols.nrow(), AipsError);
  const Int polId = ddCols.polarizationId()(whichShape);
  DebugAssert(polId >= 0 && polId < static_cast<Int>(polCols.nrow()),
	      AipsError);
  const Int spwId = ddCols.spectralWindowId()(whichShape);
  DebugAssert(spwId >= 0 && spwId < static_cast<Int>(spwCols.nrow()),
	      AipsError);
  const Int nCorr = polCols.numCorr()(polId);
  DebugAssert(nCorr > 0, AipsError);
  const Int nChan = spwCols.numChan()(spwId);
  DebugAssert(nChan > 0, AipsError);
  return IPosition(2, nCorr, nChan);
}

void MSConcat::checkCategories(const ROMSMainColumns& otherCols) const {
  const Vector<String> cat = flagCategories();
  const Vector<String> otherCat = otherCols.flagCategories();
  const uInt nCat = cat.nelements();
  if (nCat != otherCat.nelements()) {
    throw(AipsError(String("MSConcat::checkCategories\n") + 
		    String("cannot concatenate this measurement set as ") +
		    String("it has a different number of flag categories")));
  }
  for (uInt c = 0; c < nCat; c++) {
    if (cat(c) != otherCat(c)) {
      throw(AipsError(String("MSConcat::checkCategories\n") + 
		      String("cannot concatenate this measurement set as ") +
		      String("it has different flag categories")));
    }
  }
}

Block<uInt> MSConcat::copyAntennaAndFeed(const MSAntenna& otherAnt,
					 const MSFeed& otherFeed) {
  const uInt nAntIds = otherAnt.nrow();
  Block<uInt> antMap(nAntIds);

  const ROMSAntennaColumns otherAntCols(otherAnt);
  MSAntennaColumns& antCols = antenna();
  MSAntenna& ant = itsMS.antenna();
  const Quantum<Double> tol(1, "m");
  const ROTableRow otherAntRow(otherAnt);
  TableRow antRow(ant);

  const String& antIndxName = MSFeed::columnName(MSFeed::ANTENNA_ID);
  MSFeed& feed = itsMS.feed();
  const ROTableRow otherFeedRow(otherFeed);
  TableRow feedRow(feed);
  TableRecord feedRecord;
  ColumnsIndex feedIndex(otherFeed, Vector<String>(1, antIndxName));
  RecordFieldPtr<Int> antInd(feedIndex.accessKey(), antIndxName);
  RecordFieldId antField(antIndxName);
  
  for (uInt a = 0; a < nAntIds; a++) {
    const Int newAntId = 
      antCols.matchAntenna(otherAntCols.positionMeas()(a), tol);
    if (newAntId >= 0) {
      antMap[a] = newAntId;
      // Should really check that the FEED table contains all the entries for
      // this antenna and that they are the same. I'll just assume this for
      // now.
    } else { // need to add a new entry in the ANTENNA subtable
      antMap[a] = ant.nrow();
      ant.addRow();
      antRow.putMatchingFields(antMap[a], otherAntRow.get(a));
      // Copy all the feeds associated with the antenna into the feed
      // table. I'm assuming that they are not already there.
      *antInd = a;
      const Vector<uInt> feedsToCopy = feedIndex.getRowNumbers();
      const uInt nFeedsToCopy = feedsToCopy.nelements();
      uInt destRow = feed.nrow();
      feed.addRow(nFeedsToCopy);
      for (uInt f = 0; f < nFeedsToCopy; f++, destRow++) {
	feedRecord = otherFeedRow.get(feedsToCopy(f));
	feedRecord.define(antField, static_cast<Int>(antMap[a]));
	feedRow.putMatchingFields(destRow, feedRecord);
      }
    }
  }
  return antMap;
}

Block<uInt>  MSConcat::copyField(const MSField& otherFld) {
  const uInt nFlds = otherFld.nrow();
  Block<uInt> fldMap(nFlds);
  const Quantum<Double> tolerance(.1, "deg");
  const ROMSFieldColumns otherFieldCols(otherFld);
  MSFieldColumns& fieldCols = field();

  const MDirection::Types dirType = MDirection::castType(
    fieldCols.referenceDirMeasCol().getMeasRef().getType());
  const MDirection::Types otherDirType = MDirection::castType(
    otherFieldCols.referenceDirMeasCol().getMeasRef().getType());
  
  MDirection::Convert dirCtr;
  if (dirType != otherDirType) { // setup a converter
    dirCtr = MDirection::Convert(otherDirType, dirType);
  }
  MDirection refDir, delayDir, phaseDir;
  MSField& fld = itsMS.field();
  const ROTableRow otherFldRow(otherFld);
  TableRow fldRow(fld);
  for (uInt f = 0; f < nFlds; f++) {
    delayDir = otherFieldCols.delayDirMeas(f);
    phaseDir = otherFieldCols.phaseDirMeas(f);
    refDir = otherFieldCols.referenceDirMeas(f);
    if (dirType != otherDirType) {
      delayDir = dirCtr(delayDir.getValue());
      phaseDir = dirCtr(phaseDir.getValue());
      refDir = dirCtr(refDir.getValue());
    }

    const Int newFld = 
      fieldCols.matchDirection(refDir, delayDir, phaseDir, tolerance);
    if (newFld >= 0) {
      fldMap[f] = newFld;
    } else { // need to add a new entry in the FIELD subtable
      fldMap[f] = fld.nrow();
      fld.addRow();
      fldRow.putMatchingFields(fldMap[f], otherFldRow.get(f));
      if (dirType != otherDirType) {
	DebugAssert(fieldCols.numPoly()(fldMap[f]) == 0, AipsError);
	Vector<MDirection> vdir(1, refDir);
	fieldCols.referenceDirMeasCol().put(fldMap[f], vdir);
	vdir(0) = delayDir;
	fieldCols.delayDirMeasCol().put(fldMap[f], vdir);
	vdir(0) = phaseDir;
	fieldCols.phaseDirMeasCol().put(fldMap[f], vdir);
      }
    }
  }
  return fldMap;
}

Block<uInt> MSConcat::copySpwAndPol(const MSSpectralWindow& otherSpw,
				    const MSPolarization& otherPol,
				    const MSDataDescription& otherDD) {
  const uInt nDDs = otherDD.nrow();
  Block<uInt> ddMap(nDDs);
  
  const ROMSSpWindowColumns otherSpwCols(otherSpw);
  MSSpectralWindow& spw = itsMS.spectralWindow();
  MSSpWindowColumns& spwCols = spectralWindow();
  const ROTableRow otherSpwRow(otherSpw);
  TableRow spwRow(spw);
  const ROMSPolarizationColumns otherPolCols(otherPol);
  MSPolarization& pol = itsMS.polarization();
  MSPolarizationColumns& polCols = polarization();
  const ROTableRow otherPolRow(otherPol);
  TableRow polRow(pol);

  const ROMSDataDescColumns otherDDCols(otherDD);
  MSDataDescColumns& ddCols = dataDescription();
  const Quantum<Double> freqTol(0.01, "Hz");

  const String& spwIdxName = 
    MSDataDescription::columnName(MSDataDescription::SPECTRAL_WINDOW_ID);
  const String& polIdxName = 
    MSDataDescription::columnName(MSDataDescription::POLARIZATION_ID);
  Vector<String> ddIndexCols(2);
  ddIndexCols(0) = spwIdxName;
  ddIndexCols(1) = polIdxName;
  ColumnsIndex ddIndex(itsMS.dataDescription(), ddIndexCols);
  RecordFieldPtr<Int> newSpwPtr(ddIndex.accessKey(), spwIdxName);
  RecordFieldPtr<Int> newPolPtr(ddIndex.accessKey(), polIdxName);
  Vector<Int> corrInt;
  Vector<Stokes::StokesTypes> corrPol;

  for (uInt d = 0; d < nDDs; d++) {
    Bool matchedDD = True;
    DebugAssert(otherDDCols.spectralWindowId()(d) >= 0 &&
		otherDDCols.spectralWindowId()(d) < 
		static_cast<Int>(otherSpw.nrow()), AipsError);
    const uInt otherSpwId = 
      static_cast<uInt>(otherDDCols.spectralWindowId()(d));
    DebugAssert(otherSpwCols.numChan()(otherSpwId) > 0, AipsError);    
    *newSpwPtr = 
      spwCols.matchSpw(otherSpwCols.refFrequencyMeas()(otherSpwId),
		       static_cast<uInt>(otherSpwCols.numChan()(otherSpwId)),
		       otherSpwCols.totalBandwidthQuant()(otherSpwId),
		       otherSpwCols.ifConvChain()(otherSpwId), freqTol);
    
    if (*newSpwPtr < 0) {
      // need to add a new entry in the SPECTRAL_WINDOW subtable
      *newSpwPtr= spw.nrow();
      spw.addRow();
      spwRow.putMatchingFields(*newSpwPtr, otherSpwRow.get(otherSpwId));
      // There cannot be an entry in the DATA_DESCRIPTION Table
      matchedDD = False;
    }
    

    DebugAssert(otherDDCols.polarizationId()(d) >= 0 &&
		otherDDCols.polarizationId()(d) < 
		static_cast<Int>(otherPol.nrow()), AipsError);
    const uInt otherPolId = 
      static_cast<uInt>(otherDDCols.polarizationId()(d));

    otherPolCols.corrType().get(otherPolId, corrInt, True);
    const uInt nCorr = corrInt.nelements();
    corrPol.resize(nCorr);
    for (uInt p = 0; p < nCorr; p++) {
      corrPol(p) = Stokes::type(corrInt(p));
    }
    *newPolPtr = polCols.match(corrPol);
    if (*newPolPtr < 0) {
      // need to add a new entry in the POLARIZATION subtable
      *newPolPtr= pol.nrow();
      pol.addRow();
      polRow.putMatchingFields(*newPolPtr, otherPolRow.get(otherPolId));
      // Again there cannot be an entry in the DATA_DESCRIPTION Table
      matchedDD = False;
    }

    if (matchedDD) {
      // We need to check if there exists an entry in the DATA_DESCRIPTION
      // table with the required spectral window and polarization index.
      ddMap[d] = ddIndex.getRowNumber(matchedDD);
    }
    
    if (!matchedDD) {
      // Add an entry to the data description sub-table
      ddMap[d] = ddCols.nrow();
      itsMS.dataDescription().addRow(1);
      ddCols.polarizationId().put(ddMap[d], *newPolPtr);
      ddCols.spectralWindowId().put(ddMap[d], *newSpwPtr);
    }
  }
  return ddMap;
}
// Local Variables: 
// compile-command: "gmake MSConcat"
// End: 
