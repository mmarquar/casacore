//# Utilities.h: Bag of unrelated classes and groups for general use.
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


#if !defined (AIPS_MODULE_UTILITIES_H)
#define AIPS_MODULE_UTILITIES_H

#include <aips/Utilities/Assert.h>
#include <aips/Utilities/BinarySearch.h>
#include <aips/Utilities/BitVector.h>
#include <aips/Utilities/Compare.h>
#include <aips/Utilities/Copy.h>
#include <aips/Utilities/CountedPtr.h>
#include <aips/Utilities/DataType.h>
#include <aips/Utilities/DynBuffer.h>
#include <aips/Utilities/Fallible.h>
#include <aips/Utilities/GenSort.h>
#include <aips/Utilities/Notice.h>
#include <aips/Utilities/PtrHolder.h>
#include <aips/Utilities/RegSequence.h>
#include <aips/Utilities/Regex.h>
#include <aips/Utilities/RegexError.h>
#include <aips/Utilities/Sequence.h>
#include <aips/Utilities/Sort.h>
#include <aips/Utilities/SortError.h>
#include <aips/Utilities/String.h>
#include <aips/Utilities/ValType.h>
#include <aips/Utilities/cregex.h>
#include <aips/Utilities/generic.h>

// <module>
//
// <summary> Classes and global functions for general use </summary>

// <reviewed reviewer="Friso Olnon" date="1995/03/20" demos="">
// </reviewed>

// <synopsis>
//
// This module is a bag of unrelated mini-modules, classes and
// global functions. The following functional groups can be recognized:
// <ul>
//  <li> Object utilities:
//  <ul>
//   <li> <a href="Utilities/Compare.html">Compare</a>
//    objects with each other. A signature for comparison functions 
//    is defined (required for comparison functions used in the
//    <linkto class="Sort">Sort</linkto>
//    class), and one such function is provided.
//   <li> <a href="Utilities/Copy.html">Copy</a>
//    objects from one place to another.
//   <li> <a href="Utilities/Fallible.html">Mark</a>
//    objects as valid or invalid.
//   <li> <a href="Utilities/Notice.html">Notices</a>
//    provide basic support for shared access of data by various objects.
//   <li> <a href="Utilities/Sort.html">Sort</a>
//    objects on one or more keys, in ascending or descending order.
//    <linkto class=GenSort>Fast sorting</linkto>
//    is provided for certain types of objects.
//   <li> <linkto group="BinarySearch.h#binarysearch">Binary Search</linkto>
//    templated functions for sorted containers (ascending or descending order) 
//    are available.
//  </ul> 
//  <li> Logical utilities:
//  <ul>
//   <li> <a href="Utilities/Assert.html">Assertion</a>
//    lets you throw an error when a condition in not fullfilled.
//   <li> <a href="Utilities/BitVector.html">Bit vectors</a>
//    are an efficient method to keep True/False information on a set of
//    items or conditions.
//  </ul> 
//  <li> Pointer utilities
//  <ul>
//   <li> <a href="Utilities/CountedPtr.html">Counted pointers</a>
//    provide support for reference counting.
//   <li> <a href="Utilities/PtrHolder.html">Pointer holders</a>
//    can be used to hold allocated pointers which should be deleted
//    when an exception is thrown.
//  </ul> 
//  <li> Datatype utilities
//  <ul>
//   <li> <a href="Utilities/DataType.html">DataType</a>
//    enumerates the possible data types in the table system.
//   <li> <a href="Utilities/ValType.html">ValType</a>
//    describes the data types and their undefined values.
//  </ul> 
//  <li> Other utilities
//  <ul>
//   <li> <a href="Utilities/DynBuffer.html">Dynamic buffers</a>
//    are used to store data in dynamically allocated buffers.
//   <li> <a href="Utilities/Regex.html">Regular expressions</a>
//    are supported by the class <linkto class=Regex>Regex</linkto> and
//    the associated function library 
//    <a href="Utilities/cregex.html">cregex</a>.
//   <li> <a href="Utilities/Sequence.html">Sequences</a>
//    of any datatype can be derived from the base class
//    <linkto class=Sequence>Sequence</linkto>.
//    One example is <linkto class=uIntSequence>uIntSequence</linkto>,
//    provided for general use. Another is
//    <linkto class=RegSequence>RegSequence</linkto>, exclusively used
//    by the <a href="RTTI/Register.html">Register</a> function.
//   <li> <a href="Utilities/String.html">Strings</a>.
//    for the C++ preprocessor
//  </ul>
// </ul>
//  </ul>
//
// <note role=tip> You may want to look at the individual header files
// to see whether you might not prefer to include only the header
// files you really need; it may be more efficient to do so.
// </note>
//
// </synopsis>

// <todo asof="1995/03/20">
//   <li> insert String description
// </todo>

// </module>

#endif

