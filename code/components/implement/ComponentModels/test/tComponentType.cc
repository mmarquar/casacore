//# ClassFileName.cc:  this defines ClassName, which ...
//# Copyright (C) 1997,1998
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

//# Includes

#include <aips/aips.h>
#include <aips/Exceptions/Error.h>
#include <aips/Utilities/Assert.h>
#include <trial/ComponentModels/ComponentType.h>

int main() {
  try {
    ComponentType::Shape e(ComponentType::POINT);
    AlwaysAssert(ComponentType::name(e) == "Point", AipsError);
    e = ComponentType::GAUSSIAN;
    AlwaysAssert(ComponentType::name(e) == "Gaussian", AipsError);
    e = ComponentType::UNKNOWN;
    AlwaysAssert(ComponentType::name(e) == "Unknown", AipsError);
    e = ComponentType::NUMBER_SHAPES;
    AlwaysAssert(ComponentType::name(e) == "Unknown", AipsError);
    String s("point");
    AlwaysAssert(ComponentType::shape(s) == ComponentType::POINT, AipsError);
    s = "GAUSSIAN";
    AlwaysAssert(ComponentType::shape(s) == ComponentType::GAUSSIAN,AipsError);
    s = "Pointer";
    AlwaysAssert(ComponentType::shape(s) == ComponentType::UNKNOWN, AipsError);
  }
  catch (AipsError x) {
    cerr << x.getMesg() << endl;
    cout << "FAIL" << endl;
    return 1;
  } end_try;
  cout << "OK" << endl;
  return 0;
}
// Local Variables: 
// compile-command: "gmake OPTLIB=1 tComponentType"
// End: 
