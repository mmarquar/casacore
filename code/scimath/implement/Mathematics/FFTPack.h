//# extern_fft.h: C++ wrapper functions for FORTRAN FFT code
//# Copyright (C) 1993,1994,1995,1997
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

#if !defined(AIPS_EXTERN_FFT_H)
#define AIPS_EXTERN_FFT_H

#include <aips/aips.h>

// <summary>Complex to complex transforms</summary>
// <synopsis>
// These are C++ wrapper functions for the FORTRAN complex to complex transform
// routines in the files fftpak.f and dfftpack.f. The purpose of these
// definitions is to overload the functions so that C++ users can access the
// functions in either fftpak or dfftpack with an identical function names.

// These routines only do one-dimensional transforms with the first element of
// the array being the "origin" of the transform. The <linkto
// class="FFTServer">FFTServer</linkto> class uses these functions to implement
// multi-dimensional complex to complex transforms with the origin of the
// transform either at the centre or the first element of the Array.

// Before using the forward transform <src>cfftf</src> or the backward
// transform src>cfftb</src> the work array <src>wsave</src> must be
// initialised using the <src>cffti</src> function.

// <note role=warning> 
// These functions assume that it is possble to convert between AIPS++ numeric
// types and those used by Fortran. That it is possible to convert between
// Float & float, Double & double and Int & int.
// </note>
// <note role=warning> 
// These function also assume that a Complex array is stored as pairs of
// floating point numbers, with no intervening gaps, and with the real
// component first ie., <src>[re0,im0,re1,im1, ...]</src> so that the following
// type casts work,
// <srcblock>
// Complex * complexPtr;
// Float * floatPtr = (Float * ) complexPtr;
// </srcblock>
// and allow a Complex number to be accessed as a pair of real numbers. If this
// assumption is bad then float Arrays will have to generated by copying the
// complex ones. Ultimately this assumption about Complex<->Float Array
// conversion should be put somewhere central like Array2Math.cc.</note>
// </synopsis>

// <group name="complex-to-complex-fft">

// cffti initializes the array wsave which is used in both cfftf and cfftb. The
// prime factorization of n together with a tabulation of the trigonometric
// functions are computed and stored in wsave.
// 
// Input parameter:
// <dl compact>
// <dt><b>n</b>
// <dd>       the length of the sequence to be transformed
// </dl>
// Output parameter:
// <dl compact>
// <dt><b>wsave</b>
// <dd>    a work array which must be dimensioned at least 4*n+15
//         the same work array can be used for both cfftf and cfftb
//         as long as n remains unchanged. different wsave arrays
//         are required for different values of n. the contents of
//         wsave must not be changed between calls of cfftf or cfftb.
// </dl>
// <group>
void cffti(Int n, Float * wsave);
void cffti(Int n, Double * wsave);
// </group>

// cfftf computes the forward complex discrete Fourier
// transform (the Fourier analysis). Equivalently, cfftf computes
// the Fourier coefficients of a complex periodic sequence.
// the transform is defined below at output parameter c.
// 
// The transform is not normalized. To obtain a normalized transform
// the output must be divided by n. Otherwise a call of cfftf
// followed by a call of cfftb will multiply the sequence by n.
// 
// The array wsave which is used by subroutine cfftf must be
// initialized by calling subroutine <src>cffti(n,wsave)</src>.
// 
// Input parameters:
// <dl compact>
// <dt><b>n</b>
// <dd>   The length of the complex sequence c. The method is
//        more efficient when n is the product of small primes.
// <dt><b>c</b>
// <dd>    A complex array of length n which contains the sequence to be
//         transformed.
// <dt><b>wsave</b>
// <dd>    A real work array which must be dimensioned at least 4n+15
//         by the program that calls cfftf. The wsave array must be
//         initialized by calling subroutine <src>cffti(n,wsave)</src> and a
//         different wsave array must be used for each different
//         value of n. This initialization does not have to be
//         repeated so long as n remains unchanged thus subsequent
//         transforms can be obtained faster than the first.
//         The same wsave array can be used by cfftf and cfftb.
// </dl>
// Output parameters:
// <dl compact>
// <dt><b>c</b>
// <dd>   for j=1,...,n<br>
//            c(j)=the sum from k=1,...,n of<br>
//                  c(k)*exp(-i*(j-1)*(k-1)*2*pi/n)<br>
//                        where i=sqrt(-1)<br>
// <dt><b>wsave</b>
// <dd>    contains initialization calculations which must not be
//         destroyed between calls of subroutine cfftf or cfftb
// </dl>
// <group>
void cfftf(Int n, Complex * c, Float * wsave);
void cfftf(Int n, DComplex * c, Double * wsave);
// </group>

