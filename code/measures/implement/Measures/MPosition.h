//# MPosition.h: A Measure: position on Earth
//# Copyright (C) 1995,1996,1997,1998,1999
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

#if !defined(AIPS_MPOSITION_H)
#define AIPS_MPOSITION_H

//# Includes
#include <aips/aips.h>
#include <aips/Measures/MeasBase.h>
#include <aips/Measures/MeasRef.h>
#include <aips/Quanta/MVPosition.h>

//# Forward Declarations
class MPosition;
class MCPosition;
template <class M> class MeasConvert;
template <class M> class ArrayMeasColumn;
template <class M> class ROArrayMeasColumn;
template <class M> class ScalarMeasColumn;
template <class M> class ROScalarMeasColumn;

//# Typedefs

// <summary>
//  A Measure: position on Earth
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="tMeasure" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=Measure>Measure</linkto> class 
// </prerequisite>
//
// <etymology>
// </etymology>
//
// <synopsis>
// MPosition forms derived Measure class for an instant in time.
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// </motivation>
//
// <todo asof="1996/02/21">
//	<li>
// </todo>

class MPosition : public MeasBase<MVPosition, MeasRef<MPosition> > {

public:
//# Friends
// Conversion of data
    friend class MeasConvert<MPosition>;

//# Enumerations
// Types of known MPositions
// <note role=warning> The order defines the order in the translation matrix FromTo
// in the getConvert routine. Do not change the order without
// changing the array. Additions should be made before N_types, and
// an additional row and column should be coded in FromTo, and
// in showType().</note>
    enum Types {ITRF,
		WGS84,
		N_Types,
		DEFAULT=ITRF};

//# Typedefs
  // Measure value container for this class (i.e. MPosition::MVType)
  typedef class MVPosition MVType;
  // Measure conversion routines for this class (i.e. MPosition::MCType)
  typedef class MCPosition MCType;
  // Measure reference (i.e. MPosition::Ref)
  typedef class MeasRef<MPosition> Ref;
  // Measure Convert (i.e. MPosition::Convert)
    typedef class MeasConvert<MPosition> Convert;
// Measure table Columns (e.g., MPosition::ROScalarColumn)
    typedef class ROScalarMeasColumn<MPosition> ROScalarColumn;
    typedef class ScalarMeasColumn<MPosition> ScalarColumn;
    typedef class ROArrayMeasColumn<MPosition> ROArrayColumn;
    typedef class ArrayMeasColumn<MPosition> ArrayColumn;

//# Constructors
// <note role=tip> In the following constructors and other functions, all 
// <em>MeasRef</em> can be replaced with simple <src>Measure::TYPE</src>
// where no offsets or frames are needed in the reference. For reasons
// of compiler limitations the formal arguments had to be specified as
// <em>uInt</em> rather than the Measure enums that should be used as actual 
// arguments.</note>
// Default constructor; generates the ITRF centre
    MPosition();
// Create from data and reference
// <group>
    MPosition(const MVPosition &dt);
    MPosition(const MVPosition &dt, const MPosition::Ref &rf);
    MPosition(const MVPosition &dt, uInt rf);
    MPosition(const Quantity &dt, const Quantity &dt1, const Quantity &dt2);
    MPosition(const Quantity &dt, const Quantity &dt1, const Quantity &dt2,
	      const MPosition::Ref &rf);
    MPosition(const Quantity &dt, const Quantity &dt1, const Quantity &dt2,
	      uInt rf);
    MPosition(const Quantity &dt0, const Quantum<Vector<Double> > &dt);
    MPosition(const Quantity &dt0, const Quantum<Vector<Double> > &dt, 
	      const MPosition::Ref &rf);
    MPosition(const Quantity &dt0, const Quantum<Vector<Double> > &dt, 
	      uInt rf);
    MPosition(const Measure *dt);
    MPosition(const MeasValue *dt);
// </group>

  // <group>
  MPosition(const MPosition &);
  MPosition &operator=(const MPosition &);
  // </group>

//# Destructor
    virtual ~MPosition();

//# Operators

//# General Member Functions
// Tell me your type
// <group>
    virtual const String &tellMe() const;
    static const String &showMe();
    virtual uInt type() const;
    static void assert(const Measure &in);
// </group>
// Translate reference code
    static const String &showType(uInt tp);
// Translate string to reference code
// <group>
  static Bool getType(MPosition::Types &tp, const String &in);
  Bool giveMe(MPosition::Ref &mr, const String &in);
// This one for historic reasons only
  Bool giveMe(const String &in, MPosition::Ref &mr);
// </group>
  // Set the offset in the reference (False if non-matching Measure)
  virtual Bool setOffset(const Measure &in);
  // Set the reference type to the specified String. False if illegal
  // string, reference set to DEFAULT.
  virtual Bool setRefString(const String &in);
  // Get the default reference type
  virtual const String &getDefaultType() const;
  // Get a list of all known reference codes. nall returns the number in list,
  // nextra the number of specials (like planets) that should be at 
  // end of list). typ returns the list of corresponding types.
  // <group>
  virtual const String *const allTypes(Int &nall, Int &nextra,
				       const uInt *&typ) const;
  static const String *const allMyTypes(Int &nall, Int &nextra,
					const uInt *&typ);
  // </group>
  // Get the reference type (for records, including codes like R_)
  virtual String getRefString() const;
  // Get my type (as Register)
  static uInt myType();

// Get Measure data
// <group>
    Quantum<Vector<Double> > get(const Unit &inunit) const;
    Quantum<Vector<Double> > getAngle() const;
    Quantum<Vector<Double> > getAngle(const Unit &inunit) const;
// </group>

// Make copy
// <group>
    virtual Measure *clone() const;
// </group>

private:
//# Enumerations

//# Data

//# Member functions

};

#endif
