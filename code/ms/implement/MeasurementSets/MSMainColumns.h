//# NewMSmainColumns.h: provides easy access to NewMeasurementSet main table columns
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

#if !defined(AIPS_NEWMSMAINCOLUMNS_H)
#define AIPS_NEWMSMAINCOLUMNS_H

#include <aips/aips.h>
#include <aips/Measures/MEpoch.h>
#include <aips/Measures/Muvw.h>
#include <aips/Tables/ScalarColumn.h>
#include <aips/Tables/ArrayColumn.h>
#include <aips/TableMeasures/ScalarMeasColumn.h>
#include <aips/TableMeasures/ScalarQuantColumn.h>
#include <aips/TableMeasures/ArrayQuantColumn.h>

class NewMeasurementSet;

// <summary>
// A class to provide easy read-only access to NewMeasurementSet main table columns
// </summary>

// <use visibility=export>

// <reviewed reviewer="Bob Garwood" date="1997/02/01" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> NewMeasurementSet
//   <li> ArrayColumn
//   <li> ScalarColumn
// </prerequisite>
//
// <etymology>
// RONewMSColumns stands for Read-Only NewMeasurementSet Table columns.
// </etymology>
//
// <synopsis>
// This class provides read-only access to the columns in the NewMeasurementSet.
// It does the declaration of all the Scalar and ArrayColumns with the
// correct types, so the application programmer doesn't have to
// worry about getting those right. There is an access function
// for every predefined column. Access to non-predefined columns will still
// have to be done with explicit declarations.
// <note role=warning> The Table that is used to construct this class must not
// be destroyed (or go out of scope) before this class does. Otherwise the
// scalar and array columns use by this class will be left dangling.</note>
// </synopsis>
// </synopsis>
//
// <example>
// <srcblock>
// // use as follows
// NewMeasurementSet ms("myNewMS"); 
// RONewMSColumns msc(ms);
// // show data from row 5
// cout << msc.data()(5);
// // show name of antenna on row 3 in antenna table
// cout << msc.antenna().name();
// </srcblock>
// </example>
//
// <motivation>
// See <linkto class=NewMSColumns> NewMSColumns</linkto> for the motivation.
// </motivation>
//
// <todo asof="1997/02/01">
//   <li> We might decide to merge all the NewMSColumn classes with the
//        corresponding NewMeasurementSet classes.
// </todo>

class RONewMSMainColumns
{
public:
  // Create a columns object that accesses the data in the specified Table
  RONewMSMainColumns(const NewMeasurementSet& ms);

  // The desctructor does nothing special
  ~RONewMSMainColumns();

  // Access to required columns
  // <group>
  const ROScalarColumn<Int>& antenna1() const {return antenna1_p;}
  const ROScalarColumn<Int>& antenna2() const {return antenna2_p;}
  const ROScalarColumn<Int>& arrayId() const {return arrayId_p;}
  const ROScalarColumn<Int>& dataDescId() const {return dataDescId_p;}
  const ROScalarColumn<Double>& exposure() const {return exposure_p;}
  const ROScalarQuantColumn<Double>& exposureQuant() const { 
    return exposureQuant_p;}
  const ROScalarColumn<Int>& feed1() const {return feed1_p;}
  const ROScalarColumn<Int>& feed2() const {return feed2_p;}
  const ROScalarColumn<Int>& fieldId() const {return fieldId_p;}
  const ROArrayColumn<Bool>& flag() const {return flag_p;}
  const ROArrayColumn<Bool>& flagCategory() const {return flagCategory_p;}
  const ROScalarColumn<Bool>& flagRow() const {return flagRow_p;}
  const ROScalarColumn<Double>& interval() const {return interval_p;}
  const ROScalarQuantColumn<Double>& intervalQuant() const {
    return intervalQuant_p;}
  const ROScalarColumn<Int>& observationId() const {return observationId_p;}
  const ROScalarColumn<Int>& processorId() const {return processorId_p;}
  const ROScalarColumn<Int>& scanNumber() const {return scanNumber_p;}
  const ROArrayColumn<Float>& sigma() const {return sigma_p;}
  const ROScalarColumn<Int>& stateId() const {return stateId_p;}
  const ROScalarColumn<Double>& time() const {return time_p;}
  const ROScalarQuantColumn<Double>& timeQuant() const {
    return timeQuant_p;}
  const ROScalarMeasColumn<MEpoch>& timeMeas() const {
    return timeMeas_p;}
  const ROScalarColumn<Double>& timeCentroid() const {return timeCentroid_p;}
  const ROScalarQuantColumn<Double>& timeCentroidQuant() const {
    return timeCentroidQuant_p;}
  const ROScalarMeasColumn<MEpoch>& timeCentroidMeas() const {
    return timeCentroidMeas_p;}
  const ROArrayColumn<Double>& uvw() const {return uvw_p;}
  const ROArrayQuantColumn<Double>& uvwQuant() const {
    return uvwQuant_p;}
  const ROScalarMeasColumn<Muvw>& uvwMeas() const {
    return uvwMeas_p;}
  const ROArrayColumn<Float>& weight() const {return weight_p;}
  // </group>

