//# ImageStatistics.cc: generate statistics from an image
//# Copyright (C) 1996,1997
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

#include <aips/aips.h>
#include <aips/Arrays/Array.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/VectorIter.h>
#include <aips/Arrays/MaskedArray.h>
#include <aips/Arrays/MaskArrMath.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Exceptions/Error.h>
#include <aips/Inputs/Input.h>
#include <aips/Logging/LogIO.h>
#include <aips/Mathematics/Math.h>
#include <aips/Utilities/String.h>
  
#include <trial/Images/ImageUtilities.h>
#include <trial/Images/ImageStatistics.h>
#include <trial/Images/PagedImage.h>
#include <trial/Lattices/ArrayLattice.h>
#include <trial/Lattices/LatticeIterator.h>
#include <trial/Lattices/LatticeStepper.h>


#include <iostream.h>
#include <iomanip.h>
#include <stdlib.h>
#include <strstream.h>


// C wrappers for PGPLOT

extern "C" {
#include <cpgplot.h>
};

// Public functions

template <class T>
ImageStatistics<T>::ImageStatistics (const PagedImage<T>& imageU,
                                     LogIO& osU) : os_p(osU)
//
// Constructor. 
//
{
   goodParameterStatus_p = True;
   pStoreImage_p = 0;
   needStorageImage_p = True;
  
   if (setNewImage(imageU)) {

// range_p is only accessed if !noInclude || !noExclude
// which means setInExcRange has been called and range_p
// filled in

      noInclude_p = True;
      noExclude_p = True;
      doList_p = False;

// Plotting device is empty on construction. nxy_p is only accessed 
// if device is not empty which means setPlotting has been called so
// nxy_p will have been given a default


// Default cursor axes are entire image

      Vector<Int> cursorAxes(pInImage_p->ndim());
      for (Int i=0; i<pInImage_p->ndim(); i++) cursorAxes(i) = i;

      goodParameterStatus_p = setAxes(cursorAxes);
   } else {
      goodParameterStatus_p = False;
   }
}
 

template <class T>
ImageStatistics<T>::~ImageStatistics()
//
// Destructor.  Delete storage image.
//
{
   if (pStoreImage_p != 0) delete pStoreImage_p;
}


template <class T>
Bool ImageStatistics<T>::setAxes (const Vector<Int>& axesU)
//
// This function sets the cursor axes and the display axes
//
{
   if (!goodParameterStatus_p) {
      os_p << LogIO::SEVERE << "Internal class status is bad" << LogIO::POST;
      return False;
   }

// Set cursor arrays
                          
   cursorAxes_p.resize(0);
   cursorAxes_p = axesU;
   ostrstream os;
   if (!ImageUtilities::setCursor(nVirCursorIter_p, cursorShape_p, 
        cursorAxes_p, pInImage_p, True, 2, os)) {
      os_p << LogIO::SEVERE << "Invalid cursor axes given" << LogIO::POST;
      goodParameterStatus_p = False;
      return False;
   }
   
   
// Set display axes array
 
   ImageUtilities::setDisplayAxes (displayAxes_p, cursorAxes_p, pInImage_p->ndim());


// Signal that we have changed the axes and need a new accumulaiton
// image

   needStorageImage_p = True;

   return True;
}


template <class T>
Bool ImageStatistics<T>::setInExCludeRange(const Vector<Double>& includeU,
                                           const Vector<Double>& excludeU)
//
// Assign the desired exclude range
//
{
   if (!goodParameterStatus_p) {
      os_p << LogIO::SEVERE << "Internal class status is bad" << LogIO::POST;
      return False;
   }
      
   ostrstream os;
   if (!ImageUtilities::setIncludeExclude(range_p, noInclude_p, noExclude_p,
                                          includeU, excludeU, os)) {
      os_p << LogIO::SEVERE << "Invalid pixel in/exclusion range" << LogIO::POST;
      goodParameterStatus_p = False;
      return False;
   }


// Signal that we have changed the pixel range and need a new accumulaiton
// image
    
   needStorageImage_p = True;

   return True;
}


template <class T>
Bool ImageStatistics<T>::setList (const Bool& doList)
//
// See if user wants to list statistics as well as plot them
//
{
   if (!goodParameterStatus_p) {
      os_p << LogIO::SEVERE << "Internal class status is bad" << LogIO::POST;
      return False;
   }
      
   doList_p = doList;

   return True;
} 


template <class T>
Bool ImageStatistics<T>::setPlotting(const Vector<Int>& statsToPlotU,
                                     const String& deviceU,
                                     const Vector<Int>& nxyU)
//
// Assign the desired PGPLOT device name and number
// of subplots
//
{     
   if (!goodParameterStatus_p) {
      os_p << LogIO::SEVERE << "Internal class status is bad" << LogIO::POST;
      return False;
   }


// Make sure requested statistics are valid

   statsToPlot_p.resize(0);
   statsToPlot_p = statsToPlotU;
   for (Int i=0; i<statsToPlot_p.nelements(); i++) {
      if (statsToPlot_p(i) < 0 || statsToPlot_p(i) > NSTATS-1) {
         os_p << LogIO::SEVERE << "Invalid statistic requested for display" 
              << endl << LogIO::POST;
         goodParameterStatus_p = False;
         return False;
      }
   }   
   

// Plotting device and subplots.  nxy_p is set to [1,1] if zero length
 
   device_p = deviceU;
   nxy_p.resize(0);
   nxy_p = nxyU;
   ostrstream os;
   if (!ImageUtilities::setNxy(nxy_p, os)) {
      os_p << LogIO::SEVERE << "Invalid number of subplots" << LogIO::POST;
      goodParameterStatus_p = False;
      return False;
   }

// Set mean and sigma if no statistics requested

   if (!device_p.empty() && statsToPlot_p.nelements()==0) {
      statsToPlot_p.resize(2);
      statsToPlot_p(0) = MEAN;
      statsToPlot_p(1) = SIGMA;
   }

   return True;
}


template <class T>
Bool ImageStatistics<T>::setNewImage(const PagedImage<T>& image)
//    
// Assign pointer to image
//
{ 
   if (!goodParameterStatus_p) {
      os_p << LogIO::SEVERE << "Internal class status is bad" << LogIO::POST;
      return False;
   }
  
   pInImage_p = &image;
   DataType imageType = imagePixelType(pInImage_p->name());
  
   if (imageType !=TpFloat && imageType != TpDouble) {
      os_p << LogIO::SEVERE << "Statistics can only be evaluated from images of type : " 
           << TpFloat << " and " << TpDouble << endl << LogIO::POST;
      goodParameterStatus_p = False;
      pInImage_p = 0;
      return False;
   }

// Signal that we have changed the image and need a new accumulation
// image

   needStorageImage_p = True;
   return True;
}