// cfftb computes the backward complex discrete Fourier
// transform (the Fourier synthesis). Equivalently, cfftb computes
// a complex periodic sequence from its Fourier coefficients.
// The transform is defined below with output parameter c.
// 
// A call of cfftf followed by a call of cfftb will multiply the
// sequence by n.
// 
// The array wsave which is used by subroutine cfftb must be
// initialized by calling <src>cffti(n,wsave)</src>.
// 
// Input parameters:
// <dl compact>
// <dt><b>n</b>
// <dd>          The length of the complex sequence c. The method is
//               more efficient when n is the product of small primes.
// <dt><b>c</b>
// <dd>          A complex array of length n which contains the sequence to be
//               transformed.
// <dt><b>wsave</b> 
// <dd>          A real work array which must be dimensioned at least 4n+15
//               in the program that calls cfftb. The wsave array must be
//               initialized by calling subroutine <src>cffti(n,wsave)</src>
//               and a different wsave array must be used for each different
//               value of n. This initialization does not have to be
//               repeated so long as n remains unchanged thus subsequent
//               transforms can be obtained faster than the first.
//               The same wsave array can be used by cfftf and cfftb.
// </dl>
// Output parameters:
// <dl compact>
// <dt><b>c</b>     
// <dd>          for j=1,...,n<br>
//                 c(j)=the sum from k=1,...,n of<br>
//                     c(k)*exp(i*(j-1)*(k-1)*2*pi/n)<br>

// <dt><b>wsave</b>
// <dd>          contains initialization calculations which must not be
//               destroyed between calls of subroutine cfftf or cfftb
// </dl>
// <group>
void cfftb(Int n, Complex * c, Float * wsave);
void cfftb(Int n, DComplex * c, Double * wsave);
// </group>
// </group>

// <summary>Real to complex & complex to real transforms</summary>
// <synopsis>
// These are C++ wrapper functions for the FORTRAN real to complex & complex to
// real transform routines in the files fftpak.f and dfftpack.f. The purpose of
// these definitions is to overload the functions so that C++ users can access
// the functions in either fftpak or dfftpack with an identical function names.

// These routines only do one-dimensional transforms with the first element of
// the array being the "origin" of the transform. The <linkto
// class="FFTServer">FFTServer</linkto> class uses these functions to implement
// real to complex and complex to real multi-dimensional transforms with the
// origin of the transform either at the centre or the first element of the
// Array.

// Before using the forward transform <src>rfftf</src> or the backward
// transform src>rfftb</src> the work array <src>wsave</src> must be
// initialised using the <src>rffti</src> function.

// The "ez" routines provide a simpler interface to the real to complex
// transforms. They do not destroy the input arrays and are available in single
// precision only. Internally they use use the "rfft" functions.

// <note role=warning> 
// These functions assume that it is possble to convert between AIPS++ numeric
// types and those used by Fortran. That it is possible to convert between
// Float & float, Double & double and Int & int.
// </note>
// </synopsis>

// <group name="real-to-complex-fft">

// rffti initializes the array wsave which is used in both <src>rfftf</src> and
// <src>rfftb</src>. The prime factorization of n together with a tabulation of
// the trigonometric functions are computed and stored in wsave.
//
// Input parameter:
// <dl compact>
// <dt><b>n</b>
// <dd>       the length of the sequence to be transformed.
// </dl>
// Output parameter:
// <dl compact>
// <dt><b>wsave</b>
// <dd>    a work array which must be dimensioned at least 2*n+15.
//         the same work array can be used for both rfftf and rfftb
//         as long as n remains unchanged. different wsave arrays
//         are required for different values of n. the contents of
//         wsave must not be changed between calls of rfftf or rfftb.
// </dl>
// <group>
void rffti(Int n, Float * wsave);
void rffti(Int n, Double * wsave);
// </group>

