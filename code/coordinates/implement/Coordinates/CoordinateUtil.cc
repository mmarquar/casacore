//# CoordinateUtils.cc: 
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002
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

#include <trial/Coordinates/CoordinateUtil.h>
#include <trial/Coordinates/CoordinateSystem.h>
#include <trial/Coordinates/DirectionCoordinate.h>
#include <trial/Coordinates/LinearCoordinate.h>
#include <trial/Coordinates/Projection.h>
#include <trial/Coordinates/SpectralCoordinate.h>
#include <trial/Coordinates/StokesCoordinate.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Arrays/Vector.h>
#include <aips/Exceptions/Error.h>
#include <aips/Mathematics/Math.h>
#include <aips/Measures/MDirection.h>
#include <aips/Measures/MFrequency.h>
#include <aips/Measures/MEpoch.h>
#include <aips/Measures/MPosition.h>
#include <aips/Measures/MeasTable.h>
//#include <aips/Measures/MCDirection.h>
#include <aips/Measures/MeasConvert.h>
#include <aips/Quanta/MVEpoch.h>
#include <aips/Quanta/MVDirection.h>
#include <aips/Quanta/MVPosition.h>
#include <aips/Logging/LogIO.h>
#include <aips/OS/Time.h>
#include <aips/Quanta/QC.h>
#include <aips/Quanta/MVTime.h>
#include <aips/Quanta/MVEpoch.h>
#include <aips/Utilities/Assert.h>
#include <aips/Utilities/GenSort.h>
#include <aips/Utilities/String.h>

#include <aips/strstream.h>


void CoordinateUtil::addDirAxes(CoordinateSystem & coords){
  Matrix<Double> xform(2, 2); xform = 0.0; xform.diagonal() = 1.0;
  DirectionCoordinate dirAxes(MDirection::J2000, 
			       Projection(Projection::SIN),
			       0.0, 0.0, // Ref is at RA = 0, Dec = 0
			       1.0, 1.0, // The increment is overwritten below
			       xform,    // Rotation matrix
			       0.0, 0.0, // Ref pixel is 0,0
			       999.0, 999.0);
  // reset the increment to 1 minute of arc on both axes
  Vector<String> units(2); units = String("'"); 
  Vector<Double> inc(2); inc(0) = -1.0; inc(1) = 1.0;
  dirAxes.setWorldAxisUnits(units);
  AlwaysAssert(dirAxes.setIncrement(inc) == True, AipsError);
  // Add the direction coordinates to the system. 
  coords.addCoordinate(dirAxes);
}
void CoordinateUtil::addIQUVAxis(CoordinateSystem & coords){
  Vector<Int> pols(4);
  pols(0) = Stokes::I;
  pols(1) = Stokes::Q;
  pols(2) = Stokes::U;
  pols(3) = Stokes::V;
  StokesCoordinate polAxis(pols);
  // Add the stokes coordinates to the system. 
  coords.addCoordinate(polAxis);
}
void CoordinateUtil::addIAxis(CoordinateSystem & coords){
  Vector<Int> pols(1);
  pols(0) = Stokes::I;
  StokesCoordinate polAxis(pols);
  // Add the stokes coordinates to the system. 
  coords.addCoordinate(polAxis);
}

Bool CoordinateUtil::addStokesAxis(CoordinateSystem & coords,
                                   uInt shape)
{
   if (shape<1 || shape>4) return False;
//
   Vector<Int> which;
   if (shape==1) {
      which.resize(1);
      which(0) = Stokes::I;
   } else if (shape==2) {
      which.resize(2);
      which(0) = Stokes::I;
      which(1) = Stokes::Q;
   } else if (shape==3) {
      which.resize(3);
      which(0) = Stokes::I;
      which(1) = Stokes::Q;
      which(2) = Stokes::U;
   } else if (shape==4) {
      which.resize(4);
      which(0) = Stokes::I;
      which(1) = Stokes::Q;
      which(2) = Stokes::U;
      which(3) = Stokes::V;
   }
   StokesCoordinate sc(which);
   coords.addCoordinate(sc);
   return True;
}


void CoordinateUtil::addFreqAxis(CoordinateSystem & coords)
{
  SpectralCoordinate freqAxis(MFrequency::LSRK,               // Local standard of rest
			      1415E6,                         // ref. freq. = 1415MHz
			      1E3,                            // 1 kHz bandwidth/channel
			      0.0,                            // channel 0 is the ref.
                              QC::HI.getValue(Unit("Hz")));   // HI
  coords.addCoordinate(freqAxis);
}

void CoordinateUtil::addLinearAxes(CoordinateSystem & coords, 
                                   const Vector<String>& names,
                                   const IPosition& shape)
{
    const uInt n = names.nelements();
//
    Vector<String> units(n);
    Vector<Double> refVal(n);
    Vector<Double> refPix(n);
    Vector<Double> inc(n);
//     
    for (uInt i=0; i<n; i++) {
       refVal(i) = 0.0;
       inc(i) = 1.0; 
       if (shape.nelements()==n) {
          refPix(i) = Double(Int((shape(i) + 1)/2));
       } else {
          refPix(i) = 0.0;
       }
       units(i) = String("arcsec");
    } 
    Matrix<Double> pc(n, n);
    pc = 0.0; 
    pc.diagonal() = 1.0;
//
    LinearCoordinate lc(names, units, refVal, inc, pc, refPix);
    coords.addCoordinate(lc);
}

