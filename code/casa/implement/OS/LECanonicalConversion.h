//# LECanonicalConversion.h: A class with static functions to convert little endian canonical format
//# Copyright (C) 2002
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

#if !defined(AIPS_LECANONICALCONVERSION_H)
#define AIPS_LECANONICALCONVERSION_H

//# Includes
#include <casa/aipsxtype.h>
#include <casa/OS/Conversion.h>


// Define the little endian canonical sizes of the built-in data types.
// These are the same for all machine architectures.

#define SIZE_LECAN_CHAR     1
#define SIZE_LECAN_UCHAR    1
#define SIZE_LECAN_SHORT    2
#define SIZE_LECAN_USHORT   2
#define SIZE_LECAN_INT      4
#define SIZE_LECAN_UINT     4
#define SIZE_LECAN_INT64    8
#define SIZE_LECAN_UINT64   8
#define SIZE_LECAN_FLOAT    4
#define SIZE_LECAN_DOUBLE   8
//#//define SIZE_LECAN_LDOUBLE 16


// Define for each data format if a conversion is needed from the
// local format to the little endian canonical format (or vice-versa).
// This allows for optimizations in, for example, AipsIO.
// The canonical format is ASCII for strings, IEEE for floating point
// and 2-complement for integers (all most significant bit last)
// with the lengths as shown above.
// The function checkConvert() can be used to check if the flags are
// set correctly.

// Conversion is needed for big endian architectures (like SUN),
// because the bytes have to be swapped (thus not for data with length 1).
#define CONVERT_LECAN_CHAR     0
#define CONVERT_LECAN_UCHAR    0

#if !defined(AIPS_LITTLE_ENDIAN)
#  define CONVERT_LECAN_SHORT    1
#  define CONVERT_LECAN_USHORT   1
#  define CONVERT_LECAN_INT      1
#  define CONVERT_LECAN_UINT     1
#  define CONVERT_LECAN_INT64    1
#  define CONVERT_LECAN_UINT64   1
#  define CONVERT_LECAN_FLOAT    1
#  define CONVERT_LECAN_DOUBLE   1
//#//#  define CONVERT_LECAN_LDOUBLE  1
#else

// Conversion is not needed for IEEE data.
// Change the definitions below if new architectures are being used.
#  define CONVERT_LECAN_SHORT    0
#  define CONVERT_LECAN_USHORT   0
#  define CONVERT_LECAN_INT      0
#  define CONVERT_LECAN_UINT     0
#  define CONVERT_LECAN_INT64    0
#  define CONVERT_LECAN_UINT64   0
#  define CONVERT_LECAN_FLOAT    0
#  define CONVERT_LECAN_DOUBLE   0
// LDOUBLE is 8 bytes on SUN, but 16 bytes canonical.
//#//#  define CONVERT_LECAN_LDOUBLE  1
#endif



// <summary>
// A class with static functions to convert little endian canonical format
// </summary>

// <use visibility=export>

// <reviewed reviewer="Friso Olnon" date="1996/11/06" tests="tLECanonicalConversion" demos="">
// </reviewed>

// <synopsis>
// This class consists of several static functions to convert
// data from local (=native) format to a little endian canonical format.
// The canonical length of each data type is:
// <br>- Bool: 1 bit
// <br>- char: 1 byte
// <br>- short: 2 bytes
// <br>- int: 4 bytes
// <br>- Int64: 8 bytes
// <br>- float: 4 bytes
// <br>- double: 8 bytes
// <br> This canonical format is little-endian IEEE format, so on PCs
// the conversion is only a copy operation. On e.g. SUNs
// however, it involves a byte swap to convert from little
// endian to big endian.
// <p>
// The class also contains conversion functions making it possible to
// specify the number of bytes (in local format) instead of the number
// of values. These functions are included to make it possible to have
// the same signature as memcpy.
// <p>
// The current implementation of this class works on big- and little-endian
// machines using IEEE format. When using on other machines (e.g. VAX)
// the toLocal and fromLocal functions have to be changed.
// <p>
// Note that no functions are provided to handle Bools. Instead class
// <linkto class=Conversion>Conversion</linkto> provides functions to
// convert Bools to/from bits.
// </synopsis>

