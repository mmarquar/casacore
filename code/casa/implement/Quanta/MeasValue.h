//# MeasValue.h: Base class for values in a Measure
//# Copyright (C) 1996,1997
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

#if !defined(AIPS_MEASVALUE_H)
#define AIPS_MEASVALUE_H

#if defined(_AIX)
#pragma implementation ("MeasValue.cc")
#endif

//# Includes
#include <aips/aips.h>

//# Forward Declarations
imported class ostream;

// <summary>
// Base class for values in a Measure
// </summary>

// <use visibility=export>

// <reviewed reviewer="tcornwel" date="1996/02/22" tests="tMeasMath" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=Measure>Measure</linkto> class
// </prerequisite>
//
// <etymology>
// </etymology>
//
// <synopsis>
// MeasValue forms the abstract base class for the values of quantities within
// a reference frame. Examples of derived classes are:
// <ul>
//   <li> <linkto class=MVEpoch>MVEpoch</linkto>: a moment in time
//   <li> <linkto class=MVDirection>MVDirection</linkto>: a direction in space
//   <li> <linkto class=MVPositition>MVPosition</linkto>: a position on Earth
//   <li> <linkto class=MVFrequency>MVFrequency</linkto>
//   <li> <linkto class=MVRadialVelocity>MVRadialVelocity</linkto>
//   <li> <linkto class=MVDoppler>MVDoppler</linkto>
// </ul>
// MeasValue is the generic name for the more specific instances like, e.g.,
// <linkto class=MVEpoch>MVEpoch</linkto>, an instant in time.<br>
// MeasValues can in general be constructed from an appropiate value, or array
// of values.<br>
// The value can be expressed in the internally used units (e.g.
// days for time, a 3-vector for direction in space), as an array of internally
// used units, or as a <linkto class=Quantum>Quantum</linkto>: a value with
// appropiate units. <src>Vector<Quantum<type> ></src>
// and <src>Quantum<Vector<type> ></src> can
// also be used.
//
// The value of the <src>MeasValue</src> can be obtained by a variety of 
// <src>get</src> functions, returning in general internal or <src>Quantum</src>
// values. Special formatting (like hh:mm:ss.t, dd.mm.ss.t, yy/mm/dd etc)
// are catered for in <em>conversion-type</em> classes like 
// <linkto class=MVTime>MVTime</linkto>, <linkto class=MVAngle>MVAngle</linkto>
//
// Note that the class is a pure virtual class. No instances can be created,
// but it describes the minimum set of functions necessary in derived functions.
// <note role=warning> In the member description a number of <em>dummy</em> routines are
// present. They are the only way I have found to get <em>cxx2html</em> to
// get the belonging text properly presented.
// </note>
// </synopsis>
//
// <example>
// See individual MV and Measure classes
// </example>
//
// <motivation>
// To be able to specify a physical entity appropiate for the measured
// quantity.
// </motivation>
//
// <todo asof="1997/04/15">
// </todo>

class MeasValue {

public:
  //# Enumerations
  
  //# Typedefs
  
  //# Friends
  // Output a MeasValue
  friend ostream &operator<<(ostream &os, const MeasValue &meas);
  
  //# Constructor
  // Each derived class should have at least the following constructors:
  // <srcblock>
  //	MV()			// some default
  //	MV(Double)		// some default or error if vector expected
  //	MV(Vector<Double>)	// with check for array number of elements
  //	MV(Quantity)
  //	MV(Vector<Quantity>)
  //	MV(Quantum<Vector<Double> >
  // </srcblock>
  // Float (or other standard type) versions could be added if appropiate.
  // Dummy for cxx2html
  void dummy_constr() const {;};
  
  //# Destructor
  // Destructor
  virtual ~MeasValue();

  //# Operators
  // The following operators should be present at least.
  // <srcblock>
  // MV &operator+=(const MV &meas);
  // MV &operator-=(const MV &meas);
  // Bool operator==(const MV &meas) const;
  // Bool operator!=(const MV &meas) const;
  // Bool near(const MV &meas, Double tol = 1e-13) const;
  // Bool nearAbs(const MV &meas, Double tol = 1e-13) const;
  // </srcblock>
  // Dummy for cxx2html
  void dummy_operator() const {;};
  
  //# General Member Functions
  // Get the type (== Register(M*)) of derived MeasValue
  virtual uInt type() const = 0;
  // Assert that we are the correct MeasValue type
  // <thrown>
  //   <li> AipsError if wrong MeasValue type
  // </thrown>
  // Each Measure should have:
  // <src> static void assert(const MeasValue &in); </src>
  // <group>
   // Print a MeasValue
  virtual void print(ostream &os) const = 0;
  
  // Clone a MeasValue
  virtual MeasValue *clone() const = 0;
  
  // Some of the Measure values used need the occasional adjustments to proper
  // values. Examples are MVDirection (direction cosines) which have to be
  // normalised to a length of 1 and MEpoch (time) which have to have its
  // precision maintained. For others it is an effctive no-operation.
  // <group>
  // Adjust value
  virtual void adjust();
  // Adjust value and return a normalisation value
  virtual void adjust(Double &val);
  // Re-adjust, i.e. undo a previous adjust, with value
  virtual void readjust(Double val);
  // </group>
private:
};

//# Global functions
// <summary> Global functions </summary>
// <group name=Output>
// Output declaration
ostream &operator<<(ostream &os, const MeasValue &meas);
// </group>

#endif
