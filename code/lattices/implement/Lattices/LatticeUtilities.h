//# LatticeUtilities.h: useful global functions for Lattices
//# Copyright (C) 1995,1996,1997,1999,2000,2001
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

#if !defined(AIPS_LATTICE_UTILITIES_H)
#define AIPS_LATTICE_UTILITIES_H


#include <aips/aips.h>

template <class T> class Array;
template <class T> class Lattice;
template <class T> class MaskedLattice;
class IPosition;
class LogIO;
class Slicer;

// <summary>Static functions for Lattices</summary>
// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="tLatticeUtilities.cc" demos="">
// </reviewed>
//
// <prerequisite>
//   <li> <linkto class="Lattice">Lattice</linkto>
// </prerequisite>
//
// <synopsis>
// Some static helper functions for Lattices
// </synopsis>
//
// <motivation>
// Common functionality not appropriate for Lattice member functions
// </motivation>
//
// <todo asof="2001/08/27">
//   <li> nothing I know of
// </todo>
//


class LatticeUtilities 
{
   public:

// Collapse the specified axes by averaging
// <group>
   template <class T>
   static void collapse (Array<T>& out, const IPosition& axes,
                         const MaskedLattice<T>& in,
                         Bool dropDegenerateAxes);
//
   template <class T>
   static void collapse (Array<T>& data, Array<Bool>& mask,
                         const IPosition& axes, 
                         const MaskedLattice<T>& lat,
                         Bool dropDegenerateAxes);
// </group>

// Copy pixels and mask from input to output.  Zero output pixels
// where mask is False (bad)
   template <class T>
   static void copyAndZero(LogIO& os, MaskedLattice<T>& out, MaskedLattice<T>& in);

// Copy data and mask from input to output.
   template <class T>
   static void copyDataAndMask (LogIO& os, MaskedLattice<T>& out,
                                MaskedLattice<T>& in);

// Replicate array through lattice in the specified region
   template <class T>
   static void replicate (Lattice<T>& im,
                          const Slicer& region,
                          const Array<T>& pixels);
};

// <summary>Global functions on Lattices</summary>
// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="tLatticeUtilities.cc" demos="">
// </reviewed>
//
// <prerequisite>
//   <li> <linkto class="Lattice">Lattice</linkto>
// </prerequisite>
//
// <synopsis>
// Global functions using Lattices
// </synopsis>
//
// <example>
// <h4>Example 1:</h4>
// Copy the lattice-type data between two Images.// <srcblock>
//
// PagedImage<Float> myImg ("myimagefile");
// Float lmin;
// Float lmax;
// IPosition posMin = myImg.shape();
// IPosition posMax = myImg.shape();
// minMax( lmin, lmax, posMin, posMax, myImg );
//
// </srcblock>
// </example>
//
//
// <motivation>
// Algorithms like CLEAN need to know the position of the MIN and MAX
// of an image, but easy things like LEL's min and max don't tell you
// the location of the min and max.  It seems there may be other global
// functions involving lattices.
// </motivation>
//
// <todo asof="1999/10/27">
//   <li> nothing I know of
// </todo>
//
// <group name=LatticeUtilities>

// This global function finds the max of a Lattice, and also
// the IPositions of the max.  (LEL does not get you the IPositions of the
// min and max)

   template <class T>
   void minMax(T & min, T & max, 
               IPosition & posMin, IPosition & posMax, 
               const Lattice<T>& lat);



#endif


