//# ImageStatistics.cc: generate statistics from an image
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
//# $Id$

#include <trial/Images/ImageStatistics.h>

#include <aips/Arrays/Matrix.h>
#include <trial/Coordinates.h>  
#include <aips/Exceptions/Error.h>
#include <aips/Logging/LogIO.h>
#include <trial/Images/ImageUtilities.h>
#include <trial/Images/ImageInterface.h>
#include <trial/Images/ImageExprParse.h>
#include <aips/Lattices/TempLattice.h>
#include <trial/Lattices/LatticeStatistics.h>
#include <trial/Lattices/LattStatsSpecialize.h>
#include <aips/Mathematics/Math.h>
#include <aips/Utilities/Assert.h>
#include <aips/Utilities/LinearSearch.h>
#include <aips/Utilities/String.h>
#include <aips/Utilities/DataType.h>


#include <iostream.h>
#include <iomanip.h>
#include <stdlib.h>
#include <strstream.h>


// Public functions

template <class T>
ImageStatistics<T>::ImageStatistics (const ImageInterface<T>& image,
                                     LogIO& os, 
                                     Bool showProgress,
                                     Bool forceDisk)
// 
// Constructor
//
: LatticeStatistics<T>(image, os, showProgress, forceDisk),
  pInImage_p(0),
  lelExpr_p("")
{
   if (!setNewImage(image)) {
      os_p << error_p << LogIO::EXCEPTION;
   }
}


template <class T>
ImageStatistics<T>::ImageStatistics (const ImageInterface<T>& image,
                                     Bool showProgress,
                                     Bool forceDisk)
// 
// Constructor
//
: LatticeStatistics<T>(image, showProgress, forceDisk),
  pInImage_p(0),
  lelExpr_p("")
{
   if (!setNewImage(image)) {
      os_p << error_p << LogIO::EXCEPTION;
   }
}


template <class T>
ImageStatistics<T>::ImageStatistics(const ImageStatistics<T> &other) 
//
// Copy constructor.  Storage image is not copied.
//
: LatticeStatistics<T>(other),
  pInImage_p(0)
{
   if (pInImage_p!=0) delete pInImage_p;
   pInImage_p = other.pInImage_p->cloneII();
}


template <class T>
ImageStatistics<T> &ImageStatistics<T>::operator=(const ImageStatistics<T> &other)
//
// Assignment operator.  Storage image is not copied
//
{
   if (this != &other) {
      LatticeStatistics<T>::operator=(other);
//
      if (pInImage_p!=0) delete pInImage_p;
      pInImage_p = other.pInImage_p->cloneII();
      lelExpr_p = other.lelExpr_p;
   }
   return *this;
}


 

template <class T>
ImageStatistics<T>::~ImageStatistics()
//
// Destructor.  
//
{
   delete pInImage_p;
   pInImage_p = 0;
}



template <class T>
Bool ImageStatistics<T>::setNewImage(const ImageInterface<T>& image)
{ 
   if (!goodParameterStatus_p) {
      return False;
   }

// Make a clone of the image

   if (pInImage_p!=0) delete pInImage_p;
   pInImage_p = image.cloneII();


// Pass it on to LatticeStatistics

   goodParameterStatus_p = setNewLattice(image);
//
   return goodParameterStatus_p;
}



template <class T>
Bool ImageStatistics<T>::getBeamArea (Double& beamArea) const
//
// Get beam volume if present.  ALl this beamy stuff should go to
// a class called GaussianBeam and be used by GaussianCOnvert as well
//
{
   beamArea = -1.0;
   ImageInfo ii = pInImage_p->imageInfo();
   Vector<Quantum<Double> > beam = ii.restoringBeam();
   CoordinateSystem cSys = pInImage_p->coordinates();
   String imageUnits = pInImage_p->units().getName();
   imageUnits.upcase();
//
   Int afterCoord = -1;   
   Int dC = cSys.findCoordinate(Coordinate::DIRECTION, afterCoord);
   if (beam.nelements()==3 && dC!=-1 && imageUnits==String("JY/BEAM")) {
      DirectionCoordinate dCoord = cSys.directionCoordinate(dC);
      Vector<String> units(2);
      units(0) = "rad"; units(1) = "rad";
      dCoord.setWorldAxisUnits(units);
      Vector<Double> deltas = dCoord.increment();
//
      Double major = beam(0).getValue(Unit("rad"));
      Double minor = beam(1).getValue(Unit("rad"));
      beamArea = 1.1331 * major * minor / abs(deltas(0) * deltas(1));
      return True;
   } else {
      return False;
   }
}


template <class T>
Bool ImageStatistics<T>::listStats (Bool hasBeam, const IPosition& dPos,
                                    const Matrix<T>& stats)
