//# IPosition2.cc: A vector of integers, used to index into arrays (for Array<Int>)
//# Copyright (C) 1993,1994,1995,1996
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

//# This source file is not needed if you aren't interested in converting
//# to and from Array<Int>, i.e. if you don't want IPosition's to depend
//# on arrays.

#include <aips/Lattices/IPosition.h>
#include <aips/Arrays/Vector.h>
#include <aips/Exceptions/Error.h>
#include <aips/Utilities/Copy.h>
#include <aips/Utilities/Assert.h>

IPosition::IPosition(const Array<Int> &other)
: size(0), data(0)
{
    if (other.nelements() == 0) {
	return; // Be slightly loose about conformance checking
    }
    if (other.ndim() != 1) {
	throw(AipsError("IPosition::IPosition(const Array<Int> &other) - "
			"other is not one-dimensional"));
    }
    size = other.nelements();
    if (size <= BufferLength) {
        data = buffer_p;
    } else {
	data = new Int[size];
	if (data == 0) {
	    throw(AllocError("IPosition::IPosition(const Array<Int> &other) - "
			     "new[] of internal buffer fails", size));
	}
    }
    Bool del;
    const Int *storage = other.getStorage(del);
    // We could optimize away this copy in the case when storage is already
    // a copy.
    objcopy(data, storage, size);
    other.freeStorage(storage, del);

    DebugAssert(ok(), AipsError);
}

Vector<Int> IPosition::asVector() const
{
    DebugAssert(ok(), AipsError);

    // Make an array which is the correct size
    Vector<Int> retval(nelements());
    for (uInt i=0; i<nelements(); i++)
	retval(i) = (*this)(i);

    return retval;
}
