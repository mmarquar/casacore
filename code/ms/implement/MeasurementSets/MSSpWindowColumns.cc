//# MSSpWindowColumns.cc:  provides easy access to MeasurementSet columns
//# Copyright (C) 1996,1999,2000,2002
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

#include <ms/MeasurementSets/MSSpWindowColumns.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/Exceptions/Error.h>
#include <casa/BasicMath/Math.h>
#include <ms/MeasurementSets/MSSpectralWindow.h>
#include <measures/Measures/MeasRef.h>
#include <casa/Quanta/MVFrequency.h>
#include <casa/Quanta/Quantum.h>
#include <casa/Quanta/UnitVal.h>
#include <tables/Tables/ColDescSet.h>
#include <tables/Tables/TableDesc.h>
#include <casa/Utilities/Assert.h>

namespace casa { //# NAMESPACE CASA - BEGIN

ROMSSpWindowColumns::
ROMSSpWindowColumns(const MSSpectralWindow& msSpWindow):
  chanFreq_p(msSpWindow, MSSpectralWindow::
	     columnName(MSSpectralWindow::CHAN_FREQ)),
  chanWidth_p(msSpWindow, MSSpectralWindow::
	      columnName(MSSpectralWindow::CHAN_WIDTH)),
  effectiveBW_p(msSpWindow, MSSpectralWindow::
		columnName(MSSpectralWindow::EFFECTIVE_BW)),
  flagRow_p(msSpWindow, MSSpectralWindow::
	    columnName(MSSpectralWindow::FLAG_ROW)),
  freqGroup_p(msSpWindow, MSSpectralWindow::
	      columnName(MSSpectralWindow::FREQ_GROUP)),
  freqGroupName_p(msSpWindow, MSSpectralWindow::
		  columnName(MSSpectralWindow::FREQ_GROUP_NAME)),
  ifConvChain_p(msSpWindow, MSSpectralWindow::
		columnName(MSSpectralWindow::IF_CONV_CHAIN)),
  measFreqRef_p(msSpWindow, MSSpectralWindow::
		columnName(MSSpectralWindow::MEAS_FREQ_REF)),
  name_p(msSpWindow, MSSpectralWindow::
	 columnName(MSSpectralWindow::NAME)),
  netSideband_p(msSpWindow, MSSpectralWindow::
		columnName(MSSpectralWindow::NET_SIDEBAND)),
  numChan_p(msSpWindow, MSSpectralWindow::
	    columnName(MSSpectralWindow::NUM_CHAN)),
  refFrequency_p(msSpWindow, MSSpectralWindow::
		 columnName(MSSpectralWindow::REF_FREQUENCY)),
  resolution_p(msSpWindow, MSSpectralWindow::
	       columnName(MSSpectralWindow::RESOLUTION)),
  totalBandwidth_p(msSpWindow, MSSpectralWindow::
		   columnName(MSSpectralWindow::TOTAL_BANDWIDTH)),
  assocNature_p(),
  assocSpwId_p(),
  bbcNo_p(),
  bbcSideband_p(),
  dopplerId_p(),
  receiverId_p(),
  chanFreqMeas_p(msSpWindow, MSSpectralWindow::
		 columnName(MSSpectralWindow::CHAN_FREQ)),
  refFrequencyMeas_p(msSpWindow, MSSpectralWindow::
		     columnName(MSSpectralWindow::REF_FREQUENCY)),
  chanFreqQuant_p(msSpWindow, MSSpectralWindow::
		  columnName(MSSpectralWindow::CHAN_FREQ)),
  chanWidthQuant_p(msSpWindow, MSSpectralWindow::
		   columnName(MSSpectralWindow::CHAN_WIDTH)),
  effectiveBWQuant_p(msSpWindow, MSSpectralWindow::
		     columnName(MSSpectralWindow::EFFECTIVE_BW)),
  refFrequencyQuant_p(msSpWindow, MSSpectralWindow::
		      columnName(MSSpectralWindow::REF_FREQUENCY)),
  resolutionQuant_p(msSpWindow, MSSpectralWindow::
		    columnName(MSSpectralWindow::RESOLUTION)),
  totalBandwidthQuant_p(msSpWindow, MSSpectralWindow::
			columnName(MSSpectralWindow::TOTAL_BANDWIDTH))
{
  attachOptionalCols(msSpWindow);
}

ROMSSpWindowColumns::~ROMSSpWindowColumns() {}

Int ROMSSpWindowColumns::
matchSpw(const MFrequency& refFreq, uInt nChan, 
	 const Quantum<Double>& bandwidth, Int ifChain,
	 const Quantum<Double>& tolerance, Int tryRow) const {
  uInt r = nrow();
  if (r == 0) return -1;
  // Convert the reference frequency to Hz
  const MFrequency::Types refType = 
    MFrequency::castType(refFreq.getRef().getType());
  const Double refFreqInHz = refFreq.getValue().getValue();
  // Convert the totalBandwidth to Hz
  const Unit Hz("Hz");
  DebugAssert(bandwidth.check(Hz.getValue()), AipsError);
  const Double bandwidthInHz = bandwidth.getValue(Hz);
  // Convert the tolerance to Hz
  DebugAssert(tolerance.check(Hz.getValue()), AipsError);
  const Double tolInHz = tolerance.getValue(Hz);
  // Main matching loop
  if (tryRow >= 0) {
    const uInt tr = tryRow;
    if (tr >= r) {
      throw(AipsError("ROMSSpWindowColumns::match(...) - "
                      "the row you suggest is too big"));
    }
    if (!flagRow()(tr) &&
	matchNumChan(tr, nChan) &&
	matchIfConvChain(tr, ifChain) &&
	matchTotalBandwidth(tr, bandwidthInHz, nChan*tolInHz) &&
 	matchRefFrequency(tr, refType, refFreqInHz, tolInHz)) {
      return tr;
    }
    if (tr == r-1) r--;
  }
  while (r > 0) {
    r--;
    if (!flagRow()(r) &&
	matchNumChan(r, nChan) &&
	matchIfConvChain(r, ifChain) &&
	matchTotalBandwidth(r, bandwidthInHz, nChan*tolInHz) &&
 	matchRefFrequency(r, refType, refFreqInHz, tolInHz)) {
      return r;
    }
  }
  return -1;
}

Vector<Int> ROMSSpWindowColumns::
allMatchedSpw(const MFrequency& refFreq, uInt nChan, 
	 const Quantum<Double>& bandwidth, Int ifChain,
	 const Quantum<Double>& tolerance) const {
  uInt r = nrow();
  Vector<Int> matched;
  if (r == 0) return matched;
  // Convert the reference frequency to Hz
  const MFrequency::Types refType = 
    MFrequency::castType(refFreq.getRef().getType());
  const Double refFreqInHz = refFreq.getValue().getValue();
  // Convert the totalBandwidth to Hz
  const Unit Hz("Hz");
  DebugAssert(bandwidth.check(Hz.getValue()), AipsError);
  const Double bandwidthInHz = bandwidth.getValue(Hz);
  // Convert the tolerance to Hz
  DebugAssert(tolerance.check(Hz.getValue()), AipsError);
  const Double tolInHz = tolerance.getValue(Hz);


  Int numMatch=0;
  for (Int k=0; k < r; ++k){
    
    if (!flagRow()(k) &&
	matchNumChan(k, nChan) &&
	matchIfConvChain(k, ifChain) &&
	matchTotalBandwidth(k, bandwidthInHz, nChan*tolInHz) &&
 	matchRefFrequency(k, refType, refFreqInHz, tolInHz)) {
      ++numMatch;
      matched.resize(numMatch, True);
      matched(numMatch-1)=k;
    }

  }

  return matched;

}


Int ROMSSpWindowColumns::
matchSpw(const MFrequency& refFreq, uInt nChan, 
	 const Quantum<Double>& bandwidth, Int ifChain,
	 const Quantum<Double>& tolerance, Vector<Double>& otherFreqs, 
	 Bool& reversed) const {

  reversed=False;
  
  Int matchedSpw=-1;

  Vector<Int> allMatchSpw=
    allMatchedSpw(refFreq, nChan, bandwidth, ifChain, tolerance);
 
  Int nMatches=allMatchSpw.shape()(0);
  if(nMatches==0) return -1;



  // if only one channel then return the first match
  if (nChan == 1) return allMatchSpw[0];
  Double tolInHz= tolerance.get("Hz").getValue();
  for (Int k=0; k < nMatches; ++k){

    matchedSpw=allMatchSpw[k];
      
    if(matchChanFreq(matchedSpw, otherFreqs, tolInHz)){ 
      return matchedSpw;
    }
    else{ 
      Vector<Double> reverseFreq(otherFreqs.shape());
      for (uInt k=0; k < nChan ; ++k){
	reverseFreq[k]=otherFreqs[nChan-1-k];
      }
      if(matchChanFreq(matchedSpw, reverseFreq, tolInHz)){
	reversed=True;
	return matchedSpw;
      }

    }

  }
 

  return -1;
}
ROMSSpWindowColumns::ROMSSpWindowColumns():
  chanFreq_p(),
  chanWidth_p(),
  effectiveBW_p(),
  flagRow_p(),
  freqGroup_p(),
  freqGroupName_p(),
  ifConvChain_p(),
  measFreqRef_p(),
  name_p(),
  netSideband_p(),
  numChan_p(),
  refFrequency_p(),
  resolution_p(),
  totalBandwidth_p(),
  assocNature_p(),
  assocSpwId_p(),
  bbcNo_p(),
  bbcSideband_p(),
  dopplerId_p(),
  receiverId_p(),
  chanFreqMeas_p(),
  refFrequencyMeas_p(),
  chanFreqQuant_p(),
  chanWidthQuant_p(),
  effectiveBWQuant_p(),
  refFrequencyQuant_p(),
  resolutionQuant_p(),
  totalBandwidthQuant_p()
{
}

void ROMSSpWindowColumns::attach(const MSSpectralWindow& msSpWindow)
{
  chanFreq_p.attach(msSpWindow, MSSpectralWindow::
		    columnName(MSSpectralWindow::CHAN_FREQ));
  chanWidth_p.attach(msSpWindow, MSSpectralWindow::
		     columnName(MSSpectralWindow::CHAN_WIDTH));
  effectiveBW_p.attach(msSpWindow, MSSpectralWindow::
		       columnName(MSSpectralWindow::EFFECTIVE_BW));
  flagRow_p.attach(msSpWindow, MSSpectralWindow::
		   columnName(MSSpectralWindow::FLAG_ROW));
  freqGroup_p.attach(msSpWindow, MSSpectralWindow::
		     columnName(MSSpectralWindow::FREQ_GROUP));
  freqGroupName_p.attach(msSpWindow, MSSpectralWindow::
			 columnName(MSSpectralWindow::FREQ_GROUP_NAME));
  ifConvChain_p.attach(msSpWindow, MSSpectralWindow::
		       columnName(MSSpectralWindow::IF_CONV_CHAIN));
  measFreqRef_p.attach(msSpWindow, MSSpectralWindow::
		       columnName(MSSpectralWindow::MEAS_FREQ_REF));
  name_p.attach(msSpWindow, MSSpectralWindow::
		columnName(MSSpectralWindow::NAME));
  netSideband_p.attach(msSpWindow, MSSpectralWindow::
		       columnName(MSSpectralWindow::NET_SIDEBAND));
  numChan_p.attach(msSpWindow, MSSpectralWindow::
		   columnName(MSSpectralWindow::NUM_CHAN));
  refFrequency_p.attach(msSpWindow, MSSpectralWindow::
			columnName(MSSpectralWindow::REF_FREQUENCY));
  resolution_p.attach(msSpWindow, MSSpectralWindow::
		      columnName(MSSpectralWindow::RESOLUTION));
  totalBandwidth_p.attach(msSpWindow, MSSpectralWindow::
			  columnName(MSSpectralWindow::TOTAL_BANDWIDTH));
  chanFreqMeas_p.attach(msSpWindow, MSSpectralWindow::
			columnName(MSSpectralWindow::CHAN_FREQ));
  refFrequencyMeas_p.attach(msSpWindow, MSSpectralWindow::
			    columnName(MSSpectralWindow::REF_FREQUENCY));
  chanFreqQuant_p.attach(msSpWindow, MSSpectralWindow::
			 columnName(MSSpectralWindow::CHAN_FREQ));
  chanWidthQuant_p.attach(msSpWindow, MSSpectralWindow::
			  columnName(MSSpectralWindow::CHAN_WIDTH));
  effectiveBWQuant_p.attach(msSpWindow, MSSpectralWindow::
			    columnName(MSSpectralWindow::EFFECTIVE_BW));
  refFrequencyQuant_p.attach(msSpWindow, MSSpectralWindow::
			     columnName(MSSpectralWindow::REF_FREQUENCY));
  resolutionQuant_p.attach(msSpWindow, MSSpectralWindow::
			   columnName(MSSpectralWindow::RESOLUTION));
  totalBandwidthQuant_p.attach(msSpWindow, MSSpectralWindow::
			       columnName(MSSpectralWindow::TOTAL_BANDWIDTH));
  attachOptionalCols(msSpWindow);
}

void ROMSSpWindowColumns::
attachOptionalCols(const MSSpectralWindow& msSpWindow)
{
  const ColumnDescSet& cds=msSpWindow.tableDesc().columnDescSet();
  const String& assocNature=
    MSSpectralWindow::columnName(MSSpectralWindow::ASSOC_NATURE);
  if (cds.isDefined(assocNature)) assocNature_p.attach(msSpWindow,assocNature);
  const String& assocSpwId=
    MSSpectralWindow::columnName(MSSpectralWindow::ASSOC_SPW_ID);
  if (cds.isDefined(assocSpwId)) assocSpwId_p.attach(msSpWindow,assocSpwId);
  const String& bbcNo=
    MSSpectralWindow::columnName(MSSpectralWindow::BBC_NO);
  if (cds.isDefined(bbcNo)) bbcNo_p.attach(msSpWindow,bbcNo);
  const String& bbcSideband=
    MSSpectralWindow::columnName(MSSpectralWindow::BBC_SIDEBAND);
  if (cds.isDefined(bbcSideband)) bbcSideband_p.attach(msSpWindow,bbcSideband);
  const String& dopplerId=
    MSSpectralWindow::columnName(MSSpectralWindow::DOPPLER_ID);
  if (cds.isDefined(dopplerId)) dopplerId_p.attach(msSpWindow,dopplerId);
  const String& receiverId=
    MSSpectralWindow::columnName(MSSpectralWindow::RECEIVER_ID);
  if (cds.isDefined(receiverId)) receiverId_p.attach(msSpWindow,receiverId);
}

Bool ROMSSpWindowColumns::
matchRefFrequency(uInt row, MFrequency::Types refType, 
		  Double refFreqInHz, Double tolInHz) const {
  DebugAssert(row < nrow(), AipsError);
  const MFrequency rowFreq = refFrequencyMeas()(row);
  if (MFrequency::castType(rowFreq.getRef().getType()) != refType) {
    return False;
  }
  return nearAbs(rowFreq.getValue().getValue(), refFreqInHz, tolInHz);
}

Bool ROMSSpWindowColumns::
matchChanFreq(uInt row, const Vector<Double>& chanFreqInHz,
	      Double tolInHz) const {
  DebugAssert(row < nrow(), AipsError);
  DebugAssert(chanFreq().ndim(row) == 1, AipsError);
  // Check the number of channels
  const uInt nChan = chanFreq().shape(row)(0);
  if (nChan != chanFreqInHz.nelements()) return False;
  // Check the values in each channel
  return allNearAbs(chanFreq()(row), chanFreqInHz, tolInHz);
}
  
Bool ROMSSpWindowColumns::
matchIfConvChain(uInt row, Int ifChain) const {
  DebugAssert(row < nrow(), AipsError);
  return ifChain == ifConvChain()(row);
}

Bool ROMSSpWindowColumns::
matchTotalBandwidth(uInt row, Double bandwidthInHz,
		    Double tolInHz) const {
  DebugAssert(row < nrow(), AipsError);
  return nearAbs(totalBandwidth()(row), bandwidthInHz, tolInHz);
}

Bool ROMSSpWindowColumns::
matchNumChan(uInt row, Int nChan) const {
  DebugAssert(row < nrow(), AipsError);
  return nChan == numChan()(row);
}

MSSpWindowColumns::MSSpWindowColumns(MSSpectralWindow& msSpWindow):
  ROMSSpWindowColumns(msSpWindow),
  chanFreq_p(msSpWindow, MSSpectralWindow::
	     columnName(MSSpectralWindow::CHAN_FREQ)),
  chanWidth_p(msSpWindow, MSSpectralWindow::
	      columnName(MSSpectralWindow::CHAN_WIDTH)),
  effectiveBW_p(msSpWindow, MSSpectralWindow::
		columnName(MSSpectralWindow::EFFECTIVE_BW)),
  flagRow_p(msSpWindow, MSSpectralWindow::
	    columnName(MSSpectralWindow::FLAG_ROW)),
  freqGroup_p(msSpWindow, MSSpectralWindow::
	      columnName(MSSpectralWindow::FREQ_GROUP)),
  freqGroupName_p(msSpWindow, MSSpectralWindow::
		  columnName(MSSpectralWindow::FREQ_GROUP_NAME)),
  ifConvChain_p(msSpWindow, MSSpectralWindow::
		columnName(MSSpectralWindow::IF_CONV_CHAIN)),
  measFreqRef_p(msSpWindow, MSSpectralWindow::
		columnName(MSSpectralWindow::MEAS_FREQ_REF)),
  name_p(msSpWindow, MSSpectralWindow::
	 columnName(MSSpectralWindow::NAME)),
  netSideband_p(msSpWindow, MSSpectralWindow::
		columnName(MSSpectralWindow::NET_SIDEBAND)),
  numChan_p(msSpWindow, MSSpectralWindow::
	    columnName(MSSpectralWindow::NUM_CHAN)),
  refFrequency_p(msSpWindow, MSSpectralWindow::
		 columnName(MSSpectralWindow::REF_FREQUENCY)),
  resolution_p(msSpWindow, MSSpectralWindow::
	       columnName(MSSpectralWindow::RESOLUTION)),
  totalBandwidth_p(msSpWindow, MSSpectralWindow::
		   columnName(MSSpectralWindow::TOTAL_BANDWIDTH)),
  assocNature_p(),
  assocSpwId_p(),
  bbcNo_p(),
  bbcSideband_p(),
  dopplerId_p(),
  receiverId_p(),
  chanFreqMeas_p(msSpWindow, MSSpectralWindow::
		 columnName(MSSpectralWindow::CHAN_FREQ)),
  refFrequencyMeas_p(msSpWindow, MSSpectralWindow::
		     columnName(MSSpectralWindow::REF_FREQUENCY)),
  chanFreqQuant_p(msSpWindow, MSSpectralWindow::
		  columnName(MSSpectralWindow::CHAN_FREQ)),
  chanWidthQuant_p(msSpWindow, MSSpectralWindow::
		   columnName(MSSpectralWindow::CHAN_WIDTH)),
  effectiveBWQuant_p(msSpWindow, MSSpectralWindow::
		     columnName(MSSpectralWindow::EFFECTIVE_BW)),
  refFrequencyQuant_p(msSpWindow, MSSpectralWindow::
		      columnName(MSSpectralWindow::REF_FREQUENCY)),
  resolutionQuant_p(msSpWindow, MSSpectralWindow::
		    columnName(MSSpectralWindow::RESOLUTION)),
  totalBandwidthQuant_p(msSpWindow, MSSpectralWindow::
			columnName(MSSpectralWindow::TOTAL_BANDWIDTH))
{
  attachOptionalCols(msSpWindow);
}

MSSpWindowColumns::~MSSpWindowColumns() {}

MSSpWindowColumns::MSSpWindowColumns():
  ROMSSpWindowColumns(),
  chanFreq_p(),
  chanWidth_p(),
  effectiveBW_p(),
  flagRow_p(),
  freqGroup_p(),
  freqGroupName_p(),
  ifConvChain_p(),
  measFreqRef_p(),
  name_p(),
  netSideband_p(),
  numChan_p(),
  refFrequency_p(),
  resolution_p(),
  totalBandwidth_p(),
  assocNature_p(),
  assocSpwId_p(),
  bbcNo_p(),
  bbcSideband_p(),
  dopplerId_p(),
  receiverId_p(),
  chanFreqMeas_p(),
  refFrequencyMeas_p(),
  chanFreqQuant_p(),
  chanWidthQuant_p(),
  effectiveBWQuant_p(),
  refFrequencyQuant_p(),
  resolutionQuant_p(),
  totalBandwidthQuant_p()
{
}

void MSSpWindowColumns::attach(MSSpectralWindow& msSpWindow)
{
  ROMSSpWindowColumns::attach(msSpWindow);
  chanFreq_p.attach(msSpWindow, MSSpectralWindow::
		    columnName(MSSpectralWindow::CHAN_FREQ));
  chanWidth_p.attach(msSpWindow, MSSpectralWindow::
		     columnName(MSSpectralWindow::CHAN_WIDTH));
  effectiveBW_p.attach(msSpWindow, MSSpectralWindow::
		       columnName(MSSpectralWindow::EFFECTIVE_BW));
  flagRow_p.attach(msSpWindow, MSSpectralWindow::
		   columnName(MSSpectralWindow::FLAG_ROW));
  freqGroup_p.attach(msSpWindow, MSSpectralWindow::
		     columnName(MSSpectralWindow::FREQ_GROUP));
  freqGroupName_p.attach(msSpWindow, MSSpectralWindow::
			 columnName(MSSpectralWindow::FREQ_GROUP_NAME));
  ifConvChain_p.attach(msSpWindow, MSSpectralWindow::
		       columnName(MSSpectralWindow::IF_CONV_CHAIN));
  measFreqRef_p.attach(msSpWindow, MSSpectralWindow::
		       columnName(MSSpectralWindow::MEAS_FREQ_REF));
  name_p.attach(msSpWindow, MSSpectralWindow::
		columnName(MSSpectralWindow::NAME));
  netSideband_p.attach(msSpWindow, MSSpectralWindow::
		       columnName(MSSpectralWindow::NET_SIDEBAND));
  numChan_p.attach(msSpWindow, MSSpectralWindow::
		   columnName(MSSpectralWindow::NUM_CHAN));
  refFrequency_p.attach(msSpWindow, MSSpectralWindow::
			columnName(MSSpectralWindow::REF_FREQUENCY));
  resolution_p.attach(msSpWindow, MSSpectralWindow::
		      columnName(MSSpectralWindow::RESOLUTION));
  totalBandwidth_p.attach(msSpWindow, MSSpectralWindow::
			  columnName(MSSpectralWindow::TOTAL_BANDWIDTH));
  chanFreqMeas_p.attach(msSpWindow, MSSpectralWindow::
			columnName(MSSpectralWindow::CHAN_FREQ));
  refFrequencyMeas_p.attach(msSpWindow, MSSpectralWindow::
			    columnName(MSSpectralWindow::REF_FREQUENCY));
  chanFreqQuant_p.attach(msSpWindow, MSSpectralWindow::
			 columnName(MSSpectralWindow::CHAN_FREQ));
  chanWidthQuant_p.attach(msSpWindow, MSSpectralWindow::
			  columnName(MSSpectralWindow::CHAN_WIDTH));
  effectiveBWQuant_p.attach(msSpWindow, MSSpectralWindow::
			    columnName(MSSpectralWindow::EFFECTIVE_BW));
  refFrequencyQuant_p.attach(msSpWindow, MSSpectralWindow::
			     columnName(MSSpectralWindow::REF_FREQUENCY));
  resolutionQuant_p.attach(msSpWindow, MSSpectralWindow::
			   columnName(MSSpectralWindow::RESOLUTION));
  totalBandwidthQuant_p.attach(msSpWindow, MSSpectralWindow::
			       columnName(MSSpectralWindow::TOTAL_BANDWIDTH));
  attachOptionalCols(msSpWindow);
}

void MSSpWindowColumns::
attachOptionalCols(MSSpectralWindow& msSpWindow)
{
  const ColumnDescSet& cds=msSpWindow.tableDesc().columnDescSet();
  const String& assocNature=
    MSSpectralWindow::columnName(MSSpectralWindow::ASSOC_NATURE);
  if (cds.isDefined(assocNature)) assocNature_p.attach(msSpWindow,assocNature);
  const String& assocSpwId=
    MSSpectralWindow::columnName(MSSpectralWindow::ASSOC_SPW_ID);
  if (cds.isDefined(assocSpwId)) assocSpwId_p.attach(msSpWindow,assocSpwId);
  const String& bbcNo=
    MSSpectralWindow::columnName(MSSpectralWindow::BBC_NO);
  if (cds.isDefined(bbcNo)) bbcNo_p.attach(msSpWindow,bbcNo);
  const String& bbcSideband=
    MSSpectralWindow::columnName(MSSpectralWindow::BBC_SIDEBAND);
  if (cds.isDefined(bbcSideband)) bbcSideband_p.attach(msSpWindow,bbcSideband);
  const String& dopplerId=
    MSSpectralWindow::columnName(MSSpectralWindow::DOPPLER_ID);
  if (cds.isDefined(dopplerId)) dopplerId_p.attach(msSpWindow,dopplerId);
  const String& receiverId=
    MSSpectralWindow::columnName(MSSpectralWindow::RECEIVER_ID);
  if (cds.isDefined(receiverId)) receiverId_p.attach(msSpWindow,receiverId);
}


// Local Variables: 
// compile-command: "gmake MSSpWindowColumns"
// End: 


} //# NAMESPACE CASA - END