// rfftf computes the Fourier coefficients of a real perodic sequence (Fourier
// analysis). The transform is defined below at output parameter r.
// 
// Input parameters:
// <dl compact>
// <dt><b>n</b>
// <dd>    the length of the array r to be transformed.  The method
//         is most efficient when n is a product of small primes.
//         n may change so long as different work arrays are provided
// <dt><b>r</b>
// <dd>    a real array of length n which contains the sequence
//         to be transformed
// <dt><b>wsave</b>
// <dd>    a work array which must be dimensioned at least 2*n+15
//         in the program that calls rfftf. The wsave array must be
//         initialized by calling subroutine <src>rffti(n,wsave)</src> and a
//         different wsave array must be used for each different
//         value of n. This initialization does not have to be
//         repeated so long as n remains unchanged thus subsequent
//         transforms can be obtained faster than the first.
//         The same wsave array can be used by rfftf and rfftb.
// </dl>
// output parameters
// <dl compact>
// <dt><b>r</b>
// <dd>    r(1) = the sum from i=1 to i=n of r(i)<br>
//         if n is even set l = n/2   , if n is odd set l = (n+1)/2<br>
//         then for k = 2,...,l<br>
//              r(2*k-2) = the sum from i = 1 to i = n of<br>
//                         r(i)*cos((k-1)*(i-1)*2*pi/n)<br>
//              r(2*k-1) = the sum from i = 1 to i = n of<br>
//                         -r(i)*sin((k-1)*(i-1)*2*pi/n)<br>
//         if n is even<br>
//              r(n) = the sum from i = 1 to i = n of<br>
//                   (-1)**(i-1)*r(i)<br>
// 
//         note:
//              this transform is unnormalized since a call of rfftf
//              followed by a call of rfftb will multiply the input
//              sequence by n.
// <dt><b>wsave</b>
// <dd>    contains results which must not be destroyed between
//         calls of rfftf or rfftb.
// </dl>
// <group>
void rfftf(Int n, Float * r, Float * wsave);
void rfftf(Int n, Double * r, Double * wsave);
// </group>


// rfftb computes the real perodic sequence from its Fourier coefficients
// (Fourier synthesis). The transform is defined below at output parameter r.
// 
// Input parameters:
// <dl compact>
// <dt><b>n</b>
// <dd>    the length of the array r to be transformed.  The method
//         is most efficient when n is a product of small primes.
//         n may change so long as different work arrays are provided
// <dt><b>r</b>
// <dd>    a real array of length n which contains the sequence
//         to be transformed
// <dt><b>wsave</b>
// <dd>    a work array which must be dimensioned at least 2*n+15
//         in the program that calls rfftb. The wsave array must be
//         initialized by calling subroutine <src>rffti(n,wsave)</src> and a
//         different wsave array must be used for each different
//         value of n. This initialization does not have to be
//         repeated so long as n remains unchanged thus subsequent
//         transforms can be obtained faster than the first.
//         The same wsave array can be used by rfftf and rfftb.
// </dl>
// Output parameters:
// <dl compact>
// <dt><b>r</b>
// <dd>    for n even and for i = 1,...,n<br>
//              r(i) = r(1)+(-1)**(i-1)*r(n)<br>
//                   plus the sum from k=2 to k=n/2 of<br>
//                    2.*r(2*k-2)*cos((k-1)*(i-1)*2*pi/n)<br>
//                   -2.*r(2*k-1)*sin((k-1)*(i-1)*2*pi/n)<br>
//         for n odd and for i = 1,...,n<br>
//              r(i) = r(1) plus the sum from k=2 to k=(n+1)/2 of<br>
//                   2.*r(2*k-2)*cos((k-1)*(i-1)*2*pi/n)<br>
//                  -2.*r(2*k-1)*sin((k-1)*(i-1)*2*pi/n)<br>
// 
//         note:
//              this transform is unnormalized since a call of rfftf
//              followed by a call of rfftb will multiply the input
//              sequence by n.
// <dt><b>wsave</b>
// <dd>    contains results which must not be destroyed between
//         calls of rfftb or rfftf.
// </dl>
// <group>
void rfftb(Int n, Float * r, Float * wsave);
void rfftb(Int n, Double * r, Double * wsave);
// </group>

