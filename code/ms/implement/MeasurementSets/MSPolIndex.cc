//# MSPolIndex.cc: implementation of MSPolIndex.h
//# Copyright (C) 2000,2001,2002
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

#include <trial/MeasurementSets/MSPolIndex.h>
#include <aips/Arrays/MaskedArray.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Arrays/ArrayUtil.h>

//-------------------------------------------------------------------------

MSPolarizationIndex::MSPolarizationIndex(const MSPolarization& 
					 polarizationTable)
  : msPolarizationCols_p(polarizationTable)
{ 
// Construct from an MS POLARIZATION subtable
// Input:
//    polarizationTable   const MSPolarization&   Input MSPolarization 
//                                                sub-table
// Output to private data:
//    msPolarizationCols_p   ROMSPolarizationColumns   MSPolarization columns 
//                                                     accessor
//    polarizationIds_p      Vector<Int>               Polarization id.'s
//    nrows_p                Int                       Number of rows
//
  // Generate an array of polarization id's, used in later queries
  nrows_p = msPolarizationCols_p.nrow();
  polarizationIds_p.resize(nrows_p);
  indgen(polarizationIds_p);
};

//-------------------------------------------------------------------------

Vector<Int> MSPolarizationIndex::matchCorrTypeAndProduct(const Vector<Int>& 
							 corrType,
							 const Matrix<Int>&
							 corrProduct)
{
// Match a set of polarization correlation types and receptor cross-products
// Input:
//    corrType       const Vector<Int>&       Set of polarization correlation
//                                            types (as defined in Stokes.h)
//    corrProduct    const Matrix<Int>&       Set of receptor cross-products
// Output:
//    matchCorrTypeAndProduct   Vector<Int>   Matching polarization id.'s
//

  // Match the polarization correlation types and receptor cross-products 
  // by row and correlation index
  uInt numCorr = min(corrType.nelements(), corrProduct.ncolumn());
  uInt nrows = msPolarizationCols_p.nrow();
  Vector<Bool> corrMatch(nrows, False);
  for (uInt row=0; row<nrows; row++) {
    Vector<Int> rowCorrType;
    msPolarizationCols_p.corrType().get(row, rowCorrType);
    Matrix<Int> rowCorrProduct;
    msPolarizationCols_p.corrProduct().get(row, rowCorrProduct);
    corrMatch(row) = (rowCorrType.nelements() == numCorr &&
		      rowCorrProduct.ncolumn() == numCorr);

    if (corrMatch(row)) {
      for (uInt i=0; i < numCorr; i++) {
	corrMatch(row) = (corrMatch(row) &&
			  rowCorrType(i) == corrType(i) &&
			  rowCorrProduct(0,i) == corrProduct(0,i) &&
			  rowCorrProduct(1,i) == corrProduct(1,i));
      };
    };
  };

  LogicalArray maskArray(corrMatch);
  MaskedArray<Int> maskRowNumbers(polarizationIds_p, maskArray);
  return maskRowNumbers.getCompressedArray();
};

//-------------------------------------------------------------------------
