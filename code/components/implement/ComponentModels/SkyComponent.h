//# SkyComponent.h: this defines SkyComponent.h
//# Copyright (C) 1996,1997,1998
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

#if !defined(AIPS_SKYCOMPONENT_H)
#define AIPS_SKYCOMPONENT_H

#include <aips/aips.h>
#include <aips/Utilities/CountedPtr.h>
#include <trial/ComponentModels/ComponentType.h>

class GlishRecord;
template<class T> class ImageInterface;
class MDirection;
class MFrequency;
class MVAngle;
class SkyCompRep;
class String;
template<class T> class Vector;
template<class T> class Flux;

// <summary>A component of a model of the sky </summary>

// <use visibility=export>
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite> 
// <li> MDirection 
// </prerequisite>
//

// <synopsis> 

// This base class is used by a number of classes that provide
// components used to represent the sky brightness. It abstracts the
// commonality between different components of a model like
// GaussianComponent, PointComponent, and perhaps in the future
// DiskComponent & SpheroidComponent. In particular it allows the user to
// sample the component at any specified direction in the sky as well as grid
// the component onto a specified image.

// The functions in this class basically allow the user to sample the intensity
// of the component by specifying either a direction or an image onto which the
// component should be projected. 

// The type function returns a string stating the actual type of component
// that is used.
// </synopsis>

// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Model fitting is an important part of astronomical data
// reduction/interpretation. This class defines a model component. Many
// components can be strung together (using the class ComponentList) to
// construct a model. 
// </motivation>

// <thrown>
// Exceptions are not directly thrown by this class
// </thrown>
//
// <todo asof="1997/05/01">
//   <li> 
// </todo>

class SkyComponent
{
public:
  // The default is a PointComponent at the J2000 north pole with a flux of
  // 1Jy in the I polarization only. 
  SkyComponent();

  // Construct a SkyComponent of the specified shape. The default direction is
  // the J2000 north pole and the default flux is 1 Jy in the I polarisation
  // only. Use the setFlux and SetDirection functions to change this after
  // construction. The spectral variation is assumed to be constant.
  SkyComponent(ComponentType::Shape shape);
  
  // Construct a SkyComponent of the specified spatial and spectral shapes. The
  // default direction is the J2000 north pole and the default flux is 1 Jy in
  // the I polarisation only. Use the setFlux and SetDirection functions to
  // change this after construction.
  SkyComponent(ComponentType::Shape shape, 
	       ComponentType::SpectralShape spectralModel);
  
  // The copy Constructor uses reference semantics
  SkyComponent(const SkyComponent & other);

  // a virtual destructor is needed so that the actual destructor in derived
  // classes will be used.
  virtual ~SkyComponent();

  // The assignment operator uses reference semantics
  SkyComponent & operator=(const SkyComponent & other);

  // Return the intensity (in Jy/pixel) of the component at the specified
  // direction. The Vector contains all the polarizations (Stokes I,Q,U,V) of
  // the radiation and must be of length 4. The returned Vector contains the
  // different polarizations of the radiation and the pixel size is assumed to
  // be square.
  virtual void sample(Vector<Double> & result, 
		      const MDirection & sampleDir,
		      const MVAngle & pixelSize) const;

  // Project the component onto an Image. The default implementation calls the
  // sample function once for the centre of each pixel. The image needs
  // only have a one (and only one) direction axis. Other axes are optional and
  // if there is no Stokes axes then it is assumed that the polarization is
  // Stokes::I. The component is gridded equally onto all other axes of the
  // image (ie. spectral axes).
  virtual void project(ImageInterface<Float> & plane) const;

  // return a reference to the flux of the component. Because this is a
  // reference, manipulation of the flux values is performed through the
  // functions in the Flux class. eg., <src>comp.flux().setValue(newVal)</src>
  // <group>
  virtual Flux<Double> & flux();
  virtual const Flux<Double> & flux() const;
  // </group>

  // set/get the direction of (usually the centre) of the component.
  // <group>
  virtual void setDirection(const MDirection & newPos);
  virtual void direction(MDirection & compDir) const;
  // </group>

