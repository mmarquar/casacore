//# tComponentUpdate.cc: test the component update functions
//# Copyright (C) 2000,2001
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

#include <trial/Wnbt/ComponentUpdate.h>
#include <aips/Utilities/Assert.h>
#include <aips/BasicSL/Complex.h>
#include <aips/Arrays/Array.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Arrays/VectorIter.h>
#include <aips/Arrays/IPosition.h>
#include <trial/ComponentModels/ComponentList.h>
#include <trial/ComponentModels/ComponentType.h>
#include <trial/ComponentModels/SkyComponent.h>

#include <aips/iostream.h>

int main() {
  cout << "Test ComponentUpdate" << endl;
  cout << "-------------------------------------------------------" << endl;
  
  ComponentList alist;
  alist.add(SkyComponent(ComponentType::POINT));
  ComponentUpdate upd(alist);
  // A dummy data list to check array indices
  Array<DComplex> der(IPosition(3, 3, alist.nelements(), 2));
  Vector<DComplex> dat(2);
  dat = DComplex(120);
  Int n(0);
  for (Int i=0; i<2; i++) {
    for (uInt j=0; j<alist.nelements(); j++) {
      for (Int k=0; k<3; k++) der(IPosition(3,k,j,i)) = DComplex(n++);
    };
  };
  upd.makeEquations(der, dat);
  Matrix<Double> sol, err;
  upd.solve(sol, err);
  cout << "Solutions: " << endl;
  ReadOnlyVectorIterator<Double> isol(sol); 
  ReadOnlyVectorIterator<Double> ierr(err); 
  Int i(0);
  while (!isol.pastEnd()) {
    cout << i << ":\t" << isol.vector() << endl;
    cout << "  \t" << ierr.vector() << endl;
    isol.next();
    ierr.next();
  };

  cout << "-------------------------------------------------------" << endl;
  cout << "OK" << endl;
  return 0;
}
