//# AipsrcBool.cc: Specialisation for AipsrcValue<Bool>
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

#include <aips/Tasking/AipsrcValue.h>
#include <aips/Utilities/Regex.h>
#include <aips/Utilities/Assert.h>

// The following global function defined for gcc 1.0.3b
AipsrcValue<Bool> &AipsrcValueBoolInit();

// This specialisation is necessary to be able to analyse all values that
// are supposed to be True (strings starting with one of 'tTyY123456789')

//# Constructor
template <> AipsrcValue<Bool>::AipsrcValue() :
  tlst(0), ntlst(0) {}

//# Destructor
template <> AipsrcValue<Bool>::~AipsrcValue() {}

template <> Bool AipsrcValue<Bool>::find(Bool &value,
					 const String &keyword) {
  const Regex tTrue("^([tT]|[yY]|[1-9])");
  String res;
  Bool x = Aipsrc::find(res, keyword, 0);
  if (x) value = (res.contains(tTrue));
  return x;
}

template <> Bool AipsrcValue<Bool>::find(Bool &value, const String &keyword, 
					 const Bool &deflt) {
  return (find(value, keyword) ? True : (value = deflt, False));
}


// The following construction necessary since the gnu compiler does not (yet)
// support static templated data.
// egcs 1.1.b requires it to be in front of its use.
template <> AipsrcValue<Bool> &AipsrcValue<Bool>::init() {
  // The following necessary for 1.0.3b gnu compiler; which cannot compile next
  // line.
  //  static AipsrcValue<Bool> myp;
  return AipsrcValueBoolInit();
}


template <> uInt AipsrcValue<Bool>::registerRC(const String &keyword,
					       const Bool &deflt) {
  AipsrcValue<Bool> &gcl = init();
  uInt n = Aipsrc::registerRC(keyword, gcl.ntlst);
  gcl.tlst.resize(n);
  find ((gcl.tlst)[n-1], keyword, deflt);
  return n;
}

template <> const Bool &AipsrcValue<Bool>::get(uInt keyword) {
  AipsrcValue<Bool> &gcl = init();
  AlwaysAssert(keyword > 0 && keyword <= gcl.tlst.nelements(), AipsError);
  return (gcl.tlst)[keyword-1];
}

template <> void AipsrcValue<Bool>::set(uInt keyword, const Bool &deflt) {
  AipsrcValue<Bool> &gcl = init();
  AlwaysAssert(keyword > 0 && keyword <= gcl.tlst.nelements(), AipsError);
  (gcl.tlst)[keyword-1] = deflt;
}

template <> void AipsrcValue<Bool>::save(uInt keyword) {
  AipsrcValue<Bool> &gcl = init();
  AlwaysAssert(keyword > 0 && keyword <= gcl.tlst.nelements(), AipsError);
  ostrstream oss;
  if ((gcl.tlst)[keyword-1]) {
    oss << "true";
  } else {
    oss << "false";
  };
  Aipsrc::save((gcl.ntlst)[keyword-1], String(oss));
}

AipsrcValue<Bool> &AipsrcValueBoolInit() {
  static AipsrcValue<Bool> myp;
  return myp;
}
