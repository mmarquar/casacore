//# tMeasComet.cc: MeasComet test
//# Copyright (C) 2000
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
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
#include <aips/aips.h>
#include <aips/Measures.h>
#include <aips/Measures/MeasComet.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Utilities/String.h>
#include <aips/Quanta/MVTime.h>
#include <aips/Quanta/MVRadialVelocity.h>
#include <aips/Quanta/MVPosition.h>
#include <aips/Quanta/MVDirection.h>
#include <aips/Arrays/Vector.h>
#include <strstream.h>
#include <iomanip.h>

main() {
  try {
    cout << "Test MeasComet..." << endl;
    cout << "--------------------------------------" << endl;
    
    {
      MeasComet comet("VGEO");
      cout << "Opened VGEO" << endl;
      cout << "--------------------------------------" << endl;
      cout << "Name:           " << comet.getName() << endl;
      cout << "Type:           " << 
	MDirection::showType(comet.getType()) << endl;
      cout << "Topography:     " << comet.getTopo() << endl;
      cout << "Start:          " <<
	MVTime(comet.getStart()).string(MVTime::YMD) << endl;
      cout << "End:            " <<
	MVTime(comet.getEnd()).string(MVTime::YMD) << endl;
      cout << "Entries:        " << comet.nelements() << endl;
      cout << "--------------------------------------" << endl;
      cout << "Radial velocity:" << endl;
      for (Double x=50802.75; x<50803.0625001; x += 10.0/60./24.) {
	MVRadialVelocity y;
	cout << MVTime(x).string(MVTime::YMD) << " " <<
	  comet.getRadVel(y, x) << ": " << y << endl;
      };
      cout << "--------------------------------------" << endl;
      cout << "Position:" << endl;
      for (Double x=50802.75; x<50803.0625001; x += 10.0/60./24.) {
	MVPosition y;
	cout << MVTime(x).string(MVTime::YMD) << " " <<
	  comet.get(y, x) << ": " << y << endl;
      };
      cout << "--------------------------------------" << endl;
      cout << "Disk longitude and latitude:" << endl;
      for (Double x=50802.75; x<50803.0625001; x += 10.0/60./24.) {
	MVDirection y;
	cout << MVTime(x).string(MVTime::YMD) << " " <<
	  comet.getDisk(y, x) << ": " << y << endl;
      };
    }
    
    {
      cout << "--------------------------------------" << endl;
      MeasComet comet("VTOP");
      cout << "Opened VTOP" << endl;
      cout << "--------------------------------------" << endl;
      cout << "Name:           " << comet.getName() << endl;
      cout << "Type:           " << 
	MDirection::showType(comet.getType()) << endl;
      cout << "Topography:     " << comet.getTopo() << endl;
      cout << "Start:          " <<
	MVTime(comet.getStart()).string(MVTime::YMD) << endl;
      cout << "End:            " <<
	MVTime(comet.getEnd()).string(MVTime::YMD) << endl;
      cout << "Entries:        " << comet.nelements() << endl;
      cout << "--------------------------------------" << endl;
      cout << "Radial velocity:" << endl;
      for (Double x=50802.75; x<50803.0625001; x += 10.0/60./24.) {
	MVRadialVelocity y;
	cout << MVTime(x).string(MVTime::YMD) << " " <<
	  comet.getRadVel(y, x) << ": " << y << endl;
      };
      cout << "--------------------------------------" << endl;
    }

  } catch (AipsError x) {
    cout << x.getMesg() << endl;
  } end_try;
  
  exit(0);
}
