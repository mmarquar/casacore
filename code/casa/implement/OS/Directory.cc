//# Directory.cc: Class to define a Directory
//# Copyright (C) 1996,1997,1999
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


#include <aips/Exceptions.h>
#include <aips/OS/Directory.h>
#include <aips/OS/DirectoryIterator.h>
#include <aips/OS/RegularFile.h>
#include <aips/OS/SymLink.h>

#include <aips/Arrays/Vector.h>
#include <aips/Arrays/ArrayMath.h>

#include <unistd.h>                 // needed for rmdir, unlink
#include <sys/stat.h>               // needed for mkdir
#include <errno.h>                  // needed for errno
#include <string.h>                 // needed for strerror

#if defined(AIPS_SOLARIS) || defined(AIPS_OSF)
#  if defined(AIPS_OSF)
    extern "C" {                    // missing in system include file
#  endif
#  include <sys/statvfs.h>          // needed for statvfs
#  if defined(AIPS_OSF)
    }
#  endif
#  define statfs statvfs
#else
#  include <sys/vfs.h>
#  if defined(AIPS_IRIX)
#    include <sys/statfs.h>
#    define f_bavail f_bfree
#  endif
#endif

// Shouldn't be needed, but is needed to get rename under linux. The
// man page claims it's in unistd.h.
#include <stdio.h>


Directory::Directory()
: File()                         // sets to working directory
{}

Directory::Directory (const Path& name)
: File(name)
{
    checkPath();
}

Directory::Directory (const String& name)
: File(name)
{
    checkPath();
}

Directory::Directory (const File& name)
: File(name)
{
    checkPath();
}

Directory::Directory (const Directory& directory)
: File    (directory),
  itsFile (directory.itsFile)
{}

Directory::~Directory()
{}

Directory& Directory::operator= (const Directory& directory)
{
    if (this != &directory) {
	File::operator= (directory);
	itsFile = directory.itsFile;
    }
    return *this;
}

void Directory::checkPath()
{
    itsFile = *this;
    // If exists, check if it is a directory.
    // If the file is a symlink, resolve the entire symlink chain.
    // Otherwise check if it can be created.
    if (exists()) {
	if (isSymLink()) {
	    itsFile = SymLink(*this).followSymLink();
	    // Error if no directory and exists or cannot be created.
	    if (!itsFile.isDirectory()) {
		if (itsFile.exists() || !itsFile.canCreate()) {
		    throw (AipsError ("Directory: " + path().expandedName() +
				      " is a symbolic link not"
				      " pointing to a valid directory"));
		}
	    }
	} else if (!isDirectory()) {
	    throw (AipsError ("Directory: " + path().expandedName() +
			      " exists, but is no directory"));
	}
    } else {
	if (!canCreate()) {
	    throw (AipsError ("Directory: " + path().expandedName() +
			      " does not exist and cannot be created"));
	}
    }
}

uInt Directory::nEntries() const
{
    uInt nentries = 0;
    DirectoryIterator iter(*this);
    while (! iter.pastEnd()) {
	nentries++;
	iter++;
    }
    return nentries;
}

Bool Directory::isEmpty() const
{
    DirectoryIterator iter(*this);
    while (! iter.pastEnd()) {
	return False;
    }
    return True;
}