// <example>
// <srcblock>
// void someFunction (const uInt* data, uInt nrval)
// {
//     char* buffer = new char[nrval*LECanonicalConversion::canonicalSize(data)];
//     LECanonicalConversion::fromLocal (buffer, data, nrval);
//     ....
//     delete [] buffer;
// }
// </srcblock>
// </example>

// <motivation>
// AIPS++ data will be stored in a canonical format.
// To read these data conversion functions are needed.
// However, these functions do not use any other AIPS++ classes,
// so they can easily be used in any other software system.
// </motivation>

// <todo asof="$DATE$">
//  <li> Support data type long double.
// </todo>


class LECanonicalConversion
{
public:
    // Convert one value from canonical format to local format.
    // The from and to buffer should not overlap.
    // <group>
    static unsigned int toLocal (char&           to, const void* from);
    static unsigned int toLocal (unsigned char&  to, const void* from);
    static unsigned int toLocal (short&          to, const void* from);
    static unsigned int toLocal (unsigned short& to, const void* from);
    static unsigned int toLocal (int&            to, const void* from);
    static unsigned int toLocal (unsigned int&   to, const void* from);
    static unsigned int toLocal (Int64&          to, const void* from);
    static unsigned int toLocal (uInt64&         to, const void* from);
    static unsigned int toLocal (float&          to, const void* from);
    static unsigned int toLocal (double&         to, const void* from);
    // </group>
    
    // Convert one value from local format to canonical format.
    // The from and to buffer should not overlap.
    // <group>
    static unsigned int fromLocal (void* to, const char&           from);
    static unsigned int fromLocal (void* to, const unsigned char&  from);
    static unsigned int fromLocal (void* to, const short&          from);
    static unsigned int fromLocal (void* to, const unsigned short& from);
    static unsigned int fromLocal (void* to, const int&            from);
    static unsigned int fromLocal (void* to, const unsigned int&   from);
    static unsigned int fromLocal (void* to, const Int64&          from);
    static unsigned int fromLocal (void* to, const uInt64&         from);
    static unsigned int fromLocal (void* to, const float&          from);
    static unsigned int fromLocal (void* to, const double&         from);
    // </group>
    
    // Convert nr values from canonical format to local format.
    // The from and to buffer should not overlap.
    // <group>
    static unsigned int toLocal (char*           to, const void* from,
				 unsigned int nr);
    static unsigned int toLocal (unsigned char*  to, const void* from,
				 unsigned int nr);
    static unsigned int toLocal (short*          to, const void* from,
				 unsigned int nr);
    static unsigned int toLocal (unsigned short* to, const void* from,
				 unsigned int nr);
    static unsigned int toLocal (int*            to, const void* from,
				 unsigned int nr);
    static unsigned int toLocal (unsigned int*   to, const void* from,
				 unsigned int nr);
    static unsigned int toLocal (Int64*          to, const void* from,
				 unsigned int nr);
    static unsigned int toLocal (uInt64*         to, const void* from,
				 unsigned int nr);
    static unsigned int toLocal (float*          to, const void* from,
				 unsigned int nr);
    static unsigned int toLocal (double*         to, const void* from,
				 unsigned int nr);
    // </group>

    // Convert nr values from local format to canonical format.
    // The from and to buffer should not overlap.
    // <group>
    static unsigned int fromLocal (void* to, const char*           from,
				   unsigned int nr);
    static unsigned int fromLocal (void* to, const unsigned char*  from,
				   unsigned int nr);
    static unsigned int fromLocal (void* to, const short*          from,
				   unsigned int nr);
    static unsigned int fromLocal (void* to, const unsigned short* from,
				   unsigned int nr);
    static unsigned int fromLocal (void* to, const int*            from,
				   unsigned int nr);
    static unsigned int fromLocal (void* to, const unsigned int*   from,
				   unsigned int nr);
    static unsigned int fromLocal (void* to, const Int64*          from,
				   unsigned int nr);
    static unsigned int fromLocal (void* to, const uInt64*         from,
				   unsigned int nr);
    static unsigned int fromLocal (void* to, const float*          from,
				   unsigned int nr);
    static unsigned int fromLocal (void* to, const double*         from,
				   unsigned int nr);
    // </group>

