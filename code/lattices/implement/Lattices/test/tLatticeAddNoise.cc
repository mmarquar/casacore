//# tLatticeAddNoise.cc: Test program for class LatticeAddNoise
//# Copyright (C) 1998,1999,2000,2001
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

#include <trial/Lattices/LatticeAddNoise.h>
#include <aips/Lattices/ArrayLattice.h>
#include <aips/Lattices/LatticeIterator.h>
#include <aips/Lattices/LatticeStepper.h>
#include <aips/Logging/LogIO.h>
#include <aips/Mathematics/Math.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Arrays/IPosition.h>
#include <aips/Utilities/Assert.h>
#include <aips/Exceptions/Error.h>
#include <aips/iostream.h>

void test0 ();
void test0Complex ();
void test1 (LatticeAddNoise::Types type);
void test1Complex (LatticeAddNoise::Types type);
void checkStats (Float av0, const Lattice<Float>& data,
                 LatticeAddNoise::Types type);
void checkStatsComplex (Float av0, const Lattice<Complex>& data,
                        LatticeAddNoise::Types type);
main ()
{
  try {

// Float

      cerr << "Float" << endl;
      cerr << "Test 0" << endl;
      test0();

// Bug in Geometric distribution (defected) so left out for now

      cerr << "Test 1" << endl;
      const uInt n = LatticeAddNoise::NTYPES;
      for (uInt i=0; i<n; i++) {
         LatticeAddNoise::Types type = static_cast<LatticeAddNoise::Types>(i);
         cerr << "Type = " << LatticeAddNoise::typeToString(type) << endl;
         if (type!=LatticeAddNoise::GEOMETRIC) test1 (type);
      }

// Complex
 
      cerr << "Complex" << endl; 
      cerr << "Test 0" << endl;
      test0Complex();
//
      cerr << "Test 1" << endl;
      for (uInt i=0; i<n; i++) {
         LatticeAddNoise::Types type = static_cast<LatticeAddNoise::Types>(i);
         cerr << "Type = " << LatticeAddNoise::typeToString(type) << endl;
         if (type!=LatticeAddNoise::GEOMETRIC) test1Complex (type);
      }
  } catch (AipsError x) {
    cerr << "Caught exception: " << x.getMesg() << endl;
    cout << "FAIL" << endl;
    return 1;
  } 

  cout << "OK" << endl;
  return 0;
}


void test0 ()
{
   IPosition shape(2, 1024, 1024);
   ArrayLattice<Float> lat(shape);
   lat.set(0.0);

// Constructor 

   Vector<Double> pars(2);
   pars(0) = 0.5;
   pars(1) = 1.0;
   LatticeAddNoise lan(LatticeAddNoise::NORMAL, pars);
   lan.add(lat);
   checkStats(pars(0), lat, LatticeAddNoise::NORMAL);

// Default constructor and set

   LatticeAddNoise lan2;
   lan2.set (LatticeAddNoise::NORMAL, pars);
   lat.set(0.0);
   lan2.add(lat);
   checkStats(pars(0), lat, LatticeAddNoise::NORMAL);

// Assigment

   LatticeAddNoise lan3;
   lan3 = lan2;
   lat.set(0.0);
   lan3.add(lat);
   checkStats(pars(0), lat, LatticeAddNoise::NORMAL);
}

void test0Complex ()
{
   IPosition shape(2, 1024, 1024);
   ArrayLattice<Complex> lat(shape);
   lat.set(Complex(0,0));

// Constructor 

   Vector<Double> pars(2);
   pars(0) = 0.5;
   pars(1) = 1.0;
   LatticeAddNoise lan(LatticeAddNoise::NORMAL, pars);
   lan.add(lat);
   checkStatsComplex (pars(0), lat, LatticeAddNoise::NORMAL);

// Default constructor and set

   LatticeAddNoise lan2;
   lan2.set(LatticeAddNoise::NORMAL, pars);
   lat.set(Complex(0.0,0.0));
   lan2.add(lat);
   checkStatsComplex (pars(0), lat, LatticeAddNoise::NORMAL);

// Assigment

   LatticeAddNoise lan3;
   lan3 = lan2;
   lat.set(Complex(0.0,0.0));
   lan3.add(lat);
   checkStatsComplex (pars(0), lat, LatticeAddNoise::NORMAL);
}

void test1 (LatticeAddNoise::Types type)
{
   Vector<Double> pars;
   pars = LatticeAddNoise::defaultParameters(type);
   LatticeAddNoise lan(type, pars);
//
   IPosition shape(2, 1024, 1024);
   ArrayLattice<Float> lat(shape);
   lat.set(0.0);
   lan.add(lat);
//
   Float av = pars(0);
   if (type==LatticeAddNoise::DISCRETEUNIFORM ||
       type==LatticeAddNoise::UNIFORM) {
      av = (pars(0) + pars(1)) / 2.0;
   } else if (type==LatticeAddNoise::WEIBULL ||
              type==LatticeAddNoise::BINOMIAL) {
      av = -1.1e30;
   }
   checkStats(av, lat, type);
}

void test1Complex (LatticeAddNoise::Types type)
{
   Vector<Double> pars;
   pars = LatticeAddNoise::defaultParameters(type);
   LatticeAddNoise lan(type, pars);
//
   IPosition shape(2, 1024, 1024);
   ArrayLattice<Complex> lat(shape);
   lat.set(Complex(0.0, 0.0));
   lan.add(lat);
//
   Float av = pars(0);
   if (type==LatticeAddNoise::DISCRETEUNIFORM ||
       type==LatticeAddNoise::UNIFORM) {
      av = (pars(0) + pars(1)) / 2.0;
   } else if (type==LatticeAddNoise::WEIBULL ||
              type==LatticeAddNoise::BINOMIAL) {
      av = -1.1e30;
   }
   checkStatsComplex(av, lat, type);
}



void checkStats (Float av0,  const Lattice<Float>& data,
                 LatticeAddNoise::Types type)
{
   Double n = data.shape().product();
   Float av = mean(data.get());
   Float var = variance(data.get());
   Float sig = sqrt(var);
   LogIO os(LogOrigin("tLatticeAddNoise", "checkStats", WHERE));
//
   if (av0 > -1e30) {
      if (abs(av - av0) > 3*sig/sqrt(n)) {
         os << "Expected, observed and error in mean = " << av0 << ", " << av  
            << ", " << sig/sqrt(n) 
            << " for distribution " << LatticeAddNoise::typeToString(type) << LogIO::EXCEPTION;
      } 
   }
}



void checkStatsComplex (Float av0,  const Lattice<Complex>& data,
                        LatticeAddNoise::Types type)
{
   ArrayLattice<Float> realLattice(real(data.get()));
   ArrayLattice<Float> imagLattice(imag(data.get()));
//
   checkStats(av0, realLattice, type);
   checkStats(av0, imagLattice, type);
}


