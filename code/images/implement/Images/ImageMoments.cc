//# ImageMoments.cc:  generate moments from an image
//# Copyright (C) 1995,1996,1997,1998
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
//   

#include <aips/aips.h>
#include <aips/Arrays/Array.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Containers/Block.h>
#include <aips/Containers/Record.h>
#include <aips/Containers/RecordFieldId.h>
#include <aips/Exceptions/Error.h>
#include <aips/Functionals/Gaussian1D.h>
#include <aips/Logging/LogIO.h>
#include <aips/Mathematics/Constants.h>
#include <aips/Mathematics/Math.h>
#include <aips/Mathematics/Convolver.h>
#include <aips/OS/Directory.h>
#include <aips/OS/File.h>
#include <aips/OS/Path.h>
#include <aips/Tables/Table.h>
#include <aips/Utilities/DataType.h>
#include <aips/Utilities/String.h>
#include <aips/Arrays/ArrayPosIter.h>

#include <trial/Coordinates.h>
#include <trial/Coordinates/CoordinateUtil.h>
#include <trial/Fitting/NonLinearFitLM.h>
#include <trial/Images/ImageMomentsProgress.h>
#include <trial/Images/ImageStatistics.h>
#include <trial/Images/ImageInterface.h>
#include <trial/Images/PagedImage.h>
#include <trial/Images/ImageUtilities.h>
#include <trial/Lattices/ArrayLattice.h>
#include <trial/Lattices/CopyLattice.h>
#include <trial/Lattices/LatticeApply.h>
#include <trial/Lattices/LatticeIterator.h>
#include <trial/Lattices/LatticeStepper.h>
#include <trial/Lattices/MomentCalculator.h>
#include <trial/Lattices/PixelBox.h>
#include <trial/Lattices/PagedArray.h>
#include <trial/Lattices/SubLattice.h>
#include <trial/Lattices/TiledLineStepper.h>
#include <trial/Tasking/ApplicationEnvironment.h>
#include <trial/Tasking/PGPlotter.h>

#include <trial/Images/ImageMoments.h>

#include <strstream.h>
#include <iomanip.h>




template <class T> 
ImageMoments<T>::ImageMoments (ImageInterface<T>& image, 
                               LogIO &os) : os_p(os)
//
// Constructor. 
//
{
   goodParameterStatus_p = True;

   momentAxisDefault_p = -10;
   momentAxis_p = momentAxisDefault_p;
   moments_p.resize(1);
   moments_p(0) = INTEGRATED;
   kernelTypes_p.resize(0);
   kernelWidths_p.resize(0);
   nxy_p.resize(0);
   range_p.resize(0);
   smoothAxes_p.resize(0);
   blc_p.resize(0);
   trc_p.resize(0);
   inc_p.resize(0);
   peakSNR_p = 3;
   stdDeviation_p = 0.0;
   yMin_p = 0.0;
   yMax_p = 0.0;
   out_p = "";
   psfOut_p = "";
   smoothOut_p = "";
   doWindow_p = False;
   doFit_p = False;
   doAuto_p = True;   
   doSmooth_p = False;
   noInclude_p = True;
   noExclude_p = True;
   fixedYLimits_p = False;

   if (setNewImage(image)) {

// Region defaults to entire image
                                     
      IPosition blc, trc, inc;
      goodParameterStatus_p = setRegion(blc, trc, inc, False);

   } else {
      goodParameterStatus_p = False;
   }
}

template <class T>
ImageMoments<T>::ImageMoments(const ImageMoments<T> &other)
                      : os_p(other.os_p),
                        pInImage_p(other.pInImage_p),
                        momentAxis_p(other.momentAxis_p),
                        momentAxisDefault_p(other.momentAxisDefault_p),
                        kernelTypes_p(other.kernelTypes_p),
                        kernelWidths_p(other.kernelWidths_p),
                        nxy_p(other.nxy_p),
                        moments_p(other.moments_p),
                        range_p(other.range_p),
                        smoothAxes_p(other.smoothAxes_p),
                        blc_p(other.blc_p),
                        trc_p(other.trc_p),
                        inc_p(other.inc_p),
                        peakSNR_p(other.peakSNR_p),
                        stdDeviation_p(other.stdDeviation_p),
                        yMin_p(other.yMin_p),
                        yMax_p(other.yMax_p),
                        plotter_p(other.plotter_p),
                        out_p(other.out_p),
                        psfOut_p(other.psfOut_p),
                        smoothOut_p(other.smoothOut_p),
                        goodParameterStatus_p(other.goodParameterStatus_p),
                        doWindow_p(other.doWindow_p),
                        doFit_p(other.doFit_p),
                        doAuto_p(other.doAuto_p),
                        doSmooth_p(other.doSmooth_p),
                        noInclude_p(other.noInclude_p),
                        noExclude_p(other.noExclude_p),
                        fixedYLimits_p(other.fixedYLimits_p)
//
// Copy constructor
//
{}


template <class T>
ImageMoments<T>::ImageMoments(ImageMoments<T> &other)
                      : os_p(other.os_p),
                        pInImage_p(other.pInImage_p),
                        momentAxis_p(other.momentAxis_p),
                        momentAxisDefault_p(other.momentAxisDefault_p),
                        kernelTypes_p(other.kernelTypes_p),
                        kernelWidths_p(other.kernelWidths_p),
                        nxy_p(other.nxy_p),
                        moments_p(other.moments_p),
                        range_p(other.range_p),
                        smoothAxes_p(other.smoothAxes_p),
                        blc_p(other.blc_p),
                        trc_p(other.trc_p),
                        inc_p(other.inc_p),
                        peakSNR_p(other.peakSNR_p),
                        stdDeviation_p(other.stdDeviation_p),
                        yMin_p(other.yMin_p),
                        yMax_p(other.yMax_p),
                        plotter_p(other.plotter_p),
                        out_p(other.out_p),
                        psfOut_p(other.psfOut_p),
                        smoothOut_p(other.smoothOut_p),
                        goodParameterStatus_p(other.goodParameterStatus_p),
                        doWindow_p(other.doWindow_p),
                        doFit_p(other.doFit_p),
                        doAuto_p(other.doAuto_p),
                        doSmooth_p(other.doSmooth_p),
                        noInclude_p(other.noInclude_p),
                        noExclude_p(other.noExclude_p),
                        fixedYLimits_p(other.fixedYLimits_p)
//
// Copy constructor
//
{}



template <class T> 
ImageMoments<T>::~ImageMoments ()
//
// Destructor does nothing
//
{}


template <class T>
ImageMoments<T> &ImageMoments<T>::operator=(const ImageMoments<T> &other)
//
// Assignment operator
//
{
   if (this != &other) {
      
// Assign to image pointer
      
      pInImage_p = other.pInImage_p;  
      

// Do the rest
      
      os_p = other.os_p;
      momentAxis_p = other.momentAxis_p;
      momentAxisDefault_p = other.momentAxisDefault_p;
      kernelTypes_p = other.kernelTypes_p;
      kernelWidths_p = other.kernelWidths_p;
      nxy_p = other.nxy_p;
      moments_p = other.moments_p;
      range_p = other.range_p;
      smoothAxes_p = other.smoothAxes_p;
      blc_p = other.blc_p;
      trc_p = other.trc_p;  
      inc_p = other.inc_p;
      peakSNR_p = other.peakSNR_p;
      stdDeviation_p = other.stdDeviation_p;
      yMin_p = other.yMin_p;
      yMax_p = other.yMax_p;
      plotter_p = other.plotter_p;
      out_p = other.out_p;
      psfOut_p = other.psfOut_p;
      smoothOut_p = other.smoothOut_p;
      goodParameterStatus_p = other.goodParameterStatus_p;
      doWindow_p = other.doWindow_p;
      doFit_p = other.doFit_p;
      doAuto_p = other.doAuto_p;
      doSmooth_p = other.doSmooth_p;
      noInclude_p = other.noInclude_p;
      noExclude_p = other.noExclude_p;
      fixedYLimits_p = other.fixedYLimits_p;
   }
   return *this;
}