    // Convert nr values from canonical format to local format.
    // The from and to buffer should not overlap.
    // <group>
    static unsigned int toLocalChar     (void* to, const void* from,
					 unsigned int nr);
    static unsigned int toLocalUChar    (void* to, const void* from,
					 unsigned int nr);
    static unsigned int toLocalShort    (void* to, const void* from,
					 unsigned int nr);
    static unsigned int toLocalUShort   (void* to, const void* from,
					 unsigned int nr);
    static unsigned int toLocalInt      (void* to, const void* from,
					 unsigned int nr);
    static unsigned int toLocalUInt     (void* to, const void* from,
					 unsigned int nr);
    static unsigned int toLocalInt64    (void* to, const void* from,
					 unsigned int nr);
    static unsigned int toLocalUInt64   (void* to, const void* from,
					 unsigned int nr);
    static unsigned int toLocalFloat    (void* to, const void* from,
					 unsigned int nr);
    static unsigned int toLocalDouble   (void* to, const void* from,
					 unsigned int nr);
    // </group>
    
    // Convert nr values from local format to canonical format.
    // The from and to buffer should not overlap.
    // <group>
    static unsigned int fromLocalChar     (void* to, const void* from,
					   unsigned int nr);
    static unsigned int fromLocalUChar    (void* to, const void* from,
					   unsigned int nr);
    static unsigned int fromLocalShort    (void* to, const void* from,
					   unsigned int nr);
    static unsigned int fromLocalUShort   (void* to, const void* from,
					   unsigned int nr);
    static unsigned int fromLocalInt      (void* to, const void* from,
					   unsigned int nr);
    static unsigned int fromLocalUInt     (void* to, const void* from,
					   unsigned int nr);
    static unsigned int fromLocalInt64    (void* to, const void* from,
					   unsigned int nr);
    static unsigned int fromLocalUInt64   (void* to, const void* from,
					   unsigned int nr);
    static unsigned int fromLocalFloat    (void* to, const void* from,
					   unsigned int nr);
    static unsigned int fromLocalDouble   (void* to, const void* from,
					   unsigned int nr);
    // </group>
    
    // Convert values from canonical format to local format.
    // The from and to buffer should not overlap.
    // The number of values involved is determined from the argument
    // <src>nrbytes</src>, which gives the number of bytes in local format.
    // The signature of this function is the same as <src>memcpy</src>, so
    // that memcpy can directly be used if no conversion is needed.
    // <group>
    static void* byteToLocalChar     (void* to, const void* from,
				      unsigned int nrbytes);
    static void* byteToLocalUChar    (void* to, const void* from,
				      unsigned int nrbytes);
    static void* byteToLocalShort    (void* to, const void* from,
				      unsigned int nrbytes);
    static void* byteToLocalUShort   (void* to, const void* from,
				      unsigned int nrbytes);
    static void* byteToLocalInt      (void* to, const void* from,
				      unsigned int nrbytes);
    static void* byteToLocalUInt     (void* to, const void* from,
				      unsigned int nrbytes);
    static void* byteToLocalInt64    (void* to, const void* from,
				      unsigned int nrbytes);
    static void* byteToLocalUInt64   (void* to, const void* from,
				      unsigned int nrbytes);
    static void* byteToLocalFloat    (void* to, const void* from,
				      unsigned int nrbytes);
    static void* byteToLocalDouble   (void* to, const void* from,
				      unsigned int nrbytes);
    // </group>
    
    // Convert values from local format to canonical format.
    // The from and to buffer should not overlap.
    // The number of values involved is determined from the argument
    // <src>nrbytes</src>, which gives the number of bytes in local format.
    // The signature of this function is the same as <src>memcpy</src>, so
    // that memcpy can directly be used if no conversion is needed.
    // <group>
    static void* byteFromLocalChar     (void* to, const void* from,
					unsigned int nrbytes);
    static void* byteFromLocalUChar    (void* to, const void* from,
					unsigned int nrbytes);
    static void* byteFromLocalShort    (void* to, const void* from,
					unsigned int nrbytes);
    static void* byteFromLocalUShort   (void* to, const void* from,
					unsigned int nrbytes);
    static void* byteFromLocalInt      (void* to, const void* from,
					unsigned int nrbytes);
    static void* byteFromLocalUInt     (void* to, const void* from,
					unsigned int nrbytes);
    static void* byteFromLocalInt64    (void* to, const void* from,
					unsigned int nrbytes);
    static void* byteFromLocalUInt64   (void* to, const void* from,
					unsigned int nrbytes);
    static void* byteFromLocalFloat    (void* to, const void* from,
					unsigned int nrbytes);
    static void* byteFromLocalDouble   (void* to, const void* from,
					unsigned int nrbytes);
    // </group>
    
