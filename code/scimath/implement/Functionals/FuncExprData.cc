//# FuncExprData.cc: Data and enumerations for functional expressions
//# Copyright (C) 2001,2002
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

//# Includes
#include <trial/Functionals/FuncExprData.h>
#include <aips/iostream.h>
#include <aips/iomanip.h>

//# Constructors
FuncExprData::FuncExprData() :
  una2_p(), una1_p(), bin2_p(), bin1_p(),
  spop_p(), func_p() {
  static const ExprOperator olist[] = {
    {UNAMIN, 	"-",		UNA1,	RTLPRI+4, 	1, 0, 0, NONE},
    {UNAPLUS, 	"+",	   	UNA1,	RTLPRI+4, 	1, 0, 0, NONE},
    {NON, 	"!",		UNA1,	28, 		1, 0, 0, NONE},
    {POW, 	"**",		BIN2,	RTLPRI,		2, 0, 1, NONE},
    {GTE, 	">=",		BIN2,	32,		2, 0, 1, NONE},
    {LTE, 	"<=",		BIN2,	32, 		2, 0, 1, NONE},
    {EQ, 	"==",		BIN2,	32, 		2, 0, 1, NONE},
    {NEQ, 	"!=",		BIN2,	32, 		2, 0, 1, NONE},
    {OR, 	"||",		BIN2,	20, 		2, 0, 1, NONE},
    {AND, 	"&&",		BIN2,	20, 		2, 0, 1, NONE},
    {ADD, 	"+",		BIN1,	36, 		2, 0, 1, NONE},
    {SUB, 	"-",		BIN1,	36, 		2, 0, 1, NONE},
    {MUL, 	"*",		BIN1,	40, 		2, 0, 1, NONE},
    {DIV, 	"/",		BIN1,	40, 		2, 0, 1, NONE},
    {POW, 	"^",		BIN1,	RTLPRI, 	2, 0, 1, NONE},
    {GT, 	">",		BIN1,	32, 		2, 0, 1, NONE},
    {LT, 	"<",		BIN1,	32, 		2, 0, 1, NONE},
    {CONST, 	"CONST",	SPEC,	SPCPRI,		0, 0,-1, NONE},
    {PARAM, 	"PARAM",	SPEC,	SPCPRI,		0, 0,-1, NONE},
    {ARG, 	"ARG",		SPEC,	SPCPRI,		0, 0,-1, NONE},
    {LBRACE, 	"{",		SPEC,	SPCPRI,		0, 0, 0, NONE},
    {RBRACE, 	"}",		SPEC,	FINPRI,		0, 0, 0, NONE},
    {LPAREN, 	"(",		SPEC,	SPCPRI,		0, 0, 0, SAVENV},
    {RPAREN, 	")",		SPEC,	FINPRI,		0, 0, 0, FINAL},
    {LBR, 	"[",		SPEC,	SPCPRI,		0, 0, 0, SAVENV},
    {RBR, 	"]",		SPEC,	FINPRI,		0, 0, 0, FINAL},
    {COMMA, 	",",		BIN1,	FINPRI,		0, 0, 0, FINAL},
    {FINISH, 	"FINISH",	SPEC,	FINPRI,		0, 0, 0, FINAL},
    {SIN, 	"sin",		FUNC,	SPCPRI,		1, 1, 1, SAVENV},
    {COS, 	"cos",		FUNC,	SPCPRI,		1, 1, 1, SAVENV},
    {ATAN, 	"atan",		FUNC,	SPCPRI,		1, 2, 1, SAVENV},
    {ATAN2, 	"atan2",	FUNC,	SPCPRI,		2, 2, 1, SAVENV},
    {ASIN, 	"asin",		FUNC,	SPCPRI,		1, 1, 1, SAVENV},
    {ACOS, 	"acos",		FUNC,	SPCPRI,		1, 1, 1, SAVENV},
    {EXP, 	"exp",		FUNC,	SPCPRI,		1, 1, 1, SAVENV},
    {EXP2, 	"exp2",		FUNC,	SPCPRI,		1, 1, 1, SAVENV},
    {EXP10, 	"exp10",	FUNC,	SPCPRI,		1, 1, 1, SAVENV},
    {LOG, 	"ln",		FUNC,	SPCPRI,		1, 1, 1, SAVENV},
    {LOG2, 	"log2",		FUNC,	SPCPRI,		1, 1, 1, SAVENV},
    {LOG10, 	"log",		FUNC,	SPCPRI,		1, 1, 1, SAVENV},
    {PI, 	"pi",		FUNC,	SPCPRI,		0, 1, 1, SAVENV},
    {EE, 	"ee",		FUNC,	SPCPRI,		0, 1, 1, SAVENV},
    {ABS, 	"abs",		FUNC,	SPCPRI,		1, 1, 1, SAVENV},
    {FLOOR, 	"floor",	FUNC,	SPCPRI,		1, 1, 1, SAVENV},
    {CEIL, 	"ceil",		FUNC,	SPCPRI,		1, 1, 1, SAVENV},
    {ROUND, 	"round",	FUNC,	SPCPRI,		1, 1, 1, SAVENV},
    {INT, 	"int",		FUNC,	SPCPRI,		1, 1, 1, SAVENV},
    {FRACT, 	"fract",	FUNC,	SPCPRI,		1, 1, 1, SAVENV},
    {SQRT, 	"sqrt",		FUNC,	SPCPRI,		1, 1, 1, SAVENV},
    {COMPLEX, 	"complex",	FUNC,	SPCPRI,		1, 1, 1, SAVENV},
    {REAL, 	"real",		FUNC,	SPCPRI,		1, 1, 1, SAVENV},
    {IMAG, 	"imag",		FUNC,	SPCPRI,		1, 1, 1, SAVENV},
    {AMPL, 	"ampl",		FUNC,	SPCPRI,		1, 1, 1, SAVENV},
    {PHASE, 	"phase",	FUNC,	SPCPRI,		1, 1, 1, SAVENV},

    // End of list
    {NOP,	"NOP",		SPEC,	FINPRI,		0, 0, 0, NONE}
  };
  for (uInt i=0; olist[i].code != NOP; ++i) {
    switch (olist[i].category) {
    case UNA2:
      una2_p[olist[i].name] = olist[i];
      break;
    case UNA1:
      una1_p[olist[i].name] = olist[i];
      break;
    case BIN2:
      bin2_p[olist[i].name] = olist[i];
      break;
    case BIN1:
      bin1_p[olist[i].name] = olist[i];
      break;
    case SPEC:
      spop_p[olist[i].name] = olist[i];
      break;
    case FUNC:
      func_p[olist[i].name] = olist[i];
      break;
    default:
      break;
    }

    allop_p[olist[i].code] = olist[i];
  };
}

