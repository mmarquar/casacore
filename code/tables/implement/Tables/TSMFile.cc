//# TSMFile.cc: Tiled Hypercube Storage Manager for tables
//# Copyright (C) 1995,1996,1997
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


//# Includes
#include <aips/Tables/TSMFile.h>
#include <aips/Tables/TiledStMan.h>
#include <aips/Tables/Table.h>
#include <aips/Tables/DataManError.h>
#include <aips/IO/AipsIO.h>
#include <aips/Utilities/String.h>


TSMFile::TSMFile (TiledStMan* stman, uInt fileSequenceNr)
: stmanPtr_p  (stman),
  fileSeqnr_p (fileSequenceNr),
  file_p      (0),
  length_p    (0)
{
    // Create the file.
    char strc[8];
    sprintf (strc, "_TSM%i", fileSeqnr_p);
    String fileName = stmanPtr_p->fileName() + strc;
    file_p = new BucketFile (fileName);
    if (file_p == 0) {
	throw (TSMError ("Creation of file " + fileName + " failed"));
    }
}

TSMFile::TSMFile (TiledStMan* stman, AipsIO& ios, uInt seqnr)
: stmanPtr_p (stman),
  file_p     (0)
{
    getObject (ios);
    if (seqnr != fileSeqnr_p) {
	throw (DataManInternalError ("TSMFile::TSMFile"));
    }
    char strc[8];
    sprintf (strc, "_TSM%i", fileSeqnr_p);
    String fileName = stmanPtr_p->fileName() + strc;
    file_p = new BucketFile (fileName, stmanPtr_p->table().isWritable());
    if (file_p == 0) {
	throw (TSMError ("Creation of file " + fileName + " failed"));
    }
}

TSMFile::~TSMFile()
{
    delete file_p;
}


void TSMFile::putObject (AipsIO& ios) const
{
    ios << 1;                    // version 1
    ios << fileSeqnr_p;
    ios << length_p;
}

void TSMFile::getObject (AipsIO& ios)
{
    uInt version;
    ios >> version;
    ios >> fileSeqnr_p;
    ios >> length_p;
}
