//# ImageUtilities.cc:  Helper class for accessing images
//# Copyright (C) 1996,1997
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
//

#include <aips/Utilities/String.h>
#include <aips/Arrays/Vector.h>
#include <aips/OS/File.h>
#include <aips/OS/Path.h>
#include <aips/Tables/Table.h>
#include <aips/Tables/TableDesc.h>
#include <aips/Tables/SetupNewTab.h>
#include <trial/Images/ImageUtilities.h>
#include <trial/Coordinates/CoordinateSystem.h>


Bool ImageUtilities::setIncludeExclude (Vector<Float>& range, 
                                        Bool& noInclude,
                                        Bool& noExclude,
                                        const Vector<Double>& include,
                                        const Vector<Double>& exclude,
                                        ostream& os)
//
// Take the user's data inclusion and exclusion data ranges and
// generate the range and Booleans to say what sort it is
//
// Inputs:
//   include   Include range given by user. Zero length indicates
//             no include range
//   exclude   Exclude range given by user. As above.
//   os        Output stream for reporting
// Outputs:
//   noInclude If True user did not give an include range
//   noExclude If True user did not give an exclude range
//   range     A pixel value selection range.  Will be resized to
//             zero length if both noInclude and noExclude are True
//   Bool      True if successfull, will fail if user tries to give too
//             many values for includeB or excludeB, or tries to give
//             values for both
{
   noInclude = True;
   range.resize(0);
   if (include.nelements() == 0) {
//
   } else if (include.nelements() == 1) {
      range.resize(2);
      range(0) = -abs(include(0));
      range(1) =  abs(include(0));
      noInclude = False;
   } else if (include.nelements() == 2) {
      range.resize(2);
      range(0) = min(include(0),include(1));
      range(1) = max(include(0),include(1));
      noInclude = False;
   } else {
      os << endl << "Too many elements for argument include" << endl;
      return False;
   }

   noExclude = True;
   if (exclude.nelements() == 0) {
//
   } else if (exclude.nelements() == 1) {
      range.resize(2);
      range(0) = -abs(exclude(0));
      range(1) =  abs(exclude(0));
      noExclude = False;
   } else if (exclude.nelements() == 2) {
      range.resize(2);
      range(0) = min(exclude(0),exclude(1));
      range(1) = max(exclude(0),exclude(1));
      noExclude = False;
   } else {
      os << endl << "Too many elements for argument exclude" << endl;
      return False;
   }
   if (!noInclude && !noExclude) {  
      os << "You can only give one of arguments include or exclude" << endl;
      return False;
   }
   return True;
}


Bool ImageUtilities::setNxy (Vector<Int>& nxy,
                             ostream& os)
//
// Deal with the keyword setting the number of subplots
// in x and y
//
// Input:
//   os      Output stream
// Input/output
//   nxy     The Int vector containing the filled in inputs on output
// Output:
//   Bool    False if user gave something silly
//
{
   Int n = nxy.nelements();
   nxy.resize(2,True);
   if (n > 2) {
      os << "Too many elements for argument nxy" << endl;
      return False;
   } else if (n == 2) {
      nxy(0) = max(1,nxy(0));
      nxy(1) = max(1,nxy(1));
   } else if (n == 1) {
      nxy(0) = max(1,nxy(0));
      nxy(1) = nxy(0);
   } else {
      nxy(0) = 1;
      nxy(1) = 1;
   }
   return True;
}


Int ImageUtilities::inVector (const Int& target, 
                              const Vector<Int>& vector)
//
// Ascertain whether the values of a Vector<Int> contains a specified target
//
// Input:
//  target  The target integer
//  vector  The vector
// Output:
//  The index if found, else -1
//  
{
   for (Int i=0; i<vector.nelements(); i++) {
      if (target==vector(i)) return i;
   }
   return -1;
}



Bool ImageUtilities::setCursor (Int& nVirCursorIter,
                                IPosition& cursorShape, 
                                Vector<Int>& cursorAxes, 
                                const Lattice<Float>* pImage,
                                const Bool& optimumEntireImage,
                                const Int& maxDim,
                                ostream& os)
