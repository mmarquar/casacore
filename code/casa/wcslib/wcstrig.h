/*=============================================================================
*
*   WCSLIB - an implementation of the FITS WCS proposal.
*   Copyright (C) 1995-2000, Mark Calabretta
*
*   This library is free software; you can redistribute it and/or modify it
*   under the terms of the GNU Library General Public License as published
*   by the Free Software Foundation; either version 2 of the License, or (at
*   your option) any later version.
*
*   This library is distributed in the hope that it will be useful, but
*   WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library
*   General Public License for more details.
*
*   You should have received a copy of the GNU Library General Public License
*   along with this library; if not, write to the Free Software Foundation,
*   Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*   Correspondence concerning WCSLIB may be directed to:
*      Internet email: mcalabre@atnf.csiro.au
*      Postal address: Dr. Mark Calabretta,
*                      Australia Telescope National Facility,
*                      P.O. Box 76,
*                      Epping, NSW, 2121,
*                      AUSTRALIA
*
*   Author: Mark Calabretta, Australia Telescope National Facility
*   $Id$
*===========================================================================*/

#ifndef WCSLIB_TRIG
#define WCSLIB_TRIG

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__STDC__) && !defined(__cplusplus)
#ifndef const
#define const
#endif
#endif

#if __STDC__ || defined(__cplusplus)
   double cosd(const double);
   double sind(const double);
   double tand(const double);
   double acosd(const double);
   double asind(const double);
   double atand(const double);
   double atan2d(const double, const double);
#else
   double cosd();
   double sind();
   double tand();
   double acosd();
   double asind();
   double atand();
   double atan2d();
#endif

/* Domain tolerance for asin and acos functions. */
#define WCSTRIG_TOL 1e-10

#ifdef __cplusplus
};
#endif

#endif /* WCSLIB_TRIG */
