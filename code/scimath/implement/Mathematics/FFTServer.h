//# FFTServer.h: A class with methods for Fast Fourier Transforms
//# Copyright (C) 1994,1995,1996,1997
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

#if !defined(AIPS_FFTSERVER_H)
#define AIPS_FFTSERVER_H

#if defined(_AIX)
#pragma implementation ("FFTServer.cc")
#endif

#include <aips/aips.h>
#include <aips/Lattices/IPosition.h>
#include <aips/Containers/Block.h>

template <class T> class Array;

// <summary>A class with methods for Fast Fourier Transforms</summary>

// <prerequisite> 
// <li> <linkto module=Arrays>The Arrays module</linkto>
// </prerequisite>

// <etymology> The FFTServer class, can do Fast Fourier Transforms of
// any length and dimensionality.
// </etymology>


// <synopsis>

// The FFTServer class provides methods for performing n-dimensional Fast
// Fourier Transforms with real and complex Array's of arbitrary size and
// dimensionality. It can do either real to complex, complex to real, or
// complex to complex transforms with the "origin" of the transform either at
// the centre of the Array or at the first element.

// Because the output from a real to complex transform is Hermitian only half
// of the complex result is returned. Similarly with a complex to real
// transform only half of the complex plane needs to be supplied.
// <note role=warning> The complex to real transform does not check that the
// imaginary component of the values where u=0 are zero </note>

// This class can be initialised with a shape that indicates the length of the
// transforms that will be performed, and whether they are going to be
// real<->complex transforms or complex<->complex ones. This initialization
// sets up variety of internal buffers and computes factorizations and twiddle
// factors used during the transform. The initialised transform shape is always
// compared with the shape of the supplied arguments when a transform is done
// and the FFTServer class will automatically resize itself if necessary. So
// the default constructor is perfectly safe to use.

// With any transform the output Array must either be the correct shape for the
// desired output or zero length (ie not contain any elements). If it is zero
// length then it will be resized to the correct shape. For a complex->complex
// transform the output Array will be the same shape as the input Array. For a
// real->complex transform the output Array will be the same size as the input
// Array except in the first dimension which will have a length of (nx+2)/2. So
// if nx=7 the output length will be 4 and if nx=8 the output length will be 5,
// on the first axis. nx is the length of the first axis on the input Array.

// For complex to real transforms the output length on the first axis is not
// uniquely defined by the shape of the complex input Array. This class uses the
// following algorithm to work out the length of the first output Array on the
// output Array. 
// <ul> 
// <li> If the size of the output Array is non-zero then it must match the size
// of the input Array except for the first axis. The length of the first axis
// must either be 2*cx-2 or 2*cx-1 and this determines the length of the
// transform on the first axis. cx is the length of the first axis on the input
// Array.
// <li> If the size of the output Array is zero then scan the imaginary
// components of the values at the end of the first axis on the input Array (ie
// at <src>[cx-1,....]</src> If any of these are non-zero the output Array
// will have an odd length.
// <li> Otherwise if all the imaginary components described above are zero then
// look at the current size of the FFTServer object (either defined at
// construction time or with the resize function). If it matches the size of
// the input Array except for the first axis and if the length on this axis is
// either 2*cx-2 or 2*cx-1 then use that to determine the size of the output
// Array.
// <li> Otherwise assume the output Array will an even length of 2*cx-2 on its
// first axis.
// </ul>

// This class does transforms using the the FORTRAN fftpack package. This
// package only does one dimensional transforms and this class decomposes
// multi-dimensional transforms into a series of 1-dimensional ones.

// In this class a forward transform is defined as one that goes from the real
// to the complex (or the time to frequency) domain. In a forward transform the
// exponent is -1 and no scaling is done on the output.  The backward transform
// goes from the complex to the real (or the frequency to the time) domain. The
// exponent is +1 and the result is always scaled by 1/N were N is the total
// number of elements in the Array.

// The origin of the transform is defined as the point were setting only that
// element to one, and then doing a forward transform results in an Array that
// is all one. The <src>fft</src> member functions in this class all assume
// that the origin of the Transform is at the centre of the Array ie. at
// <src>[nx/2,ny/2,...]</src> were the indexing begins at zero. Because the
// fftpack software assumes the origin of the transform is at the first element
// ie.,<src>[0,0,...]</src> this class flips the data in the Array around to
// compensate. This flipping currently takes about one 20% of the total
// transform time and can be avoided by using the <src>fft0</src> member
// functions which do not flip the data.

