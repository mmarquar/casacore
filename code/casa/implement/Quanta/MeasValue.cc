//# MeasValue.cc: Base class for values in a Measure
//# Copyright (C) 1996
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
#include <aips/Measures/MeasValue.h>

//# Constants

//# Constructors

//# Destructor
MeasValue::~MeasValue() {}

//# Operators

//# Member functions
void MeasValue::adjust() {}

void MeasValue::adjust(Double &val) {
    val = 1.0;
}

void MeasValue::readjust(Double val) {
    val = 1.0; // Only to suppress warning message
}

//# Global functions
ostream &operator<<(ostream &os, const MeasValue &meas) {
    meas.print(os);
    return os;
}