//
// From the cursorAxes array, set the cursorShape array which will be 
// used to define the Image iterator
//
// Input:
//   pImage       Pointer to the lattice we will iterate through
//   optimumEntireImage and maxDim
//                An aips++ image is stored in a tiled fashion and this means
//                it can be read optimally fast if you don't care about the
//                order in which data chunks are returned.
//
//                optimumEntireImage = True
//                   If the cursorAxes array requests the ENTIRE image then
//                   the cursor shape is set to that for which the image can be 
//                   iterated through optimally fast.   Otherwise, the cursor shape 
//                   is just set to reflect the values in cursorAxes
//
//                optimEntireImage = False The cursor shape is just set to
//                   reflect the values in cursorAxes.
//
//                   However, it may be additionally modified by the maxDim
//                   argument. This sets the maximum number of dimensions the 
//                   cursor shape can have.  This allows you to prevent huge arrays 
//                   being potentially allocated for the cursor if you are reading 
//                   large multi-dimensional images but the data order is important 
//                   (hence optimumEntireImage=False).  Setting, say, maxDim=2 limits 
//                   the cursor shape to a plane and you have to iterate through the 
//                   image rather trying to get it all in one chunk.  
//   os           Output stream for reporting purposes
// Input/output:
//   cursorAxes   Axes (0-relative) given by user.  These are the axes for which
//                (in general -- see optimumEntireImage and maxDim) the cursor 
//                shape is set to the corresponding axis size.  This signifies
//                which axes of the image we want to work something out from.
//                E.g. if we were reading a cube, and cursorAxes=0,1  we could 
//                work out statistics of each plane and display them as a 
//                function of plane number.
//
//                If cursorAxes is of zero length on input this means all axes 
//                and on output cursorAxes is filled in.  
//
//                Note that if we use optimumEntireImage or maxDim to set the
//                cursorShape, cursorAxes, on output, still reflects what it was
//                on input.  E.g. if on input cursorAxes is of size 0, the image is
//                a cube and maxDim=2, then on output cursorShape=[nx,ny,1] and
//                cursorAxes=[0,1,2] not just [0,1]  The reason for this is
//                that cursorAxes is further used to set the display axes by
//                ImageUtilities::setDisplayAxes (the complement of cursor axes)
//
// Outputs:
//   nVirCursorIter
//                The user's desired virtual cursor shape (set by cursorAxes) may not
//                be read in one iteration -- it may be capped by maxDim
//                or set according to optimumEntireImage.  This tells
//                how many actual cursor chunks fits into the virtual cursor
//                that the user asked for.  Fractional chunks count as one.
//   cursorShape  The shape of the cursor which will be used to iterate through
//                the image
//   Bool         True if successful
{
//
// First set the cursorAxes array 

   Int i;
   Int nImageDim = pImage->ndim();
   if (cursorAxes.nelements() == 0) {

// User didn't give any axes. 

      cursorAxes.resize(nImageDim); 
      for (i=0; i<nImageDim; i++) cursorAxes(i) = i;
   }
   else {

// Verify user's axes are ok

      if (!ImageUtilities::verifyAxes(pImage->shape().nelements(), cursorAxes, os)) 
         return False;
   }
 

// Now set the array giving the cursor shape for each axis of the image.
 
   cursorShape.resize (nImageDim, False);
   cursorShape = 1;

   if (optimumEntireImage) {
      if (cursorAxes.nelements() == nImageDim) {

// Set optimum image reading shape

         cursorShape = pImage->niceCursorShape(pImage->maxPixels());
      } else {

// Set axis size 

         for (i=0; i<nImageDim; i++) {
            if (ImageUtilities::inVector(i, cursorAxes) != -1) 
               cursorShape(i) = pImage->shape()(i);
         }
      }
   } else {

// Set axis size 

      for (i=0; i<nImageDim; i++) {
         if (ImageUtilities::inVector(i, cursorAxes) != -1) 
            cursorShape(i) = pImage->shape()(i);
      }

// Limit dimensionality

      Int nDim = 0;
      for (i=0; i<nImageDim; i++) {
         if (cursorShape(i) > 1) nDim++;
         if (nDim > maxDim) cursorShape(i) = 1;
      }
   }

// Find the number of iterations to get through the user's virtual cursor as opposed 
// to the possibly dimensionally capped one.  Overhanging iterations (or partial
// cursor shape fits) count as one full iteration

   IPosition virtualCursorShape(nImageDim);
   for (i=0; i<nImageDim; i++) {
      if (ImageUtilities::inVector(i, cursorAxes) != -1) 
         virtualCursorShape(i) = pImage->shape()(i);
      else
         virtualCursorShape(i) = 1;
   }
   nVirCursorIter = 1;
   for (i=0; i<nImageDim; i++) {
      Int iRat = Int(virtualCursorShape(i)/cursorShape(i));
      Int rem  = virtualCursorShape(i) - (iRat*cursorShape(i));
      if (rem > 0) iRat++;
      nVirCursorIter *= iRat;
   }

   return True;
}


