//# ImageRegrid.h: Regrid Images
//# Copyright (C) 1996,1997,1998,1999,2000,2001
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

#if !defined(TRIAL_IMAGEREGRID_H)
#define TRIAL_IMAGEREGRID_H

#include <aips/aips.h>
#include <aips/Measures/MDirection.h>
#include <aips/Measures/MFrequency.h>
#include <trial/Mathematics/Interpolate2D.h>

template<class T> class MaskedLattice;
template<class T> class ImageInterface;
template<class T> class Lattice;
template<class T> class LatticeIterator;
template<class T> class Vector;
template<class T> class Matrix;
template<class T> class Cube;

class CoordinateSystem;
class DirectionCoordinate;
class Coordinate;
class ObsInfo;
class IPosition;
class Unit;
class ProgressMeter;

// <summary>This regrids one image to match the coordinate system of another</summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class="ImageInterface">ImageInterface</linkto>
//   <li> <linkto class="CoordinateSystem">CoordinateSystem</linkto>
//   <li> <linkto class="Interpolate2D">Interpolate2D</linkto>
//   <li> <linkto class="InterpolateArray1D">InterpolateArray1D</linkto>
// </prerequisite>
//
// <etymology>
//  Regrids, or resamples, images.  
// </etymology>
//
// <synopsis>
//  This class enables you to regrid one image to the coordinate
//  system of another.    You can regrid any or all of the
//  axes in the image.  A range of interpolation schemes are available.
//
//  It will cope with coordinate systems being in different orders
//  (coordinate, world axes, pixel axes).  The basic approach is to
//  make a mapping from the input to the output coordinate systems,
//  but the output CoordinateSystem is preserved.
//
//  Any DirectionCoordinate is regridded with a coupled 2D 
//  interpolation scheme.  All other axes are regridded with
//  a 1D interpolation scheme.    StokesCoordinates cannot be
//  regridded.
//
//  Multiple passes are made through the data, and the output of 
//  each pass is the input of the next pass.  The intermediate 
//  images are stored as TempImages which may be in memory or 
//  on disk, depending on theri size.
//
//  It can also simply insert this image into that one via
//  an integer shift.
// </synopsis>
//
// <example>
// 
// <srcblock>
// </srcblock>
// </example>
//
// <motivation> 
// A common image analysis need is to regrid images, e.g. to compare
// images from different telescopes.
// </motivation>
//
// <thrown>
// <li> AipsError 
// </thrown>
//
// <todo asof="1999/04/20">
//   <li> Reference frame changes (e.g. J2000 -> B1950)
//   <li> 1D interpolation does not handle input masks
// </todo>

