//# TiledStMan.cc: Storage manager for tables using tiled hypercubes
//# Copyright (C) 1995,1996
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

#include <aips/Tables/TiledStMan.h>
#include <aips/Tables/TSMColumn.h>
#include <aips/Tables/TSMDataColumn.h>
#include <aips/Tables/TSMCoordColumn.h>
#include <aips/Tables/TSMIdColumn.h>
#include <aips/Tables/TSMCube.h>
#include <aips/Tables/TSMFile.h>
#include <aips/Tables/Table.h>
#include <aips/Tables/TableDesc.h>
#include <aips/Tables/ColumnDesc.h>
#include <aips/Arrays/Vector.h>
#include <aips/Lattices/IPosition.h>
#include <aips/Utilities/DataType.h>
#include <aips/Utilities/String.h>
#include <aips/Utilities/BinarySearch.h>
#include <aips/Utilities/GenSort.h>
#include <aips/IO/AipsIO.h>
#include <aips/Tables/DataManError.h>



TiledStMan::TiledStMan ()
: DataManager       (),
  nrrow_p           (0),
  nrdim_p           (0),
  nrCoordVector_p   (0),
  persMaxCacheSize_p(0),
  maxCacheSize_p    (0),
  fileSet_p         (1, (TSMFile*)0),
  userSetCache_p    (False)
{}

TiledStMan::TiledStMan (const String& hypercolumnName, uInt maximumCacheSize)
: DataManager       (),
  nrrow_p           (0),
  nrdim_p           (0),
  nrCoordVector_p   (0),
  persMaxCacheSize_p(maximumCacheSize),
  maxCacheSize_p    (maximumCacheSize),
  fileSet_p         (1, (TSMFile*)0),
  hypercolumnName_p (hypercolumnName),
  userSetCache_p    (False)
{}

TiledStMan::~TiledStMan()
{
    uInt i;
    for (i=0; i<ncolumn(); i++) {
	delete colSet_p[i];
    }
    for (i=0; i<cubeSet_p.nelements(); i++) {
	delete cubeSet_p[i];
    }
    for (i=0; i<fileSet_p.nelements(); i++) {
	delete fileSet_p[i];
    }
}

IPosition TiledStMan::makeTileShape (const IPosition& hypercubeShape,
				     uInt nrPixelsPerTile)
{
    Vector<double> weight(hypercubeShape.nelements());
    weight = double(1);
    return makeTileShape (hypercubeShape, weight, nrPixelsPerTile);
}
IPosition TiledStMan::makeTileShape (const IPosition& hypercubeShape,
				     const Vector<double>& weight,
				     uInt nrPixelsPerTile)
{
    uInt nrdim = hypercubeShape.nelements();
    if (weight.nelements() != nrdim) {
	throw (TSMError ("makeTileShape: nelements mismatch"));
    }
    double nrLeft = nrPixelsPerTile;
    Vector<double> tmpShape(nrdim);
    IPosition tileShape(nrdim);
    tileShape = 0;
    uInt i;
    // Iterate until the tile shape is set nicely.
    // This is needed to prevent tile shape dimensions from underflow
    // or overflow.
    while (True) {
	double prod = 1;
	uInt n = 0;
	for (i=0; i<nrdim; i++) {
	    if (tileShape(i) == 0) {
		prod *= hypercubeShape(i) * weight(i);
		n++;
	    }
	}
	// Exit if nothing left.
	if (n == 0) {
	    break;
	}
	double factor = pow (nrLeft / prod, double(1) / n);
	double maxDiff;
	double diff;
	Int maxIndex = -1;
	// Calculate the tile shape for the remaining dimensions.
	// Determine the greatest difference in case of underflow/overflow.
	// (note that the reciproke is used, thus in fact the minimum matters).
	// That tile dimension will be set and the iteration starts again.
	for (i=0; i<nrdim; i++) {
	    if (tileShape(i) == 0) {
		diff = hypercubeShape(i) * weight(i) * factor;
		tmpShape(i) = diff;
		if (diff > 1) {
		    diff = hypercubeShape(i) / diff;
		}
		if (maxIndex < 0  ||  diff < maxDiff) {
		    maxDiff  = diff;
		    maxIndex = i;
		}
	    }
	}
	// If there is no underflow/overflow we can copy the dimensions
	// and exit.
	if (maxDiff >= 1) {
	    for (i=0; i<nrdim; i++) {
		if (tileShape(i) == 0) {
		    tileShape(i) = Int(tmpShape(i) + 0.5);   // round-off
		}
	    }
	    break;
	}
	// Set the dimension with the greatest difference.
	if (tmpShape(maxIndex) < 1) {
	    tileShape(maxIndex) = 1;
	}else{
	    tileShape(maxIndex) = hypercubeShape(maxIndex);
	    nrLeft /= tileShape(maxIndex);
	}
    }
    // Optimize the tile shape by recalculating tile length for the same
    // number of tiles.
    for (i=0; i<nrdim; i++) {
	uInt nrtile = (hypercubeShape(i) + tileShape(i) - 1) / tileShape(i);
	tileShape(i) = (hypercubeShape(i) + nrtile - 1) / nrtile;
    }
    return tileShape;
}


