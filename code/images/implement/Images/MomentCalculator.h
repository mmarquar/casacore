//# MomentCalculator.h: 
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

#if !defined(AIPS_MOMENTCALCULATOR_H)
#define AIPS_MOMENTCALCULATOR_H

//# Includes
#include <aips/aips.h>
#include <trial/Lattices/LineCollapser.h>
#include <aips/Functionals/Gaussian1D.h>
#include <trial/Tasking/PGPlotter.h>

//# Forward Declarations
template <class T> class Vector;
template <class T> class ImageMoments;

// <summary>
// Abstract base class for moment calculator classes
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class="ImageMoments">ImageMoments</linkto>
//   <li> <linkto class="LatticeApply">LatticeApply</linkto>
//   <li> <linkto class="LineCollapser">LineCollapser</linkto>
// </prerequisite>

// <synopsis>
//  This class, its concrete derived classes, and the classes LineCollapser,
//  ImageMoments and LatticeApply are connected as follows.   LatticeApply offers 
//  functions so that the application programmer does not need to worry about how 
//  to optimally iterate through a Lattice; it deals with tiling and to a 
//  lesser extent memory.    LatticeApply functions are used by offering a class 
//  object to them that has a member function with a name and signature 
//  specified by an abstract base class that LatticeApply uses and the 
//  offered class inherits from.   Specifically, in this case, MomentCalcBase
//  inherits from LineCollapser and LatticeApply uses objects and methods of this
//  class (bit does not inherit from it).  This defines the functions
//  <src>collapse</src> and <src>multiCollapse</src> which operate on a vector
//  extracted from a Lattice.  The former returns one number, the latter a vector
//  of numbers from that profile.  MomentCalcBase is a base class for
//  for moment calculation so that the collapse and multiCollapse 
//  functions are used to compute moments  (e.g., mean, sum, sum squared, 
//  intensity weighted velocity etc).
//
//  It is actually the concrete classes derived from MomentCalcBase (call them,
//  as a group, the MomentCalculator classes) that implement the <src>collapse</src> 
//  and <src>multiCollapse</src> functions.  These derived classes allow different 
//  algorithms to be written with which moments of the vector can be computed. 
//
//  Now, so far, we have a LatticeApply function which iterates through Lattices,
//  extracts vectors, and offers them up to functions implemented in the derived 
//  MomentCalculator classes to compute the moments.   As well as that, we need some
//  class to actually construct the MomentCalculator classes and to feed them to 
//  LatticeApply.   This is the role of the ImageMoments class.  It is a high level 
//  class which takes control information from users specifying which moments they 
//  would like to calculate and how.   It also provides the ancilliary masking lattice to 
//  the MomentCalculator constructors. The actual computational work is done by the 
//  MomentCalculator classes. So ImageMoments, MomentCalcBase and its derived 
//  MomentCalculator classes are really one unit; none of them are useful without 
//  the others.  The separation of functionality is caused by having the
//  LatticeApply class that knows all about optimally iterating through Lattices.
//
//  The coupling between these classes is done partly by the "friendship".   ImageMoments
//  grants friendship to MomentCalcBase so that the latter has access to the private data and 
//  private functions of ImageMoments.  MomentCalcBase then operates as an interface between 
//  its derived MomentCalculator classes and ImageMoments. It retrieves private data 
//  from ImageMoments, and also activates private functions in ImageMoments, on behalf 
//  of the MomentCalculator classes. The rest of the coupling is done via the constructors 
//  of the derived MomentCalculator classes.  
//
//  Finally, MomentCalcBase also has a number of protected functions that are common to its
//  derived classes (e.g. plotting, fitting, accumulating sums etc).  It also has protected 
//  data that is common to all the MomentCalculator classes.  This protected data is accessed 
//  directly by name rather than with interface functions as there is too much of it.  Of 
//  course, since MomentCalcBase is an abstract base class, it is up to the MomentCalculator 
//  classes to give the MomentCalcBase protected data objects values.
//
//  For discussion about different moments and algorithms to compute them see the 
//  discussion in <linkto class="ImageMoments">ImageMoments</linkto> and also in
//  the derived classes documentation.
// </synopsis>

