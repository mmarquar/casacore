//# Constants.h: Mathematical and numerical constants
//# Copyright (C) 1993,1994,1995,1997,1998,1999,2000,2001
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

#if !defined(AIPS_C_H)
#define AIPS_C_H

#if defined (sun) && ! defined (AIPS_SOLARIS) 
#   include <sys/limits.h>
#else
#   include <limits.h>
#endif

#include <float.h>
#if !defined(AIPS_DARWIN)
#include <values.h>
#endif

#if defined (AIPS_OSF)
#   define LN_MAXFLOAT (M_LN2 * FMAXEXP)
#   define LN_MINFLOAT (M_LN2 * (FMINEXP -1))
#endif
#include <aips/aips.h>

// <summary>Mathematical and numerical constants.</summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="tConstants" demos="">

//# // <prerequisite>
//# // </prerequisite>

//# // <etymology>
//# // </etymology>

// <synopsis>
// The constants and conversion factors are defined here as double precision
// values.  Where single precision calculations are done in a situation where
// processing speed is of concern, for example within the inner loop of an
// expensive algorithm, a separate single precision variable should be defined
// for use within the loop.
//
// </synopsis>
//
// <note role=warning> The following list is generated by hand, and may be incomplete.
// After future revision of ccx2html the real data will be displayed </note>

// <h3>Floating point limits</h3>
// <srcblock>
// flt_min		the minimum single precision floating point number, 
// 			excluding denormalised numbers
// minfloat		the minimum single precision floating point number, 
// 			including denormalised numbers
// dbl_min		the minimum double precision floating point number,
// 			excluding denormalised numbers
// mindouble		the minimum double precision floating point number,
// 			including denormalised numbers
// flt_max		the maximum single precision floating point number 
// dbl_max		the maximum double precision floating point number 
// flt_epsilon		Minimum single precision floating point number X
// 			such that 1+X does not equal X
// dbl_epsilon		Minimum double precision floating point number X
//			such that 1+X does not equal X
// </srcblock>
// <h3> Irrationals </h3>
// <srcblock>
// sqrt2		sqrt(2)
// sqrt3		sqrt(3)
// _1_sqrt2		1/sqrt(2)
// _1_sqrt3		1/sqrt(3)
// </srcblock>

// <h3>Pi and functions thereof</h3>
// <srcblock>
// pi                   pi
// _2pi                 2*pi
// pi_2                 pi/2
// pi_4                 pi/4
// _1_pi                1/pi
// _2_pi                2/pi
// _1_sqrtpi            1/sqrt(pi)
// _2_sqrtpi            2/sqrt(pi)
// </srcblock>

// <h3>e and functions thereof</h3>
// <srcblock>
// e                    e
// ln2                  ln(2)
// ln10                 ln(10)
// log2e                log2(e)
// log10e               log10(e)
// </srcblock>

// <h3>gamma and functions thereof</h3>
// <srcblock>
// gamma		gamma
// lngamma		ln(gamma)
// etogamma		e**gamma
// </srcblock>

// <h3>Fundamental physical constants (SI units)</h3>
// <note role=warning> Preserved for legacy reasons only.
// See <linkto class=QC>QC class</linkto> for other physical constants. </note>
// <srcblock>
// c			velocity of light (m/s)
// </srcblock>

// <h3>Numerical conversion factors</h3>
// <srcblock>
// yotta                e+24 (Y)
// zetta                e+21 (Z)
// exa                  e+18 (E)
// peta                 e+15 (P)
// tera                 e+12 (T)
// giga                 e+09 (G)
// mega                 e+06 (M)
// kilo                 e+03 (k)
// hecto                e+02 (h)
// deka                 e+01 (da)
// deci                 e-01 (d)
// centi                e-02 (c)
// milli                e-03 (m)
// micro                e-06 (u)
// nano                 e-09 (n)
// pico                 e-12 (p)
// femto                e-15 (f)
// atto                 e-18 (a)
// zepto                e-21 (z)
// yocto                e-24 (y)
// </srcblock>

// <h3>Angular measure</h3>
// <srcblock>
// radian		radian
// circle		circle
// degree		degree
// arcmin		arcminute
// arcsec		arcsecond
// </srcblock>