String TiledStMan::dataManagerName() const
    { return hypercolumnName_p; }


void TiledStMan::reopenRW()
{
    for (uInt i=0; i<fileSet_p.nelements(); i++) {
	fileSet_p[i]->bucketFile()->setRW();
    }
}


//# Does the storage manager allow to add rows? (yes)
Bool TiledStMan::canAddRow() const
{
    return True;
}


const IPosition& TiledStMan::hypercubeShape (uInt rownr)
{
    return getHypercube(rownr)->cubeShape();
}

const IPosition& TiledStMan::tileShape (uInt rownr)
{
    return getHypercube(rownr)->tileShape();
}

void TiledStMan::setCacheSize (uInt rownr,
			       const IPosition& sliceShape,
			       const IPosition& windowStart,
			       const IPosition& windowLength,
			       const IPosition& axisPath, Bool forceSmaller)
{
    // Set the cache size for the given hypercube.
    getHypercube(rownr)->setCacheSize (sliceShape, windowStart, windowLength,
				       axisPath, forceSmaller);
    userSetCache_p = True;
}

void TiledStMan::setCacheSize (uInt rownr, uInt nbytes, Bool forceSmaller)
{
    // Set the cache size (in buckets) for the given hypercube.
    TSMCube* hypercube = getHypercube(rownr);
    hypercube->setCacheSize (nbytes / hypercube->bucketSize(), forceSmaller);
    userSetCache_p = True;
}

void TiledStMan::clearCaches()
{
    for (uInt i=0; i<cubeSet_p.nelements(); i++) {
	if (cubeSet_p[i] != 0) {
	    cubeSet_p[i]->clearCache();
	}
    }
    userSetCache_p = False;
}

void TiledStMan::showCacheStatistics (ostream& os) const
{
    for (uInt i=0; i<cubeSet_p.nelements(); i++) {
	if (cubeSet_p[i] != 0) {
	    cubeSet_p[i]->showCacheStatistics (os);
	}
    }
}

TSMCube* TiledStMan::singleHypercube()
{
    if (cubeSet_p.nelements() != 1  ||  cubeSet_p[0] == 0) {
	throw (TSMError ("TiledStMan: function on hypercolumn " +
			 hypercolumnName_p + " cannot be done "
			 "when it is using multiple hypercubes"));
    }
    return cubeSet_p[0];
}