    // Get the value conversion function for the given type.
    // <group>
    static Conversion::ValueFunction* getToLocal (const char*);
    static Conversion::ValueFunction* getToLocal (const unsigned char*);
    static Conversion::ValueFunction* getToLocal (const short*);
    static Conversion::ValueFunction* getToLocal (const unsigned short*);
    static Conversion::ValueFunction* getToLocal (const int*);
    static Conversion::ValueFunction* getToLocal (const unsigned int*);
    static Conversion::ValueFunction* getToLocal (const Int64*);
    static Conversion::ValueFunction* getToLocal (const uInt64*);
    static Conversion::ValueFunction* getToLocal (const float*);
    static Conversion::ValueFunction* getToLocal (const double*);
    static Conversion::ValueFunction* getFromLocal (const char*);
    static Conversion::ValueFunction* getFromLocal (const unsigned char*);
    static Conversion::ValueFunction* getFromLocal (const short*);
    static Conversion::ValueFunction* getFromLocal (const unsigned short*);
    static Conversion::ValueFunction* getFromLocal (const int*);
    static Conversion::ValueFunction* getFromLocal (const unsigned int*);
    static Conversion::ValueFunction* getFromLocal (const Int64*);
    static Conversion::ValueFunction* getFromLocal (const uInt64*);
    static Conversion::ValueFunction* getFromLocal (const float*);
    static Conversion::ValueFunction* getFromLocal (const double*);
    // </group>

    // Get the byte conversion function for the given type.
    // The function <src>memcpy</src> is returned when a conversion
    // is not needed.
    // <group>
    static Conversion::ByteFunction* getByteToLocal (const char*);
    static Conversion::ByteFunction* getByteToLocal (const unsigned char*);
    static Conversion::ByteFunction* getByteToLocal (const short*);
    static Conversion::ByteFunction* getByteToLocal (const unsigned short*);
    static Conversion::ByteFunction* getByteToLocal (const int*);
    static Conversion::ByteFunction* getByteToLocal (const unsigned int*);
    static Conversion::ByteFunction* getByteToLocal (const Int64*);
    static Conversion::ByteFunction* getByteToLocal (const uInt64*);
    static Conversion::ByteFunction* getByteToLocal (const float*);
    static Conversion::ByteFunction* getByteToLocal (const double*);
    static Conversion::ByteFunction* getByteFromLocal (const char*);
    static Conversion::ByteFunction* getByteFromLocal (const unsigned char*);
    static Conversion::ByteFunction* getByteFromLocal (const short*);
    static Conversion::ByteFunction* getByteFromLocal (const unsigned short*);
    static Conversion::ByteFunction* getByteFromLocal (const int*);
    static Conversion::ByteFunction* getByteFromLocal (const unsigned int*);
    static Conversion::ByteFunction* getByteFromLocal (const Int64*);
    static Conversion::ByteFunction* getByteFromLocal (const uInt64*);
    static Conversion::ByteFunction* getByteFromLocal (const float*);
    static Conversion::ByteFunction* getByteFromLocal (const double*);
    // </group>

    // Return the canonical length for the various data types.
    // <group>
    static unsigned int canonicalSize (const char*);
    static unsigned int canonicalSize (const unsigned char*);
    static unsigned int canonicalSize (const short*);
    static unsigned int canonicalSize (const unsigned short*);
    static unsigned int canonicalSize (const int*);
    static unsigned int canonicalSize (const unsigned int*);
    static unsigned int canonicalSize (const Int64*);
    static unsigned int canonicalSize (const uInt64*);
    static unsigned int canonicalSize (const float*);
    static unsigned int canonicalSize (const double*);
    //#//static unsigned int canonicalSize (const long double*);
    // </group>

    // Reverse 2 bytes.
    static void reverse2 (void* to, const void* from);

    // Reverse 4 bytes.
    static void reverse4 (void* to, const void* from);

    // Reverse 8 bytes.
    static void reverse8 (void* to, const void* from);

