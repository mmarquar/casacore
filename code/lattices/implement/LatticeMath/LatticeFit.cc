//# LatticeFit.cc: Fit every line of pixels parallel to any axis in a Lattice.
//# Copyright (C) 1994,1995,1999,2000,2001,2002
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

#include <trial/Fitting/LatticeFit.h>

#include <aips/Functionals/Function.h>
#include <aips/Lattices/Lattice.h>
#include <aips/Lattices/LatticeIterator.h>
#include <aips/Lattices/LatticeStepper.h>
#include <aips/Arrays/IPosition.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Glish/GlishEvent.h>
#include <aips/Glish/GlishArray.h>
#include <aips/Exceptions/Error.h>

uInt baselineFit(Lattice<Float> &outImage,
		 Vector<Float> &fittedParameters,
		 LQLinearFit<Float> &fitter, 
		 const Lattice<Float> &inImage,
		 uInt whichAxis,
		 const Vector<Bool> &fitMask,
		 Bool returnResiduals,
		 GlishSysEventSource *eventStream)
{
    IPosition outShape = outImage.shape();
    IPosition inShape = inImage.shape();

    if (outShape != inShape) {
	throw(AipsError("::baselineFit - outImage.shape() != inImage.shape()"));
    }

    if (whichAxis >= outImage.ndim()) {
	throw(AipsError("::baselineFit - whichAxis does not exist in image"));
    }
    if (Int(fitMask.nelements()) != outShape(whichAxis)) {
	throw(AipsError("::baselineFit - improperly specified mask"));
    }

    // reset any visual or other displays
    if (eventStream && eventStream->connected()) {
	eventStream->postEvent("progress", GlishArray(0.0));
    }


    // These selections etc will get easier when masked arrays are available.
    Int nPointsToFit = fitMask.nelements();

    // Set up x and sigma
    Vector<Float> x(nPointsToFit);
    Vector<Float> y(nPointsToFit);
    Vector<Float> sigma(nPointsToFit);

    Int count, i;

    // data points with sigma = -1.0 are ignored in fitting
    for (count = 0, i = 0; i < nPointsToFit; i++) {
      if (fitMask(i)) {
	x(i) = count;
	count++;
	sigma(i) = 1.0;
      } else {
	sigma(i) = -1.0;
      }
    }

    // For simplicity this now just iterates through the cube "line by line".
    // It might be considerably more efficient to iterate through plane by
    // plane though (earlier versions of the code did this, however it has
    // been changed to get it working quickly).

    IPosition cursorShape(outShape.nelements());
    cursorShape = 1;
    cursorShape(whichAxis) = inShape(whichAxis);

    LatticeIterator<Float>    outIter(outImage, cursorShape);
    RO_LatticeIterator<Float> inIter(inImage, cursorShape);

    Vector<Float> xall(inShape(whichAxis));
    indgen(xall);
    Vector<Float> solution(xall.nelements());
    Vector<Float> yall(xall.nelements());

    count = 0;
    uInt nspectra = inImage.nelements() / inShape(whichAxis);

    fittedParameters.resize(0);
    for (inIter.reset(), outIter.reset(); 
	 ! inIter.atEnd(); inIter++, outIter++, count++) {
        yall = inIter.vectorCursor();
	fittedParameters=fitter.fit(x, yall, sigma);
	for (uInt ii=0; ii < solution.nelements(); ii++) {
	    solution(ii) = (*fitter.fittedFunction())(xall(ii)).value();
	}
	if (returnResiduals) {
	    outIter.woVectorCursor() = (yall - solution);
	} else {
	    outIter.woVectorCursor() = solution;
	}
	if (eventStream && eventStream->connected()) {
	    eventStream->postEvent("progress",
				   GlishArray(Double(count)/Double(nspectra)));
	}
    }

    if (eventStream && eventStream->connected()) {
	eventStream->postEvent("progress", GlishArray(1.0));
    }
    return count;
}