uInt TiledStMan::getLengthOffset (uInt nrPixels, Block<uInt>& dataOffset,
				  Block<uInt>& localOffset,
				  uInt& localTileLength) const
{
    localTileLength = 0;
    uInt length = 0;
    uInt nrcol = dataCols_p.nelements();
    dataOffset.resize (nrcol);
    localOffset.resize (nrcol);
    for (uInt i=0; i<nrcol; i++) {
	dataOffset[i] = length;
	localOffset[i] = localTileLength;
	length += dataCols_p[i]->dataLength (nrPixels);
	localTileLength += nrPixels * dataCols_p[i]->localPixelSize();
    }
    return length;
}

void TiledStMan::readTile (char* local,
			   const Block<uInt>& localOffset,
			   const char* external,
			   const Block<uInt>& externalOffset,
			   uInt nrPixels)
{
    uInt nr = dataCols_p.nelements();
    for (uInt i=0; i<nr; i++) {
	dataCols_p[i]->readTile (local + localOffset[i],
				 external + externalOffset[i],
				 nrPixels);
    }
}

void TiledStMan::writeTile (char* external,
			    const Block<uInt>& externalOffset,
			    const char* local,
			    const Block<uInt>& localOffset,
			    uInt nrPixels)
{
    uInt nr = dataCols_p.nelements();
    for (uInt i=0; i<nr; i++) {
	dataCols_p[i]->writeTile (external + externalOffset[i],
				  local + localOffset[i],
				  nrPixels);
    }
}


DataManagerColumn* TiledStMan::makeScalarColumn (const String& columnName,
						 int dataType,
						 const String& dataTypeId)
{
    return makeIndArrColumn (columnName, dataType, dataTypeId);
}
DataManagerColumn* TiledStMan::makeDirArrColumn (const String& columnName,
						 int dataType,
						 const String& dataTypeId)
{
    return makeIndArrColumn (columnName, dataType, dataTypeId);
}
DataManagerColumn* TiledStMan::makeIndArrColumn (const String& columnName,
						 int dataType,
						 const String&)
{
    //# Check if data type is not TpOther.
    throwDataTypeOther (columnName, dataType);
    //# Extend colSet_p block if needed.
    if (ncolumn() >= colSet_p.nelements()) {
	colSet_p.resize (colSet_p.nelements() + 32);
    }
    TSMColumn* colp = new TSMColumn (this, dataType, columnName);
    if (colp == 0) {
	throw (AllocError ("TiledStMan::makeIndArrColumn", 1));
    }
    colSet_p[ncolumn()] = colp;
    return colp;
}

int TiledStMan::coordinateDataType (const String& columnName) const
{
    for (uInt i=0; i<coordColSet_p.nelements(); i++) {
	if (coordColSet_p[i] != 0) {
	    if (columnName == coordColSet_p[i]->columnName()) {
		return coordColSet_p[i]->dataType();
	    }
	}
    }
    throw (TSMError ("coordinateDataType: column " + columnName +
		     " is unknown"));
    return 0;
}

// Get the proper array data type.
int TiledStMan::arrayDataType (int dataType) const
{
    switch (dataType) {
    case TpBool:
	return TpArrayBool;
    case TpChar:
	return TpArrayChar;
    case TpUChar:
	return TpArrayUChar;
    case TpShort:
	return TpArrayShort;
    case TpUShort:
	return TpArrayUShort;
    case TpInt:
	return TpArrayInt;
    case TpUInt:
	return TpArrayUInt;
    case TpFloat:
	return TpArrayFloat;
    case TpDouble:
	return TpArrayDouble;
    case TpComplex:
	return TpArrayComplex;
    case TpDComplex:
	return TpArrayDComplex;
    case TpString:
	return TpArrayString;
    }
    return dataType;
}


IPosition TiledStMan::defaultTileShape() const
{
    return IPosition();
}


Bool TiledStMan::canReallocateColumns() const
    { return True; }

DataManagerColumn* TiledStMan::reallocateColumn (DataManagerColumn* column)
{
    for (uInt i=0; i<ncolumn(); i++) {
	if (column == colSet_p[i]) {
	    TSMColumn* ptr = colSet_p[i];
	    colSet_p[i] = ptr->unlink();
	    delete ptr;
	    return colSet_p[i];
	}
    }
    // The column is not part of this storage manager, so return column itself.
    return column;
}
    

