//# aipstype.h: Global initialization for standard aips++ types
//# Copyright (C) 2000,2001,2002
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

#if !defined(AIPS_AIPS_COMPLEX_H)
#define AIPS_AIPS_COMPLEX_H

// Define the standard types used by aips++

#if defined(AIPS_SGI)
#include <complex.h>
#else
#include <complex>
#define NEEDS_LOG10_COMPLEX
#endif

using std::real;
using std::imag;
using std::norm;
using std::abs;
using std::arg;
using std::conj;
using std::cos;
using std::cosh;
using std::sin;
using std::sinh;
using std::tan;
using std::tanh;
using std::exp;
using std::log;
using std::sqrt;
using std::polar;
using std::pow;

#endif