// <h3>Solid angular measure</h3>
// <srcblock>
// steradian		steradian
// sphere		sphere
// square_degree	square degree
// square_arcmin	square arcminute
// square_arcsec	square arcsecond
// </srcblock>

// <h3>Time interval</h3>
// <srcblock>
// second               second
// minute               minute
// hour                 hour
// day                  day
// </srcblock>

// <h3> Machine constants </h3>
//
// Implementation-defined limits usually defined in <src><limits.h></src>,
// <src><float.h></src>, and <src><values.h></src> as preprocessor
// defines. They are 
// Inclusion of <src><aips/Mathematics/Constants.h</src> is
// sufficient to ensure that they are defined for any particular
// implementation, and the correct functioning of the <src>tConstants</src>
// test program guarantees this.
//
// In future use will be made of the (standard) <em>numeric_limits</em>
// template from the <src><limits></src> include file.

// <srcblock>
//
// Refer to Section 3.2c, pp28-30 of
// "The Annotated C++ Reference Manual",
// Ellis, M.A., and Stroustrup, B.,
// Addison-Wesley Publishing Company, 1990.
// IBSN 0-201-51459-1.
//
//    and
//
// Appendix B11, pp257-8 of
// "The C Programming Language", 2nd ed.,
// Kernighan, B.W., and Ritchie, D.M.,
// Prentice Hall Software Series, 1988.
// IBSN 0-13-110362-8.
//
// </srcblock>

// <h3> Constants defined in limits.h </h3>
// (these are part of the ANSI C and hence POSIX standards).
// Acceptable limits defined by the standard are quoted.
// <srcblock>
//
// CHAR_BIT              8  Maximum bits in a byte.
// CHAR_MIN           0 or  Minimum value of 'char'.
//               SCHAR_MIN
// CHAR_MAX   UCHAR_MAX or  Maximum value of 'char'.
//               SCHAR_MAX
// SCHAR_MIN          -127  Minimum value of 'signed char'.
// SCHAR_MAX          +127  Maximum value of 'signed char'.
// UCHAR_MAX           255  Maximum value of 'unsigned char'.
// MB_LEN_MAX               Maximum bytes in multibyte character.
//
// SHRT_MIN         -32767  Minimum value of 'short'.
// SHRT_MAX         +32767  Maximum value of 'short'.
// USHRT_MAX         65535  Maximum value of 'unsigned short'.
//
// INT_MIN          -32767  Minimum value of 'int'.
// INT_MAX          +32767  Maximum value of 'int'.
// UINT_MAX          65535  Maximum value of 'unsigned int'.
//
// LONG_MIN    -2147483647  Minimum value of 'long'.
// LONG_MAX    +2147483647  Maximum value of 'long'.
// ULONG_MAX    4294967295  Maximum value of 'unsigned long'.
//
// </srcblock>

// <h3> Constants defined in float.h </h3>
// (these are part of the ANSI C and hence POSIX standards).
// Acceptable limits defined by the standard are quoted.
// <srcblock>
//
// FLT_RADIX             2  Radix of exponent representation.
// FLT_ROUNDS               Floating point rounding mode for addition
//                            -1: indeterminate
//                             0: towards zero
//                             1: to nearest
//                             2: toward +infinity
//                             3: toward -infinity
//
// FLT_MIN_EXP              Minimum negative integer N such that FLT_RADIX
// DBL_MIN_EXP              raised to the Nth minus 1 is a normalized
// LDBL_MIN_EXP             floating point number.
//
// FLT_MAX_EXP              Maximum integer N such that FLT_RADIX raised to
// DBL_MAX_EXP              the Nth minus 1 is representable.
// LDBL_MAX_EXP
//
// FLT_MIN_10_EXP      -37  Minimum negative integer N such that 10 raised
// DBL_MIN_10_EXP      -37  to the Nth is in the range of normalized
// LDBL_MIN_10_EXP     -37  floating point numbers.
//
// FLT_MAX_10_EXP       37  Maximum integer N such that 10 raised to the
// DBL_MAX_10_EXP       37  Nth minus 1 is representable.
// LDBL_MAX_10_EXP      37
//
// FLT_MANT_DIG             Number of base FLT_RADIX digits in mantissa.
// DBL_MANT_DIG
// LDBL_MANT_DIG 
//
// FLT_DIG               6  Decimal digits of precision.
// DBL_DIG              10
// LDBL_DIG             10
//
// FLT_EPSILON        1E-5  Minimum floating point number X such  that
// (use C::flt_epsilon in preference to this)
// DBL_EPSILON        1E-9  1.0 + X does not equal 1.0.
// (use C::dbl_epsilon in preference to this)
// LDBL_EPSILON       1E-9
//
// FLT_MIN           1E-37  Minimum normalized positive floating point
// (use C::flt_min in preference to this)
// DBL_MIN           1E-37  number
// (use C::dbl_min in preference to this)
// LDBL_MIN          1E-37
//
// FLT_MAX           1E+37  Maximum representable floating point number.
// (use C::flt_max in preference to this)
// DBL_MAX           1E+37
// (use C::dbl_max in preference to this)
// LDBL_MAX          1E+37
//
// </srcblock>