void TiledStMan::setup()
{
    uInt i;
    // Determine if the data has to be stored in canonical format.
    // For the time being this is the only format.
    asCanonical_p = True;
    // Get the description of the hypercolumn.
    Vector<String> dataNames;
    Vector<String> coordNames;
    Vector<String> idNames;
    const TableDesc& tableDesc = table().tableDesc();
    nrdim_p = tableDesc.hypercolumnDesc (hypercolumnName_p, dataNames,
					 coordNames, idNames);
    // Determine the number of vector coordinates.
    // This is the dimensionality of the cells.
    nrCoordVector_p = tableDesc.columnDesc(dataNames(0)).ndim();
    // Check if the required columns are bound
    // and get the pointers to those columns.
    dataCols_p.resize (dataNames.nelements());
    dataColSet_p.resize (dataNames.nelements());
    coordColSet_p.resize (nrdim_p);
    idColSet_p.resize (idNames.nelements());
    uInt nrDataBound = getBindings (dataNames, dataColSet_p, True);
    uInt nrCoordBound = getBindings (coordNames, coordColSet_p, False);
    uInt nrIdBound = getBindings (idNames, idColSet_p, True);
    // Check if no non-TiledStMan columns are bound.
    if (nrDataBound + nrCoordBound + nrIdBound  !=  ncolumn()) {
	throw (TSMError ("non-TiledStMan columns bound"));
    }
    // Let the derived class do some more checks.
    setupCheck (tableDesc, dataNames);
    // Find the first fixed shape data column.
    // Check if FixedShape column shapes of data and coordinate columns match.
    for (i=0; i<dataColSet_p.nelements(); i++) {
	fixedCellShape_p = dataColSet_p[i]->shapeColumn();
	if (fixedCellShape_p.nelements() > 0) {
	    break;
	}
    }
    checkShapeColumn (fixedCellShape_p);
    // Construct the various TSMColumn objects.
    for (i=0; i<coordColSet_p.nelements(); i++) {
	if (coordColSet_p[i] != 0) {
	    coordColSet_p[i] = coordColSet_p[i]->makeCoordColumn (i);
	}
    }
    for (i=0; i<idColSet_p.nelements(); i++) {
	idColSet_p[i] = idColSet_p[i]->makeIdColumn();
    }
    uInt nrd = dataColSet_p.nelements();
    PtrBlock<TSMDataColumn*> dataColSet(nrd);
    for (i=0; i<nrd; i++) {
	dataColSet[i] = dataColSet_p[i]->makeDataColumn();
    }
    // Organize the pixel offset in the data columns in descending
    // order of external pixel length.
    // The sort is stable, so equal lengths will always occur in
    // the same order.
    // In that way we are sure that their data are aligned in a tile
    // (which may be needed for TSMCube::accessLine).
    Block<uInt> lengths(nrd);
    for (i=0; i<nrd; i++) {
	lengths[i] = dataColSet[i]->tilePixelSize();
    }
    Vector<uInt> inx;
    GenSortIndirect<uInt>::sort (inx, lengths, nrd, Sort::Descending);
    // Rearrange the objects and set their column number.
    // In this way function setLengths will behave correctly.
    for (i=0; i<nrd; i++) {
	dataCols_p[i] = dataColSet[inx(i)];
	dataCols_p[i]->setColumnNumber (i);
	dataColSet_p[i] = dataCols_p[i];
    }
}


void TiledStMan::setupCheck (const TableDesc&,
			     const Vector<String>&) const
{}

