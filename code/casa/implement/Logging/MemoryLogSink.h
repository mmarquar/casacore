//# MemoryLogSink.h: Save log messages in memory
//# Copyright (C) 2001
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

#if !defined(AIPS_MEMORYLOGSINK_H)
#define AIPS_MEMORYLOGSINK_H

//# Includes
#include <aips/aips.h>
#include <aips/Logging/LogSinkInterface.h>
#include <aips/Containers/Block.h>
#include <aips/Utilities/String.h>

//# Forward Declarations

// <summary>
// Save log messages in memory.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="tLogging.cc" demos="dLogging.cc">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=LogSinkInterface>LogSinkInterface</linkto>
// </prerequisite>
//
// <synopsis>
// Unlike the other classes derived from 
// <linkto class=LogSinkInterface>LogSinkInterface</linkto>, there are utility
// functions in this class which might be of some modest interest. In
// particular, the member functions which define the structure of the table
// and define the column names might be of interest.
//
// This class posts messages which pass the filter to an AIPS++
// <linkto class=Table>Table</linkto>. It puts ever field of the
// <linkto class=LogMessage>LogMessage</linkto> into its own column.
// </synopsis>
//
// <example>
// See <linkto file="Logging.h">Logging.h</linkto>.
// </example>
//
// <motivation>
// For temporary images log messages must be held in memory temporarily.
// </motivation>
//
//# <todo asof="2001/06/12">
//# </todo>

class MemoryLogSink : public LogSinkInterface
{
public:
  // Create an empty sink without a filter.
  MemoryLogSink();

  // Create an empty sink with the given filter.
  MemoryLogSink (const LogFilter& filter);

  // Copy constructor (copy semantics).
  MemoryLogSink (const MemoryLogSink& other);

  // Assignment (copy semantics).
  MemoryLogSink& operator= (const MemoryLogSink& other);
  
  virtual ~MemoryLogSink();

  // Get number of messages in sink.
  virtual uInt nelements() const;

  // Get given part of the i-th message from the sink.
  // <group>
  virtual Double getTime (uInt i) const;
  virtual String getPriority (uInt i) const;
  virtual String getMessage (uInt i) const;
  virtual String getLocation (uInt i) const;
  virtual String getObjectID (uInt i) const;
  // </group>

  // If the message passes the filter, write it to the log table.
  virtual Bool postLocally (const LogMessage& message);

  // Write a message (usually from another logsink) into the local one.
  virtual void writeLocally (Double time, const String& message,
			     const String& priority, const String& location,
			     const String& objectID);

private:
  // Avoid duplicating code in copy ctor and assignment operator
  void copy_other (const MemoryLogSink& other);

  // Rezize the blocks to the given size, but at least 64 elements
  // more than the current size.
  void resize (uInt nrnew);

  uInt          nmsg_p;
  Block<Double> time_p;
  Block<String> priority_p;
  Block<String> message_p;
  Block<String> location_p;
  Block<String> objectID_p;
};


#endif
