//# SpectralElement.h: Describes (a set of related) spectral lines
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

#if !defined(TRIAL_SPECTRALELEMENT_H)
#define TRIAL_SPECTRALELEMENT_H

//# Includes
#include <aips/aips.h>
#include <aips/Arrays/Vector.h>
#include <aips/Utilities/RecordTransformable.h>

//# Forward Declarations
class String;
class GlishRecord;
#include <aips/iosfwd.h>

// <summary>
// Describes (a set of related) spectral lines
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="tSpectralFit" demos="">
// </reviewed>

// <prerequisite>
//   <li> <ahlink module=Functionals>Functionals</ahlink> module
//   <li> <linkto class=RecordInterface>RecordInterface</linkto> class
// </prerequisite>
//
// <etymology>
// From spectral line and element
// </etymology>
//
// <synopsis>
// The SpectralElement class is a container for a spectral line descriptor.
// It can contain a single line (like a Gaussian profile), or a set of
// related lines (like a doublet or so).
//
// The element can be used in the
// <ahlink class=SpectralFit>SpectralFit</ahlink> class and in the
// <ahlink class=SpectralEstimate>SpectralEstimate</ahlink> class.
//
// The default type is a Gaussian, defined as:
// <srcblock>
//	AMPL.exp[ -(x-CENTER)<sup>2</sup>/2 SIGMA<sup>2</sup>]
// </srcblock>
//
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// To have a contained fitting of spectral profiles to an observed spectrum
// </motivation>
//
// <todo asof="2001/02/04">
//   <li> add more profile types
// </todo>

class SpectralElement : public RecordTransformable {
 public:

  //# Enumerations
  // Types of spectral lines known
  enum Types {
    // A gaussian profile
    GAUSSIAN,
    // A polynomial baseline
    POLYNOMIAL,
    N_Types
  };

  //# Friends

  //# Constructors
  // Default constructor creates a default Gaussian element with an amplitude
  // of 1; an integral <src>(sigma=2sqrt(ln2)/pi)</src> of 1;
  // a central frequency of zero.
  SpectralElement();
  // Construct with given type and values
  // <thrown>
  //   <li> AipsError if sigma == 0.0
  //   <li> AipsError if type not GAUSSIAN
  // </thrown>
  SpectralElement(SpectralElement::Types tp, const Double ampl,
		  const Double center, const Double sigma);

  // Construct an n-degree polynomial
  explicit SpectralElement(const uInt n);
  // Construct the given tp with the given param
  // <thrown>
  //   <li> AipsError if incorrect number of parameters (e.g. not 3 for GAUSSIAN)
  //   <li> AipsError if sigma == 0.0
  // </thrown>
  SpectralElement(SpectralElement::Types tp, const Vector<Double> &param);
  // Copy constructor (deep copy)
  // <thrown>
  //   <li> AipsError if sigma == 0.0
  // </thrown>
  SpectralElement(const SpectralElement &other);

  //#Destructor
  // Destructor
  ~SpectralElement();

  //# Operators
  // Assignment (copy semantics)
  // <thrown>
  //   <li> AipsError if sigma == 0.0
  // </thrown>
  SpectralElement &operator=(const SpectralElement &other);
  // Evaluate the value of the element at x
  Double operator()(const Double x) const;

  //# Member functions
  // Get all the types available as String and codes, and number available
  static const String *const allTypes(Int &nall,
				      const SpectralElement::Types *&typ);
  // Get a string from the type
  static const String &fromType(SpectralElement::Types tp);
  // Get a type from a (non-case sensitive; minimum match) String
  static Bool toType(SpectralElement::Types &tp,
		     const String &typName);

  // Get the data for this element
  // <thrown>
  //  <li> AipsError if element does not have data
  //	   (e.g. amplitude for POLYNOMIAL)
  // </thrown>
  // <group>
  // Get type of this element
  SpectralElement::Types getType() const { return tp_p; }
  // Get amplitude
  Double getAmpl() const;
  // Get center value
  Double getCenter() const;
  // Get the width
  // <group>
  Double getSigma() const;
  Double getFWHM() const;
  // </group>
  // Get the degree of e.g. polynomial
  uInt getDegree() const;
  // </group>

  // Set data for element
  // <group>
  // Set all data
  // <thrown>
  //   <li> AipsError if incorrect number of parameters (e.g. not 3 for GAUSSIAN)
  //   <li> AipsError if sigma == 0.0
  // </thrown>
  void set(SpectralElement::Types tp, const Vector<Double> &param);
  // Set amplitude
  // <thrown>
  //   <li> AipsError if non GAUSSIAN
  // </thrown>
  void setAmpl(Double ampl);
  // Set center
  // <thrown>
  //   <li> AipsError if non GAUSSIAN
  // </thrown>
  void setCenter(Double center);
  // Set width
  // <thrown>
  //   <li> AipsError if non GAUSSIAN
  //   <li> AipsError if sigma == 0.0
  // </thrown>
  void setSigma(Double sigma);
  // </group>
  // Set degree
  // <thrown>
  //   <li> AipsError if non POLYNOMIAL
  // </thrown>
  void setDegree(uInt n);

  // Construct from record.  Must hold fields "type" (String) and 
  // "parameters" (Vector<Double>).  For type=GAUSSIAN, parameters
  // holds amplitude, center and sigma. For type=POLYNOMIAL,
  // parameters(0) holds the degree.
  static SpectralElement* fromRecord(const RecordInterface &container);

  // Create a SpectralElement from a record.
  // An error message is generated, and False
  // returned if an invalid record is given. A valid record will return True.
  // A valid record contains the following fields (any additional fields are
  // ignored):
  // <ul>
  // <li> type = TpString: type of element (gaussian etc; case
  //	 insensitive)
  // <li> parameters = TpVector(Double): one or more values giving the parameters
  //		for the type
  // </ul>
  // A SpectralElement can be created from a string. In that case the string
  // will only indicate the type of element (like gaussian), and will
  // create a default element of that given type.
  // Error messages are postfixed to error.
  // <group>
  virtual Bool fromRecord(String &error, const RecordInterface &in);
  virtual Bool fromString(String &error, const String &in);
  Bool fromRecord(String &error, const GlishRecord &in);
  // </group>

  // Save to a record.  The return will be False and an error
  // message generated only if the SpectralElement is illegal (could not happen)
  // Error messages are postfixed to error.
  // <group>
  virtual Bool toRecord(String &error, RecordInterface &out) const;
  Bool toRecord(String &error, GlishRecord &out) const;
  // </group>

  // Get the identification of a record
  virtual const String &ident() const;

 private:
  //#Data
  // type of element
  SpectralElement::Types tp_p;
  // A number (like polynomial degree or number of doublet lines)
  uInt n_p;
  // The parameters of the function. I.e. the polynomial coefficients;
  // amplitude, center and sigma of a Gaussian.
  Vector<Double> par_p;

  //# Member functions
  // Check if GAUSSIAN type
  // <thrown>
  //   <li> AipsError if non-Gaussian
  // </thrown>
  void SpectralElement::checkGauss() const;
  // Check if POLYNOMIAL type
  // <thrown>
  //   <li> AipsError if non-polynomial
  // </thrown>
  void SpectralElement::checkPoly() const;
  // Check if sigma non-equal to zero and positive if a GAUSSIAN
  // <thrown>
  //   <li> AipsError if illegal sigm
  // </thrown>
  void SpectralElement::check() const;

};

//# Global functions
// <summary> Global functions </summary>
// <group name=Output>
// Output declaration
ostream &operator<<(ostream &os, const SpectralElement &elem);
// </group>

#endif


