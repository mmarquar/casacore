//# RefColumn.cc: Abstract base class for a table column
//# Copyright (C) 1994,1995,1996,1997
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

#include <aips/Arrays/Vector.h>
#include <aips/Tables/RefColumn.h>
#include <aips/Tables/RefTable.h>
#include <aips/Lattices/IPosition.h>


RefColumn::RefColumn (const BaseColumnDesc* bcdp,
		      RefTable* reftab, BaseColumn* bcp)
: BaseColumn (bcdp),
  refTabPtr_p(reftab),
  colPtr_p   (bcp)
{}

RefColumn::~RefColumn()
{}


Bool RefColumn::isWritable() const
    { return colPtr_p->isWritable(); }

Bool RefColumn::isStored() const
    { return colPtr_p->isStored(); }

TableRecord& RefColumn::rwKeywordSet()
    { return colPtr_p->rwKeywordSet(); }
TableRecord& RefColumn::keywordSet()
    { return colPtr_p->keywordSet(); }


uInt RefColumn::nrow() const
    { return refTabPtr_p->nrow(); }

void RefColumn::initialize (uInt startRow, uInt endRow)
{
    uInt rownr;
    for (uInt i=startRow; i<endRow; i++) {
	rownr = refTabPtr_p->rootRownr(i);
	colPtr_p->initialize (rownr, rownr);
    }
}

void RefColumn::setShape (uInt rownr, const IPosition& shape)
    { colPtr_p->setShape (refTabPtr_p->rootRownr(rownr), shape); }

void RefColumn::setShape (uInt rownr, const IPosition& shape,
			  const IPosition& tileShape)
    { colPtr_p->setShape (refTabPtr_p->rootRownr(rownr), shape, tileShape); }

uInt RefColumn::ndimColumn() const
    { return colPtr_p->ndimColumn(); }

IPosition RefColumn::shapeColumn() const
    { return colPtr_p->shapeColumn(); }

uInt RefColumn::ndim (uInt rownr) const
    { return colPtr_p->ndim (refTabPtr_p->rootRownr(rownr)); }

IPosition RefColumn::shape(uInt rownr) const
    { return colPtr_p->shape (refTabPtr_p->rootRownr(rownr)); }

Bool RefColumn::isDefined (uInt rownr) const
    { return colPtr_p->isDefined (refTabPtr_p->rootRownr(rownr)); }


Bool RefColumn::canAccessSlice (Bool& reask) const
    { return colPtr_p->canAccessSlice (reask); }

Bool RefColumn::canChangeShape() const
    { return colPtr_p->canChangeShape(); }


void RefColumn::get (uInt rownr, void* dataPtr) const
    { colPtr_p->get (refTabPtr_p->rootRownr(rownr), dataPtr); }

void RefColumn::getSlice (uInt rownr, const Slicer& ns, void* dataPtr) const
    { colPtr_p->getSlice (refTabPtr_p->rootRownr(rownr), ns, dataPtr); }

void RefColumn::put (uInt rownr, const void* dataPtr)
    { colPtr_p->put (refTabPtr_p->rootRownr(rownr), dataPtr); }

void RefColumn::putSlice (uInt rownr, const Slicer& ns, const void* dataPtr)
    { colPtr_p->putSlice (refTabPtr_p->rootRownr(rownr), ns, dataPtr); }


ColumnCache& RefColumn::columnCache()
    { return colCache_p; }


void RefColumn::makeSortKey (Sort& sortobj, ObjCompareFunc* cmpFunc,
			     Int order, const void*& dataSave)
    { colPtr_p->makeRefSortKey (sortobj, cmpFunc, order,
				refTabPtr_p->rowNumbers(), dataSave); }

void RefColumn::freeSortKey (const void*& dataSave)
    { colPtr_p->freeSortKey (dataSave); }

void RefColumn::allocIterBuf (void*& lastVal, void*& curVal,
			      ObjCompareFunc*& cmpFunc)
    { colPtr_p->allocIterBuf (lastVal, curVal, cmpFunc); }

void RefColumn::freeIterBuf (void*& lastVal, void*& curVal)
    { colPtr_p->freeIterBuf (lastVal, curVal); }