  // Access to optional columns
  // <group>
  const ROScalarColumn<Int>& antenna3() const {return antenna3_p;}
  const ROScalarColumn<Bool>& baselineRef() const {return baselineRef_p;}
  const ROArrayColumn<Complex>& correctedData() const {return correctedData_p;}
  const ROArrayColumn<Complex>& data() const {return data_p;}
  const ROScalarColumn<Int>& feed3() const {return feed3_p;}
  const ROArrayColumn<Float>& floatData() const {return floatData_p;}
  const ROArrayColumn<Float>& imagingWeight() const {return imagingWeight_p;}
  const ROArrayColumn<Complex>& lagData() const {return lagData_p;}
  const ROArrayColumn<Complex>& modelData() const {return modelData_p;}
  const ROScalarColumn<Int>& phaseId() const {return phaseId_p;}
  const ROScalarColumn<Int>& pulsarBin() const {return pulsarBin_p;}
  const ROScalarColumn<Int>& pulsarGateId() const {return pulsarGateId_p;}
  const ROArrayColumn<Float>& sigmaSpectrum() const {return sigmaSpectrum_p;}
  const ROScalarColumn<Double>& timeExtraPrec() const {return timeExtraPrec_p;}
  const ROScalarQuantColumn<Double>& timeExtraPrecQuant() const {
    return timeExtraPrecQuant_p;}
  const ROArrayColumn<Double>& uvw2() const {return uvw2_p;}
  const ROScalarMeasColumn<Muvw>& uvw2Meas() const {
    return uvw2Meas_p;}
  const ROArrayQuantColumn<Double>& uvw2Quant() const {
    return uvw2Quant_p;}
  const ROArrayColumn<Complex>& videoPoint() const {return videoPoint_p;}
  const ROArrayColumn<Float>& weightSpectrum() const {return weightSpectrum_p;}
  // </group>

  // Convenience function that returns the number of rows in any of the columns
  uInt nrow() const {return antenna1_p.nrow();}
  
protected:
  //# default constructor creates a object that is not usable. Use the attach
  //# function correct this.
  RONewMSMainColumns();

  //# attach this object to the supplied table.
  void attach(const NewMeasurementSet& ms);

private:
  //# Make the assignment operator and the copy constructor private to prevent
  //# any compiler generated one from being used.
  RONewMSMainColumns(const RONewMSMainColumns&);
  RONewMSMainColumns& operator=(const RONewMSMainColumns&);

  //# Check if any optional columns exist and if so attach them.
  void attachOptionalCols(const NewMeasurementSet& ms);
  
  //# required columns
  ROScalarColumn<Int> antenna1_p;
  ROScalarColumn<Int> antenna2_p;
  ROScalarColumn<Int> arrayId_p;
  ROScalarColumn<Int> dataDescId_p;
  ROScalarColumn<Double> exposure_p;
  ROScalarColumn<Int> feed1_p;
  ROScalarColumn<Int> feed2_p;
  ROScalarColumn<Int> fieldId_p;
  ROArrayColumn<Bool> flag_p;
  ROArrayColumn<Bool> flagCategory_p;
  ROScalarColumn<Bool> flagRow_p;
  ROScalarColumn<Double> interval_p;
  ROScalarColumn<Int> observationId_p;
  ROScalarColumn<Int> processorId_p;
  ROScalarColumn<Int> scanNumber_p;
  ROArrayColumn<Float> sigma_p;
  ROScalarColumn<Int> stateId_p;
  ROScalarColumn<Double> time_p;
  ROScalarColumn<Double> timeCentroid_p;
  ROArrayColumn<Double> uvw_p;
  ROArrayColumn<Float> weight_p;
  //# optional columns
  ROScalarColumn<Int> antenna3_p;
  ROScalarColumn<Bool> baselineRef_p;
  ROArrayColumn<Complex> data_p;
  ROScalarColumn<Int> feed3_p;
  ROArrayColumn<Float> floatData_p;
  ROArrayColumn<Complex> lagData_p;
  ROScalarColumn<Int> phaseId_p;
  ROScalarColumn<Int> pulsarBin_p;
  ROScalarColumn<Int> pulsarGateId_p;
  ROArrayColumn<Float> sigmaSpectrum_p;
  ROScalarColumn<Double> timeExtraPrec_p;
  ROArrayColumn<Double> uvw2_p;
  ROArrayColumn<Complex> videoPoint_p;
  ROArrayColumn<Float> weightSpectrum_p;

