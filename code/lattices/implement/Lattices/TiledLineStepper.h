//# TiledStepper.h:  Steps a Vector cursor optimally through a tiled Lattice
//# Copyright (C) 1997
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

#if !defined(AIPS_TILED_STEPPER_H)
#define AIPS_TILED_STEPPER_H

#include <aips/aips.h>
#include <trial/Lattices/LatticeNavigator.h>
#include <trial/Lattices/LatticeIndexer.h>
#include <aips/Lattices/IPosition.h>

#if defined(_AIX)
#pragma implementation ("TiledStepper.cc")
#endif 

// <summary> traverse a Tiled Lattice optimally with a Vector cursor </summary>
// <use visibility=export>

// <reviewed reviewer="" date="" tests="">

// <prerequisite>
// <list>
//   <li> <linkto class=LatticeNavigator> LatticeNavigator </linkto>
// </list>
// </prerequisite>

// <etymology>
// TiledStepper is used to step a Vector cursor optimally through a Lattice
// that is tiled.
// </etymology>

// <synopsis> 
// When you wish to traverse a Lattice (say, a PagedArray or an Image) you
// will usually create a LatticeIterator.  Once created, you may attach a
// LatticeNavigator to the iterator. A TiledStepper, is a concrete class
// derived from the abstract LatticeNavigator that allows you to move
// a Vector cursor through the Lattice in a way that will minimise the
// amount of cache memory consumed.
//
// Some Lattices (in particular PagedArrays) are stored (on disk) in
// tiles. For an N-dimensional Lattice a tile is is an N-dimensional
// subsection with fewer elements along each axis. For example a Lattice of
// shape [512,512,4,32] may have a tile shape of [32,16,4,16], and there
// will be 16*32*1*2 (=1024) tiles in the entire Lattice. To allow efficient
// access of the data in a Lattice some tiles are cached in memory. As each
// tile may consume a fair bit of memory (in this example 128kBytes,
// assuming each element consumes 4 bytes), it is desirable to minimise the
// number of tiles held in the cache. But it is also desirable to minimise
// the number of times a tiles must be read into or written from the
// cache as this may require a time consuming operation like disk I/O.
//
// Now suppose you wanted to traverse a Lattice with a Vector cursor of
// length 512 pixel aligned along the x-axis. Using a
// <linkto class=LatticeStepper>LatticeStepper</linkto>, each Vector is
// retrieved from the Lattice sequentially and without any consideration of
// the underlying tile shape. What is the optimal cache size for the above
// example?
//
// Suppose we have a cache size of 16 ie., the number of tiles along the
// x-axis. Then Vectors beginning at positions [0,0,0,0] to [0,15,0,0] will
// be stored in the cache. But the next Vector beginning at position
// [0,16,0,0] will flush the cache and read in another 16 tiles. This I/O
// takes time and will occur 16 times for each plane in the four dimensional
// Lattice. Further when the cursor moves to position [0,0,1,0] the 16 tiles
// that where initially in the cache will need to be read again. To avoid
// all this cache I/O it is better to have a bigger cache.
//
// Suppose the cache size is 16*32 (=512) ie., enough tiles to contain an
// (x,y)-plane. Then the cache size will not be flushed until the cursor is
// moved to position [0,0,0,16]. Further the cache will never need to read
// back into memory tiles that had previously been stored in there. The
// cache is big enough to store tiles until they have been completely
// used. But this cache is 64MBytes in size, and consumes too much memory
// for many computers.
//
// This where a tiledStepper is useful. Because it knows the shape of the
// tiles in the underlying Lattice it moves the cursor to return all the
// Vectors in the smallest possible cache of tiles before moving on to the
// next set of tiles. Using the above example again, the tiledStepper will
// move the beginning of the Vector cursor in the following pattern.
// <srcblock>
// [0,0,0,0], [0,1,0,0], [0,2,0,0], ... [0,15,0,0]
// [0,0,1,0], [0,1,1,0],            ... [0,15,1,0], 
//                                  ... [0,15,3,0],
// [0,0,0,1], ...                       [0,15,3,15]
// </srcblock>
// Moving the Vector cursor through all 16*4*16 (=1024 positions) can be
// done by caching only 16 tiles in memory (those along the x-axis). Hence
// the cache size need only be 2MBytes in size. Further once all 1024
// vectors have been returned it is not necessary to read these 16 tiles
// back into memory. All the data in those tiles has already been
// accessed. Using a TiledStepper rather than a LatticeStepper has, in this
// example, resulted in a drop in the required cache size from 64MBytes down
// to 2MBytes. 
//
// In constructing a TiledStepper, you specify the Lattice shape, the
// tile shape and the axis the Vector cursor will be aligned with. Specifying
// an axis=0 will align the cursor with the x-axis and axis=2 will produce a
// cursor that is along the z-axis. The length of the cursor is always the
// same as the number of elements in the Lattice along the axis the cursor
// is aligned with.
//
// The cursor position can be incremented or decremented to retrieve the
// next Vector in the Lattice. The position of the next Vector in the
// Lattice will depend on the tile shape, and is described above.
//
// This navigator does not currently support sub-sectioning.
//
// </synopsis> 