// ezffti initializes the array wsave which is used in
// both ezfftf and ezfftb. The prime factorization of n together with
// a tabulation of the trigonometric functions are computed and
// stored in wsave.
// 
// Input parameter:
// <dl compact>
// <dt><b>n</b>
// <dd>    the length of the sequence to be transformed.
// </dl>
// Output parameter:
// <dl compact>
// <dt><b>wsave</b>
// <dd>    a work array which must be dimensioned at least 3*n+15.
//         The same work array can be used for both ezfftf and ezfftb
//         as long as n remains unchanged. Different wsave arrays
//         are required for different values of n.
// </dl>
void ezffti(Int n, Float * wsave);

// ezfftf computes the Fourier coefficients of a real
// perodic sequence (Fourier analysis). The transform is defined
// below at output parameters azero, a and b. ezfftf is a simplified
// but slower version of rfftf.
// 
// Input parameters:
// <dl compact>
// <dt><b>n</b>
// <dd>    the length of the array r to be transformed.  The method
//         is most efficient when n is the product of small primes.
// <dt><b>r</b>
// <dd>    a real array of length n which contains the sequence
//         to be transformed. r is not destroyed.
// <dt><b>wsave</b>
// <dd>    a work array which must be dimensioned at least 3*n+15
//         in the program that calls ezfftf. The wsave array must be
//         initialized by calling subroutine <src>ezffti(n,wsave)</src> and a
//         different wsave array must be used for each different
//         value of n. This initialization does not have to be
//         repeated so long as n remains unchanged thus subsequent
//         transforms can be obtained faster than the first.
//         The same wsave array can be used by ezfftf and ezfftb.
// </dl>
// Output parameters:
// <dl compact>
// <dt><b>azero</b>
// <dd>    the sum from i=1 to i=n of r(i)/n
// <dt><b>a,b</b>
// <dd>    real arrays of length n/2 (n even) or (n-1)/2 (n odd)<br>
//         for n even<br>
//            b(n/2)=0, and <br>
//            a(n/2) is the sum from i=1 to i=n of (-1)**(i-1)*r(i)/n<br>
// 
//         for n even define kmax=n/2-1<br>
//         for n odd  define kmax=(n-1)/2<br>
//         then for  k=1,...,kmax<br>
//            a(k) equals the sum from i=1 to i=n of<br>
//                   2./n*r(i)*cos(k*(i-1)*2*pi/n)<br>
//            b(k) equals the sum from i=1 to i=n of<br>
//                   2./n*r(i)*sin(k*(i-1)*2*pi/n)<br>
// </dl>
void ezfftf(Int n, Float * r, Float * azero, Float * a, Float * b, 
	    Float * wsave);

// ezfftb computes a real perodic sequence from its
// Fourier coefficients (Fourier synthesis). The transform is
// defined below at output parameter r. ezfftb is a simplified
// but slower version of rfftb.
// 
// Input parameters:
// <dl compact>
// <dt><b>n</b>       
// <dd>    the length of the output array r.  The method is most
//         efficient when n is the product of small primes.
// <dt><b>azero</b>
// <dd>    the constant Fourier coefficient
// <dt><b>a,b</b>
// <dd>    arrays which contain the remaining Fourier coefficients
//         these arrays are not destroyed.
//         The length of these arrays depends on whether n is even or
//         odd.
//         If n is even n/2    locations are required,
//         if n is odd (n-1)/2 locations are required.
// <dt><b>wsave</b>
// <dd>    a work array which must be dimensioned at least 3*n+15.
//         in the program that calls ezfftb. The wsave array must be
//         initialized by calling subroutine <src>ezffti(n,wsave)</src> and a
//         different wsave array must be used for each different
//         value of n. This initialization does not have to be
//         repeated so long as n remains unchanged thus subsequent
//         transforms can be obtained faster than the first.
//         The same wsave array can be used by ezfftf and ezfftb.
// </dl>
// Output parameters:
// <dl compact>
// <dt><b>r</b>
// <dd>    if n is even define kmax=n/2<br>
//         if n is odd  define kmax=(n-1)/2<br>
//         then for i=1,...,n<br>
//              r(i)=azero plus the sum from k=1 to k=kmax of<br>
//              a(k)*cos(k*(i-1)*2*pi/n)+b(k)*sin(k*(i-1)*2*pi/n)<br>
//         where<br>
//              c(k) = .5*cmplx(a(k),-b(k))   for k=1,...,kmax<br>
//              c(-k) = conjg(c(k))<br>
//              c(0) = azero<br>
//                   and i=sqrt(-1)<br>
// </dl>
void ezfftb(Int n, Float * r, Float * azero, Float * a, Float * b, 
	    Float * wsave);
