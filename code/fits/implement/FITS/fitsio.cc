//# fitsio.cc:
//# Copyright (C) 1993,1994,1995,1996,1997
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

# include <aips/FITS/hdu.h>
# include <aips/FITS/fitsio.h>
# include <string.h>

FitsIO::~FitsIO() {
}

FitsInput::~FitsInput() {
	delete &fin;
}

FitsDiskInput::~FitsDiskInput() {
}

FitsDiskOutput::~FitsDiskOutput() {
}

FitsStdInput::~FitsStdInput() {
}

FitsStdOutput::~FitsStdOutput() {
}

FitsTape9Input::~FitsTape9Input() {
}

FitsTape9Output::~FitsTape9Output() {
}

void FitsInput::errmsg(FitsErrs e, char *s) {
	errs << "FitsInput error:  ";
	if (fin.fname() == 0 || *fin.fname() == '\0') 
		errs << "File Descriptor " << fin.fdes();
	else
		errs << "File " << fin.fname();
	errs << " Physical record " << fin.blockno()
	      << " logical record " << fin.recno() << " --\n\t" << s << "\n";
	err_status = e;
}

char *FitsDiskInput::skip(int n) { // skip n logical records and read
	if (n == 0)
	    return read();
	while (n) {
	    current += recsize;
	    if (current >= iosize)
		break;
	    --n;
	    ++rec_no;
	}
	if (n == 0)
	    return read();
	int phy_rec = n / nrec; // find # physical records to skip
	if (phy_rec > 0) {
	    if (lseek(fd,(long)(phy_rec * blocksize),SEEK_CUR) == -1) {
		errmsg(READERR,"Error performing lseek");
                iosize = 0;
	    }	    
	    n -= phy_rec * nrec;
	    rec_no += phy_rec * nrec;
	    block_no += phy_rec;
	}
	while (n--)
	    read();
        return read();
}

BlockInput &FitsInput::make_input(const char *n, const FITS::FitsDevice &d, 
	int b, ostream &e) 
{
    BlockInput *bptr = 0;

	switch (d) {
	    case FITS::Disk:
		bptr =  new FitsDiskInput(n,FitsRecSize,b,e);
		break;
	    case FITS::Tape9: 
		bptr = new FitsTape9Input(n,FitsRecSize,b,e);
		break;
	    case FITS::Std: 
		bptr = new FitsStdInput(FitsRecSize,e);
		break;
	}
    // Dereferences a null pointer if "d" was not caught in the above switch.
        return *bptr;
}

void FitsOutput::errmsg(FitsErrs e, char *s) {
	errs << "FitsOutput error:  ";
	if (fout.fname() == 0 || *fout.fname() == '\0') 
		errs << "File Descriptor " << fout.fdes();
	else
		errs << "File " << fout.fname();
	errs << " Physical record " << fout.blockno()
	      << " logical record " << fout.recno() << " --\n\t" << s << "\n";
	err_status = e;
}

BlockOutput &FitsOutput::make_output(const char *n, const FITS::FitsDevice &d, 
	int b, ostream &e) 
{
    BlockOutput *bptr = 0;
	switch (d) {
	    case FITS::Disk:
		bptr =  new FitsDiskOutput(n,FitsRecSize,b,e);
		break;
	    case FITS::Tape9: 
		bptr =  new FitsTape9Output(n,FitsRecSize,b,e);
		break;
	    case FITS::Std: 
		bptr =  new FitsStdOutput(FitsRecSize,e);
		break;
	}

    // Dereferences a null pointer if "d" was not caught in the above
    // switch.
    return *bptr;
}

FitsOutput::FitsOutput(const char *n, const FITS::FitsDevice &d, int b, 
	ostream &e) : 
	FitsIO(e), fout(make_output(n,d,b,e)) {
	if (fout.err()) {
	    rec_type = FITS::EndOfFile;
	    errmsg(IOERR,"Error constructing output");
	    return;
	}
	curr = new char [FitsRecSize];
	if (!curr) {
	    rec_type = FITS::EndOfFile;
	    errmsg(MEMERR,"Could not allocate storage for output buffer.");
	}
}

