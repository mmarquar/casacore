//# UVWMachine.h: Converts UVW coordinates between coordinate systems 
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
//# $Id$
#if !defined(AIPS_UVWMACHINE_H)
#define AIPS_UVWMACHINE_H

//# Includes
#include <aips/aips.h>
#include <aips/Measures.h>
#include <aips/Measures/MCDirection.h>
#include <aips/Measures/MVPosition.h>
#include <aips/Measures/RotMatrix.h>

//# Forward Declarations
class MeasFrame;
template <class T> class Vector;

// <summary>
// Converts UVW coordinates between coordinate systems 
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="tUVWMachine.cc" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=FTMachine>FTMachine</linkto> class
//   <li> <linkto class=MDirection>MDirection</linkto> class
// </prerequisite>
//
// <etymology>
// From UVW coordinates and machinery
// </etymology>
//
// <synopsis>
// The construction of a UVWMachine class object creates a machine that can
// convert UVW coordinates from one coordinate system to another. In addition
// it can also supply the phase rotation necessary to move to a new position.
//
// The constructors need an input 
// <linkto class=MDirection>MDirection</linkto>  to specify the input UVW
// coordinates reference direction and coordinate system.
// An EW flag can be specified to indicate the different type of UVW 
// coordinates (not implemented yet).
//
// The constructors also need an output coordinate system
// (<linkto class=MeasRef>MDirection::Ref</linkto>) or an output
// MDirection (including the reference coordinate system). The first case
// indicates that the real position on the sky does not change (only the
// coordinate system used); the second indicates that the UVW should be
// for a new position on the sky (in addition to maybe a different
// coordinate system). In the first case only the UVW coordinates will change,
// in the second the UVW positions will change, but also the phase of the
// observed data will have to change. For some conversions a reference
// <linkto class=MeasFrame>frame</linkto> is needed (to indicate e.g. the
// position or time). This frame can be part of one of the constructor
// references, but can also be specified separately in the constructor. A
// change in the frame parameter can be made outside the UVWMachine
// by e.g. <em>frame.reset(anMEpoch)</em>.
// <note role=caution>
// If the frame is changed by the user of the conversion machine, the
// machine has to be reinitialised before using it for output by using
// <em>reCalculate()</em>.
// </note>
// The output of the machine is either a set of rotation matrices that can
// be used to convert UVW coordinates (and, if necessary, phases); or the
// UVW conversion and actual phase can be calculated from a given
// UVW coordinate (or set of coordinates).
// <note role=tip> Since e.g. in an EW interferometer (or any set of baselines
// on a line) the phase correction and UVW transform scales with the length
// of the baseline, conversion of a nominal (say 1m) baseline suffices to
// easily calculate others. The same is true for baselines in a plane,
// where a conversion of two orthogonal baselines in that plane will suffice.
// </note>
// </synopsis>
//
// <example>
// <srcblock>
//	// Given a current phase stopping Centre
//	MDirection indir(Quantity(3.25745692, "rad"),
//		   	 Quantity(0.040643336,"rad"),
//		   	 MDirection::Ref(MDirection::B1950));
//	// Conversion to J2000 is set by:
//	UVWMachine uvm(MDirection::Ref(MDirection::J2000), indir);
//	// The rotation matrix to go to new UVW is obtained by:
//	RotMatrix rm(uvm.rotationUVM());
//	// If an UVW specified:
//	MVPosition uvw((-739.048461, -1939.10604, 1168.62562);
//	// This can be converted by e.g.:
//	uvw *= rm;
//	// Or, alternatively, by e.g.:
//	uvm.convertUVW(uvw);
// </srcblock>
// </example>
//
// <motivation>
// To aid making maps in different coordinate systems
// </motivation>
//
// <todo asof="1998/01/21">
//   <li> add EW UVW coordinates
//   <li> check if non right-handed coordinates systems (like AzEl) are
//	handled correctly
//   <li> provide a MVuvw and Muvw class to cater for second order effects
//		appropiately
// </todo>