  //# columns required for synthesis applications - all optional
  ROArrayColumn<Complex> correctedData_p;
  ROArrayColumn<Float> imagingWeight_p;
  ROArrayColumn<Complex> modelData_p;

  //# Access to Measure columns
  ROScalarMeasColumn<MEpoch> timeMeas_p;
  ROScalarMeasColumn<MEpoch> timeCentroidMeas_p;
  ROScalarMeasColumn<Muvw> uvwMeas_p;
  //# optional Measure columns
  ROScalarMeasColumn<Muvw> uvw2Meas_p;

  //# Access to Quantum columns
  ROScalarQuantColumn<Double> exposureQuant_p;
  ROScalarQuantColumn<Double> intervalQuant_p;
  ROScalarQuantColumn<Double> timeQuant_p;
  ROScalarQuantColumn<Double> timeCentroidQuant_p;
  ROArrayQuantColumn<Double> uvwQuant_p;
  //# optional Quantum columns
  ROScalarQuantColumn<Double> timeExtraPrecQuant_p;
  ROArrayQuantColumn<Double> uvw2Quant_p;

};

// <summary>
// A class to provide easy read-write access to NewMeasurementSet main table columns
// </summary>

// <use visibility=export>

// <reviewed reviewer="Bob Garwood" date="1997/02/01" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> NewMeasurementSet
//   <li> ArrayColumn
//   <li> ScalarColumn
// </prerequisite>
//
// <etymology>
// NewMSMainColumns stands for NewMeasurementSet main Table columns.
// </etymology>
//
// <synopsis>
// This class provides access to the columns in the NewMeasurementSet.
// It does the declaration of all the Scalar and ArrayColumns with the
// correct types, so the application programmer doesn't have to
// worry about getting those right. There is an access function
// for every predefined column. Access to non-predefined columns will still
// have to be done with explicit declarations.
// <note role=warning> The Table that is used to construct this class must not
// be destroyed (or go out of scope) before this class does. Otherwise the
// scalar and array columns use by this class will be left dangling.</note>
// </synopsis>
// </synopsis>
//
// <example>
// <srcblock>
// // use as follows
// NewMeasurementSet ms("myNewMS",Table::Update); 
// NewMSColumns msc(ms);
// // show data from row 5
// cout << msc.data()(5);
// // change name of antenna on row 3 in antenna table
// msc.antenna().name().put(3,"NewAnt-3");
// </srcblock>
// </example>
//
// <motivation>
// Having to type long lists of Scalar and Array column declarations gets
// very tedious. This class attempts to relieve some of that tedium, while
// at the same time concentrating all the declarations in one place,
// making Type errors in the column declaration (only caught at run-time) less
// probable. Type errors in the use of the columns is caught at compile
// time.
// </motivation>
//
// <todo asof="1997/02/01">
//   <li> We might decide to merge this class with the NewMeasurementSet
// </todo>

class NewMSMainColumns: public RONewMSMainColumns
{
public:

  // Create a columns object that accesses the data in the specified Table
  NewMSMainColumns(NewMeasurementSet& ms);

  // The desctructor does nothing special
  ~NewMSMainColumns();