template <class T> 
Bool ImageMoments<T>::setNewImage(ImageInterface<T>& image)
//
// Assign pointer to image
//
{
   if (!goodParameterStatus_p) {
      os_p << LogIO::SEVERE << "Internal class status is bad" << LogIO::POST;
      return False;
   }

   T *dummy = 0;
   DataType imageType = whatType(dummy);

   if (imageType !=TpFloat && imageType != TpDouble) {
       os_p << LogIO::SEVERE << "Moments can only be evaluated from images of type : " <<
         TpFloat << " and " << TpDouble << LogIO::POST;
      goodParameterStatus_p = False;
      pInImage_p = 0;
      return False;
   }

// Assign pointer 

   pInImage_p = &image;

   return True;
}


template <class T>
Bool ImageMoments<T>::setMoments(const Vector<Int>& momentsU)
//
// Assign the desired moments
//
{
   if (!goodParameterStatus_p) {
      os_p << LogIO::SEVERE << "Internal class status is bad" << LogIO::POST;
      return False;
   }

   moments_p.resize(0);
   moments_p = momentsU;


// Check number of moments

   Int nMom = moments_p.nelements();
   if (nMom <= 0) {
      os_p << LogIO::SEVERE << "No moments requested" << LogIO::POST;
      goodParameterStatus_p = False;
      return False;
   } else if (nMom > NMOMENTS) {
      os_p << LogIO::SEVERE << "Too many moments specified" << LogIO::POST;
      goodParameterStatus_p = False;
      return False;
   }

   for (Int i=0; i<nMom; i++) {
      if (moments_p(i) < 0 || moments_p(i) > NMOMENTS-1) {
         os_p << LogIO::SEVERE << "Illegal moment requested" << LogIO::POST;
         goodParameterStatus_p = False;
         return False;
      }
   }
   return True;
}


template <class T>
Bool ImageMoments<T>::setMomentAxis(const Int& momentAxisU)
//
// Assign the desired moment axis.  Zero relative.
//
{
   if (!goodParameterStatus_p) {
      os_p << LogIO::SEVERE << "Internal class status is bad" << LogIO::POST;
      return False;
   }

   momentAxis_p= momentAxisU;
   if (momentAxis_p == momentAxisDefault_p) {
     momentAxis_p = CoordinateUtil::findSpectralAxis(pInImage_p->coordinates());
     if (momentAxis_p == -1) {
       os_p << LogIO::SEVERE << "There is no spectral axis in this image -- specify the axis" << LogIO::POST;
       goodParameterStatus_p = False;
       return False;
     }
   } else {
      if (momentAxis_p < 0 || momentAxis_p > Int(pInImage_p->ndim()-1)) {
         os_p << LogIO::SEVERE << "Illegal moment axis; out of range" << LogIO::POST;
         goodParameterStatus_p = False;
         return False;
      }
      if (pInImage_p->shape()(momentAxis_p) <= 1) {
         os_p << LogIO::SEVERE << "Illegal moment axis; it has only 1 pixel" << LogIO::POST;
         goodParameterStatus_p = False;
         return False;
      }
   }

   return True;
}


template <class T>
Bool ImageMoments<T>::setRegion(const IPosition& blcU,
                                const IPosition& trcU,
                                const IPosition& incU,
                                const Bool& listRegion)
//
// Select the region of interest
//
{   
   if (!goodParameterStatus_p) {
      os_p << LogIO::SEVERE << "Internal class status is bad" << LogIO::POST;
      return False;
   }
 
// Check OK
  
   blc_p.resize(0);
   blc_p = blcU;
   trc_p.resize(0);
   trc_p = trcU;
   inc_p.resize(0);
   inc_p = incU;   
   ImageUtilities::verifyRegion(blc_p, trc_p, inc_p, pInImage_p->shape());
   if (listRegion) {
      os_p << LogIO::NORMAL << "Selected region : " << blc_p+1<< " to "
           << trc_p+1 << LogIO::POST;
   }

   return True;
}
 


template <class T>
Bool ImageMoments<T>::setWinFitMethod(const Vector<Int>& methodU)
//
// Assign the desired windowing and fitting methods
//
{

   if (!goodParameterStatus_p) {
      os_p << LogIO::SEVERE << "Internal class status is bad" << LogIO::POST;
      return False;
   }

// No extra methods set

   if (methodU.nelements() == 0) return True;


// Check legality

   for (Int i = 0; i<Int(methodU.nelements()); i++) {
      if (methodU(i) < 0 || methodU(i) > NMETHODS-1) {
         os_p << LogIO::SEVERE << "Illegal method given" << LogIO::POST;
         goodParameterStatus_p = False;
         return False;
      }
   }


// Assign Boooools

   doWindow_p = ToBool(ImageUtilities::inVector(WINDOW, methodU)!=-1);
   doFit_p    = ToBool(ImageUtilities::inVector(FIT, methodU)!=-1);
   doAuto_p   = ToBool(ImageUtilities::inVector(INTERACTIVE, methodU)==-1);

   return True;
}


template <class T>
Bool ImageMoments<T>::setSmoothMethod(const Vector<Int>& smoothAxesU,
                                      const Vector<Int>& kernelTypesU,
                                      const Vector<Double>& kernelWidthsU)
//
// Assign the desired smoothing parameters. 
//
{
   if (!goodParameterStatus_p) {
      os_p << LogIO::SEVERE << "Internal class status is bad" << LogIO::POST;
      return False;
   }
 

// First check the smoothing axes

   Int i;
   if (smoothAxesU.nelements() > 0) {
      smoothAxes_p = smoothAxesU;
      for (i=0; i<Int(smoothAxes_p.nelements()); i++) {
         if (smoothAxes_p(i) < 0 || smoothAxes_p(i) > Int(pInImage_p->ndim()-1)) {
            os_p << LogIO::SEVERE << "Illegal smoothing axis given" << LogIO::POST;
            goodParameterStatus_p = False;
            return False;
         }
      }
      doSmooth_p = True;
   } else {
      doSmooth_p = False;
      return True;
   }


// Now check the smoothing types

   if (kernelTypesU.nelements() > 0) {
      kernelTypes_p = kernelTypesU;
      for (i=0; i<Int(kernelTypes_p.nelements()); i++) {
         if (kernelTypes_p(i) < 0 || kernelTypes_p(i) > NKERNELS-1) {
            os_p << LogIO::SEVERE << "Illegal smoothing kernel types given" << LogIO::POST;
            goodParameterStatus_p = False;
            return False;
         }
      }
   } else {
      os_p << LogIO::SEVERE << "Smoothing kernel types were not given" << LogIO::POST;
      goodParameterStatus_p = False;
      return False;
   }


// Check user gave us enough smoothing types
 
   if (smoothAxesU.nelements() != kernelTypes_p.nelements()) {
      os_p << LogIO::SEVERE << "Different number of smoothing axes to kernel types" << LogIO::POST;
      goodParameterStatus_p = False;
      return False;
   }


// Now the desired smoothing kernels widths.  Allow for Hanning
// to not be given as it is always 1/4, 1/2, 1/4

   kernelWidths_p.resize(smoothAxes_p.nelements());
   Int nK = kernelWidthsU.nelements();
   for (i=0; i<Int(smoothAxes_p.nelements()); i++) {
      if (kernelTypes_p(i) == HANNING) {

// For Hanning, width is always 3

         kernelWidths_p(i) = 3;
      } else if (kernelTypes_p(i) == BOXCAR) {

// For box must be odd number greater than 1

         if (i > nK-1) {
            os_p << LogIO::SEVERE << "Not enough smoothing widths given" << LogIO::POST;
            goodParameterStatus_p = False;
            return False;
         } else {
            Int intKernelWidth = Int(kernelWidthsU(i)+0.5);
            if (intKernelWidth < 2) {
               os_p << LogIO::SEVERE << "Boxcar kernel width of " << intKernelWidth << 
                       " is too small" << LogIO::POST;
               goodParameterStatus_p = False;
               return False;
            } else {

// Make sure it's an odd integer

               if(makeOdd(intKernelWidth)) {
                  os_p << LogIO::SEVERE << "Increasing boxcar width on axis " << i+1 <<
                          " to " << intKernelWidth << LogIO::POST;
               }
               kernelWidths_p(i) = Double(intKernelWidth);
            }
         }
      } else if (kernelTypes_p(i) == GAUSSIAN) {
         if (i > nK-1) {
            os_p << LogIO::SEVERE << "Not enough smoothing widths given" << LogIO::POST;
            goodParameterStatus_p = False;
            return False;
         } else {
            if (kernelWidthsU(i) < 1.5) {
               os_p << LogIO::SEVERE << "Gaussian kernel width of " << kernelWidthsU(i) << 
                       " is too small" << LogIO::POST;
               goodParameterStatus_p = False;
               return False;
            } else {
               kernelWidths_p(i) = kernelWidthsU(i);
            }
         }
      } else {
         os_p << LogIO::SEVERE << "Internal logic error" << LogIO::POST;
         goodParameterStatus_p = False;
         return False;
      }
   }

   return True;
}