CoordinateSystem CoordinateUtil::defaultCoords2D(){
  CoordinateSystem coords;
  CoordinateUtil::addDirAxes(coords);
  return coords;
}
CoordinateSystem CoordinateUtil::defaultCoords3D(){
  CoordinateSystem coords;
  CoordinateUtil::addDirAxes(coords);
  CoordinateUtil::addFreqAxis(coords);
  return coords;
}
CoordinateSystem CoordinateUtil::defaultCoords4D(){
  CoordinateSystem coords;
  CoordinateUtil::addDirAxes(coords);
  CoordinateUtil::addIQUVAxis(coords);
  CoordinateUtil::addFreqAxis(coords);
  return coords;
}

CoordinateSystem CoordinateUtil::defaultCoords(uInt dims){
  switch (dims){
  case 2:
    return CoordinateUtil::defaultCoords2D();
  case 3:
    return CoordinateUtil::defaultCoords3D();
  case 4:
    return CoordinateUtil::defaultCoords4D();
  default:
    throw(AipsError("defaultCoords() - cannot create cordinates except "
		    "for a 2, 3 or 4-dimensional image"));
    // The following line is just to suppress a compiler warning that this
    // function does not always return a CoordinateSystem. It is never
    // executed.
    return CoordinateUtil::defaultCoords2D();
  }
}

Int CoordinateUtil::findSpectralAxis(const CoordinateSystem & coords) 
{
  const Int coordinate = coords.findCoordinate(Coordinate::SPECTRAL);
  if (coordinate < 0) return coordinate;
//
  AlwaysAssert(coords.findCoordinate(Coordinate::SPECTRAL, coordinate)
	       == -1, AipsError);
  const Vector<Int> pixelAxes = coords.pixelAxes(coordinate);
  AlwaysAssert(pixelAxes.nelements() == 1, AipsError);
  return pixelAxes(0);
}

void CoordinateUtil::findSpectralAxis(Int& pixelAxis, Int& worldAxis, 
                                      Int& coordinate, const CoordinateSystem & coords)
{
  pixelAxis = -1;
  worldAxis = -1;
  coordinate = coords.findCoordinate(Coordinate::SPECTRAL);
  if (coordinate < 0) return;
//
  AlwaysAssert(coords.findCoordinate(Coordinate::SPECTRAL, coordinate)
               == -1, AipsError);
//
  const Vector<Int> pixelAxes = coords.pixelAxes(coordinate);
  AlwaysAssert(pixelAxes.nelements() == 1, AipsError);
  pixelAxis = pixelAxes(0);
//
  const Vector<Int> worldAxes = coords.worldAxes(coordinate);
  AlwaysAssert(worldAxes.nelements() == 1, AipsError);
  worldAxis = worldAxes(0);
//
  return;  
}



Vector<Int> CoordinateUtil::findDirectionAxes(const CoordinateSystem & coords)
{
  const Int coordinate = coords.findCoordinate(Coordinate::DIRECTION);
  Vector<Int> retVal;
  if (coordinate < 0)  return retVal;
//
  AlwaysAssert(coords.findCoordinate(Coordinate::DIRECTION, coordinate)
	       == -1, AipsError);
  retVal = coords.pixelAxes(coordinate);
  return retVal;
}


void CoordinateUtil::findDirectionAxes(Vector<Int>& pixelAxes,
                                       Vector<Int>& worldAxes,
                                       Int& coordinate,
                                       const CoordinateSystem & coords) 
{
  pixelAxes.resize(0);
  worldAxes.resize(0);
  coordinate = coords.findCoordinate(Coordinate::DIRECTION);
  if (coordinate < 0) return;
//
  AlwaysAssert(coords.findCoordinate(Coordinate::DIRECTION, coordinate)
	       == -1, AipsError);
//
  pixelAxes = coords.pixelAxes(coordinate);
  AlwaysAssert(pixelAxes.nelements() == 2, AipsError);
//
  worldAxes = coords.worldAxes(coordinate);
  AlwaysAssert(worldAxes.nelements() == 2, AipsError);
//
   return;
}


Int CoordinateUtil::findStokesAxis(Vector<Stokes::StokesTypes>& whichPols, 
                                   const CoordinateSystem& coords)
{
  const Int coordinate = coords.findCoordinate(Coordinate::STOKES);
  if (coordinate < 0) {
    whichPols.resize(1);
    whichPols(0) = Stokes::I;
    return coordinate;
  }
  AlwaysAssert(coords.findCoordinate(Coordinate::STOKES, coordinate) == -1, 
	       AipsError);
  const Vector<Int> pixelAxes = coords.pixelAxes(coordinate);
  AlwaysAssert(pixelAxes.nelements() == 1, AipsError);
  const StokesCoordinate& polCoord = coords.stokesCoordinate(coordinate);
  const Vector<Int> polsAsInts = polCoord.stokes();
  const uInt nStokes = polsAsInts.nelements();
  whichPols.resize(nStokes);
  for (uInt i = 0; i < nStokes; i++) {
    whichPols(i) = (Stokes::StokesTypes) polsAsInts(i);
  }
  return pixelAxes(0);
}