Bool ImageUtilities::setCursor (Int& nVirCursorIter,
                                IPosition& cursorShape, 
                                Vector<Int>& cursorAxes, 
                                const Lattice<Double>* pImage,
                                const Bool& optimumEntireImage,
                                const Int& maxDim,
                                ostream& os)
//
// From the cursorAxes array, set the cursorShape array which will be 
// used to define the Image iterator
//
// Input:
//   pImage       Pointer to the lattice we will iterate through
//   optimumEntireImage and maxDim
//                An aips++ image is stored in a tiled fashion and this means
//                it can be read optimally fast if you don't care about the
//                order in which data chunks are returned.
//
//                optimumEntireImage = True
//                   If the cursorAxes array requests the ENTIRE image then
//                   the cursor shape is set to that for which the image can be 
//                   iterated through optimally fast.   Otherwise, the cursor shape 
//                   is just set to reflect the values in cursorAxes
//
//                optimEntireImage = False The cursor shape is just set to
//                   reflect the values in cursorAxes.
//
//                   However, it may be additionally modified by the maxDim
//                   argument. This sets the maximum number of dimensions the 
//                   cursor shape can have.  This allows you to prevent huge arrays 
//                   being potentially allocated for the cursor if you are reading 
//                   large multi-dimensional images but the data order is important 
//                   (hence optimumEntireImage=False).  Setting, say, maxDim=2 limits 
//                   the cursor shape to a plane and you have to iterate through the 
//                   image rather trying to get it all in one chunk.  
//   os           Output stream for reporting purposes
// Input/output:
//   cursorAxes   Axes (0-relative) given by user.  These are the axes for which
//                (in general -- see optimumEntireImage and maxDim) the cursor 
//                shape is set to the corresponding axis size.  This signifies
//                which axes of the image we want to work something out from.
//                E.g. if we were reading a cube, and cursorAxes=0,1  we could 
//                work out statistics of each plane and display them as a 
//                function of plane number.
//
//                If cursorAxes is of zero length on input this means all axes and on output
//                cursorAxes is filled in.  
//
//                Note that if we use optimumEntireImage or maxDim to set the
//                cursorShape, cursorAxes, on output, still reflects what it was
//                on input.  E.g. if on input cursorAxes is of size 0, the image is
//                a cube and maxDim=2, then on output cursorShape=[nx,ny,1] and
//                cursorAxes=[0,1,2] not just [0,1]  The reason for this is
//                that cursorAxes is further used to set the display axes by
//                ImageUtilities::setDisplayAxes (the complement of cursor axes)
//
// Outputs:
//   nVirCursorIter
//                The user's desired virtual cursor shape (set by cursorAxes) may not
//                be read in one iteration -- it may be capped by maxDim
//                or set according to optimumEntireImage.  This tells
//                how many actual cursor chunks fits into the virtual cursor
//                that the user asked for.  Fractional chunks count as one.
//   cursorShape  The shape of the cursor which will be used to iterate through
//                the image
//   Bool         True if successful
{
//
// First set the cursorAxes array 

   Int i;
   Int nImageDim = pImage->ndim();
   if (cursorAxes.nelements() == 0) {

// User didn't give any axes. 

      cursorAxes.resize(nImageDim); 
      for (i=0; i<nImageDim; i++) cursorAxes(i) = i;
   }
   else {

// Verify user's axes are ok

      if (!ImageUtilities::verifyAxes(pImage->shape().nelements(), cursorAxes, os)) 
         return False;
   }
 

// Now set the array giving the cursor shape for each axis of the image.
 
   cursorShape.resize (nImageDim, False);
   cursorShape = 1;

   if (optimumEntireImage) {
      if (cursorAxes.nelements() == nImageDim) {

// Set optimum image reading shape

         cursorShape = pImage->niceCursorShape(pImage->maxPixels());
      } else {

// Set axis size 

         for (i=0; i<nImageDim; i++) {
            if (ImageUtilities::inVector(i, cursorAxes) != -1) 
               cursorShape(i) = pImage->shape()(i);
         }
      }
   } else {

// Set axis size 

      for (i=0; i<nImageDim; i++) {
         if (ImageUtilities::inVector(i, cursorAxes) != -1) 
            cursorShape(i) = pImage->shape()(i);
      }

// Limit dimensionality

      Int nDim = 0;
      for (i=0; i<nImageDim; i++) {
         if (cursorShape(i) > 1) nDim++;
         if (nDim > maxDim) cursorShape(i) = 1;
      }
   }

// Find the number of iterations to get through the user's virtual cursor as opposed 
// to the possibly dimensionally capped one.  Overhanging iterations (or partial
// cursor shape fits) count as one full iteration

   IPosition virtualCursorShape(nImageDim);
   for (i=0; i<nImageDim; i++) {
      if (ImageUtilities::inVector(i, cursorAxes) != -1) 
         virtualCursorShape(i) = pImage->shape()(i);
      else
         virtualCursorShape(i) = 1;
   }
   nVirCursorIter = 1;
   for (i=0; i<nImageDim; i++) {
      Int iRat = Int(virtualCursorShape(i)/cursorShape(i));
      Int rem  = virtualCursorShape(i) - (iRat*cursorShape(i));
      if (rem > 0) iRat++;
      nVirCursorIter *= iRat;
   }

   return True;
}


