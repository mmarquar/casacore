//# SimOrdMapIO.h: SimOrdMap IO operations
//# Copyright (C) 1993,1994,1995
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

#if !defined(AIPS_SIMORDMAPIO_H)
#define AIPS_SIMORDMAPIO_H

#if defined(_AIX)
#pragma implementation ("SimOrdMapIO.cc")
#endif

#include <aips/aips.h>
#include <aips/Containers/SimOrdMap.h>

// Class declarations.
class AipsIO;
imported class ostream;

// This header file defines the AipsIO functions for template SimpleOrderedMap.
// If these functions were defined in the SimpleOrderedMap class itself,
// it would require that AipsIO was defined for the template arguments.
// This would prevent using SimpleOrderedMap with imported classes.

template<class K, class V>
AipsIO& operator<<(AipsIO&, const SimpleOrderedMap<K,V>&);

template<class K, class V>
AipsIO& operator>>(AipsIO&, SimpleOrderedMap<K,V>&);

template<class K, class V>
ostream& operator<<(ostream&, const SimpleOrderedMap<K,V>&);

#endif
