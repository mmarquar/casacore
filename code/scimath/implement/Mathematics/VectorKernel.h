//# VectorKernel.h: generate Vector kernels
//# Copyright (C) 1996,1997,1998,1999
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

#if !defined(AIPS_VECTORKERNEL_H)
#define AIPS_VECTORKERNEL_H


//# Includes
#include <aips/aips.h>
#include <aips/Arrays/Vector.h>

//# Forward Declarations


// <summary>
// Make a Vector smoothing kernel from a kernel specification
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class="Vector">Vector</linkto>
//   <li> <linkto class="SepImageConvolver">SepImageConvolver</linkto>
// </prerequisite>

// <etymology>
// Returns a vector from a smoothing kernel specification.
// </etymology>

// <synopsis>
// This class takes a smoothing kernel type and a width, and
// generates  a Vector holding that kernel.  It can be used
// in conjunction with the separable image convolver,
// SepImageConvolver
// </synopsis>

// <example>
// <srcBlock>
// </srcBlock>
// </example>

// <motivation>
// </motivation>

// <todo asof="1990/03/31">
//   <li> 
// </todo>
 

class VectorKernel
{
public:

enum KernelTypes {

// Box-car smoothing kernel
   BOXCAR,

// Gaussian smoothing kernel
   GAUSSIAN,

// Hanning smoothing kernel
   HANNING,

   NKERNELS};

// Create kernel vector for width in pixels.  For Gaussian, width is FWHM,
// for Boxcar, width is full width.  For Hanning width is ignored.
// <group>   
   static Vector<Double> make(KernelTypes kernelType, Double width, uInt shape);
   static Vector<Float> make(KernelTypes kernelType, Float width, uInt shape);
// </group>   


// Helper function to convert a string containing a list of desired smoothed kernel types
// to the correct <src>Vector<Int></src> required for the <src>setSmooth</src> function.
// This may be usful if your user interface involves strings rather than integers.
// A new value is added to the output vector (which is resized appropriately) if any of the 
// substrings "boxcar", "gaussian" or "hanning" (actually "box", "gauss", and "hann"
// will do) is present.
   static Vector<Int> toKernelTypes (const String& kernels);
   static VectorKernel::KernelTypes toKernelType (const String& kernel);

};

#endif