template <class T>  
Bool ImageMoments<T>::setInExCludeRange(const Vector<Double>& includeU,
                                        const Vector<Double>& excludeU)
//
// Assign the desired exclude range           
//
{
   if (!goodParameterStatus_p) {
      os_p << LogIO::SEVERE << "Internal class status is bad" << LogIO::POST;
      return False;
   }

   Vector<Double> include = includeU;
   Vector<Double> exclude = excludeU;

   ostrstream os;
   if (!ImageUtilities::setIncludeExclude(range_p, noInclude_p, noExclude_p,
                                          include, exclude, os)) {
      os_p << LogIO::SEVERE << "Invalid pixel inclusion/exclusion ranges" << LogIO::POST;
      goodParameterStatus_p = False;
      return False;
   }

   return True; 
}


template <class T> 
Bool ImageMoments<T>::setSnr(const Double& peakSNRU,
                             const Double& stdDeviationU)
//
// Assign the desired snr.  The default assigned in
// the constructor is 3,0
//
{
   if (!goodParameterStatus_p) {
      os_p << LogIO::SEVERE << "Internal class status is bad" << LogIO::POST;
      return False;
   }

   if (peakSNRU <= 0.0) {
      peakSNR_p = 3.0;
   } else {
      peakSNR_p = peakSNRU;
   }
   if (stdDeviationU <= 0.0) {
      stdDeviation_p = 0.0;
   } else {
      stdDeviation_p = stdDeviationU;
   }

   return True;
} 


template <class T>
Bool ImageMoments<T>::setOutName(const String& outU)
//
// Assign the desired output file name
//
{
   if (!goodParameterStatus_p) {
      os_p << LogIO::SEVERE << "Internal class status is bad" << LogIO::POST;
      return False;
   }

   out_p = outU;
   return True;
}
 

template <class T>
Bool ImageMoments<T>::setPsfOutName(const String& psfOutU)
//
// Assign the desired output PSF file name
//
{
   if (!goodParameterStatus_p) {
      os_p << LogIO::SEVERE << "Internal class status is bad" << LogIO::POST;
      return False;
   }

   psfOut_p = psfOutU;
   return True;
}



template <class T>
Bool ImageMoments<T>::setSmoothOutName(const String& smoothOutU) 
//
// Assign the desired smoothed image output file name
// 
{ 
   if (!goodParameterStatus_p) {
      os_p << LogIO::SEVERE << "Internal class status is bad" << LogIO::POST;
      return False;
   }

   smoothOut_p = smoothOutU;  
   return True;
}


template <class T>
Bool ImageMoments<T>::setPlotting(const PGPlotter& plotterU,
                                  const Vector<Int>& nxyU,
                                  const Bool yIndU)
//   
// Assign the desired PGPLOT device name and number
// of subplots
//
{ 
   if (!goodParameterStatus_p) {
      os_p << LogIO::SEVERE << "Internal class status is bad" << LogIO::POST;
      return False;
   }

   plotter_p = plotterU;
   fixedYLimits_p = ToBool(!yIndU);
   nxy_p.resize(0);
   nxy_p = nxyU;
   if (!ImageUtilities::setNxy(nxy_p, os_p.output())) {
      goodParameterStatus_p = False;
      return False;
   }
   return True;
}
 


template <class T>
Vector<Int> ImageMoments<T>::toMethodTypes (const String& methods)
// 
// Helper function to convert a string containing a list of desired smoothed kernel types
// to the correct <src>Vector<Int></src> required for the <src>setSmooth</src> function.
// 
// Inputs:
//   methods     SHould contain some of "win", "fit", "inter"
//
{
   Vector<Int> methodTypes(3);
   if (!methods.empty()) {
      String tMethods = methods;
      tMethods.upcase();

      Int i = 0;
      if (tMethods.contains("WIN")) {
         methodTypes(i) = WINDOW;
         i++;
      }
      if (tMethods.contains("FIT")) {
         methodTypes(i) = FIT;
         i++;
      }
      if (tMethods.contains("INTER")) {
         methodTypes(i) = INTERACTIVE;
         i++;
      }
      methodTypes.resize(i, True);
   } else {
      methodTypes.resize(0);
   }
   return methodTypes;
} 


template <class T>
Vector<Int> ImageMoments<T>::toKernelTypes (const String& kernels)
// 
// Helper function to convert a string containing a list of desired smoothed kernel types
// to the correct <src>Vector<Int></src> required for the
// <src>setSmooth</src> function.
// 
// Inputs:
//   kernels   Should contain some of "box", "gauss", "hann"
//
{
// Convert to an array of strings

   const Vector<String> kernelStrings = ImageUtilities::getStrings(kernels);

// Convert strings to appropriate enumerated value

   Vector<Int> kernelTypes(kernelStrings.nelements());

   for (Int i=0; i<Int(kernelStrings.nelements()); i++) {
      String tKernels= kernelStrings(i);
      tKernels.upcase();

      if (tKernels.contains("BOX")) {
         kernelTypes(i) = BOXCAR;
      } else if (tKernels.contains("GAUSS")) {
         kernelTypes(i) = GAUSSIAN;
      } else if (tKernels.contains("HANN")) {
         kernelTypes(i) = HANNING;
      }
   }

// Return result

   return kernelTypes;
} 


