//# GaussianShape.h:
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

#if !defined(AIPS_GAUSSIANSHAPE_H)
#define AIPS_GAUSSIANSHAPE_H

#include <aips/aips.h>
#include <trial/ComponentModels/ComponentShape.h>
#include <trial/ComponentModels/ComponentType.h>
#include <aips/Measures/MDirection.h>
#include <aips/Measures/MVDirection.h>
#include <aips/Functionals/Gaussian2D.h>

class MVAngle;
class RecordInterface;
class String;
class doubleG_COMPLEX;
typedef doubleG_COMPLEX DComplex;
template <class T> class Vector;

// <summary>A Gaussian model for the spatial distribution of emission</summary>

// <use visibility=export> 

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=ComponentShape>ComponentShape</linkto>
// </prerequisite>

// <synopsis> 

// A GaussianShape models the spatial distribution of radiation from the sky as
// a two-dimensional Gaussian function with user specified major axis width,
// minor axis width and position angle.

// The reference direction is defined in celestial co-ordinates, using a
// <linkto class=MDirection>MDirection</linkto> object. It indicates where the
// centre of the Gaussian is on the sky. The direction can be specified both in
// the constructor or with the <src>setRefDirection</src> function.

// The width of the Gaussian is defined as the angle subtended by the full
// width at half maximum of the Gaussian. The major axis has the larger width
// and is aligned North-South when the position angle is zero. A positive
// position angle moves the Northern side of the component to the East.  The
// axial ratio is the ratio of the minor to major axis widths. The major axis
// MUST not be smaller than the minor axis otherwise an AipsError is thrown.

// The flux, or integrated intensity, is always normalised to one. This class
// does not model the actual flux or its variation with frequency. It solely
// models the way the emission varies with position on the sky.

// The <src>scale</src> member function is used to sample the component at any
// point on the sky. The scale factor calculated by this function is the
// proportion of the flux that is within a specified pixel size centered on the
// specified direction. Ultimatly this function will integrate the emission
// from the Gaussian over the entire pixel but currently it just assumes the
// flux can be calculated by the height of the Gaussian at the centre of the
// pixel scaled by the pixel area. This is <em>NOT<\em> accurate for Gaussians
// whose width is small compared with the pixel size.

// This class contains functions that return the Fourier transform of the
// component at a specified spatial frequency. There are described more fully
// in the description of the <src>visibility</src> functions below.

// This class also contains functions which perform the conversion between
// records and GaussianShape objects. This defines how a GaussianShape object
// is represented in glish. The format of the record that is generated and
// accepted by these functions is:
// <srcblock>
// c := [type = "gaussian",
//       direction = [type = "direction",
//                    refer = "j2000",
//                    m0 = [value = 0, unit = "deg"]
//                    m1 = [value = 0, unit = "deg"]
//                   ]
//      ]
// </srcblock>
// The direction field contains a record representation of a direction measure
// and its format is defined in the Measures module. Its refer field defines
// the reference frame for the direction and the m0 and m1 fields define the
// latitude and longitude in that frame.

// </synopsis>
//
// <example>
// Suppose I had an image of a region of the sky and we wanted to subtract
// a gaussian source from it. This could be done as follows:
// <ul> 
// <li> Construct a GaussianCompRep to represent the gaussian source
// <li> Project the component onto an image
// <li> Convolve the image by the gaussian spread function
// <li> subtract the convolved model from the dirty image.
// </ul>
// Shown below is the code to perform the first two steps in this process. See
// the <linkto class=Convolver>Convolver</linkto> class and the
// <linkto module=Lattice>Lattice</linkto> module for the functions necessary
// to perform the last two items. This example is also available in the
// <src>dGaussianCompRep.cc</src> file.
// <srcblock>
// Quantity J1934_ra = Quantity(19.0/24*360, "deg") + Quantity(39, "'");
// Quantity J1934_dec = Quantity(-63, "deg") + Quantity(43, "'");
// MDirection J1934_dir(J1934_ra, J1934_dec, MDirection::J2000);
// ComponentFlux<Double> J1934_flux(6.28, 0.1, 0.15, 0.01);
// GaussianCompRep J1934(J1934_flux, J1934_dir);
// // This component can now be projected onto an image
// CoordinateSystem coords;
// {
//   Double pixInc = Quantity(1, "''").getValue("rad");
//   Matrix<Double> xform(2,2);
//   xform = 0.0; xform.diagonal() = 1.0;
//   Double refPixel = 32.0;
//   DirectionCoordinate dirCoord(MDirection::J2000,
// 				 Projection(Projection::SIN),
// 				 J1934_ra.getValue("rad"),
// 				 J1934_dec.getValue("rad"),
// 				 pixInc , pixInc, xform,
// 				 refPixel, refPixel);
//   coords.addCoordinate(dirCoord);
// }
// CoordinateUtil::addIQUVAxis(coords);
// CoordinateUtil::addFreqAxis(coords);
// PagedImage<Float> skyModel(IPosition(4,64,64,4,8), coords, 
// 			     "model_tmp.image");
// skyModel.set(0.0f);
// J1934.project(skyModel);
// </srcblock>
// </example>
//
// <todo asof="1997/07/16">
//   <li> Nothing so far
// </todo>