// Some of the member functions in this class scramble the input Array. This
// can be avoided, at the expense of copying the input Array to temporary
// storage, by either:
// <ul> <li> Ensuring the input Array is a const Array.
//      <li> Setting the constInput Flag to True.
// </ul>

// <note role=warning> This class assumes that a Complex array is stored as
// pairs of floating point numbers, with no intervening gaps, and with the real
// component first ie., <src>[re0,im0,re1,im1, ...]</src>. This means that the
// following type casts work,
// <srcblock>
// S * complexPtr;
// T * realPtr = (T * ) complexPtr;
// </srcblock>
// and allow a Complex number to be accessed as a pair of real numbers. If this
// assumption is bad then real Arrays will have to generated by copying the
// complex ones. Ultimately this assumption about Complex<->Real Array
// conversion should be put somewhere central like Array2Math.cc.

// </synopsis>

// <templating arg=T,S>
// The T argument must be of type Float or Double. <br>
// If T=Float then S must be Complex. <br>
// If T=Double the S must be DComplex. <br>
// These are the only possible instantions of this class.
// </templating>
//
// <example>
// Do a real to complex Transform of a 1-Dimensional Vector.  The following
// example can trivially be extended to any number of dimensions.
// <srcblock>
// FFTServer<Float,Complex> server;
// Vector<Float> input(32), output(17);
// input = 0.0f;
// input(16) = 1.0f;
// cout << "Input:" << input.ac() << endl;
// server.fft(output.ac(), input.ac());
// cout << "Output:" << output.ac() << endl;
// </srcblock>
// </example>
//
// <thrown>
// <li> AipsError: if the input and output Array have bad or incompatible shapes.
// </thrown>
//
// <todo asof="1997/10/22">
//   <li> The time taken to flip the Array can be reduced, if all the Array
//   dimensions are even, by pre-multiplying the every other element on the input
//   Array by -1. Then no flipping needs to be done on the output Array.
// </todo>
template<class T, class S> class FFTServer
{
public:
  // The default constructor. Initializes the server to do transforms of length
  // 1! 
  FFTServer();

  // Initialize the Server to do transforms on Arrays of the specified
  // shape. See the resize function for a description of the
  // complexTransforms flag.
  FFTServer(const IPosition & fftSize, const Bool complexTransforms=False);
  
  // copy constructor. The copied server is initializes to do transforms of the
  // same length as the other server. Uses copy (and not reference) semantics
  // so that changing the transform length of one server does not affect the
  // other server.
  FFTServer(const FFTServer<T,S> & other);

  // destructor
  ~FFTServer();
  
  // The assignment operator which does the same thing as the copy
  // constructor.
  FFTServer<T,S> & operator=(const FFTServer<T,S> & other);

  // Modify the FFTServer object to do transforms of the supplied shape. If
  // complexTransforms is True then it is assumed that complex to complex
  // transforms are going to be done. Otherwise it is assumed that Real to
  // complex transforms (and vice-versa) will be done. The shape is the shape of
  // the real array (or complex one if complex to complex transforms are being
  // done). In general it is not necessary to use this function as all the fft
  // & fft0 functions will automatically resize the server, if necessary, to
  // match their input arguments
  void resize(const IPosition & fftSize,
		 const Bool complexTransforms=False);

  // Real to complex fft. The origin of the transform is in the centre of the
  // Array. Because of the Hermitian property the output Array only contains
  // half of the complex result. Its size must either be zero, ie. have no
  // elements, or a size that is appropriate to the input Array size,
  // ie. <src>shape = [(nx+2)/2, ny, nz,...]</src>.  Otherwise an AipsError is
  // thrown. 
  // If constInput is False then the input Array may be scrambled after calling
  // this function. Setting constInput to True will guarantee this does not
  // happen by making an internal copy of the input Array. Alternatively you
  // can use the version of this function that has a const input Array. This
  // also makes an internal copy.
  // <group>
  void fft(Array<S> & cResult, Array<T> & rData, const Bool constInput=False);
  void fft(Array<S> & cResult, const Array<T> & rData);
  // </group>

  // Complex to real fft. The origin of the transform is in the centre of the
  // Array. Because of the Hermitian property the input Array only contains half
  // of the complex values. The output Array's size must either be zero, ie
  // have no elements, or a size that is appropriate to the input Array size,
  // ie. either <src>shape = [2*cx-2, cy, cz,...]</src> or 
  // <src>shape = [2*cx-1, cy, cz,...]</src>.  Otherwise an AipsError is
  // thrown.

  // To decide on whether the first axis of the output array is odd or even the
  // following algorithm is used.
  // <ul>
  // <li> If the size of the output Array is non-zero:<br>
  //      Then its first dimension must be either either 2*cx-1 or 2*cx-2 (and
  //      the other dimensions must be the same as the input array). Otherwise
  //      an AipsError is thrown.
  // <li> If the number of elements in the output Array is zero:<br>
  //      Then scan the imaginary components of the last points on the first
  //      axis of the input Array. If any are not zero then the first axis of
  //      the output array is 2*cx-1. Otherwise look at the current size of the
  //      FFTServer. If its first dimension is either 2*cx-1 or 2*cx-2 then use
  //      that dimension. Otherwise assume the first dimension is 2*cx-2, ie
  //      an even length.
  // </ul>

  // If constInput is False then the input Array may be scrambled after calling
  // this function. Setting constInput to True will guarantee this does not
  // happen by making an internal copy of the input Array. Alternatively you
  // can use the version of this function that has a const input Array. This
  // also makes an internal copy
  // <group>
  void fft(Array<T> & rResult, Array<S> & cData, const Bool constInput=False);
  void fft(Array<T> & rResult, const Array<S> & cData);
  // </group>

  // Complex to complex in-place fft. The origin of the transform is in the
  // centre of the Array. The direction of the transform is controlled by the
  // toFrequency variable. If True then a forward, or time to frequency,
  // transform is performed. If False a backward or frequency to time transform
  // is done. Scaling by 1/N, where N is the number of elements in the Array,
  // is always done on the backward transform.
  void fft(Array<S> & cValues, const Bool toFrequency=True);

  // Complex to complex fft. The origin of the transform is in the centre of
  // the Array. The direction of the transform is controlled by the toFrequency
  // variable. If True then a forward, or time to frequency, transform is
  // performed. If False a backward or frequency to time transform is
  // done. Scaling by 1/N, where N is the number of elements in the Array, is
  // always done on the backward transform. The size of the output Array must
  // either be zero , ie one dimensional but with no elements, or the same as
  // the input Array, ie. <src>shape = [cx, cy, cz,...]</src>.  Otherwise an
  // AipsError is thrown.
  void fft(Array<S> & cResult, const Array<S> & cData,
	   const Bool toFrequency=True);

  // The following functions are equivalent to the <src>fft</src> functions
  // described above except that the origin of the transform is the first
  // element of the Array, ie. [0,0,0...], rather than the centre element, ie
  // [nx/2, ny/2, nz/2, ...]. As the underlying FORTRAN functions assume that
  // the origin of the transform is the first element these routines are in
  // general faster than the equivalent ones with the origin at the centre of
  // the Array.
  // <group>
  void fft0(Array<S> & cResult, Array<T> & rData, const Bool constInput=False);
  void fft0(Array<S> & cResult, const Array<T> & rData);
  void fft0(Array<T> & rResult, Array<S> & cData, const Bool constInput=False);
  void fft0(Array<T> & rResult, const Array<S> & cData);
  void fft0(Array<S> & cValues, const Bool toFrequency=True);
  void fft0(Array<S> & cResult, const Array<S> & cData,
	    const Bool toFrequency=True);
  // </group>
private:
  // Flips the quadrants in a complex Array so that the point at
  // cData.shape()/2 moves to the origin. This moves, for example, the point 
  // at [8,3] to the origin ([0,0]) in an array of shape [16,7].
  // With an Array with an odd length two flips do NOT restore the data to its
  // original state. So the when toZero=False this routine does an unflip
  // operation that does restore the data to its original state for odd length
  // arrays.
  // When passed a Hermitian Array where half the complex plane is implicit 
  // (eg as produced by a real->complex Transform) it is not necessary to flip
  // the first dimension of the Array. In this case the isHermition flag
  // should be set to True. For complex<->complex transforms this should be
  // False.
  // <group>
  void flip(Array<T> & rData, const Bool toZero, const Bool isHermition);
  void flip(Array<S> & cData, const Bool toZero, const Bool isHermition);
  // </group>
  // finds the shape of the output array when doing complex -> real transforms
  IPosition determineShape(const IPosition & rShape, const Array<S> & cData);
  // The size of the last FFT done by this object
  IPosition theSize;
  // Whether the last FFT was complex <-> complex or not
  Bool theComplexFlag;
  // twiddle factors and factorisations used by fftpack
  PtrBlock<Block<T> *> theWork;
  // buffer for copying non-contigious arrays to contigious ones. This is done
  // so that the FFT's have a better chance of fitting into cache and hence
  // going faster. 
  // This buffer is also used as temporary storage when flipping the data.
  Block<S> theBuffer;
};
#endif