// <h3> Constants defined in values.h </h3>
// (not part of the POSIX standard).
// <note role=warning> These constants will disappear in the near future.
// Do not use them in new code. </note>

// <srcblock>
//
// HIBITS            Value of a short integer with only the high-order
//                   bit set (in most implementations, 0x8000).
//
// HIBITL            Value of a long integer with only the high-order
//                   bit set (in most implementations, 0x80000000).
//
// MAXSHORT          Maximum value of a signed short integer (in most
//                   implementations, 0x7FFF = 32767).
//
// MAXLONG           Maximum value of a signed long integer (in most
//                   implementations, 0x7FFFFFFF = 2147483647).
//
// MAXINT            Maximum value of a signed regular integer (usually
//                   the same as MAXSHORT or MAXLONG).
//
// MINFLOAT          Minimum positive value of a single-precision
//                   floating-point number (use C::minfloat in preference 
//                   to this)
//
// MINDOUBLE         Minimum positive value of a double-precision
//                   floating-point number (use C::mindouble in preference 
//                   to this)
//
// MAXFLOAT          Maximum value of a single-precision floating-point number
//
// MAXDOUBLE         Maximum value of a double-precision floating-point number
//
// FSIGNIF           Number of significant bits in the mantissa of a
//                   single-precision floating-point number.
//
// DSIGNIF           Number of significant bits in the mantissa of a
//                   double-precision floating-point number.
// </srcblock>

// <note role=tip>
// Once cxx2html accepts the namespace type, the Dummy_Constants_Class can be
// removed.<br>
// Anyway, to use a constant (e.g. pi) use the C::pi construct now and later.
// </note>


class Dummy_Constants_class {
};

namespace C {
  //#--------------------------------------------------------------------
  //  Mathematical constants
  //#--------------------------------------------------------------------
  //# <group>
  
  // Irrationals:
  // <group>
  // sqrt(2)
  extern const Double sqrt2;
  // sqrt(3)
  extern const Double sqrt3;
  // 1/sqrt(2)
  extern const Double _1_sqrt2;
  // 1/sqrt(3)
  extern const Double _1_sqrt3;
  // </group>

  // Pi and functions thereof:
  // <group>
  // pi
  extern const Double pi;
  // 2*pi
  extern const Double _2pi;
  // pi/2
  extern const Double pi_2;
  // pi/4
  extern const Double pi_4;
  // 1/pi
  extern const Double _1_pi;
  // 2/pi
  extern const Double _2_pi;
  // 1/sqrt(pi)
  extern const Double _1_sqrtpi;
  // 2/sqrt(pi)
  extern const Double _2_sqrtpi;
  // </group>
  
  // e and functions thereof:
  // <group>
  // e
  extern const Double e;
  // ln(2)
  extern const Double ln2;  
  // ln(10)
  extern const Double ln10; 
  // log2(e)
  extern const Double log2e; 
  // log10(e)
  extern const Double log10e;
  // </group>
  
  // gamma and functions thereof:
  // <group>
  // gamma
  extern const Double gamma;
  // ln(gamma)
  extern const Double lngamma;
  // e**gamma
  extern const Double etogamma;
  // </group>
  