template <class T>
Bool ImageMoments<T>::createMoments()
//
// This function does all the work
//
{
   if (!goodParameterStatus_p) {
      os_p << LogIO::SEVERE << LogIO::POST 
           << "Internal status of class is bad.  You have ignored errors" << endl;
      os_p << "in setting the arguments." << LogIO::POST;
      return False;
   }
   

// Find spectral axis and its units

   if (momentAxis_p == momentAxisDefault_p) {
     momentAxis_p = CoordinateUtil::findSpectralAxis(pInImage_p->coordinates());
     if (momentAxis_p == -1) {
       os_p << LogIO::SEVERE << endl << "There is no spectral axis in this image -- specify "
	 "the axis" << LogIO::POST;
       return False;
     }
     if (pInImage_p->shape()(momentAxis_p) <= 1) {
        os_p << LogIO::SEVERE << "Illegal moment axis; it has only 1 pixel" << LogIO::POST;
        goodParameterStatus_p = False;
        return False;
     }
   }
   Int worldMomentAxis = pInImage_p->coordinates().pixelAxisToWorldAxis(momentAxis_p);
   String momentAxisUnits = pInImage_p->coordinates().worldAxisUnits()(worldMomentAxis);
//   cout << "momentAxisUnits = " << momentAxisUnits << endl;
   os_p << LogIO::NORMAL << endl << "Moment axis type is "
        << pInImage_p->coordinates().worldAxisNames()(worldMomentAxis) << LogIO::POST;


// Check the user's requests are allowed

   if (!checkMethod()) return False;


// Check that input and output image names aren't the same.
// if there is only one output image

   if (moments_p.nelements() == 1) {
      if (!out_p.empty() && (out_p == pInImage_p->name())) {
         os_p << LogIO::SEVERE << "Input image and output image have same name" << LogIO::POST;
         return False;
      }
   } 


// Try and set some useful Booools.

   Bool smoothClipMethod = False;
   Bool windowMethod = False;
   Bool fitMethod = False;
   Bool clipMethod = False;
   Bool doPlot = plotter_p.isAttached();

   if (doSmooth_p && !doWindow_p) {
      smoothClipMethod = True;      
   } else if (doWindow_p) {
      windowMethod = True;
   } else if (doFit_p) {
      fitMethod = True;
   } else {
      clipMethod = True;
   }     


// Create table to map input to output axes

         
   Int i, j;
   Int  inDim = pInImage_p->ndim();
   Int outDim = pInImage_p->ndim() - 1;
   IPosition ioMap(outDim);
   for (i=0,j=0; i<inDim; i++) {
      if (i != momentAxis_p) {
         ioMap(j) = i;
         j++;
      }
   }            

// We only smooth the image if we are doing the smooth/clip method
// or possibly the interactive window method.  Note that the convolution
// routines can only handle convolution when the image fits fully in core
// at present.
   
   PagedImage<T>* pSmoothedImage = 0;
   String smoothName;
   if (doSmooth_p) {
      if (!smoothImage(smoothName, pSmoothedImage)) {
         os_p << LogIO::SEVERE << "Error convolving image" << LogIO::POST;
         return False;
      }


// Find the auto Y plot range.   The smooth & clip and the window
// methods only plot the smoothed data.

      if (doPlot && fixedYLimits_p && (smoothClipMethod || windowMethod)) {
         os_p.priority(LogMessage::SEVERE);
         ImageStatistics<T> stats(*pSmoothedImage, os_p);

         Array<T> data;
         stats.getMin(data);
         yMin_p = data(IPosition(data.nelements(),0));
         stats.getMax(data);
         yMax_p = data(IPosition(data.nelements(),0));
         os_p.priority(LogMessage::NORMAL);
      }
   }


// Find the auto Y plot range if not smoothing and stretch
// limits for plotting

   if (fixedYLimits_p && doPlot) {
      if (!doSmooth_p && (clipMethod || windowMethod || fitMethod)) {
         os_p.priority(LogMessage::SEVERE);
         ImageStatistics<T> stats(*pInImage_p, os_p);
         stats.setRegion (blc_p, trc_p, inc_p, False);

         Array<T> data;
         if (!stats.getMin(data)) {
            os_p << LogIO::SEVERE << "Error finding minimum of input image" << LogIO::POST;
            return False;
         }
         yMin_p = data(IPosition(data.nelements(),0));
         stats.getMax(data);
         yMax_p = data(IPosition(data.nelements(),0));
         os_p.priority(LogMessage::NORMAL);
      }
      ImageUtilities::stretchMinMax(yMin_p, yMax_p);
   }


// Set output images shape
   
   IPosition outImageShape(outDim);
   for (j=0; j<outDim; j++) outImageShape(j) = trc_p(ioMap(j)) - blc_p(ioMap(j)) + 1;


// Account for subsectioning and removal of the collapsed moment axis
// in the coordinate system.  


   CoordinateSystem outImageCoord = 
     pInImage_p->coordinates().subImage(blc_p.asVector(), IPosition(inDim,1).asVector());
   outImageCoord.removePixelAxis(momentAxis_p, Double(0.0));
   outImageCoord.removeWorldAxis(worldMomentAxis, Double(0.0));



// Create a vector of pointers for output images 

   PtrBlock<Lattice<T> *> outPt(moments_p.nelements());
   for (i=0; i<Int(outPt.nelements()); i++) outPt[i] = 0;


// Loop over desired output moments

   String suffix;
   Bool goodUnits;
   Bool giveMessage = True;
   Unit imageUnits = pInImage_p->units();
   
   for (i=0; i<Int(moments_p.nelements()); i++) {

// Set moment image units and assign pointer to output moments array
// Value of goodUnits is the same for each output moment image

      Unit momentUnits;
      goodUnits = setOutThings(suffix, momentUnits, imageUnits, momentAxisUnits, moments_p(i));
   
// Create output image(s)

      PagedImage<T>* imgp;
      const String in = pInImage_p->name();   
      if (moments_p.nelements() == 1) {
         if (out_p.empty()) out_p = in+suffix;
         imgp = new PagedImage<T>(outImageShape, outImageCoord, out_p);
         os_p << LogIO::NORMAL << "Created " << out_p << LogIO::POST;
         imgp->setMiscInfo(pInImage_p->miscInfo());
      } else {
         if (out_p.empty()) out_p = in;
         imgp = new PagedImage<T>(outImageShape, outImageCoord,
				  out_p+suffix);
         os_p << LogIO::NORMAL << "Created " << out_p+suffix << LogIO::POST;
         imgp->setMiscInfo(pInImage_p->miscInfo());
      }
      outPt[i] = imgp;

// Set output image units if possible

      if (goodUnits) {
         imgp->setUnits(momentUnits);
      } else {
        if (giveMessage) {
           os_p << LogIO::NORMAL 
                << "Could not determine the units of the moment image(s) so the units " << endl;
           os_p << "will be the same as those of the input image. This may not be very useful." << LogIO::POST;
           giveMessage = False;
        }
      }
   } 



// If the user is using the automatic, non-fitting window method, they need
// a good assement of the noise.  The user can input that value, but if
// they don't, we work it out here.

   Double noise;
   if ( stdDeviation_p <=0 && ( (doWindow_p && doAuto_p) || (doFit_p && !doWindow_p && doAuto_p) ) ) {
      if (pSmoothedImage) {
         os_p << LogIO::NORMAL << "Evaluating noise level from smoothed image" << LogIO::POST;
         if (!whatIsTheNoise (noise, pSmoothedImage)) return False;
      } else {
         os_p << LogIO::NORMAL << "Evaluating noise level from input image" << LogIO::POST;
         if (!whatIsTheNoise (noise, pInImage_p)) return False;
      }
      stdDeviation_p = noise;
   }


// Set up some plotting things
         
   if (doPlot) {
      plotter_p.subp(nxy_p(0), nxy_p(1));
      plotter_p.ask(True);
      plotter_p.sch(1.5);
      plotter_p.vstd();
   }        
   

// Create appropriate MomentCalculator object 

   os_p << LogIO::NORMAL << "Begin computation of moments" << LogIO::POST;

   MomentCalcBase<T>* pMomentCalculator = 0;   
   if (clipMethod || smoothClipMethod) {
      pMomentCalculator = new MomentClip<T>(pSmoothedImage, *this, os_p, outPt.nelements());
   } else if (windowMethod) {
      pMomentCalculator = new MomentWindow<T>(pSmoothedImage, *this, os_p, outPt.nelements());
   } else if (fitMethod) {
      pMomentCalculator = new MomentFit<T>(*this, os_p, outPt.nelements());
   }

// Iterate optimally through the image, compute the moments, fill the output lattices

   const PixelBox region(blc_p, trc_p, pInImage_p->shape());
   ImageMomentsProgress* pProgressMeter = new ImageMomentsProgress();
   LatticeApply<T>::lineMultiApply(outPt, *pInImage_p, region,
                                   *pMomentCalculator, momentAxis_p, 
                                   pProgressMeter);
// Clean up
         
   delete pMomentCalculator;
   delete pProgressMeter;
   for (i=0; i<Int(moments_p.nelements()); i++) delete outPt[i];

   if (pSmoothedImage) {
      delete pSmoothedImage;
       
// Remove the smoothed image file if they don't want to save it
 
      if (smoothOut_p.empty()) {
         Directory dir(smoothName);
         dir.removeRecursive();
      }
   }
   

// Success guarenteed !

   return True;

}