// <example>
//  Since MomentCalcBase is an abstract class, we defer code examples to
//  the derived classes.
// </example>

// <motivation>
// We were desirous of writing functions to optimally iterate through Lattices
// so that the application programmer did not have to know anything about tiling
// or memory if possible.   These are the LatticeApply functions. To incorporate 
// ImageMoments into this scheme required some of it to be shifted into 
// MomentCalcBase and its derived classes.
// </motivation>

// <todo asof="yyyy/mm/dd">
//  Derive more classes !
// </todo>


template <class T> class MomentCalcBase : public LineCollapser<T>
{
public:
   virtual ~MomentCalcBase();

protected:


// A number of private data members are kept here in the base class
// as they are common to the derived classes.  Since this class
// is abstract, they have to be filled by the derived classes.

// This vector is a container for all the possible moments that
// can be calculated.  They are in the order given by the ImageMoments
// enum MomentTypes
   Vector<T> calcMoments_p;

// This vector is a container for the moments that the user actually
// asked to see.  They are selected from calcMoments_p
   Vector<T> retMoments_p;

// This vector tells us which elements of the calcMoments_p vector
// we wish to select and put into retMoments_p
   Vector<Int> selectMoments_p;

// Although the general philosophy of these classes is to compute
// all the posisble moments and then select the ones we want,
// some of them are too expensive to calculate unless they are
// really wanted.  These are the median moments and those that
// require a second pass.  These control Bools tell us whether
// we really want to compute the expensive ones.
   Bool doMedianI_p, doMedianV_p, doAbsDev_p;


// These vectors are used to transform coordinates between pixel and world
   Vector<Double> pixelIn_p, worldOut_p;

// All computations involving Coordinate conversions are relatively expensive
// This Bool signifies whether we need coordinate calculations or not for
// any of the moments
   Bool doCoordCalc_p;

// This vector houses the world coordinate values for the profile if it
// was from a separable axis. This means this vector can be pre computed 
// just once, instead of working out the coordinates for each profile 
// (expensive).  It should only be filled if doCoordCalc_p is True
   Vector<Double> sepWorldCoord_p;

// This gives the plotter name.  If no plotting, it won't be attached
   PGPlotter plotter_p;

// This Bool tells us whether we want to see all profiles plotted with the 
// Y range or whether they are to be scaled individually
   Bool fixedYLimits_p;

// When we are plotting, if we have asked to all profiles with the same 
// Y min and max, these are the values to use
   Float yMinAuto_p, yMaxAuto_p;

// This vector is used to hold the abcissa values when plotting profiles
   Vector<T> abcissa_p;

// This string tells us the name of the moment axis (VELO or FREQ etc)
   String momAxisType_p;


// Accumulate statistical sums from a vector
   void accumSums (Double& s0,
                   Double& s0Sq,
                   Double& s1,
                   Double& s2,
                   Int& iMin,
                   Int& iMax,
                   Double& dMin,
                   Double& dMax,
                   const Int index,   
                   const T datum,
                   const Double coord);

// Determine if the spectrum is pure noise 
   Bool allNoise(T& dMean,
                 const Vector<T>& data,
                 const Double peakSNR,
                 const Double stdDeviation);

// Return the blc and trc of the input Lattice from the
// ImageMoments object
// <group>
   IPosition& blc(ImageMoments<T>& iMom);
   IPosition& trc(ImageMoments<T>& iMom);
// </group>


// Check validity of constructor inputs
   void constructorCheck(Vector<T>& retMoments,
                         Vector<T>& calcMoments,
                         const Vector<Int>& selectMoments,
                         const Int nLatticeOut);

// Find out from the selectMoments array whether we want
// to compute the more expensive moments
   void costlyMoments(ImageMoments<T>& iMom,
                      Bool& doMedianI,  
                      Bool& doMedianV,
                      Bool& doAbsDev);

// Return plotting device from ImageMoments object
   PGPlotter& device(ImageMoments<T>& iMom);

// Return automatic/interactive switch from the ImageMoments object
   Bool& doAuto(ImageMoments<T>& iMom);

// Return the Bool saying whether we need to compute coordinates
// or not for the requested moments
   Bool doCoordCalc(ImageMoments<T>& iMom);

// Return the Bool from the ImageMoments object saying whether we 
// are going to fit Gaussians to the profiles or not.
   Bool& doFit(ImageMoments<T>& iMom);

// Draw a horizontal line across the full x range of the plot
   void drawHorizontal(const T& y,
                       PGPlotter& plotter);

// Draw a spectrum on the current panel with the box already drawn on
   void drawLine (const Vector<T>& x,
                  const Vector<T>& y,
                  PGPlotter& plotter);

// Draw and label a spectrum on the current panel
   void drawLine (const Vector<T>& x,
                  const Vector<T>& y,
                  const Bool fixedYLimits,
                  const Float yMinAuto,
                  const Float yMaxAuto,
                  const String xLabel, 
                  const String yLabel, 
                  const String title,
                  PGPlotter& plotter);

// Draw on lines marking the mean and +/- sigma
   void drawMeanSigma  (const T dMean,
                        const T dSigma,
                        PGPlotter& plotter);


// Draw a vertical line of the given length at a given abcissa
   void drawVertical(const T x,
                     const T yMin,
                     const T yMax,
                     PGPlotter& plotter);

// Draw two vertical lines marking a spectral window
   void drawWindow(const Vector<Int>& window,
                   PGPlotter& plotter);

// Fit a Gaussian to x and y arrays given guesses for the gaussian parameters
   Bool fitGaussian (T& peak,
                     T& pos,
                     T& width,
                     T& level,
                     const Vector<T>& x,
                     const Vector<T>& y,
                     const T peakGuess,
                     const T posGuess,
                     const T widthGuess,
                     const T levelGuess);
                        
// Return the fixed Y-plotting limits switch from the
// ImageMomemts object
   Bool& fixedYLimits(ImageMoments<T>& iMom);

// Automatically fit a Gaussian to a spectrum, including finding the
// starting guesses.
   Bool getAutoGaussianFit(Vector<T>& gaussPars,
                           const Vector<T>& x,
                           const Vector<T>& y,
                           const Double peakSNR,
                           const Double stdDeviation,
                           PGPlotter& plotter,
                           const Bool fixedYLimits,
                           const Float yMinAuto,
                           const Float yMaxAuto,
                           const String xLabel,
                           const String yLabel,
                           const String title);

// Automatically work out a guess for the Gaussian parameters
   void getAutoGaussianGuess(T& peakGuess,    
                             T& posGuess,
                             T& widthGuess,
                             const Vector<T>& x,
                             const Vector<T>& y);

// Automatically determine the spectral window
   void getAutoWindow(Vector<Int>& window,
                      const Vector<T>& x,
                      const Vector<T>& y,
                      const Double peakSNR,
                      const Double stdDeviation,
                      const Bool doFit,
                      PGPlotter& plotter,
                      const Bool fixedYLimits,                 
                      const Float yMinAuto,                 
                      const Float yMaxAuto,                 
                      const String xLabel,
                      const String yLabel,
                      const String title);

// Automatically determine the spectral window via Bosma's algorithm
   Bool getBosmaWindow (Vector<Int>& window,
                        const Vector<T>& x,
                        const Vector<T>& y,
                        const Double peakSNR,
                        const Double stdDeviation,
                        PGPlotter& plotter,
                        const Bool fixedYLimits,
                        const Float yMinAuto,
                        const Float yMaxAuto,
                        const String xLabel,
                        const String yLabel,
                        const String title);

// Read the cursor button
   void getButton(Bool& reject,
                  Bool& redo,
                  PGPlotter& plotter);


// Interactively specify the spectral window with the cursor
   void getInterDirectWindow(Bool& allSubsequent,
                             LogIO& os,
                             Vector<Int>& window,
                             const Vector<T>& x,
                             const Vector<T>& y,
                             const Bool fixedYLimits,   
                             const Float yMinAuto,   
                             const Float yMaxAuto,
                             const String xLabel,
                             const String yLabel,
                             const String title,
                             PGPlotter& plotter);

// Interactively define a guess for a Gaussian fit, and then
// do the fit.  Do this repeatedly  until the user is content.
   Bool getInterGaussianFit(Vector<T>& gaussPars,
                            LogIO& os,
                            const Vector<T>& x,
                            const Vector<T>& y,  
                            const Bool fixedYLimits,
                            const Float yMinAuto,
                            const Float yMaxAuto,
                            const String xLabel,
                            const String yLabel,
                            const String title,
                            PGPlotter& plotter);

// Interactively define a guess for the Gaussian parameters
   void getInterGaussianGuess(T& peakGuess,
                              T& posGuess,
                              T& widthGuess,
                              Vector<Int>& window,
                              Bool& reject,
                              LogIO& os,
                              const Int nPts,
                              PGPlotter& plotter);

// Interactively define the spectral window
   void getInterWindow (Bool& allSubsequent,
                        LogIO& os,
                        Vector<Int>& window,
                        const Bool doFit,
                        const Vector<T>& x,
                        const Vector<T>& y,
                        const Bool fixedYLimits,
                        const Float yMinAuto,
                        const Float yMaxAuto,
                        const String xLabel,
                        const String yLabel,
                        const String title,
                        PGPlotter& plotter);

// Read the cursor and return its coordinates if not off the plot.
// Also interpret which button was pressed
   Bool getLoc(T& x,
               Bool& allSubsequent,
               Bool& ditch,
               Bool& redo,
               LogIO& os,
               const Bool final,
               PGPlotter& plotter);
                        
// Compute the world coordinate for the given moment axis pixel   
   Double getMomentCoord(ImageMoments<T>& iMom,
                         Vector<Double>& pixelIn,
                         Vector<Double>& worldOut,
                         const Double index);

// Resize an abcissa vector for plotting
   void makeAbcissa(Vector<T>& x,
                    const Int& n);
                 
// Return the moment axis from the ImageMomemts object
   Int& momentAxis(ImageMoments<T>& iMom);

// Return the name of the moment/profile axis
   String momentAxisName(ImageMoments<T>& iMom);

// Return the number of moments that the ImageMoments class can calculate
   Int nMaxMoments() const;

// Return the peak SNR for determonation of all noise sepctra from
// the ImageMoments object
   Double& peakSNR(ImageMoments<T>& iMom);

// Return the selected pixel intensity range from the ImageMoments 
// object and the Bools describing whether it is inclusion or exclusion
   void range(Vector<Float>& pixelRange,                
              Bool& doInclude,
              Bool& doExlude,
              ImageMoments<T>& iMom);

// The MomentCalculators compute a vector of all possible moments.
// This function returns a vector which selects the desired moments from that
// "all moment" vector.
   Vector<Int> selectMoments(ImageMoments<T>& iMom);

// Fill the ouput moments array
   void setCalcMoments(ImageMoments<T>& iMom,
                       Vector<T>& calcMoments,
                       Vector<Double>& pixelIn,
                       Vector<Double>& worldOut,
                       const Bool doCoordCalc,
                       const T dMedian,
                       const T vMedian,
                       const Int nPts,
                       const Double s0,   
                       const Double s1,
                       const Double s2,
                       const Double s0Sq,
                       const Double sumAbsDev,
                       const Double dMin,
                       const Double dMax,
                       const Int iMin,
                       const Int iMax);

// Take the fitted Gaussian parameters and set an N-sigma window.
// If the window is too small return a Fail condition.
   Bool setNSigmaWindow(Vector<Int>& window,  
                        const T pos,
                        const T width,
                        const Int nPts,
                        const Int N);

// Fill a string with the position of the cursor
   void setPosLabel(String& title,
                    const IPosition& pos);

// Set up separable moment axis coordinate vector and
// conversion vectors if not separable
   void setUpCoords (ImageMoments<T>& iMom,
                     Vector<Double>& pixelIn,
                     Vector<Double>& worldOut,
                     Vector<Double>& sepWorldCoord,
                     LogIO& os);

// Plot the Gaussian fit
   void showGaussFit(const T peak,
                     const T pos,    
                     const T width,
                     const T level,
                     const Vector<T>& x,
                     const Vector<T>& y,
                     PGPlotter& plotter);

// Return standard deviation of image from ImageMoments object
   Double& stdDeviation(ImageMoments<T>& iMom);
 
// Return the auto y min and max from the ImageMoments object
   void yAutoMinMax(Float& yMin, 
                    Float& yMax, 
                    ImageMoments<T>& iMom);

protected:
   // Check if #pixels is indeed 1.
   virtual void init (uInt nOutPixelsPerCollapse);
};


