//# TiledColumnStMan.h: Tiled Column Storage Manager
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

#if !defined(AIPS_TILEDCOLUMNSTMAN_H)
#define AIPS_TILEDCOLUMNSTMAN_H

#if defined(_AIX)
#pragma implementation ("TiledColumnStMan.cc")
#endif 

//# Includes
#include <aips/aips.h>
#include <aips/Tables/TiledStMan.h>
#include <aips/Lattices/IPosition.h>
#include <aips/Utilities/String.h>

//# Forward Declarations


// <summary>
// Tiled Column Storage Manager.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="">
// </reviewed>

// <prerequisite>
//# Classes you should understand before using this one.
//   <li> <linkto class=TiledStMan>TiledStMan</linkto>
//   <li> <linkto class=TSMCube>TSMCube</linkto>
//   <li> <linkto class=ROTiledStManAccessor>ROTiledStManAccessor</linkto>
//        for a discussion of the maximum cache size
// </prerequisite>

// <etymology>
// TiledColumnStMan is the Tiled Storage Manager storing
// an entire column as one hypercube.
// </etymology>

// <synopsis>
// TiledColumnStMan is a derivation from TiledStMan, the abstract
// tiled storage manager class. A description of the basics
// of tiled storage managers is given in the
// <linkto module=Tables:TiledStMan>Tables module</linkto> description.
// <p>
// TiledColumnStMan allows the user to create a tiled hypercube for
// an entire data column and extend it in an automatic way.
// It is meant to be used for fixed shaped data which have to
// be accessed in various directions.
// <p>
// The TiledColumnStMan has the following (extra) properties:
// <ul>
//  <li> Addition of a row results in the extension of the hypercube.
//       The data cells in all rows have to have the same shape. Therefore
//       the columns stored by a TiledColumnStMan storage manager
//       have to be fixed shaped (i.e. FixedShape attribute set in their
//       column descriptions).
//  <li> Coordinates for the hypercubes can be defined and (of course)
//       their shapes have to match the hypercube shape.
//       Their values have to be put explicitly (so it is not possible
//       to define them via an extendHypercube call like in
//       <linkto class=TiledDataStMan>TiledDataStMan</linkto>).
//  <li> The tile shape of the hypercube has to be defined by means
//       of the TiledColumnStMan constructor.
// </ul>
// </synopsis> 

// <motivation>
// This tiled storage manager does not require any special action
// (like calling add/extendHypercube) when used with a column
// containing equally shaped arrays.
// </motivation>

// <example>
// <srcblock>
//  // Define the table description and the columns in it.
//  TableDesc td ("", "1", TableDesc::Scratch);
//  td.addColumn (ArrayColumnDesc<float>  ("RA", 1));
//  td.addColumn (ArrayColumnDesc<float>  ("Dec", 1));
//  td.addColumn (ScalarColumnDesc<float> ("Velocity"));
//  td.addColumn (ArrayColumnDesc<float>  ("Image", 2));
//  // Define the 3-dim hypercolumn with its data and coordinate columns.
//  // Note that its dimensionality must be one higher than the dimensionality
//  // of the data cells.
//  td.defineHypercolumn ("TSMExample",
//			  3,
//			  stringToVector ("Image"),
//			  stringToVector ("RA,Dec,Velocity"));
//  // Now create a new table from the description.
//  SetupNewTable newtab("tTiledColumnStMan_tmp.data", td, Table::New);
//  // Create a TiledColumnStMan storage manager for the hypercolumn
//  // and bind the columns to it.
//  // The tile shape has to be specified for the storage manager.
//  TiledColumnStMan sm1 ("TSMExample", IPosition(3,16,32,32));
//  newtab.bindAll (sm1);
//  // Create the table.
//  Table table(newtab);
//  // Define the values for the coordinates of the hypercube.
//  Vector<float> raValues(512);
//  Vector<float> DecValues(512);
//  indgen (raValues);
//  indgen (decValues, float(100));
//  ArrayColumn<float> ra (table, "RA");
//  ArrayColumn<float> dec (table, "Dec");
//  ScalarColumn<float> velocity (table, "Velocity");
//  ArrayColumn<float> image (table, "Image");
//  Cube<float> imageValues(IPosition(2,512,512));
//  indgen (imageValues);
//  // Write some data into the data columns.
//  uInt i;
//  for (i=0; i<64; i++) {
//      table.addRow();
//	image.put (i, imageValues);
//      // The RA and Dec have to be put only once, because they
//      // are the same for each row.
//      if (i == 0) {
//          ra.put (i, raValues);
//          dec.put (i, decValues);
//      }
//      velocity.put (i, float(i));
//  }
// </srcblock>
// </example>

//# <todo asof="$DATE:$">
//# A List of bugs, limitations, extensions or planned refinements.
//# </todo>


class TiledColumnStMan : public TiledStMan
{
public:
    // Create a TiledDataStMan storage manager for the hypercolumn
    // with the given name. The columns used should have the FixedShape
    // attribute set.
    // The hypercolumn name is also the name of the storage manager.
    // The given tile shape will be used.
    // The given maximum cache size (default is unlimited) is persistent,
    // thus will be reused when the table is read back. Note that the class
    // <linkto class=ROTiledStManAccessor>ROTiledStManAccessor</linkto>
    // allows one to overwrite the maximum cache size temporarily.
    // Its description contains a discussion about the effects of
    // setting a maximum cache.
    TiledColumnStMan (const String& hypercolumnName,
		      const IPosition& tileShape,
		      uInt maximumCacheSize = 0);

    ~TiledColumnStMan();

    // Clone this object.
    // It does not clone TSMColumn objects possibly used.
    DataManager* clone() const;

    // Get the type name of the data manager (i.e. TiledColumnStMan).
    String dataManagerType() const;

    // Make the object from the type name string.
    // This function gets registered in the DataManager "constructor" map.
    static DataManager* makeObject (const String& dataManagerType);

private:
    // Create a TiledColumnStMan.
    // This constructor is private, because it should only be used
    // by makeObject.
    TiledColumnStMan();

    // Add rows to the storage manager.
    // This will extend the hypercube.
    void addRow (uInt nrrow);

    // Get the hypercube in which the given row is stored.
    virtual TSMCube* getHypercube (uInt rownr);

    // Get the hypercube in which the given row is stored.
    // It also returns the position of the row in that hypercube.
    virtual TSMCube* getHypercube (uInt rownr, IPosition& position);

    // Check if the hypercolumn definition fits this storage manager.
    virtual void setupCheck (const TableDesc& tableDesc,
			     const Vector<String>& dataNames) const;

    // Flush the data and close the storage manager.
    void close (AipsIO&);

    // Let the storage manager create files as needed for a new table.
    // This allows a column with an indirect array to create its file.
    void create (uInt nrrow);

    // Open the storage manager file for an existing table and read in
    // the data and let the TSMColumn objects read their data.
    void open (uInt nrrow, AipsIO&);


    //# Declare data members.
    IPosition tileShape_p;
};



#endif