class UVWMachine {
public:
//# Constructors
  // Construct a UVW conversion machine from the in coordinate and its
  // system to the out coordinate system (output absolute direction 
  //remains the same)
  UVWMachine(const MDirection::Ref &out, const MDirection &in,
	     Bool EW=False);
  // Construct a UVW conversion machine from the in coordinate and its
  // system to the out coordinate and its system
  UVWMachine(const MDirection &out, const MDirection &in,
	     Bool EW=False);
  // Construct UVW conversion machine with an explicitly given frame
  // <group>
  UVWMachine(const MDirection::Ref &out, const MDirection &in,
	     const MeasFrame &frame, Bool EW=False);
  UVWMachine(const MDirection &out, const MDirection &in, 
	     const MeasFrame &frame, Bool EW=False);
  // </group>
  // Copy constructor
  UVWMachine(const UVWMachine &other);
  // Copy assignments
  UVWMachine &operator=(const UVWMachine &other);

  //# Destructor
  ~UVWMachine();

  //# Operators
  // Return converted UVW coordinates
  // <group>
  Vector<Double> operator()(const Vector<Double> &uv) const;
  Vector<Vector<Double> > operator()(const Vector<Vector<Double> > &uv) const;
  MVPosition operator()(const MVPosition &uv) const;
  Vector<MVPosition > operator()(const Vector<MVPosition > &uv) const;
  // </group>

  //# Member functions
  // Return the new phase centre coordinates
  const MDirection &phaseCenter() const;
  // Return a rotation matrix that can be used to convert UVW coordinates:
  // UVW(new) = UVW(old) * rotationUVW()
  const RotMatrix &rotationUVW() const;
  // Return a position vector that can produce the phase correction:
  // dPhase = rotationPhase * UVW(new)
  const MVPosition &rotationPhase() const;
  // replace UVW with converted values
  // <group>
  void convertUVW(Vector<Double> &uv) const;
  void convertUVW(Vector<Vector<Double> > &uv) const;
  void convertUVW(MVPosition &uv) const;
  void convertUVW(Vector<MVPosition > &uv) const;
  // </group>
  // Get phase shift (in implied units of UVW), and change input uvw as well
  // <group>
  Double getPhase(Vector<Double> &uv) const;
  Vector<Double> getPhase(Vector<Vector<Double> > &uv) const;
  Double getPhase(MVPosition &uv) const;
  Vector<Double> getPhase(Vector<MVPosition > &uv) const;
  // </group>
  // Replace UVW with converted, and return phase
  // <group>
  void convertUVW(Double &phase, Vector<Double> &uv) const;
  void convertUVW(Vector<Double> &phase, Vector<Vector<Double> > &uv) const;
  void convertUVW(Double &phase, MVPosition &uv) const;
  void convertUVW(Vector<Double> &phase, Vector<MVPosition> &uv) const;
  // </group>

  // Recalculate the parameters for the machine after e.g. a frame change
  void reCalculate();

private:

  //# Data
  // EW flag
  Bool ew_p;
  // Zero phase flag (for speed)
  Bool zp_p;
  // Old phase centre
  MDirection in_p;
  // New coordinate reference
  MDirection::Ref outref_p;
  // Old phase centre in new coordinates
  MDirection outin_p;
  // New phase centre
  MDirection out_p;
  // Rotation Matrix to go from input UVW to coordinate system
  RotMatrix rot1_p;
  // Rotation matrix to go from old system to new system
  RotMatrix rot2_p;
  // Rotation Matrix to go from new coordinate system to output UVW
  RotMatrix rot3_p;
  // UVW rotation
  RotMatrix uvrot_p;
  // Phase rotation
  MVPosition phrot_p;
  // Conversion engine
  MDirection::Convert conv_p;

  //# Constructors
  // default constructor: not implemented
  UVWMachine();

  //# Private Member Functions
  // Initialise machinery
  void init();
  // Copy data members
  void copy(const UVWMachine &other);
};

#endif