void CoordinateUtil::findStokesAxis(Int& pixelAxis, Int& worldAxis, 
                                    Int& coordinate, const CoordinateSystem & coords)
{
  pixelAxis = -1;
  worldAxis = -1;
  coordinate = coords.findCoordinate(Coordinate::STOKES);
  if (coordinate < 0) return;
//
  AlwaysAssert(coords.findCoordinate(Coordinate::STOKES, coordinate)
               == -1, AipsError);
//
  const Vector<Int> pixelAxes = coords.pixelAxes(coordinate);
  AlwaysAssert(pixelAxes.nelements() == 1, AipsError);
  pixelAxis = pixelAxes(0);
//
  const Vector<Int> worldAxes = coords.worldAxes(coordinate);
  AlwaysAssert(worldAxes.nelements() == 1, AipsError);
  worldAxis = worldAxes(0);
//
  return;  
}



Bool CoordinateUtil::removeAxes(CoordinateSystem& cSys,
                                Vector<Double>& worldReplacement,
                                const Vector<uInt>& worldAxes,
                                const Bool removeThem)
//
// Remove all the world axes and associated pixel axes
// derived from the given list (a list to keep or remove)
// of world axes.
// This is awkward because as soon as you remove an 
// axis, they all shuffle down one !  The replacement values
// are optional.  If these vectors are the wrong length,
// (e.g. 0), the reference pixels/values are used.  The used
// values are returned.
//
{
// Bug out if nothing to do

   if (worldAxes.nelements() == 0) return True;

// Make sure the world axes are valid

   uInt i,j;
   for (i=0; i<worldAxes.nelements(); i++) {
      if (worldAxes(i) >= cSys.nWorldAxes()) return False;         
   }

// Make a list of the axes to remove in ascending order
// with no duplicates

   Vector<uInt> remove(cSys.nWorldAxes());
   if (removeThem) {
      remove.resize(worldAxes.nelements());
      remove = worldAxes;
      GenSort<uInt>::sort(remove, Sort::Ascending, Sort::NoDuplicates);
   } else {
      for (i=0,j=0; i<cSys.nWorldAxes(); i++) {
         if (!anyEQ(worldAxes, i)) remove(j++) = i;
      }
      remove.resize(j,True);
   }
   const uInt nRemove = remove.nelements();
   if (nRemove==0) return True;

// Set the replacement values for the removal world axes
// if the user didn't give any or got it wrong

   if (worldReplacement.nelements() != nRemove) {
      worldReplacement.resize(nRemove);
      for (i=0; i<nRemove; i++) {
         worldReplacement(i) = cSys.referenceValue()(remove(i));
      }
   }


// Now for each world axis in the list, get rid of the world and associated
// pixel axis
 
   uInt worldAxis = remove(0);
   for (i=0; i<nRemove; i++) {

// Remove the axis

      if (!cSys.removeWorldAxis(worldAxis, worldReplacement(i))) return False;

// Find the next world axis to eradicate

      if (i+1<remove.nelements()) worldAxis = remove(i+1) - 1;
   }
   return True;
}      



Bool CoordinateUtil::removePixelAxes(CoordinateSystem& cSys,
                                     Vector<Double>& pixelReplacement,
                                     const Vector<uInt>& pixelAxes,
                                     const Bool removeThem)
{
// Bug out if nothing to do

   if (pixelAxes.nelements() == 0) return True;

// Make sure the pixel axes are valid

   uInt i,j;
   for (i=0; i<pixelAxes.nelements(); i++) {
      if (pixelAxes(i) >= cSys.nPixelAxes()) return False;         
   }

// Make a list of the axes to remove in ascending order
// with no duplicates

   Vector<uInt> remove(cSys.nPixelAxes());
   if (removeThem) {
      remove.resize(pixelAxes.nelements());
      remove = pixelAxes;
      GenSort<uInt>::sort(remove, Sort::Ascending, Sort::NoDuplicates);
   } else {
      for (i=0,j=0; i<cSys.nPixelAxes(); i++) {
         if (!anyEQ(pixelAxes, i)) remove(j++) = i;
      }
      remove.resize(j,True);
   }
   const uInt nRemove = remove.nelements();
   if (nRemove==0) return True;

// Set the replacement values for the removed pixel axes
// if the user didn't give any or got it wrong

   if (pixelReplacement.nelements() != nRemove) {
      pixelReplacement.resize(nRemove);
      for (i=0; i<nRemove; i++) {
         pixelReplacement(i) = cSys.referencePixel()(remove(i));
      }
   }


// Now for each pixel axis in the list, get rid of it
 
   uInt pixelAxis = remove(0);
   for (i=0; i<nRemove; i++) {

// Remove the axis

      if (!cSys.removePixelAxis(pixelAxis, pixelReplacement(i))) return False;

// Find the next world axis to eradicate

      if (i+1<remove.nelements()) pixelAxis = remove(i+1) - 1;
   }
   return True;
}      