// Inline functions.  These are for operations that occur for each
// element of each profile.

template <class T>
inline void MomentCalcBase<T>::accumSums(Double& s0,
                                         Double& s0Sq,
                                         Double& s1,
                                         Double& s2,
                                         Int& iMin,
                                         Int& iMax,
                                         Double& dMin,
                                         Double& dMax,   
                                         const Int i,  
                                         const T datum,
                                         const Double coord)
//
// Accumulate statistical sums from this datum
//
// Input:
//  i              Index
//  datum          Pixel value
//  coord          Coordinate value on moment axis
// Input/output:  
//  iMin,max       index of dMin and dMax
//  dMin,dMax      minimum and maximum value
// Output:
//  s0             sum (I)
//  s0Sq           sum (I*I)
//  s1             sum (I*v)
//  s2             sum (I*v*v)
{
   const Double dDatum = Double(datum);
   s0 += dDatum;
   s0Sq += dDatum*dDatum;
   s1 += dDatum*coord;
   s2 += dDatum*coord*coord;
   if (dDatum < dMin) {
     iMin = i;
     dMin = dDatum;
   }
   if (dDatum > dMax) {
     iMax = i;
     dMax = dDatum;
   }
}

template <class T>
inline Double MomentCalcBase<T>::getMomentCoord(ImageMoments<T>& iMom,
                                                Vector<Double>& pixelIn,
                                                Vector<Double>& worldOut,
                                                const Double momentPixel)
