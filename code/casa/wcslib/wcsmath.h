/*============================================================================
*
*   WCSLIB 4.0 - an implementation of the FITS WCS standard.
*   Copyright (C) 1995-2005, Mark Calabretta
*
*   WCSLIB is free software; you can redistribute it and/or modify it under
*   the terms of the GNU General Public License as published by the Free
*   Software Foundation; either version 2 of the License, or (at your option)
*   any later version.
*
*   WCSLIB is distributed in the hope that it will be useful, but WITHOUT ANY
*   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
*   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
*   details.
*
*   You should have received a copy of the GNU General Public License along
*   with WCSLIB; if not, write to the Free Software Foundation, Inc.,
*   59 Temple Place, Suite 330, Boston, MA  02111-1307, USA
*
*   Correspondence concerning WCSLIB may be directed to:
*      Internet email: mcalabre@atnf.csiro.au
*      Postal address: Dr. Mark Calabretta
*                      Australia Telescope National Facility, CSIRO
*                      PO Box 76
*                      Epping NSW 1710
*                      AUSTRALIA
*
*   Author: Mark Calabretta, Australia Telescope National Facility
*   http://www.atnf.csiro.au/~mcalabre/index.html
*   $Id$
*=============================================================================
*
*   Definition of mathematical constants used by WCSLIB.
*
*===========================================================================*/

#ifndef WCSLIB_MATH
#define WCSLIB_MATH

#ifdef PI
#undef PI
#endif

#ifdef D2R
#undef D2R
#endif

#ifdef R2D
#undef R2D
#endif

#ifdef SQRT2
#undef SQRT2
#endif

#ifdef SQRT2INV
#undef SQRT2INV
#endif

#define PI 3.141592653589793238462643
#define D2R PI/180.0
#define R2D 180.0/PI
#define SQRT2 1.4142135623730950488
#define SQRT2INV 1.0/SQRT2

#ifdef UNDEFINED
#undef UNDEFINED
#endif

#define UNDEFINED 987654321.0e99
#define undefined(value) (value == UNDEFINED)

#endif /* WCSLIB_MATH */
