//# ComponentShape.h:
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

#if !defined(AIPS_COMPONENTSHAPE_H)
#define AIPS_COMPONENTSHAPE_H

#include <aips/aips.h>
#include <trial/ComponentModels/ComponentType.h>

class MDirection;
class MVAngle;
class RecordInterface;
class String;
class doubleG_COMPLEX;
typedef doubleG_COMPLEX DComplex;
template <class T> class Vector;

// <summary>Base class for component shapes</summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=MDirection>MDirection</linkto>
// </prerequisite>
//
// <synopsis>

// This abstract base class defines the interface for different classes which
// specify the shape of a component. The most fundamental derived class is the
// <linkto class=PointShape>point</linkto> shape class but the <linkto
// class=GaussianShape>Gaussian</linkto> shape is also available. These classes
// model the spatial shape of emission from the sky. Classes derived from the
// <linkto class=SpectralModel>SpectralModel</linkto> class are used to model
// the spectral characteristics.

// This class parameterises all possible shapes with two quantities.
// <dl>
// <dt> A reference direction. 
// <dd> This is specified using an <linkto class=MDirection>MDirection</linkto>
//      and indicates the direction on the sky of some defined reference point
//      within the shape. Usually this reference point is the centre of the
//      shape.
// <dt> A Vector of parameters.
// <dd> This contains other parameters that the are defined differently for
//      different shapes. The length of the vector may vary for different
//      parameter shapes. 
// </dl>
// 
// The basic operation of classes using this interface is to determine the
// flux as a function of direction on the sky. Classes derived from this one
// assume that the Flux (or integrated intensity) of the component they
// represent is always one. The scale functions in this class then calculate
// the proprtion of the flux that is inclosed within a pixel of specified size
// centred in a specified direction. This scale factor can be multiplied by the
// actual flux to determine the flux from the specified pixel.

// The interface also defines functions which calculate the analytic Fourier
// transform of the component at any specified spatial frequency.

// </synopsis>

// <example> 
// Because ComponentShape is an abstract base class, an actual instance of this
// class cannot be constructed. However the interface it defines can be used
// inside a function. This is always recommended as it allows functions which
// have ComponentShapes as arguments to work for any derived class.
// <srcblock>
// void printShape(const ComponentShape & theShape) {
//   cout << "This is a " 
//        << ComponentType::name(theShape.shape())
//        << " shape with a reference direction of "
//        << theShape.direction() << endl;
// }
// </srcblock>
// </example>

// <motivation>
// The Shape base class was split from the SkyCompRep base class so that mixing
// components with different spatial and spectral shapes did not result in a
// combinatorial explosion in the number of classes required.
// </motivation>
//
// <todo asof="1998/03/01">
//   <li> Get the project function working again.
// </todo>

class ComponentShape
{
public:
  // a virtual destructor is needed so that the actual destructor in the
  // derived class will be used.
  virtual ~ComponentShape();

  // return the actual component shape.
  virtual ComponentType::Shape shape() const = 0;

  // set/get the reference direction
  // <group>
  virtual void setRefDirection(const MDirection & newRefDir) = 0;
  virtual const MDirection & refDirection() const = 0;
  virtual void refDirection(MDirection & refDir) const;
  // </group>

  // Return the scaling factor that indicates the proportion of the flux that
  // is in the specified pixel in the specified direction. The pixels are
  // assumed to be square.
  virtual void scale(Double & scaleFactor, const MDirection & direction, 
		     const MVAngle & pixelSize) const;
  virtual Double scale(const MDirection & direction, 
		       const MVAngle & pixelSize) const = 0;

  //# Project the shape onto an Image by sampling the component at the centre
  //# of each pixel add adding it the existing pixel value. The Image must
  //# contain one Direction Axis. Because this class does not know the flux,
  //# polarisation or frequency of the component these terms can be combined
  //# into a scale factor that is used to pre-multiply the values added to the
  //# image. If the image has spectral or stokes axes then only a subImage with
  //# one plane should be supplied.
  //# virtual void project(ImageInterface<Float> & plane, 
  //#		       const Double scale) const = 0;

  // Return the Fourier transform of the component at the specified point in
  // the spatial frequency domain. The point is specified by a 3-element vector
  // (u,v,w) that has units of meters and the frequency of the observation, in
  // Hertz. These two quantities can be used to derive the required spatial
  // frequency <src>(s = uvw*freq/c)</src>. The w component is not used in
  // these functions.

  // The "origin" of the transform is the reference direction of the
  // component. This means, for symmetric components where the reference
  // direction is at the centre, that the Fourier transform will always be
  // real.
  virtual void visibility(DComplex & result, const Vector<Double> & uvw,
			  const Double & frequency) const = 0;

  // returns True if the shape is symmetric about the reference direction.
  virtual Bool isSymmetric() const = 0;

  // return the number of parameters in this shape and set/get them.
  // <group>
  virtual uInt nShapeParameters() const = 0;
  virtual void setShapeParameters(const Vector<Double> & newParms) = 0;
  virtual void shapeParameters(Vector<Double> & compParms) const = 0;
  // </group>

  // This functions convert between a record and a ComponentShape. This way
  // derived classes can interpret fields in the record in a class specific
  // way. These functions define how the shape is represented in glish.  They
  // return False if the record is malformed and append an error message to the
  // supplied string giving the reason.
  // <group>
  virtual Bool fromRecord(String & errorMessage, 
			  const RecordInterface & record) = 0;
  virtual Bool toRecord(String & errorMessage,
			RecordInterface & record) const = 0;
  // </group>

  // Function which checks the internal data of this class for correct
  // dimensionality and consistant values. Returns True if everything is fine
  // otherwise returns False.
  virtual Bool ok() const = 0;
protected:
  //# These functions are used by derived classes implementing concrete
  //# versions of the toRecord and fromRecord member functions.
  Bool addDir(String & errorMessage, RecordInterface & record) const;
  Bool readDir(String & errorMessage, const RecordInterface & record);
};
#endif