// 
// Find the value of the world coordinate on the moment axis
// for the given moment axis pixel value. 
//
// Input
//   momentPixel   is the index in the profile extracted from the data
//                 so it is relative to the start of the extracted profile
// Input/output
//   pixelIn       Pixels to convert.  Must all be filled in except for
//                 pixelIn(momentPixel).
//   worldOut      Vector to hold result
//
// Should really return a Fallible as I don't check and see
// if the coordinate transformation fials or not
//
{
   pixelIn(iMom.momentAxis_p) = momentPixel + iMom.blc_p(iMom.momentAxis_p);
   iMom.pInImage_p->coordinates().toWorld(worldOut, pixelIn);
   return worldOut(iMom.momentAxis_p);
}



// <summary> Computes simple clipped, and masked moments</summary>
// <use visibility=export>
// 
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// 
// <prerequisite>
//   <li> <linkto class="ImageMoments">ImageMoments</linkto>
//   <li> <linkto class="LatticeApply">LatticeApply</linkto>
//   <li> <linkto class="MomentCalcBase">MomentCalcBase</linkto>
//   <li> <linkto class="LineCollapser">LineCollapser</linkto>
// </prerequisite>
//
// <synopsis>
//  This concrete class is derived from the abstract base class MomentCalcBase
//  which provides an interface layer to the ImageMoments driver class.
//  ImageMoments creates a MomentClip object and passes it to the LatticeApply
//  function, lineMultiApply. This function iterates through a given lattice, 
//  and invokes the multiCollapse member function of MomentClip on each vector 
//  of pixels that it extracts from the input lattice.  The multiCollapse 
//  function returns a vector of moments which are inserted into the output 
//  lattices also supplied to the LatticeApply function.
//
//  MomentClip computes moments directly from a vector of pixel intensities 
//  extracted from the primary lattice.  An optional pixel intensity inclusion 
//  or exclusion range can be applied.   It can also compute a mask based on the 
//  inclusion or exclusion ranges applied to an ancilliary lattice (the ancilliary 
//  vector corresponding to the primary vector is extracted).  This mask is then 
//  applied to the primary vector for moment computation (ImageMoments offers
//  a smoothed version of the primary lattice as the ancilliary lattice)
//
//  The constructor takes an ImageMoments object; the one that is constructing
//  the MomentClip object of course.   There is much control information embodied  
//  in the state of the ImageMoments object.  This information is extracted by the 
//  MomentCalcBase class and passed on to MomentClip for consumption.
//
//  Note that the ancilliary lattice is only accessed if the ImageMoments 
//  object indicates that a pixel inclusion or exclusion range has been 
//  given as well as the pointer to the lattice having a non-zero value.
//
//  See the <linkto class="ImageMoments">ImageMoments</linkto>
//  for discussion about the moments that are available for computation.
//
// </synopsis>
//
// <example>
// This example comes from ImageMoments.   outPt is a pointer block holding
// pointers to the output lattices.  The ancilliary masking lattice is
// just a smoothed version of the input lattice.
//
// <srcBlock>
// 
//// Construct desired moment calculator object.  Use it polymorphically 
//// via a pointer to the base class.  os_P is a LogIO object.
//
//   MomentCalcBase<T>* pMomentCalculator = 0;
//   if (clipMethod || smoothClipMethod) {
//      pMomentCalculator = new MomentClip<T>(pSmoothedImage, *this, os_p, outPt.nelements());
//   } else if (windowMethod) {
//      pMomentCalculator = new MomentWindow<T>(pSmoothedImage, *this, os_p, outPt.nelements());
//   } else if (fitMethod) {
//      pMomentCalculator = new MomentFit<T>(*this, os_p, outPt.nelements());
//   }
//
//// Iterate optimally through the image, compute the moments, fill the output lattices
//
//   LatticeApply<T>::lineMultiApply(outPt, *pInImage_p, *pMomentCalculator,
//                                   momentAxis_p, blc_p, trc_p, True,
//                                   True, "Compute Moments");
//   delete pMomentCalculator;
//
// </srcBlock>
// </example>
//
//
// <todo asof="yyyy/mm/dd">
// </todo>



