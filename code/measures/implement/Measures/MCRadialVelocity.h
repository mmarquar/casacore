//# MCRadialVelocity.h: MRadialVelocity conversion routines 
//# Copyright (C) 1995,1996,1997,1998,1999,2000,2002
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

#if !defined(AIPS_MCRADIALVELOCITY_H)
#define AIPS_MCRADIALVELOCITY_H

//# Includes
#include <aips/aips.h>
#include <aips/Measures/MeasBase.h>
#include <aips/Measures/MeasRef.h>
#include <aips/Measures/MCBase.h>
#include <aips/Measures/MConvertBase.h>
#include <aips/Measures/MRadialVelocity.h>

//# Forward Declarations
class MCRadialVelocity;
class MDoppler;
class MVPosition;
class MVDirection;
class Aberration;
class String;

//# Typedefs

// <summary> MRadialVelocity conversion routines </summary>

// <use visibility=local>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="tMeasure" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=Measure>Measure</linkto> class 
//   <li> <linkto class=MCBase>MCBase</linkto> base class
//   <li> <linkto class=MConvertBase>overall conversion</linkto>  class
// </prerequisite>
//
// <etymology>
// Measure, Convert and RadialVelocity
// </etymology>
//
// <synopsis>
// Contains state machinery and cashing for actual conversions
//
// <example>
// Get the Doppler shift for an oberved HI RadialVelocity of 100 km/s
// <srcblock>
//	#include <aips/Measures.h>
//	#include <aips/Measures/MRadialVelocity.h>
//	#include <aips/Measures/MDoppler.h>
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
// <todo asof="1997/04/14">
//   <li> look at relativistic effects
// </todo>

class MCRadialVelocity : public MCBase {
  
public:
  //# Friends
  // Conversion of data
  friend class MeasConvert<MRadialVelocity>;
  
  //# Constructors
  // Default constructor
  MCRadialVelocity();
  
  //# Destructor
  ~MCRadialVelocity();

  //# Member functions
  // Show the state of the conversion engine (mainly for debugging purposes)
  static String showState();
  
private:
  //# Enumerations
  // The list of actual routines provided.
  // <note role=warning> Each <src>AA_BB</src> in the list points to routine
  // that can be used in the FromTo list in the getConvert routine.
  // In addition the type to which each is converted should be in the
  // ToRef array, again in the proper order. </note>
  enum Routes {
    LSRD_BARY,
    BARY_LSRD,
    BARY_GEO,
    GEO_TOPO,
    GEO_BARY,
    TOPO_GEO,
    LSRD_GALACTO,
    GALACTO_LSRD,
    LSRK_BARY,
    BARY_LSRK,
    N_Routes };
  
  //# Typedefs
  
  //# Operators
  
  //# General Member Functions
  
  //# Enumerations
  
  //# Cached Data
  MVPosition *MVPOS1;
  MVDirection *MVDIR1;
  Aberration *ABERFROM;
  Aberration *ABERTO;

  //# State machine data
  // Has state matrix been made
  static Bool stateMade_p;
  // Transition list
  static uInt ToRef_p[N_Routes][3];
  // Transition matrix
  static uInt FromTo_p[MRadialVelocity::N_Types][MRadialVelocity::N_Types];
  
  //# Constructors
  // Copy constructor (not implemented)
  MCRadialVelocity(const MCRadialVelocity &other);
  // Assignment (not implemented)
  MCRadialVelocity &operator=(const MCRadialVelocity &other);
  
  //# Member functions
  // Create conversion function pointer
  virtual void getConvert(MConvertBase &mc,
			  const MRBase &inref,
			  const MRBase &outref);
  
  // Create help structures for Measure conversion routines
  virtual void initConvert(uInt which, MConvertBase &mc);
  
  // Delete the pointers used in the MeasConvert help structure cache
  virtual void clearConvert();
  
  // Routine to convert RadialVelocity from one reference frame to another
  virtual void doConvert(MeasValue &in,
			 MRBase &inref,
			 MRBase &outref,
			 const MConvertBase &mc);
  // Conversion routine to cater for inheritance question
  void doConvert(MVRadialVelocity &in,
		 MRBase &inref,
		 MRBase &outref,
		 const MConvertBase &mc);
  
};

#endif
