//# VisSet.cc: Implementation of VisSet
//# Copyright (C) 1996,1997,1998,1999,2000,2001
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

#include <trial/MeasurementEquations/VisSet.h>
#include <trial/MeasurementEquations/VisBuffer.h>
#include <aips/MeasurementSets/MSColumns.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Arrays/ArrayUtil.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Exceptions/Error.h>
#include <aips/Containers/Record.h>
#include <aips/Tables/ArrColDesc.h>
#include <aips/Tables/ScaColDesc.h>
#include <aips/Tables/TableDesc.h>
#include <aips/Tables/TableRecord.h>
#include <aips/Tables/TiledDataStMan.h>
#include <aips/Tables/TiledColumnStMan.h>
#include <aips/Tables/TiledDataStManAccessor.h>
#include <aips/Tables/TableIter.h>
#include <aips/Arrays/Slice.h>
#include <aips/Arrays/Slicer.h>
#include <aips/Utilities/GenSort.h>
#include <aips/iostream.h>

#include <aips/Logging/LogMessage.h>
#include <aips/Logging/LogSink.h>


VisSet::VisSet(MeasurementSet& ms,const Block<Int>& columns, 
	       const Matrix<Int>& chanSelection, Double timeInterval)
:ms_p(ms)
{
    LogSink logSink;
    LogMessage message(LogOrigin("VisSet","VisSet"));

    // sort out the channel selection
    Int nSpw=ms_p.spectralWindow().nrow();
    MSSpWindowColumns msSpW(ms_p.spectralWindow());
    selection_p.resize(2,nSpw);
    // fill in default selection
    selection_p.row(0)=0; //start
    selection_p.row(1)=msSpW.numChan().getColumn(); 
    for (uInt i=0; i<chanSelection.ncolumn(); i++) {
      Int spw=chanSelection(2,i);
      if (spw>=0 && spw<nSpw && chanSelection(0,i)>=0 && 
	  chanSelection(0,i)+chanSelection(1,i)<=selection_p(1,spw)) {
	// looks like a valid selection, implement it
	selection_p(0,spw)=chanSelection(0,i);
	selection_p(1,spw)=chanSelection(1,i);
      }
    }

    Bool init=True;
    if (ms.tableDesc().isColumn("MODEL_DATA")) {
      TableColumn col(ms,"MODEL_DATA");
      if (col.keywordSet().isDefined("CHANNEL_SELECTION")) {
	Matrix<Int> storedSelection;
	col.keywordSet().get("CHANNEL_SELECTION",storedSelection);
	if (selection_p.shape()==storedSelection.shape() && 
	    allEQ(selection_p,storedSelection)) {
	  init=False;
	} 
      }
    }
    if (init) {
      message.message("Initializing MODEL_DATA, CORRECTED_DATA and IMAGING_WEIGHT columns");

      logSink.post(message);
      removeColumns(ms);
      addColumns(ms);
      ArrayColumn<Complex> mcd(ms,"MODEL_DATA");
      mcd.rwKeywordSet().define("CHANNEL_SELECTION",selection_p);
    }

    iter_p=new VisIter(ms_p,columns,timeInterval);
    for (uInt spw=0; spw<selection_p.ncolumn(); spw++) {
      iter_p->selectChannel(1,selection_p(0,spw),selection_p(1,spw),0,spw);
    }

    // Fill the CORRECTED_DATA, MODEL_DATA and IMAGING_WEIGHT
    // columns when first created.
    if (init) {
      for (iter_p->originChunks(); iter_p->moreChunks(); iter_p->nextChunk()) {
	for (iter_p->origin(); iter_p->more(); (*iter_p)++) {
	  Cube<Complex> data;
	  iter_p->setVis(iter_p->visibility(data,VisibilityIterator::Observed),
		     VisibilityIterator::Corrected);
	  data=Complex(1.0,0.0);
	  iter_p->setVis(data,VisibilityIterator::Model);
	};
      };
      iter_p->originChunks();
    };
}

VisSet::VisSet(const VisSet& vs,const Block<Int>& columns, 
	       Double timeInterval)
{
    ms_p=vs.ms_p;
    selection_p.resize(vs.selection_p.shape());
    selection_p=vs.selection_p;

    iter_p=new VisIter(ms_p,columns,timeInterval);
    for (uInt spw=0; spw<selection_p.ncolumn(); spw++) {
      iter_p->selectChannel(1,selection_p(0,spw),selection_p(1,spw),0,spw);
    }
}

VisSet& VisSet::operator=(const VisSet& other) 
{
    if (this == &other) return *this;
    ms_p=other.ms_p;
    selection_p.resize(other.selection_p.shape());
    selection_p=other.selection_p;
    *iter_p=*(other.iter_p);
    return *this;
}

VisSet::~VisSet() {
  ms_p.flush();
  delete iter_p; iter_p=0;
};