void TiledStMan::checkCubeShape (const TSMCube* hypercube,
				 const IPosition& cubeShape) const
{
    // Check if the dimensionalities are correct.
    if (cubeShape.nelements() != nrdim_p) {
	throw (TSMError ("addHypercube dimensionality mismatch"));
    }
    // Check if all dimensions are > 0.
    // Only the last one in shape can be 0 (meaning extensible).
    for (uInt i=0; i<nrdim_p-1; i++) {
	if (cubeShape(i) == 0) {
	    throw (TSMError ("addHypercube dimensions are zero"));
	}
    }
    // Check if cube shape matches fixed shaped columns.
    checkShapeColumn (cubeShape);
    // Check if cube shape matches possibly already defined coordinates.
    if (hypercube != 0) {
	checkCoordinatesShapes (hypercube, cubeShape);
    }
}

void TiledStMan::checkShapeColumn (const IPosition& shape) const
{
    // There is nothing to check if no shape is given.
    if (shape.nelements() == 0) {
	return;
    }
    uInt i;
    // First check if fixed data columns match.
    for (i=0; i<dataColSet_p.nelements(); i++) {
	const IPosition& shapeColumn = dataColSet_p[i]->shapeColumn();
	for (uInt j=0; j<shapeColumn.nelements(); j++) {
	    if (shape(j) != shapeColumn(j)) {
		throw (TSMError ("Mismatch in fixed shape of data column "
				 + dataColSet_p[i]->columnName()));
	    }
	}
    }
    for (i=0; i<nrCoordVector_p; i++) {
	if (coordColSet_p[i] != 0) {
	    const IPosition& shapeColumn = coordColSet_p[i]->shapeColumn();
	    if (shapeColumn.nelements() > 0) {
		if (shape(i) != shapeColumn(0)) {
		    throw (TSMError
			     ("Mismatch in fixed shape of coordinate column "
			      + coordColSet_p[i]->columnName()));
		}
	    }
	}
    }
}

void TiledStMan::checkCoordinatesShapes (const TSMCube* hypercube,
					 const IPosition& cubeShape) const
{
    //# Check for all coordinates if their length (if defined)
    //# matches the hypercube shape.
    for (uInt i=0; i<coordColSet_p.nelements(); i++) {
	if (coordColSet_p[i] != 0) {
	    uInt size = hypercube->coordinateSize
		                            (coordColSet_p[i]->columnName());
	    if (size != 0  &&  size != cubeShape(i)) {
		throw (TSMError ("Mismatch in shape of coordinate column "
				 + coordColSet_p[i]->columnName()));
	    }
	}
    }
}


void TiledStMan::initCoordinates (TSMCube* hypercube)
{
    for (uInt i=0; i<coordColSet_p.nelements(); i++) {
	if (coordColSet_p[i] != 0) {
	    hypercube->extendCoordinates (Record(),
					  coordColSet_p[i]->columnName(),
					  hypercube->cubeShape()(i));
	}
    }
}


uInt TiledStMan::getBindings (const Vector<String>& columnNames,
			      PtrBlock<TSMColumn*>& colSet,
			      Bool mustExist) const
{
    colSet = (TSMColumn*)0;
    uInt nrfound = 0;
    uInt j;
    Bool found = False;
    for (uInt i=0; i<columnNames.nelements(); i++) {
	for (j=0; j<ncolumn(); j++) {
	    if (columnNames(i) == colSet_p[j]->columnName()) {
		colSet[i] = colSet_p[j];
		found = True;
		nrfound++;
		break;
	    }
	}
	if (!found  &&  mustExist) {
	    throw (TSMError ("TiledStMan column " + columnNames(i) +
			     " is not bound"));
	}
    }
    return nrfound;
}