//
// List the statistics for this row to the logger
//
// Inputs:
//   dPos    The location of the start of the cursor in the
//           storage image for this row
//   stats   Statistics matrix
// Outputs:
//   Bool    Indicates coordinate transformations failed
//
{
   if (!haveLogger_p) {

// We will consider this situation as successful

      return True;
   }

   os_p << endl;

// Set up the manipulators. We list the number of points as an integer so find
// out how big the field width needs to be.  Min of 6 so label fits.

   T* dummy(0);
   DataType type = whatType(dummy);
   Int oDWidth = 14;
   if (type==TpComplex) {
      oDWidth = 2*oDWidth + 3;    // (x,y)
   }


// Have to convert LogIO object to ostream before can apply
// the manipulators

   Int oPrec = 6;
   setStream(os_p.output(), oPrec);

// Write the pixel and world coordinate of the higher order display axes to the logger

   if (displayAxes_p.nelements()>1) {
      String hLabel, xLabel;
      getLabels(hLabel, xLabel, dPos);
      os_p << hLabel << endl;
   }

// Find the width of the field into which we are going to write the coordinate value
// of the first display axis.  Do this by formatting a dummy value.

   Vector<String> sWorld(1);
   Vector<Double> pixels(1);
   pixels(0) = 1.0;
   IPosition blc(pInImage_p->ndim(),0);
   IPosition trc(pInImage_p->shape()-1);

   CoordinateSystem cSys = pInImage_p->coordinates();
   ImageUtilities::pixToWorld(sWorld, cSys,
                              displayAxes_p(0), cursorAxes_p, 
                              blc, trc, pixels, -1);
   String cName = 
     ImageUtilities::shortAxisName(cSys.worldAxisNames()(displayAxes_p(0)));
   Int oCWidth = max(cName.length(), sWorld(0).length()) + 1;
   
// Write headers

   const uInt nStatsAxes = cursorAxes_p.nelements();
   os_p << endl;
   Int len0;
   if (nStatsAxes == 1) {
      os_p << "Profile ";
      len0 = 8;
   }
   else if (nStatsAxes == 2) {
      os_p << "Plane ";
      len0 = 6;
   }
   else if (nStatsAxes == 3) {
      os_p << "Cube ";
      len0 = 5;
   }
   else {
      os_p << "Hyper-cube ";
      len0 = 11;
   }

   os_p.output() << setw(oCWidth) << cName;
   os_p.output() << setw(oDWidth) << "Npts";
   os_p.output() << setw(oDWidth) << "Sum";
   if (hasBeam) os_p.output() << setw(oDWidth) << "FluxDensity";
   os_p.output() << setw(oDWidth) << "Mean"; 
   if (doRobust_p) os_p.output() << setw(oDWidth) << "Median"; 
   os_p.output() << setw(oDWidth) << "Rms";
   os_p.output() << setw(oDWidth) << "Sigma";
   if (doneLEL_p) os_p.output() << setw(oDWidth) << "LEL"; 
   os_p.output() << setw(oDWidth) << "Minimum";
   os_p.output() << setw(oDWidth) << "Maximum" << endl;


// Convert pixel coordinates Vector of the first display axis to world coordinates

   const uInt n1 = stats.shape()(0);
   sWorld.resize(n1);
   pixels.resize(n1);
//
   for (uInt j=0; j<n1; j++) pixels(j) = Double(j);
   if (!ImageUtilities::pixToWorld(sWorld, cSys,
                              displayAxes_p(0), cursorAxes_p, 
                              blc, trc, pixels, -1)) return False;


// Write statistics to logger.  We write the pixel location
// relative to the parent image

   for (uInt j=0; j<n1; j++) {
      os_p.output() << setw(len0)     << j+blcParent_p(displayAxes_p(0))+1;
      os_p.output() << setw(oCWidth)   << sWorld(j);
//
      ostrstream os00; setStream(os00, oPrec);
      os00 << stats.column(NPTS)(j);   
//
      os_p.output() << setw(oDWidth)   << String(os00);   
//
      if (LattStatsSpecialize::hasSomePoints(stats.column(NPTS)(j))) {

// I hate ostrstreams.  The bloody things are one shot.
   
         ostrstream os0, os1, os2, os3, os4, os5, os6, os7, os8, os9;
         setStream(os0, oPrec); setStream(os1, oPrec); setStream(os2, oPrec);
         setStream(os3, oPrec); setStream(os4, oPrec); setStream(os5, oPrec);
         setStream(os6, oPrec); setStream(os7, oPrec); setStream(os8, oPrec);
         setStream(os9, oPrec); 
//
         os0 << stats.column(SUM)(j);
         if (hasBeam) os1 << stats.column(FLUX)(j);
         os2 << stats.column(MEAN)(j);
         if (doRobust_p) os8 << stats.column(MEDIAN)(j);
         os3 << stats.column(RMS)(j);
         os4 << stats.column(SIGMA)(j);
         os5 << stats.column(MIN)(j);
         os6 << stats.column(MAX)(j);
         os9 << stats.column(LEL)(j);
//
         os_p.output() << setw(oDWidth)   << String(os0);
         if (hasBeam) os_p.output() << setw(oDWidth)   << String(os1);
         os_p.output() << setw(oDWidth)   << String(os2);
         if (doRobust_p) os_p.output() << setw(oDWidth)   << String(os8);
         if (doneLEL_p) os_p.output() << setw(oDWidth)   << String(os9);
         os_p.output() << setw(oDWidth)   << String(os3);
         os_p.output() << setw(oDWidth)   << String(os4);
         os_p.output() << setw(oDWidth)   << String(os5);
         os_p.output() << setw(oDWidth)   << String(os6);
      }
      os_p.output() << endl;
   }
   os_p.post();

   return True;
}


