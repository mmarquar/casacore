//# Mscolumnsc.cc:  provides easy access to NewMeasurementSet columns
//# Copyright (C) 1996,1999,2000
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

#include <aips/MeasurementSets/NewMSColumns.h>
#include <aips/MeasurementSets/NewMeasurementSet.h>

RONewMSColumns::RONewMSColumns(const NewMeasurementSet& ms):
  RONewMSMainColumns(ms),
  antenna_p(ms.antenna()),
  dataDesc_p(ms.dataDescription()),
  doppler_p(ms.doppler()),
  feed_p(ms.feed()),
  field_p(ms.field()),
  flagCmd_p(ms.flagCmd()),
  freqOffset_p(ms.freqOffset()),
  history_p(ms.history()),
  observation_p(ms.observation()),
  pointing_p(ms.pointing()),
  polarization_p(ms.polarization()),
  processor_p(ms.processor()),
  source_p(ms.source()),
  spectralWindow_p(ms.spectralWindow()),
  state_p(ms.state()),
  sysCal_p(ms.sysCal()),
  weather_p(ms.weather())
{
}

RONewMSColumns::~RONewMSColumns() {}

NewMSColumns::NewMSColumns(NewMeasurementSet& ms):
  NewMSMainColumns(ms),
  antenna_p(ms.antenna()),
  dataDesc_p(ms.dataDescription()),
  doppler_p(ms.doppler()),
  feed_p(ms.feed()),
  field_p(ms.field()),
  flagCmd_p(ms.flagCmd()),
  freqOffset_p(ms.freqOffset()),
  history_p(ms.history()),
  observation_p(ms.observation()),
  pointing_p(ms.pointing()),
  polarization_p(ms.polarization()),
  processor_p(ms.processor()),
  source_p(ms.source()),
  spectralWindow_p(ms.spectralWindow()),
  state_p(ms.state()),
  sysCal_p(ms.sysCal()),
  weather_p(ms.weather())
{
}

NewMSColumns::~NewMSColumns() {}

void NewMSColumns::setEpochRef(MEpoch::Types ref)
{
  // Adjust the relevant columns in the main table
  NewMSMainColumns::setEpochRef(ref);
  // Now the same for the subtables.
  feed().setEpochRef(ref);
  field().setEpochRef(ref);
  flagCmd().setEpochRef(ref);
  history().setEpochRef(ref);
  observation().setEpochRef(ref);
  pointing().setEpochRef(ref);
  if (!freqOffset_p.isNull()) {
    freqOffset_p.setEpochRef(ref);
  }
  if (!source_p.isNull()) {
    source().setEpochRef(ref);
  }
  if (!sysCal_p.isNull()) {
    sysCal_p.setEpochRef(ref);
  }
  if (!weather_p.isNull()) {
    weather_p.setEpochRef(ref);
  }
}

void NewMSColumns::setDirectionRef(MDirection::Types ref)
{
  field().setDirectionRef(ref);
  pointing().setDirectionRef(ref);
  if (!source_p.isNull()) {
    source().setDirectionRef(ref);
  }
}
// Local Variables: 
// compile-command: "gmake NewMSColumns"
// End: 
