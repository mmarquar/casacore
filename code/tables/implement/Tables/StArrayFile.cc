//# StArrayFile.cc: Read/write array in external format for a storage manager
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

#include <aips/Tables/StArrayFile.h>
#include <aips/OS/RegularFile.h>
#include <aips/IO/RegularFileIO.h>
#include <aips/IO/RawIO.h>
#include <aips/IO/CanonicalIO.h>
#include <aips/OS/CanonicalConversion.h>
#include <aips/Lattices/IPosition.h>
#include <aips/Mathematics/Complex.h>
#include <aips/Utilities/String.h>
#include <aips/OS/Path.h>
#include <aips/Tables/DataManError.h>
#include <aips/Utilities/Assert.h>


StManArrayFile::StManArrayFile (const String& fname, ByteIO::OpenOption fop,
				uInt version, Bool canonical,
				uInt bufferSize)
: leng_p    (16),
  version_p (version),
  hasPut_p  (False)
{
    // The maximum version is 1.
    if (version_p > 1) {
	version_p = 1;
    }
    //# Open file name as input and/or output; throw exception if it fails.
    file_p = new RegularFileIO (RegularFile(fname), fop, bufferSize);
    AlwaysAssert (file_p != 0, AipsError);
    if (canonical) {
	iofil_p = new CanonicalIO (file_p);
    }else{
	iofil_p = new RawIO (file_p);
    }
    AlwaysAssert (iofil_p != 0, AipsError);
    swput_p = iofil_p->isWritable();
    //# Get the version and length for an existing file.
    //# Otherwise set put-flag.
    if (iofil_p->seek (0, ByteIO::End) > 0) {
	setpos (0);
	get (version_p);
	iofil_p->read (1, &leng_p);
    }else{
	hasPut_p = True;
    }
    if (canonical) {
	sizeChar_p   = CanonicalConversion::canonicalSize ((Char*)0);
	sizeuChar_p  = CanonicalConversion::canonicalSize ((uChar*)0);
	sizeShort_p  = CanonicalConversion::canonicalSize ((Short*)0);
	sizeuShort_p = CanonicalConversion::canonicalSize ((uShort*)0);
	sizeInt_p    = CanonicalConversion::canonicalSize ((Int*)0);
	sizeuInt_p   = CanonicalConversion::canonicalSize ((uInt*)0);
	sizeLong_p   = CanonicalConversion::canonicalSize ((Long*)0);
	sizeuLong_p  = CanonicalConversion::canonicalSize ((uLong*)0);
	sizeFloat_p  = CanonicalConversion::canonicalSize ((Float*)0);
	sizeDouble_p = CanonicalConversion::canonicalSize ((Double*)0);
    }else{
	sizeChar_p   = sizeof(Char);
	sizeuChar_p  = sizeof(uChar);
	sizeShort_p  = sizeof(Short);
	sizeuShort_p = sizeof(uShort);
	sizeInt_p    = sizeof(Int);
	sizeuInt_p   = sizeof(uInt);
	sizeLong_p   = sizeof(Long);
	sizeuLong_p  = sizeof(uLong);
	sizeFloat_p  = sizeof(Float);
	sizeDouble_p = sizeof(Double);
    }
}


//# Close the file.
//# Delete it if required.
StManArrayFile::~StManArrayFile ()
{
    //# Write the version and file length at the beginning.
    flush (False);
    delete iofil_p;
    delete file_p;
}


Bool StManArrayFile::flush (Bool fsync)
{
    if (hasPut_p) {
	setpos (0);
	put (version_p);
	iofil_p->write (1, &leng_p);
	hasPut_p = False;
	return True;
    }
    return False;
}

void StManArrayFile::setpos (uLong pos)
{
    uLong newpos = iofil_p->seek (pos);
    if (newpos != pos) {
	throw (DataManError ("StManArrayFile::setpos failed"));
    }
}


void StManArrayFile::put (uLong fileOff, uInt arrayOff, uInt nr,
			  const Float* data)
{
    setpos (fileOff + arrayOff*sizeFloat_p);
    iofil_p->write (nr, data);
    hasPut_p = True;
}

uInt StManArrayFile::putShape (const IPosition& shape, uLong& offset,
			       const Float*)
    { return putRes (shape, offset, sizeFloat_p); }

void StManArrayFile::get (uLong fileOff, uInt arrayOff, uInt nr, Float* data)
{
    setpos (fileOff + arrayOff*sizeFloat_p);
    iofil_p->read (nr, data);
}

void StManArrayFile::copyArrayFloat (uLong to, uLong from, uInt nr)
    { copyData (to, from, nr*sizeFloat_p); }


//# Put a vector at the given offset.
#define STMANARRAYFILE_PUTGET(T,SIZEDTYPE) \
void StManArrayFile::put (uLong fileOff, uInt arrayOff, uInt nr, \
			  const T* data) \
{ \
    setpos (fileOff + arrayOff*SIZEDTYPE); \
    iofil_p->write (nr, data); \
    hasPut_p = True; \
} \
uInt StManArrayFile::putShape (const IPosition& shape, uLong& offset, \
			       const T*) \
    { return putRes (shape, offset, SIZEDTYPE); } \