// Private member functions

template <class T> 
Bool ImageMoments<T>::checkMethod ()
// 
// Make sure we can do what the user wants
//  
{


// Make a plotting check. They must give the plotting device for interactive methods.  
// Plotting can be invoked passively for other methods.

   if ( ((doWindow_p && !doAuto_p) ||
         (!doWindow_p && doFit_p && !doAuto_p)) && !plotter_p.isAttached()) {
      os_p << LogIO::SEVERE << "You have not given a plotting device" << LogIO::POST;
      return False;
   } 


// Only can have the median coordinate under certain conditions
   
   if (ImageUtilities::inVector(MEDIAN_COORDINATE, moments_p) != -1) {
      Bool noGood = False;
      if (doWindow_p || doFit_p || doSmooth_p) {
         noGood = True;
      } else {
         if (noInclude_p && noExclude_p) {
            noGood = True;
         } else {
           if (range_p(0)*range_p(1) < 0) noGood = True;
         }
      }
      if (noGood) {
         os_p << LogIO::SEVERE;
         os_p << "You have asked for the median coordinate moment, but it is only" << endl;
         os_p << "available with the basic (no smooth, no window, no fit) method " << endl;
         os_p << "and a pixel range that is either all positive or all negative" << LogIO::POST;
         return False;
      }
   }


// Now check all the silly methods

   const Bool doInter = ToBool(!doAuto_p);

   if (!( (!doSmooth_p && !doWindow_p && !doFit_p && ( noInclude_p &&  noExclude_p) && !doInter) ||
          ( doSmooth_p && !doWindow_p && !doFit_p && (!noInclude_p || !noExclude_p) && !doInter) ||
          (!doSmooth_p && !doWindow_p && !doFit_p && (!noInclude_p || !noExclude_p) && !doInter) ||

          ( doSmooth_p &&  doWindow_p && !doFit_p && ( noInclude_p &&  noExclude_p) &&  doInter) ||
          (!doSmooth_p &&  doWindow_p && !doFit_p && ( noInclude_p &&  noExclude_p) &&  doInter) ||
          ( doSmooth_p &&  doWindow_p && !doFit_p && ( noInclude_p &&  noExclude_p) && !doInter) ||
          (!doSmooth_p &&  doWindow_p && !doFit_p && ( noInclude_p &&  noExclude_p) && !doInter) ||
          (!doSmooth_p &&  doWindow_p &&  doFit_p && ( noInclude_p &&  noExclude_p) &&  doInter) ||
          (!doSmooth_p &&  doWindow_p &&  doFit_p && ( noInclude_p &&  noExclude_p) && !doInter) ||
          ( doSmooth_p &&  doWindow_p &&  doFit_p && ( noInclude_p &&  noExclude_p) &&  doInter) ||
          ( doSmooth_p &&  doWindow_p &&  doFit_p && ( noInclude_p &&  noExclude_p) && !doInter) ||
   
          (!doSmooth_p && !doWindow_p &&  doFit_p && ( noInclude_p &&  noExclude_p) &&  doInter) ||
          (!doSmooth_p && !doWindow_p &&  doFit_p && ( noInclude_p &&  noExclude_p) && !doInter) )) {

      os_p << LogIO::NORMAL << "You have asked for an invalid combination of methods" << LogIO::POST;
      os_p << LogIO::NORMAL << "Valid combinations are: " << LogIO::POST << LogIO::POST;



      os_p <<  "Smooth    Window      Fit   in/exclude   Interactive " << endl;
      os_p <<  "-----------------------------------------------------" << endl;
   
// Basic method. Just use all the data
   
      os_p <<  "  N          N         N        N            N       " << endl;
                       
// Smooth and clip, or just clip
                  
      os_p <<  "  Y/N        N         N        Y            N       " << endl << endl;
                  
// Direct interactive window selection with or without smoothing
                  
      os_p <<  "  Y/N        Y         N        N            Y       " << endl;

// Automatic windowing via Bosma's algorithm with or without smoothing
 
      os_p <<  "  Y/N        Y         N        N            N       " << endl;

// Windowing by fitting Gaussians (selecting +/- 3-sigma) automatically or interactively 
// with or without out smoothing
          
      os_p <<  "  Y/N        Y         Y        N            Y/N     " << endl;
          
// Interactive and automatic Fitting of Gaussians and the moments worked out
// directly from the fits
          
      os_p <<  "  N          N         Y        N            Y/N     " << endl << endl;


      os_p <<  "You have asked for" << endl << endl;
      if (doSmooth_p) 
         os_p <<  "  Y";
      else
         os_p <<  "  N";

      if (doWindow_p) 
         os_p <<  "          Y";
      else
         os_p <<  "          N";
      if (doFit_p) 
         os_p <<  "         Y";
      else
         os_p <<  "         N";
      if (noInclude_p && noExclude_p)
         os_p <<  "        N";
      else
         os_p <<  "        Y";
      if (doAuto_p)
         os_p <<  "            Y";
      else
         os_p <<  "            N";
      os_p <<  endl;
      os_p <<  "-----------------------------------------------------" << endl << LogIO::POST;
      return False;
   }


// Tell them what they are getting
          
   os_p << endl << endl
        << "***********************************************************************" << endl;
   os_p << LogIO::NORMAL << "You have selected the following methods" << endl;
   if (doWindow_p) {
      os_p << "The window method" << endl;
      if (doFit_p) {
         if (doInter)
            os_p << "   with window selection via interactive Gaussian fitting" << endl;
         else
            os_p << "   with window selection via automatic Gaussian fitting" << endl;
      } else {
         if (doInter)
            os_p << "   with interactive direct window selection" << endl;
         else     
            os_p << "   with automatic window selection via the Bosma algorithm" << endl;
      }           
      if (doSmooth_p) {
         os_p << "   operating on the smoothed image.  The moments are still" << endl;
         os_p << "   evaluated from the unsmoothed image" << endl;
      } else
         os_p << "   operating on the unsmoothed image" << endl;
   } else if (doFit_p) {
      if (doInter)
         os_p << "The interactive Gaussian fitting method" << endl;
      else
         os_p << "The automatic Gaussian fitting method" << endl;
          
      os_p << "   operating on the unsmoothed data" << endl;
      os_p << "   The moments are evaluated from the fits" << endl;
   } else if (doSmooth_p) {
      os_p << "The smooth and clip method.  The moments are evaluated from" << endl;
      os_p << "   the masked unsmoothed image" << endl;
   } else {
      if (noInclude_p && noExclude_p)
         os_p << "The basic show it as it is method !" << endl;
      else
         os_p << "The clip method" << endl;
   }
   os_p << endl << endl << LogIO::POST;
   
      
   return True;   
}


template <class T> 
void ImageMoments<T>::drawHistogram (const T& dMin,
                                     const Int& nBins,
                                     const T& binWidth,
                                     const Vector<T>& y,
                                     PGPlotter& plotter)
//
// Draw a histogram on the current window
//
{ 
   plotter.box ("BCNST", 0.0, 0, "BCNST", 0.0, 0);
   plotter.lab ("Intensity", "Number", "");

   const Float width = float(binWidth)/2.0;
   Float centre = Float(dMin) + width;
   Float xx,yy;

   for (Int i=0; i<nBins; i++) {
      xx = centre - width;
      yy = float(y(i));
      plotter.move (xx, 0.0);
      plotter.draw (xx, yy);
      plotter.move (xx, yy);

      xx = centre + width;
      plotter.draw (xx, yy);
      plotter.move (xx, yy);
      plotter.draw (xx, 0.0);
      
      centre += binWidth;
   }                     
}


