//# aipsdef.h: Global initialization for special aips++ macros
//# Copyright (C) 2000,2001
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

#if !defined(AIPS_AIPSDEF_H)
#define AIPS_AIPSDEF_H

// Define the aips++ global macros

// Defined the "aips_name2" macro which is used to join two tokens.

#if defined(__STDC__) || defined(__ANSI_CPP__) || defined(__hpux)
#define aips_name2(a,b) a##b
#else
#define aips_name2(a,b) a/**/b
#endif

// If AIPS_DEBUG is not defined, then the symbol expands to (0) which in an
// if should be removed by the dead code eliminator of any optimizer; thus
// using this in your code should have no performance penalty in the normal
// case. If compiled with AIPS_DEBUG, then aips_debug is (defined to )
// a global boolean variable (so it can be turned on and off in a debugger) 
// which is initialized to True.

extern Bool aips_debug_on;

#if !defined(AIPS_DEBUG)
#define aips_debug (0)
#else
// The reason that we just don't make this a variable here is so that
// we can link against libraries compiled with or without AIPS_DEBUG
// without having any missing symbols.
#define aips_debug aips_debug_on
#endif

// Define the use of the old string and stream classes (for sgi at the moment)
// The use of the old classes (including complex) can be overwritten
// with the use of -DAIPS_USE_NEW_SGI.
// It also defines and uses the std namespace as required.
// It is the intention to use the relevant 'USE_OLD' macros only
// if and when closely related to the String class itself; the Complex
// class and the stream proxy includes.
// Note that for the gcc compiler 'std::' is recognised as '::' for now.
#if defined(__sgi)
#if !defined(AIPS_USE_NEW_SGI)
#define USE_OLD_STRING
#define AIPS_USE_OLD_STRING
#define AIPS_USE_OLD_STREAM
#else
#undef USE_OLD_STRING
#undef AIPS_USE_OLD_STRING
#undef AIPS_USE_OLD_STREAM
#undef AIPS_USE_OLD_COMPLEX
namespace std {};
using namespace std;
#endif
#endif

#endif