  //#--------------------------------------------------------------------
  //# Mathematical constants
  //#--------------------------------------------------------------------
  //# </group>
  
  
  //#--------------------------------------------------------------------
  //  Machine constants
  //#--------------------------------------------------------------------
  //# <group>
  // floating point limits
  // <group>
  // the minimum single precision floating point number, 
  // excluding denormalised numbers
  extern const Double flt_min;
  // the minimum single precision floating point number, 
  // including denormalised numbers
  extern const Double minfloat;
  // the minimum double precision floating point number,
  // excluding denormalised numbers
  extern const Double dbl_min;
  // the minimum double precision floating point number,
  // including denormalised numbers
  extern const Double mindouble;
  // the maximum single precision floating point number 
  extern const Double  flt_max;
  // the maximum double precision floating point number 
  extern const Double  dbl_max;
  // Minimum single precision floating point number X such that 1+X does not
  // equal X
  extern const Double flt_epsilon;
  // Minimum double precision floating point number X such that 1+X does not
  // equal X
  extern const Double dbl_epsilon;
  // </group>
  
  //#--------------------------------------------------------------------
  //# Machine constants
  //#--------------------------------------------------------------------
  //# </group>
  
  //#--------------------------------------------------------------------
  //#  Physical constants, and quantities
  //#--------------------------------------------------------------------
  //# <group>
  
  // Fundamental physical constants (SI units):
  // <group>
  // velocity of light -- for legacy reasons only -- refer to PC
  extern const Double c;
  // </group>
  
  //#--------------------------------------------------------------------
  //# Physical constants, and quantities
  //#--------------------------------------------------------------------
  //# </group>
  
  
  
  //#--------------------------------------------------------------------
  //#  Physical units
  //#--------------------------------------------------------------------
  //# <group>
  
  //#-----------------------------
  //#  Numerical conversion factors
  //#-----------------------------
  //# <group>
  
  //  Numerical conversion factors
  // <group>
  // e+24 (Y)
  extern const Double yotta;
  // e+21 (Z)
  extern const Double zetta;
  // e+18 (E)
  extern const Double exa;
  // e+15 (P)
  extern const Double peta;
  // e+12 (T)
  extern const Double tera;
  // e+09 (G)
  extern const Double giga;
  // e+06 (M)
  extern const Double mega;
  // e+03 (k)
  extern const Double kilo;
  // e+02 (h)
  extern const Double hecto;
  // e+01 (da)
  extern const Double deka;
  // e-01 (d)
  extern const Double deci;
  // e-02 (c)
  extern const Double centi;
  // e-03 (m)
  extern const Double milli;
  // e-06 (u)
  extern const Double micro;
  // e-09 (n)
  extern const Double nano;
  // e-12 (p)
  extern const Double pico;
  // e-15 (f)
  extern const Double femto;
  // e-18 (a)
  extern const Double atto;
  // e-21 (z)
  extern const Double zepto;
  // e-24 (y)
  extern const Double yocto;
  // </group>
  
  // Angular measure:
  // <group>
  // radian
  extern const Double radian;
  // circle
  extern const Double circle; 
  // degree
  extern const Double degree;
  // arcminute
  extern const Double arcmin;
  // arcsecond
  extern const Double arcsec;
  // </group>
  
  // Solid angular measure:
  // <group>
  // steradian
  extern const Double steradian;
  // sphere
  extern const Double sphere;
  // square degree
  extern const Double square_degree;
  // square arcminute
  extern const Double square_arcmin;
  // square arcsecond
  extern const Double square_arcsec;
  // </group>
  
  //#-----------------------------
  //# Numerical conversion factors
  //#-----------------------------
  //# </group>
  
  
  //#----------------------------
  //#  Physical conversion factors
  //#----------------------------
  //# <group>
  
  // Time interval [T]:
  // <group>
  // second
  extern const Double second;
  // minute
  extern const Double minute;
  // hour
  extern const Double hour;
  // day
  extern const Double day;
  // </group>
  
  //#----------------------------
  //# Physical conversion factors
  //#----------------------------
  //# <group>
  
  //#--------------------------------------------------------------------
  //# Physical units
  //#--------------------------------------------------------------------
  //# <group>
  
};

#endif
