//# SpectralList.cc: A set of SpectralElements
//# Copyright (C) 2001
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
#include <trial/Wnbt/SpectralList.h>

#include <aips/Arrays/Vector.h>
#include <aips/Exceptions/Error.h>
#include <trial/Wnbt/SpectralElement.h>

#include <aips/iostream.h>

//# Constructors
SpectralList::SpectralList() :
  nmax_p(0), list_p(0) {}

SpectralList::SpectralList(uInt nmax) :
  nmax_p(nmax), list_p(0) {
}

SpectralList::SpectralList(const SpectralElement &in) :
  nmax_p(0), list_p(1) {
  list_p[0] = new SpectralElement(in);
}

SpectralList::SpectralList(const SpectralList &other) :
  nmax_p(other.nmax_p), list_p(other.list_p.nelements()) {
  for (uInt i=0; i<list_p.nelements(); i++) {
    list_p[i] = new SpectralElement(*other.list_p[i]);
  };
}

SpectralList::~SpectralList() {
  for (uInt i=0; i<list_p.nelements(); i++) {
    delete list_p[i]; list_p[i] = 0;
  };
  list_p.resize(0);
};

SpectralList &SpectralList::operator=(const SpectralList &other) {
  if (this != &other) {
    nmax_p = other.nmax_p;
    list_p.resize(other.list_p.nelements(), False);
    for (uInt i=0; i<list_p.nelements(); i++) {
      list_p[i] = new SpectralElement(*other.list_p[i]);
    };
  };
  return *this;
}

Double SpectralList::operator()(const Double x) const {
  Double s(0);
  for (uInt i=0; i<list_p.nelements(); i++) s += (*list_p[i])(x);
  return s;
}

const SpectralElement &SpectralList::operator[](const uInt n) const {
  if (n >= list_p.nelements()) {
      throw(AipsError("SpectralList: Illegal index for element"));
  };
  return *list_p[n];
}

SpectralElement &SpectralList::operator[](const uInt n) {
  if (n >= list_p.nelements()) {
      throw(AipsError("SpectralList: Illegal index for element"));
  };
  return *list_p[n];
}

void SpectralList::evaluate(Vector<Float> &y) const {
  for (uInt j=0; j<y.nelements(); j++) {
    if (list_p.nelements() > 0) y(j) = (*list_p[0])(j);
    else y(j) = 0;
  };
  for (uInt i=1; i<list_p.nelements(); i++) {
    for (uInt j=0; j<y.nelements(); j++) y(j) += (*list_p[i])(j);
  };
}

void SpectralList::evaluate(Vector<Float> &y, const Vector<Float> &x) const {
  y.resize(x.nelements());
  for (uInt j=0; j<x.nelements(); j++) {
    if (list_p.nelements() > 0) y(j) = (*list_p[0])(x(j));
    else y(j) = 0;
  };
  for (uInt i=1; i<list_p.nelements(); i++) {
    for (uInt j=0; j<x.nelements(); j++) y(j) += (*list_p[i])(x(j));
  };
}

void SpectralList::evaluate(Vector<Double> &y) const {
  for (uInt j=0; j<y.nelements(); j++) {
    if (list_p.nelements() > 0) y(j) = (*list_p[0])(j);
    else y(j) = 0;
  };
  for (uInt i=1; i<list_p.nelements(); i++) {
    for (uInt j=0; j<y.nelements(); j++) y(j) += (*list_p[i])(j);
  };
}

void SpectralList::evaluate(Vector<Double> &y, const Vector<Double> &x) const {
  y.resize(x.nelements());
  for (uInt j=0; j<x.nelements(); j++) {
    if (list_p.nelements() > 0) y(j) = (*list_p[0])(x(j));
    else y(j) = 0;
  };
  for (uInt i=1; i<list_p.nelements(); i++) {
    for (uInt j=0; j<x.nelements(); j++) y(j) += (*list_p[i])(x(j));
  };
}