CoordinateSystem CoordinateUtil::makeCoordinateSystem(const IPosition& shape,
                                                      Bool doLinear)
{         
   const uInt n = shape.nelements();
   CoordinateSystem cSys;

// Attach an ObsInfo record so images that are made
// with this have something sensible

   ObsInfo obsInfo;
   obsInfo.setObserver(String("NoY2K"));
   obsInfo.setTelescope(String("ATCA"));

// It must be easier than this...  USe 0.0001
// so that roundoff does not tick the 0 to 24

   Time time(2000, 1, 1, 0, 0, 0.0001);
   MVTime time2(time);
   MVEpoch time4(time2);
   MEpoch date(time4);
   obsInfo.setObsDate(date);
   cSys.setObsInfo(obsInfo);
//
   if (doLinear) {
      Vector<String> names(n);
      for (uInt i=0; i<n; i++) {
         ostrstream oss;
         oss << (i+1);
         String t(oss);
         names(i) = "linear" + t;
      }
      CoordinateUtil::addLinearAxes(cSys, names, shape);
      return cSys;
   }
//
   Bool doneStokes = False;
   Bool doneFreq = False;
//
   if (n==1) {
      CoordinateUtil::addFreqAxis(cSys);
      return cSys;
   }
//          
   if (n>=2) {
      CoordinateUtil::addDirAxes(cSys);
   }
//
   if (n>=3) {
      if (CoordinateUtil::addStokesAxis(cSys, uInt(shape(2)))) {
         doneStokes = True;
      } else {
         CoordinateUtil::addFreqAxis(cSys);
         doneFreq = True;   
      }  
   }
//
   uInt nDone = 0;
   if (n>=4) {
      Bool ok = True;
      nDone = 4;
      if (doneStokes) {
         CoordinateUtil::addFreqAxis(cSys);
         doneFreq = True;
      } else {
         if (CoordinateUtil::addStokesAxis(cSys, uInt(shape(3)))) {
            doneStokes = True;
         } else {
            if (!doneFreq) {
               CoordinateUtil::addFreqAxis(cSys);
               doneFreq = True;
            } else {
               ok = False;
               nDone = 3;
            }
         } 
      }
   }


// Linear for the rest

   if (nDone==3 || n >=5) {
      const uInt nLeft = n - nDone;
      if (nLeft > 0) {
         IPosition shape2(nLeft);
         Vector<String> names(nLeft);
         for (uInt i=0; i<nLeft; i++) {
            shape2(i) = shape(i+nDone);
            ostrstream oss;
            oss << (i+1);
            String t(oss);
            names(i) = "linear" + t;
         }
         CoordinateUtil::addLinearAxes(cSys, names, shape2);
      }
   }
   return cSys;
} 



Bool CoordinateUtil::makeDirectionMachine(LogIO& os, MDirection::Convert& machine,
                                          const DirectionCoordinate& dirCoordTo,
                                          const DirectionCoordinate& dirCoordFrom,
                                          const ObsInfo& obsTo,
                                          const ObsInfo& obsFrom) 
{
   const MDirection::Types& typeFrom = dirCoordFrom.directionType();
   const MDirection::Types& typeTo = dirCoordTo.directionType();
   Bool typesEqual = (typeTo==typeFrom);
//
   MEpoch epochFrom = obsFrom.obsDate();
   MEpoch epochTo = obsTo.obsDate();
   Double t1 = epochFrom.getValue().get();
   Double t2 = epochTo.getValue().get();
   Bool epochEqual = near(t1,t2);
//
   String telFrom = obsFrom.telescope();
   String telTo = obsTo.telescope();
   Bool posEqual = (telFrom==telTo);

// Everything is the same for input and output so we don't 
// need a machine to convert anything

   if (typesEqual && epochEqual && posEqual) return False;

// Start with simplest machine, just MDirection::Types.  If it does 
// the conversion, that's all we need.  If not, we need more.

   MDirection::Ref refFrom(typeFrom);
   MDirection::Ref refTo(typeTo);
   machine = MDirection::Convert(refFrom, refTo);
//
   MDirection fromMD;
   dirCoordFrom.toWorld(fromMD, dirCoordFrom.referencePixel());
   Bool ok = True;
   try {
      MDirection toMD = machine(fromMD);
   } catch (AipsError x) {
      ok = False;
   }
   if (ok) {
      if (typeFrom==typeTo) {
         return False;
      } else {          
         return True;
      }
   }

// The conversion failed, so we need either or both of epoch 
// and position in the machine.  

   Double t = epochFrom.getValue().get();
   if (t<0.0) {
      os << "In setting up the DirectionCoordinate conversion machinery" << endl;
      os << "The output CoordinateSystem has no valid epoch" << LogIO::EXCEPTION;
   }
   t = epochTo.getValue().get();
   if (t<0.0)) {
      os << "In setting up the DirectionCoordinate conversion machinery" << endl;
      os << "The input CoordinateSystem has no valid epoch" << LogIO::EXCEPTION;
   }