Bool ImageUtilities::setCursor (Int& nVirCursorIter,
                                IPosition& cursorShape, 
                                Vector<Int>& cursorAxes, 
                                const Lattice<Int>* pImage,
                                const Bool& optimumEntireImage,
                                const Int& maxDim,
                                ostream& os)
//
// From the cursorAxes array, set the cursorShape array which will be 
// used to define the Image iterator
//
// Input:
//   pImage       Pointer to the lattice we will iterate through
//   optimumEntireImage and maxDim
//                An aips++ image is stored in a tiled fashion and this means
//                it can be read optimally fast if you don't care about the
//                order in which data chunks are returned.
// 
//                optimumEntireImage = True
//                   If the cursorAxes array requests the ENTIRE image then
//                   the cursor shape is set to that for which the image can be
//                   iterated through optimally fast.   Otherwise, the cursor shape 
//                   is just set to reflect the values in cursorAxes                     
//
//                optimEntireImage = False The cursor shape is just set to
//                   reflect the values in cursorAxes.           
//   
//                   However, it may be additionally modified by the maxDim
//                   argument. This sets the maximum number of dimensions the 
//                   cursor shape can have.  This allows you to prevent huge arrays 
//                   being potentially allocated for the cursor if you are reading 
//                   large multi-dimensional images but the data order is important 
//                   (hence optimumEntireImage=False).  Setting, say, maxDim=2 limits 
//                   the cursor shape to a plane and you have to iterate through the 
//                   image rather trying to get it all in one chunk.  
//   os           Output stream for reporting purposes
// Input/output:
//   cursorAxes   Axes (0-relative) given by user.  These are the axes for which
//                (in general -- see optimumEntireImage and maxDim) the cursor 
//                shape is set to the corresponding axis size.  This signifies
//                which axes of the image we want to work something out from.
//                E.g. if we were reading a cube, and cursorAxes=0,1  we could 
//                work out statistics of each plane and display them as a 
//                function of plane number.
//
//                If cursorAxes is of zero length on input this means all axes and on output
//                cursorAxes is filled in.  
//
//                Note that if we use optimumEntireImage or maxDim to set the
//                cursorShape, cursorAxes, on output, still reflects what it was
//                on input.  E.g. if on input cursorAxes is of size 0, the image is
//                a cube and maxDim=2, then on output cursorShape=[nx,ny,1] and
//                cursorAxes=[0,1,2] not just [0,1]  The reason for this is
//                that cursorAxes is further used to set the display axes by
//                ImageUtilities::setDisplayAxes (the complement of cursor axes)
//
// Outputs:
//   nVirCursorIter
//                The user's desired virtual cursor shape (set by cursorAxes) may not
//                be read in one iteration -- it may be capped by maxDim
//                or set according to optimumEntireImage.  This tells
//                how many actual cursor chunks fits into the virtual cursor
//                that the user asked for.  Fractional chunks count as one.
//   cursorShape  The shape of the cursor which will be used to iterate through
//                the image
//   Bool         True if successful
{
//
// First set the cursorAxes array 

   Int i;
   Int nImageDim = pImage->ndim();
   if (cursorAxes.nelements() == 0) {

// User didn't give any axes. 

      cursorAxes.resize(nImageDim); 
      for (i=0; i<nImageDim; i++) cursorAxes(i) = i;
   }
   else {

// Verify user's axes are ok

      if (!ImageUtilities::verifyAxes(pImage->shape().nelements(), cursorAxes, os)) 
         return False;
   }
 

// Now set the array giving the cursor shape for each axis of the image.
 
   cursorShape.resize (nImageDim, False);
   cursorShape = 1;

   if (optimumEntireImage) {
      if (cursorAxes.nelements() == nImageDim) {

// Set optimum image reading shape

         cursorShape = pImage->niceCursorShape(pImage->maxPixels());
      } else {

// Set axis size 

         for (i=0; i<nImageDim; i++) {
            if (ImageUtilities::inVector(i, cursorAxes) != -1) 
               cursorShape(i) = pImage->shape()(i);
         }
      }
   } else {

// Set axis size 

      for (i=0; i<nImageDim; i++) {
         if (ImageUtilities::inVector(i, cursorAxes) != -1) 
            cursorShape(i) = pImage->shape()(i);
      }

// Limit dimensionality

      Int nDim = 0;
      for (i=0; i<nImageDim; i++) {
         if (cursorShape(i) > 1) nDim++;
         if (nDim > maxDim) cursorShape(i) = 1;
      }
   }

// Find the number of iterations to get through the user's virtual cursor as opposed 
// to the possibly dimensionally capped one.  Overhanging iterations (or partial
// cursor shape fits) count as one full iteration

   IPosition virtualCursorShape(nImageDim);
   for (i=0; i<nImageDim; i++) {
      if (ImageUtilities::inVector(i, cursorAxes) != -1) 
         virtualCursorShape(i) = pImage->shape()(i);
      else
         virtualCursorShape(i) = 1;
   }
   nVirCursorIter = 1;
   for (i=0; i<nImageDim; i++) {
      Int iRat = Int(virtualCursorShape(i)/cursorShape(i));
      Int rem  = virtualCursorShape(i) - (iRat*cursorShape(i));
      if (rem > 0) iRat++;
      nVirCursorIter *= iRat;
   }

   return True;
}
 
    
Bool ImageUtilities::setCursor (Int& nVirCursorIter,
                                IPosition& cursorShape, 
                                Vector<Int>& cursorAxes, 
                                const Lattice<Complex>* pImage,
                                const Bool& optimumEntireImage,
                                const Int& maxDim,
                                ostream& os)
