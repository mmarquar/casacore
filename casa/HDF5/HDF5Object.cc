//# HDF5Object.cc: An abstract base class representing an HDF5 object
//# Copyright (C) 2008
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

//# Includes
#include <casa/HDF5/HDF5Object.h>
#include <casa/HDF5/HDF5Error.h>

namespace casa { //# NAMESPACE CASA - BEGI

  HDF5Object::~HDF5Object()
  {}

#ifdef HAVE_HDF5
  Bool HDF5Object::hasHDF5Support()
    { return True; }
  void HDF5Object::throwNoHDF5()
  {}
#else
  Bool HDF5Object::hasHDF5Support()
    { return False; }
  void HDF5Object::throwNoHDF5()
  {
    throw HDF5Error("HDF5 support is not compiled into this casacore version");
  }
#endif

  void throwInvHDF5()
  {
    throw HDF5Error("HDF5 hid_t or hsize_t have incorrect type in HDF5Object");
  }

}