TSMCube* TiledStMan::makeHypercube (const IPosition& cubeShape,
				    const IPosition& tileShape,
				    const Record& values)
{
    //# Check if the cube shape is correct.
    checkCubeShape (0, cubeShape);
    // Check whether all id and coordinate values are given correctly.
    checkValues (idColSet_p, values);
    checkCoordinates (coordColSet_p, cubeShape, values);
    // Check whether no double id values are given.
    if (getCubeIndex (values) >= 0) {
	throw (TSMError ("addHypercube with already existing id values"));
    }
    // Pick a TSMFile object for the hypercube.
    // Non-extensible cubes share the first file; others get their own file.
    uInt filenr = 0;
    if (cubeShape(nrdim_p - 1) == 0) {
	filenr = fileSet_p.nelements();
	fileSet_p.resize (filenr + 1);
	fileSet_p[filenr] = 0;
    }
    // Create the file when needed.
    if (fileSet_p[filenr] == 0) {
	createFile (filenr);
    }
    // Create a TSMCube object.
    // Its data will be written at the end of the file.
    TSMCube* hypercube = new TSMCube (this, fileSet_p[filenr],
				      cubeShape, tileShape, values);
    if (hypercube == 0) {
	throw (AllocError ("TiledStMan::addHypercube", 1));
    }
    return hypercube;
}

void TiledStMan::createFile (uInt index)
{
    TSMFile* file = new TSMFile (this, index);
    if (file == 0) {
	throw (AllocError ("TiledStMan::addHypercube", 1));
    }
    fileSet_p[index] = file;
}


Int TiledStMan::getCubeIndex (const Record& idValues) const
{
    // When there are no id columns, return the one and single hypercube
    // (or -1 if no one created yet).
    if (idColSet_p.nelements() == 0) {
	if (cubeSet_p.nelements() == 0) {
	    return -1;
	}
	return 0;
    }
    // Look if a hypercube matches the id values.
    for (uInt i=0; i<cubeSet_p.nelements(); i++) {
	if (cubeSet_p[i]->matches (idColSet_p, idValues)) {
	    return i;
	}
    }
    return -1;
}


void TiledStMan::checkValues (const PtrBlock<TSMColumn*>& colSet,
			      const Record& values) const
{
    // Check if all values are given and if their data types match.
    for (uInt i=0; i<colSet.nelements(); i++) {
	if (colSet[i] != 0) {
	    const String& name = colSet[i]->columnName();
	    if (! values.isDefined (name)) {
		throw (TSMError ("No value given for column " + name));
	    }
	    if (values.dataType(name) != colSet[i]->dataType()) {
		throw (TSMError ("Data type mismatch for column " + name));
	    }
	}
    }
}

void TiledStMan::checkCoordinates (const PtrBlock<TSMColumn*>& coordColSet,
				   const IPosition& cubeShape,
				   const Record& values) const
{
    // Check if the coordinates data types and shapes are correct,
    // i.e. if the coordinates shapes match the hypercube shape.
    for (uInt i=0; i<coordColSet.nelements(); i++) {
	if (coordColSet[i] != 0) {
	    const String& name = coordColSet[i]->columnName();
	    if (values.isDefined (name)) {
		int dataType = arrayDataType (coordColSet[i]->dataType());
		if (values.dataType(name) != dataType) {
		    throw (TSMError ("Data type mismatch for coordinate " +
				     name));
		}
		IPosition shape = values.shape (name);
		if (shape.nelements() != 1) {
		    throw (TSMError ("Values of coordinate " + name +
				     " do not form a vector"));
		}
		if (shape(0) != cubeShape(i)) {
		    throw (TSMError ("Shape mismatch for coordinate " + name));
		}
	    }
	}
    }
}


uInt TiledStMan::addedNrrow (const IPosition& shape, uInt incrInLastDim) const
{
    uInt nrrowAdded = 1;
    for (uInt i=nrCoordVector_p; i<nrdim_p-1; i++) {
	nrrowAdded *= shape(i);
    }
    return nrrowAdded * incrInLastDim;
}


AipsIO* TiledStMan::headerFileCreate()
{
    AipsIO* file = new AipsIO (fileName(), ByteIO::New);
    if (file == 0) {
	throw (AllocError ("TiledStMan::headerFileCreate", 1));
    }
    return file;
}

