//# Spectral2Element.cc: Record conversion for SpectralElement
//# Copyright (C) 2001
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

//# Includes
#include <trial/Wnbt/SpectralElement.h>

#include <aips/Arrays/ArrayMath.h>
#include <aips/Containers/RecordInterface.h>
#include <aips/Containers/RecordFieldId.h>
#include <aips/Containers/Record.h>
#include <aips/Mathematics/Constants.h>
#include <aips/Utilities/String.h>
#include <aips/Utilities/DataType.h>

//# Member functions 
Bool SpectralElement::toRecord(String &error, RecordInterface &out) const {
  out.define(RecordFieldId("type"), fromType(tp_p));
  out.define(RecordFieldId("parameters"), par_p);  
  out.define(RecordFieldId("errors"), err_p);
  return True;
}

Bool SpectralElement::fromRecord(String &error, const RecordInterface &in) {
  if (in.isDefined("type") && in.isDefined("parameters") &&
      in.type(in.idToNumber(RecordFieldId("type"))) == TpString) {
    String stp;
    SpectralElement::Types tp;
    in.get(RecordFieldId("type"), stp);
    if (!SpectralElement::toType(tp, stp)) {
      error += String("Unknown spectral type in SpectralElement::fromRecord\n");
      return False;
    }

// Accomodate possibility of record coming from Glish in any old type

    Vector<Double> param;
    DataType dataType = in.dataType("parameters");
    if (dataType==TpArrayDouble) {
       in.get("parameters", param);
    } else if (dataType==TpArrayFloat) {
       Vector<Float> v;
       in.get("parameters", v);
       param.resize(v.nelements());
       convertArray(param,v);
    } else if (dataType==TpArrayInt) {
       Vector<Int> v;
       in.get("parameters", v);
       param.resize(v.nelements());
       convertArray(param,v);
    } else {
      error += String("Parameters field must be double, float or int\n");
      return False;
    }
//
    if (tp == GAUSSIAN) {
      if (param.nelements() != 3) {
	error += String("Illegal number of parameters for Gaussian element\n");
	return False;
      };
      if (param(2) <= 0.0) {
	error += String("The width of a gaussian element must be positive\n");
	return False;
      };
      par_p.resize(3);
    } else if (tp_p == POLYNOMIAL) {
      if (param.nelements() == 0) {
	error += String("Polynomial spectral element must have order "
			"of at least zero");
	return False;
      };
      n_p = param.nelements()-1;
      par_p.resize(n_p+1);
    };
//
    par_p = param;
    return True;
  };
  error += String("Illegal spectral element record in fromRecord\n");
  return False;
}

Bool SpectralElement::fromString(String &error, const String &in) {
  SpectralElement::Types tp;
  if (!SpectralElement::toType(tp, in)) {
    error += String("Unknown spectral type in SpectralElement::fromString\n");
    return False;
  };
  if (tp == GAUSSIAN) {
    par_p.resize(3);
    par_p(0) = 1.0;
    par_p(1) = 0.0;
    par_p(2) = 2*sqrt(C::ln2)/C::pi;
  } else if (tp_p == POLYNOMIAL) {
    n_p = 0;
    par_p.resize(n_p);
    par_p = 0;
  };
  return True;
}