template <class T>
Bool ImageStatistics<T>::display()
// 
// This function displays (plotting and listing) the requested
// statistics as a function of the display axes
//
{
   if (!goodParameterStatus_p) {
     os_p << LogIO::SEVERE << endl 
          << "The internal status of class is bad.  You have ignored errors" << endl
          << "in setting the arguments." << endl << endl << LogIO::POST;
     return False;
   }


// Generate storage image if required

   if (needStorageImage_p) generateStorageImage();


// If we don't have any display axes just summarise the image statistics

   if (displayAxes_p.nelements() == 0) {
     summStats ();
     return True;
   }


// Sizes of axis coordinate arrays

   Int nStatsAxes = cursorAxes_p.nelements();
   Int nDisplayAxes = displayAxes_p.nelements();

    
// Declare an array to hold the averaged pixel index of the statistics axes.

   Vector<Float> sPos(nStatsAxes);
   sPos = 0.0;
   for (Int i=0; i<nStatsAxes; i++) 
      sPos(i) = 0.5*Float(pInImage_p->shape()(cursorAxes_p(i))-1);


// Size of plotting abcissa axis

   n1_p = pStoreImage_p->shape()(0);


// Allocate abcissa and oridnate arrays for plotting and listing

   pAbc_p = new float[n1_p];
   for (i=0; i<n1_p; i++) pAbc_p[i] = i + 1.0;
   for (i=0; i<NSTATS; i++) pOrd_p[i] = new float[n1_p];


// Open plotting device if required and set up some plotting things

   Bool doPlot = False;
   if (!device_p.empty()) {
      if(cpgbeg(0, device_p.chars(), nxy_p(0), nxy_p(1)) != 1) {
         os_p << LogIO::SEVERE << endl << "Couldn't open display device" 
              << endl << LogIO::POST;
         return False;
      }
      doPlot = True;

      cpgask(1);
      cpgsch (1.2);
      cpgsvp(0.1,0.9,0.1,0.9);
   }


// Iterate through storage image. The cursor may be of > 2 dimensions, but only the
// first (first display axsi) and last (statistics) axes are of non-unit size, so it
// is effectively a matrix.  Thus we can use the matrix cursor for fast indexing.

   IPosition cursorShape(pStoreImage_p->ndim(),1);
   cursorShape(0) = pStoreImage_p->shape()(0);
   cursorShape(pStoreImage_p->ndim()-1) = NACCUM;
   RO_LatticeIterator<Double> pixelIterator(*pStoreImage_p, cursorShape);


   for (pixelIterator.reset(); !pixelIterator.atEnd(); pixelIterator++) {


// Convert accumulations to mean, sigma, and rms. Make sure we do all calculations
// with double precision values. 
 
      Matrix<Double> matrix(pixelIterator.matrixCursor());
      for (i=0; i<n1_p; i++) {
         Int nPts = Int(matrix(i,NPTS)+0.1);
         if (nPts > 0) {
            pOrd_p[MEAN][i] = matrix(i,SUM) / matrix(i,NPTS);
            Double tmp = (matrix(i,SUMSQ) - (matrix(i,SUM)*matrix(i,SUM)/matrix(i,NPTS))) / 
                         (matrix(i,NPTS)-1);
            if (tmp > 0.0) {
               pOrd_p[SIGMA][i]  = sqrt(tmp);
            } else {
               pOrd_p[SIGMA][i]  = 0.0;
            }
            pOrd_p[RMS][i] = sqrt(matrix(i,SUMSQ)/matrix(i,NPTS));
         }
      }


// Extract the accumulation values from the cursor matrix into rows (note stupid
// aips++ convention) and copy to float arrays for plotting

      for (i=0; i<NACCUM; i++) {
         Bool deleteIt;
         const Double* pt = pixelIterator.matrixCursor().column(i).getStorage(deleteIt);
         for (Int j=0; j<n1_p; j++) pOrd_p[i][j] = pt[j];
         pixelIterator.matrixCursor().column(i).freeStorage(pt, deleteIt);
      }


// Plot statistics

      if (doPlot) plotStats (pixelIterator.position());


// List statistics

      if (!doPlot || (doPlot && doList_p)) listStats (pixelIterator.position());

    }


// Deallocate memory

   delete [] pAbc_p;
   for (Int j=0; j<NSTATS; j++) delete [] pOrd_p[j];
   if (doPlot) cpgend();
   return True;
}


template <class T>
Bool ImageStatistics<T>::getNPts(Array<T>& stats)
// 
// This function retrieves the NPTS statistics from the
// accumulation image
//
{
// Resize to zero

   stats.resize(IPosition(1,0));


// Check class status

   if (!goodParameterStatus_p) {
     os_p << LogIO::SEVERE << endl 
          << "The internal status of class is bad.  You have ignored errors" << endl
          << "in setting the arguments." << endl << endl << LogIO::POST;
     return False;
   }

// Do it

   retrieveStorageStatistic (stats, Int(NPTS));
   return True;
}


template <class T>
Bool ImageStatistics<T>::getSum(Array<T>& stats)
// 
// This function retrieves the SUM statistics from the
// accumulation image
//
{
// Resize to zero

   stats.resize(IPosition(1,0));

 
// Check class status
 
   if (!goodParameterStatus_p) {
     os_p << LogIO::SEVERE << endl
          << "The internal status of class is bad.  You have ignored errors" << endl
          << "in setting the arguments." << endl << endl << LogIO::POST;
     return False; 
   }

// Do it

   retrieveStorageStatistic (stats, Int(SUM));
   return True;
}


template <class T>
Bool ImageStatistics<T>::getSumSquared (Array<T>& stats)
// 
// This function retrieves the SUMSQ statistics from the
// accumulation image
//
{
// Resize to zero

   stats.resize(IPosition(1,0));

 
// Check class status
 
   if (!goodParameterStatus_p) {
     os_p << LogIO::SEVERE << endl
          << "The internal status of class is bad.  You have ignored errors" << endl 
          << "in setting the arguments." << endl << endl << LogIO::POST;
     return False; 
   }

// Do it

   retrieveStorageStatistic (stats, Int(SUMSQ));
   return True;
}