    // Move 2 bytes.
    static void move2 (void* to, const void* from);

    // Move 4 bytes.
    static void move4 (void* to, const void* from);

    // Move 8 bytes.
    static void move8 (void* to, const void* from);

private:
    // This class should not be constructed
    // (so declare the constructor private).
    LECanonicalConversion();
};



inline void LECanonicalConversion::reverse2 (void* to, const void* from)
{
    ((char*)to)[0] = ((const char*)from)[1];
    ((char*)to)[1] = ((const char*)from)[0];
}
inline void LECanonicalConversion::reverse4 (void* to, const void* from)
{
    ((char*)to)[0] = ((const char*)from)[3];
    ((char*)to)[1] = ((const char*)from)[2];
    ((char*)to)[2] = ((const char*)from)[1];
    ((char*)to)[3] = ((const char*)from)[0];
}
inline void LECanonicalConversion::reverse8 (void* to, const void* from)
{
    ((char*)to)[0] = ((const char*)from)[7];
    ((char*)to)[1] = ((const char*)from)[6];
    ((char*)to)[2] = ((const char*)from)[5];
    ((char*)to)[3] = ((const char*)from)[4];
    ((char*)to)[4] = ((const char*)from)[3];
    ((char*)to)[5] = ((const char*)from)[2];
    ((char*)to)[6] = ((const char*)from)[1];
    ((char*)to)[7] = ((const char*)from)[0];
}

inline void LECanonicalConversion::move2 (void* to, const void* from)
{
    ((char*)to)[0] = ((const char*)from)[0];
    ((char*)to)[1] = ((const char*)from)[1];
}
inline void LECanonicalConversion::move4 (void* to, const void* from)
{
    ((char*)to)[0] = ((const char*)from)[0];
    ((char*)to)[1] = ((const char*)from)[1];
    ((char*)to)[2] = ((const char*)from)[2];
    ((char*)to)[3] = ((const char*)from)[3];
}
inline void LECanonicalConversion::move8 (void* to, const void* from)
{
    ((char*)to)[0] = ((const char*)from)[0];
    ((char*)to)[1] = ((const char*)from)[1];
    ((char*)to)[2] = ((const char*)from)[2];
    ((char*)to)[3] = ((const char*)from)[3];
    ((char*)to)[4] = ((const char*)from)[4];
    ((char*)to)[5] = ((const char*)from)[5];
    ((char*)to)[6] = ((const char*)from)[6];
    ((char*)to)[7] = ((const char*)from)[7];
}



inline unsigned int LECanonicalConversion::toLocal (char& to,
						    const void* from)
{
    to = *(char*)from;
    return SIZE_LECAN_CHAR;
}

inline unsigned int LECanonicalConversion::toLocal (unsigned char& to,
						    const void* from)
{
    to = *(unsigned char*)from;
    return SIZE_LECAN_UCHAR;
}

inline unsigned int LECanonicalConversion::toLocal (short& to,
						    const void* from)
{
    if (sizeof(short) != 2) {
	if (((signed char*)from)[0] < 0) {
	    to = -1;
	}else{
	    to = 0;
	}
    }
#if !defined(AIPS_LITTLE_ENDIAN)
    reverse2 (&to, from);
#else
    move2 (((char*)&to)+sizeof(short)-2, from);
#endif
    return SIZE_LECAN_SHORT;
}

inline unsigned int LECanonicalConversion::toLocal (unsigned short& to,
						    const void* from)
{
    if (sizeof(unsigned short) != 2) {
	to = 0;
    }
#if !defined(AIPS_LITTLE_ENDIAN)
    reverse2 (&to, from);
#else
    move2 (((char*)&to)+sizeof(unsigned short)-2, from);
#endif
    return SIZE_LECAN_USHORT;
}

inline unsigned int LECanonicalConversion::toLocal (int& to,
						    const void* from)
{
    if (sizeof(int) != 4) {
	if (((signed char*)from)[0] < 0) {
	    to = -1;
	}else{
	    to = 0;
	}
    }
#if !defined(AIPS_LITTLE_ENDIAN)
    reverse4 (&to, from);
#else
    move4 (((char*)&to)+sizeof(int)-4, from);
#endif
    return SIZE_LECAN_INT;
}