template <class T> 
void ImageMoments<T>::drawVertical (const T& loc,
                                    const T& yMin,
                                    const T& yMax,
                                    PGPlotter& plotter) 
{
// If the colour index is zero, we are trying to rub something
// out, so don't monkey with the ci then

   Int ci;
   ci = plotter.qci();
   if (ci!=0) plotter.sci (3);

   plotter.move (Float(loc), Float(yMin));
   plotter.draw (Float(loc), Float(yMax));
   plotter.updt();
   plotter.sci (ci);
}


template <class T> 
void ImageMoments<T>::drawLine (const Vector<T>& x,
                                const Vector<T>& y,
                                PGPlotter& plotter)
//
// Draw  a spectrum on the current panel
// with the box already drawn
//
{
// Copy from templated floating type to float

   const int n = x.nelements();
   Vector<Float> xData(n);
   Vector<Float> yData(n);
   for (Int i=0; i<n; i++) {
      xData(i) = x(i);
      yData(i) = y(i);
   }
   plotter.line (xData, yData);
   plotter.updt ();
}



template <class T> 
Bool ImageMoments<T>::getLoc (T& x,
                              T& y,
                              PGPlotter& plotter,
                              LogIO& os)
//
// Read the PGPLOT cursor and return its coordinates if not off the plot
// and any button other than last pushed
//
{
// Fish out window

   Vector<Float> minMax(4);
   minMax = plotter.qwin();

// Position and read cursor

   Float xx = x;
   Float yy = y;
   String str;

   readCursor(plotter, xx, yy, str);
   if (xx >= minMax(0) && xx <= minMax(1) && 
       yy >= minMax(2) && yy <= minMax(3)) {
      x = xx;
      y = yy;
   } else {
      plotter.message("Cursor out of range");
      return False;
   }
   return True;
}



template <class T> 
Bool ImageMoments<T>::makeOdd (Int& i)
{
   const Int j = i / 2;
   if (2*j == i) {
      i++;
      return True;
   }
   return False;
}


template <class T> 
void ImageMoments<T>::makePSF (Array<T>& psf,
                               Matrix<T>& psfSep)
//
// Generate an array containing the convolving function
//
// Output:
//   psf             PSF 
//   psfSep          Separable PSF
//
{

   Int i, j, k;

// Find the largest axis number the user wants to smooth

   const Int psfDim = max(smoothAxes_p.ac()) + 1;


// Work out the shape of the PSF.

   IPosition psfShape(psfDim);
   for (i=0,k=0; i<psfDim; i++) {
      if (ImageUtilities::inVector(i, smoothAxes_p)==-1) {
         psfShape(i) = 1;
      } else {
         if (kernelTypes_p(k) == GAUSSIAN) { 
            const Double sigma = kernelWidths_p(k) / sqrt(Double(8.0) * C::ln2);
            psfShape(i) = (Int(5*sigma + 0.5) + 1) * 2;
         } else if (kernelTypes_p(k) == BOXCAR) {
            const Int intKernelWidth = Int(kernelWidths_p(k)+0.5);
            psfShape(i) = intKernelWidth + 1;
         } else if (kernelTypes_p(k) == HANNING) {
            psfShape(i) = 4;
         }
         k++;
      }
   }


// Resize separable PSF matrix

   Int nAxes = psfDim;
   Int nPts = max(psfShape.asVector().ac());
   psfSep.resize(nPts,nAxes);


// Now fill the separable PSF

   for (i=0,k=0; i<psfDim; i++) {

      if (ImageUtilities::inVector(i, smoothAxes_p)==-1) {

// If this axis is not in the user's list, make the shape
// of the PSF array 1

         psfShape(i) = 1;
         psfSep(0,i) = 1.0;
      } else {
         if (kernelTypes_p(k) == GAUSSIAN) { 

// Gaussian. The volume error is less than 6e-5% for +/- 5 sigma limits

            const Double sigma = kernelWidths_p(k) / sqrt(Double(8.0) * C::ln2);
            const Int refPix = psfShape(i)/2;

            const Double norm = 1.0 / (sigma * sqrt(2.0 * C::pi));
            const Double gWidth = kernelWidths_p(k);
            const Gaussian1D<Double> gauss(norm, Double(refPix), gWidth);
//            os_p << LogIO::NORMAL << "Volume = " << 1/norm << LogIO::POST;

            for (j=0; j<psfShape(i); j++) psfSep(j,i) = gauss(Double(j));
         } else if (kernelTypes_p(k) == BOXCAR) {
            const Int intKernelWidth = Int(kernelWidths_p(k)+0.5);
            const Int refPix = psfShape(i)/2;

            const Int iw = (intKernelWidth-1) / 2;
            for (j=0; j<psfShape(i); j++) {
               if (abs(j-refPix) > iw) {
                  psfSep(j,i) = 0.0;
               } else {
                  psfSep(j,i) = 1.0 / Float(intKernelWidth);
               }
            }
         } else if (kernelTypes_p(k) == HANNING) {
            psfSep(0,i) = 0.25;
            psfSep(1,i) = 0.5;
            psfSep(2,i) = 0.25;
            psfSep(3,i) = 0.0;
         }
         k++;
      }
   }

//   os_p << LogIO::NORMAL << "PSF shape = " << psfShape << LogIO::POST;


// Resize non-separable PSF array

   psf.resize(psfShape);


// Set up position iterator

   ArrayPositionIterator posIterator (psf.shape(), IPosition(psfDim,0), 0);


// Iterate through PSF array and fill it with product of separable PSF

   Float val;
   Float sum = 0.0;
   int index;
   for (posIterator.origin(); !posIterator.pastEnd(); posIterator.next()) {
      for (i=0,val=1.0; i<psfDim; i++) {
         index = posIterator.pos()(i);
         val *= psfSep(index,i);
      }
      psf(posIterator.pos()) = val;
      sum = sum + val;
   } 

//   os_p << LogIO::NORMAL << "Sum of PSF = " << sum << LogIO::POST;

}



template <class T> 
Bool ImageMoments<T>::setOutThings(String& suffix, 
                                   Unit& momentUnits,
                                   const Unit& imageUnits,
                                   const String& momentAxisUnits,
                                   const Int moment)