// </group>

// <summary>Sine transform</summary>
// <synopsis>
// These are C++ wrapper functions for the FORTRAN sine transform
// routines in the files fftpak.f and dfftpack.f. The purpose of these
// definitions is to overload the functions so that C++ users can access the
// functions in either fftpak or dfftpack with an identical function names.

// These routines only do one-dimensional transforms with the first element of
// the array being the "origin" of the transform. These functions are not used
// <linkto class="FFTServer">FFTServer</linkto> class.

// As this function is its own inverse there is no need for sepearte forward or
// backward functions. Before using the transform <src>sint</src> the work
// array <src>wsave</src> must be initialised using the <src>sinti</src>
// function.

// <note role=warning> 
// These functions assume that it is possble to convert between AIPS++ numeric
// types and those used by Fortran. That it is possible to convert between
// Float & float, Double & double and Int & int.
// </note>
// </synopsis>
// <group name="sine-transform">

// sinti initializes the array wsave which is used in
// subroutine sint. The prime factorization of n together with
// a tabulation of the trigonometric functions are computed and
// stored in wsave.
// 
// Input parameter:
// <dl compact>
// <dt><b>n</b>
// <dd>    the length of the sequence to be transformed.  the method
//         is most efficient when n+1 is a product of small primes.
// </dl>
// Output parameter:
// <dl compact>
// <dt><b>wsave</b>
// <dd>    a work array with at least int(2.5*n+15) locations.
//         different wsave arrays are required for different values
//         of n. The contents of wsave must not be changed between
//         calls of sint.
// </dl>
// <group>
void sinti(Int n, Float * wsave);
void sinti(Int n, Double * wsave);
// </group>

// subroutine sint computes the discrete Fourier sine transform
// of an odd sequence x(i). The transform is defined below at
// output parameter x.
// sint is the unnormalized inverse of itself since a call of sint
// followed by another call of sint will multiply the input sequence
// x by 2*(n+1).
// The array wsave which is used by subroutine sint must be
// initialized by calling subroutine <src>sinti(n,wsave)</src>.
// 
// Input parameters:
// <dl compact>
// <dt><b>n</b>
// <dd>    the length of the sequence to be transformed.  The method
//         is most efficient when n+1 is the product of small primes.
// <dt><b>x</b>
// <dd>    an array which contains the sequence to be transformed
// <dt><b>wsave</b>
// <dd>    a work array with dimension at least int(2.5*n+15)
//         in the program that calls sint. The wsave array must be
//         initialized by calling subroutine <src>sinti(n,wsave)</src> and a
//         different wsave array must be used for each different
//         value of n. This initialization does not have to be
//         repeated so long as n remains unchanged thus subsequent
//         transforms can be obtained faster than the first.
// </dl>
// Output parameters:
// <dl compact>
// <dt><b>x</b>
// <dd>    for i=1,...,n<br>
//              x(i) = the sum from k=1 to k=n<br>
//                   2*x(k)*sin(k*i*pi/(n+1))<br>
// 
//              a call of sint followed by another call of
//              sint will multiply the sequence x by 2*(n+1).
//              Hence sint is the unnormalized inverse
//              of itself.
// 
// <dt><b>wsave</b>
// <dd>    contains initialization calculations which must not be
//         destroyed between calls of sint.
// </dl>
// <group>
void sint(Int n, Float * x, Float * wsave);
void sint(Int n, Double * x, Double * wsave);
// </group>
// </group>

// <summary>Cosine transform</summary>
// <synopsis>
// These are C++ wrapper functions for the FORTRAN cosine transform
// routines in the files fftpak.f and dfftpack.f. The purpose of these
// definitions is to overload the functions so that C++ users can access the
// functions in either fftpak or dfftpack with an identical function names.