//
// From the cursorAxes array, set the cursorShape array which will be 
// used to define the Image iterator
//
// Input:
//   pImage       Pointer to the lattice we will iterate through
//   optimumEntireImage and maxDim
//                An aips++ image is stored in a tiled fashion and this means
//                it can be read optimally fast if you don't care about the
//                order in which data chunks are returned.
// 
//                optimumEntireImage = True
//                   If the cursorAxes array requests the ENTIRE image then
//                   the cursor shape is set to that for which the image can be
//                   iterated through optimally fast.   Otherwise, the cursor shape 
//                   is just set to reflect the values in cursorAxes                     
//
//                optimEntireImage = False The cursor shape is just set to
//                   reflect the values in cursorAxes.           
//   
//                   However, it may be additionally modified by the maxDim
//                   argument. This sets the maximum number of dimensions the 
//                   cursor shape can have.  This allows you to prevent huge arrays 
//                   being potentially allocated for the cursor if you are reading 
//                   large multi-dimensional images but the data order is important 
//                   (hence optimumEntireImage=False).  Setting, say, maxDim=2 limits 
//                   the cursor shape to a plane and you have to iterate through the 
//                   image rather trying to get it all in one chunk.  
//   os           Output stream for reporting purposes
// Input/output:
//   cursorAxes   Axes (0-relative) given by user.  These are the axes for which
//                (in general -- see optimumEntireImage and maxDim) the cursor 
//                shape is set to the corresponding axis size.  This signifies
//                which axes of the image we want to work something out from.
//                E.g. if we were reading a cube, and cursorAxes=0,1  we could 
//                work out statistics of each plane and display them as a 
//                function of plane number.
//
//                If cursorAxes is of zero length on input this means all axes and on output
//                cursorAxes is filled in.  
//
//                Note that if we use optimumEntireImage or maxDim to set the
//                cursorShape, cursorAxes, on output, still reflects what it was
//                on input.  E.g. if on input cursorAxes is of size 0, the image is
//                a cube and maxDim=2, then on output cursorShape=[nx,ny,1] and
//                cursorAxes=[0,1,2] not just [0,1]  The reason for this is
//                that cursorAxes is further used to set the display axes by
//                ImageUtilities::setDisplayAxes (the complement of cursor axes)
//
// Outputs:
//   nVirCursorIter
//                The user's desired virtual cursor shape (set by cursorAxes) may not
//                be read in one iteration -- it may be capped by maxDim
//                or set according to optimumEntireImage.  This tells
//                how many actual cursor chunks fits into the virtual cursor
//                that the user asked for.  Fractional chunks count as one.
//   cursorShape  The shape of the cursor which will be used to iterate through
//                the image
//   Bool         True if successful
{
//
// First set the cursorAxes array 

   Int i;
   Int nImageDim = pImage->ndim();
   if (cursorAxes.nelements() == 0) {

// User didn't give any axes. 

      cursorAxes.resize(nImageDim); 
      for (i=0; i<nImageDim; i++) cursorAxes(i) = i;
   }
   else {

// Verify user's axes are ok

      if (!ImageUtilities::verifyAxes(pImage->shape().nelements(), cursorAxes, os)) 
         return False;
   }
 

// Now set the array giving the cursor shape for each axis of the image.
 
   cursorShape.resize (nImageDim, False);
   cursorShape = 1;

   if (optimumEntireImage) {
      if (cursorAxes.nelements() == nImageDim) {

// Set optimum image reading shape

         cursorShape = pImage->niceCursorShape(pImage->maxPixels());
      } else {

// Set axis size 

         for (i=0; i<nImageDim; i++) {
            if (ImageUtilities::inVector(i, cursorAxes) != -1) 
               cursorShape(i) = pImage->shape()(i);
         }
      }
   } else {

// Set axis size 

      for (i=0; i<nImageDim; i++) {
         if (ImageUtilities::inVector(i, cursorAxes) != -1) 
            cursorShape(i) = pImage->shape()(i);
      }

// Limit dimensionality

      Int nDim = 0;
      for (i=0; i<nImageDim; i++) {
         if (cursorShape(i) > 1) nDim++;
         if (nDim > maxDim) cursorShape(i) = 1;
      }
   }

// Find the number of iterations to get through the user's virtual cursor as opposed 
// to the possibly dimensionally capped one.  Overhanging iterations (or partial
// cursor shape fits) count as one full iteration

   IPosition virtualCursorShape(nImageDim);
   for (i=0; i<nImageDim; i++) {
      if (ImageUtilities::inVector(i, cursorAxes) != -1) 
         virtualCursorShape(i) = pImage->shape()(i);
      else
         virtualCursorShape(i) = 1;
   }
   nVirCursorIter = 1;
   for (i=0; i<nImageDim; i++) {
      Int iRat = Int(virtualCursorShape(i)/cursorShape(i));
      Int rem  = virtualCursorShape(i) - (iRat*cursorShape(i));
      if (rem > 0) iRat++;
      nVirCursorIter *= iRat;
   }

   return True;
}
 
    


