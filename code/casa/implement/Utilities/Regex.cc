//# Regex.cc: Regular expression class
//# Copyright (C) 1993,1994,1995,1996,1997,2000,2001
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

// Regex class implementation

#include <aips/Utilities/cregex.h>

#include <aips/Utilities/Regex.h>
#include <aips/Utilities/String.h>
#ifndef USE_OLD_STRING
#include <stdexcept>
#include <iostream>
#else
#include <aips/Utilities/RegexError.h>
#include <iostream.h>
#endif

Regex::Regex() {
  create("",0,0,0); }

void Regex::create(const String& exp, Int fast, Int bufsize, 
		    const Char* transtable) {
  str     = new String(exp);
  fastval = fast;
  bufsz   = bufsize;
  trans   = 0;
  if (transtable) {
    trans = new Char[256];
    memcpy(trans, transtable, 256);
  };
  Int tlen = exp.length();
  buf = new re_pattern_buffer;
  reg = new re_registers;
  if (fast) buf->fastmap = new Char[256];
  else buf->fastmap = 0;
  buf->translate = trans;
  if (tlen > bufsize)
    bufsize = tlen;
  buf->allocated = bufsize;
  buf->buffer = (Char *) malloc(buf->allocated);
  Int orig = a2_re_set_syntax(RE_NO_BK_PARENS+       // use () for grouping
			    RE_NO_BK_VBAR+         // use | for OR
			    RE_INTERVALS+          // intervals are possible
			    RE_NO_BK_CURLY_BRACES+ // use {} for interval
			    RE_CHAR_CLASSES+       // [:upper:], etc. possible
			    RE_NO_BK_REFS+         // backreferences possible
			    RE_NO_EMPTY_RANGES+    // e.g. [z-a] is empty set
			    RE_CONTEXTUAL_INVALID_OPS);
  Char* msg = a2_re_compile_pattern((Char*)(exp.chars()), tlen, buf);
  a2_re_set_syntax(orig);
  if (msg != 0) {
#ifndef USE_OLD_STRING
    throw(invalid_argument("Regex"));
#else
    throw(RegexExpressnError(msg));
#endif
  } else if (fast) a2_re_compile_fastmap(buf);
}

void Regex::dealloc() {
  free(buf->buffer);
  delete [] buf->fastmap;
  delete buf;
  delete reg;
  delete str;
  delete [] trans;
}

Int Regex::match_info(Int& start, Int& length, Int nth) const {
  if ((unsigned)(nth) >= RE_NREGS) return 0;
  else {
    start = reg->start[nth];
    length = reg->end[nth] - start;
    return start >= 0 && length >= 0;
  };
}

Bool Regex::OK() const {
  Bool v = buf != 0;             // have a regex buf
  v &= buf->buffer != 0;         // with a pat
  return v;
}

ostream &operator<<(ostream &ios, const Regex &exp) {
  return ios << *exp.str;
}

Int Regex::search(const Char *s, Int len, Int &matchlen, Int startpos) const {
  Int matchpos, pos, range;
  if (startpos >= 0) {
    pos = startpos;
    range = len - startpos;
  } else {
    pos = len + startpos;
    range = -pos;
  };
  matchpos = a2_re_search_2(buf, 0, 0, (Char*)s, len, pos, range, reg, len);
  if (matchpos >= 0) matchlen = reg->end[0] - reg->start[0];
  else matchlen = 0;
  return matchpos;
}

Int Regex::match(const Char *s, Int len, Int p) const {
  if (p < 0) {
    p += len;
    if (p > len) return -1;
    return a2_re_match_2(buf, 0, 0, (Char*)s, p, 0, reg, p);
  } else if (p > len) return -1;
  else return a2_re_match_2(buf, 0, 0, (Char*)s, len, p, reg, len);
}

Regex::Regex(const String &exp, Bool fast, Int sz,
	      const Char *translation) {
  create(exp, fast, sz, translation);
}

Regex::~Regex() {
  dealloc();
}

Regex::Regex(const Regex &that) {
  create(*that.str, that.fastval, that.bufsz, that.trans);
}

