//# MCPosition.cc:  MPosition conversion routines 
//# Copyright (C) 1995,1996,1997
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
#ifdef __GNUG__
#include <aips/Measures/Quantum.h>
typedef Quantum<Double> gpp_MCPosition_bug1;
#endif
#include <aips/Arrays/Vector.h>
#include <aips/Mathematics/Math.h>
#include <aips/Utilities/Assert.h>
#include <aips/Measures/MCPosition.h>
#include <aips/Measures/MeasTable.h>

//# Constructors
MCPosition::MCPosition() :
  DVEC1(0) {}

//# Destructor
MCPosition::~MCPosition() {
  clearConvert();
}

//# Operators

//# Member functions

void MCPosition::getConvert(MConvertBase &mc,
			    const MRBase &inref, 
			    const MRBase &outref) {
// Array of conversion routines to call
    static const MCPosition::Routes 
	FromTo[MPosition::N_Types][MPosition::N_Types] = {
    { MCPosition::N_Routes,    MCPosition::ITRF_WGS84},
    { MCPosition::WGS84_ITRF,  MCPosition::N_Routes}
    };

// List of codes converted to
    static const MPosition::Types ToRef[MCPosition::N_Routes] = {
	MPosition::WGS84, MPosition::ITRF
	};

    Int iin  = inref.getType();
    Int iout = outref.getType();
    Int tmp;
    while (iin != iout) {
	tmp = FromTo[iin][iout];
	iin = ToRef[tmp];
	mc.addMethod(tmp);
	initConvert(tmp, mc);
    };
}

void MCPosition::clearConvert() {
  delete DVEC1; DVEC1 = 0;
}

//# Conversion routines
void MCPosition::initConvert(uInt which, MConvertBase &mc) {
  if (!DVEC1) {
    DVEC1 = new Vector<Double>(3);
  };

  switch (which) {

  case ITRF_WGS84:
    break;

  case WGS84_ITRF:
    break;

  default:
    break;
	
  }
}

void MCPosition::doConvert(MeasValue &in,
			   MRBase &inref,
			   MRBase &outref,
			   const MConvertBase &mc) {
  doConvert(*(MVPosition*)&in,
	    inref, outref, mc);
}

void MCPosition::doConvert(MVPosition &in, 
			   MRBase &inref,
			   MRBase &outref,
			   const MConvertBase &mc) {
    
  Double g1, g2, g3;

  for (Int i=0; i<mc.nMethod(); i++) {

    switch (mc.getMethod(i)) {
	
    case ITRF_WGS84: {
      Double d1, d2;
// Get angles
      *DVEC1 = in.get();
// Get flattening
      g1 = MeasTable::WGS84(1); g1 = 1.0/g1;
      g1 = 2*g1 - g1*g1;
// Iterate
      d2 = (*DVEC1)(0) * cos((*DVEC1)(2));
      do {
	g2 = (*DVEC1)(2);
	d1 = sin(g2);
	g3 = 1.0/sqrt(1 - g1 * d1 * d1);
	(*DVEC1)(2) = atan((in(2) + 
			    MeasTable::WGS84(0) * g3 * g1 * d1)/d2);
      }
      while ( !nearAbs((*DVEC1)(2), g2, 1e-10));
      (*DVEC1)(0) = d2/cos((*DVEC1)(2)) - MeasTable::WGS84(0) * g3;
      in = MVPosition(Quantity((*DVEC1)(0),"m"),
		      (*DVEC1)(1), (*DVEC1)(2));
    };
    break;

    case WGS84_ITRF: {
// Equatorial radius
      g1 = MeasTable::WGS84(0);
// Flattening
      g2 = MeasTable::WGS84(1);
      g2 = 1.0 - 1.0/g2; g2 *= g2;
// C
      g3 = in(0) * in(0) +
	in(1) * in(1) +
	g2 * in(2) * in(2);
      g3 = g1 * sqrt(1.0/g3);
// S
      g2 *= g3;
// Apply
      in(0) *= (1.0 + g3);
      in(1) *= (1.0 + g3);
      in(2) *= (1.0 + g2);
    }	
    break;

    default:
      break;
	
    }
  }
}