// Now add the epoch to the machine and see if that works

   {
      MeasFrame frameFrom;
      MeasFrame frameTo;
//
      frameFrom.set(epochFrom);
      frameTo.set(epochTo);
      MDirection::Ref refFrom(typeFrom, frameFrom);
      MDirection::Ref refTo(typeTo, frameTo);
      machine = MDirection::Convert(refFrom, refTo);
//
      ok = True;
      try {
         MDirection toMD = machine(fromMD);
      } catch (AipsError x) {
         ok = False;
      }
      if (ok) return True;
   }

// Now add the position to the machine and see if that works

   if (telFrom==String("UNKNOWN")) {
      os << "In setting up the DirectionCoordinate conversion machinery" << endl;
      os << "The output CoordinateSystem has no valid observatory name - cannot divine its position" << LogIO::EXCEPTION;
   }
   if (telTo==String("UNKNOWN")) {
      os << "In setting up the DirectionCoordinate conversion machinery" << endl;
      os << "The input CoordinateSystem has no valid observatory name - cannot divine its position" << LogIO::EXCEPTION;
   }
//
   MPosition posFrom, posTo;
   Bool found = MeasTable::Observatory(posFrom, telFrom);
   if (!found) {
      os << "In setting up the DirectionCoordinate conversion machinery" << endl;
      os << "Cannot lookup the observatory name " << telFrom << " in the AIPS++" << endl;
      os << "data base.  Please request that it be added" << LogIO::EXCEPTION;
   }
   found = MeasTable::Observatory(posTo, telTo);
   if (!found) {
      os << "In setting up the DirectionCoordinate conversion machinery" << endl;
      os << "Cannot lookup the observatory name " << telTo << " in the AIPS++" << endl;
      os << "data base.  Please request that it be added" << LogIO::EXCEPTION;
   }
//
   {
      MeasFrame frameFrom;
      MeasFrame frameTo;
//
      frameFrom.set(posFrom);
      frameTo.set(posTo);
      MDirection::Ref refFrom(typeFrom, frameFrom);
      MDirection::Ref refTo(typeTo, frameTo);
      machine = MDirection::Convert(refFrom, refTo);
//
      ok = True;
      try {
         MDirection toMD = machine(fromMD);
      } catch (AipsError x) {
         ok = False;
      }
      if (ok) return True;
   }

// Well looks like we need both

   {
      MeasFrame frameFrom(posFrom, epochFrom);
      MeasFrame frameTo(posTo, epochTo);
//
      MDirection::Ref refFrom(typeFrom, frameFrom);
      MDirection::Ref refTo(typeTo, frameTo);
//
      machine = MDirection::Convert(refFrom, refTo);
      ok = True;
      try {
         MDirection toMD = machine(fromMD);
      } catch (AipsError x) {
         ok = False;
      }
      if (!ok) {
         os << "Unable to convert between the inputand output  " <<
               "DirectionCoordinates - this is surprising !" << LogIO::EXCEPTION;
      }
   }
//
   return True;
}
 

Bool CoordinateUtil::makeFrequencyMachine(LogIO& os, MFrequency::Convert& machine,
                                          Int coordinateTo, Int coordinateFrom,
                                          const CoordinateSystem& coordsTo,
                                          const CoordinateSystem& coordsFrom)
{
   MDirection dirTo, dirFrom;
   {
      Coordinate::Type type = Coordinate::DIRECTION;
      Int afterCoord = -1;
      Int c = coordsTo.findCoordinate(type, afterCoord);
      if (c<0) {
         os << "No Direction coordinate in 'to' CoordinateSystem" << LogIO::EXCEPTION;
      }
      const DirectionCoordinate& dCoord = coordsTo.directionCoordinate(c);   
      const Vector<Double>& rp = dCoord.referencePixel();   
      if (!dCoord.toWorld(dirTo, rp)) {
         os << dCoord.errorMessage() << LogIO::EXCEPTION;
      }
   }
//
   {
      Coordinate::Type type = Coordinate::DIRECTION;
      Int afterCoord = -1;
      Int c = coordsFrom.findCoordinate(type, afterCoord);
      if (c<0) {
         os << "No Direction coordinate in 'from' CoordinateSystem" << LogIO::EXCEPTION;
      }
      const DirectionCoordinate& dCoord = coordsFrom.directionCoordinate(c);   
      const Vector<Double>& rp = dCoord.referencePixel();   
      if (!dCoord.toWorld(dirFrom, rp)) {
         os << dCoord.errorMessage() << LogIO::EXCEPTION;
      }
   }
//
   MFrequency::Types typeTo, typeFrom;
   {
      Coordinate::Type type = Coordinate::SPECTRAL;
      Int afterCoord = -1;
      Int c = coordsTo.findCoordinate(type, afterCoord);
      if (c<0) {
         os << "No Spectral coordinate in 'to' CoordinateSystem" << LogIO::EXCEPTION;
      }
      const SpectralCoordinate& sCoord = coordsTo.spectralCoordinate(c);   
      typeTo = sCoord.frequencySystem();
   }
   {
      Coordinate::Type type = Coordinate::SPECTRAL;
      Int afterCoord = -1;
      Int c = coordsFrom.findCoordinate(type, afterCoord);
      if (c<0) {
         os << "No Spectral coordinate in 'from' CoordinateSystem" << LogIO::EXCEPTION;
      }
      const SpectralCoordinate& sCoord = coordsFrom.spectralCoordinate(c);   
      typeFrom = sCoord.frequencySystem();
   }
//
   const ObsInfo& obsInfoTo = coordsTo.obsInfo();
   const ObsInfo& obsInfoFrom = coordsFrom.obsInfo();
//   
   String telFrom = obsInfoFrom.telescope();
   String telTo = obsInfoTo.telescope();
   MPosition posFrom, posTo; 
   Bool found = MeasTable::Observatory(posFrom, telFrom);
   if (!found) {
      os << "Cannot lookup the observatory name " << telFrom << " in the AIPS++" << endl;
      os << "data base.  Please request that it be added" << LogIO::EXCEPTION;
   }
   found = MeasTable::Observatory(posTo, telTo);
   if (!found) {
      os << "Cannot lookup the observatory name " << telTo << " in the AIPS++" << endl;
      os << "data base.  Please request that it be added" << LogIO::EXCEPTION;
   }
//
   return makeFrequencyMachine(os, machine, typeTo, typeFrom,
                               dirTo, dirFrom, 
                               obsInfoTo.obsDate(), 
                               obsInfoFrom.obsDate(), 
                               posTo, posFrom);
}