FitsOutput::FitsOutput(ostream &e) : FitsIO(e), 
	fout(*(BlockOutput *)(new FitsStdOutput(FitsRecSize,e))) {
	if (fout.err()) {
	    rec_type = FITS::EndOfFile;
	    errmsg(IOERR,"Error constructing output");
	    return;
	}
	curr = new char [FitsRecSize];
	if (!curr) {
	    rec_type = FITS::EndOfFile;
	    errmsg(MEMERR,"Could not allocate storage for output buffer.");
	}
}

FitsOutput::~FitsOutput() {
	if (hdu_inprogress())
	    errmsg(BADOPER,"ERROR! Output closed before HDU was complete."); 
	delete &fout;
	delete [] curr;
}

void FitsInput::init() {
	if (fin.err())
		errmsg(IOERR,"Error constructing input");
	else {
	    curr = fin.read();
	    got_rec = True;
	    if (!curr) {
	        errmsg(EMPTYFILE,"This is an empty file.");
	        rec_type = FITS::EndOfFile;
	        return;
	    }
	    if (fin.err()) {
	        errmsg(IOERR,"Error reading first record.");
	        rec_type = FITS::BadBeginningRecord;
	        return;
	    }
	    kc.parse(curr,kw,0,errs,True);
	    HeaderDataUnit::HDUErrs n;
	    if (!HeaderDataUnit::determine_type(kw,hdu_type,data_type,errs,n)) {
	        errmsg(BADBEGIN,"Unrecognizable record at the beginning.");
	        rec_type = FITS::BadBeginningRecord;
	        return;
	    }
	    if (!(hdu_type == FITS::PrimaryArrayHDU || 
		  hdu_type == FITS::PrimaryGroupHDU)) {
	        errmsg(NOPRIMARY,"Missing primary header-data unit.");
	    } else {
                isaprimary = True;
	        if (kw(FITS::SIMPLE)->asBool() == True) 
		    valid_fits = True;
		else
		    errs << "\n>>> WARNING!  Value of keyword SIMPLE is \
FALSE;\n              this file may not be a valid FITS file.\n\n";
	        if (kw(FITS::EXTEND))
		    if (kw.curr()->asBool() == True)
		        extend = True;
	    }
	    rec_type = FITS::HDURecord;
	}
}

char * FitsInput::read_sp() {
	err_status = OK;
	if (rec_type == FITS::BadBeginningRecord) {
		if (got_rec) {
			got_rec = False;
			return curr;
		}
		curr = fin.read();
		if (!curr) {
	    		rec_type = FITS::EndOfFile;
			got_rec = True;
	    		return 0;
		}
		if (fin.err()) {
	    		errmsg(IOERR,"Error reading record.");
	    		return curr;
		}
		kw.delete_all();
		kc.parse(curr,kw,0,errs,True);
	        HeaderDataUnit::HDUErrs n;
	        if (!HeaderDataUnit::determine_type(kw,
			hdu_type,data_type,errs,n))
	            return curr;
	        if (!(hdu_type == FITS::PrimaryArrayHDU || 
		      hdu_type == FITS::PrimaryGroupHDU)) {
	            errmsg(NOPRIMARY,"Missing primary header-data unit.");
	        } else {
	            if (kw(FITS::SIMPLE)->asBool() == True) 
		        valid_fits = True;
		    else
		    errs << "\n>>> WARNING!  Value of keyword SIMPLE is \
FALSE;\n              this file may not be a valid FITS file.\n\n";
	            if (kw(FITS::EXTEND))
		        if (kw.curr()->asBool() == True)
		            extend = True;
		}
	        rec_type = FITS::HDURecord;
		got_rec = True;
		return 0;
	} else if (rec_type == FITS::UnrecognizableRecord) {
		if (got_rec) {
			got_rec = False;
			return curr;
		}
		curr = fin.read();
		if (!curr) {
	    		rec_type = FITS::EndOfFile;
			got_rec = True;
	    		return 0;
		}
		if (fin.err()) {
	    		errmsg(IOERR,"Error reading record.");
	    		return curr;
		}
		kw.delete_all();
		kc.parse(curr,kw,0,errs,True);
	        HeaderDataUnit::HDUErrs n;
	        if (!HeaderDataUnit::determine_type(kw,
			hdu_type,data_type,errs,n))
	            return curr;
	        rec_type = FITS::HDURecord;
		got_rec = True;
		return 0;
	} else if (rec_type == FITS::SpecialRecord) {
		if (got_rec) {
			got_rec = False;
			return curr;
		}
		curr = fin.read();
		if (!curr) {
	    		rec_type = FITS::EndOfFile;
			got_rec = True;
			err_status = OK;
	    		return 0;
		}
		if (fin.err()) {
	    		errmsg(IOERR,"Error reading record.");
	    		return curr;
		}
		err_status = OK;
		return curr;
	}
	return 0;
}