template <class T> class MomentClip : public MomentCalcBase<T>
{
public:

// Constructor.  The pointer is to an ancilliary  lattice used as a mask.
// If no masking lattice is desired, the pointer value must be zero.  We also 
// need the ImageMoments object which is calling us, the ImageMoments 
// logger, and the number of output lattices ImageMoments has created.
   MomentClip(Lattice<T>* pMaskLattice,
              ImageMoments<T>& iMom,
              LogIO& os,
              const Int nLatticeOut);

// Destructor (does nothing).
  ~MomentClip();

// This function is not implemented and throws an exception.
   virtual T process(const Vector<T>& vector,
		     const IPosition& pos);

// This function returns a vector of numbers from each input vector.
// the output vector contains the moments known to the ImageMoments
// object passed into the constructor.
   virtual Vector<T>& multiProcess(const Vector<T>& vector,
				   const IPosition& pos);


private:

   ImageMoments<T>& iMom_p;
   LogIO& os_p;
   Lattice<T>* pMaskLattice_p; 
   const Vector<T>* pMaskProfile_p;
   Vector<T> maskSliceRef_p;
   Vector<T> selectedData_p;
   Vector<Int> selectedDataIndex_p;
   Bool doInclude_p, doExclude_p;
   Vector<Float> range_p;
   IPosition blc_p, trc_p, stride_p, sliceShape_p;

};



