//# MemoryIO.h: Class for IO in memory
//# Copyright (C) 1996,1999
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

#if !defined(AIPS_MEMORYIO_H)
#define AIPS_MEMORYIO_H

#if defined (_AIX)
#pragma implementation ("MemoryIO.cc")
#endif

//# Includes
#include <aips/aips.h>
#include <aips/IO/ByteIO.h>


// <summary> 
// Class for IO in memory.
// </summary>

// <use visibility=export>

// <reviewed reviewer="Friso Olnon" date="1996/11/06" tests="tByteIO" demos="">
// </reviewed>

// <prerequisite> 
//  <li> <linkto class=ByteIO>ByteIO</linkto>
// </prerequisite>

// <synopsis>
// This class is doing IO in a buffer in memory.
// It is part of the entire IO framework. It can for
// instance be used to store data in canonical format in a
// memory string and obtain it later.
// <p>
// The memory buffer can be dynamic, so it will be expanded when needed.
// This is done by allocating a larger buffer, copy the contents and
// throw the old buffer away.
// <br>
// The memory buffer can also be static to be sure that the pointer to
// the buffer will not change.
// The expand size determines if the memory buffer is static or dynamic.
// An expand size zero indicates a static buffer.
// <p>
// The memory buffer is seekable and readable. It depends on the
// constructor whether it is writable.
// <p>
// There are several ways in which the buffer can be created/passed:
// <ul>
//  <li> Dynamic by passing an initial size and an expand size.
//       However, an expand size zero can be used to assure that no more
//       data is written than fits in the initial buffer (so once the
//       buffer is created it gets static).
//       In this way the buffer is readable and writable.
//  <li> Static by passing a const buffer and its length.
//       In this way the buffer is not writable.
//  <li> Dynamic or static by passing a non-const buffer, its length,
//       and an expand size (zero = static, >0 = dynamic)
//     . The OpenOption indicates whether the buffer will be writable.
//       Only for ByteIO::Old it will not be writable.
//       The OpenOption also determines the initial seek position.
//       Usually it is 0, but for ByteIO::Append it is the end of the buffer.
// </ul>
// The user can obtain a pointer to the buffer to extract the
// stored data from it. The length of the data can also be obtained.
// <p>
// Usually this class will be used in combination with, say, CanonicalIO
// and AipsIO.

// <example>
// <srcblock>
//    // Create dynamic (expandable) memory buffer of length 100.
//    // Use that as the sink of RawIO in AipsIO.
//    MemoryIO membuf (100);
//    RawIO rawio (&membuf);
//    AipsIO stream (&rawio);
//    // Write values.
//    stream << (Int)10;
//    stream << True;
//    // Seek to beginning of buffer and read data in.
//    stream.setpos (0);
//    Int vali;
//    Bool valb;
//    stream >> vali >> valb;
//
//    // One can obtain the buffer and its length and use it later.
//    // (e.g. to write it in a non-AipsIO file).
//    uChar* bufptr = membuf.getBuffer();
//    uLong  length = membuf.length();
//
//    // It can also used to construct another MemoryIO object from it.
//    // The following memory buffer is static and readonly.
//    MemoryIO membuf2 (bufptr, length);
//    membuf2.read (sizeof(vali), vali);
//    membuf2.read (sizeof(valb), valb);
// </srcblock>
// </example>

// <motivation> 
// Make it possible to do IO in a memory buffer.
// The first implementation used strstreambuf from the iostream package.
// However, that did not allow seeking and it was hard to get the length.
// </motivation>


class MemoryIO: public ByteIO
{
public:
    // Construct a dynamic object with the given initial length.
    explicit MemoryIO (uLong initialSize=65536, uLong expandSize=32768);

    // Construct from a buffer with the given length.
    // The buffer is readonly and cannot be expanded.
    MemoryIO (const void* buffer, uLong size);

    // Construct from the given buffer with the given length.
    // The Byte::Option determines how the buffer will be used.
    // The seek pointer is set to the beginning of the buffer, unless
    // told otherwise below.
    // <dl>
    //  <dt> New, NewNoReplace and Scratch
    //  <dd> The buffer is empty and is read/write.
    //  <dt> Old
    //  <dd> The buffer contains <src>size</src> bytes and is readonly.
    //  <dt> Update, Delete
    //  <dd> The buffer contains <src>size</src> bytes and is read/write.
    //  <dt> Append
    //  <dd> The buffer contains <src>size</src> bytes and is read/write.
    //       The seek pointer is set to the end of the buffer.
    // </dl>
    // When the buffer is writable, it will be expanded if needed.
    // This means that <src>buffer</src> does not point to the data
    // anymore. However, when <src>expandSize==0</src>, the buffer
    // cannot be expanded and the pointer is always valid.
    // <br>When canDelete is True, buffer expansion means that the
    // old buffer gets deleted.
    MemoryIO (void* buffer, uLong size, ByteIO::OpenOption,
	      uLong expandSize=0, Bool canDelete=False);

    // Delete the Memory object.
    // The data buffer is not deleted when constructed with the
    // constructor taking a buffer pointer.
    ~MemoryIO();

    // Write the number of bytes.
    // When needed it expands the buffer.
    // An exception is thrown when the buffer is not writable or
    // when buffer expansion fails or is not possible.
    virtual void write (uInt size, const void* buf);

    // Read the number of bytes.
    // An exception is thrown if <src>size</src> bytes are not available.
    virtual void read (uInt size, void* buf);    

    // Reset the position pointer to the given value. It returns the
    // new position.
    // An exception is thrown when seeking before the start of the
    // buffer or when seeking past the end of a readonly buffer.
    // When seeking past the end of a writable buffer, the required
    // amount of bytes is added and initialized to zero.
    virtual Long seek (Long offset, ByteIO::SeekOption = ByteIO::Begin);

    // Clear the buffer; i.e. set the data length and seek pointer to zero.
    void clear();

    // Get the buffer containing the data.
    // <br>The length of the data in the buffer can be obtained using the
    // length() function.
    const uChar* getBuffer() const;

    // Get the length of the data in the buffer.
    virtual Long length() const;

    // Get the allocated length of the buffer.
    uLong allocated() const;
       
    // Get the expand size (0 = not expandable).
    uLong expandSize() const;
       
    // Is the IO stream readable?
    virtual Bool isReadable() const;

    // Is the IO stream writable?
    virtual Bool isWritable() const;

    // Is the IO stream seekable?
    virtual Bool isSeekable() const;

private:
    uChar* itsBuffer;
    uLong  itsAlloc;
    uLong  itsExpandSize;
    uLong  itsUsed;
    uLong  itsPosition;
    Bool   itsReadable;
    Bool   itsWritable;
    Bool   itsCanDelete;

    // Copy constructor, should not be used.
    MemoryIO (const MemoryIO& that);

    // Assignment, should not be used.
    MemoryIO& operator= (const MemoryIO& that);

    // Expand the buffer to at least the given size.
    // The expandSize will be used if that gives a larger new size.
    // In this way the buffer does no get expanded too often.
    // It returns a false status when the buffer cannot be expanded.
    Bool expand (uLong minSize);
};


inline void MemoryIO::clear()
{
    itsUsed = itsPosition = 0;
}
inline const uChar* MemoryIO::getBuffer() const
{
    return itsBuffer;
}
inline uLong MemoryIO::allocated() const
{
    return itsAlloc;
}
inline uLong MemoryIO::expandSize() const
{
    return itsExpandSize;
}


#endif