template <class T>
Bool ImageStatistics<T>::getMin(Array<T>& stats)
// 
// This function retrieves the MIN statistics from the
// accumulation image
//
{
// Resize to zero

   stats.resize(IPosition(1,0));

 
// Check class status
 
   if (!goodParameterStatus_p) {
     os_p << LogIO::SEVERE << endl
          << "The internal status of class is bad.  You have ignored errors" << endl
          << "in setting the arguments." << endl << endl << LogIO::POST;
     return False; 
    }

// Do it

   retrieveStorageStatistic (stats, Int(MIN));
   return True;
}


template <class T>
Bool ImageStatistics<T>::getMax(Array<T>& stats)
// 
// This function retrieves the MAX statistics from the
// accumulation image
//
{
// Resize to zero

   stats.resize(IPosition(1,0));


// Check class status
 
   if (!goodParameterStatus_p) {
     os_p << LogIO::SEVERE << endl
          << "The internal status of class is bad.  You have ignored errors" << endl
          << "in setting the arguments." << endl << endl << LogIO::POST;
     return False; 
   }

// Do it
   retrieveStorageStatistic (stats, Int(MAX));
   return True;
}



template <class T>
Bool ImageStatistics<T>::getMean(Array<T>& stats)
// 
// This function calculates the MEAN statistics from the
// accumulation image
//
{
// Resize to zero

   stats.resize(IPosition(1,0));


// Check class status
 
   if (!goodParameterStatus_p) {
     os_p << LogIO::SEVERE << endl
          << "The internal status of class is bad.  You have ignored errors" << endl
          << "in setting the arguments." << endl << endl << LogIO::POST;
     return False; 
   }

// Do it

   calculateStatistic (stats, Int(MEAN));
   return True;
}


template <class T>
Bool ImageStatistics<T>::getSigma(Array<T>& stats)
// 
// This function calculates the SIGMA statistics from the
// accumulation image
//
{
// Resize to zero

   stats.resize(IPosition(1,0));

 
// Check class status
 
   if (!goodParameterStatus_p) {
     os_p << LogIO::SEVERE << endl
          << "The internal status of class is bad.  You have ignored errors" << endl
          << "in setting the arguments." << endl << endl << LogIO::POST;
     return False; 
   }

// Do it

   calculateStatistic (stats, Int(SIGMA));
   return True;
}


template <class T>
Bool ImageStatistics<T>::getRms(Array<T>& stats)
// 
// This function calculates the RMS statistics from the
// accumulation image
//
{
// Resize to zero

   stats.resize(IPosition(1,0));

 
// Check class status
 
   if (!goodParameterStatus_p) {
     os_p << LogIO::SEVERE << endl
          << "The internal status of class is bad.  You have ignored errors" << endl
          << "in setting the arguments." << endl << endl << LogIO::POST;
     return False; 
   }

// Do it

   calculateStatistic (stats, Int(RMS));
   return True;
}



// Private functions

template <class T>
void ImageStatistics<T>::accumulate (Int& nIter,
                                     const IPosition& cursorPos,
                                     const Array<Float>& cursor)
//
// Main work routine which takes the data in the current cursor and
// accumulates it into a storage or accumulation image at the appropriate
// locations.  Thus it collapses all data on the cursor axes and accumulates
// as a function of the display axes.
//
// Inputs:
//   cursorPos       Location in image of BLC of cursor
//   cursor          Cursor array
// Input/output:
//   nIter           The number of iterations through the image so far. It is
//                   reset to zero every time we do enough iterations to have
//                   worked through the entire virtual cursor (specified by the user's
//                   axes) so that we know we have to reinit min and max.  
{

// Iterate through cursor array by vectors as its faster than
// doing it pixel by pixel

   ReadOnlyVectorIterator<T> posIt(cursor);
   IPosition tMinPos(cursor.ndim()), tMaxPos(cursor.ndim());
   Double sMin, sMax;
   Double sum = 0;
   Double sumsq = 0;
   Double tmp;
   Int nPts = 0;
   Int n1 = posIt.vector().nelements();
   Int i;
   Bool include = Bool(!noInclude_p);
   Bool exclude = Bool(!noExclude_p);


// Iterate 

   Bool init = True;
   while (!posIt.pastEnd()) {
      Int orig = posIt.vector().origin()(0);

      if (include) {

// Inclusion range

         for (i=0; i<n1; i++) {
            tmp = posIt.vector()(i+orig);
            if (tmp >= range_p(0) && tmp <= range_p(1)) {
               if (init) {
                  sMin = tmp + 1;
                  sMax = tmp - 1;
                  init = False;
               }
               accumulate2 (sum, sumsq, sMin, sMax, nPts, tMinPos, tMaxPos,
                            i, posIt.pos(), tmp);
            }
         }
      } else if (exclude) {

// Exclusion range

         for (i=0; i<n1; i++) {
            tmp = posIt.vector()(i+orig);
            if (tmp < range_p(0) || tmp > range_p(1)) {
               if (init) {                
                  sMin = tmp + 1;
                  sMax = tmp - 1;
                  init = False;
               }
               accumulate2 (sum, sumsq, sMin, sMax, nPts, tMinPos, tMaxPos,
                            i, posIt.pos(), tmp);
            }
         }
      } else {

// All data accepted

         if (init) {
            sMin = posIt.vector()(orig) + 1;
            sMax = posIt.vector()(orig) - 1;
            init = False;
         }
         for (i=0; i<n1; i++) accumulate2 (sum, sumsq, sMin, sMax, nPts, tMinPos,
                                           tMaxPos, i, posIt.pos(), 
                                           posIt.vector()(i+orig));
      }

      posIt.next();
   }


// Fill storage image.   


   IPosition storeImagePos(pStoreImage_p->ndim());
   Int lastAxis = pStoreImage_p->ndim() - 1;
   for (i=0; i<lastAxis; i++) storeImagePos(i) = cursorPos(displayAxes_p(i));

   storeImagePos(lastAxis)  = NPTS;
   tmp = (*pStoreImage_p)(storeImagePos);
   tmp = tmp + nPts;
   (*pStoreImage_p)(storeImagePos) = tmp;

   storeImagePos(lastAxis) = SUM;
   tmp = (*pStoreImage_p)(storeImagePos);

   tmp = tmp + sum;
   (*pStoreImage_p)(storeImagePos) = tmp;

   storeImagePos(lastAxis) = SUMSQ;
   tmp = (*pStoreImage_p)(storeImagePos);
   tmp = tmp + sumsq;
   (*pStoreImage_p)(storeImagePos) = tmp;

   storeImagePos(lastAxis) = MIN;
   tmp = (*pStoreImage_p)(storeImagePos);
   if (nIter == 0 || (nIter !=0 && sMin < tmp)) {
     (*pStoreImage_p)(storeImagePos) = sMin;
     minPos_p = cursorPos;
     for (i=0; i<cursor.ndim(); i++) minPos_p(i) += tMinPos(i);
   }

   storeImagePos(lastAxis) = MAX;
   tmp = (*pStoreImage_p)(storeImagePos);
   if (nIter == 0 || (nIter !=0 && sMax > tmp)) {
     (*pStoreImage_p)(storeImagePos) = sMax;
     maxPos_p = cursorPos;
     for (i=0; i<cursor.ndim(); i++) maxPos_p(i) += tMaxPos(i);
   }

// Work out if it is time to initialize the min and max accumulators
// This algorithm will only work if the virtual cursor is worked through
// before the next one is encountered, or the virtual cursor is the 
// whole image

   nIter++;
   if (nIter == nVirCursorIter_p) nIter = 0;

}