// <summary> Computes moments from a windowed profile </summary>
// <use visibility=export>
// 
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// 
// <prerequisite>
//   <li> <linkto class="ImageMoments">ImageMoments</linkto>
//   <li> <linkto class="LatticeApply">LatticeApply</linkto>
//   <li> <linkto class="MomentCalcBase">MomentCalcBase</linkto>
//   <li> <linkto class="LineCollapser">LineCollapser</linkto>
// </prerequisite>
//
// <synopsis>
//  This concrete class is derived from the abstract base class MomentCalcBase
//  which provides an interface layer to the ImageMoments driver class.
//  ImageMoments creates a MomentWindow object and passes it to the LatticeApply
//  function lineMultiApply.  This function iterates through a given lattice, 
//  and invokes the multiCollapse member function of MomentWindow on each profile
//  of pixels that it extracts from the input lattice.  The multiCollapse function 
//  returns a vector of moments which are inserted into the output lattices also
//  supplied to the LatticeApply function.
//
//  MomentWindow computes moments from a subset of the pixels selected from  the
//  input profile.  This subset is a simple index range, or window.  The window is
//  selected, for each profile, that is thought to surround the spectral feature 
//  of interest.  This window can be found from the primary lattice, or from an 
//  ancilliary lattice (ImageMoments offers a smoothed version of the primary 
//  lattice as the ancilliary lattice).  The moments are always computed from 
//  primary lattice data.   
//
//  For each profile, the window can be found either interactively or automatically.
//  There are two interactive methods.  Either you just mark the window with the
//  cursor, or you interactively fit a Gaussian to the profile and the +/- 3-sigma
//  window is returned.  There are two automatic methods.  Either Bosma's converging
//  mean algorithm is used, or an automatically  fit Gaussian +/- 3-sigma window
//  is returned.
// 
//  The constructor takes an ImageMoments object; the one that is constructing
//  the MomentWindow object of course.   There is much control information embodied  
//  in the state  of the ImageMomemts object.  This information is extracted by the
//  MomentCalcBase class and passed on to MomentClip for consumption.
//
//  Note that the ancilliary lattice is only accessed if the pointer to it
//  is non zero.
//  
//  See the <linkto class="ImageMoments">ImageMoments</linkto>
//  for discussion about the moments that are available for computation.
//  
// </synopsis>
//
// <example>
// <srcBlock>
// This example comes from ImageMoments.   outPt is a pointer block holding
// pointers to the output lattices.  The ancilliary masking lattice is
// just a smoothed version of the input lattice.  os_P is a LogIO object.
//   
// <srcBlock>
// 
//// Construct desired moment calculator object.  Use it polymorphically via 
//// a pointer to the base class.
//  
//   MomentCalcBase<T>* pMomentCalculator = 0;
//   if (clipMethod || smoothClipMethod) {
//      pMomentCalculator = new MomentClip<T>(pSmoothedImage, *this, os_p, outPt.nelements());
//   } else if (windowMethod) {
//      pMomentCalculator = new MomentWindow<T>(pSmoothedImage, *this, os_p, outPt.nelements());
//   } else if (fitMethod) {
//      pMomentCalculator = new MomentFit<T>(*this, os_p, outPt.nelements());
//   }
//  
//// Iterate optimally through the image, compute the moments, fill the output lattices
//  
//   LatticeApply<T>::lineMultiApply(outPt, *pInImage_p, *pMomentCalculator,  
//                                   momentAxis_p, blc_p, trc_p, True,
//                                   True, "Compute Moments");
//   delete pMomentCalculator;
//  
// </srcBlock>
// </example>
//
// <motivation>
// </motivation>
//
// <todo asof="yyyy/mm/dd">
// </todo>


