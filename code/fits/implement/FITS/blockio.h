//# blockio.h:
//# Copyright (C) 1993,1994,1995,1996
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

# if !defined(AIPS_FITS_BLOCK_IO_H)
# define AIPS_FITS_BLOCK_IO_H

//# Temporary hack until OS dependent flags are developed
//# for the compilation system.

#if !defined(UNIX) && !defined(MSDOS)
#define UNIX
#endif

# include <stdlib.h>
# include <iostream.h>
# if defined(UNIX)
#   include <unistd.h>
# endif
# if defined(MSDOS)

#   include <io.h>
# endif
# include <fcntl.h>
#include <aips/aips.h>

//<category lib=aips module=FITS sect="Blocked I/O">   
//<summary> fixed-length blocked sequentual I/O base class </summary> 
//<synopsis>
// BlockIO is a low level base class that implements fixed-length 
// blocked sequential I/O. Its derived classes, BlockInput and BlockOutput
// are used by the FitsInput and FitsOutput classes. Users will hardly ever
// need to use this class directly.
//</synopsis>
//<todo>
// <li> ifdef kludges until OS dependent flags are developed
//       for the compilation system.
//</todo>

class BlockIO {
    public:
	// error return code
	enum IOErrs { OK, NOSUCHFILE, NOMEM, OPENERR, CLOSEERR, 
		READERR, WRITEERR };
	int err() const { return (int)err_status; }

	//  number of physical blocks read/written
	int blockno() const { return block_no; }

	//  number of logical records read/written
	int recno() const { return rec_no; }

	// name of file associated with I/O stream, if applicable
	const char *fname() const { return filename; }

	// file descriptor associated with I/O stream, if applicable
	int fdes() const { return fd; }
    protected:
	// Construction can be done either from a filename with open options 
	// or from a file descriptor.
	//
	// The remaining arguments are the the logical record size and number
	// of records that make up a physical record followed by the 
	// output stream that is used to write error messages to.
	//<group>
	BlockIO(const char *, int, int, int = 1, ostream & = cout);
	BlockIO(int,               int, int = 1, ostream & = cout);
	virtual ~BlockIO();
	//</group>

	char *filename;		// name of file
	int options;		// options on open statement
	const int recsize;	// size in bytes of a logical record
	const int nrec;		// maximum number of logical records
	const int blocksize;	// size in bytes of physical records
	ostream &ioerr;		// ostream to write error messages to
	IOErrs err_status;	// error number
	int fd;			// file descriptor
	char *buffer;		// the actual data buffer itself
	int block_no;		// number of physical blocks read/written
	int rec_no;		// number of logical records read/written
	int current; 		// offset to current logical record
	// size of record in buffer
	int iosize;  		

	// set the error message and error number for later recovery
	void errmsg(IOErrs, char *);
};

//<summary> fixed-length blocked sequential input base class</summary>
//<prerequisite>
//<list>
//   <item> BlockIO
//</list>
//</prerequisite>

class BlockInput : public BlockIO {
    public:
	// Construction can be done either from a filename or from
	// a file descriptor.
	//
	// The remaining arguments are the the logical record size and number
	// of records that make up a physical record followed by the 
	// output stream that is used to write error messages to.
	//<group>
	BlockInput(const char *, int, int = 1, ostream & = cout);
	BlockInput(int,          int, int = 1, ostream & = cout);
	virtual ~BlockInput();
	//</group>

	// read the next logical record or first
	// skip N logical records and then read the next one.
	// (note it is not possible to skip a record without
	// reading a record). 
	//<note role=caution> these functions return a pointer to an
	// internal record. The user must make sure that
	// after destruction of this class no dangling pointers
	// are left.
	//</note>
	//<group>
	virtual char *read(); 	  
	virtual char *skip(int);  
	//</group>
};

//<summary> fixed-length blocked sequential output base class</summary>
//<prerequisite>
//<list>
//   <item> BlockIO
//</list>
//</prerequisite>             

class BlockOutput : public BlockIO {
    public:
	// Construction can be done either from a filename or from
	// a file descriptor.
	//
	// The remaining arguments are the the logical record size and number
	// of records that make up a physical record followed by the 
	// output stream that is used to write error messages to.
	//<group>
	BlockOutput(const char *, int, int = 1, ostream & = cout);
	BlockOutput(int,          int, int = 1, ostream & = cout);
	virtual ~BlockOutput();
	//</group>

	// write the next logical record. The input must point
	// to a logical record
	virtual int write(char *);
};

# endif

