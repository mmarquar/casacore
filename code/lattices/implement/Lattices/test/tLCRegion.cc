//# tLCRegion.cc: Test program for derived LCRegion classes.
//# Copyright (C) 1997
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

#include <trial/Lattices/LCBox.h>
#include <trial/Lattices/LCEllipsoid.h>
#include <aips/Tables/TableRecord.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/ArrayIO.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Utilities/Assert.h>
#include <iostream.h>


void doIt (const IPosition& latticeShape,
	   const Vector<Float>& center,
	   const Vector<Float>& radii)
{
    LCEllipsoid cir (center, radii, latticeShape);
    cout << cir.hasMask() << ' ' << cir.maskArray() << endl;
    cout << cir.box().start() << cir.box().end()
	 << cir.box().length() << cir.latticeShape() << endl;
    cout << cir.center().ac() << cir.radii().ac() << endl;
}

void doIt (const IPosition& latticeShape,
	   const IPosition& start,
	   const IPosition& end,
	   const IPosition& center,
	   Float radius)
{
    LCBox box (start, end, latticeShape);
    cout << box.hasMask() << ' ' << box.maskArray() << endl;
    cout << box.box().start() << box.box().end()
	 << box.box().length() << box.latticeShape() << endl;
    LCEllipsoid cir (center, radius, latticeShape);
    cout << cir.hasMask() << ' ' << cir.maskArray() << endl;
    cout << cir.box().start() << cir.box().end()
	 << cir.box().length() << cir.latticeShape() << endl;
    cout << cir.center().ac() << cir.radii().ac() << endl;
    {
	// Test cloning.
	LCRegionFixed* boxcop = (LCRegionFixed*)(box.clone());
	AlwaysAssertExit (box.hasMask() == boxcop->hasMask());
	AlwaysAssertExit (allEQ (box.maskArray(), boxcop->maskArray()));
	AlwaysAssertExit (box.box().start() == boxcop->box().start());
	AlwaysAssertExit (box.box().end() == boxcop->box().end());
	AlwaysAssertExit (box.box().stride() == boxcop->box().stride());
	AlwaysAssertExit (box.box().length() == boxcop->box().length());
	delete boxcop;
	LCRegionFixed* circop = (LCRegionFixed*)(cir.clone());
	AlwaysAssertExit (cir.hasMask() == circop->hasMask());
	AlwaysAssertExit (allEQ (cir.maskArray(), circop->maskArray()));
	AlwaysAssertExit (cir.box().start() == circop->box().start());
	AlwaysAssertExit (cir.box().end() == circop->box().end());
	AlwaysAssertExit (cir.box().stride() == circop->box().stride());
	AlwaysAssertExit (cir.box().length() == circop->box().length());
	AlwaysAssertExit (allEQ (cir.center().ac(),
				 ((LCEllipsoid*)circop)->center().ac()));
	AlwaysAssertExit (allEQ (cir.radii().ac(),
				 ((LCEllipsoid*)circop)->radii().ac()));
	delete circop;
    }
    {
	// Test persistency.
	LCRegionFixed* boxcop = (LCRegionFixed*)
	                    (LCRegion::fromRecord (box.toRecord(""), ""));
	AlwaysAssertExit (box.hasMask() == boxcop->hasMask());
	AlwaysAssertExit (allEQ (box.maskArray(), boxcop->maskArray()));
	AlwaysAssertExit (box.box().start() == boxcop->box().start());
	AlwaysAssertExit (box.box().end() == boxcop->box().end());
	AlwaysAssertExit (box.box().stride() == boxcop->box().stride());
	AlwaysAssertExit (box.box().length() == boxcop->box().length());
	delete boxcop;
	LCRegionFixed* circop = (LCRegionFixed*)
	                    (LCRegion::fromRecord (cir.toRecord(""), ""));
	AlwaysAssertExit (cir.hasMask() == circop->hasMask());
	AlwaysAssertExit (allEQ (cir.maskArray(), circop->maskArray()));
	AlwaysAssertExit (cir.box().start() == circop->box().start());
	AlwaysAssertExit (cir.box().end() == circop->box().end());
	AlwaysAssertExit (cir.box().stride() == circop->box().stride());
	AlwaysAssertExit (cir.box().length() == circop->box().length());
	AlwaysAssertExit (allEQ (cir.center().ac(),
				 ((LCEllipsoid*)circop)->center().ac()));
	AlwaysAssertExit (allEQ (cir.radii().ac(),
				 ((LCEllipsoid*)circop)->radii().ac()));
	delete circop;
    }
}


main()
{
    try {
	doIt (IPosition (2,11,20),
	      IPosition (2,3,4), IPosition (2,7,8),
	      IPosition (2,5,10), 5.);
	doIt (IPosition (2,10,20),
	      IPosition (2,3,4), IPosition (2,7,8),
	      IPosition (2,4,16), 5.);
	Vector<Float> center(2), radii(2);
	radii(0) = radii(1) = 5.01;
	center(0) = 5;
	center(1) = 10.5;
	doIt (IPosition (2,11,20), center, radii);
	radii(0) = 4;
	radii(1) = 8;
	center(1) = 10;
	doIt (IPosition (2,11,20), center, radii);
    } catch (AipsError x) {
	cout << "Caught exception: " << x.getMesg() << endl;
	return 1;
    } end_try;
    cout << "OK" << endl;
    return 0;
}
