//# MMapIO.h: Memory-mapped IO on a file
//#
//#  Copyright (C) 2009
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
//#  $Id$

#ifndef CASA_MMAPIO_H
#define CASA_MMAPIO_H

//# Includes
#include <casa/IO/LargeFiledesIO.h>
#include <casa/OS/RegularFile.h>

namespace casa
{

// <summary>
// Memory-mapped IO on a file.
// </summary>

// <synopsis>
// Memory-mapped IO lets the OS take care of caching file segments.
// This is particularly useful for the Tiled Storage Manager which keeps a
// cache of tiles. When using memory-mapped IO it does not need to do that
// anymore.
//
// On 32-bit systems its use is limited because for large files the 4 GB
// memory space is insufficient. However, for 64-bit systems the memory
// space is large enough to make use of it.
//
// In the general case there is direct access to the mapped file space.
// The read and write methods copies the data into/from a buffer.
// However, to avoid the copying it is possible to get a direct pointer
// to the mapped data. This should be used with care, because writing to
// it will cause a segmentation if the file is readonly. If the file is
// writable, writing into the mapped data segment means changing the file
// contents.
// </synopsis>

class MMapIO: public LargeFiledesIO
{
public:
  // Open the given file and map it entirely into memory with read access.
  // The map has also write access when the file is opened for write.
  explicit MMapIO (const RegularFile& regularFile,
                   ByteIO::OpenOption = ByteIO::Old);

  // Destructor.
  // It will flush and unmap the file.
  ~MMapIO();

  // Flush changed mapped data to the file.
  // Nothing is done if the file is readonly.
  void flush();

  // Write the number of bytes from the seek position on.
  // The file will be extended and remapped when writing beyond end-of-file.
  // In that case possible pointers obtained using <src>getXXPointer</src>
  // are not valid anymore.
  virtual void write (uInt size, const void* buf);

  // Read <src>size</src> bytes from the File. Returns the number of bytes
  // actually read. Will throw an exception (AipsError) if the requested
  // number of bytes could not be read unless throwException is set to
  // False. Will always throw an exception if the file is not readable or
  // the system call returns an undocumented value.
  virtual Int read (uInt size, void* buf, Bool throwException=True);

  // Get a read or write pointer to the given position in the mapped file.
  // An exception is thrown if beyond end-of-file or it not writable.
  // These functions should be used with care. If the pointer is used to
  // access data beyond the file size, a segmentation fault will occur.
  // So it means that the write pointer can only be used to update the file,
  // not to extend it. The <src>seek</src> and <src>write</src> functions
  // should be used to extend a file.
  // <group>
  const void* getReadPointer (Int64 offset) const;
  void* getWritePointer (Int64 offset);
  // </group>

  // Get the file size.
  Int64 getFileSize() const
    { return itsFileSize; }

protected:
  // Reset the position pointer to the given value. It returns the
  // new position.
  virtual Int64 doSeek (Int64 offset, ByteIO::SeekOption);

private:
  // Map the entire file.
  void mapFile();

  // Unmap the file.
  void unmapFile();

  // Forbid copy constructor and assignment
  // <group>
  MMapIO (const MMapIO&);
  MMapIO& operator= (const MMapIO&);
  // </group>

  Int64  itsFileSize;       //# File size
  Int64  itsPosition;       //# Current seek position
  char*  itsPtr;            //# Pointer to memory map
  int    itsFD;
  Bool   itsIsWritable;
  String itsFileName;
};

} // end namespace

#endif