void ImageUtilities::setDisplayAxes (Vector<Int>& displayAxes, 
                                     const Vector<Int>& cursorAxes, 
                                     const Int& nImageDim)
//
// Set up array describing the display axes; these are the axes
// for which the result (e.g. statistics or histogram will
// be displayed as a function of) and are the complement of
// the cursor axes.
//
// Input:
//   cursorAxes     The cursor axes
//   nImageDim      Number of dimensions to image
// Output:
//   displayAxes    The display axes.  If the cursor axes
//                  are all of the image axes, there
//                  will be no display axes
{
   Int i,j;
   Int nCursorAxes = cursorAxes.nelements();

// See if the statistics axes are the full image, easy if so.

   if (nCursorAxes == nImageDim) 
      displayAxes.resize(0);
   else {

// The statistics axes are a subset of those in the image; set display axes

      displayAxes.resize(nImageDim, False);
      for (i=0,j=0; i<nImageDim; i++) {
         if (ImageUtilities::inVector(i, cursorAxes) == -1) {
            displayAxes(j) = i;
            j++;
         }
      }
      displayAxes.resize(j, True);
   }
}


Table ImageUtilities::setScratchTable (const String &inFileName, 
                                       const String &name)
//
// Return a Scratch Table where the Table names was constructed from 
// the directory of a given file,  a specified string and a unique 
// number worked out by this function.
//
// Scratch Tables are deleted when destructed
//
// Inputs
//   inFileName   The directory in which this file lives will be used
//                for the directory of the output Table
//   name         The name of the Table will be the directory/name_NNN
//                where NNN is a unique number
//
// Example:
//
//   Table table = ImageUtilities::setScratchTable(inImage.name(),
//                               String("Scratch_"));
//   PagedArray<Float> scratch(inImage.shape(), table);
//
//   and if inImage.name() was say "mydata/ngc1234" then
//   the table name would be "mydata/Scratch_NNN"
//
{
   File inputImageName(inFileName);
   const String path = inputImageName.path().dirName() + "/";
      
   Path fileName = File::newUniqueName(path, name);
   SetupNewTable setup(fileName.absoluteName(), TableDesc(), Table::Scratch);
   return Table(setup);
}