  // Read-write access to required columns
  // <group>
  ScalarColumn<Int>& antenna1() {return antenna1_p;}
  ScalarColumn<Int>& antenna2() {return antenna2_p;}
  ScalarColumn<Int>& arrayId() {return arrayId_p;}
  ScalarColumn<Int>& dataDescId() {return dataDescId_p;}
  ScalarColumn<Double>& exposure() {return exposure_p;}
  ScalarQuantColumn<Double>& exposureQuant() { 
    return exposureQuant_p;}
  ScalarColumn<Int>& feed1() {return feed1_p;}
  ScalarColumn<Int>& feed2() {return feed2_p;}
  ScalarColumn<Int>& fieldId() {return fieldId_p;}
  ArrayColumn<Bool>& flag() {return flag_p;}
  ArrayColumn<Bool>& flagCategory() {return flagCategory_p;}
  ScalarColumn<Bool>& flagRow() {return flagRow_p;}
  ScalarColumn<Double>& interval() {return interval_p;}
  ScalarQuantColumn<Double>& intervalQuant() {
    return intervalQuant_p;}
  ScalarColumn<Int>& observationId() {return observationId_p;}
  ScalarColumn<Int>& processorId() {return processorId_p;}
  ScalarColumn<Int>& scanNumber() {return scanNumber_p;}
  ArrayColumn<Float>& sigma() {return sigma_p;}
  ScalarColumn<Int>& stateId() {return stateId_p;}
  ScalarColumn<Double>& time() {return time_p;}
  ScalarQuantColumn<Double>& timeQuant() {
    return timeQuant_p;}
  ScalarMeasColumn<MEpoch>& timeMeas() {
    return timeMeas_p;}
  ScalarColumn<Double>& timeCentroid() {return timeCentroid_p;}
  ScalarQuantColumn<Double>& timeCentroidQuant() {
    return timeCentroidQuant_p;}
  ScalarMeasColumn<MEpoch>& timeCentroidMeas() {
    return timeCentroidMeas_p;}
  ArrayColumn<Double>& uvw() {return uvw_p;}
  ArrayQuantColumn<Double>& uvwQuant() {
    return uvwQuant_p;}
  ScalarMeasColumn<Muvw>& uvwMeas() {
    return uvwMeas_p;}
  ArrayColumn<Float>& weight() {return weight_p;}
  // </group>

  // Read-write access to optional columns
  // <group>
  ScalarColumn<Int>& antenna3() {return antenna3_p;}
  ScalarColumn<Bool>& baselineRef() {return baselineRef_p;}
  ArrayColumn<Complex>& correctedData() {return correctedData_p;}
  ArrayColumn<Complex>& data() {return data_p;}
  ScalarColumn<Int>& feed3() {return feed3_p;}
  ArrayColumn<Float>& floatData() {return floatData_p;}
  ArrayColumn<Float>& imagingWeight() {return imagingWeight_p;}
  ArrayColumn<Complex>& lagData() {return lagData_p;}
  ArrayColumn<Complex>& modelData() {return modelData_p;}
  ScalarColumn<Int>& phaseId() {return phaseId_p;}
  ScalarColumn<Int>& pulsarBin() {return pulsarBin_p;}
  ScalarColumn<Int>& pulsarGateId() {return pulsarGateId_p;}
  ArrayColumn<Float>& sigmaSpectrum() {return sigmaSpectrum_p;}
  ScalarColumn<Double>& timeExtraPrec() {return timeExtraPrec_p;}
  ScalarQuantColumn<Double>& timeExtraPrecQuant() {
    return timeExtraPrecQuant_p;}
  ArrayColumn<Double>& uvw2() {return uvw2_p;}
  ScalarMeasColumn<Muvw>& uvw2Meas() {
    return uvw2Meas_p;}
  ArrayQuantColumn<Double>& uvw2Quant() {
    return uvw2Quant_p;}
  ArrayColumn<Complex>& videoPoint() {return videoPoint_p;}
  ArrayColumn<Float>& weightSpectrum() {return weightSpectrum_p;}
  // </group>