//# Operators

//# Member functions
void FuncExprData::print(ostream &os, const
			 map<String, FuncExprData::ExprOperator> &m) const {
  for (map<String, FuncExprData::ExprOperator>::const_iterator 
	 pos = m.begin(); pos != m.end(); pos++) print(os, pos->second);
}

void FuncExprData::print(ostream &os, const 
			 FuncExprData::ExprOperator &pos) const {
  os << setfill('0') << setw(2) << pos.code << ": " <<
    pos.name << setfill(' ') << setw(9-pos.name.length()) << ":" <<
    pos.category << ":" <<
    setfill('0') << setw(2) << pos.priority << ":" <<
    pos.narg << ":" <<
    setfill('0') << setw(2) << pos.nresult << ":" << endl;
}

//# Global functions
ostream &operator<<(ostream &os, const FuncExprData &ed) {
  os << "Unary operators with 2 characters:" << endl;
  ed.print(os, ed.unary2());
  os << "Unary operators with 1 character:" << endl;
  ed.print(os, ed.unary1());
  os << "Binary operators with 2 characters:" << endl;
  ed.print(os, ed.binary2());
  os << "Binary operators with 1 character:" << endl;
  ed.print(os, ed.binary1());
  os << "Special operations:" << endl;
  ed.print(os, ed.special());
  os << "Functions:" << endl;
  ed.print(os, ed.function());
  return os;
}

//# Templates (for test purposes)
template class map<String, FuncExprData::ExprOperator>;
template class map<FuncExprData::opTypes, FuncExprData::ExprOperator>;

// Should only be needed for gcc, solaris native, kai, and sgi not necessary.

#if defined(AIPS_GCC)
template class
_Rb_tree<FuncExprData::opTypes,
			 pair<FuncExprData::opTypes const,
  FuncExprData::ExprOperator>,
  _Select1st<pair<FuncExprData::opTypes const,
  FuncExprData::ExprOperator> >,
  less<FuncExprData::opTypes>,
  allocator<FuncExprData::ExprOperator> >;
template class
_Rb_tree<String,
  pair<String const,
  FuncExprData::ExprOperator>,
  _Select1st<pair<String const,
  FuncExprData::ExprOperator> >, 
  less<String>,
  allocator<FuncExprData::ExprOperator> >;
#endif