void FitsInput::read_header_rec() {
	curr = fin.read();
	got_rec = True;
	if (!curr) {
	    rec_type = FITS::EndOfFile;
	    return;
	}
	if (fin.err()) {
	    errmsg(IOERR,"Error reading first record of new header.");
	    rec_type = FITS::UnrecognizableRecord;
	    return;
	}
	kw.delete_all();
	kc.parse(curr,kw,0,errs,True);
	HeaderDataUnit::HDUErrs n;
	if (!HeaderDataUnit::determine_type(kw,hdu_type,data_type,errs,n)) {
	    rec_type = FITS::SpecialRecord;
	    return;
	}
	if (hdu_type == FITS::PrimaryArrayHDU || hdu_type == FITS::PrimaryGroupHDU) 
	    errmsg(BADPRIMARY,"Misplaced primary header-data unit.");
	rec_type = FITS::HDURecord;
        header_done = False;
}

int FitsInput::skip_hdu() { //Skip an entire header-data unit
	err_status = OK;
	if ((rec_type != FITS::HDURecord) || header_done) {
		errmsg(BADOPER,"Illegal operation on FITS input");
		return (int)err_status;
	}
	int i;
	FitsKeyword *x, *y;
	kw.first(); y = kw.next(); // set the list pointer
        for (; ; kc.parse(curr,kw,0,errs,False)) {
	    // The worst error is if there is no END keyword.
	    kw.last(); x = kw.prev(); // do backwards search for END 
	    if (x->kw().name() == FITS::END)
	        break;
	    while (x != y) {
		x = kw.prev();
		if (x->kw().name() == FITS::END)
		    break;
	    }
	    if (x->kw().name() == FITS::END)
		break;
	    kw.last(); y = kw.prev(); // reset the list pointer
	    kw.last(); // return list iterator to last position
	    curr = fin.read(); // read the next record
	    if (!curr) {
	        errmsg(BADEOF,"Unexpected end of file.");
	        rec_type = FITS::EndOfFile;
	        return (int)err_status;
	    }
	    if (fin.err()) {
	        errmsg(IOERR,"Error reading header record.");
	        rec_type = FITS::UnrecognizableRecord;
	        return (int)err_status;
	    }
	    // This attempts to deal with the problem of no END keyword
            // by searching for non-text data in the first 8 bytes.
	    for (i = 0; i < 8; ++i)
		if (!FITS::isa_text(curr[i]))
		    break;
	    if (i < 8) {
		errmsg(MISSKEY,"Missing END keyword.  Non-text data \
found in name field.\n\tEnd of keywords assumed.");
		break;
	    }
	}
	if (!extend) {
	    if (kw(FITS::EXTEND))
	        if (kw.curr()->asBool() == True)
		    extend = True;
	}
	HeaderDataUnit::HDUErrs nerr;
	Int nd;
	if (!HeaderDataUnit::compute_size(kw,data_size,nd,
		hdu_type,data_type,errs,nerr)) {
		item_size = 0;
		data_type = FITS::NOVALUE;
		data_size = 0;
	    	errmsg(BADSIZE,"Error computing size of data.");
	        rec_type = FITS::UnrecognizableRecord;
	        return (int)err_status;
	}
	item_size = FITS::fitssize(data_type);
	int n = data_size / FitsRecSize; // compute number of records to skip
	if (data_size % FitsRecSize) 
		n++;
	if (n) {
		curr = fin.skip(n - 1);
		if (!curr) {
		    errmsg(BADEOF,"Unexpected end of file.");
	    	    rec_type = FITS::EndOfFile;
	    	    return (int)err_status;
		}
		if (fin.err()) {
	    	    errmsg(IOERR,"Error skipping data records.");
	    	    rec_type = FITS::UnrecognizableRecord;
	    	    return (int)err_status;
		}
	}
	read_header_rec();
	if (err()) 
		return (int)err_status;	
	return 0;
}