template <class T>
void ImageStatistics<T>::accumulate2 (Double& sum,
                                      Double& sumsq,  
                                      Double& sMin, 
                                      Double& sMax,
                                      Int& nPts,
                                      IPosition& tMinPos,
                                      IPosition& tMaxPos,
                                      const Int& i,
                                      const IPosition& pos,
                                      const Double& datum)
{
   nPts++;
   sum += datum;
   sumsq += datum*datum;

   if (datum > sMax) {
      sMax = datum;
      tMaxPos = pos;
      tMaxPos(0) += i;
   }
   if (datum < sMin) {
      sMin = datum;
      tMinPos = pos;
      tMinPos(0) += i;
   }
}


template <class T>
void ImageStatistics<T>::calculateStatistic (Array<T>& slice, const Int& ISTAT)
//
// Calculate desired statistic from storage image and return in array
//
// Input/output:
//  slice      The statistics are returned in this array.  SHould be of zero
//             size on input.  WIll be of zero size on output if there 
//             were no good points.
//
{

// Generate storage image if required

   if (needStorageImage_p) generateStorageImage();

// Fill output

   Int nDim = pStoreImage_p->ndim() - 1;
   if (nDim == 0) {

// No display axes

      Double nPts  = (*pStoreImage_p)(IPosition(1,NPTS));
      if (Int(nPts+0.1) >  0) {
         slice.resize(IPosition(1,1));
         Double sum   = (*pStoreImage_p)(IPosition(1,SUM));
         Double sumSq = (*pStoreImage_p)(IPosition(1,SUMSQ));

         if (ISTAT == MEAN) {
            slice(IPosition(1,0)) = sum / nPts;
         } else if (ISTAT == SIGMA) {
            Double var = (sumSq - sum*sum/nPts)/(nPts-1);
            if (var > 0.0) {
               slice(IPosition(1,0)) = sqrt(var);
            } else {
               slice(IPosition(1,0)) = 0.0;
            }
         } else if (ISTAT == RMS) {
            slice(IPosition(1,0)) = sqrt(sumSq/nPts);
         } else {
           os_p << LogIO::SEVERE << "Internal error" << endl << LogIO::POST;
          slice.resize(IPosition(1,0));
         }
      }
   } else {

// Some display axes present.  First resize image

      IPosition shape(nDim);
      for (Int i=0; i<nDim; i++) shape(i) = pStoreImage_p->shape()(i);
      slice.resize(shape);
      slice = 0.0;

      Int nStatsAxes = cursorAxes_p.nelements();
      Int nDisplayAxes = displayAxes_p.nelements();

    
// Iterate through storage image. The cursor may be of > 2 dimensions, but only the
// first (first display axsi) and last (statistics) axes are of non-unit size, so it
// is effectively a matrix.  Thus we can use the matrix cursor for fast indexing.

      IPosition cursorShape(pStoreImage_p->ndim(),1);
      cursorShape(0) = pStoreImage_p->shape()(0);
      cursorShape(pStoreImage_p->ndim()-1) = NACCUM;
      RO_LatticeIterator<Double> pixelIterator(*pStoreImage_p, cursorShape);

      VectorIterator<T> sliceIterator(slice);
      Int n1 = sliceIterator.vector().nelements();
      Bool noGoodPoints = True;

      for (pixelIterator.reset(); !pixelIterator.atEnd(); 
           pixelIterator++,sliceIterator.next()) {


// Convert accumulations to mean, sigma, and rms. Make sure we do all calculations
// with double precision values. 
 
         Matrix<Double> matrix(pixelIterator.matrixCursor());

         if (ISTAT == MEAN) {
            for (i=0; i<n1; i++) {
               if (Int(matrix(i,NPTS)+0.1) > 0) {
                  sliceIterator.vector()(i) = matrix(i,SUM) / matrix(i,NPTS);
                  noGoodPoints = False;
               }
            }
         } else if (ISTAT == SIGMA) {
            for (i=0; i<n1; i++) {
               if (Int(matrix(i,NPTS)+0.1) > 0) {
                  Double tmp = (matrix(i,SUMSQ) - (matrix(i,SUM)*matrix(i,SUM)/matrix(i,NPTS))) / 
                               (matrix(i,NPTS)-1);
                  noGoodPoints = False;
                  if (tmp > 0.0) sliceIterator.vector()(i) = sqrt(tmp);
               }
            }
         } else if (ISTAT == RMS) {
            for (i=0; i<n1; i++) {
               if (Int(matrix(i,NPTS)+0.1) > 0) {
                  sliceIterator.vector()(i) = sqrt(matrix(i,SUMSQ)/matrix(i,NPTS));
                  noGoodPoints = False;
               }
            }
         } else {
           os_p << LogIO::SEVERE << "Internal error" << endl << LogIO::POST;
           slice.resize(IPosition(1,0));
         }
      }

// If there were no decent opints return a nothing array

      if (noGoodPoints) slice.resize(IPosition(1,0));

   }
}



template <class T>
void ImageStatistics<T>::copyCursor (Array<T>&slice, const Array<Double>& cursor)
{
// Set up to iterate by vectors 

   ReadOnlyVectorIterator<Double> cursorIt(cursor);
   VectorIterator<T> sliceIt(slice);
   Int n1 = cursorIt.vector().nelements();
   Int i;

// Iterate and copy

   while (!cursorIt.pastEnd()) {
      for (i=0; i<n1; i++) sliceIt.vector()(i) = cursorIt.vector()(i);

      cursorIt.next();
      sliceIt.next();
   }
}




