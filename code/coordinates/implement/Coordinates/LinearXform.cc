//# <ClassFileName.h>: this defines <ClassName>, which ...
//# Copyright (C) 1997,1998,1999
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

#include <trial/Coordinates/LinearXform.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Arrays/MatrixMath.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Mathematics/Math.h>
#include <aips/Utilities/Assert.h>
#include <aips/Utilities/LinearSearch.h>
#include <wcslib/lin.h>
#include <stdlib.h>


LinearXform::LinearXform(uInt naxis)
  : linprm_p(make_linprm(naxis)),
    isPCDiagonal_p(True)
{
    AlwaysAssert(linprm_p, AipsError);
    Vector<Double> crpix(naxis);
    Vector<Double> cdelt(naxis);
    cdelt = 1.0;
    crpix = 0.0;
    Matrix<Double> pc(naxis,naxis);
    pc = 0.0; 
    if(naxis > 1){
       pc.diagonal() = 1.0;
    }else{
       pc = 1.0; 
    }
    set_linprm(linprm_p, crpix, cdelt, pc);
}

LinearXform::LinearXform(const Vector<Double> &crpix,
                         const Vector<Double> &cdelt)
  : linprm_p(make_linprm(crpix.nelements())),
    isPCDiagonal_p(True)
{
    AlwaysAssert(linprm_p, AipsError);
    uInt naxis = cdelt.nelements();
    AlwaysAssert(crpix.nelements() == naxis, AipsError);
    Matrix<Double> pc(naxis,naxis);
    pc = 0.0;
    pc.diagonal() = 1.0;
    set_linprm(linprm_p, crpix, cdelt, pc);
}

LinearXform::LinearXform(const Vector<Double> &crpix,
                         const Vector<Double> &cdelt,
			 const Matrix<Double> &pc)
  : linprm_p(make_linprm(crpix.nelements()))
{
    AlwaysAssert(linprm_p, AipsError);
    uInt naxis = cdelt.nelements();
    AlwaysAssert(crpix.nelements() == naxis && pc.nrow() == naxis &&
		 pc.ncolumn() == naxis, AipsError);
    set_linprm(linprm_p, crpix, cdelt, pc);
//
    Double zero = 0.0;
    Double tol = 1e-12;

// See if pc is diagonal.  We do this purely for
// use in the Fourier inversion stuff.  Urk.

    isPCDiagonal_p = True;
    for (uInt j=0; j<pc.nrow(); j++) {
       for (uInt i=0; i<pc.ncolumn(); i++) {
         if (i!=j && !::near(pc(j,i),zero,tol)) {
            isPCDiagonal_p = False;
            break;
         }
       }
    }
}

LinearXform::LinearXform(const LinearXform &other)
  : linprm_p(make_linprm(other.linprm_p->naxis)),
    isPCDiagonal_p(other.isPCDiagonal_p)
{
    AlwaysAssert(linprm_p, AipsError);
    set_linprm(linprm_p, other.crpix(), other.cdelt(), other.pc());
}

LinearXform &LinearXform::operator=(const LinearXform &other)
{
    if (this != &other) {
        delete_linprm(linprm_p);
	linprm_p = make_linprm(other.linprm_p->naxis);
        isPCDiagonal_p = other.isPCDiagonal_p;
	AlwaysAssert(linprm_p, AipsError);
	set_linprm(linprm_p, other.crpix(), other.cdelt(), other.pc());
    }
    return *this;
}

LinearXform::~LinearXform()
{
    delete_linprm(linprm_p);
}

uInt LinearXform::nWorldAxes() const
{
    return linprm_p->naxis;
}

Bool LinearXform::forward(const Vector<Double> &world, 
				Vector<Double> &pixel, String &errorMsg) const
{
    const uInt max_size = 10;
    uInt naxis = world.nelements();
    AlwaysAssert(naxis <= max_size, AipsError);
    pixel.resize(naxis);

    // We could optimize this to directly use the storage in world and pixel if
    // it is contiguous. Optimize if necessary.
    double tmpWorld[max_size], tmpPixel[max_size];
    uInt i;
    for (i=0; i<naxis; i++) {
        tmpWorld[i] = world(i);
    }
    int errnum = linfwd(tmpWorld, linprm_p, tmpPixel);
    if (errnum) {
        errorMsg = "wcs linfwd_error: ";
	errorMsg += linfwd_errmsg[errnum];
	return False;
    }
    for (i=0; i<naxis; i++) {
        pixel(i) = tmpPixel[i];
    }
    return True;
}