void VisSet::flush() {
  ms_p.flush();
};

VisIter& VisSet::iter() { return *iter_p; }

// Set or reset the channel selection on all iterators
void VisSet::selectChannel(Int nGroup,Int start, Int width, Int increment, 
			   Int spectralWindow)
{
  iter_p->selectChannel(nGroup,start,width,increment,spectralWindow); 
  iter_p->origin();
}
Int VisSet::numberAnt() const 
{
  return ((MeasurementSet&)ms_p).antenna().nrow(); // for single (sub)array only..
}
Int VisSet::numberSpw() const 
{
  return ((MeasurementSet&)ms_p).spectralWindow().nrow(); 
}
Vector<Int> VisSet::startChan() const
{
  return selection_p.row(0);
}
Vector<Int> VisSet::numberChan() const 
{
  return selection_p.row(1); 
}
Int VisSet::numberCoh() const 
{
  return ms_p.nrow();
}

void VisSet::removeColumns(Table& tab) 
{
  if (tab.tableDesc().isColumn("MODEL_DATA"))
    tab.removeColumn("MODEL_DATA");
  if (tab.tableDesc().isColumn("CORRECTED_DATA")) 
    tab.removeColumn("CORRECTED_DATA");
  if (tab.tableDesc().isColumn("IMAGING_WEIGHT")) 
    tab.removeColumn("IMAGING_WEIGHT");
}

