//# File.h: Class to get file information and a base for other file classes
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

#if !defined(AIPS_FILE_H)
#define AIPS_FILE_H

//# Includes
#include <aips/aips.h>
#include <aips/OS/Path.h>
#include <aips/Utilities/String.h>

//# Forward Declarations.
imported struct stat;


// <summary> 
// Class to get file information and a base for other file classes.
// </summary>

// <use visibility=export>

// <prerequisite> 
//    <li> Basic knowledge of the UNIX file system 
//    <li> <linkto class=Path>Path</linkto> 
// </prerequisite>

// <etymology> 
// 'File' is used in a traditional sense.
// </etymology>

// <synopsis> 
// The File class provides the primary functions needed by all kinds of 
// files (directories, regular files, symbolic links, named pipes etc.).
// These shared functions serve mostly to return information about a 
// particular file -- for instance, its type, its ownership, read, write
// and execute permissions, date of latest access and the path on secundary 
// storage associated with this file. Every file object has, by definition, 
// a <linkto class=Path>Path</linkto> object associated with it which
// defines the file name.
// <p>
// See also the derived classes
// <linkto class=RegularFile>RegularFile</linkto>,
// <linkto class=Directory>Directory</linkto>, and
// <linkto class=SymLink>SymLink</linkto>.
// <br>
// This class does not contain virtual functions, because a lot of functions 
// have different parameters, e.g. 'create' for RegularFile has one parameter
// and 'create' for SymLink has two parameters. 
// </synopsis>

// <example>
// <srcblock>
//    File myFile("someFileName");
//    if (myFile.exists()) {
//	myFile.setPermissions(0644);
//	if (myFile.isRegular()) {
//	    cout << "this file is a regular file" << endl;
//      }
//    }
//    else if (!myFile.exists()) {
//	  if (!myFile.canCreate()){
//	      cout << "cannot create this file" << endl;
//	  } 
//    }
// </srcblock>
// </example>

// <motivation> 
// File systems operations are a notorious source of porting problems.
// The file class provides a standard interface for programmers to use.
// </motivation>


class File
{
public: 

    // Construct a File object whose Path is set to the current working 
    // directory. 
    File();
    
    // Construct a File object whose Path is set to the given Path.
    // <group>
    File (const Path& path);
    File (const String& path);
    // </group>

    // Copy constructor (copy semantics).
    File (const File& that);

    ~File();
    
    // Assignment (copy semantics).
    File& operator= (const File& that);

    // Returns the pathname of the file.
    const Path& path() const;

    // Check if the file is a regular file. If the boolean followSymLink is
    // False a symbolic link will not be followed.
    Bool isRegular (Bool followSymLink = True) const;

    // Check if the file is a directory. If the boolean followSymLink is
    // False a symbolic link will not be followed.
    Bool isDirectory (Bool followSymLink = True) const;

    // Check if the file is a symbolic link.
    Bool isSymLink() const;

    // Check if the file is a pipe.
    Bool isPipe() const;

    // Check if the file is a character special file.
    Bool isCharacterSpecial() const;

    // Check if the file is a block special file.
    Bool isBlockSpecial() const;

    // Check if the file is a socket.
    Bool isSocket() const;

    // Check if the file exists.
    Bool exists() const;

    // Check if the file is readable.
    Bool isReadable() const;

    // Check if the file is writable.
    Bool isWritable() const;

    // Check if the file is executable.
    Bool isExecutable() const;

    // Check if a file can be created.
    Bool canCreate() const;
    
    // Return the userID of the file.
    long userID() const; 

    // Return the groupID of the file.
    long groupID() const;
    
    // Return the permissions as a decimal value.
    uInt readPermissions() const;

    // Set permission with perm. Perm is an octal value.
    void setPermissions (uInt permissions);

    // Update access time and modification time of a file.
    void touch (uInt time);