inline unsigned int LECanonicalConversion::toLocal (unsigned int& to,
						    const void* from)
{
    if (sizeof(unsigned int) != 4) {
	to = 0;
    }
#if !defined(AIPS_LITTLE_ENDIAN)
    reverse4 (&to, from);
#else
    move4 (((char*)&to)+sizeof(unsigned int)-4, from);
#endif
    return SIZE_LECAN_UINT;
}

inline unsigned int LECanonicalConversion::toLocal (Int64& to,
						    const void* from)
{
    if (sizeof(Int64) != 8) {
	if (((signed char*)from)[0] < 0) {
	    to = -1;
	}else{
	    to = 0;
	}
    }
#if !defined(AIPS_LITTLE_ENDIAN)
    reverse8 (&to, from);
#else
    move8 (((char*)&to)+sizeof(Int64)-8, from);
#endif
    return SIZE_LECAN_INT64;
}

inline unsigned int LECanonicalConversion::toLocal (uInt64& to,
						    const void* from)
{
    if (sizeof(uInt64) != 8) {
	to = 0;
    }
#if !defined(AIPS_LITTLE_ENDIAN)
    reverse8 (&to, from);
#else
    move8 (((char*)&to)+sizeof(uInt64)-8, from);
#endif
    return SIZE_LECAN_UINT64;
}

inline unsigned int LECanonicalConversion::toLocal (float& to,
						    const void* from)
{
#if !defined(AIPS_LITTLE_ENDIAN)
    reverse4 (((char*)&to)+sizeof(float)-4, from);
#else
    move4 (&to, from);
#endif
    return SIZE_LECAN_FLOAT;
}

inline unsigned int LECanonicalConversion::toLocal (double& to,
						    const void* from)
{
#if !defined(AIPS_LITTLE_ENDIAN)
    reverse8 (((char*)&to)+sizeof(double)-8, from);
#else
    move8 (&to, from);
#endif
    return SIZE_LECAN_DOUBLE;
}


inline unsigned int LECanonicalConversion::fromLocal (void* to,
						      const char& from)
{
    *(char*)to = from;
    return SIZE_LECAN_CHAR;
}
inline unsigned int LECanonicalConversion::fromLocal (void* to,
						     const unsigned char& from)
{
    *(unsigned char*)to = from;
    return SIZE_LECAN_UCHAR;
}

inline unsigned int LECanonicalConversion::fromLocal (void* to,
						      const short& from)
{
#if !defined(AIPS_LITTLE_ENDIAN)
    reverse2 (to, &from);
#else
    move2 (to, ((char*)&from)+sizeof(short)-2);
#endif
    return SIZE_LECAN_SHORT;
}

inline unsigned int LECanonicalConversion::fromLocal (void* to,
						    const unsigned short& from)
{
#if !defined(AIPS_LITTLE_ENDIAN)
    reverse2 (to, &from);
#else
    move2 (to, ((char*)&from)+sizeof(unsigned short)-2);
#endif
    return SIZE_LECAN_USHORT;
}

inline unsigned int LECanonicalConversion::fromLocal (void* to,
						      const int& from)
{
#if !defined(AIPS_LITTLE_ENDIAN)
    reverse4 (to, &from);
#else
    move4 (to, ((char*)&from)+sizeof(int)-4);
#endif
    return SIZE_LECAN_INT;
}

inline unsigned int LECanonicalConversion::fromLocal (void* to,
						      const unsigned int& from)
{
#if !defined(AIPS_LITTLE_ENDIAN)
    reverse4 (to, &from);
#else
    move4 (to, ((char*)&from)+sizeof(unsigned int)-4);
#endif
    return SIZE_LECAN_UINT;
}

inline unsigned int LECanonicalConversion::fromLocal (void* to,
						      const Int64& from)
{
#if !defined(AIPS_LITTLE_ENDIAN)
    reverse8 (to, &from);
#else
    move8 (to, ((char*)&from)+sizeof(Int64)-8);
#endif
    return SIZE_LECAN_INT64;
}

inline unsigned int LECanonicalConversion::fromLocal (void* to,
						      const uInt64& from)
{
#if !defined(AIPS_LITTLE_ENDIAN)
    reverse8 (to, &from);
#else
    move8 (to, ((char*)&from)+sizeof(uInt64)-8);
#endif
    return SIZE_LECAN_UINT64;
}