Regex &Regex::operator=(const Regex &that) {
  dealloc();
  create(*that.str, that.fastval, that.bufsz, that.trans);
  return *this;
}

Regex &Regex::operator=(const String &strng) {
  dealloc();
  create(strng, 0, 40, 0);
  return *this;
}

const String &Regex::regexp() const {
  return *str;
}

const Char *Regex::transtable() const {
  return trans;
}

String Regex::fromPattern(const String &pattern) {
    enum CState{stream, bracketopen, escapechar};
    Int len = 0;
    Int bracecount = 0;
    Int inbrcount = 0;
    Int pattLeng = pattern.length();
    String result(3 * pattLeng, Char(0));
    CState state = stream;
    for (Int i=0; i<pattLeng; i++) {
	Char c = pattern[i];
	switch(state) {
	case stream :

	    switch (c) {
	    case '^':
	    case '$':
	    case '(':
	    case ')':
	    case '.':
	    case '+':
	    case '|':
		// Special chars have to be escaped.
		result[len++] = '\\';
		break;
	    case '{':
		// Opening brace gets ((
		result[len++] = '(';
		c = '(';
		bracecount++;
		break;
	    case ',':
		// Comma after opening brace gets )|(
		// Otherwise it's still a comma.
		if (bracecount) {
		    result[len++] = ')';
		    result[len++] = '|';
		    c = '(';
		}
		break;
	    case '}':
		// Closing brace after opening brace gets ))
		// Otherwise it's still an opening brace.
		if (bracecount) {
		    bracecount--;
		    result[len++] = ')';
		    c = ')';
		}
		break;
	    case '[':
		// Opening bracket puts us in a special state.
		state = bracketopen;
		inbrcount = 0;
		break;
	    case '*':
		// * gets .*
		result[len++] = '.';
		break;
	    case '?':
		// ? gets .
		c = '.';
		break;
	    case '\\':
		// Backslash puts us in a special state.
		state = escapechar;
		break;
	    // leave all other chars unchanged
	    }
	    break;
	    
	case bracketopen:
	    // A closing bracket immediately after the start of a bracket
	    // expression is a literal ] and not the end of the expression.
	    // Otherwise a closing bracket puts us back in the normal state.
	    if (c == ']'  &&  inbrcount) {
		state = stream;
	    } else if (c == '!'  &&  !inbrcount) {
		// A starting ! is a not.
		c = '^';
	    }
	    inbrcount++;
	    break;
	    
	case escapechar:
	    // An escaped comma can be turned into a normal comma, thus
	    // does not need the backslash.
	    if (c != ',') {
		result[len++] = '\\';
	    }
	    state = stream;
	    break;
	}
	// Wait with storing an escape character.
	if (state != escapechar) {
	    result[len++] = c;
	}
    }
    // Store a trailing backslash.
    if (state == escapechar) {
	result[len++] = '\\';
    }
    return String(result.chars(), len);
}

String Regex::fromString(const String &strng) {
    Int strLeng = strng.length();
    String result(2 * strLeng, Char(0));
    Int len = 0;
    for (Int i=0; i<strLeng; i++) {
	Char c = strng[i];
	// Escape special characters.
	switch (c) {
	case '^':
	case '$':
	case '[':
	case ']':
	case '*':
	case '+':
	case '?':
	case '.':
	case '|':
	case '{':
	case '}':
	case '(':
	case ')':
	case '\\':
	    result[len++] = '\\';
	}
	result[len++] = c;
    };
    return String(result.chars(), len);
}


// some built-in Regular expressions

const Regex RXwhite("[ \n\t\r\v\f]+", 1);
const Regex RXint("-?[0-9]+", 1);
const Regex RXdouble("-?(([0-9]+\\.[0-9]*)|([0-9]+)|(\\.[0-9]+))([eE][+-]?[0-9]+)?", 1, 200);
const Regex RXalpha("[A-Za-z]+", 1);
const Regex RXlowercase("[a-z]+", 1);
const Regex RXuppercase("[A-Z]+", 1);
const Regex RXalphanum("[0-9A-Za-z]+", 1);
const Regex RXidentifier("[A-Za-z_][A-Za-z0-9_]*", 1);
