//# tNQSimButterworthBandpass: test the NQSimButterworthBandpass class
//# Copyright (C) 2001,2002
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

#define DIAGNOSTICS
#ifdef DEBUG 
#define DIAGNOSTICS
#endif

#include <aips/Functionals/NQSimButterworthBandpass.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Utilities/Assert.h>
#include <aips/iostream.h>

int main() {
    NQSimButterworthBandpass<Double> butt(1,1,-1,1,0,1);

    AlwaysAssertExit(butt.getCenter()    ==  0 &&
		     butt.getPeak()      ==  1   );
    AlwaysAssertExit(butt.getMinOrder()  ==  1 &&
		     butt.getMaxOrder()  ==  1   );
    AlwaysAssertExit(butt.getMinCutoff() == -1 &&
		     butt.getMaxCutoff() ==  1   );

    butt.setPeak(10);
    AlwaysAssertExit(butt.getPeak() == 10);
    butt.setCenter(5);
    AlwaysAssertExit(butt.getCenter() == 5);
    butt.setMinOrder(4);
    AlwaysAssertExit(butt.getMinOrder() == 4);
    butt.setMaxOrder(5);
    AlwaysAssertExit(butt.getMaxOrder() == 5);
    butt.setMinCutoff(1);
    AlwaysAssertExit(butt.getMinCutoff() == 1);
    butt.setMaxCutoff(9);
    AlwaysAssertExit(butt.getMaxCutoff() == 9);

    Double pk = butt.getPeak(), cen = butt.getCenter(), irt2 = 1.0/sqrt(2.0);
    AlwaysAssertExit(butt(cen) == pk);
    AlwaysAssertExit(butt(butt.getMinCutoff()) == irt2*pk && 
	             butt(butt.getMaxCutoff()) == irt2*pk);
    AlwaysAssertExit(butt(6*butt.getMinCutoff()-5*cen) < 1e-2*pk);
    AlwaysAssertExit(butt(6*butt.getMaxCutoff()+5*cen) < 1e-2*pk);

    cout << "OK" << endl;
    return 0;
}