template <class T>
Bool ImageStatistics<T>::findNextDatum (Int& iFound, 
                                        const Int& n,
                                        const float* pn, 
                                        const Int& iStart,
                                        const Bool& findGood)
//
// Find the next good (or bad) point in an array.
// A good point in the array has a non-zero value.
//
// Inputs:
//  n        Number of points in array
//  pn       Pointer to array
//  iStart   The index of the first point to consider
//  findGood If True look for next good point.  
//           If False look for next bad point
// Outputs:
//  iFound   Index of found point
//  Bool     False if didn't find another valid datum
{
   for (Int i=iStart; i<n; i++) {
      if ( (findGood && pn[i]>0.5) ||
           (!findGood && pn[i]<0.5) ) {
        iFound = i;
        return True;
      }
   }
   return False;
}


template <class T>
Bool ImageStatistics<T>::findNextLabel (String& subLabel,
                                        Int& iLab,
                                        String& label)
//
// Find the next comma delimitered sublabel in a string
//
// Inputs:
//  label    The label
//  iLab     The number of the current sublabel (starts at 0)
// Output 
//  subLabel The next sublabel
//  Bool     False if there were no more sublabels
//
{
   static Int iStart=0;
   if (iLab==0) iStart = 0;
   Int iLen = label.length();

   if (iStart >= iLen) {
      subLabel = "";
      return False;
   }

   for (Int i=iStart; i<iLen; i++) {
      String c(label.elem(i));
      if (c == ",") {
         Int n = i - iStart;
         subLabel = String(label(iStart,n));
         iStart = i + 1;        
         return True;

      }
   }


// substring extends to end of string

   Int n = iLen - iStart;
   subLabel = String(label(iStart,n));
   iStart = iLen;
   return True;
}
      


template <class T>
void ImageStatistics<T>::generateStorageImage()
//
// Iterate through the image and generate the accumulation image
{
// Work out dimensions of accumulation image, resize 
// and initialize it

   IPosition storeImageShape;
   ImageUtilities::setStorageImageShape(storeImageShape, True, Int(NACCUM),
                                        displayAxes_p, pInImage_p->shape());

// Create new storage image.  Delete old one first !

   if (pStoreImage_p != 0) delete pStoreImage_p;
   pStoreImage_p = new ArrayLattice<Double>(storeImageShape);
   pStoreImage_p->set(Double(0.0));
   os_p << LogIO::NORMAL << "Created new storage image" << endl << LogIO::POST;
   needStorageImage_p = False;     


// Set up min/max location variables

   minPos_p.resize(pInImage_p->ndim());
   maxPos_p.resize(pInImage_p->ndim());


// Set up pixel iterator and navigator

   RO_LatticeIterator<T> pixelIterator(*pInImage_p, cursorShape_p);


// Iterate through image and accumulate statistical sums

   Int nIter =0;
   for (pixelIterator.reset(); !pixelIterator.atEnd(); pixelIterator++) {
      accumulate (nIter, pixelIterator.position(), 
                  pixelIterator.cursor());
   }  
}



template <class T>
void ImageStatistics<T>::lineSegments (Int& nSeg,
                                       Vector<Int>& start,
                                       Vector<Int>& nPts,
                                       const float* pn,
                                       const Int& n)
//
// Examine an array and determine how many segments
// of good points it consists of.    A good point
// occurs if the array value is greater than zero.
//
// Inputs:
//   pn    The array
//   n     Number of points in array
// Outputs:
//   nSeg  Number of segments
//   start Indices of start of each segment
//   nPts  Number of points in segment
//
{
   Bool none;
   Bool finish = False;
   nSeg = 0;
   Int iGood, iBad;
   start.resize(n);
   nPts.resize(n);

   for (Int i=0; !finish;) {
      Bool ok = findNextDatum (iGood, n, pn, i, True);
      if (!ok) {
         finish = True;
      } else {
         nSeg++;
         start(nSeg-1) = iGood;

         Bool ok2 = findNextDatum (iBad, n, pn, iGood, False);
         if (!ok2) {
            nPts(nSeg-1) = n - start(nSeg-1);
            finish = True;
         } else { 
            nPts(nSeg-1) = iBad - start(nSeg-1);
            i = iBad + 1;
         }
      }
   }
   start.resize(nSeg,True);
   nPts.resize(nSeg,True);
}


template <class T>
void ImageStatistics<T>::listStats (const IPosition& dPos)
//
// List the statistics for this line to the standard output
//
// Inputs:
//   dPos    The location of the start of the cursor in the
//           storage image for this line 
//
{
   Int nDisplayAxes = displayAxes_p.nelements();
   Int nStatsAxes = cursorAxes_p.nelements();
   os_p << LogIO::NORMAL << endl;


// Write the value of the higher order display axes

   if (nDisplayAxes > 1) {
      for (Int j=1; j<nDisplayAxes; j++) {
         os_p << pInImage_p->coordinates().worldAxisNames()(displayAxes_p(j))
              << " = " << dPos(j)+1;
         if (j < nDisplayAxes-1) os_p << ", ";
      }
   }

// Set up the manipulators. We list the number of points as an integer so find
// out how big the field width needs to be.  Min of 6 so label fits.

   Int nMax = 0;
   for (Int j=0; j<n1_p; j++) nMax = max(nMax, Int(pOrd_p[NPTS][j]+0.1));
   Int logNMax = Int(log10(nMax+1.5));

// Have to convert LogIO object to ostream before can apply
// the manipulators

   Int oIWidth = max(6, logNMax);
   Int oDWidth = 15;
   Int oPrec = 6;

   os_p.output().fill(' '); 
   os_p.output().precision(oPrec);
   os_p.output().setf(ios::scientific, ios::floatfield);
   os_p.output().setf(ios::left, ios::adjustfield);


// Write headers

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

   os_p.output() << setw(oDWidth) << pInImage_p->coordinates().
     worldAxisNames()(displayAxes_p(0));
   os_p.output() << setw(oIWidth) << "Npts";
   os_p.output() << setw(oDWidth) << "Sum";
   os_p.output() << setw(oDWidth) << "Mean"; 
   os_p.output() << setw(oDWidth) << "Rms";
   os_p.output() << setw(oDWidth) << "Sigma";
   os_p.output() << setw(oDWidth) << "Minimum";
   os_p.output() << setw(oDWidth) << "Maximum" << endl;


// Write statistics

   for (j=0; j<n1_p; j++) {
      os_p.output() << setw(len0)     << j+1;
      os_p.output() << setw(oDWidth)   << pAbc_p[j];
      os_p.output() << setw(oIWidth)   << Int(pOrd_p[NPTS][j]+0.1);

      if (Int(pOrd_p[NPTS][j]+0.1) > 0) {
         os_p.output() << setw(oDWidth)   << pOrd_p[SUM][j];
         os_p.output() << setw(oDWidth)   << pOrd_p[MEAN][j];
         os_p.output() << setw(oDWidth)   << pOrd_p[RMS][j];
         os_p.output() << setw(oDWidth)   << pOrd_p[SIGMA][j];
         os_p.output() << setw(oDWidth)   << pOrd_p[MIN][j];
         os_p.output() << setw(oDWidth)   << pOrd_p[MAX][j];
      }
      os_p << endl;
   }
   os_p.post();
}




