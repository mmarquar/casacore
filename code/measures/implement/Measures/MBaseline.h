//# MBaseline.h: A Measure: Baseline on Earth
//# Copyright (C) 1998
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

#if !defined(AIPS_MBASELINE_H)
#define AIPS_MBASELINE_H

//# Includes
#include <aips/aips.h>
#include <aips/Measures/MeasBase.h>
#include <aips/Measures/MeasRef.h>
#include <trial/Measures/MVBaseline.h>

//# Forward Declarations
class MBaseline;
class MCBaseline;
template <class M, class F, class MC> class MeasConvert;

//# Typedefs

// <summary> A Measure: Baseline on Earth </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="tMBaseline" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=Measure>Measure</linkto> class 
// </prerequisite>
//
// <etymology>
// From Measure and Baseline
// </etymology>
//
// <synopsis>
// MBaseline forms derived Measure class for an interferometer baseline.
// Baselines can be given in any of the direction types, or as ITRF, the 
// IERS base.<br>
// Note that at the moment no correction for Earth tides (error <~ 0.05 mm/km
// EW baseline), plate motion (not relevant for telescopes on same plate) and
// relativistic effects are incorporated. B1950 has the same caveat as in
// <linkto class=MDirection>MDirection</linkto>.
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// </motivation>
//
// <todo asof="1998/04/20">
//	<li> add some Earth tide model
// </todo>

class MBaseline : public MeasBase<MVBaseline, MeasRef<MBaseline> > {

public:
//# Friends
// Conversion of data
    friend class MeasConvert<MBaseline, MVBaseline, MCBaseline>;

//# Enumerations
// Types of known MBaselines
// <note role=warning>
// The order defines the order in the translation matrix FromTo
// in the getConvert routine. Do not change the order without
// changing the array. Additions should be made before N_types, and
// an additional row and column should be coded in FromTo, and
// in showType().</note>
    enum Types {
      ITRF,
      J2000,
      JMEAN,
      JTRUE,
      APP,
      B1950,
      BMEAN,
      BTRUE,
      GALACTIC,
      HADEC,
      AZEL,
      AZELSW,
      JNAT,
      ECLIPTIC,  
      MECLIPTIC,
      TECLIPTIC,
      SUPERGAL,
      N_Types,
      // Defaults
      DEFAULT=ITRF,
      // Synonyms
      AZELNE=AZEL};

//# Typedefs
// Measure reference
    typedef class MeasRef<MBaseline> Ref;
// MeasConvert use
    typedef class MeasConvert<MBaseline,MVBaseline,MCBaseline> Convert;

//# Constructors
// <note role=tip> In the following constructors and other functions, all 
// <em>MeasRef</em> can be replaced with simple <src>Measure::TYPE</src>
// where no offsets or frames are needed in the reference. For reasons
// of compiler limitations the formal arguments had to be specified as
// <em>uInt</em> rather than the Measure enums that should be used as actual 
// arguments.</note>
// Default constructor; generates the ITRF centre
    MBaseline();
// Create from data and reference
// <group>
    MBaseline(const MVBaseline &dt);
    MBaseline(const MVBaseline &dt, const MBaseline::Ref &rf);
    MBaseline(const MVBaseline &dt, uInt rf);
    MBaseline(const Measure *dt);
    MBaseline(const MeasValue *dt);
// </group>

  // <group>
  MBaseline(const MBaseline &);
  MBaseline &operator=(const MBaseline &);
  // </group>

//# Destructor
    ~MBaseline();

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
  static Bool getType(MBaseline::Types &tp, const String &in);
  Bool giveMe(MBaseline::Ref &mr, const String &in);
// </group>
  // Set the offset in the reference (False if non-matching Measure)
  virtual Bool setOffset(const Measure &in);
  // Set the reference type to the specified String. False if illegal
  // string, reference set to DEFAULT.
  virtual Bool setRefString(const String &in);
  // Get the default reference type
  virtual const String &getDefaultType() const;
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