inline unsigned int LECanonicalConversion::fromLocal (void* to,
						      const float& from)
{
#if !defined(AIPS_LITTLE_ENDIAN)
    reverse4 (to, &from);
#else
    move4 (to, &from);
#endif
    return SIZE_LECAN_FLOAT;
}

inline unsigned int LECanonicalConversion::fromLocal (void* to,
						      const double& from)
{
#if !defined(AIPS_LITTLE_ENDIAN)
    reverse8 (to, &from);
#else
    move8 (to, &from);
#endif
    return SIZE_LECAN_FLOAT;
}


inline unsigned int LECanonicalConversion::toLocal (char*           to,
						    const void* from,
						    unsigned int nr)
{
    return toLocalChar (to, from, nr);
}
inline unsigned int LECanonicalConversion::toLocal (unsigned char*  to,
						    const void* from,
						    unsigned int nr)
{
    return toLocalUChar (to, from, nr);
}
inline unsigned int LECanonicalConversion::toLocal (short*          to,
						    const void* from,
						    unsigned int nr)
{
    return toLocalShort (to, from, nr);
}
inline unsigned int LECanonicalConversion::toLocal (unsigned short* to,
						    const void* from,
						    unsigned int nr)
{
    return toLocalUShort (to, from, nr);
}
inline unsigned int LECanonicalConversion::toLocal (int*            to,
						    const void* from,
						    unsigned int nr)
{
    return toLocalInt (to, from, nr);
}
inline unsigned int LECanonicalConversion::toLocal (unsigned int*   to,
						    const void* from,
						    unsigned int nr)
{
    return toLocalUInt (to, from, nr);
}
inline unsigned int LECanonicalConversion::toLocal (Int64*          to,
						    const void* from,
						    unsigned int nr)
{
    return toLocalInt64 (to, from, nr);
}
inline unsigned int LECanonicalConversion::toLocal (uInt64*         to,
						    const void* from,
						    unsigned int nr)
{
    return toLocalUInt64 (to, from, nr);
}
inline unsigned int LECanonicalConversion::toLocal (float*          to,
						    const void* from,
						    unsigned int nr)
{
    return toLocalFloat (to, from, nr);
}
inline unsigned int LECanonicalConversion::toLocal (double*         to,
						    const void* from,
						    unsigned int nr)
{
    return toLocalDouble (to, from, nr);
}

inline unsigned int LECanonicalConversion::fromLocal (void* to,
						    const char*           from,
						    unsigned int nr)
{
    return fromLocalChar (to, from, nr);
}
inline unsigned int LECanonicalConversion::fromLocal (void* to,
						    const unsigned char*  from,
						    unsigned int nr)
{
    return fromLocalUChar (to, from, nr);
}
inline unsigned int LECanonicalConversion::fromLocal (void* to,
						    const short*          from,
						    unsigned int nr)
{
    return fromLocalShort (to, from, nr);
}
inline unsigned int LECanonicalConversion::fromLocal (void* to,
						    const unsigned short* from,
						    unsigned int nr)
{
    return fromLocalUShort (to, from, nr);
}
inline unsigned int LECanonicalConversion::fromLocal (void* to,
						    const int*            from,
						    unsigned int nr)
{
    return fromLocalInt (to, from, nr);
}
inline unsigned int LECanonicalConversion::fromLocal (void* to,
						    const unsigned int*   from,
						    unsigned int nr)
{
    return fromLocalUInt (to, from, nr);
}
inline unsigned int LECanonicalConversion::fromLocal (void* to,
						    const Int64*          from,
						    unsigned int nr)
{
    return fromLocalInt64 (to, from, nr);
}
inline unsigned int LECanonicalConversion::fromLocal (void* to,
						    const uInt64*         from,
						    unsigned int nr)
{
    return fromLocalUInt64 (to, from, nr);
}
inline unsigned int LECanonicalConversion::fromLocal (void* to,
						    const float*          from,
						    unsigned int nr)
{
    return fromLocalFloat (to, from, nr);
}
inline unsigned int LECanonicalConversion::fromLocal (void* to,
						    const double*         from,
						    unsigned int nr)
{
    return fromLocalDouble (to, from, nr);
}