template <class T>
void ImageStatistics<T>::plotStats (const IPosition& dPos)
//
// Plot the desired statistics.  
//
// Inputs:
//   dPos    The location of the start of the cursor in the 
//           storage image for this line 
//
{

// Work out what we are plotting

   Bool doMean  = Bool(ImageUtilities::inVector(Int(MEAN), statsToPlot_p) != -1);
   Bool doSigma = Bool(ImageUtilities::inVector(Int(SIGMA), statsToPlot_p) != -1);
   Bool doRms   = Bool(ImageUtilities::inVector(Int(RMS), statsToPlot_p) != -1);
   Bool doSum   = Bool(ImageUtilities::inVector(Int(SUM), statsToPlot_p) != -1);
   Bool doSumSq = Bool(ImageUtilities::inVector(Int(SUMSQ), statsToPlot_p) != -1);
   Bool doMin   = Bool(ImageUtilities::inVector(Int(MIN), statsToPlot_p) != -1);
   Bool doMax   = Bool(ImageUtilities::inVector(Int(MAX), statsToPlot_p) != -1);
   Bool doNPts  = Bool(ImageUtilities::inVector(Int(NPTS), statsToPlot_p) != -1);


   Bool none;
   Bool first = True;
   Int nL = 0;
   Int nR = 0;

// Find extrema.  Return if there were no valid points to plot

   Float yMin, yMax, xMin, xMax, yLMin, yLMax, yRMin, yRMax;

   minMax(none, xMin, xMax, pAbc_p, pOrd_p[NPTS], n1_p);
   if (none) return;

// Left hand y axis

   if (doMean) {
      minMax(none, yLMin, yLMax, pOrd_p[MEAN], pOrd_p[NPTS], n1_p);
      first = False;
      nL++;
   }
   if (doSum) {
      minMax(none, yMin, yMax, pOrd_p[SUM], pOrd_p[NPTS], n1_p);
      if (first) {
         yLMin = yMin;
         yLMax = yMax;
      } else {
         yLMin = min(yLMin,yMin);
         yLMax = max(yLMax,yMax);
      }
      first = False;
      nL++;
   }
   if (doSumSq) {
      minMax(none, yMin, yMax, pOrd_p[SUMSQ], pOrd_p[NPTS], n1_p);
      if (first) {
         yLMin = yMin;
         yLMax = yMax;
      } else {
         yLMin = min(yLMin,yMin);
         yLMax = max(yLMax,yMax);
      }
      first = False;
      nL++;
   }
   if (doMin) {
      minMax(none, yMin, yMax, pOrd_p[MIN], pOrd_p[NPTS], n1_p);
      if (first) {
         yLMin = yMin;
         yLMax = yMax;
      } else {
         yLMin = min(yLMin,yMin);
         yLMax = max(yLMax,yMax);
      }
      first = False;
      nL++;
   }
   if (doMax) {
      minMax(none, yMin, yMax, pOrd_p[MAX], pOrd_p[NPTS], n1_p);
      if (first) {
         yLMin = yMin;
         yLMax = yMax;
      } else {
         yLMin = min(yLMin,yMin);
         yLMax = max(yLMax,yMax);
      }
      first = False;
      nL++;
   }
   if (doNPts) {
      minMax(none, yMin, yMax, pOrd_p[NPTS], pOrd_p[NPTS], n1_p);
      if (first) {
         yLMin = yMin;
         yLMax = yMax;
      } else {
         yLMin = min(yLMin,yMin);
         yLMax = max(yLMax,yMax);
      }
      first = False;
      nL++;
   }


// Right hand y axis

   first = True;
   if (doSigma) {
      minMax(none, yRMin, yRMax, pOrd_p[SIGMA], pOrd_p[NPTS], n1_p);
      first = False;
      nR++;
   }
   if (doRms) {
      minMax(none, yMin, yMax, pOrd_p[RMS], pOrd_p[NPTS], n1_p);
      if (first) {
         yRMin = yMin;
         yRMax = yMax;
      } else {
         yRMin = min(yRMin,yMin);
         yRMax = max(yRMax,yMax);
      }
      nR++;
   }

   ImageUtilities::stretchMinMax(xMin, xMax); 
   if (nL>0) ImageUtilities::stretchMinMax(yLMin, yLMax);
   if (nR>0) ImageUtilities::stretchMinMax(yRMin, yRMax);


// Set axis labels.

   String xLabel = pInImage_p->coordinates().
     worldAxisNames()(displayAxes_p(0)) + " (pixels)";
   String yLLabel = "";
   String yRLabel = "";

   Int nLLabs = 0;
   if (nL>0) {
      if (doMean) {
         yLLabel += "Mean,";
         nLLabs++;
      }
      if (doSum) {
         yLLabel += "Sum,";
         nLLabs++;
      }
      if (doSumSq) {
         yLLabel += "Sum Squared,";
         nLLabs++;
      }
      if (doMin) {
         yLLabel += "Min,";
         nLLabs++;
      }
      if (doMax) {
         yLLabel += "Max,";
         nLLabs++;
      }
      if (doNPts) {
         yLLabel += "nPts,";
         nLLabs++;
      }
      yLLabel.del(Int(yLLabel.length()-1),1);
   }

   Int nRLabs = 0;
   if (nR>0) {
      if (doSigma) {
         yRLabel += "Sigma,";
         nRLabs++;
      }
      if (doRms) {
         yRLabel += "Rms,";
         nRLabs++;
      }
      yRLabel.del(Int(yRLabel.length()-1),1);
   }
   
// Do plots

   Vector<Int> lCols(nL);
   Vector<Int> rCols(nR);
   int ls = 0;
   int i = -1;
   Bool initColours = True;
   cpgpage();

   if (nL>0) {
      cpgswin(xMin, xMax, yLMin, yLMax);
      if (nR>0) 
         cpgbox("BCNST", 0.0, 0, "BNST", 0.0, 0);
      else
         cpgbox("BCNST", 0.0, 0, "BCNST", 0.0, 0);
      cpglab(xLabel.chars(), "", "");

      if (doMean) {
         if (++ls > 5) ls = 1;
         cpgsls (ls);

         lCols(++i) = niceColour (initColours);
         cpgsci (lCols(i));

         multiPlot ( n1_p, pAbc_p, pOrd_p[MEAN], pOrd_p[NPTS]);
      }
      if (doSum) {
         if (++ls > 5) ls = 1;
         cpgsls (ls);

         lCols(++i) = niceColour (initColours);
         cpgsci (lCols(i));

         multiPlot (n1_p, pAbc_p, pOrd_p[SUM], pOrd_p[NPTS]);
      }
      if (doSumSq) {
         if (++ls > 5) ls = 1;
         cpgsls (ls);

         lCols(++i) = niceColour (initColours);
         cpgsci (lCols(i));

         multiPlot (n1_p, pAbc_p, pOrd_p[SUMSQ], pOrd_p[NPTS]);
      }
      if (doMin) {
         if (++ls > 5) ls = 1;
         cpgsls (ls);

         lCols(++i) = niceColour (initColours);
         cpgsci (lCols(i));

         multiPlot (n1_p, pAbc_p, pOrd_p[MIN], pOrd_p[NPTS]);
      }
      if (doMax) {
         if (++ls > 5) ls = 1;
         cpgsls (ls);

         lCols(++i) = niceColour (initColours);
         cpgsci (lCols(i));

         multiPlot (n1_p, pAbc_p, pOrd_p[MAX], pOrd_p[NPTS]);
      }
      if (doNPts) {
         if (++ls > 5) ls = 1;
         cpgsls (ls);

         lCols(++i) = niceColour (initColours);
         cpgsci (lCols(i));

         multiPlot (n1_p, pAbc_p, pOrd_p[NPTS], pOrd_p[NPTS]);
      }

// Y label

      multiColourYLabel ("L", yLLabel, lCols, nLLabs);
   }
   cpgsls (1);
   cpgsci (1);


   i = -1;
   if (nR>0) {
      cpgswin(xMin, xMax, yRMin, yRMax);
      cpgsci (1); 
      if (nL>0) 
         cpgbox("", 0.0, 0, "CMST", 0.0, 0);
      else {
         cpgbox("BCNST", 0.0, 0, "BCMST", 0.0, 0);
         cpglab(xLabel.chars(), "", "");
      }

      if (doSigma) {
         if (++ls > 5) ls = 1;
         cpgsls(ls);

         rCols(++i) = niceColour (initColours);
         cpgsci (rCols(i));

         multiPlot (n1_p, pAbc_p, pOrd_p[SIGMA], pOrd_p[NPTS]);
      }
      if (doRms) {
         if (++ls > 5) ls = 1;
         cpgsls(ls);

         rCols(++i) = niceColour (initColours);
         cpgsci (rCols(i));

         multiPlot (n1_p, pAbc_p, pOrd_p[RMS], pOrd_p[NPTS]);
      }

// Y label

      multiColourYLabel ("R", yRLabel, rCols, nRLabs);

   }
   cpgsls(1);
   cpgsci (1);


// Write values of other display axes on plot
   
   const int BUFL=64;
   char buf[BUFL];
   ostrstream oss(buf,BUFL,ios::out);

   if (displayAxes_p.nelements() > 1) {
      for (Int j=1; j<displayAxes_p.nelements(); j++) {
         oss << "  " << pInImage_p->coordinates().
	   worldAxisNames()(displayAxes_p(j))
             << "=" << dPos(j)+1;
      }   
      oss << ends;
      char* tLabel = oss.str();


// Write on plot
      
      float xb[4], yb[4];
      cpgqtxt (0.0, 0.0, 0.0, 0.0, "X", xb, yb);
      float dx = xb[3] - xb[0];
      cpgqtxt (0.0, 0.0, 0.0, 0.0, tLabel, xb, yb);
      float dy = yb[1] - yb[0];

      float mx = xMin + dx;
      float my;
      if (nR > 0) 
         my = yRMax + 0.5*dy;
      else
         my = yLMax + 0.5*dy;

      int tbg;
      cpgqtbg(&tbg);
      cpgstbg(0);
      cpgptxt (mx, my, 0.0, 0.0, tLabel);
      cpgstbg(tbg);
   }
}


