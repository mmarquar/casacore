//# MSTableImpl.cc:  the class that hold measurements from telescopes
//# Copyright (C) 1995,1996,1997,1999,2000
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

#include <aips/MeasurementSets/MSTableImpl.h>
#include <aips/Utilities/String.h>
#include <aips/Tables/SetupNewTab.h>
#include <aips/Tables/TableDesc.h>
#include <aips/Tables/ColDescSet.h>
#include <aips/Tables/TableRecord.h>
#include <aips/Tables/ScaColDesc.h>
#include <aips/Tables/ScaRecordColDesc.h>
#include <aips/Tables/ArrColDesc.h>
#include <aips/Tables/StManAipsIO.h>
#include <aips/Tables/ForwardCol.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Arrays/Vector.h>
#include <aips/Exceptions/Error.h>
#include <aips/MeasurementSets/MeasurementSet.h>

#include <aips/TableMeasures/TableMeasRefDesc.h>
#include <aips/TableMeasures/TableMeasValueDesc.h>
#include <aips/TableMeasures/TableMeasDesc.h>
#include <aips/TableMeasures/TableQuantumDesc.h>
#include <aips/Measures/MDirection.h>
#include <aips/Measures/MDoppler.h>
#include <aips/Measures/MEpoch.h>
#include <aips/Measures/MFrequency.h>
#include <aips/Measures/MPosition.h>
#include <aips/Measures/MRadialVelocity.h>
#include <aips/Measures/MBaseline.h>
#include <aips/Measures/Muvw.h>
#include <aips/Measures/MEarthMagnetic.h>

void MSTableImpl::addMeasColumn(TableDesc& td, const String& column, 
				const String& measure, const String& refCol) {
  String meas = measure;
  meas.downcase();
  TableMeasRefDesc measRef;
  TableMeasValueDesc measVal(td, column);
  if (!refCol.empty()) measRef=TableMeasRefDesc(td,refCol);
  if (meas == "direction") {
    if (refCol.empty()) measRef=TableMeasRefDesc(MDirection::DEFAULT);
    TableMeasDesc<MDirection> measCol(measVal, measRef);
    measCol.write(td);
  } else if (meas == "doppler") {
    if (refCol.empty()) measRef=TableMeasRefDesc(MDoppler::DEFAULT);
    TableMeasDesc<MDoppler> measCol(measVal, measRef);
    measCol.write(td);
  } else if (meas == "epoch") {
    if (refCol.empty()) measRef=TableMeasRefDesc(MEpoch::DEFAULT);
    TableMeasDesc<MEpoch> measCol(measVal, measRef);
    measCol.write(td);
  } else if (meas == "frequency") {
    if (refCol.empty()) measRef=TableMeasRefDesc(MFrequency::DEFAULT);
    TableMeasDesc<MFrequency> measCol(measVal, measRef);
    measCol.write(td);
  } else if (meas == "position") {
    if (refCol.empty()) measRef=TableMeasRefDesc(MPosition::DEFAULT);
    TableMeasDesc<MPosition> measCol(measVal, measRef);
    measCol.write(td);
  } else if (meas == "radialvelocity") {
    if (refCol.empty()) measRef=TableMeasRefDesc(MRadialVelocity::DEFAULT);
    TableMeasDesc<MRadialVelocity> measCol(measVal, measRef);
    measCol.write(td);
  } else if (meas == "baseline") {
    if (refCol.empty()) measRef=TableMeasRefDesc(MBaseline::DEFAULT);
    TableMeasDesc<MBaseline> measCol(measVal, measRef);
    measCol.write(td);
  } else if (meas == "uvw") {
    if (refCol.empty()) measRef=TableMeasRefDesc(Muvw::DEFAULT);
    TableMeasDesc<Muvw> measCol(measVal, measRef);
    measCol.write(td);
  } else if (meas == "earthmagnetic") {
    if (refCol.empty()) measRef=TableMeasRefDesc(MEarthMagnetic::DEFAULT);
    TableMeasDesc<MEarthMagnetic> measCol(measVal, measRef);
    measCol.write(td);
  }
}


Bool MSTableImpl::initialized_p(False);

Int MSTableImpl::mapType(const SimpleOrderedMap<Int,String>& columnMap,
			 const String &name)
{
    // find first occurrance of name in the map (must be only occurrance)
 
    Int type = 0; //# 0=UNDEFINED_COLUMN for all enums
    for (uInt i=0; i<columnMap.ndefined(); i++) {
        if (columnMap.getVal(i) == name) {
            type = columnMap.getKey(i);
            break;
        }
    }
    return type;
}