// <example>
// This example is of a global function that will do a 2-D inplace
// complex Fourier transform of an arbitrary large Lattice (which
// must have at least two dimensions).
//
// A two dimensional transform is done by successive one dimensional
// transforms along all the rows and then all the columns in the
// lattice. Scoping is used to destroy iterators once they have been
// used. This frees up the cache memory associated with the cursor in each
// iterator.
//
// <srcblock>
// void FFT2DComplex(Lattice<Complex>& cArray,
// 		  const Int direction)
// {
//   const uInt ndim = cArray.ndim();
//   AlwaysAssert(ndim > 1, AipsError);
//   const IPosition latticeShape = cArray.shape();
//   const uInt nx=latticeShape(0);
//   const uInt ny=latticeShape(1);
//   const IPosition tileShape = cArray.niceCursorShape(cArray.maxPixels());
//
//   {
//     TiledStepper tsx(latticeShape, tileShape, 0);
//     LatticeIterator<Complex> lix(cArray, tsx);
//     FFTServer<Float,Complex> fftx(IPosition(1, nx));
//     for (lix.reset();!lix.atEnd();lix++)
//       fftx.cxfft(lix.vectorCursor(), direction);
//   }
//   {
//     TiledStepper tsy(latticeShape, tileShape, 1);
//     LatticeIterator<Complex> liy(cArray, tsy);
//     FFTServer<Float,Complex> ffty(IPosition(1, ny));
//     for (liy.reset();!liy.atEnd();liy++)
//       ffty.cxfft(liy.vectorCursor(), direction);
//   }
// };
// </srcblock>
// </example>

// <motivation>
// Moving through a Lattice by equal sized chunks, and without regard
// to the nature of the data, is a basic and common procedure.  
// </motivation>
//
// <todo asof="1997/03/28">
//  <li> Provide support for Matrix and higher dimensional cursors
//       can be used.
//  <li> Provide support for Lattice subsectioning.
// </todo>

class TiledStepper: public LatticeNavigator {
public:

  // Construct a TiledStepper by specifying the Lattice shape, a tile shape
  // and the axis along which the Vector cursor will lie (0 means the
  // x-axis). Is is nearly always advisable to make the tileShape identical
  // to the Lattice tileShape. This can be obtained by
  // <src>lat.niceCursorShape(lat.maxPixels())</src> where <src>lat</src> is
  // a Lattice object.
  TiledStepper(const IPosition & latticeShape, 
	       const IPosition & tileShape, 
	       const uInt axis);

  // the copy constructor which uses copy semantics.
  TiledStepper(const TiledStepper &other);
    
  // destructor (does nothing)
  ~TiledStepper();

  // The assignment operator which uses copy semantics.
  TiledStepper &operator=(const TiledStepper &other);

  // Increment operator (postfix or prefix version) - move the cursor
  // forward one step. Returns True if the cursor was moved.  Both functions
  // do the same thing.
  // <group>
  virtual Bool operator++(); 
  virtual Bool operator++(Int);
  // </group>