void StManArrayFile::get (uLong fileOff, uInt arrayOff, uInt nr, T* data) \
{ \
    setpos (fileOff + arrayOff*SIZEDTYPE); \
    iofil_p->read (nr, data); \
} \
void StManArrayFile::aips_name2(copyArray,T) (uLong to, uLong from, uInt nr)\
    { copyData (to, from, nr*SIZEDTYPE); }

STMANARRAYFILE_PUTGET(Char, sizeChar_p)
STMANARRAYFILE_PUTGET(uChar, sizeuChar_p)
STMANARRAYFILE_PUTGET(Short, sizeShort_p)
STMANARRAYFILE_PUTGET(uShort, sizeuShort_p)
STMANARRAYFILE_PUTGET(Int, sizeInt_p)
STMANARRAYFILE_PUTGET(uInt, sizeuInt_p)
STMANARRAYFILE_PUTGET(Long, sizeLong_p)
STMANARRAYFILE_PUTGET(uLong, sizeuLong_p)
//#//STMANARRAYFILE_PUTGET(Float, sizeFloat_p)
STMANARRAYFILE_PUTGET(Double, sizeDouble_p)
//#//STMANARRAYFILE_PUTGET(long double, sizeLDouble_p)


//# Handle it for Bool.
void StManArrayFile::put (uLong fileOff, uInt arrayOff, uInt nr,
			  const Bool* data)
{
    //# Bools are stored as bits, thus a bit more complex.
    uInt start  = arrayOff / 8;
    uInt stbit  = arrayOff - 8 * start;
    uInt end    = (arrayOff + nr) / 8;
    uInt endbit = arrayOff + nr - 8 * end;
    if (endbit != 0) {
	end++;
    }
    //# Allocate a buffer of the required length.
    //# Read first and/or last byte when partially accessed.
    uChar* buf = new uChar[end - start];
    if (endbit != 0) {
	setpos (fileOff + end - 1);
	iofil_p->read (1, buf+end-start-1);
    }
    if (stbit != 0  &&  start < end-1) {
	setpos (fileOff + start);
	iofil_p->read (1, buf);
    }
    Conversion::boolToBit (buf, data, stbit, nr);
    setpos (fileOff + start);
    iofil_p->write (end - start, buf);
    hasPut_p = True;
    delete [] buf;
}
uInt StManArrayFile::putShape (const IPosition& shape, uLong& offset,
			       const Bool*)
    { return putRes (shape, offset, 0.125); }
void StManArrayFile::get (uLong fileOff, uInt arrayOff, uInt nr, Bool* data)
{
    //# Bools are stored as bits, thus a bit more complex.
    uInt start  = arrayOff / 8;
    uInt stbit  = arrayOff - 8 * start;
    uInt end    = (arrayOff + nr) / 8;
    uInt endbit = arrayOff + nr - 8 * end;
    if (endbit != 0) {
	end++;
    }
    //# Allocate a buffer of the required length.
    uChar* buf = new uChar[end - start];
    setpos (fileOff + start);
    iofil_p->read (end - start, buf);
    Conversion::bitToBool (data, buf, stbit, nr);
    delete [] buf;
}
void StManArrayFile::copyArrayBool (uLong to, uLong from, uInt nr)
    { copyData (to, from, (nr+7)/8); }


//# Handle it for Complex and String.
//# A Complex consists of 2 float values.
//# For a string its file offset gets stored (as a uInt), while the
//# string itself will be put at the end of the file.
uInt StManArrayFile::putShape (const IPosition& shape, uLong& offset,
			       const Complex*)
    { return putRes (shape, offset, 2*sizeFloat_p); }
uInt StManArrayFile::putShape (const IPosition& shape, uLong& offset,
			       const DComplex*)
    { return putRes (shape, offset, 2*sizeDouble_p); }
uInt StManArrayFile::putShape (const IPosition& shape, uLong& offset,
			       const String*)
    { return putRes (shape, offset, sizeuInt_p); }

//# Put a complex vector at the given file offset.
void StManArrayFile::put (uLong fileOff, uInt arrayOff, uInt nr,
			  const Complex* data)
{
    setpos (fileOff + arrayOff*2*sizeFloat_p);
    iofil_p->write (2*nr, (const Float*)data);
    hasPut_p = True;
}
void StManArrayFile::put (uLong fileOff, uInt arrayOff, uInt nr,
			  const DComplex* data)
{
    setpos (fileOff + arrayOff*2*sizeDouble_p);
    iofil_p->write (2*nr, (const Double*)data);
    hasPut_p = True;
}

