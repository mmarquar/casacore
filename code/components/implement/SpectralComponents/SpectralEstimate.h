//# SpectralEstimate.h: Get an initial estimate for spectral lines
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
//#
//# $Id$

#if !defined(TRIAL_SPECTRALESTIMATE_H)
#define TRIAL_SPECTRALESTIMATE_H

//# Includes
#include <aips/aips.h>

//# Forward Declarations
class SpectralElement;
template <class T> class Vector;

// <summary>
// Get an initial estimate for spectral lines
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="tSpectralFit" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=SpectralFit>SpectralFit</linkto> class
// </prerequisite>
//
// <etymology>
// From spectral line and estimate
// </etymology>
//
// <synopsis>
// The SpectralEstimate class obtains a initial guess for spectral
// components. The current implementation uses the entire 
// profile as signal region.  The second derivative of
// the profile in the signal region is calculated by fitting
// a second degree polynomal. The smoothing parameter Q
// determines the number of points used for this (=2*Q+1).
// The gaussians can then be estimated as described by
// Schwarz, 1968, Bull.Astr.Inst.Netherlands, Volume 19, 405.
//
// The elements guessed  can be used in the
// <linkto class=SpectralFit>SpectralFit</linkto> class.
//
// The default type found is a Gaussian, defined as:
// <srcblock>
//	AMPL.exp[ -(x-CENTER)<sup>2</sup>/2 SIGMA<sup>2</sup>]
// </srcblock>
//
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// To have an automatic method to find spectral lines
// </motivation>
//
// <todo asof="2001/02/14">
//   <li> find a way to get to absorption lines as well
//   <li> add more estimation options
// </todo>

class SpectralEstimate {
 public:

  //# Enumerations
  //# Friends

  //# Constructors
  // Default constructor creates a default estimator
  SpectralEstimate();
  // Construct with 
  SpectralEstimate(const Double ampl,
		   const Double center, const Double sigma);
  // Copy constructor (deep copy)
  SpectralEstimate(const SpectralEstimate &other);

  //# Operators
  // Assignment (copy semantics)
  SpectralEstimate &operator=(const SpectralEstimate &other);

  //# Member functions
  // Get the number of estimates found in a profile
  ///  uInt estimate(const Vector<Float> &prof);
  ///  uInt estimate(const Double *const prof);
  uInt estimate(Double * prof);
  // Get the data for the n-th element
  ///  const SpectralElement &element(uInt which) const;
  const SpectralElement element(uInt which) const;

  // Set data for element
  // <group>
  // </group>

  //#Destructor
  // Destructor
  ~SpectralEstimate();

 private:
  //#Data

  //# Member functions
Int compar(const SpectralElement &p1,
	   const SpectralElement &p2);

Int window( Double  y[] ,
	    Int    n ,             /* length of profile */
	    Double  cutoff ,        /* the critical level */
	    Double  rms ,           /* nois level in profile */
	    Int    *wstart ,       /* begin of signal region */
	    Int    *wend );       /* end of signal region */

void findc2( Double  y[] ,           /* the profile */
	     Double  work[] ,        /* the derivative */
	     Int    n ,             /* length of profile */
	     Int    iwlo ,          /* start of window */
	     Int    iwhi ,          /* end of window */
	     Int    q ) ;            /* smoothing parameter */

Int findga( Double  y[] ,           /* the profile */
	    Double  work[] ,        /* the second derivative */
	    Int    n ,             /* length of profile */
	    Int    iwlo ,          /* start of window */
	    Int    iwhi ,          /* end of window */
	    Double  cutoff ,        /* critical level */
	    Double   sigmin );       /* minimum value for sigma */

};

#endif


