//# IterError.cc:
//# Copyright (C) 1993,1994,1995,2000,2003
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

#include <aips/Containers/IterError.h>


// The normal constructor when throwing the exception.
IterError::IterError (const char *msg) : 
          AipsError(msg ? msg : "Iterator Error.") {}

IterError::~IterError () throw()
{ ; }

// The normal constructor when throwing the exception.
IterBoundaryError::IterBoundaryError (const char *msg) : 
          IterError(msg ? msg : "Iterator boundaries exceeded.") {}

IterBoundaryError::~IterBoundaryError () throw()
{ ; }


// The normal constructor when throwing the exception.
IterInitError::IterInitError (const char *msg) : 
          IterError(msg ? msg : "Iterator initialization error.") {}

IterInitError::~IterInitError () throw()
{ ; }

// The normal constructor when throwing the exception.
InvalidIterError::InvalidIterError (const char *msg) : 
          IterError(msg ? msg : "Use of invalid iterator.") {}

InvalidIterError::~InvalidIterError () throw()
{ ; }