Bool CoordinateUtil::makeFrequencyMachine(LogIO& os, MFrequency::Convert& machine,
                                          MFrequency::Types typeTo, MFrequency::Types typeFrom,
                                          const MDirection& dirTo, const MDirection& dirFrom,
                                          const MEpoch& epochTo, const MEpoch& epochFrom,
                                          const MPosition& posTo, const MPosition& posFrom)

{
// Create frames

   MeasFrame frameFrom;
   MeasFrame frameTo;

// Add Direction

   frameFrom.set(dirFrom);
   frameTo.set(dirTo);

// Add Epoch   

   Double t = epochFrom.getValue().get();
   if (t<0.0) {
      os << "In setting up the SpectralCoordinate conversion machinery" << endl;
      os << "The output CoordinateSystem has no valid epoch" << LogIO::EXCEPTION;
   }
   t = epochTo.getValue().get();
   if (t<0.0) {
      os << "In setting up the SpectralCoordinate conversion machinery" << endl;
      os << "The input CoordinateSystem has no valid epoch" << LogIO::EXCEPTION;
   }
   frameFrom.set(epochFrom);
   frameTo.set(epochTo);

// Add the position 

   frameFrom.set(posFrom);
   frameTo.set(posTo);

// Make the machine

   MFrequency::Ref refFrom(typeFrom, frameFrom);
   MFrequency::Ref refTo(typeTo, frameTo);
   machine = MFrequency::Convert(refFrom, refTo);

// Test a conversion

   Bool ok = True;
   MFrequency freqTo;
   Quantum<Double> freq(1.0e9, Unit(String("Hz")));
   MFrequency freqFrom(freq, typeFrom);
   try {
      freqTo = machine(freqFrom);
   } catch (AipsError x) {
      ok = False;
   }
   if (!ok) {
      os << "Unable to convert between the input and output SpectralCoordinates" << endl;
      os << "this probably means one is in the REST frame which requires" << endl;
      os << "the radial velocity - this is not implemented yet" << LogIO::EXCEPTION;
   }
//
   return ok;
}


Bool CoordinateUtil::isSky (LogIO& os, const CoordinateSystem& cSys)
{   
   const uInt nPixelAxes = cSys.nPixelAxes();
   if (nPixelAxes != 2) {
      os << "The CoordinateSystem is not two dimensional" << LogIO::EXCEPTION;
}  
   Bool xIsLong = True;
   Int dirCoordinate = cSys.findCoordinate(Coordinate::DIRECTION);
   if (dirCoordinate==-1) {
      os << "There is no DirectionCoordinate (sky) in this CoordinateSystem" << LogIO::EXCEPTION;
   }
//
   Vector<Int> dirPixelAxes = cSys.pixelAxes(dirCoordinate);
   if (dirPixelAxes(0) == -1 || dirPixelAxes(1) == -1) {
      os << "The pixel axes for the DirectionCoordinate have been removed" << LogIO::EXCEPTION;
   }
 
// Which axis is longitude and which is latitude

   if(dirPixelAxes(0)==0 && dirPixelAxes(1)==1) {
      xIsLong = True;
   } else {
      xIsLong = False;
   }
   return xIsLong;
} 


