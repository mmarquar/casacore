//# Path.cc: Class to define a pathname
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


#include <aips/OS/Path.h>
#include <aips/Utilities/Assert.h>
#include <aips/Exceptions.h>

#include <pwd.h>                    // needed for getpwnam
#include <unistd.h>                 // needed for pathconf
#include <limits.h>                 // needed for PATH_MAX, etc.
#include <ctype.h>                  // needed for isprint


// The maximum number of bytes in a pathname is 255 (_POSIX_PATH_MAX)
// Definition for POSIX systems
#if defined (_POSIX_PATH_MAX)
static const int pathmax_posix = _POSIX_PATH_MAX;
#else
staic const int pathmax_posix = 255;
#endif


// The maximum number of bytes in a pathname is PATH_MAX 
#if defined (PATH_MAX)
static int pathMax = PATH_MAX;
#else
static int pathMax = 0;
#endif

const int PATH_MAX_GUESS = 1024; // if PATH_MAX is indeterminate
                                 // we're not guaranteed this is adequate. 

// The maximum number of bytes in a filename is 14 (_POSIX_NAME_MAX)
// Definition for POSIX systems
#if defined (_POSIX_NAME_MAX)
const int namemax_posix = _POSIX_NAME_MAX;
#else
const int namemax_posix = 14;
#endif


// The maximum number of bytes in a filename is NAME_MAX
#if defined (NAME_MAX)
static int nameMax = NAME_MAX;
#else
static int nameMax = 0;
#endif

const int NAME_MAX_GUESS = 255;  // if NAME_MAX is indeterminate
                                   // we're not guaranteed this is adequate. '


Path::Path() 
: itsOriginalPathName ("."), 
  itsAbsolutePathName (0),
  itsExpandedPathName (0)
{
    itsAbsolutePathName = new String(); 
    itsExpandedPathName = new String();
}

Path::Path (const String& pathName)
: itsOriginalPathName (pathName), 
  itsAbsolutePathName (0),
  itsExpandedPathName (0)
{
    if (itsOriginalPathName.empty()) { 
	itsOriginalPathName = ".";    
    }
    itsAbsolutePathName = new String(); 
    itsExpandedPathName = new String();
}    

Path::Path (const Path& that)
: itsOriginalPathName (that.itsOriginalPathName), 
  itsAbsolutePathName (0),
  itsExpandedPathName (0)
{
    itsAbsolutePathName = new String (* (that.itsAbsolutePathName));
    itsExpandedPathName = new String (* (that.itsExpandedPathName));
}

Path::~Path()
{
    delete itsAbsolutePathName;
    delete itsExpandedPathName;
}   

Path& Path::operator= (const Path& that)
{
    if (this != &that) {   
	itsOriginalPathName = that.itsOriginalPathName;
	delete itsAbsolutePathName;
	itsAbsolutePathName = new String (* (that.itsAbsolutePathName));
	delete itsExpandedPathName;
	itsExpandedPathName = new String (* (that.itsExpandedPathName));
    }
    return *this;
}

void Path::append (const String& string)
{
    if  (!string.empty()) {
	if ((itsOriginalPathName.lastchar() != '/') && 
	    (string.firstchar() != '/')) {
	    itsOriginalPathName += "/";
	}
	itsOriginalPathName += string;
	*itsAbsolutePathName = "";
	*itsExpandedPathName = "";
    }
}

const String& Path::expandedName() const
{
    if (itsExpandedPathName->empty()) {
	*itsExpandedPathName = expandName (itsOriginalPathName);
    }
    return *itsExpandedPathName;
}

const String& Path::absoluteName() const
{
    if (itsAbsolutePathName->empty()) {
	*itsAbsolutePathName = makeAbsoluteName (expandedName());
    }
    return *itsAbsolutePathName;
}

