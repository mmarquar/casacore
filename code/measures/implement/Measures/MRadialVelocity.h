//# MRadialVelocity.h: A Measure: radial velocity
//# Copyright (C) 1995, 1996
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

#if !defined(AIPS_MRADIALVELOCITY_H)
#define AIPS_MRADIALVELOCITY_H

#if defined(_AIX)
#pragma implementation ("MRadialVelocity.cc")
#endif

//# Includes
#include <aips/aips.h>
#include <aips/Measures/Measure.h>
#include <aips/Measures/MeasBase.h>
#include <aips/Measures/MeasRef.h>
#include <aips/Measures/MeasConvert.h>
#include <aips/Measures/MVRadialVelocity.h>

//# Forward Declarations
class MRadialVelocity;
class MDoppler;

//# Typedefs

// <summary>
// A Measure: radial velocity
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
// MRadialVelocity is a derived Measure class for radio velocity.<br>
// An MRadialVelocity can be generated from a simple value (or an
// <linkto class=MVRadialVelocity>MVRadialVelocity</linkto> object), which is then
// interpreted as a RadialVelocity in m/s, and a reference, with an LSR type
// as default.<br>
// It can also be generated from a Quantity, where the interpretation
// depends on the dimensionality of the Quantity:
// <ul>
// <it> velocity (e.g. AU/a)
// </ul>
// The different reference types of a RadialVelocity are:
// <ul>
//  <li> MRadialVelocity::LSR --- Local Standard of Rest (J2000) -- as the
//		dynamical definition (IAU, [9,12,7] km/s in galactic coordinates
//  <li> MRadialVelocity::LSRK -- LSR as a kinematical definition -- 19.4 km/s
//		in direction ra,dec = [270,+30] deg (B1900)
//  <li> MRadialVelocity::BARY -- Barycentric (J2000)
//  <li> MRadialVelocity::GEO --- Geocentric
//  <li> MRadialVelocity::TOPO -- Topocentric
//  <li> MRadialVelocity::GALACTO -- Galacto centric (with rotation of 220 km/s
//		in direction l,b = [90,0] deg.
//  <li> MRadialVelocity::DEFAULT = LSR
// <ul>
// <p>
// Conversion between the different types is done with the standard
// <linkto class=MeasConvert>MeasConvert</linkto> class 
// (<src>MRadialVelocity::Convert</src> in this case).<br>
// Some of the conversions are only possible if frame information has been 
// filled in. The following frame information is necessary if a conversion
// goes to or from the (different) specified types:
// <ul>
//  <li><em>Epoch</em>: TOPO, GEO
//  <li><em>Position</em>: TOPO
//  <li><em>Direction</em> all
// </ul>
// <br>
// <note role=caution> For large radial velocities (of order c) the conversions are
// not precise, and not completely reversable, due to unknown transverse
// velocities, and the additive way in which corrections are done. They
// are correct to first order wrt relativistic effects
// </note>
// An MRadialVelocity can be created from an
// <linkto class=MDoppler>MDoppler</linkto> 
// by the <src>fromDoppler()</src> member. It can be converted to an MDoppler
// with the <src>toDoppler()</src>. Comparable methods are available
// for <linkto class=MFrequency>MFrequency</linkto> as 
// <src>toRadial()</src> and <src>fromRadial</src>.<br>
// </synopsis>
//
// <example>
// Get the Doppler shift for an oberved HI RadialVelocity of 100 km/s
// <srcblock>
//	cout << "Redshift for 100 km/s: " <<
//		MDoppler::Convert( MRadialVelocity( Quantity(100., "km/s"),
//				      MRadialVelocity::TOPO).toDoppler(QC::HI),
//				   MDoppler::Z)() << endl;
// </srcblock>				   
// </example>
//
// <motivation>
// </motivation>
//
// <todo asof="1996/10/14">
//   <li> test sign of conversions independently
//   <li> look at relativistic effects
// </todo>