template <class T>
void ImageStatistics<T>::multiColourYLabel (const String& LRLoc, 
                                            String& label,      
                                            const Vector<Int>& colours,
                                            const Int& nLabs)
//
// Draw each Y-axis sublabel in a string with a different colour
//
{
// Get attributes

   float x1, x2, y1, y2;
   cpgqwin (&x1, &x2, &y1, &y2);
   int sci;
   cpgqci (&sci);


// Find y-location of start of string as fraction of window

   float xb[4], yb[4];
   cpgqtxt (0.0, 0.0, 90.0, 0.0, label.chars(), xb, yb);
   float dy = yb[2]-yb[0];
   float yLoc = abs(0.5*(y2-y1-dy)/(y2-y1));


// Loop over number of sub-labels and write them in colour

   String subLabel;
   float just = 0.0;
   float disp = 2.5;
   if (LRLoc == "R") disp = 3.0;
   for (Int iLab=0; iLab<nLabs; iLab++) {

// Fish out next sub label

      Bool ok = findNextLabel (subLabel, iLab, label);
      if (!ok) {
         cpgsci (sci);
         return;
      } 
      
       
// Write it

      if (iLab < nLabs-1) subLabel = subLabel + ",";
      if (iLab > 0) subLabel.prepend(" ");
      cpgsci (colours(iLab));
      cpgmtxt (LRLoc.chars(), disp, yLoc, just, subLabel.chars());


// Increment y location.  pgqtxt won't count a leading blank so
// replace it with a character for length counting purposes. These
// stupid string classes make this very hard work.

      String s2;
      if (iLab > 0) {
         String s(subLabel(1,subLabel.length()-1));
         s2 = "x" + s;
      } else
         s2 = subLabel;
      cpgqtxt (0.0, 0.0, 90.0, 0.0, s2.chars(), xb, yb);
      dy = abs((yb[2]-yb[0])/(y2-y1));
      yLoc += dy;
   }                       

// Set colour back to what it was

   cpgsci (sci);
   return;
}