AipsIO* TiledStMan::headerFileOpen()
{
    AipsIO* file = new AipsIO (fileName());
    if (file == 0) {
	throw (AllocError ("TiledStMan::headerFileCreate", 1));
    }
    return file;
}



void TiledStMan::headerFilePut (AipsIO& headerFile, uInt nrCube)
{
    uInt i;
    headerFile.putstart ("TiledStMan", 1);
    //# Write StMan sequence number, the number of rows and columns,
    //# and the column data types.
    //# This is only done to check it when reading back.
    headerFile << sequenceNr();
    headerFile << nrrow_p;
    headerFile << ncolumn();
    for (i=0; i<ncolumn(); i++) {
	headerFile << colSet_p[i]->dataType();
    }
    headerFile << hypercolumnName_p;
    headerFile << persMaxCacheSize_p;
    headerFile << nrdim_p;
    headerFile << fileSet_p.nelements();
    for (i=0; i<fileSet_p.nelements(); i++) {
	if (fileSet_p[i] == 0) {
	    headerFile << False;
	}else{
	    headerFile << True;
	    fileSet_p[i]->putObject (headerFile);
	}
    }
    headerFile << nrCube;
    for (i=0; i<nrCube; i++) {
	cubeSet_p[i]->putObject (headerFile);
    }
    headerFile.putend();
}

void TiledStMan::headerFileGet (AipsIO& headerFile, uInt tabNrrow)
{
    nrrow_p = tabNrrow;
    uInt i;
    headerFile.getstart ("TiledStMan");
    //# Get and check the number of rows and columns and the column types.
    uInt nrrow, nrcol, seqnr;
    int  dtype;
    headerFile >> seqnr;
    headerFile >> nrrow;
    headerFile >> nrcol;
    if (seqnr != sequenceNr()  ||  nrrow != nrrow_p  ||  nrcol != ncolumn()) {
	throw (DataManInternalError
	          ("TiledStMan::headerFileGet: mismatch in seqnr,#row,#col"));
    }
    for (i=0; i<ncolumn(); i++) {
	headerFile >> dtype;
	if (dtype != colSet_p[i]->dataType()) {
	    throw (DataManInternalError
		      ("TiledStMan::headerFileGet: mismatch in data type"));
	}
    }
    headerFile >> hypercolumnName_p;
    headerFile >> persMaxCacheSize_p;
    maxCacheSize_p = persMaxCacheSize_p;
    // Setup the various things (i.e. initialize other variables).
    setup();
    uInt nrdim;
    headerFile >> nrdim;
    if (nrdim != nrdim_p) {
	throw (DataManInternalError
	              ("TiledStMan::headerFileGet: mismatch in nrdim"));
    }
    uInt nrFile;
    Bool flag;
    headerFile >> nrFile;
    fileSet_p.resize (nrFile);
    for (i=0; i<nrFile; i++) {
	headerFile >> flag;
	if (flag) {
	    fileSet_p[i] = new TSMFile (this, headerFile, i);
	    if (fileSet_p[i] == 0) {
		throw (AllocError ("TiledStMan::headerFileGet" ,1));
	    }
	}
    }
    uInt nrCube;
    headerFile >> nrCube;
    cubeSet_p.resize (nrCube);
    for (i=0; i<nrCube; i++) {
	cubeSet_p[i] = new TSMCube (this, headerFile);
	if (cubeSet_p[i] == 0) {
	    throw (AllocError ("TiledStMan::headerFileGet" ,1));
	}
    }
    headerFile.getend();
}

void TiledStMan::headerFileClose (AipsIO* headerFile)
{
    delete headerFile;
}


TSMFile* TiledStMan::getFile (uInt sequenceNumber)
{
    //# Do internal check to see if TSMFile really exists.
    if (sequenceNumber >= fileSet_p.nelements()
    ||  fileSet_p[sequenceNumber] == 0) {
	throw (DataManInternalError ("TiledStMan::getFile"));
    }
    return fileSet_p[sequenceNumber];
}
