//# math.h: Interim solution for standard/nonstandard system cmath
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

#if !defined(AIPS_AIPS_MATH_H)
#define AIPS_AIPS_MATH_H

// Define the C standard C++ include file. 
// This is an interim solution to cater for the SGI non-existence of
// them (e.g. <cstring>)

// Make sure any special macros are set
#include <aips/aips.h>

#if defined(__sgi)
#include <math.h>
#if defined(AIPS_USE_OLD_STREAM)
inline Float abs(Float Val) {if (Val >= 0) return Val; else return -Val;}
inline Double abs(Double Val) {return fabs(Val);}
// The following is not yet part of some of the cmath include file. Should be
// removed at some stage
inline Float pow(Float f1, Float f2) {
  return Float(pow(Double(f1), Double(f2))); };
#endif
#else
#include <cmath>
// The following is not yet part of some of the cmath include file. Should be
// removed at some stage
inline Float pow(Float f1, Float f2) {
  return Float(pow(Double(f1), Double(f2))); };
#endif

#endif