template <class T> class MomentWindow : public MomentCalcBase<T>
{
public:

// Constructor.  The pointer is to a lattice containing the masking
// lattice (created by ImageMoments). Its shape must be that of the selected 
// region (blc/trc) specified in ImageMoments.   We also need the 
// ImageMoments object which is calling us, the ImageMoments logger,
// and the number of output lattices ImageMoments has created.
   MomentWindow(Lattice<T>* pMaskLattice,
                ImageMoments<T>& iMom,
                LogIO& os,
                const Int nLatticeOut);

// Destructor (does nothing).
  ~MomentWindow();

// This function is not implemented and throws an exception.
   virtual T process(const Vector<T>& vector,
		     const IPosition& pos);

// This function returns a vector of numbers from each input vector.
// the output vector contains the moments known to the ImageMoments
// object passed into the constructor.
   virtual Vector<T>& multiProcess(const Vector<T>& vector,
				   const IPosition& pos);

private:

   ImageMoments<T>& iMom_p;
   LogIO& os_p;
   Lattice<T>* pMaskLattice_p; 
   const Vector<T>* pProfile_p;
   Vector<T> maskSliceRef_p;
   Vector<T> selectedData_p;
   Double stdDeviation_p, peakSNR_p;
   Bool doAuto_p, doFit_p;
   IPosition blc_p, trc_p, stride_p, sliceShape_p;

};



