//# Exceptions.h:  a module for exception handling
//# Copyright (C) 1995,1999
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

#if !defined (AIPS_EXCEPTIONS_H)
#define AIPS_EXCEPTIONS_H

#include <aips/Exceptions/CanDelete.h>
#include <aips/Exceptions/Excp.h>
#include <aips/Exceptions/Error.h>

// <module>
//
// <summary>
//     Exception handling
// </summary>
//
// <reviewed reviewer="" date="" demos="">
// </reviewed>
//
// <synopsis>
// This module provides the exception handling mechanism used in AIPS++.
// It allows the user to define new exception types and to <src>throw</src>,
// <src>catch</src>, and <src>rethrow</src> these exceptions. The interface
// to this exception handling mechanism is very similary to the ANSI standard
// exceptions. This will make it easy to switch when the time comes.
//
// This module supplies several <linkto file="Error.h">common exception types </linkto>.
// These provide examples for creating new errors.
//
// At some point, this exception mechanism will probably be replaced with
// compiler supported exceptions.
// </synopsis>
//
// <example>
// <em>This example shows how a more specific exception can be caught as
// a more general exception:</em>
// <srcblock>
//     #include <aips/Exceptions.h>
//     #include <iostream.h>
//     main() {
//         try {
//             throw(indexError<int>(5,"Dummy error"));
//         } catch (IndexError xx) {
//             cout << "caught IndexError" << endl;
//         } end_try;
//     }
// </srcblock>
// </example>
// </module>

#endif
