//# MCuvw.h: Muvw conversion routines 
//# Copyright (C) 1998,1999,2000,2002
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

#if !defined(AIPS_MCUVW_H)
#define AIPS_MCUVW_H

//# Includes
#include <aips/aips.h>
#include <aips/Measures/Muvw.h>
#include <aips/Measures/MeasBase.h>
#include <aips/Measures/MeasRef.h>
#include <aips/Measures/MCBase.h>
#include <aips/Measures/MConvertBase.h>
#include <aips/Measures/MeasMath.h>
#include <aips/Quanta/MVDirection.h>

//# Forward Declarations
class MCuvw;
class String;
template <class T> class Vector;

//# Typedefs

// <summary> Muvw conversion routines  </summary>

// <use visibility=local>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="tMuvw" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=Measure>Measure</linkto> class 
//   <li> <linkto class=MCBase>MCBase</linkto> base class
//   <li> <linkto class=MConvertBase>overall conversion</linkto>  class
// </prerequisite>
//
// <etymology>
// Measure, Convert and uvw
// </etymology>
//
// <synopsis>
// Contains state machinery and caching for actual conversions
// </synopsis>
//
// <example>
// See <linkto module=Measures>Measures</linkto> module description for
// conversion examples.
// </example>
//
// <motivation>
// </motivation>
//
// <todo asof="1998/04/21">
//	<li> Cater for EW baselines
// </todo>

class MCuvw : public MCBase { 
  
public:

  //# Friends
  // Conversion of data
  friend class MeasConvert<Muvw>;
  
  //# Constructors
  // Default constructor
  MCuvw();
  
  //# Destructor
  ~MCuvw();

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
    GAL_J2000,
    GAL_B1950,
    J2000_GAL,
    B1950_GAL,
    J2000_B1950,
    B1950_J2000,
    J2000_JMEAN,
    B1950_BMEAN,
    JMEAN_J2000,
    JMEAN_JTRUE,
    BMEAN_B1950,
    BMEAN_BTRUE,
    JTRUE_JMEAN,
    BTRUE_BMEAN,
    J2000_JNAT,
    JNAT_J2000,
    B1950_APP,
    APP_B1950,
    APP_TOPO,
    HADEC_AZEL,
    AZEL_HADEC,
    HADEC_TOPO,
    AZEL_AZELSW,
    AZELSW_AZEL,
    APP_JNAT,
    JNAT_APP,
    J2000_ECLIP,
    ECLIP_J2000,
    JMEAN_MECLIP,
    MECLIP_JMEAN,
    JTRUE_TECLIP,
    TECLIP_JTRUE,
    GAL_SUPERGAL,
    SUPERGAL_GAL,
    ITRF_HADEC,
    HADEC_ITRF,
    TOPO_HADEC,
    TOPO_APP,
    N_Routes
  };
  
  //# Typedefs
  
  //# Operators
  
  //# General Member Functions
  
  //# Enumerations
  
  //# Cached Data
  // Calculation class
  MeasMath measMath;
  // Belonging direction
  MVDirection MVDIR1;

  //# State machine data
  // Has state matrix been made
  static Bool stateMade_p;
  // Transition list
  static uInt ToRef_p[N_Routes][3];
  // Transition matrix
  static uInt FromTo_p[Muvw::N_Types][Muvw::N_Types];

  //# Constructors
  // Copy constructor (not implemented)
  MCuvw(const MCuvw &other);
  // Assignment (not implemented)
  MCuvw &operator=(const MCuvw &other);
  
  //# Member functions
  
  // Create conversion function pointer
  virtual void getConvert(MConvertBase &mc,
			  const MRBase &inref, 
			  const MRBase &outref);
  
  // Create help structures for Measure conversion routines
  virtual void initConvert(uInt which, MConvertBase &mc);
  
  // Delete the pointers used in the MeasConvert help structure cache
  virtual void clearConvert();
  
  // Routines to convert uvws from one reference frame to another
  virtual void doConvert(MeasValue &in,
			 MRBase &inref,
			 MRBase &outref,
			 const MConvertBase &mc);
  // Conversion routine to cater for inheritance question
  void doConvert(MVuvw &in,
		 MRBase &inref,
		 MRBase &outref,
		 const MConvertBase &mc);
  // Get the correct belonging direction from the frame
  // <group>
  void getAPP();
  void getJ2000();
  void getB1950();
  // </group>  
  // Rotate from direction to pole
  void toPole(MVPosition &in);
  // Rotate from pole to direction
  void fromPole(MVPosition &in);

};

#endif

