//# LineCollapser.h: Abstract base class to collapse lines for LatticeApply
//# Copyright (C) 1996,1997
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

#if !defined(AIPS_LINECOLLAPSER_H)
#define AIPS_LINECOLLAPSER_H
 

//# Includes
#include <aips/aips.h>

//# Forward Declarations
template <class T> class Vector;
class IPosition;

// <summary>
// Abstract base class for LatticeApply function signatures
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=LatticeApply>LatticeApply</linkto>
// </prerequisite>

// <etymology>
// </etymology>

// <synopsis>
// This is an abstract base class for the collapsing of lines to
// be used in function <src>lineApply</src> or <src>lineMultiApply</src>
// in class <linkto class=LatticeApply>LatticeApply</linkto>.
// It is meant for cases where the entire line is needed (e.g. moment
// calculation). If that is not needed (e.g. to calculate maximum),
// it is better to use function <src>LatticeApply::tiledApply</src>
// with class <linkto class=TiledCollapser>TiledCollapser</linkto>.
// <p>
// The user has to derive a concrete class from this base class
// and implement the (pure) virtual functions.
// <br> The main function is <src>process</src>, which needs to do the
// calculation.
// <br> Other functions make it possible to perform an initial check.
// </synopsis>

// <example>
// <srcblock>
// </srcblock>
// </example>

// <motivation>
// </motivation>

// <todo asof="1997/08/01">   
//   <li> 
// </todo>

template <class T> class LineCollapser
{
public:
// The init function for a derived class.
// It can be used to check if <src>nOutPixelsPerCollapse</src>
// corresponds with the number of pixels produced per collapsed line.
    virtual void init (uInt nOutPixelsPerCollapse) = 0;

// Collapse the given line and return one value from that operation.
// The position in the Lattice at the start of the line is input
// as well.
    virtual T process (const Vector<T>& line,
		       const IPosition& pos) = 0;

// Collapse the given line and return a line of values from that operation.
// The position in the Lattice at the start of the line is input
// as well.
    virtual Vector<T>& multiProcess (const Vector<T>& line,
				     const IPosition& pos) = 0;
};


#endif