int FitsInput::process_header(FITS::HDUType t, FitsKeywordList &uk) {
	err_status = OK;
	item_size = 0;
	data_type = FITS::NOVALUE;
	data_size = 0;
	bytepos = 0;
	curr_size = 0;
	if ((rec_type != FITS::HDURecord || hdu_type != t) || header_done) {
		errmsg(BADOPER,"Illegal operation on FITS input");
		return -1;
	}
	uk.delete_all();
	uk = kw;
	int cnt = 0;
	int i;
	FitsKeyword *x, *y;
	uk.first(); y = uk.next(); // set the list pointer
        for (; ; kc.parse(curr,uk,cnt,errs,True)) {
	    // The worst error is if there is no END keyword.
	    uk.last(); x = uk.prev(); // do backwards search for END 
	    if (x->kw().name() == FITS::END)
	        break;
	    while (x != y) {
		x = uk.prev();
		if (x->kw().name() == FITS::END)
		    break;
	    }
	    if (x->kw().name() == FITS::END)
		break;
	    uk.last(); y = uk.prev(); // reset the list pointer
	    uk.last(); // return list iterator to last position
	    curr = fin.read(); // read the next record
	    if (!curr) {
	        errmsg(BADEOF,"Unexpected end of file.");
	        rec_type = FITS::EndOfFile;
	        return -1;
	    }
	    if (fin.err()) {
	        errmsg(IOERR,"Error reading header record.");
	        rec_type = FITS::UnrecognizableRecord;
	        return -1;
	    }
            ++cnt;
	    // This attempts to deal with the problem of no END keyword
            // by searching for non-text data in the first 8 bytes.
	    for (i = 0; i < 8; ++i)
		if (!FITS::isa_text(curr[i]))
		    break;
	    if (i < 8) {
		errmsg(MISSKEY,"Missing END keyword.  Non-text data \
found in name field.\n\tEnd of keywords assumed.");
		break;
	    }
	}
	if (!extend) {
	    if (uk(FITS::EXTEND))
	        if (uk.curr()->asBool() == True)
		    extend = True;
	}
	HeaderDataUnit::HDUErrs n;
	Int nd;
	if (!HeaderDataUnit::compute_size(uk,data_size,nd,
		hdu_type,data_type,errs,n)) {
	    	errmsg(BADSIZE,"Error computing size of data.");
	        rec_type = FITS::UnrecognizableRecord;
	        return -1;
	}
	item_size = FITS::fitssize(data_type);
	curr_size = data_size;
        header_done = True;
	if (data_size > 0) {
		curr = fin.read();
		got_rec = True;
		if (!curr) {
		    hdu_type = FITS::NotAHDU;
		    item_size = 0;
		    data_type = FITS::NOVALUE;
		    data_size = 0;
		    curr_size = 0;
		    errmsg(BADEOF,"Unexpected end of file.");
		    rec_type = FITS::EndOfFile;
		    return -1;
		}
		if (fin.err()) {
		    hdu_type = FITS::NotAHDU;
		    item_size = 0;
		    data_type = FITS::NOVALUE;
		    data_size = 0;
		    curr_size = 0;
		    errmsg(IOERR,"Error reading first data record.");
		    rec_type = FITS::UnrecognizableRecord;
		    return -1;
		}
	} else
		read_header_rec();
	return 0;
}

