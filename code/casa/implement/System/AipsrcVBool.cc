//# AipsrcVBool.cc: Specialisation for AipsrcVector<Bool>
//# Copyright (C) 1995,1996,1997,1998,2000,2001
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

#include <aips/Tasking/AipsrcVector.h>
#include <aips/Utilities/Assert.h>
#include <aips/Utilities/Regex.h>
#include <aips/Arrays/Vector.h>

// The following global function defined for gcc 1.0.3b
AipsrcVector<Bool> &AipsrcVectorBoolInit();

// This specialisation is necessary to be able to analyse all values that
// are supposed to be True (strings starting with one of 'tTyY123456789')

//# Constructor
template <> AipsrcVector<Bool>::AipsrcVector() : 
  tlst(0), ntlst(0) {}

//# Destructor
template <> AipsrcVector<Bool>::~AipsrcVector() {}

template <> Bool AipsrcVector<Bool>::find(Vector<Bool> &value,
					  const String &keyword) {
  String res;
  Bool x = Aipsrc::find(res, keyword, 0);
  if (x) {
    const Regex ws("[ 	]+");
    const Regex tTrue("^([tT]|[yY]|[1-9])");
    res.gsub(ws, " ");
    Int m = res.freq(" ") +1;
    String *nres = new String[m];
    m = split(res, nres, m, " ");
    value = Vector<Bool>(m);
    for (Int i=0; i<m; i++) {
      value(i) = ((nres[i]).contains(tTrue));;
    };
    delete [] nres;
  };
  return x;
}

template <> Bool AipsrcVector<Bool>::find(Vector<Bool> &value,
					  const String &keyword, 
					  const Vector<Bool> &deflt) {
  return (find(value, keyword) ? True : (value = deflt, False));
}


// The following construction necessary since the gnu compiler does not (yet)
// support static templated data.
// egcs 1.1.b requires it to be in front of its use.
template <> AipsrcVector<Bool> &AipsrcVector<Bool>::init() {
  // The following necessary for 1.0.3b gnu compiler; which cannot compile next
  // line.
  //  static AipsrcVector<Bool> myp;
  return AipsrcVectorBoolInit();
}


template <> uInt AipsrcVector<Bool>::registerRC(const String &keyword,
						const Vector<Bool> &deflt) {
  AipsrcVector<Bool> &gcl = init();
  uInt n = Aipsrc::registerRC(keyword, gcl.ntlst);
  gcl.tlst.resize(n);
  find ((gcl.tlst)[n-1], keyword, deflt);
  return n;
}

template <> const Vector<Bool> &AipsrcVector<Bool>::get(uInt keyword) {
  AipsrcVector<Bool> &gcl = init();
  AlwaysAssert(keyword > 0 && keyword <= gcl.tlst.nelements(), AipsError);
  return (gcl.tlst)[keyword-1];
}

template <> void AipsrcVector<Bool>::set(uInt keyword,
					 const Vector<Bool> &deflt) {
  AipsrcVector<Bool> &gcl = init();
  AlwaysAssert(keyword > 0 && keyword <= gcl.tlst.nelements(), AipsError);
  (gcl.tlst)[keyword-1].resize(deflt.nelements());
  (gcl.tlst)[keyword-1] = deflt;
}

template <> void AipsrcVector<Bool>::save(uInt keyword) {
  AipsrcVector<Bool> &gcl = init();
  AlwaysAssert(keyword > 0 && keyword <= gcl.tlst.nelements(), AipsError);
  ostrstream oss;
  Int n = ((gcl.tlst)[keyword-1]).nelements();
  for (Int i=0; i<n; i++) {
    if (((gcl.tlst)[keyword-1])(i)) {
      oss << " true";
    } else {
      oss << " false";
    };
  };
  Aipsrc::save((gcl.ntlst)[keyword-1], String(oss));
}

AipsrcVector<Bool> &AipsrcVectorBoolInit() {
  static AipsrcVector<Bool> myp;
  return myp;
}
