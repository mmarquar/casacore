//# SpectralEstimate.cc: Get an initial estimate for spectral lines
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
#include <trial/Wnbt/SpectralEstimate.h>
#include <aips/Arrays/Vector.h>
#include <aips/Mathematics/Constants.h>
#include <aips/Mathematics/Math.h>

//# Constructors
SpectralEstimate::SpectralEstimate(const uInt maxpar) :
  useWindow_p(False), rms_p(0), cutoff_p(0),
  windowLow_p(0), windowEnd_p(0),
  q_p(2), sigmin_p(0),
  deriv_p(0), slist_p(maxpar), lprof_p(0) {
  setQ();
}

SpectralEstimate::SpectralEstimate(const Double rms,
				   const Double cutoff, const Double minsigma,
				   const uInt maxpar) :
  useWindow_p(False), rms_p(rms), cutoff_p(cutoff),
  windowLow_p(0), windowEnd_p(0),
  q_p(2), sigmin_p(minsigma),
  deriv_p(0), slist_p(maxpar), lprof_p(0) {
  setQ();
}


SpectralEstimate::SpectralEstimate(const SpectralEstimate &other) {
  ///
}

SpectralEstimate::~SpectralEstimate() {
  delete [] deriv_p; deriv_p = 0; lprof_p = 0;
}

SpectralEstimate &SpectralEstimate::operator=(const SpectralEstimate &other) {
  if (this != &other) {
    ///
  };
  return *this;
}

const SpectralList &SpectralEstimate::estimate(const Vector<Float> &prof,
					       Vector<Float> *der) {
  if (prof.nelements() != lprof_p) {
    delete [] deriv_p; deriv_p = 0; lprof_p = 0;
    lprof_p = prof.nelements();
    deriv_p = new Double[lprof_p];
  };
  // Check if signal in window
  if (!window(prof)) return slist_p;
  // Limit window
  windowEnd_p = min(windowEnd_p+q_p , Int(lprof_p)); 
  windowLow_p = max(windowLow_p-q_p , 0 );
  // Get the second derivatives
  findc2(prof);
  // Next for debugging
  if (der) {
    for (uInt i=0; i<lprof_p; i++) der->operator()(i) = deriv_p[i];
  };
  // Find the estimates (sorted)
  findga(prof);
  return slist_p;
}

void SpectralEstimate::setRMS(const Double rms) {
  rms_p = abs(rms);
}

void SpectralEstimate::setCutoff(const Double cutoff) {
  cutoff_p = max(0.0, cutoff);
}

void SpectralEstimate::setMinSigma(const Double minsigma) {
  sigmin_p = max(0.0, minsigma);
}

void SpectralEstimate::setQ(const uInt q) {
q_p = max(1, Int(q));
  a_p = 90.0/(q_p*(q_p+1)*(4*q_p*q_p-1)*(2*q_p+3));
  b_p = (q_p*(q_p+1))/3.0;
}

void SpectralEstimate::setWindowing(const Bool win) {
  useWindow_p = win;
}

void SpectralEstimate::setMaxN(const uInt maxpar) {
  slist_p.set(maxpar);
}

uInt SpectralEstimate::window(const Vector<Float> &prof) {
  windowLow_p =0;
  windowEnd_p = 0;
  if (!useWindow_p || rms_p <= 0.0 || lprof_p == 0) {
    windowEnd_p = lprof_p;
    return lprof_p;
  };
  // Total flux in profile and max position
  Double flux(0.0);
  Double pmax(prof(0));
  uInt imax(0);
  for (uInt i=0; i<lprof_p; i++) {
    if (prof(i)>pmax) {
      pmax = prof(i);
      imax = i;
    };
    flux += prof(i);
  };
  // No data
  if (pmax < cutoff_p) return 0;
  // Window boundaries; new/old base and centre; width
  Int width(-1);
  Int nw(0);
  Double bnew(flux), bold;
  Double cnew(imax), cold;
  do {
    width++;
    cold = cnew;
    bold = bnew;
    windowLow_p = max(0, Int(cold-width+0.5));
    windowEnd_p = min(Int(lprof_p), Int(cold+width+1.5));
    // flux and first moment in window
    Double s(0);
    Double c(0);
    for (Int i=windowLow_p; i<windowEnd_p; i++) {
      s += prof(i);
      c += i*prof(i);
    };
    bnew = flux-s;
    nw = lprof_p-windowEnd_p+windowLow_p;
    if (s != 0.0) {
      cnew = c/s;
      if (cnew < 0 || cnew >= lprof_p) cnew = cold;
    };
  } while (abs(bnew-bold) > rms_p && nw);
  return windowEnd_p-windowLow_p;
}