int FitsInput::read_all(FITS::HDUType t, char *addr) { // read all data into addr
	if (curr_size <= 0 || curr_size != data_size ||
	    rec_type != FITS::HDURecord || t != hdu_type || (!header_done)) {
		errmsg(BADOPER,"Illegal operation on FITS input");
		return 0;
	}
	while (curr_size >= FitsRecSize) {
	    memcpy(addr,curr,FitsRecSize);
	    addr += FitsRecSize;
	    curr_size -= FitsRecSize;
	    curr = fin.read();
	    if (!curr) {
		    errmsg(BADEOF,"Unexpected end of file.");
		    rec_type = FITS::EndOfFile;
		    return 0;
	    }
	    if (fin.err()) {
		    errmsg(IOERR,"Error reading first data record.");
		    rec_type = FITS::UnrecognizableRecord;
		    return 0;
	    }
	}
	if (curr_size) {
	    memcpy(addr,curr,curr_size);
	    curr_size = 0;
	}
	read_header_rec();
	return data_size;
}

int FitsInput::read(FITS::HDUType t, char *addr, int nb) { 
	// read next nb bytes into addr
	if (rec_type != FITS::HDURecord || t != hdu_type || (!header_done)) {
		errmsg(BADOPER,"Illegal operation on FITS input");
		return 0;
	}
	if (curr_size == 0) {
		read_header_rec();
		return 0;
	}
	if (nb > curr_size)
		nb = curr_size;
	int n = nb;
	if (bytepos == FitsRecSize) {
		curr = fin.read();
	    	if (!curr) {
		    errmsg(BADEOF,"Unexpected end of file.");
		    rec_type = FITS::EndOfFile;
		    return -1;
	    	}
	    	if (fin.err()) {
		    errmsg(IOERR,"Error reading first data record.");
		    rec_type = FITS::UnrecognizableRecord;
		    return -1;
	    	}
		bytepos = 0;
	}
	do {
	    if (n <= (FitsRecSize - bytepos)) {
	    	memcpy(addr,&curr[bytepos],n);
	    	bytepos += n;
	    	curr_size -= n;
		n = 0;
	    } else {
	    	memcpy(addr,&curr[bytepos],(FitsRecSize - bytepos));
	    	curr_size -= FitsRecSize - bytepos;
	    	n -= FitsRecSize - bytepos;
		addr += FitsRecSize - bytepos;
	    	curr = fin.read();
	    	if (!curr) {
		    errmsg(BADEOF,"Unexpected end of file.");
		    rec_type = FITS::EndOfFile;
		    return -1;
	    	}
	    	if (fin.err()) {
		    errmsg(IOERR,"Error reading first data record.");
		    rec_type = FITS::UnrecognizableRecord;
		    return -1;
	    	}
	    	bytepos = 0;
	    }
	} while ( n > 0);
	if (curr_size == 0)
		read_header_rec();
	return nb;
}

int FitsInput::skip(FITS::HDUType t, int nb) { // skip next nb bytes 
	if (rec_type != FITS::HDURecord || t != hdu_type || (!header_done)) {
		errmsg(BADOPER,"Illegal operation on FITS input");
		return 0;
	}
	if (curr_size == 0) {
		read_header_rec();
		return 0;
	}
	if (nb > curr_size)
		nb = curr_size;
	int n = nb;
	if (bytepos == FitsRecSize) {
		curr = fin.read();
	    	if (!curr) {
		    errmsg(BADEOF,"Unexpected end of file.");
		    rec_type = FITS::EndOfFile;
		    return -1;
	    	}
	    	if (fin.err()) {
		    errmsg(IOERR,"Error reading first data record.");
		    rec_type = FITS::UnrecognizableRecord;
		    return -1;
	    	}
		bytepos = 0;
	}
	do {
	    if (n <= (FitsRecSize - bytepos)) {
	    	bytepos += n;
	    	curr_size -= n;
		n = 0;
	    } else {
	    	curr_size -= FitsRecSize - bytepos;
	    	n -= FitsRecSize - bytepos;
	    	curr = fin.read();
	    	if (!curr) {
		    errmsg(BADEOF,"Unexpected end of file.");
		    rec_type = FITS::EndOfFile;
		    return -1;
	    	}
	    	if (fin.err()) {
		    errmsg(IOERR,"Error reading first data record.");
		    rec_type = FITS::UnrecognizableRecord;
		    return -1;
	    	}
	    	bytepos = 0;
	    }
	} while ( n > 0);
	if (curr_size == 0)
		read_header_rec();
	return nb;
}

