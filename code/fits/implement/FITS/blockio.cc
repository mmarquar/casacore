//# blockio.cc:
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

# include <aips/FITS/blockio.h>
# include <string.h>

void BlockIO::errmsg(IOErrs e, char *s) { 
	ioerr << "BlockIO:  ";
	if (filename == 0 || *filename == '\0')
		ioerr << "File Descriptor " << fd;
	else
		ioerr << "File " << filename;
	ioerr << " Physical record " << block_no
	      << " logical record " << rec_no << " --\n\t" << s << "\n";
	err_status = e;
}

BlockIO::BlockIO(const char *f, int o, int r, int n, ostream &s) :
	filename(0), options(o), recsize(r), nrec(n), blocksize(r * n),
        ioerr(s), err_status(OK), fd(-1), buffer(0), block_no(0), 
        rec_no(0), current(0), iosize(0) {
	if (f == 0 || (*f == '\0')) {
		errmsg(NOSUCHFILE,"No filename was specified");
		return;
	}
	if ((filename = new char [strlen(f) + 1]) == 0) {
		errmsg(NOMEM,"Cannot allocate memory");
		return;
	}
	if ((buffer = new char [blocksize]) == 0) {
		errmsg(NOMEM,"Cannot allocate memory");
		delete [] filename;
		filename = 0;
		return;
	}
	strcpy(filename,f);
# if defined(MSDOS)
	options = options | O_BINARY;
# endif
	if (options & O_CREAT)
		fd = open(filename,options,0644);
	else
		fd = open(filename,options);
	if (fd == -1) {
		errmsg(OPENERR,"Could not open file");
		delete [] filename;
		delete [] buffer;
		filename = 0;
		buffer = 0;
	}
}

BlockIO::BlockIO(int f, int r, int n, ostream &s) : 
	filename(""), options(0), recsize(r), nrec(n), blocksize(n * r), 
	ioerr(s), err_status(OK), fd(f), block_no(0), rec_no(0), current(0), 
	iosize(0) {
	if ((buffer = new char [blocksize]) == 0) {
		errmsg(NOMEM,"Cannot allocate memory");
		return;
	}
}

BlockIO::~BlockIO() {
	if (filename != 0 && strlen(filename) > 0) {
		// force a sync before closing
		// at this point, we probably don't care about an error in fsync
		fsync(fd);
		if (close(fd) == -1)
			errmsg(CLOSEERR,"Error closing file");
		delete [] filename;
	}
	delete [] buffer;
}

BlockInput::~BlockInput() {
}

char *BlockInput::read() {
    current += recsize;
    int nreadlast = 0;
    if (current >= iosize) {
	int ntoread = blocksize;
	int nreadlast;
	iosize = 0;
	// Read in a loop because, e.g., stdin or the network can break things
	// up into differently sized records. This might not quite be the right
	// thing to do with tapes, but the code doesn't work properly on tapes
	// yet in any event.
	do {
	    nreadlast = ::read(fd, buffer, (unsigned)ntoread);
	    if (nreadlast > 0) {
		ntoread -= nreadlast;
		iosize += nreadlast;
	    }
	} while (ntoread > 0 && nreadlast > 0);
	if (iosize == 0)
	    return 0;
	block_no++;
	if (nreadlast < 0) {
	    if (iosize > 0) {
		iosize -= (iosize % recsize);
	    }
	    errmsg(READERR,"Error reading record");
	    iosize = 0;
	} else if (iosize % recsize != 0) {
	    errmsg(READERR,"Wrong length record");
	    iosize -= (iosize % recsize);
	} else {
	    err_status = OK;
	}
	current = 0;
    }
    rec_no++;
    return &buffer[current];
}

char *BlockInput::skip(int n) {
	while (n--)
		read();
	return read();
}

BlockOutput::~BlockOutput() {
	if (current > 0) {
		iosize = ::write(fd,buffer,current);
		block_no++;
		if (iosize != current)
			errmsg(WRITEERR,"Error writing record");
		else
			err_status = OK;
		current = 0;
	}
}

int BlockOutput::write(char *addr) {
	memcpy(&buffer[current],addr,recsize);
	rec_no++;
	current += recsize;
	if (current >= blocksize) {
		iosize = ::write(fd,buffer,blocksize);
		block_no++;
		if (iosize != blocksize)
			errmsg(WRITEERR,"Error writing record");
		else
			err_status = OK;
		current = 0;
	}	
	return (int)err_status;
}

BlockInput::BlockInput(const char *f, int r, int n, ostream &s) :
	BlockIO(f,O_RDONLY,r,n,s) { }

BlockInput::BlockInput(int f, int r, int n, ostream &s) :
	BlockIO(f,r,n,s) { }

BlockOutput::BlockOutput(const char *f, int r, int n, ostream &s) :
	BlockIO(f,O_WRONLY|O_CREAT|O_TRUNC,r,n,s) { }

BlockOutput::BlockOutput(int f, int r, int n, ostream &s) :
	BlockIO(f,r,n,s) { }