Bool LinearXform::reverse(      Vector<Double> &world, 
                          const Vector<Double> &pixel, 
                          String &errorMsg) const
{
    const uInt max_size = 10;
    uInt naxis = pixel.nelements();
    AlwaysAssert(naxis <= max_size, AipsError);
    world.resize(naxis); 

    // We could optimize this to directly use the storage in world and pixel if
    // it is contiguous. Optimize if necessary.
    double tmpWorld[max_size], tmpPixel[max_size];
    uInt i;
    for (i=0; i<naxis; i++) {
	tmpPixel[i] = pixel(i);
    }
    int errnum = linrev(tmpPixel, linprm_p, tmpWorld);
    if (errnum) {
        errorMsg = "wcs linreb_error: ";
	errorMsg += linrev_errmsg[errnum];
	return False;
    }
    for (i=0; i<naxis; i++) {
        world(i) = tmpWorld[i];
    }
    return True;
}

Vector<Double> LinearXform::crpix() const
{
    uInt naxis = linprm_p->naxis;
    Vector<Double> tmp(naxis);
    for (uInt i=0; i<naxis; i++) {
        tmp(i) = linprm_p->crpix[i];
    }
    return tmp;
}

Vector<Double> LinearXform::cdelt() const
{
    uInt naxis = linprm_p->naxis;
    Vector<Double> tmp(naxis);
    for (uInt i=0; i<naxis; i++) {
        tmp(i) = linprm_p->cdelt[i];
    }
    return tmp;
}

Matrix<Double> LinearXform::pc() const
{
    uInt naxis = linprm_p->naxis;
    Matrix<Double> tmp(naxis,naxis);
    uInt count = 0;
    for (uInt i=0; i<naxis; i++) {
        for (uInt j=0; j<naxis; j++,count++) {
	  tmp(j,i) = linprm_p->pc[count];
	}
    }
    return tmp;
}

void LinearXform::crpix(const Vector<Double> &newvals)
{
    AlwaysAssert(newvals.nelements() == nWorldAxes(), AipsError);
    *this = LinearXform(newvals, cdelt(), pc());
}

void LinearXform::cdelt(const Vector<Double> &newvals)
{
    AlwaysAssert(newvals.nelements() == nWorldAxes(), AipsError);
    *this = LinearXform(crpix(), newvals, pc());
}

void LinearXform::pc(const Matrix<Double> &newvals)
{
    AlwaysAssert(newvals.nrow() == nWorldAxes() &&
		 newvals.ncolumn() == newvals.nrow(), AipsError);
    *this = LinearXform(crpix(), cdelt(), newvals);
}

Bool LinearXform::near(const LinearXform& other,
                       Double tol) const
{
   Vector<Int> excludeAxes;
   return near(other, excludeAxes, tol);
}


Bool LinearXform::near(const LinearXform& other,
                       const Vector<Int>& excludeAxes,
                       Double tol) const
{

// Number of pixel and world axes is the same for a LinearXform

   Vector<Bool> exclude(nWorldAxes());
   exclude = False;
   Bool found;
   uInt j = 0;
   uInt i;
   for (i=0; i<nWorldAxes(); i++) {
      if (linearSearch(found, excludeAxes, Int(i), excludeAxes.nelements()) >= 0)
        exclude(j++) = True;
   }


// Compare reference pixels and increments

   Vector<Double> d1 = this->crpix();
   Vector<Double> d2 = other.crpix();
   if (d1.nelements() != d2.nelements()) return False;
   for (i=0; i<d1.nelements(); i++) {
      if (!exclude(i)) {
         if (!::near(d1(i),d2(i),tol)) return False;
      }
   }
 
   d1 = this->cdelt();
   d2 = other.cdelt();
   if (d1.nelements() != d2.nelements()) return False;
   for (i=0; i<d1.nelements(); i++) {
      if (!exclude(i)) {
         if (!::near(d1(i),d2(i),tol)) return False;
      }
   }

// Check the matrix
                                           
   Matrix<Double> pc1 = this->pc();
   Matrix<Double> pc2 = other.pc();
   if (pc1.nrow()    != pc2.nrow())    return False;
   if (pc1.ncolumn() != pc2.ncolumn()) return False;

// Compare row by row.  An axis will turn up in the PC
// matrix in any row or column with that number. E.g.,
// values pertaining to axis "i" will be found in all 
// entries of row "i" and all entries of column "i"

   for (j=0; j<pc1.nrow(); j++) {
      Vector<Double> r1 = pc1.row(j);
      Vector<Double> r2 = pc2.row(j);
      if (!exclude(j)) {
         for (i=0; i<r1.nelements(); i++) {
            if (!exclude(i)) {
               if (!::near(r1(i),r2(i),tol)) return False;
            }
         }
      }
   }
 
   return True;
}