//# Put a string at the given file offset.
void StManArrayFile::put (uLong fileOff, uInt arrayOff, uInt nr,
			  const String* data)
{
    //# Get file offset for string offset array.
    //# Allocate a buffer to hold 4096 string offsets.
    uInt offs = fileOff + arrayOff*sizeuInt_p;
    uInt buf[4096];
    uInt i, n;
    while (nr > 0) {
	n = (nr < 4096  ?  nr : 4096);
	setpos (leng_p);                            // position at end of file
	for (i=0; i<n; i++) {
	    buf[i] = leng_p;
	    leng_p += put (data->length());         // write string length
	    leng_p += iofil_p->write (data->length(), data->chars());
	    data++;
	}
	//# Write the offsets.
	setpos (offs);
	offs += iofil_p->write (n, buf);
	hasPut_p = True;
	nr   -= n;
    }
}


//# Get a complex vector at the given file offset.
void StManArrayFile::get (uLong fileOff, uInt arrayOff, uInt nr,
			  Complex* data)
{
    setpos (fileOff + arrayOff*2*sizeFloat_p);
    iofil_p->read (2*nr, (Float*)data);
}
void StManArrayFile::get (uLong fileOff, uInt arrayOff, uInt nr,
			  DComplex* data)
{
    setpos (fileOff + arrayOff*2*sizeDouble_p);
    iofil_p->read (2*nr, (Double*)data);
}

//# Get a string at the given file offset.
void StManArrayFile::get (uLong fileOff, uInt arrayOff, uInt nr,
			  String* data)
{
    //# Get file offset for string offset array.
    //# Allocate a buffer to hold 4096 string offsets.
    uLong offs = fileOff + arrayOff*sizeuInt_p;
    uInt buf[4096];
    uInt i, n, l;
    while (nr > 0) {
	n = (nr < 4096  ?  nr : 4096);
	setpos (offs);
	offs += iofil_p->read (n, buf);
	for (i=0; i<n; i++) {
	    setpos (buf[i]);
	    get (l);                              // read string length
	    data->alloc (l);                      // resize string
	    iofil_p->read (data->length(), (char*)(data->chars()));
	    //# Do some kind of dirty trick to set the trailing zero.
	    //# The cast is needed because String::chars() returns const char*.
	    ((char*)(data->chars()))[l] = '\0';
	    data++;
	}
	nr   -= n;
    }
}


void StManArrayFile::copyArrayComplex (uLong to, uLong from, uInt nr)
    { copyData (to, from, nr*2*sizeFloat_p); }
void StManArrayFile::copyArrayDComplex (uLong to, uLong from, uInt nr)
    { copyData (to, from, nr*2*sizeDouble_p); }
void StManArrayFile::copyArrayString (uLong to, uLong from, uInt nr)
{
    String data[4096];
    uInt ndone = 0;
    for (uInt n=0; nr>0; nr-=n) {
	n = (nr < 4096  ?  nr : 4096);
	get (from, ndone, n, data);
	put (to, ndone, n, data);
	ndone += n;
    }
}

//# Copy the data of the given length from one file offset to another.
void StManArrayFile::copyData (uLong to, uLong from, uInt length)
{
    uChar buffer[32768];
    for (uInt n=0; length>0; length-=n) {
	n = (length < 32768  ?  length : 32768);
	setpos (from);
	from += iofil_p->read (n, buffer);
	setpos (to);
	to += iofil_p->write  (n, buffer);
	hasPut_p = True;
    }
}

//# Write shape and reserve file space for the array by
//# increasing the length.
//# Take care it is at 8 byte boundary.
//# Write something in the last byte to make sure the file is extended.
uInt StManArrayFile::putRes (const IPosition& shape, uLong& offset,
			     float lenElem)
{
    leng_p = 8 * ((leng_p+7) / 8);
    offset = leng_p;
    uInt n = 0;
    setpos (leng_p);
    // Put reference count in higher versions only.
    if (version_p > 0) {
	n += put (uInt(1));
    }
    n += put (shape.nelements());
    for (uInt i=0; i<shape.nelements(); i++) {
	n += put (shape(i));
    }
    // Add length of shape and of entire array to file length.
    // Take care of rounding (needed for Bool case).
    leng_p += n;
    leng_p += Int (shape.product() * lenElem + 0.95);
    setpos (leng_p - 1);
    Char c = 0;
    iofil_p->write (1, &c);
    hasPut_p = True;
    return n;
}

//# Get the shape at the given file offset
//# and returns its length in the file.
uInt StManArrayFile::getShape (uLong fileOff, IPosition& shape)
{
    setpos (fileOff);
    uInt n=0;
    if (version_p > 0) {
	uInt refCount;
	n = get (refCount);
    }
    uInt nr;
    n += get (nr);
    shape.resize (nr);
    for (uInt i=0; i<nr; i++) {
	n += get (shape(i));
    }
    return n;
}

//# Update the reference count.
uInt StManArrayFile::getRefCount (uLong offset)
{
    if (version_p == 0) {
	return 1;
    }
    setpos (offset);
    uInt refCount;
    get (refCount);
    return refCount;
}

//# Update the reference count.
void StManArrayFile::putRefCount (uInt refCount, uLong offset)
{
    // For version 0 only a dummy put (i.e. value 1) is allowed.
    if (version_p == 0) {
	AlwaysAssert (refCount==1, AipsError);
    }else{
	setpos (offset);
	put (refCount);
    }
}
