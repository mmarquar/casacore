//# UnitMap3.cc: Unit map SI unit initialisation part 1
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

#include <aips/Quanta/UnitMap.h>

//# constants

// Initialise the maps
void UnitMap::initUMSI1() {
  for (Int i=0; i<UnitDim::Dnumber; i++) {
    UnitMap::mapDef->define(UnitDim::dimName(i),
			    UnitName(UnitDim::dimName(i),
				     UnitVal(1.0,i),
				     UnitDim::dimFull(i)));
    
    // SI units
    if (i != UnitDim::Dkg) {
      UnitMap::mapSI->define(UnitDim::dimName(i),
			     UnitName(UnitDim::dimName(i),
				      UnitVal(1.0,i),
				      UnitDim::dimFull(i)));
    };
  };
  
  UnitMap::mapSI->define("$",	UnitName("$",
					 UnitVal(1.,UnitDim::Dnon),
					 "currency"));
  UnitMap::mapSI->define("%",	UnitName("%",
					 UnitVal(0.01),
					 "percent"));
  UnitMap::mapSI->define("%%",	UnitName("%%",
					 UnitVal(0.001),
					 "permille"));
  UnitMap::mapSI->define("g",	UnitName("g",
					 UnitVal(0.001,UnitDim::Dkg),
					 "gram"));
  UnitMap::mapSI->define("Bq",  UnitName("Bq",
					 UnitVal(1.,"s-1"),
					 "becquerel"));
  UnitMap::mapSI->define("Hz",  UnitName("Hz",
					 UnitVal(1.,"s-1"),
					 "hertz"));
  UnitMap::mapSI->define("C",   UnitName("C",
					 UnitVal(1.,"A.s"),
					 "coulomb"));
  UnitMap::mapSI->define("lm",  UnitName("lm",
					 UnitVal(1.,"cd.sr"),
					 "lumen"));
  UnitMap::mapSI->define("N",	UnitName("N",
					 UnitVal(1.,"kg.m.s-2"),
					 "newton"));
  UnitMap::mapSI->define("J",   UnitName("J",
					 UnitVal(1.,"N.m"),
					 "joule"));
  UnitMap::mapSI->define("W",   UnitName("W",
					 UnitVal(1.,"J.s-1"),
					 "watt"));
  UnitMap::mapSI->define("V",   UnitName("V",
					 UnitVal(1.,"W.A-1"),
					 "volt"));
  UnitMap::mapSI->define("F",   UnitName("F",
					 UnitVal(1.,"C.V-1"),
					 "farad"));
  UnitMap::mapSI->define("Gy",  UnitName("Gy",
					 UnitVal(1.,"J.kg-1"),
					 "gray"));
  UnitMap::mapSI->define("lx",  UnitName("lx",
					 UnitVal(1.,"lm.m-2"),
					 "lux"));
  UnitMap::mapSI->define("Ohm", UnitName("Ohm",
					 UnitVal(1.,"V.A-1"),
					 "ohm"));
  UnitMap::mapSI->define("Pa",  UnitName("Pa",
					 UnitVal(1.,"N.m-2"),
					 "pascal"));
  UnitMap::mapSI->define("S",   UnitName("S",
					 UnitVal(1.,"Ohm-1"),
					 "siemens"));
  UnitMap::mapSI->define("Sv",  UnitName("Sv",
					 UnitVal(1.,"J.kg-1"),
					 "sievert"));
  UnitMap::mapSI->define("Wb",  UnitName("Wb",
					 UnitVal(1.,"V.s"),
					 "weber"));
  UnitMap::mapSI->define("H",   UnitName("H",
					 UnitVal(1.,"Wb.A-1"),
					 "henry"));
  UnitMap::mapSI->define("T",   UnitName("T",
					 UnitVal(1.,"Wb.m-2"),
					 "tesla"));
}  