// <linkfrom anchor="GaussianCompRep" classes="SkyCompRep GaussianComponent">
//  <here>GaussianCompRep</here> - a gaussian component with copy semantics
// </linkfrom>

class GaussianShape: public ComponentShape
{
public:
  // The default GaussianShape is at the J2000 North Pole. with a full width at
  // half maximum (FWHM) on both axes of 1 arc-min.
  GaussianShape();


  // Construct a Gaussian component with specified direction, width and
  // position angle (North through East).
  // <group>
  GaussianShape(const MDirection & direction, const MVAngle & majorAxis,
		const MVAngle & minorAxis, const MVAngle & positionAngle);
  GaussianShape(const MDirection & direction, const MVAngle & width,
		const Double axialRatio, const MVAngle & positionAngle);
  // </group>

  // The copy constructor uses copy semantics.
  GaussianShape(const GaussianShape & other);

  // The destructor does nothing.
  virtual ~GaussianShape();

  // The assignment operator uses copy semantics.
  GaussianShape & operator=(const GaussianShape & other);

  // get the shape of the component. This function always returns
  // ComponentType::GAUSSIAN.
  virtual ComponentType::Shape shape() const;

  // set/get the reference direction of the gaussian.
  // <group>
  virtual void setRefDirection(const MDirection & newRefDir);
  virtual const MDirection & refDirection() const;
  // </group>

  // set/get the width and orientation of the Gaussian. The width of the major
  // and minor axies is the full width at half maximum. The position angle is
  // measured North through East ie a position angle of zero degrees means
  // that the major axis is North-South and a position angle of 10 degrees
  // moves the Northern edge to the East. The axial ratio is the ratio of the
  // minor to major axes widths. Hence it is always between zero and one.
  // <group>
  void setWidth(const MVAngle & majorAxis, const MVAngle & minorAxis, 
		const MVAngle & positionAngle);
  void setWidth(const MVAngle & majorAxis, const Double axialRatio, 
		const MVAngle & positionAngle);
  void width(MVAngle & majorAxis, MVAngle & minorAxis,
	     MVAngle & positionAngle) const;
  void width(MVAngle & majorAxis, Double & axialRatio,
	     MVAngle & positionAngle) const;
  void majorAxis(MVAngle & majorAxis) const;
  MVAngle majorAxis() const;
  void minorAxis(MVAngle & minorAxis) const;
  MVAngle minorAxis() const;
  void axialRatio(Double & axialRatio) const;
  Double axialRatio() const;
  void positionAngle(MVAngle & positionAngle) const;
  MVAngle positionAngle() const;
  // </group>

  // Return the scaling factor that indicates the proportion of the flux that
  // is in the specified pixel in the specified direction. The pixels are
  // assumed to be square.
  //
  // Curently this function does <em>NOT<\em> integrate the Gaussian over the
  // area of the sky subtended by the pixel. Instead it simply samples the
  // Gaussian at the centre of the pixel and scales by the pixel area. This is
  // satisfactory for Gaussians that are large compared with the size of the
  // pixel. This function will be updated to deal with small Gaussians sometime
  // in the future.
  virtual Double scale(const MDirection & direction, 
		       const MVAngle & pixelSize) const;

  // Return the Fourier transform of the component at the specified gaussian in
  // the spatial frequency domain. The point is specified by a 3 element vector
  // (u,v,w) that has units of meters and the frequency of the observation, in
  // Hertz. These two quantities can be used to derive the required spatial
  // frequency <src>(s = uvw*freq/c)</src>. The w component is not used in
  // these functions.

  // The reference position for the transform is the direction of the
  // component. As this component is symmetric about this point the transform
  // is always a real value.
  // <group>
  virtual void visibility(DComplex & result, const Vector<Double> & uvw,
			  const Double & frequency) const;
  virtual Double visibility(const Vector<Double> & uvw,
			    const Double & frequency) const;
  // </group>

  // always returns True as a Gaussian source is symmetric.
  virtual Bool isSymmetric() const;

  // set/get the shape parameters associated with the Gaussian. There are three
  // these being in order: the major axis, the minor axis and the position
  // angle. All these angular quantities are specified in radians. The Vector
  // supplied to the setParameters function must have three elements and the
  // Vector supplied to and returned by the parameters function will have three
  // elements.
  // <group>
  virtual uInt nShapeParameters() const;
  virtual void setShapeParameters(const Vector<Double> & newParms);
  virtual void shapeParameters(Vector<Double> & compParms) const;
  // </group>

  // This functions convert between a record and a GaussianShape. These
  // functions define how a GaussianShape is represented in glish and this is
  // detailed in the synopsis above.  They return False if the record is
  // malformed and append an error message to the supplied string giving the
  // reason.
  // <group>
  virtual Bool fromRecord(String & errorMessage,
			  const RecordInterface & record);
  virtual Bool toRecord(String & errorMessage,
			RecordInterface & record) const;
  // </group>

  // Function which checks the internal data of this class for correct
  // dimensionality and consistent values. Returns True if everything is fine
  // otherwise returns False.
  virtual Bool ok() const;

private:
  void updateFT();
  MDirection itsDir;
  MVDirection itsDirValue;
  MDirection::Types itsRefFrame;
  Gaussian2D<Double> itsShape;
  Gaussian2D<Double> itsFT;
};
#endif