template <class T> class ImageRegrid
{
public:

  // Default constructor
  ImageRegrid();

  // copy constructor (copy semantics)
  ImageRegrid(const ImageRegrid &other);

  // destructor
  ~ImageRegrid();

  // Assignment copy semantics)
  ImageRegrid<T>& operator=(const ImageRegrid& other);

  // Regrid inImage onto the grid specified by outImage.
  // If outImage has a writable mask, it will be updated.
  // Specify which pixel axes of outImage are to be
  // regridded
  void regrid(ImageInterface<T>& outImage, 
              typename Interpolate2D::Method method,
              const IPosition& whichOutPixelAxes,
	      const ImageInterface<T>& inImage,
              Bool replicate=False,
              Bool showProgress=False);

  // Inserts inImage into outImage.  The alignment is done by
  // placing the reference pixel of inImage at the specified reference
  // pixel of outImage.  Only integral shifts are done.
  // The CoordinateSystem of outImage is overwitten by that of
  // inImage and the new reference pixel.  If outImage has a mask,
  // it will be updated.
  // Returns False if no overlap of images, in which case the
  // output is not updated.
  Bool insert(ImageInterface<T>& outImage,
              const Vector<Double>& outReferencePixel,
              const ImageInterface<T>& inImage);

  // Print out useful debugging information (level 0 is none,
  // 1 is some, 2 is too much)
  void showDebugInfo(Int level=0) {itsShowLevel = level;};

  // Enable/disable Measures Reference conversions
  void disableReferenceConversions(Bool disable=True) {itsDisableConversions = disable;};

  // Helper function.  We are regridding from cSysIn to cSysOut for the
  // specified axes.  This function returns a CoordinateSystem which,
  // for the axes being regridded, copies the coordinates from cSysOut.
  // For the axes not being regridded, it copies the coordinates from
  // cSysIn.  This helps you build cSysOut for function regrid.
  static CoordinateSystem makeCoordinateSystem(LogIO& os,
                                               const CoordinateSystem& cSysOut,
                                               const CoordinateSystem& cSysIn,
                                               const IPosition& axes);

 private:

  Int itsShowLevel;
  Bool itsDisableConversions;
  
  // Check shape and axes.  Exception if no good.  If pixelAxes
  // of length 0, set to all axes according to shape
  void checkAxes(IPosition& outPixelAxes,
                 const IPosition& inShape,
                 const IPosition& outShape,
                 const Vector<Int>& pixelAxisMap,
                 const CoordinateSystem& outCoords);

  // Copy data and mask
  void copyDataAndMask(MaskedLattice<T>& out,
                       MaskedLattice<T>& in) const;

  // Find maps between coordinate systems
  void findMaps (uInt nDim, 
                 Vector<Int>& pixelAxisMap1,
                 Vector<Int>& pixelAxisMap2,
                 const CoordinateSystem& inCoords,
                 const CoordinateSystem& outCoords) const;

  // Find scale factor to conserve flux 
   Double findScaleFactor(const Unit& units, 
                          const DirectionCoordinate& dirIn, 
                          const DirectionCoordinate& dirOut,
                          LogIO& os) const;

  // Regrid one Coordinate
   void regridOneCoordinate (LogIO& os, IPosition& outShape2,
                             Vector<Bool>& doneOutPixelAxes,
                             MaskedLattice<T>* &finalOutPtr,  
                             MaskedLattice<T>* &inPtr,   
                             MaskedLattice<T>* &outPtr,  
                             CoordinateSystem& outCoords,
                             const CoordinateSystem& inCoords,
                             const Vector<Int>& pixelAxisMap1,
                             const Vector<Int>& pixelAxisMap2,
                             Int outPixelAxis,
                             const ImageInterface<T>& inImage,
                             const IPosition& outShape,
                             Bool replicate,
                             Bool outIsMasked, Bool showProgress,
                             typename Interpolate2D::Method method);

  // Regrid, via coordinates, 2 coupled axes
  void regrid2D (MaskedLattice<T>& outLattice,
                 const MaskedLattice<T>& inLattice,   
                 const DirectionCoordinate& inCoord,
                 const DirectionCoordinate& outCoord,   
                 const Vector<Int> inPixelAxes, 
                 const Vector<Int> outPixelAxes,
                 const Vector<Int> pixelAxisMap1,
                 const Vector<Int> pixelAxisMap2,
                 typename Interpolate2D::Method method,
                 MDirection::Convert& machine, Bool replicate,
                 Bool useMachine, Bool showProgress, Double scale);

  // Make regridding coordinate grid for this cursor.
  void make2DCoordinateGrid (Bool& allFail, Bool&missedIt,
                             Double& minInX, Double& minInY, 
                             Double& maxInX, Double& maxInY,
                             Cube<Double>& in2DPos,
                             Matrix<Bool>& failed,
                             MDirection::Convert& machine, 
                             const DirectionCoordinate& inCoord,
                             const DirectionCoordinate& outCoord,
                             uInt xInAxis, uInt yInAxis,
                             uInt xOutAxis, uInt yOutAxis,
                             const IPosition& inPixelAxes,
                             const IPosition& outPixelAxes,
                             const IPosition& inShape,
                             const IPosition& outPos,
                             const IPosition& cursorShape,
                             Bool useMachine);

  // Make replication coordinate grid for this cursor
   void make2DCoordinateGrid (Cube<Double>& in2DPos,
                              Double& minInX, Double& minInY, 
                              Double& maxInX, Double& maxInY,
                              const Vector<Double>& pixelScale, 
                              uInt xInAxis, uInt yInAxis,
                              uInt xOutAxis, uInt yOutAxis,
                              uInt xInCorrAxis, uInt yInCorrAxis,
                              uInt xOutCorrAxis, uInt yOutCorrAxis,
                              const IPosition& outPos, const IPosition& cursorShape);

  // Make regridding coordinate grid for this axis
   void make1DCoordinateGrid (Block<Float>& xOut,
                              Vector<Bool>& failed,
                              Bool& allFailed,
                              Bool& allGood,
                              const Coordinate& inCoord,
                              const Coordinate& outCoord,
                              Int inAxisInCoordinate,
                              Int outAxisInCoordinate,
                              MFrequency::Convert& machine,
                              Bool useMachine);


  // Make replication coordinate grid for this axis
   void make1DCoordinateGrid (Block<Float>& xOut, Float pixelScale) const;

  // Regrid 1 axis
  void regrid1D (MaskedLattice<T>& outLattice,
                 const MaskedLattice<T>& inLattice,
                 const Coordinate& inCoord,
                 const Coordinate& outCoord,
                 const Vector<Int>& inPixelAxes,
                 const Vector<Int>& outPixelAxes,
                 Int inAxisInCoordinate,
                 Int outAxisInCoordinate,
                 const Vector<Int> pixelAxisMap,
                 typename Interpolate2D::Method method,
                 MFrequency::Convert& machine,
                 Bool replicate,
                 Bool useMachine, Bool showProgress);

//
   void regrid2DMatrix(Lattice<T>& outCursor,
                       LatticeIterator<Bool>*& outMaskIterPtr,
                       const Interpolate2D& interp,  
                                    ProgressMeter*& pProgress,
                                    Double& iPix,
                                    uInt nDim,
                                    uInt xInAxis, uInt yInAxis,
                                    uInt xOutAxis, uInt yOutAxis,
                                    Double scale,
                                    Bool inIsMasked, Bool outIsMasked,
                                    const IPosition& outPos,
                                    const IPosition& outCursorShape,
                                    const IPosition& inChunkShape,
                                    const IPosition& inChunkBlc,
                                    const IPosition& pixelAxisMap2,
                                    Array<T>& inDataChunk,
                                    Array<Bool>*& inMaskChunkPtr,
                                    const Cube<Double>& pix2DPos,
                                    const Matrix<Bool>& failed);
};

 
#endif

