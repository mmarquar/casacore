//# Convolver.cc:  this defines Convolver a class for doing convolution
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

#include <aips/Mathematics/Convolver.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/ArrayIter.h>

#ifdef __GNUG__
typedef Vector<Int> gnu_vector_int;
#endif

template<class FType> Convolver<FType>::
Convolver(const Array<FType>& psf, Bool cachePsf){
  if (cachePsf) thePsf = psf;
  makeXfr(psf, defaultShape(psf), False, False);
}

template<class FType> Convolver<FType>::
Convolver(const Array<FType>& psf, 
	  const IPosition& imageSize,
	  Bool fullSize,
	  Bool cachePsf){
  if (cachePsf) thePsf = psf;
  makeXfr(psf, imageSize, True, fullSize);
}

template<class FType> Convolver<FType>::
Convolver(const Convolver<FType>& other){
  thePsfSize = other.thePsfSize;
  theFFTSize = other.theFFTSize;
  theXfr = other.theXfr;
  thePsf = other.thePsf;
  theFFT = other.theFFT;
}

template<class FType> Convolver<FType> & 
Convolver<FType>::operator=(const Convolver<FType> & other){
  if (this != &other) {
    thePsfSize.resize(other.thePsfSize.nelements(), False);
    thePsfSize = other.thePsfSize;
    theFFTSize.resize(other.theFFTSize.nelements(), False);
    theFFTSize = other.theFFTSize;
    theXfr.resize(other.theXfr.shape());
    theXfr = other.theXfr;
    thePsf.resize(other.thePsf.shape());
    thePsf = other.thePsf;
    theFFT = other.theFFT;
  }
  return *this;
} 

template<class FType> Convolver<FType>::
~Convolver(){}

template<class FType> IPosition Convolver<FType>::
defaultShape(const Array<FType>& psf){
  // If the user has not specified an image size assume that it is 
  // the same size as the psf. 
  return psf.shape().nonDegenerate();
}

template<class FType> IPosition Convolver<FType>::
extractShape(IPosition& psfSize, const IPosition& imageSize){
  // return an IPosition that has the same number of dimensions as the psf
  // but with the lengths of the image
  return imageSize.getFirst(psfSize.nonDegenerate().nelements());
}

template<class FType> void Convolver<FType>::
makeXfr(const Array<FType>& psf, 
	const IPosition& imageSize,
	Bool linear, Bool fullSize){
  const Array<FType> psfND = psf.nonDegenerate();
  thePsfSize = psfND.shape();
  IPosition imageNDSize = imageSize.nonDegenerate();
  uInt psfDim = thePsfSize.nelements();
  IPosition convImageSize = extractShape(thePsfSize, imageNDSize);
  theFFTSize.resize(psfDim);
  if (linear) 
    if (fullSize)
      theFFTSize = thePsfSize+extractShape(thePsfSize, imageNDSize);
    else
      for (uInt i = 0; i < psfDim; i++)
	theFFTSize(i) = max(thePsfSize(i), 
			    convImageSize(i)+2*Int((thePsfSize(i)+3)/4));
  else 
    for (uInt i = 0; i < psfDim; i++)
      theFFTSize(i) = max(thePsfSize(i), convImageSize(i));
  // set up the FFT server to do transforms of the required length
  theFFT.Set(theFFTSize);
  {
    IPosition tmp = theXfr.shape();
    tmp = 0;
    theXfr.resize(tmp); // I am to lazy to work out the correct size
  }
  // Pad the psf (if necessary) 
  if (theFFTSize != thePsfSize){
    Array<FType> paddedPsf(theFFTSize);
    IPosition blc = theFFTSize/2-thePsfSize/2;
    IPosition trc = blc + thePsfSize - 1;
    paddedPsf = 0.;  
    paddedPsf(blc, trc) = psfND;
    // And do the fft
    theXfr = theFFT.rcnyfft(paddedPsf);
  }
  else
    theXfr = theFFT.rcnyfft(psfND);
}

template<class FType> void Convolver<FType>::
makePsf(Array<FType>& psf){
  if (thePsf.nelements() == 0){
    Array<FType> paddedPsf;
    Int oddFFT = theFFTSize(0)-Int(theFFTSize(0)/2)*2;
    paddedPsf = theFFT.crnyfft(theXfr, -1, oddFFT);
    IPosition trc, blc;
    blc = (theFFTSize-thePsfSize)/2;
    trc = blc + thePsfSize - 1;
    psf = paddedPsf(blc, trc);
  }
  else
    psf.reference(thePsf);
}

