//# MSDataDescIndex: index or lookup in a MeasurementSet DATA_DESC subtable
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
//#
//# $Id$

#if !defined(AIPS_MSDATADESCINDEX_H)
#define AIPS_MSDATADESCINDEX_H

//# includes
#include <aips/aips.h>
#include <aips/MeasurementSets/MSDataDescription.h>
#include <aips/MeasurementSets/MSDataDescColumns.h>
#include <aips/Arrays/Vector.h>
#include <aips/Utilities/String.h>

//# forward declarations

// <summary>
// Class to handle lookup or indexing into a MS DATA_DESC subtable
// </summary>

// <use visibility=export>
//
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> MeasurementSet
//   <li> MSDataDescription
// </prerequisite>
//
// <etymology>
// From "MeasurementSet", "DATA_DESC subtable" and "index".
// </etymology>
//
// <synopsis>
// This class provides lookup and indexing into an MS DATA_DESC
// subtable. These services include returning rows numbers
// (which for the DATA_DESC subtable are DATA_DESC_ID's) associated 
// with specific data in the subtable.
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// Collect together all subtable indexing and lookup for the
// DATA_DESC subtable, for encapsulation and efficiency.
// </motivation>
//
// <thrown>
//    <li>
//    <li>
// </thrown>
//

class MSDataDescIndex 
{
public:
  // Construct from an MS DATA_DESC subtable
  MSDataDescIndex(const MSDataDescription& dataDescription);

  // Null destructor
  virtual ~MSDataDescIndex() {};

  // Look up DATA_DESC_ID's for a given spectral window id
  Vector<Int> matchSpwId(const Int& spwId);
  Vector<Int> matchSpwId(const Vector<Int>& spwIds);

private:
  // Disallow null constructor
  MSDataDescIndex();

  // DATA_DESC subtable column accessor
  ROMSDataDescColumns msDataDescCols_p;

  // Vector cache of DataDesc id's
  Vector<Int> dataDescIds_p;
  Int nrows_p;
};

#endif
    
