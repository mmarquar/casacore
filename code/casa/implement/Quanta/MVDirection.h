//# MVDirection.h: Vector of three direction cosines
//# Copyright (C) 1996
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

#if !defined(AIPS_MVDIRECTION_H)
#define AIPS_MVDIRECTION_H

#if defined(_AIX)
#pragma implementation ("MVDirection.cc")
#endif

//# Includes
#include <aips/aips.h>
#include <aips/Measures/MVPosition.h>

//# Forward Declarations

//# Constants (SUN compiler does not accept non-simple default arguments)

// <summary> Vector of three direction cosines </summary>

// <use visibility=local>

// <reviewed reviewer="tcornwel" date="1996/02/22" tests="tMeasMath" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=MVPosition>MVPosition</linkto>
//   <li> <linkto class=Vector>Vector</linkto>
//   <li> <linkto class=Quantum>Quantum</linkto>
// </prerequisite>
//
// <etymology>
// From Measure, Value and Direction
// </etymology>
//
// <synopsis>
// An MVDirection is a 3-vector of direction cosines. It is based on the
// MVposition class. The main difference is that the length of the
// vector will be adjusted (normalised) to a length of 1 in all operations.
// It can be constructed with:
// <ul>
//   <li> MVDirection() creates direction cosines for pole: (0,0,1)
//   <li> MVDirection(MVDirection) creates a copy
//   <li> MVDirection(Double, Double, Double) creates with
//		specified values and adjust to length of 1.
//   <li> MVDirection(Double, Double) creates a MVDirection assuming that the two
//		values are (in radians) angle along 'equator' and towards 'pole'.
//   <li> MVDirection(Quantity, Quantity) creates a MVDirection assuming angles
//		as in previous
//   <li> <src>MVDirection(Quantum<Vector<Double> >)</src> creates an MVDirection
//		from angle vector, assuming
// 		second as zero if not present, and pole if length 0. Assumes
//		a direction cosine if 3 elements
//   <li> <src>MVDirection(Vector<Double>)</src> creates an MVDirection with
//		the same restrictions as previous one
//   <li> <src>MVDirection(Vector<Quantum<Double> >)</src> creates an 
//		MVDirection with the same rstrictions as previous one; but
//		with unit check.
// </ul>
// A void adjust() function normalises the vector to a length of 1;
// a get() returns as a
// Double 2-vector the angles of the direction cosines; a getAngle() returns
// a Quantum 2-vector, (uInt) returns the indicated element, and getValue
// returns the direction cosine vector.<br>
// Direction cosines can be added and subtracted: the result will be 
// adjusted to a length of 1.<br>
// The multiplication of two direction cosines produces the inner product.<br>
// </synopsis>
//
// <example>
// See <linkto class=MDirection>MDirection</linkto>
// </example>
//
// <motivation>
// To aid coordinate transformations
// </motivation>
//
// <todo asof="1996/02/04">
// </todo>

class MVDirection : public MVPosition {

    public:
//# Friends

//# Constructors
// Default constructor generates a direction to the pole (i.e. (0,0,1))
    MVDirection();
// Copy constructor
    MVDirection(const MVPosition &other);
// Constructs with elevation = 0.
// <group>
    MVDirection(Double in0);
    MVDirection(const Quantity &angle0);
// </group>
// Creates a specified vector
    MVDirection(Double in0, Double in1, Double in2);
// Creates the direction cosines from specified angles along equator (azimuth)
// and towards pole (,elevation).
    MVDirection(Double angle0, Double angle1);
// Creates the direction cosines from specified angles
// <thrown>
//    <li> AipsError if quantities not in angle format
// </thrown>
// <group>
    MVDirection(const Quantity &angle0, const Quantity &angle1);
// If not enough angles: pole (=(0,0,1)) assumed (if none), or elevation =0 (if 1);
// direction cosines assumed (if 3).
// <thrown>
//  <li> AipsError if more than 3 values or incorrect units
// </thrown>
    MVDirection(const Quantum<Vector<Double> > &angle);
// </group>
// Create from Vector. Assumes angles if less than or equal than 2 elements.
// Assumes direction cosines if 3 elements.
// <thrown>
//  <li> AipsError if more than 3 elements
// </thrown>
// <group>
    MVDirection(const Vector<Double> &other);
    MVDirection(const Vector<Quantity> &other);
// </group>
// Copy assignment
    MVDirection &operator=(const MVDirection &other);

// Destructor
    ~MVDirection();
//# Operators
// Addition and subtraction
// <group>
    MVDirection &operator+=(const MVDirection &right);
    MVDirection operator+(const MVDirection &right) const;
    MVDirection &operator-=(const MVDirection &right);
    MVDirection operator-(const MVDirection &right) const;
// </group>

//# General Member Functions
// Adjust the direction cosines to a length of 1
    virtual void adjust();
// Adjust the direction cosines to a length of 1 and return the length value
    virtual void adjust(Double &res);
// Re-adjust : taken from MVPosition.
//
// Clone data
    virtual MeasValue *clone() const;
// Generate a 2-vector of angles (in rad)
    Vector<Double> get() const;

    protected:
//# Data
};

//# Global functions
// Rotate a position vector
MVDirection operator*(const RotMatrix &left, const MVDirection&right);
MVDirection  operator*(const MVDirection &left, const RotMatrix &right);

#endif