template<class FType> void Convolver<FType>::
linearConv(Array<FType>& result,
	   const Array<FType>& model,  
	   Bool fullSize) {
  // Check the dimensions of the model are compatible with the current psf
  IPosition imageSize = extractShape(thePsfSize, model.shape());
  if (fullSize){
    if (imageSize+thePsfSize > theFFTSize){
      resizeXfr(imageSize, True, True);
    }
  }
  else {
    Bool doResize = False;
    for (uInt i = 0; i < thePsfSize.nelements(); i++) {
      if (theFFTSize < max(thePsfSize(i), 
			   imageSize(i)+2*Int((thePsfSize(i)+3)/4)))
	doResize=True;
    }
    if (doResize)
      resizeXfr(imageSize, True, False);
  }
  // Calculate to output array size
  IPosition resultSize = model.shape();
  if (fullSize)
    resultSize.setFirst(imageSize+thePsfSize-1);
  // create space in the output array to hold the data
  result.resize(resultSize);

  ReadOnlyArrayIterator<FType> from(model, thePsfSize.nelements());
  ArrayIterator<FType> to(result, thePsfSize.nelements());

  for (from.origin(), to.origin();
       (from.pastEnd() || to.pastEnd()) == False;
       from.next(), to.next()) {
    doConvolution(to.array(), from.array(), fullSize);
  }
}

template<class FType> void Convolver<FType>::
doConvolution(Array<FType>& result,
	      const Array<FType>& model,
	      Bool fullSize) {
  IPosition modelSize = model.shape();
  Array<NumericTraits<FType>::ConjugateType> fftModel;
  if (theFFTSize != modelSize){
    // Pad the model
    Array<FType> paddedModel(theFFTSize);
    IPosition blc = (theFFTSize-modelSize)/2;
    IPosition trc = blc + modelSize - 1;
    paddedModel = 0.;
    paddedModel(blc, trc) = model;
    // And calculate its transform
    fftModel = theFFT.rcnyfft(paddedModel);
  }
  else
    fftModel = theFFT.rcnyfft(model);
  // Multiply by the transfer function
  fftModel *= theXfr;

  // Do the inverse transform
  Int oddFFT = theFFTSize(0)-Int(theFFTSize(0)/2)*2;
  Array<FType> convolvedData = theFFT.crnyfft(fftModel, -1, oddFFT);
  // Extract the required part of the convolved data
  IPosition trc, blc; 
  if (fullSize) {
    blc = IPosition(thePsfSize.nelements(), 0);
    trc = thePsfSize + modelSize - 2;
  }
  else {
    blc = (theFFTSize-modelSize)/2;
    trc = blc + modelSize - 1;
  }
  result = convolvedData(blc, trc);
}
  
template<class FType> void Convolver<FType>::
setPsf(const Array<FType>& psf, Bool cachePsf){
  if (cachePsf) thePsf = psf;
  makeXfr(psf, defaultShape(psf), False, False);
}
  
template<class FType> void Convolver<FType>::
setPsf(const Array<FType>& psf, 
       IPosition imageSize, 
       Bool fullSize,
       Bool cachePsf){
  if (cachePsf) thePsf = psf;
  makeXfr(psf, imageSize, True, fullSize);
}

template<class FType> void Convolver<FType>::
resizeXfr(const IPosition& imageSize, 
	  Bool linear,
	  Bool fullSize){
  Array<FType> psf;
  makePsf(psf);
  makeXfr(psf, imageSize, linear, fullSize);
}

template<class FType> void Convolver<FType>::
circularConv(Array<FType>& result, 
	     const Array<FType>& model){
  // Check the dimensions of the model are compatible with the current psf
  IPosition imageSize = extractShape(thePsfSize, model.shape());
  if (max(imageSize.asVector().arrayCast(), 
	  thePsfSize.asVector().arrayCast()) 
      != theFFTSize){
    resizeXfr(model.shape(), False, False);
  }
  // create space in the output array to hold the data
  result.resize(model.shape());

  ReadOnlyArrayIterator<FType> from(model, thePsfSize.nelements());
  ArrayIterator<FType> to(result, thePsfSize.nelements());

  for (from.origin(), to.origin();
       (from.pastEnd() || to.pastEnd()) == False;
       from.next(), to.next()) {
    doConvolution(to.array(), from.array(), False);
  }
}

template<class FType> const Array<FType> Convolver<FType>::
getPsf(Bool cachePsf){
  Array<FType> psf;
  makePsf(psf);
  if ((cachePsf == True) && (thePsf.nelements() == 0))
    thePsf.reference(psf);
  return psf;
}