Bool CoordinateUtil::holdsSky (Bool& holdsOneSkyAxis, const CoordinateSystem& cSys, Vector<Int> pixelAxes) 
{
   AlwaysAssert(pixelAxes.nelements()==2, AipsError);
//
   holdsOneSkyAxis = False;
   Int dirCoordinate = cSys.findCoordinate(Coordinate::DIRECTION);
   if (dirCoordinate!=-1) {
      Vector<Int> dirPixelAxes = cSys.pixelAxes(dirCoordinate);
      if ( (dirPixelAxes(0)==pixelAxes(0) && dirPixelAxes(1)==pixelAxes(1)) ||
           (dirPixelAxes(0)==pixelAxes(1) && dirPixelAxes(1)==pixelAxes(0))) {
         return True;
      }
//
      if ( (pixelAxes(0)==dirPixelAxes(0) && pixelAxes(1)!=dirPixelAxes(1)) ||
           (pixelAxes(0)!=dirPixelAxes(0) && pixelAxes(1)==dirPixelAxes(1)) ||
           (pixelAxes(0)==dirPixelAxes(1) && pixelAxes(1)!=dirPixelAxes(0)) ||
           (pixelAxes(0)!=dirPixelAxes(1) && pixelAxes(1)==dirPixelAxes(0)) ) {
         holdsOneSkyAxis = True;
      }
   }
   return False;
}


void CoordinateUtil::setNicePreferredAxisLabelUnits(CoordinateSystem& cSys)
{  
   cSys.setPreferredWorldAxisUnits(cSys.worldAxisUnits());
//
   for (uInt i = 0; i < cSys.nCoordinates(); i++) {
     Coordinate::Type type = cSys.type(i);
     if (type==Coordinate::DIRECTION) {
        DirectionCoordinate coord(cSys.directionCoordinate(i));
        Vector<String> str(coord.nWorldAxes());
        for (uInt j = 0; j < str.nelements(); j++) str(j) = "deg";
        coord.setPreferredWorldAxisUnits(str);
        cSys.replaceCoordinate(coord, i);
     } else if (type==Coordinate::SPECTRAL) {
        SpectralCoordinate coord(cSys.spectralCoordinate(i));
        Vector<String> str(coord.nWorldAxes());
        for (uInt j = 0; j < str.nelements(); j++) str(j) = "km/s";
        coord.setPreferredWorldAxisUnits(str);
        cSys.replaceCoordinate(coord, i);
     }
   }
}


Bool CoordinateUtil::findSky(String&errorMessage, Int& dC, Vector<Int>& pixelAxes,
                             Vector<Int>& worldAxes, const CoordinateSystem& cSys)
//    
// Assumes only one DirectionCoordinate .   {pixel,world}Axes says where
// in the CS the DirectionCoordinate axes are
//
{
   CoordinateUtil::findDirectionAxes (pixelAxes, worldAxes, dC, cSys);
   if (dC<0 || pixelAxes.nelements()!=2 || worldAxes.nelements()!=2) {
      errorMessage = "Image does not have 2 sky coordinate axes";
      return False;
   }
// 
   for (uInt i=0; i<2; i++) {
      if (pixelAxes(i)==-1 || worldAxes(i)==-1) {
         errorMessage = "Image does not have 2 sky coordinate axes";
         return False;
      }
   }
//
   return True;
}


Stokes::StokesTypes CoordinateUtil::findSingleStokes (LogIO& os, const CoordinateSystem& cSys,
                                                      uInt pixel)
{  
   Stokes::StokesTypes stokes(Stokes::Undefined);
   Int stokesCoordinateNumber = cSys.findCoordinate(Coordinate::STOKES);
   if (stokesCoordinateNumber==-1) {
      os << LogIO::WARN
         << "There is no Stokes coordinate in the CoordinateSystem - assuming Stokes I"
         << LogIO::POST;
      stokes = Stokes::I;
   } else {
      StokesCoordinate stokesCoordinate = cSys.stokesCoordinate(stokesCoordinateNumber);
// 
// Find out what Stokes the specified pixel belongs to.  
// 
      if (!stokesCoordinate.toWorld(stokes, Int(pixel))) {
         os << "StokesCoordinate conversion failed because "
            << stokesCoordinate.errorMessage() << LogIO::EXCEPTION;
      }
   }
   return stokes;
}

String CoordinateUtil::formatCoordinate (const IPosition& pixel, CoordinateSystem& cSys)
{
   Vector<Double> pixel2(cSys.nPixelAxes());
   for (uInt i=0; i<pixel2.nelements(); i++) pixel2(i) = pixel(i);
//
   return CoordinateUtil::formatCoordinate(pixel2, cSys);
}
   

String CoordinateUtil::formatCoordinate (const Vector<Double>& pixel, CoordinateSystem& cSys)
{
   Vector<Double> world;
//
   if (!cSys.toWorld(world, pixel)) {
      String err = String("Error converting coordinate position because ") + cSys.errorMessage();
      throw(AipsError(err));
   }
//
   String s2;  
   for (uInt i=0; i<world.nelements(); i++) {
      String s, u;
      String tmp = cSys.format(u, Coordinate::DEFAULT, world(i), i,
                               True, True, -1);
//
      if (u.empty()) {
        s = tmp;
      } else {
        s = tmp + u;
      }
//
      if (i==0) {
         s2 += s;
      } else {
         s2 += String(", ") + s;
      }
   }
//
   return s2;
}