Double Directory::freeSpace() const
{
    struct statfs buf;
#if defined(AIPS_IRIX)
    if (statfs (itsFile.path().expandedName(), &buf, sizeof(buf), 0) < 0) {
#else
    if (statfs (itsFile.path().expandedName(), &buf) < 0) {
#endif
	throw (AipsError ("Directory::freeSpace error on " +
			  itsFile.path().expandedName() +
			  ": " + strerror(errno)));
    }
    Double bsize = buf.f_bsize;
#if defined(AIPS_SOLARIS) || defined(AIPS_OSF)
    //# The fragment size usually contains the true block size.
    if (buf.f_frsize > 0) {
	bsize = buf.f_frsize;
    }
#endif
    return bsize * buf.f_bavail;
}

void Directory::create (Bool overwrite)
{
    // If overwrite is False the directory will not be overwritten.
    if (exists()) {
	if (!itsFile.isDirectory()) {
	    throw (AipsError ("Directory::create: " +
			      itsFile.path().expandedName() +
			      " already exists as a non-directory"));
	}
	if (!overwrite) {
	    throw (AipsError ("Directory::create: " +
			      itsFile.path().expandedName() +
			      " already exists"));
	}
	Directory(itsFile).removeRecursive();
    }
    if (mkdir (itsFile.path().expandedName(), 0755) < 0) {
	throw (AipsError ("Directory::create error on " +
			  itsFile.path().expandedName() +
			  ": " + strerror(errno)));
    }
}

void Directory::remove()
{
    // If the directory is not empty it cannot be removed.
    if (! isEmpty()) {
	throw (AipsError ("Directory::remove: " + path().expandedName() +
			  " is not empty"));
    }
    if (isSymLink()) {
	removeSymLinks();
    }
    rmdir (itsFile.path().absoluteName());
}

void Directory::removeFiles()
{
    DirectoryIterator iter(*this);
    while (! iter.pastEnd()) {
	File file = iter.file();
	if (! file.isDirectory (False)) {
	    unlink (file.path().originalName());
	}
	iter++;
    }
}

void Directory::removeRecursive()
{
    DirectoryIterator iter(*this);
    while (! iter.pastEnd()) {
	File file = iter.file();
	if (file.isDirectory (False)) {
	    Directory(file).removeRecursive();
	} else {
	    unlink (file.path().originalName());
	}
	iter++;
    }
    remove();
}

void Directory::copy (const Path& target, Bool overwrite,
		      Bool setUserWritePermission) const
{
    Path targetName(target);
    checkTarget (targetName, overwrite, True);
    // Remove the target if it already exists.
    File targetFile(targetName);
    if (targetFile.isRegular (False)) {
	RegularFile(targetFile).remove();
    } else if (targetFile.isDirectory (False)) {
	Directory(targetFile).removeRecursive();
    } else {
	SymLink(targetFile).remove();
    }
    // Copy the entire directory recursively using the system function cp.
    String command("cp -r ");
    command += itsFile.path().expandedName() + " " + targetName.expandedName();
    system (command);
    // Give write permission to user if needed.
    if (setUserWritePermission) {
#if defined(__hpux__) || defined(AIPS_IRIX)
	command = "chmod -R u+w ";
#else
	command = "chmod -Rf u+w ";
#endif
	command += targetName.expandedName();
	system (command);
    }
}

void Directory::move (const Path& target, Bool overwrite)
{
    Path targetPath(target);
    checkTarget (targetPath, overwrite, True);
    // Start trying to rename.
    // If source and target are the same directory, rename does nothing
    // and returns a success status.
    if (rename (path().expandedName().chars(),
		targetPath.expandedName().chars()) == 0) {
	return;
    }
    // The rename failed for one reason or another.
    // Remove the target if it already exists.
#if defined(ENOTEMPTY)
    if (errno == EEXIST || errno == ENOTEMPTY) {
#else
    if (errno == EEXIST) {
#endif
	Directory(targetPath).removeRecursive();
    } else if (errno == ENOTDIR) {
	unlink (targetPath.expandedName().chars());
    }
    // Try again.
    if (rename (path().expandedName().chars(),
		targetPath.expandedName().chars()) == 0) {
	return;
    }
    // Throw an exception if not "different file systems" error.
    if (errno != EXDEV) {
	throw (AipsError ("Directory::move error on " +
			  path().expandedName() + " to " +
			  targetPath.expandedName() +
			  ": " + strerror(errno)));
    }
    // Copy the directory and remove it thereafter.
    copy (targetPath, overwrite, False);
    removeRecursive();
}

// Vector<String> Directory::find (const String& fileName,
// 				Bool followSymLinks) const
// {
// //#//    return find ('^' + Regex::fromString (fileName) + '$', followSymLinks);
//     return find (Regex (Regex::fromString (fileName)), followSymLinks);
// }

Vector<String> Directory::find (const Regex& regexp, Bool followSymLinks,
                                Bool recursive) const
{
    DirectoryIterator iter(*this);
    Vector<String> myentries(10);
    uInt count=0;
    while (!iter.pastEnd()) {
//#//        if (iter.name().contains (regexp)) {
        if (iter.name().matches (regexp)) {
	    if (count + 1 >= myentries.nelements()) {
	        // More entries have been added - have to resize
	        myentries.resize (2*myentries.nelements(), True);
	    }
	    myentries(count) = iter.name();
	    count++;
	}
	iter++;
    }
    myentries.resize (count, True); // Trim the trailing entries

    // Now recursively add all the ones we find in subdirectories, prepending
    // the pathname.
    if (recursive) {
       iter.reset();
       while (!iter.pastEnd()) {
          File file = iter.file();
	  if (file.isDirectory (followSymLinks)) {
	     Directory subdir = file;
 	     Vector<String> subentries = subdir.find (regexp);
	     String basename = iter.name() + "/";
	     subentries = basename + subentries;
	     uInt oldsize = myentries.nelements();
	     myentries.resize (oldsize + subentries.nelements(), True);
	     myentries(Slice(oldsize, subentries.nelements())) = subentries;
	  }
  	  iter++;
       }
    }
    return myentries;
}



Vector<String> Directory::shellExpand (const Vector<String>& files, Bool stripPath)
//
// Take a list of potentially wild-carded file names, and expand
// them into (optional) absolute path and name.  Some more development
// needed to expand the path as well, if there are any wild cards in that.
//
{
   Vector<String> expInNames;
   uInt nExpInNames = 0;
   uInt k = 0;
   Regex exp;
//
   for (uInt i=0; i<files.nelements(); i++) {

// Find the directory of this file.  

      Path path(files(i));
      Directory dir(path.dirName());

// Try and expand it (minus path)

      try {
         exp = Regex::fromPattern(path.baseName());            
      } catch (RegexExpressnError x) {
         String msg = String("Error parsing file ") + files(i);
         throw (AipsError(msg));
      } end_try;

// Find all the matched files

      Vector<String> expFiles = dir.find(exp, True, False);    
      nExpInNames += expFiles.nelements();
      expInNames.resize(nExpInNames, True);

// Add the path back on to each name
      
      if (stripPath) {
         for (uInt j=0; j<expFiles.nelements(); j++) {
            expInNames(k) = expFiles(j);
            k++;
         }
      } else {
         for (uInt j=0; j<expFiles.nelements(); j++) {
            expInNames(k) = Path::addDirectory("./"+expFiles(j), path.absoluteName());  
            k++;
         }
     }
   }

// Return result

   return expInNames;
}

