//# dImageInterface.cc:  Illustrates the use of the ImageInterface base class
//# Copyright (C) 1996
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

#include <trial/Images/PagedImage.h>
#include <trial/Images/ImageInterface.h>
#include <trial/Lattices/LatticeIterator.h>
#include <aips/Lattices/IPosition.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/ArrayMath.h>
#include <trial/ImgCrdSys/ImageCoordinate.h>
#include <trial/Measures/SkyPosition.h>
#include <trial/Measures/MeasuredValue.h>
#include <aips/Mathematics/Constants.h>

Float sumPixels(const ImageInterface<Float>& image){
  uInt rowLength = image.shape()(0);
  IPosition rowShape(image.ndim());
  rowShape = 1; rowShape(0) = rowLength;
  Float sumPix = 0;
  RO_LatticeIterator<Float> iter(image, rowShape);
  while(!iter.atEnd()){
    sumPix += sum(iter.vectorCursor().ac());
    iter++;
  }
  return sumPix;
}

ImageCoordinate defaultCoords2D(){
    // Default object position is at RA=0, Dec=0
  Vector<Double> defaultPosition(2); defaultPosition = 0;
  // Default Earth position is for the centre of the earth at 0:00 UT on 1/1/96
  EarthPosition defaultObs(0.0, 1, 1, 1996, 0.0, 0.0, 0.0);
  // Default Reference Pixel is the first pixel
  Vector<Double> refPixel(2); refPixel = 1;
  // Default step size is 1 arc-sec/pixel
  Vector<Double> defaultStep(2); defaultStep = C::pi/180./60./60.; 
  ProjectedPosition defaultRAandDec(ProjectedPosition::SIN, 
                                    SkyPosition::EQUATORIAL,
                                    SkyPosition::J2000,
                                    defaultPosition, 
                                    defaultObs,
                                    0.0, // Rotation
                                    refPixel,
                                    defaultStep); 
  ImageCoordinate defaultAxes;
  defaultAxes.addAxis(defaultRAandDec);
  return defaultAxes;
}

int main(){
  PagedImage<Float> demo(IPosition(2, 10), defaultCoords2D(), 
			 "dImageInterface_tmp.image");
  demo.set(1.0f);
  cout << "Sum of all pixels is: " << sumPixels(demo) << endl;
  if (near(sumPixels(demo), 100.0f)){
    cout << "OK" << endl;
    return 0;
  }
  else {
    cout << "FAIL" << endl;
    return 1;
  }
}