// These routines only do one-dimensional transforms with the first element of
// the array being the "origin" of the transform. The <linkto
// class="FFTServer">FFTServer</linkto> class uses these functions to implement
// multi-dimensional real to real transforms with the origin of the transform
// either at the centre or the first element of the Array.

// As this function is its own inverse there is no need for separate forward or
// backward functions. Before using the transform <src>cost</src> the work
// array <src>wsave</src> must be initialised using the <src>costi</src>
// function.

// <note role=warning> 
// These functions assume that it is possble to convert between AIPS++ numeric
// types and those used by Fortran. That it is possible to convert between
// Float & float, Double & double and Int & int.
// </note>
// </synopsis>

// <group name="cosine-transform">

// costi initializes the array wsave which is used in
// subroutine cost. The prime factorization of n together with
// a tabulation of the trigonometric functions are computed and
// stored in wsave.
// 
// Input parameter:
// <dl compact>
// <dt><b>n</b>
// <dd>    the length of the sequence to be transformed.  The method
//         is most efficient when n-1 is a product of small primes.
// </dl>
// Output parameter:
// <dl compact>
// <dt><b>wsave</b>
// <dd>    a work array which must be dimensioned at least 3*n+15.
//         Different wsave arrays are required for different values
//         of n. The contents of wsave must not be changed between
//         calls of cost.
// </dl>
// <group>
void costi(Int n, Float * wsave);
void costi(Int n, Double * wsave);
// </group>

// cost computes the discrete Fourier cosine transform
// of an even sequence x(i). The transform is defined below at output
// parameter x.
// cost is the unnormalized inverse of itself since a call of cost
// followed by another call of cost will multiply the input sequence
// x by 2*(n-1). The transform is defined below at output parameter x.
// The array wsave which is used by subroutine cost must be
// initialized by calling subroutine <src>costi(n,wsave)</src>.
// 
// Input parameters:
// <dl compact>
// <dt><b>n</b>
// <dd>    the length of the sequence x. n must be greater than 1.
//         The method is most efficient when n-1 is a product of
//         small primes.
// <dt><b>x</b>
// <dd>    an array which contains the sequence to be transformed
// <dt><b>wsave</b>
// <dd>    a work array which must be dimensioned at least 3*n+15
//         in the program that calls cost. The wsave array must be
//         initialized by calling subroutine <src>costi(n,wsave)</src> and a
//         different wsave array must be used for each different
//         value of n. This initialization does not have to be
//         repeated so long as n remains unchanged thus subsequent
//         transforms can be obtained faster than the first.
// </dl>
// Output parameters:
// <dl compact>
// <dt><b>x</b>
// <dd>    for i=1,...,n<br>
//             x(i) = x(1)+(-1)**(i-1)*x(n)<br>
//              + the sum from k=2 to k=n-1<br>
//                  2*x(k)*cos((k-1)*(i-1)*pi/(n-1))<br>
// 
//              a call of cost followed by another call of
//              cost will multiply the sequence x by 2*(n-1)
//              hence cost is the unnormalized inverse
//              of itself.
// <dt><b>wsave</b>
// <dd>    contains initialization calculations which must not be
//         destroyed between calls of cost.
// </dl>
// <group>
void cost(Int n, Float * x, Float * wsave);
void cost(Int n, Double * x, Double * wsave);
// </group>
// </group>

// <summary>Quarter-wave sine transform</summary>
// <group name="quarter-wave-sine-transform">
// <group>
void sinqi(Int n, Float * wsave);
void sinqi(Int n, Double * wsave);
// </group>
// <group>
void sinqf(Int n, Float * x, Float * wsave);
void sinqf(Int n, Double * x, Double * wsave);
// </group>
// <group>
void sinqb(Int n, Float * x, Float * wsave);
void sinqb(Int n, Double * x, Double * wsave);
// </group>
// </group>

// <summary>Quarter-wave cosine transform</summary>
// <group name="quarter-wave-cosine-transform">
// <group>
void cosqi(Int n, Float * wsave);
void cosqi(Int n, Double * wsave);
// </group>
// <group>
void cosqf(Int n, Float * x, Float * wsave);
void cosqf(Int n, Double * x, Double * wsave);
// </group>
// <group>
void cosqb(Int n, Float * x, Float * wsave);
void cosqb(Int n, Double * x, Double * wsave);
// </group>
// </group>
#endif