Bool Path::isValid() const
{
    // Check if the length of the pathname is not too long
    if (itsOriginalPathName.length() > getMaxPathNameSize()) {
	return False;
    }
    // Check if pathname contains double slashes
    if (itsOriginalPathName.contains ("//")) {
	return False;
    }
    // Check if pathname contains non-printables
    for (int i = 0; i < itsOriginalPathName.length(); i++) {
	if (isprint (itsOriginalPathName[i]) == 0 ) {
	    return False;
	}
    }

    // Check if the length of the pathname is not too long
    if (itsOriginalPathName.length() > getMaxPathNameSize()) {
	return False;
    }

    // Check if filenames are not too long
    String subPathname[30];
    String sep = "/";
    int nw = split (itsOriginalPathName, subPathname, 15, sep);
    long nameSize = getMaxNameSize();
    for (i = 0; i < nw; ++i) {
	if (subPathname[i].length() > nameSize ) {
	    return False;
	}
    }
    return True;
}

Bool Path::isStrictlyPosix() const
{
    // Check if the length of the pathname is not too long, according POSIX 
    // standard
    if(itsOriginalPathName.length() > pathmax_posix ) {
	return False;
    }

    // Check if pathname contains double slashes
    if (itsOriginalPathName.contains ("//")) {
	return False;
    }

    // Check if pathname contains non-printables
    for (int i=0; i < itsOriginalPathName.length(); i++) {
	if (!isprint (itsOriginalPathName[i])) {
	    return False;
	}
    }

    // Check if filenames are not too long, according POSIX standard
    String subPathname[30];
    String sep = "/";
    int nw = split(itsOriginalPathName, subPathname, 15, sep);
    for (i = 0; i < nw; ++i) {
	if (subPathname[i].length() > namemax_posix )
	    return False;
    }
    return True;
}

uInt Path::length() const
{
    return itsOriginalPathName.length();
}

uInt Path::maxLength() const
{
    return getMaxPathNameSize();
}

String Path::baseName() const
{
    // Determine from expanded path name.
    String name = expandedName();
    // Search last slash.
    // Get rid of trailing slash.
    Int len=name.length();
    if (len > 0  &&  name[len-1] == '/') {
	len--;
    }
    Int i=len;
    while (--i >= 0  &&  name[i] != '/');
    // The base name is the part from the slash till the end.
    return name(i+1, len-i-1);
}

String Path::dirName() const
{
    String name = expandedName();
    // Search last slash.
    // Get rid of trailing slash (except if name consists of a slash only).
    Int i=name.length();
    if (i > 1  &&  name[i-1] == '/') {
	i--;
    }
    while (--i >= 0  &&  name[i] != '/');
    // If no slash, the dirname is the current directory.
    if (i < 0) {
	return ".";
    }
    // If the slash found is not the first character, skip it.
    if (i > 0) {
	i--;
    }
    return name.through(i);
}


uInt Path::getMaxPathNameSize()
{
    // pathMax is not defined(<0) then pathconf sets pathMax, 
    // if this doesn't work pathMax will get the value of PATH_MAX_GUESS
    if (pathMax <= 0) {
	return ( (pathMax = (int)pathconf ("/",_PC_PATH_MAX)) < 0) ?
	                                        pathMax : PATH_MAX_GUESS;
    }
    return pathMax;
}

long Path::getMaxNameSize()
{
    // nameMax is not defined (<0) then pathconf sets nameMax, 
    // if this doesn't work nameMax will get the value of PATH_MAX_GUESS
    if (nameMax <= 0) {
	return (nameMax = (int) (pathconf ("/",_PC_NAME_MAX))) < 0 ?
	                                        nameMax : NAME_MAX_GUESS; 
    }
    return nameMax;
}