template <class T>
void ImageStatistics<T>::multiPlot (const Int& n,
                                    const float* px,
                                    const float* py,
                                    const float* pn)
//
// Plot an array which may have some blanked points.
// Thus we plot it in segments
//
// Inputs:
//  n        Number of points (good and bad)
//  px,py    Pointers to plot arrays
//  pn       Pointer to array indicating whether
//           points in px,py should be plotted or
//           not.  If > 0 plot it.
{

// Find number of segments in this array

   Int nSeg = 0;
   Vector<Int> start;
   Vector<Int> nPts;
   lineSegments (nSeg, start, nPts, pn,  n);

// Loop over segments and plot them

   for (Int i=0; i<nSeg; i++) {
      Int ip = start(i);
      if (nPts(i) == 1) 
         cpgpt (1, &px[ip], &py[ip], 1);
      else
         cpgline (nPts(i), &px[ip], &py[ip]);
   }
}


template <class T>
void ImageStatistics<T>::minMax (Bool& none,
                                 Float& dMin, 
                                 Float& dMax,  
                                 const float* pd,           
                                 const float* pn,
                                 const Int& n)
//
// Inputs:
//   pd   Pointer to array to find min and max of
//   pn   Pointer to array which gives the number of points
//        that were used to compute the value in pt.  If zero,
//        that means there were no valid points and we don't
//        want to consider the corresponding pd[i] value
//   n    number of points in pd and pn arrays
// Outputs:
//   none No valid points in array
//   min,max
//        Min and max of array pd

{
   Bool init = True;
   none = True;

   for (Int i=0; i<n; i++) {
     if (pn[i] > 0.5) {
        if (init) {
           dMin = pd[i];
           dMax = pd[i];
           init = False;
        } else {
           dMin = min(dMin, pd[i]);
           dMax = max(dMax, pd[i]);
        }
        none = False;
     }
   }
}


template <class T>
Int ImageStatistics<T>::niceColour (Bool& initColours)
{
   static colourIndex = 1;
   if (initColours) {
      colourIndex = 1;
      initColours = False;
   }
      
   colourIndex++;
   if (colourIndex == 4 || colourIndex == 14) colourIndex++;
   return colourIndex;
}


template <class T>
void ImageStatistics<T>::retrieveStorageStatistic(Array<T>& slice, const Int& ISTAT)
//
// Retrieve values from accumulation image
//
// Input/output
//   slice        The statistics; should be of zero size on input
//
{
// Generate storage image if required

   if (needStorageImage_p) generateStorageImage();


// Fill output

   Int nDim = pStoreImage_p->ndim() - 1;
   if (nDim == 0) {
      Double tmp = (*pStoreImage_p)(IPosition(1,NPTS));
      Int nPts = Int(tmp + 0.1);
      if (nPts > 0) {
         slice.resize(IPosition(1,1));
         slice(IPosition(1,0)) = (*pStoreImage_p)(IPosition(1,ISTAT));
      }
   } else {

// Define shape of iterator cursor 

      IPosition shape(nDim);
      for (Int i=0; i<nDim; i++) shape(i) = pStoreImage_p->shape()(i);
      RO_LatticeIterator<Double> pixelIterator(*pStoreImage_p, shape);

// Discard storage image until NPTS plane reached

      for (i=0,pixelIterator.reset(); i<NPTS; i++,pixelIterator++) {;};

// Were there some good points ?  If so, continue on

      if (someGoodPoints(pixelIterator.cursor())) {

// Resize output array

         slice.resize(shape);

// Discard unwanted planes of accumulation image

         for (i=0,pixelIterator.reset(); i<ISTAT; i++,pixelIterator++) {;};


// Now copy the next cursor chunk to the output

         copyCursor (slice, pixelIterator.cursor());
      }
   }
}


template <class T>
Bool ImageStatistics<T>::someGoodPoints (const Array<Double>& nPts)
//
// If any of the points in the NPTS array are non-zero return True
{
// Set up to iterate by vectors 

   ReadOnlyVectorIterator<Double> it(nPts);
   Int n1 = it.vector().nelements();
   Int i;

// Iterate and assess.  Bug out as soon as we can

   while (!it.pastEnd()) {
      for (i=0; i<n1; i++) {
         if(Int(it.vector()(i)+0.1) > 0) return True;
      }
      it.next();
   }
   return False;
}



template <class T>
void ImageStatistics<T>::summStats ()
// 
// Print the statistics to the standard output
//
{
   Double tVal = (*pStoreImage_p)(IPosition(1,NPTS));
   Int nPts = Int(tVal + 0.1);
   Double sum = (*pStoreImage_p)(IPosition(1,SUM));
   Double sumSq = (*pStoreImage_p)(IPosition(1,SUMSQ));
   Double mean = sum/nPts;
   Double var = (sumSq - sum*sum/nPts)/(nPts-1);
   Double rms = sqrt(sumSq/nPts);
   Double dMin = (*pStoreImage_p)(IPosition(1,MIN));
   Double dMax = (*pStoreImage_p)(IPosition(1,MAX));

// Have to convert LogIO object to ostream before can apply
// the manipulators

   Int oPrec = 6;
   Int oWidth = 15;
   os_p.output().fill(' '); 
   os_p.output().precision(oPrec);
   os_p.output().setf(ios::scientific, ios::floatfield);
   os_p.output().setf(ios::left, ios::adjustfield);

   os_p << LogIO::NORMAL << endl; 
   if ( Int(nPts+0.1) > 0) {
      os_p << "No pts   = ";
      os_p.output() << setw(oWidth) << Int(nPts+0.1) << endl;
      os_p << "Sum      = ";
      os_p.output() << setw(oWidth) << sum << "       Mean     = ";
      os_p.output() << setw(oWidth) << mean << endl;
      os_p << "Variance = ";
      os_p.output() << setw(oWidth) << var;
      if (var > 0.0) {
         os_p << "       Sigma    = ";
         os_p.output() << setw(oWidth) << sqrt(var) << endl;
      } else {
         os_p << endl;
      }
      os_p << "Rms      = ";
      os_p.output() << setw(oWidth) << rms << endl;
      os_p << endl;
      os_p << "Minimum value at " << minPos_p+1 << " = ";
      os_p.output()  << setw(oWidth) << dMin << endl;
      os_p << "Maximum value at " << maxPos_p+1 << " = ";
      os_p.output()  << setw(oWidth) << dMax << endl;   
   } else
      os_p << "No valid points found " << endl;

   os_p << endl << endl;
   os_p.post();
}




