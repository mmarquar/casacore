//# tFit2D.cc: Test nonlinear least squares classes for 2D Gaussian
//# Copyright (C) 1995,1996,1998,1999,2000,2001
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

#include <aips/Fitting.h>
#include <trial/Coordinates/CoordinateSystem.h>
#include <trial/Coordinates/CoordinateUtil.h>
#include <trial/Images/PagedImage.h>
#include <trial/Fitting/Fit2D.h>
#include <trial/Tasking/PGPlotter.h>
#include <aips/Functionals/NQGaussian2D.h>
#include <aips/Inputs/Input.h>
#include <aips/Logging.h>
#include <aips/Mathematics/Math.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Mathematics/Random.h>
#include <aips/Mathematics/Constants.h>
#include <aips/Utilities/Assert.h>

NQGaussian2D<Double> addModel (Array<Float>& pixels, Double height, Double x, Double y, 
                               Double major,  Double minor, Double pa);

void addNoise (Array<Float>& pixels, Array<Float>& sigma, Double noise);

int main(int argc, char **argv)
{

  try {

//
// Inputs
// 
   Input inputs(1);
   inputs.version ("$Revision$");
   inputs.create("nmodels", "1", "nmodels"); 
   inputs.create("noise", "0.0001", "Noise");
   inputs.create("major", "10.0", "major");
   inputs.create("minor", "5.0", "minor");
   inputs.create("pa", "45", "pa");                  // +x -> +y
   inputs.create("nx", "64", "nx");
   inputs.create("ny", "64", "ny");   
   inputs.create("norm", "False", "Normalize");
   inputs.create("mask", "1,1,1,1,1,1", "Mask");
   inputs.create("include", "0.0", "include");
   inputs.create("exclude", "0.0", "exclude");
   inputs.create("outfile", "", "outfile");
//
   inputs.readArguments(argc, argv);
   const Int nModels  = inputs.getInt("nmodels");   
   const Double noise = inputs.getDouble("noise");
   Double major = inputs.getDouble("major");
   Double minor= inputs.getDouble("minor");
   Double pa = inputs.getDouble("pa") * C::pi / 180.0;          // +x -> +y
   const Int nx = inputs.getInt("nx");   
   const Int ny = inputs.getInt("ny");   
   const Bool norm = inputs.getBool("norm");
   const Block<Int> mask = inputs.getIntArray("mask");
   const Block<Double> includeRange = inputs.getDoubleArray("include");
   const Block<Double> excludeRange = inputs.getDoubleArray("exclude");
   const String outfile = inputs.getString("outfile");
//
   LogOrigin or("tFit2D", "main()", WHERE);
   LogIO logger(or);
//
   Fit2D fitter(logger);
//
   IPosition shape(2,nx,ny);
   Array<Float> pixels(shape);
   Array<Float> sigma(shape);
//
   Double xsep = nx / nModels;
   Double ysep = ny / nModels;
   Double xPos, yPos;
   if (nModels==1) {
      xPos = nx / 2.0;
      yPos = ny / 2.0;     
   } else {
      xPos = xsep / 2.0;
      yPos = ysep / 2.0;
   }
   Double height = 1.0;

//
   Vector<Double> trueHeight(nModels);
   Vector<Double> trueX(nModels);
   Vector<Double> trueY(nModels);
   Vector<Double> trueMajor(nModels);
   Vector<Double> trueMinor(nModels);
   Vector<Double> truePA(nModels);
//
   for (Int i=0; i<nModels; i++) {

// Add model to data array

      NQGaussian2D<Double> gauss2d = addModel(pixels, height, xPos, yPos, major, minor, pa);
      trueHeight(i) = height;
      trueX(i) = xPos;
      trueY(i) = yPos;
      trueMajor(i) = major;
      trueMinor(i) = minor;
      truePA(i) = pa;

// Set Parameters mask

      Vector<Double> parameters(gauss2d.nparameters());
      Vector<Bool> parameterMask(parameters.nelements(), True);
      for (uInt i=0; i<parameters.nelements(); i++) {
         parameters(i) = gauss2d[i];
         if (mask[i]==0) {
            parameterMask(i) = False;
         }
      }
      parameters(5) = Fit2D::paFromGauss2D(parameters(5));

// convert axial ratio to minor axis (availableParameter
// interface uses axial ratio)

      parameters(4) = parameters(4)*parameters(3);  
/*
      cout << "      mask      = " << parameterMask << endl;
      cout << "True values     = " << parameters << endl;
      cout << "True pa (+x -> +y) = " << parameters(5) * 180.0 / C::pi << endl;
*/

// Set starting guess

      Vector<Double> startParameters(parameters.copy());
      for (uInt i=0; i<parameters.nelements(); i++) {
         startParameters(i) = parameters(i) * 0.9;
      }
/*
      cout << "Start values    = " << startParameters << endl;
      cout << "Start pa (+x -> +y) = " << startParameters(5) * 180.0 / C::pi << endl;
*/

// Add model to fitter

      fitter.addModel (Fit2D::GAUSSIAN, startParameters, parameterMask);

// Update model

      height *= 0.75;
      xPos += xsep;
      yPos += ysep;
//
      major *= 0.9;
      minor *= 0.9;
      pa += C::pi / 180 * 20.0;
      if (pa > C::pi) pa -= C::pi;
      cerr << endl;
   }

// Add noise
 
   addNoise (pixels, sigma, noise);

// Save image

   if (outfile!=String("")) {
       CoordinateSystem cSys = CoordinateUtil::defaultCoords2D();
       PagedImage<Float> im(shape, cSys, outfile);
       im.put(pixels);  
   }

// Set other state of fitter

   if (includeRange.nelements()==2) {
      fitter.setIncludeRange(includeRange[0], includeRange[1]);
   }
   if (excludeRange.nelements()==2) {
      fitter.setExcludeRange(excludeRange[0], excludeRange[1]);
   }

// Generate estimate
/*
   Vector<Double> p = fitter.estimate(Fit2D::GAUSSIAN, pixels);
   cout << "Estimate values = " << p << endl;
   cout << "Estimate pa (+x -> +y) = " << p(5) * 180.0 / C::pi << endl;
*/

// Make fit

   Fit2D::ErrorTypes status = fitter.fit(pixels, sigma, norm);
   if (status==Fit2D::OK) {
      Vector<Double> solution = fitter.availableSolution();
      Vector<Double> cv = fitter.covariance().diagonal();
      cout << "Covariance     = " << cv << endl;
      cout << "SNR        = " << fitter.availableSolution() / sqrt(cv) << endl;
      cout << "Chi squared = " << fitter.chiSquared() << endl << endl;
      cout << "Number of iterations = " << fitter.numberIterations() << endl;
      cout << "Number of points     = " << fitter.numberPoints() << endl;
//
//   when i return errors, make a test to 3sigma or summfink
//      if (!allNear(fitter.availableSolution(), parameters, 1e-6)) {
//         throw (AipsError("Solution not accurate to 1e-6"));
//      }
//
      cout << endl << "Number of models = " << fitter.nModels() << endl;
      for (uInt i=0; i<fitter.nModels(); i++) {
        Vector<Double> xx(5);
        xx(0) = trueHeight(i); xx(1) = trueX(i); xx(2) = trueY(i); xx(3) = trueMajor(i); xx(4) = truePA(i);
//
        cout << "Model " << i << " of type " << Fit2D::type(fitter.type(i)) << endl;
        cout << "   Actual values = " << xx << endl;
        cout << "   Solution      = " << fitter.availableSolution(i) << endl;
      }
//
      Array<Float> resid;
      fitter.residual(resid, pixels);
      cout << "Residual min and max = " << min(resid) << " " << max(resid) << endl;
   } else {
     logger << fitter.errorMessage() << endl;
   }

// Test copy constructor

   {
      cout << endl << endl << "Test copy constructor" << endl;
      Fit2D fitter2(fitter);
      fitter2.fit(pixels, sigma, norm);      
      if (!allEQ(fitter.availableSolution(),fitter2.availableSolution()) ||
         fitter.numberIterations() != fitter2.numberIterations() ||
         fitter.chiSquared() != fitter2.chiSquared() ||
         fitter.numberPoints() != fitter2.numberPoints()) {
         cout << "Failed copy constructor test" << endl;
      } else {
         cout << "Copy constructor test ok" << endl;
      }
   }

// Test assignment

   {
      cout << endl << endl << "Test assignment operator" << endl;
      Fit2D fitter2(logger);
      fitter2 = fitter;
      fitter2.fit(pixels, sigma, norm);
      if (!allEQ(fitter.availableSolution(),fitter2.availableSolution()) ||
         fitter.numberIterations() != fitter2.numberIterations() ||
         fitter.chiSquared() != fitter2.chiSquared() ||
         fitter.numberPoints() != fitter2.numberPoints()) {
         cout << "Failed assignment test" << endl;
      } else {
         cout << "Assignment test ok" << endl;
      }
   }
 } catch (AipsError x) {
      cout << "Failed with message " << x.getMesg() << endl;
 }   
}