void SpectralEstimate::findc2(const Vector<Float> &prof) {
  for (Int i=windowLow_p; i<windowEnd_p; i++) {
    // Moments
    Double m0(0.0); 
    Double m2(0.0); 
    for (Int j = -q_p; j <= q_p; j++) {
      Int k = i+j;
      if (k >= 0 && k<Int(lprof_p)) {
	// add to moments
	m0 += prof(k);
	m2 += prof(k)*j*j;
      };
    };
    // get the derivative
    deriv_p[i] = a_p*(m2-b_p*m0);
  };
}

void SpectralEstimate::findga(const Vector<Float> &prof) {
  Int i(windowLow_p-1);
  // Window on Gaussian
  Int iclo(windowLow_p);
  Int ichi(windowLow_p);
  // Peak counter
  Int nmax = 0;
  SpectralElement tspel;

  while (++i < windowEnd_p) {
    if (deriv_p[i] > 0.0) {
      // At edge?
      if (i > windowLow_p && i < windowEnd_p-1) {
	// Peak in 2nd derivative
	if (deriv_p[i-1] < deriv_p[i] && deriv_p[i+1] < deriv_p[i]) nmax++;
	// At start
      } else if (i == windowLow_p && deriv_p[i+1] < deriv_p[i]) nmax++;
      // At end of window
      else if (i == windowEnd_p-1 && deriv_p[i-1] < deriv_p[i]) nmax++;
    };
    switch (nmax) {
      // Search for next peak
    case 1: 
      break;
      // Found a Gaussian
    case 2: {
      // Some moments
      Double m0m(0);
      Double m0(0);
      Double m1(0);
      Double m2(0);
      
      ichi = i;
      // Do Schwarz' calculation
      Double b = deriv_p[iclo];
      Double a = (deriv_p[ichi] - b) / (ichi-iclo);
      for (Int ic=iclo; ic<=ichi; ic++) {
	m0m += min(deriv_p[ic], 0.0);
	Double wi = deriv_p[ic] - a*(ic-iclo) - b;
	m0 += wi;
	m1 += wi*ic;
	m2 += wi*ic*ic;
      };
      // determinant
      Double det = m2*m0 - m1*m1;
      if (det > 0.0 && fabs(m0m) >  FLT_EPSILON) {
	Double   xm = m1/m0;
	
	Double sg = 1.69*sqrt(det) / fabs(m0);
	// Width above critical?
	if (sg > sigmin_p) {
	  Int is = Int(1.73*sg+0.5);
	  Int im = Int(xm+0.5);
	  Double yl(0);
	  if ((im-is) >= 0) yl = prof(im-is);
	  Double yh(0);
	  if ((im + is) <= Int(lprof_p-1)) yh = prof(im+is);
	  Double ym = prof(im);
	  Double pg = (ym-0.5*(yh+yl))/(1.0-exp(-0.5*(is*is)/sg/sg));
	  pg = min(pg, ym);
	  // Above critical level? Add to list
	  if (pg > cutoff_p) {
	    tspel.setAmpl(pg);
	    tspel.setCenter(xm);
	    tspel.setSigma(sg);
	    slist_p.insert(tspel);
	  };
	};
      };
      // Next gaussian
      iclo = ichi;
      nmax--;
      break;
    }
    default:
      iclo = i+1;
      break;
    };
  };
}