void MSTableImpl::addColumnToDesc(TableDesc &td, const String& colName,
				  Int colDType, const String& colComment,
				  const String& colUnit,
				  const String& colMeasure, Int ndim,
				  const IPosition& shape, Int option,
				  const String& refCol)
{
    // if the column already exists, simply return
    // NOTE: this does NOT check for the correct type or number of dimensions
    if (td.isColumn(colName)) return;
    if ((colDType>=TpBool && colDType<=TpString)|| colDType==TpRecord) {
	switch (colDType) {
	case TpBool:
	    td.addColumn(ScalarColumnDesc<Bool>(colName,colComment));
	    break;
	case TpInt:
	    td.addColumn(ScalarColumnDesc<Int>(colName,colComment));
	    break;
	case TpFloat:
	    td.addColumn(ScalarColumnDesc<Float>(colName,colComment));
	    break;
	case TpDouble:
	    td.addColumn(ScalarColumnDesc<Double>(colName,colComment));
	    // Check if this should be a TableMeasure column
	    if (colMeasure=="Epoch" || colMeasure=="Frequency") { 
              // Epoch & Frequency are scalar TableMeasures
	      addMeasColumn(td, colName, colMeasure, refCol);
	    }
	    break;
	case TpComplex:
	    td.addColumn(ScalarColumnDesc<Complex>(colName,colComment));
	    break;
	case TpString:
	    td.addColumn(ScalarColumnDesc<String>(colName,colComment));
	    break;
	case TpRecord:
	  // note we use TableRecord iso Record, there is no TpTableRecord
	    td.addColumn(ScalarRecordColumnDesc(colName,colComment));
	    break;
/* these are not needed in the NewMS
	case TpChar:
	    td.addColumn(ScalarColumnDesc<Char>(colName,colComment));
	    break;
	case TpUChar:
	    td.addColumn(ScalarColumnDesc<uChar>(colName,colComment));
	    break;
	case TpShort:
	    td.addColumn(ScalarColumnDesc<Short>(colName,colComment));
	    break;
	case TpUShort:
	    td.addColumn(ScalarColumnDesc<uShort>(colName,colComment));
	    break;
	case TpUInt:
	    td.addColumn(ScalarColumnDesc<uInt>(colName,colComment));
	    break;
	case TpDComplex:
	    td.addColumn(ScalarColumnDesc<DComplex>(colName,colComment));
	    break;
*/
	default:
	    break;
	}
    } else if (colDType>= TpArrayBool && colDType<= TpArrayString) {
	if (option==0) {
	    switch (colDType) {
	    case TpArrayBool:
		td.addColumn(ArrayColumnDesc<Bool>(colName,colComment,ndim));
		break;
	    case TpArrayInt:
		td.addColumn(ArrayColumnDesc<Int>(colName,colComment,ndim));
		break;
	    case TpArrayFloat:
		td.addColumn(ArrayColumnDesc<Float>(colName,colComment,ndim));
		break;
	    case TpArrayDouble:
		td.addColumn(ArrayColumnDesc<Double>(colName,colComment,ndim));
		// Check if this should be a TableMeasure column
		if (colMeasure!="") {
		  addMeasColumn(td, colName, colMeasure, refCol);
		}
		break;
	    case TpArrayComplex:
		td.addColumn(ArrayColumnDesc<Complex>(colName,colComment,ndim));
		break;
	    case TpArrayString:
		td.addColumn(ArrayColumnDesc<String>(colName,colComment,ndim));
		break;
/*
	    case TpArrayChar:
		td.addColumn(ArrayColumnDesc<Char>(colName,colComment,ndim));
		break;
	    case TpArrayUChar:
		td.addColumn(ArrayColumnDesc<uChar>(colName,colComment,ndim));
		break;
	    case TpArrayShort:
		td.addColumn(ArrayColumnDesc<Short>(colName,colComment,ndim));
		break;
	    case TpArrayUShort:
		td.addColumn(ArrayColumnDesc<uShort>(colName,colComment,ndim));
		break;
	    case TpArrayUInt:
		td.addColumn(ArrayColumnDesc<uInt>(colName,colComment,ndim));
		break;
	    case TpArrayDComplex:
		td.addColumn(ArrayColumnDesc<DComplex>(colName,colComment,ndim));
		break;
*/
	    default:
		break;
	    }
	} else {
	    switch (colDType) {
	    case TpArrayBool:
		td.addColumn(ArrayColumnDesc<Bool>(colName,colComment,
						   shape,option));
		break;
	    case TpArrayInt:
		td.addColumn(ArrayColumnDesc<Int>(colName,colComment,
						  shape,option));
		break;
	    case TpArrayFloat:
		td.addColumn(ArrayColumnDesc<Float>(colName,colComment,
						    shape,option));
		break;
	    case TpArrayDouble:
		td.addColumn(ArrayColumnDesc<Double>(colName,colComment,
						     shape,option));
		// Check if this should be a TableMeasure column
		if (colMeasure!="") {
		  addMeasColumn(td, colName, colMeasure, refCol);
		}
		break;
	    case TpArrayComplex:
		td.addColumn(ArrayColumnDesc<Complex>(colName,colComment,
						      shape,option));
		break;
	    case TpArrayString:
		td.addColumn(ArrayColumnDesc<String>(colName,colComment,
						     shape,option));
		break;
/*
	    case TpArrayChar:
		td.addColumn(ArrayColumnDesc<Char>(colName,colComment,
						   shape,option));
		break;
	    case TpArrayUChar:
		td.addColumn(ArrayColumnDesc<uChar>(colName,colComment,
						    shape,option));
		break;
	    case TpArrayShort:
		td.addColumn(ArrayColumnDesc<Short>(colName,colComment,
						    shape,option));
		break;
	    case TpArrayUShort:
		td.addColumn(ArrayColumnDesc<uShort>(colName,colComment,
						     shape,option));
		break;
	    case TpArrayUInt:
		td.addColumn(ArrayColumnDesc<uInt>(colName,colComment,
						   shape,option));
		break;
	    case TpArrayDComplex:
		td.addColumn(ArrayColumnDesc<DComplex>(colName,colComment,
						       shape,option));
		break;
*/
	    default:
		break;
	    }
	}	    
    } else {
      cerr << "MSTableImpl::addColumnToDesc - Invalid data type: "
	   << colDType <<", "<<colName<<endl;
      //	throw(AipsError ("MSTableImpl::addColumnToDesc(...) - "
      //			 "Invalid default data type for specified column"));
    }
    // now add the Unit keywords for non Measure Columns
    // and change the Epoch unit to s (from default of d)
    if ((colMeasure == "" || colMeasure == "Epoch") && colUnit != "") {
      TableQuantumDesc tqd(td,colName,Unit(colUnit));
      tqd.write(td);
    }
    // set the units for Position to "m,m,m" instead of rad,rad,m
    if (colMeasure == "Position" || colMeasure == "uvw") {
      Vector<Unit> vu(3,Unit(colUnit));
      TableQuantumDesc tqd(td,colName,vu);
      tqd.write(td);
    }
}