String Path::expandName (const String& inString) const
{
    String tempString (inString);
    int cursor = 0;
    int i = 0;
    Bool flag = True;
    int count = 0;

    // Flag is set True when an environment variable is detected. When this 
    // happens more then 25 times, there is probably a recursive variable set.
    // In that case an exception will be thrown.
    while (flag && count < 25) {  
	// flag is False, when there is not an environment variable
	// the name will not be checked again
	flag = False;     
	count++;          // count is increased when the string is 
	cursor = 0;       // walked through 
	// Replace tilde with the name of the home directory
	if (tempString.firstchar() == '~') {
	    if (tempString.length() == 1 || tempString[1] == '/') {
		char* name=getenv ("HOME");       // To get the homedirectory,
                                                  // the environment variable
		if (name != 0) {                  // HOME is used
		    tempString.del ("~",0);
		    tempString.prepend (name);
		}
	    } else {
		tempString.del ("~",0);
		getNextName (tempString, cursor);
		String temp (tempString.before(cursor));
		passwd* passWd = getpwnam(temp.chars());
		// The password file is used to get the homedirectory 
		// of "~name"
		if (passWd != 0) {
		    tempString.del (tempString.before (cursor));
		    tempString.prepend (passWd->pw_dir);
		    cursor = 0;
		}else{
		    tempString.prepend ("~");
		}
	    }
	}
	cursor = i = 0;
	while (i < tempString.length()) {
	    getNextName (tempString,i);       // i is set on the next name   
	    if (tempString[cursor] == '$') {  // Environment variable detected
		String dName (tempString.at(cursor+1,(i-cursor)-1));
		char* name = getenv (dName.chars());
		if (name != 0) {
		    String res (name);
		    res.prepend (tempString.before(cursor));
		    res += tempString.after (i-1);
		    tempString = res;
		    // name is placed in the new string and the old name is 
		    // gone
		    // flag is set True, so the name will be checked again for 
		    // environment variables
		    flag = True;
		}
	    }	
	    cursor = i + 1;
	    i++;
	}
    }
    if (flag) {
	// An exception is thrown when the string is checked more then 25 times
	throw (AipsError ("Path::expandName: recursive environment variable"));
    }
    return tempString;
}


String Path::makeAbsoluteName (const String& inString) const
{
    // If the first char is a slash the name is already absolute.
    if (inString.firstchar() == '/') {
	return inString;
    }
    // Otherwise we have a relative pathname.
    // Remove a possible leading . or ./
    String workString (inString);
    if (workString.length() > 0) {
	if (workString[0] == '.') {
	    Int from = 1;
	    if (workString.length() > 1  &&  workString[1] == '/') {
		from = 2;
	    }
	    workString = workString.from (from);
	}
    }
    // Get the working directory and prepend it.
    // Insert a / when needed.
    char temp[1024];
    getcwd (temp, 1024);
    String tempString (temp);
    // Return the working directory when no input string left.
    if (workString.empty()) {
	return tempString;
    }
    // Insert a / when needed.
    if (tempString.lastchar() != '/') {
	tempString += '/';
    }
    tempString += workString;
    return tempString;
}

void Path::getNextName (const String& inString, int& count) const
{
    // Sets count on the next slash or on the end of the string
    if ((count = inString.index ("/", count)) < 0) {
	count = inString.length();
    }
}


String Path::stripDirectory (const String& name, const String& otherName)
{
    // Get the directory of other (with the trailing /).
    String dir (Path(otherName).dirName() + '/');
    String tName(name);
    // Remove possible leading ./ from dir and name.
    while (tName.length() >= 2  &&  tName[0] == '.'  &&  tName[1] == '/') {
	tName = tName.after(1);
    }
    while (dir.length() >= 2  &&  dir[0] == '.'  &&  dir[1] == '/') {
	dir = dir.after(1);
    }
    int leng = dir.length();
    // If directory is contained in this name, return name without it.
    if (leng > 0) {
	if (tName.length() > leng  &&  tName.before(leng) == dir) {
	    // The leading ./ indicates that the directory has been stripped.
	    return "./" + tName.from (leng);
	}
    }else{
	// Also if the parent is this directory and the subtable is relative.
	if (Path(tName).expandedName()[0] != '/') {
	    return "./" + tName;
	}
    }
    return tName;
}

String Path::addDirectory (const String& name, const String& otherName)
{
    // If a directory was stripped off, add the directory of the parent.
    // Otherwise use the name as such.
    // Note that dirName will use the expanded name.
    // Start with removing possible leading ./
    String tName(name);
    while (tName.length() >= 2  &&  tName[0] == '.'  &&  tName[1] == '/') {
	tName = tName.after(1);
    }
    // If anything was removed, we have to add the directory of the other name.
    if (tName.length() < name.length()) {
	Path dir (Path(otherName).dirName());
	dir.append (tName);
	return dir.originalName();
    }
    return tName;
}