void SpectralList::residual(Vector<Float> &y) const {
  for (uInt i=0; i<list_p.nelements(); i++) {
    for (uInt j=0; j<y.nelements(); j++) y(j) -= (*list_p[i])(j);
  };
}

void SpectralList::residual(Vector<Double> &y) const {
  for (uInt i=0; i<list_p.nelements(); i++) {
    for (uInt j=0; j<y.nelements(); j++) y(j) -= (*list_p[i])(j);
  };
}

void SpectralList::residual(Vector<Float> &y, const Vector<Float> &x) const {
  if (x.nelements() != y.nelements()) {
    throw(AipsError("Unequal lengths in arguments SpectralList::residual"));
  };
  for (uInt i=0; i<list_p.nelements(); i++) {
    for (uInt j=0; j<x.nelements(); j++) y(j) -= (*list_p[i])(x(j));
  };
}

void SpectralList::residual(Vector<Double> &y, const Vector<Double> &x) const {
  if (x.nelements() != y.nelements()) {
    throw(AipsError("Unequal lengths in arguments SpectralList::residual"));
  };
  for (uInt i=0; i<list_p.nelements(); i++) {
    for (uInt j=0; j<x.nelements(); j++) y(j) -= (*list_p[i])(x(j));
  };
}

Bool SpectralList::add(const SpectralElement &in) {
  uInt i = list_p.nelements();
  if (nmax_p != 0 && i >= nmax_p) return False;
  list_p.resize(i+1);
  list_p[i] = new SpectralElement(in);
  return True;
}

Bool SpectralList::add(const SpectralList &in) {
  for (uInt i=0; i<in.nelements(); i++) if (!add(in[i])) return False;
  return True;
}

void SpectralList::insert(const SpectralElement &in) {
  uInt n = list_p.nelements();
  uInt i;
  for (i=0; i<n; i++) {
    if (compar(in, *list_p[i]) > 0) break;
  };
  if (i == n) add(in);
  else {
    if (nmax_p != 0 && n >= nmax_p) {
      delete list_p[n-1]; list_p[n-1] = 0;
    } else {
      list_p.resize(n+1);
      list_p[n++] = 0;
    };
    for (uInt j=n-1; j>i; j--) list_p[j] = list_p[j-1];
    list_p[i] = new SpectralElement(in);
  };
}

void SpectralList::insert(const SpectralList &in) {
  for (uInt i=0; i<in.nelements(); i++) insert(in[i]);
}

Bool SpectralList::set(const SpectralElement &in, const uInt which) {
  uInt i = list_p.nelements();
  if (nmax_p != 0 && which >= nmax_p) return False;
  if (which > i) return False;
  if (which == i) add(in);
  delete list_p[which]; list_p[which] = 0;
  list_p[which] = new SpectralElement(in);
  return True;
}

void SpectralList::set(const uInt nmax) {
  if (nmax != 0 && nmax < list_p.nelements()) {
    for (uInt i=nmax; i<list_p.nelements(); i++) {
      delete list_p[i]; list_p[i] = 0;
    };
    list_p.resize(nmax);
  };
  nmax_p = nmax;
}

void SpectralList::sort() {
  uInt n = list_p.nelements();
  if (n < 2) return;
  SpectralElement *x;
  for (uInt i=0; i<n-1; i++) {
    for (uInt j=n-1; j>i; j--) {
      if (compar(*list_p[j-1], *list_p[j]) < 0) {
	x = list_p[j-1];
	list_p[j-1] = list_p[j];
	list_p[j] = x;
      };
    };
  };
}

Int SpectralList::compar(const SpectralElement &p1,
			 const SpectralElement &p2 ) {
  if (p1.getAmpl() > p2.getAmpl()) return (1);
  else if (p1.getAmpl() < p2.getAmpl()) return (-1);
  else return (0);
}

ostream &operator<<(ostream &os, const SpectralList &lst) {
  os << lst.nelements() << " in SpectralList:" << endl;
  for (uInt i=0; i<lst.nelements(); i++) os << lst[i];

  return os;
}