NQGaussian2D<Double> addModel (Array<Float>& pixels, Double height, Double xcen, Double ycen,
                               Double major,  Double minor, Double pa)
{
   NQGaussian2D<Double> gauss2d;
   gauss2d.setHeight(height);
   gauss2d.setMajorAxis(major);
   gauss2d.setMinorAxis(minor);
   gauss2d.setXcenter(xcen);
   gauss2d.setYcenter(ycen);
   gauss2d.setPA(Fit2D::paToGauss2D(pa));          // +y -> -x
//
   IPosition shape = pixels.shape();
   IPosition loc(2);
   for (Int j=0; j<shape(1); j++) {
      for (Int i=0; i<shape(0); i++) {
         loc(0) = i;
         loc(1) = j;
         pixels(loc) += gauss2d(Double(i), Double(j));
      }
   }
   return gauss2d;
}


void addNoise (Array<Float>& pixels, Array<Float>& sigma, Double noise)
{
   sigma = 1.0;
   if (noise>0.0) sigma = noise;
//
   MLCG generator; 
   Normal noiseGen(&generator, 0.0, noise);  
//
   Bool deleteIt;
   Float* pData = pixels.getStorage(deleteIt);
   for (uInt k=0; k<pixels.shape().product(); k++){
      pData[k] += noiseGen();
   }
   pixels.putStorage(pData, deleteIt);
}

