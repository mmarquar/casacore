//# tObsInfo.cc: test program for class ObsInfo
//# Copyright (C) 1998,2000,2001
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

#include <trial/Coordinates/ObsInfo.h>

#include <aips/Arrays/ArrayUtil.h>
#include <aips/Utilities/Assert.h>
#include <aips/Mathematics/Math.h>
#include <aips/Containers/Record.h>

#include <strstream.h>


int main()
{
// Default constructor

    ObsInfo oi;

// Default values

    AlwaysAssertExit(oi.telescope() == oi.defaultTelescope());
    AlwaysAssertExit(oi.defaultTelescope() == "UNKNOWN");
//
    AlwaysAssertExit(oi.observer() == oi.defaultObserver());
    AlwaysAssertExit(oi.defaultObserver() == "UNKNOWN");
//
    MEpoch dflt = oi.defaultObsDate();
    AlwaysAssertExit(near(oi.obsDate().get("s").getValue(), 0.0));
//
    AlwaysAssertExit(oi.isPointingCenterInitial());
    MVDirection dmvd = oi.defaultPointingCenter();
    Vector<Double> v = dmvd.get();
    AlwaysAssertExit(near(v(0), 0.0));
    AlwaysAssertExit(near(v(1), 0.0));

// Set items

    oi.setTelescope("telescope").setObserver("observer").
	setObsDate(MVEpoch(1234.0)).setPointingCenter(MVDirection(0.01, 0.02));
    AlwaysAssertExit(oi.telescope() == "telescope" &&
		     oi.observer() == "observer" &&
		     near(oi.obsDate().get("d").getValue(), 1234.0) &&
                     near(oi.pointingCenter().get()(0),0.01) &&
                     near(oi.pointingCenter().get()(1),0.02));
    AlwaysAssertExit(!oi.isPointingCenterInitial());

// Copy constructor and assignment

    ObsInfo oi2(oi);
    AlwaysAssertExit(oi2.telescope() == "telescope" &&
		     oi2.observer() == "observer" &&
		     near(oi2.obsDate().get("d").getValue(), 1234.0) &&
                     near(oi2.pointingCenter().get()(0),0.01) &&
                     near(oi2.pointingCenter().get()(1),0.02));
    AlwaysAssertExit(!oi2.isPointingCenterInitial());

//
    Double dateVal = 55000.5;
    oi2.setTelescope("telescope2").setObserver("observer2").
	setObsDate(MVEpoch(dateVal)).setPointingCenter(MVDirection(0.03,0.04));
    oi = oi2;
    AlwaysAssertExit(oi.telescope() == "telescope2" &&
		     oi.observer() == "observer2" &&
		     near(oi.obsDate().get("d").getValue(), dateVal) &&
                     near(oi.pointingCenter().get()(0),0.03) &&
                     near(oi.pointingCenter().get()(1),0.04));
    AlwaysAssertExit(!oi.isPointingCenterInitial());

// Test output.  

    ostrstream oss;
    oss << oi << endl;
    String x(oss);
//
    String x1("Telescope: telescope2 Observer: observer2 ");
    String x2("Date Observed: Epoch: 55000::12:00:00.0000 ");
    String x3("Pointing Center: [0.998751, 0.0299715, 0.0399893]");
    String x4 = x1 + x2 + x3;
    Int iL = x4.length();
    String x5 = String(x.at(0,iL));
    AlwaysAssertExit(x5==x4);

// Record interface

    Record rec;
    String error;
    AlwaysAssertExit(oi.toRecord(error, rec));
    ObsInfo oi3;
    AlwaysAssertExit(oi3.fromRecord(error, rec));
    AlwaysAssertExit(oi3.telescope() == "telescope2" &&
		     oi3.observer() == "observer2" &&
		     near(oi3.obsDate().get("d").getValue(), dateVal) &&
                     near(oi3.pointingCenter().get()(0),0.03) &&
                     near(oi3.pointingCenter().get()(1),0.04));

// FITS

    Vector<String> error2;
    Record rec2;
    AlwaysAssertExit(oi.toFITS(error, rec2));
    ObsInfo oi4;
    AlwaysAssertExit(oi4.fromFITS(error2, rec2));
    AlwaysAssertExit(oi4.telescope() == "telescope2" &&
		     oi4.observer() == "observer2" &&
		     near(oi4.obsDate().get("d").getValue(), dateVal) &&
                     near(oi4.pointingCenter().get()(0),0.03) &&
                     near(oi4.pointingCenter().get()(1),0.04));

// This is a fragile test, but probably better to do it than not.

    Vector<String> vs = ObsInfo::keywordNamesFITS();
    AlwaysAssertExit(vs(0) == "telescop" &&
		     vs(1) == "observer" &&
		     vs(2) == "date-obs" &&
		     vs(3) == "timesys" &&
		     vs(4) == "obsra" &&
		     vs(5) == "obsdec");
//
    cout << "OK" << endl;
    return 0;
}