void MSTableImpl::addKeyToDesc(TableDesc& td, const String& keyName,
			       Int keyDType, const String& keyComment)
{
    switch (keyDType) {
    case TpInt:
	td.rwKeywordSet().define(keyName,Int(0));
	td.rwKeywordSet().setComment(keyName, keyComment);
	break;
    case TpFloat:
        td.rwKeywordSet().define(keyName, Float(0));
	td.rwKeywordSet().setComment(keyName, keyComment);
	break;
    case TpString:
	td.rwKeywordSet().define(keyName, "");
	td.rwKeywordSet().setComment(keyName, keyComment);
	break; 

    case TpTable:
//# cannot define tables in TableDesc (only in actual Table)
//#	td.rwKeywordSet().keysTable()(keywordName(key)) = Table();
//#	td.rwKeywordSet().comment(keywordName(key)) = 
//#	    keywordStandardComment(key);
	break; 
    default:
      cerr << "Data type: "<< keyDType << ", "<< keyName<< "not handled"<<endl;
      //	throw(AipsError ("MSTableImpl::addKeyToDesc(...) - "
      //			 "Data type not handled"));
    }  
}

void MSTableImpl::colMapDef(SimpleOrderedMap<Int,String>& columnMap,
			    SimpleOrderedMap<Int,Int>& colDTypeMap,
			    SimpleOrderedMap<Int,String>& colCommentMap,
			    SimpleOrderedMap<Int,String>& colUnitMap,
			    SimpleOrderedMap<Int,String>& colMeasureTypeMap,
			    Int col,
			    const String& colName,
			    Int colType,
			    const String& colComment,
			    const String& colUnit,
			    const String& colMeasureType)
{
    columnMap.define(col, colName);
    colDTypeMap.define(col, colType);
    colCommentMap.define(col, colComment);
    // no need to define these unless they are different from the
    // default, which is an empty string
    if (colUnit != "") colUnitMap.define(col, colUnit);
    if (colMeasureType != "") colMeasureTypeMap.define(col, colMeasureType);
}

void MSTableImpl::keyMapDef(SimpleOrderedMap<Int,String>& keywordMap,
			    SimpleOrderedMap<Int,Int>& keyDTypeMap,
			    SimpleOrderedMap<Int,String>& keyCommentMap,
			    Int key,
			    const String& keyName,
			    Int keyType,
			    const String& keyComment)
{
    keywordMap.define(key, keyName);
    keyDTypeMap.define(key, keyType);
    keyCommentMap.define(key, keyComment);
}