void ImageUtilities::setStorageImageShape(IPosition& storeImageShape,
                                          const Bool& last,
                                          const Int& axisSize,
                                          const Vector<Int>& displayAxes, 
                                          const IPosition& imageShape)
//
// Resize a storage image (currently an array lattice) as required
// given the display axes and the shape image being iterated through.
// The shape of the storage image is given by the sizes of the
// display axes plus one more goven by the user.  This extra one
// can be either the first or last axis.
//
// Input/output:
//   storeImageShape   The storage image shape. On input it just needs to be
//                     created.  On output it is resized and filled
// Inputs:
//   last              If true, it is the last axis of the storage shape
//                     that is of size axisSize.  If false, it is the
//                     first axis.
//   axisSize          The size of the first or last dimension of the
//                     storage image.
//   displayAxes       The display axes
//   imageShape        Image shape being iterated through
//
{
   Int nStoreImageDim = displayAxes.nelements() + 1;
   storeImageShape.resize(nStoreImageDim);

   if (last) {
      for (Int i=0; i<nStoreImageDim-1; i++) storeImageShape(i) = imageShape(displayAxes(i));
      storeImageShape(nStoreImageDim-1) = axisSize;
   } else {
      for (Int i=1; i<nStoreImageDim; i++) 
        storeImageShape(i) = imageShape(displayAxes(i-1));
      storeImageShape(0) = axisSize;
   }
}


  
Int ImageUtilities::findSpectralAxis (const IPosition& shape,
                                      const CoordinateSystem& coord)
//
// Return the index of the first axis in an image which is spectral
//
// Input:
//   shape    Shape of image
//   coord    Image coordinates
// Output:
//   the axis number by value.  -1 if there isn't one.
//                        
{

  Int spectralCoord = coord.findCoordinate(CoordinateSystem::SPECTRAL);
  if (spectralCoord < 0) {
    return -1;
  }
  
  // OK, we have one, find it!
   for (Int i=0; i<shape.nelements(); i++) {
     Int c, a;
     coord.findWorldAxis(c, a, i);
     if (c == spectralCoord) {
       return i;
     }
   }
   return -1;
}