  // Decrement operator (postfix or prefix version) - move the cursor
  // backwards one step. Returns True if the cursor was moved. Both
  // functions do the same thing.
  // <group>
  virtual Bool operator--();
  virtual Bool operator--(Int);
  // </group>

  // Function to move the cursor to the beginning of the Lattice. Also
  // resets the number of steps (<src>nsteps</src> function) to zero. 
  virtual void reset();

  // Function which returns "True" if the cursor is at the beginning of the
  // Lattice, otherwise, returns "False"
  virtual Bool atStart() const;

  // Function which returns "True" if an attempt has been made to increment
  // the cursor beyond the end of the Lattice.
  virtual Bool atEnd() const;

  // Function to return the number of steps (increments & decrements) taken
  // since construction (or since last reset).  This is a running count of
  // all cursor movement (operator++ or operator--), even though a
  // N-incremensts followed by N-decrements will always leave the cursor in
  // the original position.
  virtual uInt nsteps() const;

  // Function which returns the current position of the beginning of the
  // cursor. The <src>position</src> function is relative to the origins
  // in the main Lattice.
  // <group>
  virtual IPosition position() const;
  // </group>

  // Functions which returns the current position of the end of the
  // cursor. The <src>endPosition</src> function is relative the origins
  // in the main Lattice.
  // <group>
  virtual IPosition endPosition() const;
  // </group>

  // Functions which returns the shape of the Lattice being iterated
  // through. <src>latticeShape</src> always returns the shape of the main
  // Lattice.
  // <group>
  virtual IPosition latticeShape() const;
  // </group>

  // Function which returns the shape of the cursor. This always includes
  // all axes (ie. it includes degenerates axes)
  virtual IPosition cursorShape() const;

  // Function which returns the shape of the "tile" the cursor will iterate
  // through before moving onto the next tile. THIS IS NOT THE SAME AS THE
  // TILE SHAPE USED BY THE LATTICE. It is nearly the same except that the
  // axis the cursor is aligned with is replaced by the shape of the Lattice
  // on that axis. eg., If a Lattice has a shape of [512,512,4,32] and a
  // tile shape of [32,16,4,16] then <src>tileShape()</src> will return
  // [512,16,4,16] if the cursor is along the x-axis and [32,512,4,16] if the
  // cursor is along the y-axis.
  IPosition tileShape() const;

  // Function which returns "True" if the increment/decrement operators have
  // moved the cursor position such that part of the cursor beginning or end
  // is hanging over the edge of the Lattice. This always returns False.
  virtual Bool hangOver() const;

  // Return the bottom left hand corner (blc), top right corner (trc) or
  // step size (increment) used by the current Lattice. These always return
  // blc=0, trc=latticeShape-1, increment=1
  // <group>
  virtual IPosition blc() const;
  virtual IPosition trc() const;
  virtual IPosition increment() const;
  // </group>

  // Function which returns a pointer to dynamic memory of an exact copy 
  // of this instance.  The pointer returned by this function must
  // be deleted externally.
  virtual LatticeNavigator* clone() const;

  // Function which checks the internal data of this class for correct
  // dimensionality and consistant values. 
  // Returns True if everything is fine otherwise returns False
  virtual Bool ok() const;

  // Use until RTTI is available in g++. Does not make a copy, merely
  // returns a cast pointer.  Only implementers need to worry about this
  // function.
  // <group>
  virtual TiledStepper* castToTiler();
  virtual const TiledStepper* castToConstTiler() const;
  // </group>

private:
  // prevent the default constructor from being used externally
  TiledStepper();

  LatticeIndexer theIndexer;//# For stepping within a tile
  LatticeIndexer theTiler;  //# For stepping between tiles
  IPosition theCursorPos;   //# The current position of the iterator.
  IPosition theCursorShape; //# The shape of the cursor
  IPosition theAxisPath;    //# Path for traverseing within and between tiles
  uInt theNsteps;           //# the number of iterator steps taken thus far; 
                            //# set to 0 on reset()
  Bool theEnd;              //# is the cursor beyond the end?
  Bool theStart;            //# is the cursor at the beginning?
};

#endif