  // Return the Fourier transform of the component at the specified point in
  // the spatial frequency domain. The point is specified by a 3 element vector
  // (u,v,w) that has units of meters and the frequency of the observation, in
  // Hertz. These two quantities can be used to derive the required spatial
  // frequency <src>(s = uvw*freq/c)</src>. The w component is not used in
  // these functions.
  //
  // The reference position for the transform is the direction of the
  // component. Hence the transform is always a real value and there is no
  // phase variation due to the spatial structure if the component has a
  // symmetric shape. The returned visibility values may still be Complex if a
  // polarisation representation other than Stokes is used.
  // <group>
  virtual Flux<Double> visibility(const Vector<Double> & uvw,
				  const Double & frequency) const;
  // </group>

  // </group>
  // set/get the label associated with this component.
  // <group>
  virtual void setLabel(const String & newLabel);
  virtual void label(String & compLabel) const;
  // </group>

  // return the number of parameters in the component and set/get them.
  // <group>
  virtual uInt nParameters() const;
  virtual void setParameters(const Vector<Double> & newParms);
  virtual void parameters(Vector<Double> & compParms) const;
  // </group>

  // get the spatial shape model for this component
  virtual ComponentType::Shape shape() const;

  // get the spectral model for this component 
  virtual ComponentType::SpectralShape spectralShape() const;

  // set/get the reference frequency.
  // <group>
  virtual void setRefFrequency(const MFrequency & newRefFreq);
  virtual const MFrequency & refFrequency() const;
  // </group>

  // Return the scaling factor used to scale the flux of the component at the
  // specified frequency. The scaling factor is always 1 at the reference
  // frequency.
  virtual Double scale(const MFrequency & sampleFreq) const;

  // Return the flux (in dimensions of W/m^2/Hz) of the component at the
  // specified frequency
  virtual Flux<Double> sample(const MFrequency & sampleFreq) const;

  // return the number of parameters in the spectral shape and set/get them.
  // <group>
  virtual uInt nSpectralParameters() const;
  virtual void setSpectralParameters(const Vector<Double> & newParms);
  virtual void spectralParameters(Vector<Double> & compParms) const;
  // </group>

  // This functions convert between a glish record and a SkyComponent. This way
  // derived classes can interpret fields in the record in a class specific
  // way. These functions define how a component is represented in glish. They
  // append a message to the errorMessage string if the conversion failed for
  // any reason and then return False. Both functions return True if the
  // conversion succeded.  
  // <group>
  virtual Bool fromRecord(String & errorMessage, const GlishRecord & record);
  virtual Bool toRecord(String & errorMessage, GlishRecord & record) const;
  // </group>
  
  // Return the shape of component that the supplied record represents. Returns
  // ComponentType::POINT if there is no shape field in the component record &
  // ComponentType::UNKNOWN_SHAPE if the shape record could not be parsed
  // correctly (it then appends an appropriate error message to the
  // errorMessage String).
  static ComponentType::Shape getShape(String & errorMessage,
				       const GlishRecord & record);

  // Return the spectral shape of component that the supplied record
  // represents. Returns ComponentType::CONSTANT_SPECTRUM if there is no
  // spectrum field in the component record &
  // ComponentType::UNKNOWN_SPECTRAL_SHAPE if the spectrum record could not be
  // parsed correctly (it then appends an appropriate error message to the
  // errorMessage String).
  static ComponentType::SpectralShape getSpectralShape(String & errorMessage,
                                                   const GlishRecord & record);

  // Return a distinct copy of this component. As both the assignment operator
  // and the copy constructor use reference semantics this is the only way to
  // get a real copy.
  virtual SkyComponent copy() const;

  // Function which checks the internal data of this class for correct
  // dimensionality and consistant values. Returns True if everything is fine
  // otherwise returns False.
  virtual Bool ok() const;

protected:
  //# An easy way for derived classes to initialise the base class is to use
  //# this constructor. The pointer is taken over. 
  SkyComponent(SkyCompRep * rawPtr);

  //# Return the polymorphic pointer that is being reference counted. This is
  //# used by derived classes to cache an downcast pointer.
  SkyCompRep * rawPtr(); 
  const SkyCompRep * rawPtr() const;

  //# Check that the shape in the glishRecord matches the shape of this
  //# component. Returns False if not and appends a reason in the errorMessage
  //# String
  Bool checkShape(String & errorMessage, const GlishRecord & record) const;

  //# Check that the shape in the glishRecord matches the shape of this
  //# component. Returns False if not and appends a reason in the errorMessage
  //# String
  Bool checkSpectralShape(String & errorMessage,
			  const GlishRecord & record) const;

private:
  CountedPtr<SkyCompRep> itsCompPtr;
};
#endif