class MRadialVelocity : public MeasBase<MVRadialVelocity,MeasRef<MRadialVelocity> >
{
public:
//# Friends
// Conversion of data
    friend class MeasConvert<MRadialVelocity,MVRadialVelocity>;

//# Enumerations
// Types of known MRadialVelocity
// <note> The order defines the order in the translation matrix FromTo
// in the getConvert routine. Do not change the order without
// changing the array. Additions should be made before N_types, and
// an additional row and column should be coded in FromTo, and
// in showType().</note>
    enum Types {LSR,
		LSRK,
		BARY,
		GEO,
		TOPO,
		GALACTO,
		N_Types,
		DEFAULT=LSR};

// The list of actual routines provided.
// <note> For each <src>AA_BB</src> in the list a routine
// <src>static void AAtoBB(MVRadialVelocity &)</src> should be provided. The routines
// should be listed in the FromToRout array in the getConvert routine, in the
// order specified. In addition the type to which converted should be in the
// ToRef array, again in the proper order. </note>
    enum Routes {
	LSR_BARY,
	BARY_LSR,
	BARY_GEO,
	GEO_TOPO,
	GEO_BARY,
	TOPO_GEO,
	LSR_GALACTO,
	GALACTO_LSR,
	LSRK_BARY,
	BARY_LSRK,
	N_Routes };

//# Typedefs
// Measure reference
    typedef MeasRef<MRadialVelocity> Ref;
// Measure conversion use
    typedef MeasConvert<MRadialVelocity,MVRadialVelocity> Convert;

//# Constructors
// <note> In the following constructors and other functions, all 
// <em>MeasRef</em> can be replaced with simple <src>Measure::TYPE</src>
// where no offsets or frames are needed in the reference. For reasons
// of compiler limitations the formal arguments had to be specified as
// <em>uInt</em> rather than the Measure enums that should be used as actual 
// arguments.</note>
// Default constructor; generates a zero rest RadialVelocity
    MRadialVelocity();
// Create from data and reference
// <group>
    MRadialVelocity(const MVRadialVelocity &dt);
    MRadialVelocity(const MVRadialVelocity &dt, const MRadialVelocity::Ref &rf);
    MRadialVelocity(const MVRadialVelocity &dt, uInt rf);
    MRadialVelocity(const Quantity &dt);
    MRadialVelocity(const Quantity &dt, const MRadialVelocity::Ref &rf);
    MRadialVelocity(const Quantity &dt, uInt rf);
// </group>

//# Destructor
    ~MRadialVelocity();

//# Operators

//# General Member Functions
// Tell me your type
// <group>
    virtual const String &tellMe() const;
    static const String &showMe();
// </group>
// Translate reference code
    static const String &showType(uInt tp);
// Translate string to reference code
    Bool giveMe(const String &in, MRadialVelocity::Ref &mr);
// Get radial velocity in specified units
    Quantity get(const Unit &un) const;

// Make a Doppler velocity (as an MDoppler::BETA default) from the RadialVelocity.
// <group>
    MDoppler toDoppler();
// </group>

// Make a RadialVelocity from the Doppler velocity (assuming LSR default)
// <group>
    static MRadialVelocity fromDoppler(const MDoppler &dop);
    static MRadialVelocity fromDoppler(const MDoppler &dop,
				       MRadialVelocity::Types typ);
// </group>

// Make a copy
    virtual void *clone() const;

private:
//# Enumerations
// Usage of the MeasConvert structure cache. Additions should fit into the
// space provided in MeasConvert (see <src>MC_N_Struct</src> constant),
// and should be coded in the <src>clearConvert()</src> method.
    enum StructUse {
	MVPOS1, MVDIR1,
	ABERFROM, ABERTO,
	N_StructUse };

//# Data

//# Member functions
// Create conversion function pointer
    static void getConvert(MRadialVelocity::Convert &mc,
			   const MRadialVelocity::Ref &inref, 
			   const MRadialVelocity::Ref &outref);

// Create help structures for Measure conversion routines
    static void initConvert(uInt which, MRadialVelocity::Convert &mc);

// Delete the pointers used in the MeasConvert help structure cache
     static void clearConvert(MRadialVelocity::Convert &mc);

// Routine to convert RadialVelocity from one reference frame to another
    static void doConvert(MVRadialVelocity &in,
			  const MRadialVelocity::Ref &inref,
			  const MRadialVelocity::Ref &outref,
			  const MRadialVelocity::Convert &mc);

};

#endif