//
// Set the output image suffixes and units
//
// Input:
//   momentAxisUnits
//                The units of the moment axis
//   moment       The current selected moment
//   imageUnits   The brightness units of the input image.
// Outputs:
//   momentUnits  The brightness units of the moment
//                image. Depends upon moment type
//   suffix       suffix for output file name
//   Bool         True if could set units for moment image, false otherwise
{
   String temp;

   Bool goodUnits = True;
   Bool goodImageUnits = ToBool(!imageUnits.getName().empty());
   Bool goodAxisUnits = ToBool(!momentAxisUnits.empty());

   if (moment == AVERAGE) {
      suffix = "_MAverage";
      temp = imageUnits.getName();
      goodUnits = goodImageUnits;
   } else if (moment == INTEGRATED) {
      suffix = "_MIntegrated";
      temp = imageUnits.getName() + "." + momentAxisUnits;
      goodUnits = ToBool(goodImageUnits && goodAxisUnits);
   } else if (moment == WEIGHTED_MEAN_COORDINATE) {
      suffix = "_MWeighted_Mean_Coord";
      temp = momentAxisUnits;
      goodUnits = goodAxisUnits;
   } else if (moment == WEIGHTED_DISPERSION_COORDINATE) {
      suffix = "_MWeighted_Dispersion_Coord";
      temp = momentAxisUnits + "." + momentAxisUnits;
      goodUnits = goodAxisUnits;
   } else if (moment == MEDIAN) {
      suffix = "_MMedian";
      temp = imageUnits.getName();
      goodUnits = goodImageUnits;
   } else if (moment == STANDARD_DEVIATION) {
      suffix = "_MStandard_Deviation";
      temp = imageUnits.getName();
      goodUnits = goodImageUnits;
   } else if (moment == RMS) {
      suffix = "_MRms";
      temp = imageUnits.getName();
      goodUnits = goodImageUnits;
   } else if (moment == ABS_MEAN_DEVIATION) {
      suffix = "_MAbs_Mean_Dev";
      temp = imageUnits.getName();
      goodUnits = goodImageUnits;
   } else if (moment == MAXIMUM) {
      suffix = "_MMaximum";
      temp = imageUnits.getName();
      goodUnits = goodImageUnits;
   } else if (moment == MAXIMUM_COORDINATE) {
      suffix = "_MMaximum_Coord";
      temp = momentAxisUnits;
      goodUnits = goodAxisUnits;
   } else if (moment == MINIMUM) {
      suffix = "_MMinimum";
      temp = imageUnits.getName();
      goodUnits = goodImageUnits;
   } else if (moment == MINIMUM_COORDINATE) {
      suffix = "_MMinimum_Coord";
      temp = momentAxisUnits;
      goodUnits = goodAxisUnits;
   } else if (moment == MEDIAN_COORDINATE) {
      suffix = "_MMedian_Coord";
      temp = momentAxisUnits;
      goodUnits = goodAxisUnits;
   }
   if (goodUnits) momentUnits.setName(temp);
   return goodUnits;
}




template <class T> 
void ImageMoments<T>::saveLattice (const Lattice<T>* const pLattice,
                                   const CoordinateSystem& cSys,
                                   const IPosition& blc, 
                                   const IPosition& trc,
                                   const String& fileName)
//
// Save a Lattice to disk as a Paged Image
//
// Inputs:
//  fileName    name of disk file
//  coordinate  Coordinate System of an image from which we wish to
//              construct the Coordinates for the saved PagedImage
//  blc, trc    Region of lattice to save.
//  pLattice    Pointer to Lattice to save
//
{
   Int inDim = pLattice->ndim();

   IPosition outShape(inDim,1);
   for (Int i=0; i<inDim; i++) outShape(i) = trc(i) - blc(i) + 1;

   CoordinateSystem outCSys = cSys.subImage(blc.asVector(), IPosition(inDim,1).asVector());
   PagedImage<T> outImage(outShape, outCSys, fileName);

   CopyLattice (outImage.lc(), pLattice->lc(), blc, trc);

//   const PixelBox region(blc, trc, pLattice->shape());
//   SubLattice<T> subLattice(*pLattice, region);
//   outImage.copyData(subLattice);
}




template <class T> 
Bool ImageMoments<T>::smoothImage (String& smoothName, 
                                   PagedImage<T>*& pSmoothedImage)
//
// Smooth image.  We smooth only the sublattice that the user
// has asked for.
//
// Output
//   pSmoothedImage Pointer to smoothed Lattice
//   smoothName     Name of smoothed image file
//   Bool           True for success
{

// Check axes

   Int axMax = max(smoothAxes_p.ac()) + 1;
   if (axMax > Int(pInImage_p->ndim())) {
      os_p << LogIO::SEVERE << "You have specified a smoothing axis larger" << endl;
      os_p <<                  "than the number of axes in the image" << LogIO::POST;
      return False;
   }
      

// Create smoothed image as a PagedImage.  We delete it later
// if the user doesn't want to save it

   if (smoothOut_p.empty()) {
      File inputImageName(pInImage_p->name());
      const String path = inputImageName.path().dirName() + "/";
      Path fileName = File::newUniqueName(path, String("ImageMoments_Smooth_"));
      smoothName = fileName.absoluteName();
   } else {
      smoothName = smoothOut_p;
   }

// Set coordinate system
         
   CoordinateSystem cSys = 
     pInImage_p->coordinates().subImage(blc_p.asVector(), IPosition(pInImage_p->ndim(),1).asVector());


// Create smoothed image

   IPosition shape = trc_p - blc_p + 1;
   pSmoothedImage = new PagedImage<T>(shape, cSys, smoothName);
   pSmoothedImage->setMiscInfo(pInImage_p->miscInfo());
   pSmoothedImage->set(0.0);
   if (!smoothOut_p.empty()) {
      os_p << LogIO::NORMAL << "Created " << smoothName << LogIO::POST;
   }


// Generate convolving function

   Array<T> psf;
   Matrix<T> psfSep;
   makePSF(psf, psfSep);

// Save PSF to disk. It won't be very big generally, so use an ArrayLattice
   
   if (!psfOut_p.empty()) {
      os_p << LogIO::NORMAL << "Saving PSF file" << LogIO::POST;

// Create ArrayLattice

      ArrayLattice<T>* pPSF = new ArrayLattice<T>(psf);

// Fiddle CoordinateSystem
 
      CoordinateSystem psfCSys = pInImage_p->coordinates();
      Int coordinate, axisInCoordinate, worldAxis, pixelAxis;
      Vector<Double> refPix(smoothAxes_p.nelements());
      Int i;
      for (i=0,pixelAxis=0; pixelAxis<Int(psfCSys.nPixelAxes()); pixelAxis++) {
         if (ImageUtilities::inVector(pixelAxis, smoothAxes_p) == -1) {
            psfCSys.findPixelAxis(coordinate, axisInCoordinate, pixelAxis);
            worldAxis = psfCSys.worldAxes(coordinate)(axisInCoordinate);

            psfCSys.removePixelAxis(pixelAxis, 0.0);
            psfCSys.removeWorldAxis(worldAxis, 0.0);
         } else {
            refPix(i) = psf.shape()(i)/2.0;
            i++;
         }
      }
      Vector<Double> refValue(psfCSys.nWorldAxes());
      refValue = 0.0;
      psfCSys.setReferenceValue(refValue);

// Save image to disk
    
      IPosition blc(IPosition(pPSF->ndim(),0));
      IPosition trc(pPSF->shape());
      trc = trc - 1;
      saveLattice (pPSF, psfCSys, blc, trc, psfOut_p);
      delete pPSF;
   }


// Convolve.  PSF is separable so convolve by rows for each axis.  
// First copy input to output then smooth in situ.  

   CopyLattice(pSmoothedImage->lc(), pInImage_p->lc(), blc_p, trc_p);
//   const PixelBox region(blc_p, trc_p, pInImage_p->shape());
//   SubLattice<T> subLattice(*pInImage_p, region);
//   pSmoothedImage->copyData(subLattice);

   for (Int i=0; i<Int(psf.ndim()); i++) {
      if (psf.shape()(i) > 1) {
         os_p << LogIO::NORMAL << "Convolving axis " << i+1 << LogIO::POST;
         Vector<T> psfRow = psfSep.column(i);
         psfRow.resize(psf.shape()(i),True);
         smoothRow (pSmoothedImage, i, psfRow);
      }
   }

   return True;
}



template <class T> 
void ImageMoments<T>::smoothRow (Lattice<T>* pIn,
                                 const Int& row,
                                 const Vector<T>& psf)
{

  TiledLineStepper navIn(pIn->shape(),
			 pIn->niceCursorShape(pIn->maxPixels()),
			 row);
  LatticeIterator<T> inIt(*pIn, navIn);
  Convolver<T> conv(psf, pIn->shape()(row));
  Vector<T> result(pIn->shape()(row));

  while (!inIt.atEnd()) {
    conv.linearConv(result, inIt.vectorCursor());
    inIt.woCursor() = result;
    inIt++;
  }
}



template <class T> 
Bool ImageMoments<T>::whatIsTheNoise (Double& sigma,
                                      const Lattice<T>* pI)