  // Read-only access to required columns
  // <group>
  const ROScalarColumn<Int>& antenna1() const {
    return RONewMSMainColumns::antenna1();}
  const ROScalarColumn<Int>& antenna2() const {
    return RONewMSMainColumns::antenna2();}
  const ROScalarColumn<Int>& arrayId() const {
    return RONewMSMainColumns::arrayId();}
  const ROScalarColumn<Int>& dataDescId() const {
    return RONewMSMainColumns::dataDescId();}
  const ROScalarColumn<Double>& exposure() const {
    return RONewMSMainColumns::exposure();}
  const ROScalarQuantColumn<Double>& exposureQuant() const { 
    return RONewMSMainColumns::exposureQuant();}
  const ROScalarColumn<Int>& feed1() const {
    return RONewMSMainColumns::feed1();}
  const ROScalarColumn<Int>& feed2() const {
    return RONewMSMainColumns::feed2();}
  const ROScalarColumn<Int>& fieldId() const {
    return RONewMSMainColumns::fieldId();}
  const ROArrayColumn<Bool>& flag() const {
    return RONewMSMainColumns::flag();}
  const ROArrayColumn<Bool>& flagCategory() const {
    return RONewMSMainColumns::flagCategory();}
  const ROScalarColumn<Bool>& flagRow() const {
    return RONewMSMainColumns::flagRow();}
  const ROScalarColumn<Double>& interval() const {
    return RONewMSMainColumns::interval();}
  const ROScalarQuantColumn<Double>& intervalQuant() const {
    return RONewMSMainColumns::intervalQuant();}
  const ROScalarColumn<Int>& observationId() const {
    return RONewMSMainColumns::observationId();}
  const ROScalarColumn<Int>& processorId() const {
    return RONewMSMainColumns::processorId();}
  const ROScalarColumn<Int>& scanNumber() const {
    return RONewMSMainColumns::scanNumber();}
  const ROArrayColumn<Float>& sigma() const {
    return RONewMSMainColumns::sigma();}
  const ROScalarColumn<Int>& stateId() const {
    return RONewMSMainColumns::stateId();}
  const ROScalarColumn<Double>& time() const {
    return RONewMSMainColumns::time();}
  const ROScalarQuantColumn<Double>& timeQuant() const {
    return RONewMSMainColumns::timeQuant();}
  const ROScalarMeasColumn<MEpoch>& timeMeas() const {
    return RONewMSMainColumns::timeMeas();}
  const ROScalarColumn<Double>& timeCentroid() const {
    return RONewMSMainColumns::timeCentroid();}
  const ROScalarQuantColumn<Double>& timeCentroidQuant() const {
    return RONewMSMainColumns::timeCentroidQuant();}
  const ROScalarMeasColumn<MEpoch>& timeCentroidMeas() const {
    return RONewMSMainColumns::timeCentroidMeas();}
  const ROArrayColumn<Double>& uvw() const {
    return RONewMSMainColumns::uvw();}
  const ROArrayQuantColumn<Double>& uvwQuant() const {
    return RONewMSMainColumns::uvwQuant();}
  const ROScalarMeasColumn<Muvw>& uvwMeas() const {
    return RONewMSMainColumns::uvwMeas();}
  const ROArrayColumn<Float>& weight() const {
    return RONewMSMainColumns::weight();}
  // </group>

  // Read-only access to optional columns
  // <group>
  const ROScalarColumn<Int>& antenna3() const {
    return RONewMSMainColumns::antenna3();}
  const ROScalarColumn<Bool>& baselineRef() const {
    return RONewMSMainColumns::baselineRef();}
  const ROArrayColumn<Complex>& correctedData() const {
    return RONewMSMainColumns::correctedData();}
  const ROArrayColumn<Complex>& data() const {
    return RONewMSMainColumns::data();}
  const ROScalarColumn<Int>& feed3() const {
    return RONewMSMainColumns::feed3();}
  const ROArrayColumn<Float>& floatData() const {
    return RONewMSMainColumns::floatData();}
  const ROArrayColumn<Float>& imagingWeight() const {
    return RONewMSMainColumns::imagingWeight();}
  const ROArrayColumn<Complex>& lagData() const {
    return RONewMSMainColumns::lagData();}
  const ROArrayColumn<Complex>& modelData() const {
    return RONewMSMainColumns::modelData();}
  const ROScalarColumn<Int>& phaseId() const {
    return RONewMSMainColumns::phaseId();}
  const ROScalarColumn<Int>& pulsarBin() const {
    return RONewMSMainColumns::pulsarBin();}
  const ROScalarColumn<Int>& pulsarGateId() const {
    return RONewMSMainColumns::pulsarGateId();}
  const ROArrayColumn<Float>& sigmaSpectrum() const {
    return RONewMSMainColumns::sigmaSpectrum();}
  const ROScalarColumn<Double>& timeExtraPrec() const {
    return RONewMSMainColumns::timeExtraPrec();}
  const ROScalarQuantColumn<Double>& timeExtraPrecQuant() const {
    return RONewMSMainColumns::timeExtraPrecQuant();}
  const ROArrayColumn<Double>& uvw2() const {
    return RONewMSMainColumns::uvw2();}
  const ROScalarMeasColumn<Muvw>& uvw2Meas() const {
    return RONewMSMainColumns::uvw2Meas();}
  const ROArrayQuantColumn<Double>& uvw2Quant() const {
    return RONewMSMainColumns::uvw2Quant();}
  const ROArrayColumn<Complex>& videoPoint() const {
    return RONewMSMainColumns::videoPoint();}
  const ROArrayColumn<Float>& weightSpectrum() const {
    return RONewMSMainColumns::weightSpectrum();}
  // </group>