inline Conversion::ValueFunction* LECanonicalConversion::getToLocal
                                                      (const char*)
{
    return toLocalChar;
}
inline Conversion::ValueFunction* LECanonicalConversion::getToLocal
                                                      (const unsigned char*)
{
    return toLocalUChar;
}
inline Conversion::ValueFunction* LECanonicalConversion::getToLocal
                                                      (const short*)
{
    return toLocalShort;
}
inline Conversion::ValueFunction* LECanonicalConversion::getToLocal
                                                      (const unsigned short*)
{
    return toLocalUShort;
}
inline Conversion::ValueFunction* LECanonicalConversion::getToLocal
                                                      (const int*)
{
    return toLocalInt;
}
inline Conversion::ValueFunction* LECanonicalConversion::getToLocal
                                                      (const unsigned int*)
{
    return toLocalUInt;
}
inline Conversion::ValueFunction* LECanonicalConversion::getToLocal
                                                      (const Int64*)
{
    return toLocalInt64;
}
inline Conversion::ValueFunction* LECanonicalConversion::getToLocal
                                                      (const uInt64*)
{
    return toLocalUInt64;
}
inline Conversion::ValueFunction* LECanonicalConversion::getToLocal
                                                      (const float*)
{
    return toLocalFloat;
}
inline Conversion::ValueFunction* LECanonicalConversion::getToLocal
                                                      (const double*)
{
    return toLocalDouble;
}

inline Conversion::ValueFunction* LECanonicalConversion::getFromLocal
                                                      (const char*)
{
    return fromLocalChar;
}
inline Conversion::ValueFunction* LECanonicalConversion::getFromLocal
                                                      (const unsigned char*)
{
    return fromLocalUChar;
}
inline Conversion::ValueFunction* LECanonicalConversion::getFromLocal
                                                      (const short*)
{
    return fromLocalShort;
}
inline Conversion::ValueFunction* LECanonicalConversion::getFromLocal
                                                      (const unsigned short*)
{
    return fromLocalUShort;
}
inline Conversion::ValueFunction* LECanonicalConversion::getFromLocal
                                                      (const int*)
{
    return fromLocalInt;
}
inline Conversion::ValueFunction* LECanonicalConversion::getFromLocal
                                                      (const unsigned int*)
{
    return fromLocalUInt;
}
inline Conversion::ValueFunction* LECanonicalConversion::getFromLocal
                                                      (const Int64*)
{
    return fromLocalInt64;
}
inline Conversion::ValueFunction* LECanonicalConversion::getFromLocal
                                                      (const uInt64*)
{
    return fromLocalUInt64;
}
inline Conversion::ValueFunction* LECanonicalConversion::getFromLocal
                                                      (const float*)
{
    return fromLocalFloat;
}
inline Conversion::ValueFunction* LECanonicalConversion::getFromLocal
                                                      (const double*)
{
    return fromLocalDouble;
}


inline unsigned int LECanonicalConversion::canonicalSize (const char*)
    {return SIZE_LECAN_CHAR;}
inline unsigned int LECanonicalConversion::canonicalSize (const unsigned char*)
    {return SIZE_LECAN_UCHAR;} 
inline unsigned int LECanonicalConversion::canonicalSize (const short*)
    {return SIZE_LECAN_SHORT;}
inline unsigned int LECanonicalConversion::canonicalSize (const unsigned short*)
    {return SIZE_LECAN_USHORT;}
inline unsigned int LECanonicalConversion::canonicalSize (const int*)
    {return SIZE_LECAN_INT;}
inline unsigned int LECanonicalConversion::canonicalSize (const unsigned int*)
    {return SIZE_LECAN_UINT;}
inline unsigned int LECanonicalConversion::canonicalSize (const Int64*)
    {return SIZE_LECAN_INT64;}
inline unsigned int LECanonicalConversion::canonicalSize (const uInt64*)
    {return SIZE_LECAN_UINT64;}
inline unsigned int LECanonicalConversion::canonicalSize (const float*)
    {return SIZE_LECAN_FLOAT;}
inline unsigned int LECanonicalConversion::canonicalSize (const double*)
    {return SIZE_LECAN_DOUBLE;}
//#//inline unsigned int LECanonicalConversion::canonicalSize (const long double*)
//#//    {return SIZE_LECAN_LDOUBLE;}



#endif