Int CoordinateUtil::compareCoordinates (const CoordinateSystem& thisCsys,
					const CoordinateSystem& thatCsys)
{
  // This is the real conformance checker.
  /////  return coordinates().nearPixel (other.coordinates());    
  // See how the coordinate systems compare.
  Vector<Int> thisWorldAxes;
  Vector<Int> thatWorldAxes;
  Vector<Bool> refChange;
  if (! thisCsys.worldMap (thatWorldAxes, thisWorldAxes,
			   refChange, thatCsys)) {
    return 9;
  }
  // This must be a subset of that or that a subset of this.
  // We are interested in pixel axes only, so transform the world axes
  // to pixel axes and remove world axes without pixel axes.
  Vector<Int> thisPixelAxes = toPixelAxes (thisCsys, thatCsys, thisWorldAxes);
  Vector<Int> thatPixelAxes = toPixelAxes (thatCsys, thisCsys, thatWorldAxes);
  // thisPixelAxes tells which pixel axes of this are part of that.
  // thatPixelAxes tells which pixel axes of that are part of this.
  // Check if the axes are in the correct order (ascending).
  // I.e. it is not supported that this and that have the same axes,
  // but in a different order.
  if (! checkOrder (thisPixelAxes)) {
    return 9;
  }
  if (! checkOrder (thatPixelAxes)) {
    return 9;
  }
  // Only one of the coordinate systems can be a subset.
  Bool thisIsSubSet = anyLT (thatPixelAxes, 0);
  Bool thatIsSubSet = anyLT (thisPixelAxes, 0);
  if (thisIsSubSet) {
    if (thatIsSubSet) {
      return 9;
    }
    return -1;
  } else if (thatIsSubSet) {
    return 1;
  }
  return 0;      //equal
}

Vector<Int> CoordinateUtil::toPixelAxes (const CoordinateSystem& thisCsys,
					const CoordinateSystem& thatCsys,
					const Vector<Int>& worldAxes)
{
  // Map the world axes to pixel axes.
  Vector<Int> pixelAxes(thisCsys.nPixelAxes(), -1);
  for (uInt i=0; i<worldAxes.nelements(); i++) {
    if (worldAxes(i) >= 0) {
      Int pixAxis = thisCsys.worldAxisToPixelAxis (i);
      if (pixAxis >= 0) {
	pixelAxes(pixAxis) = thatCsys.worldAxisToPixelAxis (worldAxes(i));
      }
    }
  }
  return pixelAxes;
}

Bool CoordinateUtil::checkOrder (const Vector<Int>& pixelAxes)
{
  // Check if the mapped axes are in ascending order. I.e. we do not allow
  // that the order of axes in 2 images is different.
  Int last = -1;
  for (uInt i=0; i<pixelAxes.nelements(); i++) {
    if (pixelAxes(i) >= 0) {
      if (pixelAxes(i) <= last) {
	return False;
      }
      last = pixelAxes(i);
    }
  }
  return True;
}


Bool CoordinateUtil::findExtendAxes (IPosition& newAxes,
				     IPosition& stretchAxes,
				     const IPosition& newShape,
				     const IPosition& oldShape,
				     const CoordinateSystem& newCsys,
				     const CoordinateSystem& oldCsys)
{
  Vector<Int> oldWorldAxes;
  Vector<Int> newWorldAxes;
  Vector<Bool> refChange;
  if (! oldCsys.worldMap (newWorldAxes, oldWorldAxes,
			  refChange, newCsys)) {
    return False;
  }
  // Old must be a subset of new.
  // We are interested in pixel axes only, so transform the world axes
  // to pixel axes and remove world axes without pixel axes.
  Vector<Int> oldPixelAxes = toPixelAxes (oldCsys, newCsys, oldWorldAxes);
  Vector<Int> newPixelAxes = toPixelAxes (newCsys, oldCsys, newWorldAxes);
  // oldPixelAxes tells which pixel axes of old are not part of new.
  // newPixelAxes tells which pixel axes of new are not part of old.
  // Check if the axes are in the correct order.
  if (! checkOrder (oldPixelAxes)) {
    return False;
  }
  if (! checkOrder (newPixelAxes)) {
    return False;
  }
  // Old must be a subset of new.
  if (anyLT (oldPixelAxes, 0)) {
    return False;
  }
  // Find the new and stretch axes.
  uInt nrdim = newPixelAxes.nelements();
  if (nrdim != newShape.nelements()) {
    return False;
  }
  newAxes.resize (nrdim);
  stretchAxes.resize (nrdim);
  uInt nrn = 0;
  uInt nrs = 0;
  for (uInt i=0; i<nrdim; i++) {
    if (newPixelAxes(i) < 0) {
      newAxes(nrn++) = i;
    } else {
      if (i-nrn > oldShape.nelements()) {
	return False;
      }
      if (oldShape(i-nrn) == 1  &&  newShape(i) > 1) {
	stretchAxes(nrs++) = i;
      }
    }
  }
  newAxes.resize (nrn);
  stretchAxes.resize (nrs);
  return True;
}