String ImageUtilities::shortAxisName (const String& axisName)
//
// Look for "Right Ascension", "Declination", "Velocity",
// and "Frequency" in an axis string name and return "RA", 
// "Dec", "Vel", "Freq", respectively
// if these are found.  Anything else is returned as is.
// This will go away when I do something in the Coordinates
// classes to return long, short and FITS names
//
{
   String temp = axisName;
   temp.upcase();
   if (temp.contains("RIGHT ASCENSION")) {
     temp = "RA";
   } else if (temp.contains("DECLINATION")) {
     temp = "Dec";
   } else if (temp.contains("VELOCITY")) {
     temp = "Vel";
   } else if (temp.contains("FREQUENCY")) {
     temp = "Freq";
   } else {
     temp = axisName;
   }
   return temp;
}

Bool ImageUtilities::verifyAxes (const Int& nDim,
                                 const Vector<Int>& axes,
                                 ostream& os)
//
// Verify that an array of axes are valid for this image.
// That is make sure they are in the range 0 -> nDim-1
//
// Input:
//   nDim      Number of dimensions of image
//   axes      The axes
//   os        Output stream for reporting purposes
// Output
//   False if illegal
//   True  if   legal
//
{ 
   Int axMax = nDim - 1;
   Int nAxes = axes.nelements();

   for (Int i=0; i<nAxes; i++) {
      if (axes(i)<0 || axes(i)>axMax) {
         os << "Invalid axis given" << endl;
         return False;
      }

   for (Int j=i+1; j<nAxes; j++){
      if (axes(i) == axes(j)) {
         os << "Degenerate axes given" << endl;
         return False;
      }
    }
  }
  return True;
}


void ImageUtilities::stretchMinMax (Float& dMin, 
                                    Float& dMax)
//
// Stretch a range by 5%
//
// Input/output:
//   dMin,Max     The range to stretch
//
{
   Float delta = 0.05*(dMax-dMin);
   Float absmax = max(abs(dMax),abs(dMin));
   if (delta < 1.0e-5*absmax) delta = 0.01 * absmax;

   if (dMin==dMax) {
      if (dMin==0.0) {
         dMin = -1.0; 
         dMax = 1.0;
      }
      else {
         dMin = dMin - 0.05*dMin; 
         dMax = dMax + 0.05*dMax;
      }
   }
   else {
      dMin = dMin - delta; 
      dMax = dMax + delta;
   }
}

Vector<String> ImageUtilities::getStrings (const String& stringIn)
//
// Extract from a string consisting of comma delimitered
// substrings, an array of strings. 
//
// Inputs:
//   stringIn    String containing comma or space delimitered substrings
//
{
   Vector<String> strings;
   String subString;
   Int i = 0;
   Bool init = True;
   while (getNextSubString (subString, stringIn, init)) {
      init = False;
      strings.resize(i+1,True);
      strings(i) = subString;
      i++;
   }
   return strings;
   
}


Bool ImageUtilities::getNextSubString (String& subString,
                                       const String& inString,
                                       const Bool& init)
//
// Find the next comma or white-space delimitered substring in a string
//
// Inputs:
//  inString  The string
//  init      If True, start from beginning of string, else
//            start from the end of the last substring that was
//            found
// Output
//  substring The next substring found
//  Bool      False if there were no more substrings
//
{

// Copy input string so I can avoid the compiler moaning about
// constant strings

   String string = inString;

// Initialize character pointer

   static Int iStart=0;
   if (init) iStart = 0;
   Int iLen = string.length();


// No more characters left

   if (iStart >= iLen) {
      subString = "";
      return False;
   }

// Discard white space

   while (iStart< iLen && String(string.elem(iStart)) == " ") {iStart++;}

// There is nothing but white space at the end of the last string

   if (iStart == iLen) return False;

 
// Find next substring

   for (Int i=iStart; i<iLen; i++) {
      String c(string.elem(i));
      if (c == "," || c == " ") {
         Int n = i - iStart;
         subString = String(string(iStart,n));
         iStart = i + 1;

         return True;
      }
   }
  

// Substring extends to end of string

   Int n = iLen - iStart;
   subString = String(string(iStart,n));
   iStart = iLen;
   return True;
}