// <summary> Compute moments from a Gaussian fitted to a profile</summary>
// <use visibility=export>
// 
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// 
// <prerequisite>
//   <li> <linkto class="ImageMoments">ImageMoments</linkto>
//   <li> <linkto class="LatticeApply">LatticeApply</linkto>
//   <li> <linkto class="MomentCalcBase">MomentCalcBase</linkto>
//   <li> <linkto class="LineCollapser">LineCollapser</linkto>
// </prerequisite>
//
// <synopsis>
//  This concrete class is derived from the abstract base class MomentCalcBase
//  which provides an interface layer to the ImageMoments driver class.  
//  ImageMoments creates a MomentFit object and passes it to the LatticeApply
//  function, lineMultiApply. This function iterates through a given lattice,
//  and invokes the multiCollapse member function of MomentFit on each vector
//  of pixels that it extracts from the input lattice.  The multiCollapse
//  function returns a vector of moments which are inserted into the output
//  lattices also supplied to the LatticeApply function.
// 
//  MomentFit computes moments by fitting a Gaussian to each profile.  The
//  moments are then computed from that fit.   The fitting can be done either
//  interactively or automatically.
// 
//  The constructor takes an ImageMoments object; the one that is constructing
//  the MomentFit object of course.   There is much control information embodied
//  in the state of the ImageMoments object.  This information is extracted by the
//  MomentCalcBase class and passed on to MomentFit for consumption.
//   
//  See the <linkto class="ImageMoments">ImageMoments</linkto>
//  for discussion about the moments that are available for computation.
//  
// </synopsis>
//
// <example>
// <srcBlock>
// This example comes from ImageMoments.   outPt is a pointer block holding
// pointers to the output lattices.   os_P is a LogIO object.
//                                     
// <srcBlock>
// 
//// Construct desired moment calculator object.  Use it polymorphically via
//// a pointer to the base class.
//
//   MomentCalcBase<T>* pMomentCalculator = 0;
//   if (clipMethod || smoothClipMethod) {
//      pMomentCalculator = new MomentClip<T>(pSmoothedImage, *this, os_p, outPt.nelements());
//   } else if (windowMethod) {
//      pMomentCalculator = new MomentWindow<T>(pSmoothedImage, *this, os_p, outPt.nelements());
//   } else if (fitMethod) {
//      pMomentCalculator = new MomentFit<T>(*this, os_p, outPt.nelements());
//   }
//
//// Iterate optimally through the image, compute the moments, fill the output lattices
//
//   LatticeApply<T>::lineMultiApply(outPt, *pInImage_p, *pMomentCalculator,
//                                   momentAxis_p, blc_p, trc_p, True,
//                                   True, "Compute Moments"); 
//   delete pMomentCalculator;
// </srcBlock>
// </example>
//
// <motivation>
// </motivation>
//
// <todo asof="yyyy/mm/dd">
// </todo>


template <class T> class MomentFit : public MomentCalcBase<T>
{
public:

// Constructor.  We need the ImageMoments object which is calling us, 
// the ImageMoments logger, and the number of output lattices 
// ImageMoments has created.
   MomentFit(ImageMoments<T>& iMom,
             LogIO& os,
             const Int nLatticeOut);

// Destructor (does nothing).
  ~MomentFit();

// This function is not implemented and throws an exception.
   virtual T process(const Vector<T>& vector,
		     const IPosition& pos);

// This function returns a vector of numbers from each input vector.
// the output vector contains the moments known to the ImageMoments
// object passed into the constructor.
   virtual Vector<T>& multiProcess(const Vector<T>& vector,
				   const IPosition& pos);

private:

   ImageMoments<T>& iMom_p;
   LogIO& os_p;
   const Vector<T>* pMaskProfile_p;
   Double stdDeviation_p, peakSNR_p;
   Bool doAuto_p, doFit_p;
   Gaussian1D<T> gauss_p;

};



#endif