  // set the EPOCH reference type for the TIME and TIME_CENTROID columns.
  void setEpochRef(MEpoch::Types ref);

  // set the UVW reference type for the UVW and UVW2 (if defined) columns.
  void setUVWRef(Muvw::Types ref);

protected:
  //# default constructor creates a object that is not usable. Use the attach
  //# function correct this.
  NewMSMainColumns();

  //# attach this object to the supplied table.
  void attach(NewMeasurementSet& ms);

private:
  //# Make the assignment operator and the copy constructor private to prevent
  //# any compiler generated one from being used.
  NewMSMainColumns(const NewMSMainColumns&);
  NewMSMainColumns& operator=(const NewMSMainColumns&);

  //# Check if any optional columns exist and if so attach them.
  void attachOptionalCols(NewMeasurementSet& ms);
  
  //# required columns
  ScalarColumn<Int> antenna1_p;
  ScalarColumn<Int> antenna2_p;
  ScalarColumn<Int> arrayId_p;
  ScalarColumn<Int> dataDescId_p;
  ScalarColumn<Double> exposure_p;
  ScalarColumn<Int> feed1_p;
  ScalarColumn<Int> feed2_p;
  ScalarColumn<Int> fieldId_p;
  ArrayColumn<Bool> flag_p;
  ArrayColumn<Bool> flagCategory_p;
  ScalarColumn<Bool> flagRow_p;
  ScalarColumn<Double> interval_p;
  ScalarColumn<Int> observationId_p;
  ScalarColumn<Int> processorId_p;
  ScalarColumn<Int> scanNumber_p;
  ArrayColumn<Float> sigma_p;
  ScalarColumn<Int> stateId_p;
  ScalarColumn<Double> time_p;
  ScalarColumn<Double> timeCentroid_p;
  ArrayColumn<Double> uvw_p;
  ArrayColumn<Float> weight_p;
  //# optional columns
  ScalarColumn<Int> antenna3_p;
  ScalarColumn<Bool> baselineRef_p;
  ArrayColumn<Complex> data_p;
  ScalarColumn<Int> feed3_p;
  ArrayColumn<Float> floatData_p;
  ArrayColumn<Complex> lagData_p;
  ScalarColumn<Int> phaseId_p;
  ScalarColumn<Int> pulsarBin_p;
  ScalarColumn<Int> pulsarGateId_p;
  ArrayColumn<Float> sigmaSpectrum_p;
  ScalarColumn<Double> timeExtraPrec_p;
  ArrayColumn<Double> uvw2_p;
  ArrayColumn<Complex> videoPoint_p;
  ArrayColumn<Float> weightSpectrum_p;

  //# columns required for synthesis applications - all optional
  ArrayColumn<Complex> correctedData_p;
  ArrayColumn<Float> imagingWeight_p;
  ArrayColumn<Complex> modelData_p;

  //# Access to Measure columns
  ScalarMeasColumn<MEpoch> timeMeas_p;
  ScalarMeasColumn<MEpoch> timeCentroidMeas_p;
  ScalarMeasColumn<Muvw> uvwMeas_p;
  //# optional Measure columns
  ScalarMeasColumn<Muvw> uvw2Meas_p;

  //# Access to Quantum columns
  ScalarQuantColumn<Double> exposureQuant_p;
  ScalarQuantColumn<Double> intervalQuant_p;
  ScalarQuantColumn<Double> timeQuant_p;
  ScalarQuantColumn<Double> timeCentroidQuant_p;
  ArrayQuantColumn<Double> uvwQuant_p;
  //# optional Quantum columns
  ScalarQuantColumn<Double> timeExtraPrecQuant_p;
  ArrayQuantColumn<Double> uvw2Quant_p;

};

#endif