void FitsInput::skip_all(FITS::HDUType t) { // skip all remaining data
	if (rec_type != FITS::HDURecord || t != hdu_type || (!header_done)) {
		errmsg(BADOPER,"Illegal operation on FITS input");
		return;
	}
	if (curr_size == 0) {
		read_header_rec();
		return;
	}
	if (bytepos < FitsRecSize) {
	    if ((FitsRecSize - bytepos) >= curr_size) {
		bytepos = FitsRecSize;
		curr_size = 0;
		read_header_rec();
		return;
	    } else {
		curr_size -= FitsRecSize - bytepos;
		bytepos = FitsRecSize;
	    }
	}
	while (curr_size > 0) {
	    curr = fin.read();
	    if (!curr) {
		    errmsg(BADEOF,"Unexpected end of file.");
		    rec_type = FITS::EndOfFile;
		    return;
	    }
	    if (fin.err()) {
		    errmsg(IOERR,"Error reading first data record.");
		    rec_type = FITS::UnrecognizableRecord;
		    return;
	    }
	    curr_size -= FitsRecSize;
	}
	read_header_rec();
	return;
}

int FitsOutput::write_hdr(FitsKeywordList &kwl, FITS::HDUType t, FITS::ValueType dt, 
	Int ds, Int is) {
	if ((rec_type == FITS::EndOfFile) || (rec_type == FITS::SpecialRecord)
	    || header_done || t == FITS::NotAHDU) {
	    errmsg(BADOPER,"Illegal operation -- cannot write FITS header.");
	    return -1;
	}
	if (t == FITS::PrimaryArrayHDU || t == FITS::PrimaryGroupHDU) {
	    if (rec_type != FITS::InitialState) {
		errmsg(BADOPER,"Primary Header must be written first.");
	    	return -1;
	    } else {
                isaprimary = True;
	        if (kwl(FITS::SIMPLE)->asBool() == True) 
		    valid_fits = True;
	        if (kwl(FITS::EXTEND))
		    if (kwl.curr()->asBool() == True)
		        extend = True;
	    }
	} else if (rec_type != FITS::HDURecord) {
	    errmsg(BADOPER,"Catastrophic error!  Illegal record type.");
	    rec_type = FITS::EndOfFile;
	    return -1;
	} else {
	    if (!hdu_complete()) {
	    	errmsg(BADOPER,"Previous HDU incomplete -- cannot write header.");
	    	return -1;
	    }
	    if (!extend) {
		errmsg(BADOPER,"Cannot write extension HDU - EXTEND not True");
		return -1;
	    } else {
	    	if (t == FITS::PrimaryArrayHDU || t == FITS::PrimaryGroupHDU) {
	    	    errmsg(BADOPER,"Primary HDU already written.");
	    	    return -1;
	    	}
	    }
	}
	rec_type = FITS::HDURecord;
	hdu_type = t;
	data_type = dt;
	data_size = ds;
	item_size = is;
	curr_size = 0;
	bytepos = 0;
	kwl.first();
	kwl.next();
	while (kc.build(curr,kwl)) {
	    fout.write(curr);
	}
	fout.write(curr);
	err_status = OK;
	header_done = True;
	if (data_size == 0)
	    header_done = False;

	return 0;
}

// write all data from addr
int FitsOutput::write_all(FITS::HDUType t, char *addr, char pad) { 
	if (!hdu_inprogress()) {
	    errmsg(BADOPER,"Illegal operation -- no HDU in progress");
	    return -1;
	}
	if (t != hdu_type) {
	    errmsg(BADOPER,"Illegal operation -- incorrect HDU type");
	    return -1;
	}
	while ((data_size - curr_size) >= FitsRecSize) {
	    memcpy(curr,addr,FitsRecSize);
	    fout.write(curr);
	    addr += FitsRecSize;
	    curr_size += FitsRecSize;
	}
	bytepos = data_size - curr_size;
	if (bytepos) {
	    memcpy(curr,addr,bytepos);
	    while (bytepos < FitsRecSize)
		curr[bytepos++] = pad;
	    fout.write(curr);
	}
	data_size = 0;
	curr_size = 0;
	err_status = OK;
        header_done = False;
	return 0;
}