template <class T> 
String ImageStatistics<T>::formatCoordinate (const IPosition& pos)
{
   const CoordinateSystem& cS = pInImage_p->coordinates();
   CoordinateSystem cSys(cS);                 // Non-const
   Vector<Double> pixel(cSys.nPixelAxes());
   Vector<Double> world(cSys.nWorldAxes());
   for (uInt i=0; i<pixel.nelements(); i++) pixel(i) = pos(i);
   Bool ok = cSys.toWorld(world, pixel);
   if (!ok) {
      String err = String("Error converting coordinate position because ") + cSys.errorMessage();
      throw(AipsError(err));
   }
//
   Vector<String> s(cSys.nPixelAxes(),"");
   Vector<String> u(cSys.nPixelAxes(),"");
   for (uInt i=0; i<world.nelements(); i++) {
      Int pixelAxis = cSys.worldAxisToPixelAxis(i);
      String tmp = cSys.format(u(pixelAxis), Coordinate::DEFAULT, world(i), i, 
                               True, -1, False);
      if (u(pixelAxis).empty()) {
        s(pixelAxis) = tmp;
      } else {
        s(pixelAxis) = tmp + u(pixelAxis);
      }
   }
//
   String s2;
   for (uInt i=0; i<cSys.nPixelAxes(); i++) {
     if (i==0) {
        s2 += s(i);
     } else {
        s2 += String(", ") + s(i);
     }
   }
   return s2;
}


template <class T>
void ImageStatistics<T>::getLabels(String& hLabel, String& xLabel, const IPosition& dPos) const
//
// Get labels for top of plot and listing for the higher order axes
// and get the label for the X-axis when plotting
//
{
   CoordinateSystem cSys = pInImage_p->coordinates();
   xLabel = cSys.worldAxisNames()(displayAxes_p(0)) + " (pixels)";
//
   hLabel =String("");
   const uInt nDisplayAxes = displayAxes_p.nelements();
   ostrstream oss;
   if (nDisplayAxes > 1) {
      Vector<String> sWorld(1);
      Vector<Double> pixels(1);
      IPosition blc(pInImage_p->ndim(),0);
      IPosition trc(pInImage_p->shape()-1);
//
      for (uInt j=1; j<nDisplayAxes; j++) {
         Int worldAxis = cSys.pixelAxisToWorldAxis(displayAxes_p(j));
         String name = cSys.worldAxisNames()(worldAxis);
         pixels(0) = Double(locInLattice(dPos,False)(j));
//
         if (!ImageUtilities::pixToWorld (sWorld, cSys,
                                     displayAxes_p(j), cursorAxes_p,
                                     blc, trc, pixels, -1)) return;
//
         oss <<  ImageUtilities::shortAxisName(name)
             << " = " << locInLattice(dPos,True)(j)+1 << " (" << sWorld(0) << ")";
         if (j < nDisplayAxes-1) oss << ", ";
      }
      hLabel = String(oss);
   }
}


template <class T>
void ImageStatistics<T>::listMinMax(ostrstream& osMin,
                                    ostrstream& osMax,
                                    Int oWidth, DataType type)
{
   if (!fixedMinMax_p) {

// Find world coordinates of min and max. We list pixel coordinates
// of min/max relative to the start of the parent lattice


      String minPosString = formatCoordinate (minPos_p);
      String maxPosString = formatCoordinate (maxPos_p);
//
      os_p << "Minimum value "; 
      os_p.output() << setw(oWidth) << String(osMin);
      if (type==TpFloat) {
         os_p <<  " at " << blcParent_p + minPos_p+1 << " (" << minPosString << ")" << endl;
      }
      os_p.post();
//
      os_p << "Maximum value ";
      os_p.output() << setw(oWidth) << String(osMax);
      if (type==TpFloat) {
         os_p <<  " at " << blcParent_p + maxPos_p+1 << " (" << maxPosString << ")" << endl;
      }
      os_p << endl;
      os_p.post();
   }
}

template <class T>
Bool ImageStatistics<T>::getLEL (Array<T>& stats, const String& expr, Bool dropDeg)
{
   if (expr.empty()) {
      os_p << LogIO::WARN << "The expression string is empty" << LogIO::POST;
      stats.resize();
      return True;
   } 
//
   LatticeExprNode node = ImageExprParse::command (expr);
   Bool newExpr = (expr!=lelExpr_p);
   lelExpr_p = expr;
   return getLELNode (stats, node, dropDeg, newExpr);
}