    // Update access time and modification time of a file. This function
    // updates the file with the current time.
    void touch();

    // Time related fucnctions:
    // Return the time when the file was last accessed in seconds since
    // 00:00:00 GMT Jan 1, 1970.
    uInt accessTime() const;

    // Return the time when the file was last accessed
    // as a 26-characters String of the form:
    // Thu Feb  3 13:40:11 1994\n\0.
    String accessTimeString() const;

    // Return the time when the file was last modified in seconds since
    // 00:00:00 GMT Jan 1, 1970.
    uInt modifyTime() const;

    // Return the time when the file was last modified
    // as a 26-characters String of the form:
    // Thu Feb  3 13:40:11 1994\n\0.
    String modifyTimeString() const;

    // Return the time when the file status was last changed in seconds since
    // 00:00:00 GMT Jan 1, 1970.
    // It is set both by writing and changing the file status information,
    // such as changes of owner, group, link count, or mode.
    uInt statusChangeTime() const;

    // return the time when the file status was last changed
    // as a 26-characters String of the form:
    // Thu Feb  3 13:40:11 1994\n\0
    String statusChangeTimeString() const;

    // Create a new unique path name in the specified directory, with
    // the specified prefix and random trailing characters:
    // <srcblock>
    //    p.newUniqueName ("./", "temp")  -->  "./tempAAA00xx32"
    //    p.newUniqueName ("/home/me", "diary")  -->  "/home/me/diaryAAA00xxb0"
    // </srcblock>
    static Path newUniqueName (const String& directory, const String& prefix);

    // Create a new unique filename without a prefix.
    // As above, but all the characters in the filename are random:
    // <srcblock>
    //    p.newUniqueName ("./")  -->  "./AAA00xx32"
    //    p.newUniqueName ("/home/me")  -->  "/home/me/AAA00xxb0"
    // </srcblock>
    static Path newUniqueName (const String& directory); 

protected:
    // This function is used by <linkto class=RegularFile>RegularFile</linkto> 
    // and <linkto class=Directory>Directory</linkto> to remove all the links
    // which, when followed, ultimately resolve to a Directory or a 
    // RegularFile.
    // For example, A->B, B->C, C->D and D points to a regular file.
    // When remove() is called for a regular file A,
    // that function uses removeLinks() to remove A, B, C and D.
    void removeSymLinks();

    // Define a function for lstat.
    // This is necessary since SunOS4.1.x prototypes lstat() with a first
    // argument of type (char*), while Solaris (and presumably all other
    // reasonable OS's) prototype it with a first argument of type
    // (const char*).  Since lstat() does not change its first argument,
    // it is safe to convert our const variable to a non-const one so that
    // we can call lstat() successfully.
    int mylstat (const char* path, struct stat* buf) const;

    // Get the lstat of this file.
    // Throw an exception when it fails.
    void getstat (struct stat* buf) const;

    // Get the lstat of a file.
    // Throw an exception when it fails.
    void getstat (const File& file, struct stat* buf) const;

    // Check if the new path for a copy or move is valid.
    // An exception is thrown if:
    // <br>- the target directory is not writable
    // <br>- or the target file already exists and overwrite==False
    // <br>- or the target file already exists and is not writable
    // <br>When the targetName represents a directory, the basename
    // of the file is appended to it. This is done to cover the
    // case where the source is a symlink to a file. In that case
    // the target will get the basename of the symlink and not the
    // the basename of the file pointed to. This is not done when
    // forDirectory==True (which is used by class Directory).
    void checkTarget (Path& targetName, Bool overwrite,
		      Bool forDirectory = False) const;

private:
    // A sequence number to generate unique file names.
    static uInt uniqueSeqnr_p;

    // Full pathname of the file.
    Path itsPath;
};


inline const Path& File::path() const
{
    return itsPath;
}

inline void File::getstat (struct stat* buf) const
{
    getstat (*this, buf);
}



#endif