//
// Determine the noise level in the image by first making a histogram of 
// the image, then fitting a Gaussian between the 25% levels to give sigma
// Use the whole image.  If its the input unsmoothed image, then this
// will be all of it.  If its the smoothed image, it will be whatever
// subsection the user has asked for.
//
{

// Set up image iterator to read image optimally fast

   IPosition cursorShape(pI->ndim());
   cursorShape = pI->niceCursorShape(pI->maxPixels());
   LatticeStepper nav(pI->shape(), cursorShape, LatticeStepper::RESIZE);
   RO_LatticeIterator<T> iterator(*pI, nav);

// First pass to get data min and max

   Int i;
   T tMin, tMax;
   iterator++;
   minMax(tMin, tMax, iterator.cursor().ac());
   T dMin = tMin;
   T dMax = tMax;

   while (!iterator.atEnd()) {
      minMax(tMin, tMax, iterator.cursor().ac());
      dMin = min(dMin,tMin);
      dMax = max(dMax,tMax);
      iterator++;
   }  


// Second pass to make the histogram

   const Int nBins = 100;
   Vector<T> y(nBins);
   y = 0;
   T binWidth = (dMax - dMin) / nBins;

   Bool deleteIt;
   Int iBin;
   iterator.reset();
   while (!iterator.atEnd()) {
      const T* pt = iterator.cursor().getStorage(deleteIt);
      Int n = iterator.cursor().nelements();
      for (i=0; i<n; i++) {
         iBin = min(nBins-1, Int((pt[i]-dMin)/binWidth));
         y(iBin) += 1.0;
      }
      iterator.cursor().freeStorage(pt, deleteIt);
      iterator++;
   }  


// Enter into a (plot), select window, fit cycle until content


   T xMin, xMax, yMin, yMax, x1, x2;
   IPosition yMinPos(1), yMaxPos(1);
   minMax (yMin, yMax, yMinPos, yMaxPos, y.ac());
   yMax += yMax/20.0;
   xMin = dMin - (dMax-dMin)/20.0;
   xMax = dMax + (dMax-dMin)/20.0;

   if (plotter_p.isAttached()) {
      plotter_p.subp(1,1);
      plotter_p.swin (Float(xMin), Float(xMax), Float(yMin), Float(yMax));
   }

   Int iMin, iMax;
   Bool first = True;
   Bool more = True;
   while (more) {

// Plot histogram

      if (plotter_p.isAttached()) {
         plotter_p.page();
         drawHistogram (dMin, nBins, binWidth, y, plotter_p);
      }

      if (first) {
         for (i=yMaxPos(0); i<nBins; i++) {
            if (y(i) < yMax/4) {
               iMax = i; 
               break;
             }      
          } 
          for (i=yMaxPos(0); i>0; i--) { 
             if (y(i) < yMax/4) {
                iMin = i; 
                break;
              }      
          }

// Check range is sensible

         if (iMax <= iMin || abs(iMax-iMin) < 3) {
           os_p << LogIO::NORMAL << "The image histogram is strangely shaped, fitting to all bins" << LogIO::POST;
           iMin = 0;
           iMax = nBins-1;
         }

// Draw on plot

         if (plotter_p.isAttached()) {
            x1 = dMin + binWidth/2 + iMin*binWidth;
            x2 = dMin + binWidth/2 + iMax*binWidth;
            drawVertical (x1, yMin, yMax, plotter_p);
            drawVertical (x2, yMin, yMax, plotter_p);
         }
         first = False;

      } else if (plotter_p.isAttached()) {

// We are redoing the fit so let the user mark where they think
// the window fit should be done

         x1 = (xMin+xMax)/2;
         T y1 = (yMin+yMax)/2;
         Int i1, i2;
         i1 = i2 = 0;
  
         plotter_p.message("Mark the locations for the window");
         while (i1==i2) {
            while (!getLoc(x1, y1, plotter_p, os_p)) {};
            i1 = Int((x1 -dMin)/binWidth - 0.5);
            x1 = dMin + binWidth/2 + i1*binWidth;
            drawVertical (x1, yMin, yMax, plotter_p);

            T x2 = x1;
            while (!getLoc(x2, y1, plotter_p, os_p)) {};
            i2 = Int((x2 -dMin)/binWidth - 0.5);
            drawVertical (x2, yMin, yMax, plotter_p);

            if (i1 == i2) {
               plotter_p.message("Degenerate window, try again");
               plotter_p.eras ();
               drawHistogram (dMin, nBins, binWidth, y, plotter_p);
            }
         }


// Set window 

         iMin = min(i1, i2);
         iMax = max(i1, i2);
      }

// Now generate the distribution we want to fit.  Normalize to
// peak 1 to help fitter.  

      Int nPts2 = iMax - iMin + 1;
      Vector<T> xx(nPts2);
      Vector<T> yy(nPts2);
   
      for (i=0; i<nPts2; i++) {
         xx(i) = dMin + binWidth/2 + (i+iMin)*binWidth;
         yy(i) = y(i+iMin)/yMax;
      }


// Create fitter

      NonLinearFitLM<T> fitter;
      Gaussian1D<T> gauss;
      fitter.setFunction(gauss);


// Initial guess

      Vector<T> v(3);
      v(0) = 1.0;
      v(1) = dMin + binWidth/2 + yMaxPos(0)*binWidth;
      v(2) = nPts2*binWidth;


// Fit

      fitter.setFittedFuncParams(v);
      fitter.setMaxIter(50);
      T tol = 0.001;
      fitter.setCriteria(tol);

      Vector<T> resultSigma(nPts2);
      resultSigma = 1;
      Vector<T> solution = fitter.fit(xx, yy, resultSigma);
//      os_p << LogIO::NORMAL << "Solution=" << solution.ac() << LogIO::POST;


// Return values of fit 

      if (fitter.converged()) {
         sigma = abs(solution(2)) / sqrt(2.0);
         os_p << LogIO::NORMAL << "*** The fitted standard deviation of the noise is " << sigma << LogIO::POST << LogIO::POST;

// Now plot the fit 

         if (plotter_p.isAttached()) {
            Int nGPts = 100;
            T dx = (xMax - xMin)/nGPts;

            Gaussian1D<T> gauss(solution(0), solution(1), abs(solution(2)));
            Vector<T> xG(nGPts);
            Vector<T> yG(nGPts);

            T xx;
            for (i=0,xx=xMin; i<nGPts; xx+=dx,i++) {
               xG(i) = xx;
               yG(i) = gauss(xx) * yMax;
            }
            plotter_p.sci (7);
            drawLine (xG, yG, plotter_p);
            plotter_p.sci (1);
         }
      } else {
         os_p << LogIO::NORMAL << "The fit to determine the noise level failed." << endl;
         os_p << "Try inputting it directly" << endl;
         if (plotter_p.isAttached()) os_p << "or try a different window " << LogIO::POST;
      }

// Another go

      if (plotter_p.isAttached()) {
         plotter_p.message("Accept (click left), redo (click middle), give up (click right)");

         Float xx = float(xMin+xMax)/2;
         Float yy = float(yMin+yMax)/2;
         String str;
         readCursor(plotter_p, xx, yy, str);
         str.upcase();
 
         if (str == "D") {
            plotter_p.message("Redoing fit");
         } else if (str == "X")
            return False;
         else
            more = False;
      } else
         more = False;
   }
     
   return True;
}


template <class T> 
Bool ImageMoments<T>::readCursor (PGPlotter& plotter, Float& x,
                                  Float& y, String& ch)
{
   Record r;
   r = plotter.curs(x, y);
   Bool gotCursor;
   r.get(RecordFieldId(0), gotCursor);
   r.get(RecordFieldId(1), x);
   r.get(RecordFieldId(2), y);
   r.get(RecordFieldId(3), ch);
   return gotCursor;
}
 