Bool MSTableImpl::validate(const TableDesc& tabDesc, 
			   const TableDesc& requiredTD)
{
    Bool eqDTypes;
    Bool temp = tabDesc.columnDescSet().
	isSuperset(requiredTD.columnDescSet(), eqDTypes);
#if defined(AIPS_DEBUG)
    if (!temp) {
	cerr << "MSTableImpl::validate - tabDesc not superset of requiredTD"<<endl;
    }
#endif
    // check all of the UNIT and MEASINFO-Type values against 
    // the standard values
    Bool detail = True;
    uInt colnr = 0;
    Vector<String> colNames(requiredTD.columnNames());
    uInt ncol = colNames.nelements();
    while (temp && eqDTypes && detail && colnr < ncol) {
        TableRecord keySet = tabDesc[colNames(colnr)].keywordSet();
        TableRecord reqKeySet = requiredTD[colNames(colnr)].keywordSet();
	// check the units if defined
	if (reqKeySet.isDefined("QuantumUnits")) {
	  detail = keySet.isDefined("QuantumUnits");
#if defined(AIPS_DEBUG)
	  if (!detail) {
	    cerr <<"MSTableImpl::validate - column "<<colNames(colnr) <<
	      " doesn't have QuantumUnits"<<endl;
	  }
#endif
	  if (detail) detail = allEQ(keySet.asArrayString("QuantumUnits"), 
				     reqKeySet.asArrayString("QuantumUnits"));
#if defined(AIPS_DEBUG)
//*** testing
//	  cerr << "Column "<<colNames(colnr)<<" has QuantumUnits "<<
//	    keySet.asArrayString("QuantumUnits")<<endl;
//*** testing
	    if (!detail) {
	      cerr <<"MSTableImpl::validate - column "<<colNames(colnr) <<
		" has invalid QuantumUnits: "<< 
		keySet.asArrayString("QuantumUnits")<<endl;
	    }
#endif
	    // check Measure type if defined
	    if (reqKeySet.isDefined("MEASINFO")) {
	      detail = ToBool(keySet.isDefined("MEASINFO"));
#if defined(AIPS_DEBUG)
	      if (!detail) {
		cerr <<"MSTableImpl::validate - column "<<colNames(colnr) <<
		  " doesn't have MEASINFO"<<endl;
	      }
#endif
	      if (detail) {
		detail = ToBool(reqKeySet.asRecord("MEASINFO").asString("type")
		             == keySet.asRecord("MEASINFO").asString("type"));
	      }
#if defined(AIPS_DEBUG)
		if (!detail) {
		  cerr << "MSTableImpl::validate - column "<<colNames(colnr)
		       << " has invalid MEASURE TYPE: "
		       << keySet.asRecord("MEASINFO").asString("type") << endl;
		}
#endif
	    }
        }
        colnr++;
    }
    return ToBool(temp && eqDTypes && detail);
}
 
Bool MSTableImpl::validate(const TableRecord& tabRec, 
			   const TableDesc& requiredTD)
{
    Bool eqDTypes;
    Bool temp = tabRec.description().
	isSuperset(requiredTD.keywordSet().description(), eqDTypes);
    return ToBool(temp && eqDTypes);
}

Table MSTableImpl::referenceCopy(const Table& tab, const String& newTableName, 
				 const Block<String>& writableColumns)
{
  TableDesc td(tab.tableDesc());
  SetupNewTable setup(newTableName, td, Table::New);
  ForwardColumnEngine fwdEngine(tab);
  StManAipsIO aipsStMan;
  // first bind all columns to the forwarding engine
  setup.bindAll(fwdEngine);
  // now bind columns specified to AipsIO storage manager
  for (uInt i=0; i<writableColumns.nelements(); i++) {
    setup.bindColumn(writableColumns[i], aipsStMan);
  }
  Table msTab(setup,tab.nrow());
  msTab.rwKeywordSet() = tab.keywordSet();
  return msTab;
}

void MSTableImpl::init()
{
    if (initialized_p) return;
    initialized_p = True;
    MeasurementSet::init();
    MSAntenna::init();
    MSDataDescription::init();
    MSDoppler::init();
    MSFeed::init();
    MSField::init();
    MSFlagCmd::init();
    MSFreqOffset::init();
    MSHistory::init();
    MSObservation::init();
    MSPointing::init();
    MSPolarization::init();
    MSProcessor::init();
    MSSource::init();
    MSSpectralWindow::init();
    MSState::init();
    MSSysCal::init();
    MSWeather::init();
}




