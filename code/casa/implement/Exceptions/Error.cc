//# Error.cc: Base class for all AIPS++ errors (templated classes)
//# Copyright (C) 1993,1994,1995,1996,2000
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

#include <aips/Exceptions/Error.h>
#include <iostream.h>


template<class t>
indexError<t>::indexError(t oI, const Char *str)
: IndexError(str),
  oIndex    (oI)
{}

template<class t>
indexError<t>::indexError(t oI, const String &str)
: IndexError(str),
  oIndex    (oI)
{}

template<class t>
indexError<t>::~indexError()
{}


template<class t>
duplError<t>::duplError(t oI, const Char *str)
: DuplError(str),
  oKey     (oI)
{}

template<class t>
duplError<t>::duplError(t oI, const String &str)
: DuplError(str),
  oKey     (oI)
{}

template<class t>
duplError<t>::~duplError()
{}