int FitsOutput::write(FITS::HDUType t, char *addr, int bytes, char pad) { 
	int n;
	if (!hdu_inprogress()) {
	    errmsg(BADOPER,"Illegal operation -- no HDU in progress");
	    return -1;
	}
	if (t != hdu_type) {
	    errmsg(BADOPER,"Illegal operation -- incorrect HDU type");
	    return -1;
	}
	if ((bytes + curr_size) > data_size) {
	    errmsg(BADOPER,"Attempt to write too much data -- truncated");
	    bytes = data_size - curr_size;
	}
	if (bytes <= (FitsRecSize - bytepos)) {
	    memcpy(&curr[bytepos],addr,bytes);
	    bytepos += bytes;
	    curr_size += bytes;
	} else {
	    n = FitsRecSize - bytepos;
	    memcpy(&curr[bytepos],addr,n);
	    curr_size += n;;
	    addr += n;
	    bytes -= n;
	    fout.write(curr);
	    while (bytes >= FitsRecSize) {
		memcpy(curr,addr,FitsRecSize);
		fout.write(curr);
		addr += FitsRecSize;
		curr_size += FitsRecSize;
		bytes -= FitsRecSize;
	    }
	    bytepos = bytes;
	    if (bytes) {
		memcpy(curr,addr,bytes);
		curr_size += bytes;
	    }
	}
	// Fill up and write the last record as long as the data doesn't
	// evenly fill the last record.
	if (curr_size == data_size) {
	    if (bytes) {
		while (bytepos < FitsRecSize)
		    curr[bytepos++] = pad;
		fout.write(curr);
	    }
	    data_size = 0;
	    curr_size = 0;
            header_done = False;
	}
	err_status = OK;
	return 0;
}

int FitsOutput::write_sp (char *rec) { // write a special record
	if (rec_type == FITS::EndOfFile) {
	    errmsg(BADOPER,"Illegal operation -- EOF has been written");
	    return -1;
        }
	if (hdu_inprogress()) {
	    errmsg(BADOPER,"Illegal operation -- HDU in progress");
	    return -1;
	}
	if (rec_type != FITS::SpecialRecord)
		rec_type = FITS::SpecialRecord;
	fout.write(rec);
	return 0;
}
		

FitsDiskInput::FitsDiskInput(const char *f, int l, int n, ostream &s) :
	BlockInput(f,l,n,s) {
}

FitsDiskOutput::FitsDiskOutput(const char *f, int l, int n, ostream &s) :
	BlockOutput(f,l,n,s) {
}

FitsStdInput::FitsStdInput(int l, ostream &s) :
	BlockInput(0,l,1,s) {
}

FitsStdOutput::FitsStdOutput(int l, ostream &s) :
	BlockOutput(1,l,1,s) {
}

FitsTape9Input::FitsTape9Input(const char *f, int l, int n, ostream &s) :
	BlockInput(f,l,n,s) {
}

FitsTape9Output::FitsTape9Output(const char *f, int l, int n, ostream &s) :
	BlockOutput(f,l,n,s) {
}

FitsIO::FitsIO(ostream &e) : FitsRecSize(2880), valid_fits(False),
	extend(False), isaprimary(False), header_done(False),
	rec_type(FITS::InitialState), hdu_type(FITS::NotAHDU), errs(e), 
	err_status(OK), curr(0), bytepos(0), item_size(0), 
	data_type(FITS::NOVALUE), data_size(0), curr_size(0)  {
}

FitsInput::FitsInput(const char *n, const FITS::FitsDevice &d, int b, 
	ostream &e) : FitsIO(e), fin(make_input(n,d,b,e)), got_rec(False) {
	init();
}

FitsInput::FitsInput(ostream &e) : FitsIO(e), 
	fin(*(BlockInput *)(new FitsStdInput(FitsRecSize,e))),
	got_rec(False) {
	init();
}

