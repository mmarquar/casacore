//# Choice.h: Ask a choice to the user
//# Copyright (C) 2004
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
//#
//# $Id$

#ifndef AIPS_CHOICE_H
#define AIPS_CHOICE_H


//# Includes
#include <aips/Utilities/String.h>
#include <iostream>

//# Forward Declarations
template<class T> class Vector;


class Choice
{
public:
  // Define the signature of the choice function.
  typedef String ChoiceFunc (const String& descriptiveText,
			     const Vector<String>& choices);

  // Get a choice from the user.
  // The choice function to be used can be set using setChoiceFunc.
  // It can, for instance, be done by ObjectController.
  // Initially no choice function is set. In that case it returns
  // the first choice (so that should be the default choice).
  // If choices is zero length, an empty string is returned.
  static String choice (const String& descriptiveText,
			const Vector<String>& choices);

  // Set the choice function.
  // It returns the old choice function.
  static ChoiceFunc* setChoiceFunc (ChoiceFunc* func);

  // A choice function asking on stderr.
  static String stderrChoice (const String& descriptiveText,
			      const Vector<String>& choices)
    { return ostreamChoice (std::cerr, descriptiveText, choices); }

  // A choice function asking on stdout.
  // It outputs the descriptiveText followed by a ?, the options and a colon.
  // The default option is shown in square brackets.
  static String stdoutChoice (const String& descriptiveText,
			      const Vector<String>& choices)
    { return ostreamChoice (std::cout, descriptiveText, choices); }


private:
  // Ask on an ostream.
  static String ostreamChoice (std::ostream&,
			       const String& descriptiveText,
			       const Vector<String>& choices);

  //# Pointer to the choice function.
  static ChoiceFunc* theirChoiceFunc; 

};


#endif