// add the model and corrected data columns and the imaging weight
void VisSet::addColumns(Table& tab) 
{		    
  TableDesc td(tab.tableDesc());
  // Determine if a FLOAT_DATA column is present
  Bool floatData=td.isColumn(MS::columnName(MS::FLOAT_DATA));

  // Look for the FLOAT_DATA or DATA column among the 
  // hypercolumns to find the corresponding id column (if any)
  Vector<String> hypercolumnNames=td.hypercolumnNames();
  Bool found=False;
  String dataHypercubeId="";
  if (hypercolumnNames.nelements()>0) {
    for (uInt i=0; i<hypercolumnNames.nelements(); i++) {
      Vector<String> dataColNames,coordColNames,idColNames;
      td.hypercolumnDesc(hypercolumnNames(i),
			 dataColNames,coordColNames,
			 idColNames);
      for (uInt j=0; j<dataColNames.nelements(); j++) {
	if ((floatData && dataColNames(j)==MS::columnName(MS::FLOAT_DATA)) ||
	    (!floatData && dataColNames(j)==MS::columnName(MS::DATA))) {
	  found=idColNames.nelements()>0;
	  if (found) dataHypercubeId=idColNames(0);
	}
      }
    }
  }

  Vector<String> coordColNames(0), idColNames(1);
  TableDesc td1;
  IPosition shape,shapeWt;
  if (!found) {
    Int numCorr;
    if (floatData) {
      ArrayColumn<Float> data(tab,MS::columnName(MS::FLOAT_DATA));
      numCorr=data.shape(0)(0);
    } else {
      ArrayColumn<Complex> data(tab,MS::columnName(MS::DATA));
      numCorr=data.shape(0)(0);
    };
    Int numChan=selection_p(1,0);
    shape=IPosition(2,numCorr,numChan);
    shapeWt=IPosition(1,numChan);
  }
  if (found) {
    idColNames(0)="MODEL_HYPERCUBE_ID"; 
    td1.addColumn(ArrayColumnDesc<Complex>("MODEL_DATA","model data",2));
    td1.addColumn(ScalarColumnDesc<Int>("MODEL_HYPERCUBE_ID","hypercube index"));
  } else {
    idColNames.resize(0);
    td1.addColumn(ArrayColumnDesc<Complex>("MODEL_DATA","model data",shape,
					   ColumnDesc::Direct));
  }    
  td1.defineHypercolumn("TiledData-model",3,
			stringToVector("MODEL_DATA"),coordColNames,
			idColNames);
  TableDesc td2;
  if (found) {
    idColNames(0)="CORRECTED_HYPERCUBE_ID"; 
    td2.addColumn(ArrayColumnDesc<Complex>("CORRECTED_DATA","corrected data",2));
    td2.addColumn(ScalarColumnDesc<Int>("CORRECTED_HYPERCUBE_ID","hypercube index"));
  } else {
    td2.addColumn(ArrayColumnDesc<Complex>("CORRECTED_DATA","corrected data",
					   shape,ColumnDesc::Direct));
  }
  td2.defineHypercolumn("TiledData-corrected",3,
			stringToVector("CORRECTED_DATA"),coordColNames,
			idColNames);
  TableDesc td3;
  if (found) {
    idColNames(0)="IMAGING_WT_HYPERCUBE_ID"; 
    td3.addColumn(ArrayColumnDesc<Float>("IMAGING_WEIGHT","imaging weight",1));
    td3.addColumn(ScalarColumnDesc<Int>("IMAGING_WT_HYPERCUBE_ID","hypercube index"));
  } else {
    td3.addColumn(ArrayColumnDesc<Float>("IMAGING_WEIGHT","imaging weight",
					 shapeWt,ColumnDesc::Direct));
  }
  td3.defineHypercolumn("TiledImagingWeight",2,
		       stringToVector("IMAGING_WEIGHT"),
		       coordColNames,
		       idColNames);

  Bool tiledData=False;

  if (found) {
    // data shape may change
    TiledDataStMan tiledStMan1("TiledData-model");
    tab.addColumn(td1,tiledStMan1);
    TiledDataStMan tiledStMan2("TiledData-corrected");
    tab.addColumn(td2,tiledStMan2);
    TiledDataStMan tiledStMan3("TiledImagingWeight");
    tab.addColumn(td3,tiledStMan3);
    tiledData=True;
    TiledDataStManAccessor modelDataAccessor(tab,"TiledData-model");
    TiledDataStManAccessor corrDataAccessor(tab,"TiledData-corrected");
    TiledDataStManAccessor imWtAccessor(tab,"TiledImagingWeight");
    TableColumn* od;
    if (floatData) {
      od = new ArrayColumn<Float> (tab,MS::columnName(MS::FLOAT_DATA));
    } else {
      od = new ArrayColumn<Complex> (tab,MS::columnName(MS::DATA));
    };
    // get the hypercube ids, sort them, remove the duplicate values
    ScalarColumn<Int> hypercubeId(tab,dataHypercubeId);
    Vector<Int> ids=hypercubeId.getColumn();
    Vector<Int> hId=hypercubeId.getColumn();
    Int nId=genSort(ids,Sort::QuickSort+Sort::NoDuplicates);
    ids.resize(nId,True); // resize and copy values
    Vector<Bool> cubeAdded(nId,False);
    Record values1,values2,values3;
    Int cube;
    Int nRow=tab.nrow();
    Int count=0; // track how many rows with the same id value we've seen
    for (Int i=0; i<nRow; i++) {
      Bool last = (i==(nRow-1));
      // switch hypercubes (or define initial one)
      if (i==0 || hId(i)!=hId(i-1) || last) {
	if (last && hId(i)==hId(i-1)) count++;
	if (count>0) {
	  modelDataAccessor.extendHypercube(count,values1);
	  corrDataAccessor.extendHypercube(count,values2);
	  imWtAccessor.extendHypercube(count,values3);
	  count=0;
	}
	values1.define("MODEL_HYPERCUBE_ID",hId(i));
	values2.define("CORRECTED_HYPERCUBE_ID",hId(i));
	values3.define("IMAGING_WT_HYPERCUBE_ID",hId(i));
	for (cube=0; cube<nId; cube++) if (ids(cube)==hId(i)) break;

	// add new hypercube
	if (!cubeAdded(cube)) {
	  // if the last row is in separate cube, create row now
	  Int initialSize = (last ? 1 : 0); 
	  cubeAdded(cube)=True;
	  Int numCorr=od->shape(i)(0);
	  Int numChan=od->shape(i)(1);
	  Int tileSize=numChan/10+1;
	  IPosition cubeShape(3,numCorr,numChan,initialSize);
	  IPosition tileShape(3,numCorr,tileSize,16384/numCorr/tileSize);
	  IPosition cubeShapeWt(2,numChan,initialSize);
	  IPosition tileShapeWt(2,tileSize,16384/tileSize);
	  modelDataAccessor.addHypercube(cubeShape,tileShape,values1);
	  corrDataAccessor.addHypercube(cubeShape,tileShape,values2);
	  imWtAccessor.addHypercube(cubeShapeWt,tileShapeWt,values3);
	}
      }
      count++;
    }
    delete od;
  } else {
  // If there's no id, assume the data is fixed shape throughout
    Int numCorr;
    if (floatData) {
      ArrayColumn<Float> data(tab,MS::columnName(MS::FLOAT_DATA));
      numCorr=data.shape(0)(0);
    } else {
      ArrayColumn<Complex> data(tab,MS::columnName(MS::DATA));
      numCorr=data.shape(0)(0);
    };
    Int numChan=selection_p(1,0);
    Int tileSize=numChan/10+1;
    IPosition tileShape(3,numCorr,tileSize,16384/numCorr/tileSize);
    TiledColumnStMan tiledStMan1("TiledData-model",tileShape);
    tab.addColumn(td1,tiledStMan1);
    TiledColumnStMan tiledStMan2("TiledData-corrected",tileShape);
    tab.addColumn(td2,tiledStMan2);
    IPosition tileShapeWt(2,tileSize,16384/tileSize);
    TiledColumnStMan tiledStMan3("TiledImagingWeight",tileShapeWt);
    tab.addColumn(td3,tiledStMan3);
  }
}
