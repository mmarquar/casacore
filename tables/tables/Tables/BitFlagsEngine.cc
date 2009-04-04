//# BitFlagsEngine.cc: Implementation of helper class.
//# Copyright (C) 2009
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
#include <tables/Tables/BitFlagsEngine.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableColumn.h>
#include <tables/Tables/TableRecord.h>
#include <casa/Arrays/Array.h>
#include <casa/Containers/Record.h>
#include <casa/BasicSL/String.h>


namespace casa { //# NAMESPACE CASA - BEGIN

  BFEngineMask::BFEngineMask (uInt mask)
    : itsMask (mask)
  {}

  BFEngineMask::BFEngineMask (const Array<String>& keys,
                              uInt defaultMask)
    : itsMask     (defaultMask),
      itsMaskKeys (keys)
  {}

  void BFEngineMask::fromRecord (const RecordInterface& spec,
                                 const ROTableColumn& column,
                                 const String& prefix)
  {
    String keyName = prefix + "Mask";
    if (spec.isDefined (keyName)) {
      itsMask = spec.asuInt (keyName);
    }
    keyName += "Keys";
    if (spec.isDefined (keyName)) {
      itsMaskKeys = spec.asArrayString (keyName);
      makeMask (column);
    }
  }

  void BFEngineMask::toRecord (RecordInterface& spec,
                               const String& prefix) const
  {
    spec.define (prefix + "Mask", itsMask);
    spec.define (prefix + "MaskKeys", itsMaskKeys);
  }

  void BFEngineMask::makeMask (const ROTableColumn& column)
  {
    if (! itsMaskKeys.empty()) {
      uInt mask = 0;
      Array<String>::const_iterator iterEnd = itsMaskKeys.end();
      for (Array<String>::const_iterator iter=itsMaskKeys.begin();
           iter!=iterEnd; ++iter) {
        mask = mask | column.keywordSet().asuInt (*iter);
      }
      itsMask = mask;
    }
  }

}