linprm* LinearXform::make_linprm(int naxis) const
{
  linprm *tmp = new linprm;
  if (! tmp) {
      return 0;
  }

  tmp->flag  = 0;
  tmp->naxis = naxis;
 
 tmp->crpix = new double[naxis];
  if (! tmp->crpix) {
      delete tmp; return 0;
  }
 
  tmp->pc = new double[naxis*naxis];
  if (! tmp->pc) {
      delete [] tmp->crpix;
      delete tmp; 
      return 0;
  }

  tmp->cdelt = new double[naxis];
  if (! tmp->cdelt){
      delete[] tmp->pc; 
      delete[] tmp->crpix; 
      delete tmp; 
      return 0;
  }

  tmp->piximg = 0; // linset will set these
  tmp->imgpix = 0;
  return tmp;
}

void LinearXform::delete_linprm(linprm *&to) const
{
  if (to) {
      delete [] to->crpix;
      delete [] to->cdelt;
      delete [] to->pc;
      free(to->piximg);
      free(to->imgpix);
      delete to;
  }
  to = 0;
}

void LinearXform::set_linprm(linprm *to, const Vector<double> &crpix,
                             const Vector<double> &cdelt, const Matrix<double> &pc) const
{
    uInt naxis = crpix.nelements();
    DebugAssert(naxis == cdelt.nelements() &&
		naxis == pc.ncolumn() && naxis == pc.nrow(), AipsError);

    uInt count = 0;
    for (uInt i=0; i<naxis; i++) {
        to->crpix[i] = crpix(i);
        to->cdelt[i] = cdelt(i);
	for (uInt j=0; j<naxis; j++) {
  	    to->pc[count] = pc(j,i);
	    count++;
	}
    }
    to->flag = 0;
    int err = linset(to);
    if (err) {
        String errmsg = "wcs linset_error: ";
	errmsg += linset_errmsg[err];
	throw(AipsError(errmsg));
    }
}


LinearXform LinearXform::fourierInvert (const Vector<Bool>& axes, 
                                        const Vector<Double>& crpix, 
                                        const Vector<Double>& scale) const
{
   if (axes.nelements() != nWorldAxes()) {
      throw (AipsError("axes length is invalid"));
   }
   if (crpix.nelements() != nWorldAxes()) {
      throw (AipsError("crpix length is invalid"));
   }
   if (scale.nelements() != nWorldAxes()) {
      throw (AipsError("scale length is invalid"));
   }
//
   Matrix<Double> pc0;
   if (isPCDiagonal_p) {

// Short cut which enables us to separate out axes

      pc0 = pc();
      Vector<Double> d = pc0.diagonal();
      for (uInt i=0; i<nWorldAxes(); i++) {
         if (axes(i)) d(i) = 1.0 / d(i);
      }
      pc0.diagonal() = d;
   } else {
      if (!allEQ(axes, True)) {
         throw(AipsError("Cannot invert non-diagonal PC matrix (probably a rotated CoordinateSystem) when some axes not being transformed"));
      }
//
      pc0 = invert(pc());
   }
//
   Vector<Double> cdelt0 = cdelt();   
   Vector<Double> crpix0 = LinearXform::crpix();
   for (uInt i=0; i<nWorldAxes(); i++) {
      if (axes(i)) {
         cdelt0(i) = scale(i) / cdelt0(i);
         crpix0(i) = crpix(i);
      }
   }
   return LinearXform(crpix0, cdelt0, pc0);
}
