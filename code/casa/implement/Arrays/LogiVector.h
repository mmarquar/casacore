//# LogiVector.h:  Logical valued Vectors.
//# Copyright (C) 1994,1995
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

#if !defined (AIPS_LOGIVECTOR_H)
#define AIPS_LOGIVECTOR_H

//# There is no source file, so this pragma is not needed.
#if 0
#if defined(_AIX)
#pragma implementation ("LogiVector.cc")
#endif
#endif


#include <aips/aips.h>
#include <aips/Arrays/LogiArray.h>
#include <aips/Arrays/Vector.h>


// <summary>
//    Logical valued Vectors.
// </summary>
// <reviewed reviewer="" date="" tests="">
//
// <prerequisite>
//   <li> <linkto class=Array>Array</linkto>
//   <li> <linkto class=Vector>Vector</linkto>
//   <li> <linkto group="LogiArrayFwd.h#LogicalArray forwards">LogicalArrayFwd</linkto>
//   <li> <linkto group="LogiArray.h#LogicalArray">LogicalArray</linkto>
// </prerequisite>
//
// <etymology>
// LogicalVector declares logical valued Vectors.
// </etymology>
//
// <synopsis>
// This file contains the declarations for LogicalVectors.
// </synopsis>
//
// <motivation>
// These are the Vector specialization of LogicalArrays.
// </motivation>
//
// <todo asof="$DATE:$>
//   <li> Consider making these into classes.
//   <li> Consider replacing with builtin boolean class when that
//          makes it into the C++ compiler.
// </todo>
//
// <linkfrom anchor=LogicalVector classes="Array Vector MaskedArray">
//    <here>LogicalVector</here> -- Logical valued Vectors.
// </linkfrom>
//
// <group name=LogicalVector>


// Define LogicalVector.
//
typedef Vector<LogicalArrayElem> LogicalVector;


// </group>

#endif
