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
*===========================================================================*/

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "wcsmath.h"
#include "wcstrig.h"
#include "prj.h"


/* Projection categories. */
const int ZENITHAL          = 1;
const int CYLINDRICAL       = 2;
const int PSEUDOCYLINDRICAL = 3;
const int CONVENTIONAL      = 4;
const int CONIC             = 5;
const int POLYCONIC         = 6;
const int QUADCUBE          = 7;
const int HEALPIX           = 8;

const char prj_categories[9][32] =
      {"undefined", "zenithal", "cylindrical", "pseudocylindrical",
       "conventional", "conic", "polyconic", "quadcube", "HEALPix"};


/* Projection codes. */
const int  prj_ncode = 27;
const char prj_codes[27][4] =
      {"AZP", "SZP", "TAN", "STG", "SIN", "ARC", "ZPN", "ZEA", "AIR", "CYP",
       "CEA", "CAR", "MER", "COP", "COE", "COD", "COO", "SFL", "PAR", "MOL",
       "AIT", "BON", "PCO", "TSC", "CSC", "QSC", "HPX"};

const int AZP = 101;
const int SZP = 102;
const int TAN = 103;
const int STG = 104;
const int SIN = 105;
const int ARC = 106;
const int ZPN = 107;
const int ZEA = 108;
const int AIR = 109;
const int CYP = 201;
const int CEA = 202;
const int CAR = 203;
const int MER = 204;
const int SFL = 301;
const int PAR = 302;
const int MOL = 303;
const int AIT = 401;
const int COP = 501;
const int COE = 502;
const int COD = 503;
const int COO = 504;
const int BON = 601;
const int PCO = 602;
const int TSC = 701;
const int CSC = 702;
const int QSC = 703;
const int HPX = 801;


/* Map status return value to message. */
const char *prj_errmsg[] = {
   0,
   "Null prjprm pointer passed",
   "Invalid projection parameters",
   "One or more of the (x,y) coordinates were invalid",
   "One or more of the (phi,theta) coordinates were invalid"};

int prj_stat;


#define copysign(X, Y) ((Y) < 0.0 ? -fabs(X) : fabs(X))


/*============================================================================
* Generic routines.
*
* prjini initializes a prjprm struct to default values.
*
* prjprt prints the contents of a prjprm struct.
*
* prjset invokes the specific initialization routine based on the projection
*        code in the prjprm struct.
*
* prjx2s invokes the specific deprojection routine based on the pointer-to-
*        function stored in the prjprm struct.
*
* prjs2x invokes the specific projection routine based on the pointer-to-
*        function stored in the prjprm struct.
*
*---------------------------------------------------------------------------*/

int prjini(prj)

struct prjprm *prj;

{
   register int k;

   if (prj == 0) return 1;

   prj->flag = 0;

   strcpy(prj->code, "   ");
   prj->pv[0]  = 0.0;
   prj->pv[1]  = UNDEFINED;
   prj->pv[2]  = UNDEFINED;
   prj->pv[3]  = UNDEFINED;
   for (k = 4; k < PVN; prj->pv[k++] = 0.0);
   prj->r0     = 0.0;
   prj->phi0   = UNDEFINED;
   prj->theta0 = UNDEFINED;
   prj->bounds = 1;

   strcpy(prj->name, "Undefined");
   prj->category  = 0;
   prj->pvrange   = 0;
   prj->simplezen = 0;
   prj->equiareal = 0;
   prj->conformal = 0;
   prj->global    = 0;
   prj->divergent = 0;
   prj->x0 = 0.0;
   prj->y0 = 0.0;
   for (k = 0; k < 10; prj->w[k++] = 0.0);
   prj->n = 0;

   prj->p = prj->pv;

   return 0;
}

/*--------------------------------------------------------------------------*/

int prjprt(prj)

const struct prjprm *prj;

{
   int i, n;

   if (prj == 0) return 1;

   printf("       flag: %d\n",  prj->flag);
   printf("       code: \"%s\"\n",  prj->code);
   printf("         r0: %9f\n", prj->r0);
   printf("         pv:");
   if (prj->pvrange) {
      n = (prj->pvrange)%100;

      if (prj->pvrange/100) {
         printf(" (0)");
      } else {
         printf(" %- 11.4g", prj->pv[0]);
         n--;
      }

      for (i = 1; i <= n; i++) {
         if (i%5 == 1) {
            printf("\n           ");
         }

         if (undefined(prj->pv[i])) {
            printf("  UNDEFINED   ");
         } else {
            printf("  %- 11.4g", prj->pv[i]);
         }
      }
      printf("\n");
   } else {
      printf(" (not used)\n");
   }
   if (undefined(prj->phi0)) {
      printf("       phi0: UNDEFINED\n");
   } else {
      printf("       phi0: %9f\n", prj->phi0);
   }
   if (undefined(prj->theta0)) {
      printf("     theta0: UNDEFINED\n");
   } else {
      printf("     theta0: %9f\n", prj->theta0);
   }
   printf("     bounds: %d\n",  prj->bounds);

   printf("\n");
   printf("       name: \"%s\"\n", prj->name);
   printf("   category: %d (%s)\n", prj->category,
                                    prj_categories[prj->category]);
   printf("    pvrange: %d\n", prj->pvrange);
   printf("  simplezen: %d\n", prj->simplezen);
   printf("  equiareal: %d\n", prj->equiareal);
   printf("  conformal: %d\n", prj->conformal);
   printf("     global: %d\n", prj->global);
   printf("  divergent: %d\n", prj->divergent);
   printf("         x0: %f\n", prj->x0);
   printf("         y0: %f\n", prj->y0);
   printf("        w[]:");
   for (i = 0; i < 5; i++) {
      printf("  %- 11.4g", prj->w[i]);
   }
   printf("\n            ");
   for (i = 5; i < 10; i++) {
      printf("  %- 11.4g", prj->w[i]);
   }
   printf("\n");
   printf("          n: %d\n", prj->n);
   printf("     prjx2s: 0x%x\n", (int)prj->prjx2s);
   printf("     prjs2x: 0x%x\n", (int)prj->prjs2x);
   printf("          p: 0x%x  (= pv)\n", (int)prj->p);

   return 0;
}

/*--------------------------------------------------------------------------*/

int prjset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   /* Invoke the relevant initialization routine. */
   prj->code[3] = '\0';
   if (strcmp(prj->code, "AZP") == 0) {
      azpset(prj);
   } else if (strcmp(prj->code, "SZP") == 0) {
      szpset(prj);
   } else if (strcmp(prj->code, "TAN") == 0) {
      tanset(prj);
   } else if (strcmp(prj->code, "STG") == 0) {
      stgset(prj);
   } else if (strcmp(prj->code, "SIN") == 0) {
      sinset(prj);
   } else if (strcmp(prj->code, "ARC") == 0) {
      arcset(prj);
   } else if (strcmp(prj->code, "ZPN") == 0) {
      zpnset(prj);
   } else if (strcmp(prj->code, "ZEA") == 0) {
      zeaset(prj);
   } else if (strcmp(prj->code, "AIR") == 0) {
      airset(prj);
   } else if (strcmp(prj->code, "CYP") == 0) {
      cypset(prj);
   } else if (strcmp(prj->code, "CEA") == 0) {
      ceaset(prj);
   } else if (strcmp(prj->code, "CAR") == 0) {
      carset(prj);
   } else if (strcmp(prj->code, "MER") == 0) {
      merset(prj);
   } else if (strcmp(prj->code, "SFL") == 0) {
      sflset(prj);
   } else if (strcmp(prj->code, "PAR") == 0) {
      parset(prj);
   } else if (strcmp(prj->code, "MOL") == 0) {
      molset(prj);
   } else if (strcmp(prj->code, "AIT") == 0) {
      aitset(prj);
   } else if (strcmp(prj->code, "COP") == 0) {
      copset(prj);
   } else if (strcmp(prj->code, "COE") == 0) {
      coeset(prj);
   } else if (strcmp(prj->code, "COD") == 0) {
      codset(prj);
   } else if (strcmp(prj->code, "COO") == 0) {
      cooset(prj);
   } else if (strcmp(prj->code, "BON") == 0) {
      bonset(prj);
   } else if (strcmp(prj->code, "PCO") == 0) {
      pcoset(prj);
   } else if (strcmp(prj->code, "TSC") == 0) {
      tscset(prj);
   } else if (strcmp(prj->code, "CSC") == 0) {
      cscset(prj);
   } else if (strcmp(prj->code, "QSC") == 0) {
      qscset(prj);
   } else if (strcmp(prj->code, "HPX") == 0) {
      hpxset(prj);
   } else {
      /* Unrecognized projection code. */
      return 2;
   }

   return 0;
}

/*--------------------------------------------------------------------------*/

int prjx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag == 0) {
      if (prjset(prj)) return 2;
   }

   return prj->prjx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat);
}

/*--------------------------------------------------------------------------*/

int prjs2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag == 0) {
      if (prjset(prj)) return 2;
   }

   return prj->prjs2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat);
}

/*============================================================================
* Internal helper routine used by the *set() routines that forces
* (x,y) = (0,0) at (phi0,theta0).
*---------------------------------------------------------------------------*/

int prjoff(prj, phi0, theta0)

struct prjprm *prj;
const double phi0, theta0;

{
   int    stat;

   if (prj == 0) return 1;

   if (undefined(prj->phi0) || undefined(prj->theta0)) {
      /* Set both to the projection-specific default if either undefined. */
      prj->phi0   = phi0;
      prj->theta0 = theta0;

      prj->x0 = 0.0;
      prj->y0 = 0.0;

   } else {
      if (prj->prjs2x(prj, 1, 1, 1, 1, &(prj->phi0), &(prj->theta0),
                      &(prj->x0), &(prj->y0), &stat)) {
         return 2;
      }
   }

   return 0;
}

/*============================================================================
*   AZP: zenithal/azimuthal perspective projection.
*
*   Given:
*      prj->pv[1]   Distance parameter, mu in units of r0.
*      prj->pv[2]   Tilt angle, gamma in degrees.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to  0.0 if undefined.
*      prj->theta0  Reset to 90.0 if undefined.
*
*   Returned:
*      prj->flag     AZP
*      prj->code    "AZP"
*      prj->x0      Offset in x.
*      prj->y0      Offset in y.
*      prj->w[0]    r0*(mu+1)
*      prj->w[1]    tan(gamma)
*      prj->w[2]    sec(gamma)
*      prj->w[3]    cos(gamma)
*      prj->w[4]    sin(gamma)
*      prj->w[5]    asin(-1/mu) for |mu| >= 1, -90 otherwise
*      prj->w[6]    mu*cos(gamma)
*      prj->w[7]    1 if |mu*cos(gamma)| < 1, 0 otherwise
*      prj->prjx2s  Pointer to azpx2s().
*      prj->prjs2x  Pointer to azps2x().
*===========================================================================*/

int azpset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   prj->flag = AZP;
   strcpy(prj->code, "AZP");

   if (undefined(prj->pv[1])) prj->pv[1] = 0.0;
   if (undefined(prj->pv[2])) prj->pv[2] = 0.0;
   if (prj->r0 == 0.0) prj->r0 = R2D;

   strcpy(prj->name, "Zenithal/azimuthal perspective");
   prj->category  = ZENITHAL;
   prj->pvrange   = 102;
   prj->simplezen = prj->pv[2] == 0.0;
   prj->equiareal = 0;
   prj->conformal = 0;
   prj->global    = 0;
   prj->divergent = prj->pv[1] <= 1.0;

   prj->w[0] = prj->r0*(prj->pv[1] + 1.0);
   if (prj->w[0] == 0.0) {
      return 2;
   }

   prj->w[3] = cosd(prj->pv[2]);
   if (prj->w[3] == 0.0) {
      return 2;
   }

   prj->w[2] = 1.0/prj->w[3];
   prj->w[4] = sind(prj->pv[2]);
   prj->w[1] = prj->w[4] / prj->w[3];

   if (fabs(prj->pv[1]) > 1.0) {
      prj->w[5] = asind(-1.0/prj->pv[1]);
   } else {
      prj->w[5] = -90.0;
   }

   prj->w[6] = prj->pv[1] * prj->w[3];
   prj->w[7] = (fabs(prj->w[6]) < 1.0) ? 1.0 : 0.0;

   prj->prjx2s = azpx2s;
   prj->prjs2x = azps2x;

   return prjoff(prj, 0.0, 90.0);
}

/*--------------------------------------------------------------------------*/

int azpx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int mx, my, rowlen, rowoff, status;
   double a, b, q, r, s, t, xj, yj, yc, yc2;
   const double tol = 1.0e-13;
   register int ix, iy, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != AZP) {
      if (azpset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }

   status = 0;


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      xj = *xp + prj->x0;

      phip = phi + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip = xj;
         phip += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   phip   = phi;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      yj = *yp + prj->y0;

      yc  = yj*prj->w[3];
      yc2 = yc*yc;

      q = prj->w[0] + yj*prj->w[4];

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         xj = *phip;

         r = sqrt(xj*xj + yc2);
         if (r == 0.0) {
            *phip = 0.0;
            *thetap = 90.0;
            *(statp++) = 0;

         } else {
            *phip = atan2d(xj, -yc);

            s = r / q;
            t = s*prj->pv[1]/sqrt(s*s + 1.0);

            s = atan2d(1.0, s);

            if (fabs(t) > 1.0) {
               if (fabs(t) > 1.0+tol) {
                  *thetap = 0.0;
                  *(statp++) = 1;
                  status = 3;
                  continue;
               }
               t = copysign(90.0, t);
            } else {
               t = asind(t);
            }

            a = s - t;
            b = s + t + 180.0;

            if (a > 90.0) a -= 360.0;
            if (b > 90.0) b -= 360.0;

            *thetap = (a > b) ? a : b;
            *(statp++) = 0;
         }
      }
   }

   return status;
}

/*--------------------------------------------------------------------------*/

int azps2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int mphi, mtheta, rowlen, rowoff, status;
   double a, b, cosphi, costhe, r, s, sinphi, sinthe, t;
   register int iphi, itheta, istat, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != AZP) {
      if (azpset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }

   status = 0;


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      sinphi = sind(*phip);
      cosphi = cosd(*phip);

      xp = x + rowoff;
      yp = y + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = sinphi;
         *yp = cosphi;
         xp += rowlen;
         yp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   xp = x;
   yp = y;
   statp = stat;
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      costhe = cosd(*thetap);
      sinthe = sind(*thetap);

      for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
         s = prj->w[1]*(*yp);
         t = (prj->pv[1] + sinthe) + costhe*s;

         if (t == 0.0) {
            *xp = 0.0;
            *yp = 0.0;
            *(statp++) = 1;
            status = 4;

         } else {
            r = prj->w[0]*costhe/t;

            /* Bounds checking. */
            istat = 0;
            if (prj->bounds) {
               if (*thetap < prj->w[5]) {
                  /* Overlap. */
                  istat  = 1;
                  status = 4;

               } else if (prj->w[7] > 0.0) {
                  /* Divergence. */
                  t = prj->pv[1] / sqrt(1.0 + s*s);

                  if (fabs(t) <= 1.0) {
                     s = atand(-s);
                     t = asind(t);
                     a = s - t;
                     b = s + t + 180.0;

                     if (a > 90.0) a -= 360.0;
                     if (b > 90.0) b -= 360.0;

                     if (*thetap < ((a > b) ? a : b)) {
                        istat  = 1;
                        status = 4;
                     }
                  }
               }
            }

            *xp =  r*(*xp) - prj->x0;
            *yp = -r*(*yp)*prj->w[2] - prj->y0;
            *(statp++) = istat;
         }
      }
   }

   return status;
}

/*============================================================================
*   SZP: slant zenithal perspective projection.
*
*   Given:
*      prj->pv[1]   Distance of the point of projection from the centre of the
*                   generating sphere, mu in units of r0.
*      prj->pv[2]   Native longitude, phi_c, and ...
*      prj->pv[3]   Native latitude, theta_c, on the planewards side of the
*                   intersection of the line through the point of projection
*                   and the centre of the generating sphere, phi_c in degrees.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to  0.0 if undefined.
*      prj->theta0  Reset to 90.0 if undefined.
*
*   Returned:
*      prj->flag     SZP
*      prj->code    "SZP"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->w[0]    1/r0
*      prj->w[1]    xp = -mu*cos(theta_c)*sin(phi_c)
*      prj->w[2]    yp =  mu*cos(theta_c)*cos(phi_c)
*      prj->w[3]    zp =  mu*sin(theta_c) + 1
*      prj->w[4]    r0*xp
*      prj->w[5]    r0*yp
*      prj->w[6]    r0*zp
*      prj->w[7]    (zp - 1)^2
*      prj->w[8]    asin(1-zp) if |1 - zp| < 1, -90 otherwise
*      prj->prjx2s  Pointer to szpx2s().
*      prj->prjs2x  Pointer to szps2x().
*===========================================================================*/

int szpset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   prj->flag = SZP;
   strcpy(prj->code, "SZP");

   if (undefined(prj->pv[1])) prj->pv[1] =  0.0;
   if (undefined(prj->pv[2])) prj->pv[2] =  0.0;
   if (undefined(prj->pv[3])) prj->pv[3] = 90.0;
   if (prj->r0 == 0.0) prj->r0 = R2D;

   strcpy(prj->name, "Slant zenithal perspective");
   prj->category  = ZENITHAL;
   prj->pvrange   = 103;
   prj->simplezen = prj->pv[3] == 90.0;
   prj->equiareal = 0;
   prj->conformal = 0;
   prj->global    = 0;
   prj->divergent = prj->pv[1] <= 1.0;

   prj->w[0] = 1.0/prj->r0;

   prj->w[3] = prj->pv[1] * sind(prj->pv[3]) + 1.0;
   if (prj->w[3] == 0.0) {
      return 2;
   }

   prj->w[1] = -prj->pv[1] * cosd(prj->pv[3]) * sind(prj->pv[2]);
   prj->w[2] =  prj->pv[1] * cosd(prj->pv[3]) * cosd(prj->pv[2]);
   prj->w[4] =  prj->r0 * prj->w[1];
   prj->w[5] =  prj->r0 * prj->w[2];
   prj->w[6] =  prj->r0 * prj->w[3];
   prj->w[7] =  (prj->w[3] - 1.0) * prj->w[3] - 1.0;

   if (fabs(prj->w[3] - 1.0) < 1.0) {
      prj->w[8] = asind(1.0 - prj->w[3]);
   } else {
      prj->w[8] = -90.0;
   }

   prj->prjx2s = szpx2s;
   prj->prjs2x = szps2x;

   return prjoff(prj, 0.0, 90.0);
}

/*--------------------------------------------------------------------------*/

int szpx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int mx, my, rowlen, rowoff, status;
   double a, b, c, d, r2, sinth1, sinth2, sinthe, t, x1, xr, xy, y1, yr, z;
   const double tol = 1.0e-13;
   register int ix, iy, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != SZP) {
      if (szpset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }

   status = 0;


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      xr = (*xp + prj->x0)*prj->w[0];

      phip = phi + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip = xr;
         phip += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   phip   = phi;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      yr = (*yp + prj->y0)*prj->w[0];

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         xr = *phip;
         r2 = xr*xr + yr*yr;

         x1 = (xr - prj->w[1])/prj->w[3];
         y1 = (yr - prj->w[2])/prj->w[3];
         xy = xr*x1 + yr*y1;

         if (r2 < 1.0e-10) {
            /* Use small angle formula. */
            z = r2/2.0;
            *thetap = 90.0 - R2D*sqrt(r2/(1.0 + xy));

         } else {
            t = x1*x1 + y1*y1;
            a = t + 1.0;
            b = xy - t;
            c = r2 - xy - xy + t - 1.0;
            d = b*b - a*c;

            /* Check for a solution. */
            if (d < 0.0) {
               *phip = 0.0;
               *thetap = 0.0;
               *(statp++) = 1;
               status = 3;
               continue;
            }
            d = sqrt(d);

            /* Choose solution closest to pole. */
            sinth1 = (-b + d)/a;
            sinth2 = (-b - d)/a;
            sinthe = (sinth1 > sinth2) ? sinth1 : sinth2;
            if (sinthe > 1.0) {
               if (sinthe-1.0 < tol) {
                  sinthe = 1.0;
               } else {
                  sinthe = (sinth1 < sinth2) ? sinth1 : sinth2;
               }
            }

            if (sinthe < -1.0) {
               if (sinthe+1.0 > -tol) {
                  sinthe = -1.0;
               }
            }

            if (sinthe > 1.0 || sinthe < -1.0) {
               *phip   = 0.0;
               *thetap = 0.0;
               *(statp++) = 1;
               status = 3;
               continue;
            }

            *thetap = asind(sinthe);

            z = 1.0 - sinthe;
         }

         *phip = atan2d(xr - x1*z, -(yr - y1*z));
         *(statp++) = 0;
      }
   }

   return status;
}

/*--------------------------------------------------------------------------*/

int szps2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int mphi, mtheta, rowlen, rowoff, status;
   double a, b, cosphi, r, s, sinphi, t, u, v;
   register int iphi, itheta, istat, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != SZP) {
      if (szpset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }

   status = 0;


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      sinphi = sind(*phip);
      cosphi = cosd(*phip);

      xp = x + rowoff;
      yp = y + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = sinphi;
         *yp = cosphi;
         xp += rowlen;
         yp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   xp = x;
   yp = y;
   statp = stat;
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      s = 1.0 - sind(*thetap);
      t = prj->w[3] - s;

      if (t == 0.0) {
         for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
            *xp = 0.0;
            *yp = 0.0;
            *(statp++) = 1;
         }

         status = 4;

      } else {
         r = prj->w[6]*cosd(*thetap)/t;
         u = prj->w[4]*s/t + prj->x0;
         v = prj->w[5]*s/t + prj->y0;

         for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
            /* Bounds checking. */
            istat = 0;
            if (prj->bounds) {
               if (*thetap < prj->w[8]) {
                  /* Divergence. */
                  istat  = 1;
                  status = 4;

               } else if (fabs(prj->pv[1]) > 1.0) {
                  /* Overlap. */
                  s = prj->w[1]*(*xp) - prj->w[2]*(*yp);
                  t = 1.0/sqrt(prj->w[7] + s*s);

                  if (fabs(t) <= 1.0) {
                     s = atan2d(s, prj->w[3] - 1.0);
                     t = asind(t);
                     a = s - t;
                     b = s + t + 180.0;

                     if (a > 90.0) a -= 360.0;
                     if (b > 90.0) b -= 360.0;

                     if (*thetap < ((a > b) ? a : b)) {
                        istat  = 1;
                        status = 4;
                     }
                  }
               }
            }

            *xp =  r*(*xp) - u;
            *yp = -r*(*yp) - v;
            *(statp++) = istat;
         }
      }
   }

   return status;
}


/*============================================================================
*   TAN: gnomonic projection.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to  0.0 if undefined.
*      prj->theta0  Reset to 90.0 if undefined.
*
*   Returned:
*      prj->flag     TAN
*      prj->code    "TAN"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->prjx2s  Pointer to tanx2s().
*      prj->prjs2x  Pointer to tans2x().
*===========================================================================*/

int tanset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   prj->flag = TAN;
   strcpy(prj->code, "TAN");

   if (prj->r0 == 0.0) prj->r0 = R2D;

   strcpy(prj->name, "Gnomonic");
   prj->category  = ZENITHAL;
   prj->pvrange   = 0;
   prj->simplezen = 1;
   prj->equiareal = 0;
   prj->conformal = 0;
   prj->global    = 0;
   prj->divergent = 1;

   prj->prjx2s = tanx2s;
   prj->prjs2x = tans2x;

   return prjoff(prj, 0.0, 90.0);
}

/*--------------------------------------------------------------------------*/

int tanx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int mx, my, rowlen, rowoff;
   double r, xj, yj, yj2;
   register int ix, iy, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != TAN) {
      if (tanset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      xj = *xp + prj->x0;

      phip = phi + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip = xj;
         phip += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   phip   = phi;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      yj  = *yp + prj->y0;
      yj2 = yj*yj;

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         xj = *phip;

         r = sqrt(xj*xj + yj2);
         if (r == 0.0) {
            *phip = 0.0;
         } else {
            *phip = atan2d(xj, -yj);
         }

         *thetap = atan2d(prj->r0, r);
         *(statp++) = 0;
      }
   }

   return 0;
}

/*--------------------------------------------------------------------------*/

int tans2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int mphi, mtheta, rowlen, rowoff, status;
   double cosphi, r, s, sinphi;
   register int iphi, itheta, istat, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != TAN) {
      if (tanset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }

   status = 0;


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      sinphi = sind(*phip);
      cosphi = cosd(*phip);

      xp = x + rowoff;
      yp = y + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = sinphi;
         *yp = cosphi;
         xp += rowlen;
         yp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   xp = x;
   yp = y;
   statp = stat;
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      s = sind(*thetap);
      if (s == 0.0) {
         for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
            *xp = 0.0;
            *yp = 0.0;
            *(statp++) = 1;
         }
         status = 4;

      } else {
         r =  prj->r0*cosd(*thetap)/s;

         istat = 0;
         if (prj->bounds && s < 0.0) {
            istat  = 1;
            status = 4;
         }

         for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
            *xp =  r*(*xp) - prj->x0;
            *yp = -r*(*yp) - prj->y0;
            *(statp++) = istat;
         }
      }
   }

   return status;
}

/*============================================================================
*   STG: stereographic projection.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to  0.0 if undefined.
*      prj->theta0  Reset to 90.0 if undefined.
*
*   Returned:
*      prj->flag     STG
*      prj->code    "STG"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->w[0]    2*r0
*      prj->w[1]    1/(2*r0)
*      prj->prjx2s  Pointer to stgx2s().
*      prj->prjs2x  Pointer to stgs2x().
*===========================================================================*/

int stgset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   prj->flag = STG;
   strcpy(prj->code, "STG");

   strcpy(prj->name, "Stereographic");
   prj->category  = ZENITHAL;
   prj->pvrange   = 0;
   prj->simplezen = 1;
   prj->equiareal = 0;
   prj->conformal = 1;
   prj->global    = 0;
   prj->divergent = 1;

   if (prj->r0 == 0.0) {
      prj->r0 = R2D;
      prj->w[0] = 360.0/PI;
      prj->w[1] = PI/360.0;
   } else {
      prj->w[0] = 2.0*prj->r0;
      prj->w[1] = 1.0/prj->w[0];
   }

   prj->prjx2s = stgx2s;
   prj->prjs2x = stgs2x;

   return prjoff(prj, 0.0, 90.0);
}

/*--------------------------------------------------------------------------*/

int stgx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int mx, my, rowlen, rowoff;
   double r, xj, yj, yj2;
   register int ix, iy, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != STG) {
      if (stgset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      xj = *xp + prj->x0;

      phip = phi + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip = xj;
         phip += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   phip   = phi;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      yj  = *yp + prj->y0;
      yj2 = yj*yj;

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         xj  = *phip;

         r = sqrt(xj*xj + yj2);
         if (r == 0.0) {
            *phip = 0.0;
         } else {
            *phip = atan2d(xj, -yj);
         }

         *thetap = 90.0 - 2.0*atand(r*prj->w[1]);
         *(statp++) = 0;
      }
   }

   return 0;
}

/*--------------------------------------------------------------------------*/

int stgs2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int mphi, mtheta, rowlen, rowoff, status;
   double cosphi, r, s, sinphi;
   register int iphi, itheta, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != STG) {
      if (stgset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }

   status = 0;


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      sinphi = sind(*phip);
      cosphi = cosd(*phip);

      xp = x + rowoff;
      yp = y + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = sinphi;
         *yp = cosphi;
         xp += rowlen;
         yp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   xp = x;
   yp = y;
   statp = stat;
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      s = 1.0 + sind(*thetap);
      if (s == 0.0) {
         for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
            *xp = 0.0;
            *yp = 0.0;
            *(statp++) = 1;
         }
         status = 4;

      } else {
         r = prj->w[0]*cosd(*thetap)/s;

         for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
            *xp =  r*(*xp) - prj->x0;
            *yp = -r*(*yp) - prj->y0;
            *(statp++) = 0;
         }
      }
   }

   return status;
}

/*============================================================================
*   SIN: orthographic/synthesis projection.
*
*   Given:
*      prj->pv[1:2] Obliqueness parameters, xi and eta.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to  0.0 if undefined.
*      prj->theta0  Reset to 90.0 if undefined.
*
*   Returned:
*      prj->flag     SIN
*      prj->code    "SIN"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->w[0]    1/r0
*      prj->w[1]    xi**2 + eta**2
*      prj->w[2]    xi**2 + eta**2 + 1
*      prj->w[3]    xi**2 + eta**2 - 1
*      prj->prjx2s  Pointer to sinx2s().
*      prj->prjs2x  Pointer to sins2x().
*===========================================================================*/

int sinset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   prj->flag = SIN;
   strcpy(prj->code, "SIN");

   if (undefined(prj->pv[1])) prj->pv[1] = 0.0;
   if (undefined(prj->pv[2])) prj->pv[2] = 0.0;
   if (prj->r0 == 0.0) prj->r0 = R2D;

   strcpy(prj->name, "Orthographic/synthesis");
   prj->category  = ZENITHAL;
   prj->pvrange   = 102;
   prj->simplezen = (prj->pv[1] == 0.0 && prj->pv[2] == 0.0);
   prj->equiareal = 0;
   prj->conformal = 0;
   prj->global    = 0;
   prj->divergent = 0;

   prj->w[0] = 1.0/prj->r0;
   prj->w[1] = prj->pv[1]*prj->pv[1] + prj->pv[2]*prj->pv[2];
   prj->w[2] = prj->w[1] + 1.0;
   prj->w[3] = prj->w[1] - 1.0;

   prj->prjx2s = sinx2s;
   prj->prjs2x = sins2x;

   return prjoff(prj, 0.0, 90.0);
}

/*--------------------------------------------------------------------------*/

int sinx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int mx, my, rowlen, rowoff, status;
   const double tol = 1.0e-13;
   double a, b, c, d, eta, r2, sinth1, sinth2, sinthe, x0, xi, x1, xy, y0,
          y02, y1, z;
   register int ix, iy, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != SIN) {
      if (sinset(prj)) return 2;
   }

   xi  = prj->pv[1];
   eta = prj->pv[2];

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }

   status = 0;


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      x0 = (*xp + prj->x0)*prj->w[0];

      phip = phi + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip = x0;
         phip += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   phip   = phi;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      y0 = (*yp + prj->y0)*prj->w[0];
      y02 = y0*y0;

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         /* Compute intermediaries. */
         x0 = *phip;
         r2 = x0*x0 + y02;

         if (prj->w[1] == 0.0) {
            /* Orthographic projection. */
            if (r2 != 0.0) {
               *phip = atan2d(x0, -y0);
            } else {
               *phip = 0.0;
            }

            if (r2 < 0.5) {
               *thetap = acosd(sqrt(r2));
            } else if (r2 <= 1.0) {
               *thetap = asind(sqrt(1.0 - r2));
            } else {
               *(statp++) = 1;
               status = 3;
               continue;
            }

         } else {
            /* "Synthesis" projection. */
            xy = x0*xi + y0*eta;

            if (r2 < 1.0e-10) {
               /* Use small angle formula. */
               z = r2/2.0;
               *thetap = 90.0 - R2D*sqrt(r2/(1.0 + xy));

            } else {
               a = prj->w[2];
               b = xy - prj->w[1];
               c = r2 - xy - xy + prj->w[3];
               d = b*b - a*c;

               /* Check for a solution. */
               if (d < 0.0) {
                  *phip = 0.0;
                  *thetap = 0.0;
                  *(statp++) = 1;
                  status = 3;
                  continue;
               }
               d = sqrt(d);

               /* Choose solution closest to pole. */
               sinth1 = (-b + d)/a;
               sinth2 = (-b - d)/a;
               sinthe = (sinth1 > sinth2) ? sinth1 : sinth2;
               if (sinthe > 1.0) {
                  if (sinthe-1.0 < tol) {
                     sinthe = 1.0;
                  } else {
                     sinthe = (sinth1 < sinth2) ? sinth1 : sinth2;
                  }
               }

               if (sinthe < -1.0) {
                  if (sinthe+1.0 > -tol) {
                     sinthe = -1.0;
                  }
               }

               if (sinthe > 1.0 || sinthe < -1.0) {
                  *phip = 0.0;
                  *thetap = 0.0;
                  *(statp++) = 1;
                  status = 3;
                  continue;
               }

               *thetap = asind(sinthe);
               z = 1.0 - sinthe;
            }

            x1 = -y0 + eta*z;
            y1 =  x0 -  xi*z;
            if (x1 == 0.0 && y1 == 0.0) {
               *phip = 0.0;
            } else {
               *phip = atan2d(y1,x1);
            }
         }

         *(statp++) = 0;
      }
   }

   return status;
}

/*--------------------------------------------------------------------------*/

int sins2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int mphi, mtheta, rowlen, rowoff, status;
   double cosphi, costhe, sinphi, r, t, z, z1, z2;
   register int iphi, itheta, istat, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != SIN) {
      if (sinset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }

   status = 0;


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      sinphi = sind(*phip);
      cosphi = cosd(*phip);

      xp = x + rowoff;
      yp = y + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = sinphi;
         *yp = cosphi;
         xp += rowlen;
         yp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   xp = x;
   yp = y;
   statp = stat;
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      t = (90.0 - fabs(*thetap))*D2R;
      if (t < 1.0e-5) {
         if (*thetap > 0.0) {
            z = t*t/2.0;
         } else {
            z = 2.0 - t*t/2.0;
         }
         costhe = t;
      } else {
         z = 1.0 - sind(*thetap);
         costhe = cosd(*thetap);
      }
      r = prj->r0*costhe;

      if (prj->w[1] == 0.0) {
         /* Orthographic projection. */
         istat = 0;
         if (prj->bounds && *thetap < 0.0) {
            istat  = 1;
            status = 4;
         }

         for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
            *xp =  r*(*xp) - prj->x0;
            *yp = -r*(*yp) - prj->y0;
            *(statp++) = istat;
         }

      } else {
         /* "Synthesis" projection. */
         z *= prj->r0;
         z1 = prj->pv[1]*z - prj->x0;
         z2 = prj->pv[2]*z - prj->y0;

         for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
            istat = 0;
            if (prj->bounds) {
               t = -atand(prj->pv[1]*(*xp) - prj->pv[2]*(*yp));
               if (*thetap < t) {
                  istat  = 1;
                  status = 4;
               }
            }

            *xp =  r*(*xp) + z1;
            *yp = -r*(*yp) + z2;
            *(statp++) = istat;
         }
      }
   }

   return status;
}

/*============================================================================
*   ARC: zenithal/azimuthal equidistant projection.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to  0.0 if undefined.
*      prj->theta0  Reset to 90.0 if undefined.
*
*   Returned:
*      prj->flag     ARC
*      prj->code    "ARC"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->w[0]    r0*(pi/180)
*      prj->w[1]    (180/pi)/r0
*      prj->prjx2s  Pointer to arcx2s().
*      prj->prjs2x  Pointer to arcs2x().
*===========================================================================*/

int arcset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   prj->flag = ARC;
   strcpy(prj->code, "ARC");

   strcpy(prj->name, "Zenithal/azimuthal equidistant");
   prj->category  = ZENITHAL;
   prj->pvrange   = 0;
   prj->simplezen = 1;
   prj->equiareal = 0;
   prj->conformal = 0;
   prj->global    = 1;
   prj->divergent = 0;

   if (prj->r0 == 0.0) {
      prj->r0 = R2D;
      prj->w[0] = 1.0;
      prj->w[1] = 1.0;
   } else {
      prj->w[0] = prj->r0*D2R;
      prj->w[1] = 1.0/prj->w[0];
   }

   prj->prjx2s = arcx2s;
   prj->prjs2x = arcs2x;

   return prjoff(prj, 0.0, 90.0);
}

/*--------------------------------------------------------------------------*/

int arcx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int mx, my, rowlen, rowoff;
   double r, xj, yj, yj2;
   register int ix, iy, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != ARC) {
      if (arcset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      xj = *xp + prj->x0;

      phip = phi + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip = xj;
         phip += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   phip   = phi;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      yj  = *yp + prj->y0;
      yj2 = yj*yj;

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         xj = *phip;

         r = sqrt(xj*xj + yj2);
         if (r == 0.0) {
            *phip = 0.0;
            *thetap = 90.0;
         } else {
            *phip = atan2d(xj, -yj);
            *thetap = 90.0 - r*prj->w[1];
         }

         *(statp++) = 0;
      }
   }

   return 0;
}

/*--------------------------------------------------------------------------*/

int arcs2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int mphi, mtheta, rowlen, rowoff;
   double cosphi, r, sinphi;
   register int iphi, itheta, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != ARC) {
      if (arcset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      sinphi = sind(*phip);
      cosphi = cosd(*phip);

      xp = x + rowoff;
      yp = y + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = sinphi;
         *yp = cosphi;
         xp += rowlen;
         yp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   xp = x;
   yp = y;
   statp = stat;
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      r =  prj->w[0]*(90.0 - *thetap);

      for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
         *xp =  r*(*xp) - prj->x0;
         *yp = -r*(*yp) - prj->y0;
         *(statp++) = 0;
      }
   }

   return 0;
}

/*============================================================================
*   ZPN: zenithal/azimuthal polynomial projection.
*
*   Given:
*      prj->pv[]    Polynomial coefficients.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to  0.0 if undefined.
*      prj->theta0  Reset to 90.0 if undefined.
*
*   Returned:
*      prj->flag     ZPN
*      prj->code    "ZPN"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->n       Degree of the polynomial, N.
*      prj->w[0]    Co-latitude of the first point of inflection, radian.
*      prj->w[1]    Radius of the first point of inflection (N > 1), radian.
*      prj->prjx2s  Pointer to zpnx2s().
*      prj->prjs2x  Pointer to zpns2x().
*===========================================================================*/

int zpnset(prj)

struct prjprm *prj;

{
   int j, k, m;
   double d, d1, d2, r, zd, zd1, zd2;
   const double tol = 1.0e-13;

   if (prj == 0) return 1;

   strcpy(prj->code, "ZPN");
   prj->flag = ZPN;

   if (undefined(prj->pv[1])) prj->pv[1] = 0.0;
   if (undefined(prj->pv[2])) prj->pv[2] = 0.0;
   if (undefined(prj->pv[3])) prj->pv[3] = 0.0;
   if (prj->r0 == 0.0) prj->r0 = R2D;

   strcpy(prj->name, "Zenithal/azimuthal polynomial");
   prj->category  = ZENITHAL;
   prj->pvrange   = 30;
   prj->simplezen = 1;
   prj->equiareal = 0;
   prj->conformal = 0;
   prj->global    = 0;
   prj->divergent = 0;

   /* Find the highest non-zero coefficient. */
   for (k = PVN-1; k >= 0 && prj->pv[k] == 0.0; k--);
   if (k < 0) return 2;

   prj->n = k;

   if (k < 2) {
      /* No point of inflection. */
      prj->w[0] = PI;

   } else {
      /* Find the point of inflection closest to the pole. */
      zd1 = 0.0;
      d1  = prj->pv[1];
      if (d1 <= 0.0) {
         return 2;
      }

      /* Find the point where the derivative first goes negative. */
      for (j = 0; j < 180; j++) {
         zd2 = j*D2R;
         d2  = 0.0;
         for (m = k; m > 0; m--) {
            d2 = d2*zd2 + m*prj->pv[m];
         }

         if (d2 <= 0.0) break;
         zd1 = zd2;
         d1  = d2;
      }

      if (j == 180) {
         /* No negative derivative -> no point of inflection. */
         zd = PI;
         prj->global = 1;
      } else {
         /* Find where the derivative is zero. */
         for (j = 1; j <= 10; j++) {
            zd = zd1 - d1*(zd2-zd1)/(d2-d1);

            d = 0.0;
            for (m = k; m > 0; m--) {
               d = d*zd + m*prj->pv[m];
            }

            if (fabs(d) < tol) break;

            if (d < 0.0) {
               zd2 = zd;
               d2  = d;
            } else {
               zd1 = zd;
               d1  = d;
            }
         }
      }

      r = 0.0;
      for (m = k; m >= 0; m--) {
         r = r*zd + prj->pv[m];
      }
      prj->w[0] = zd;
      prj->w[1] = r;
   }

   prj->prjx2s = zpnx2s;
   prj->prjs2x = zpns2x;

   return prjoff(prj, 0.0, 90.0);
}

/*--------------------------------------------------------------------------*/

int zpnx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int j, k, m, mx, my, rowlen, rowoff, status;
   double a, b, c, d, lambda, r, r1, r2, rt, xj, yj, yj2, zd, zd1, zd2;
   const double tol = 1.0e-13;
   register int ix, iy, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != ZPN) {
      if (zpnset(prj)) return 2;
   }

   k = prj->n;

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }

   status = 0;


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      xj = *xp + prj->x0;

      phip = phi + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip = xj;
         phip += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   phip   = phi;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      yj  = *yp + prj->y0;
      yj2 = yj*yj;

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         xj = *phip;

         r = sqrt(xj*xj + yj2)/prj->r0;
         if (r == 0.0) {
            *phip = 0.0;
         } else {
            *phip = atan2d(xj, -yj);
         }

         if (k < 1) {
            /* Constant - no solution. */
            return 2;
         } else if (k == 1) {
            /* Linear. */
            zd = (r - prj->pv[0])/prj->pv[1];
         } else if (k == 2) {
            /* Quadratic. */
            a = prj->pv[2];
            b = prj->pv[1];
            c = prj->pv[0] - r;

            d = b*b - 4.0*a*c;
            if (d < 0.0) {
               *thetap = 0.0;
               *(statp++) = 1;
               status = 3;
               continue;
            }
            d = sqrt(d);

            /* Choose solution closest to pole. */
            zd1 = (-b + d)/(2.0*a);
            zd2 = (-b - d)/(2.0*a);
            zd  = (zd1<zd2) ? zd1 : zd2;
            if (zd < -tol) zd = (zd1>zd2) ? zd1 : zd2;
            if (zd < 0.0) {
               if (zd < -tol) {
                  *thetap = 0.0;
                  *(statp++) = 1;
                  status = 3;
                  continue;
               }
               zd = 0.0;
            } else if (zd > PI) {
               if (zd > PI+tol) {
                  *thetap = 0.0;
                  *(statp++) = 1;
                  status = 3;
                  continue;
               }
               zd = PI;
            }
         } else {
            /* Higher order - solve iteratively. */
            zd1 = 0.0;
            r1  = prj->pv[0];
            zd2 = prj->w[0];
            r2  = prj->w[1];

            if (r < r1) {
               if (r < r1-tol) {
                  *thetap = 0.0;
                  *(statp++) = 1;
                  status = 3;
                  continue;
               }
               zd = zd1;
            } else if (r > r2) {
               if (r > r2+tol) {
                  *thetap = 0.0;
                  *(statp++) = 1;
                  status = 3;
                  continue;
               }
               zd = zd2;
            } else {
               /* Disect the interval. */
               for (j = 0; j < 100; j++) {
                  lambda = (r2 - r)/(r2 - r1);
                  if (lambda < 0.1) {
                     lambda = 0.1;
                  } else if (lambda > 0.9) {
                     lambda = 0.9;
                  }

                  zd = zd2 - lambda*(zd2 - zd1);

                  rt = 0.0;
                  for (m = k; m >= 0; m--) {
                      rt = (rt * zd) + prj->pv[m];
                  }

                  if (rt < r) {
                     if (r-rt < tol) break;
                     r1 = rt;
                     zd1 = zd;
                  } else {
                     if (rt-r < tol) break;
                     r2 = rt;
                     zd2 = zd;
                  }

                  if (fabs(zd2-zd1) < tol) break;
               }
            }
         }

         *thetap = 90.0 - zd*R2D;
         *(statp++) = 0;
      }
   }

   return status;
}

/*--------------------------------------------------------------------------*/

int zpns2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int m, mphi, mtheta, rowlen, rowoff, status;
   double cosphi, r, s, sinphi;
   register int iphi, itheta, istat, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != ZPN) {
      if (zpnset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }

   status = 0;


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      sinphi = sind(*phip);
      cosphi = cosd(*phip);

      xp = x + rowoff;
      yp = y + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = sinphi;
         *yp = cosphi;
         xp += rowlen;
         yp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   xp = x;
   yp = y;
   statp = stat;
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      s = (90.0 - *thetap)*D2R;

      r = 0.0;
      for (m = prj->n; m >= 0; m--) {
         r = r*s + prj->pv[m];
      }
      r *= prj->r0;

      istat = 0;
      if (prj->bounds && s > prj->w[0]) {
         istat  = 1;
         status = 4;
      }

      for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
         *xp =  r*(*xp) - prj->x0;
         *yp = -r*(*yp) - prj->y0;
         *(statp++) = istat;
      }
   }

   return status;
}

/*============================================================================
*   ZEA: zenithal/azimuthal equal area projection.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to  0.0 if undefined.
*      prj->theta0  Reset to 90.0 if undefined.
*
*   Returned:
*      prj->flag     ZEA
*      prj->code    "ZEA"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->w[0]    2*r0
*      prj->w[1]    1/(2*r0)
*      prj->prjx2s  Pointer to zeax2s().
*      prj->prjs2x  Pointer to zeas2x().
*===========================================================================*/

int zeaset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   prj->flag = ZEA;
   strcpy(prj->code, "ZEA");

   strcpy(prj->name, "Zenithal/azimuthal equal area");
   prj->category  = ZENITHAL;
   prj->pvrange   = 0;
   prj->simplezen = 1;
   prj->equiareal = 1;
   prj->conformal = 0;
   prj->global    = 1;
   prj->divergent = 0;

   if (prj->r0 == 0.0) {
      prj->r0 = R2D;
      prj->w[0] = 360.0/PI;
      prj->w[1] = PI/360.0;
   } else {
      prj->w[0] = 2.0*prj->r0;
      prj->w[1] = 1.0/prj->w[0];
   }

   prj->prjx2s = zeax2s;
   prj->prjs2x = zeas2x;

   return prjoff(prj, 0.0, 90.0);
}

/*--------------------------------------------------------------------------*/

int zeax2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int mx, my, rowlen, rowoff, status;
   double r, s, xj, yj, yj2;
   const double tol = 1.0e-12;
   register int ix, iy, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != ZEA) {
      if (zeaset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }

   status = 0;


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      xj = *xp + prj->x0;

      phip = phi + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip = xj;
         phip += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   phip   = phi;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      yj  = *yp + prj->y0;
      yj2 = yj*yj;

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         xj  = *phip;

         r = sqrt(xj*xj + yj2);
         if (r == 0.0) {
            *phip = 0.0;
         } else {
            *phip = atan2d(xj, -yj);
         }

         s = r*prj->w[1];
         if (fabs(s) > 1.0) {
            if (fabs(r - prj->w[0]) < tol) {
               *thetap = -90.0;
            } else {
               *thetap = 0.0;
               *(statp++) = 1;
               status = 3;
               continue;
            }
         } else {
            *thetap = 90.0 - 2.0*asind(s);
         }

         *(statp++) = 0;
      }
   }

   return status;
}

/*--------------------------------------------------------------------------*/

int zeas2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int mphi, mtheta, rowlen, rowoff;
   double cosphi, r, sinphi;
   register int iphi, itheta, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != ZEA) {
      if (zeaset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      sinphi = sind(*phip);
      cosphi = cosd(*phip);

      xp = x + rowoff;
      yp = y + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = sinphi;
         *yp = cosphi;
         xp += rowlen;
         yp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   xp = x;
   yp = y;
   statp = stat;
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      r =  prj->w[0]*sind((90.0 - *thetap)/2.0);

      for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
         *xp =  r*(*xp) - prj->x0;
         *yp = -r*(*yp) - prj->y0;
         *(statp++) = 0;
      }
   }

   return 0;
}

/*============================================================================
*   AIR: Airy's projection.
*
*   Given:
*      prj->pv[1]   Latitude theta_b within which the error is minimized, in
*                   degrees.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to  0.0 if undefined.
*      prj->theta0  Reset to 90.0 if undefined.
*
*   Returned:
*      prj->flag     AIR
*      prj->code    "AIR"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->w[0]    2*r0
*      prj->w[1]    ln(cos(xi_b))/tan(xi_b)**2, where xi_b = (90-theta_b)/2
*      prj->w[2]    1/2 - prj->w[1]
*      prj->w[3]    2*r0*prj->w[2]
*      prj->w[4]    tol, cutoff for using small angle approximation, in
*                   radians.
*      prj->w[5]    prj->w[2]*tol
*      prj->w[6]    (180/pi)/prj->w[2]
*      prj->prjx2s  Pointer to airx2s().
*      prj->prjs2x  Pointer to airs2x().
*===========================================================================*/

int airset(prj)

struct prjprm *prj;

{
   const double tol = 1.0e-4;
   double cosxi;

   if (prj == 0) return 1;

   prj->flag = AIR;
   strcpy(prj->code, "AIR");

   if (undefined(prj->pv[1])) prj->pv[1] = 90.0;
   if (prj->r0 == 0.0) prj->r0 = R2D;

   strcpy(prj->name, "Airy's zenithal");
   prj->category  = ZENITHAL;
   prj->pvrange   = 101;
   prj->simplezen = 1;
   prj->equiareal = 0;
   prj->conformal = 0;
   prj->global    = 0;
   prj->divergent = 1;

   prj->w[0] = 2.0*prj->r0;
   if (prj->pv[1] == 90.0) {
      prj->w[1] = -0.5;
      prj->w[2] =  1.0;
   } else if (prj->pv[1] > -90.0) {
      cosxi = cosd((90.0 - prj->pv[1])/2.0);
      prj->w[1] = log(cosxi)*(cosxi*cosxi)/(1.0-cosxi*cosxi);
      prj->w[2] = 0.5 - prj->w[1];
   } else {
      return 2;
   }

   prj->w[3] = prj->w[0] * prj->w[2];
   prj->w[4] = tol;
   prj->w[5] = prj->w[2]*tol;
   prj->w[6] = R2D/prj->w[2];

   prj->prjx2s = airx2s;
   prj->prjs2x = airs2x;

   return prjoff(prj, 0.0, 90.0);
}

/*--------------------------------------------------------------------------*/

int airx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int k, mx, my, rowlen, rowoff, status;
   double cosxi, lambda, r, r1, r2, rt, tanxi, x1, x2, xi, xj, yj, yj2;
   const double tol = 1.0e-12;
   register int ix, iy, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != AIR) {
      if (airset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }

   status = 0;


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      xj = *xp + prj->x0;

      phip = phi + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip = xj;
         phip += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   phip   = phi;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      yj  = *yp + prj->y0;
      yj2 = yj*yj;

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         xj = *phip;

         r = sqrt(xj*xj + yj2)/prj->w[0];
         if (r == 0.0) {
            *phip = 0.0;
         } else {
            *phip = atan2d(xj, -yj);
         }


         if (r == 0.0) {
            xi = 0.0;
         } else if (r < prj->w[5]) {
            xi = r*prj->w[6];
         } else {
            /* Find a solution interval. */
            x1 = x2 = 1.0;
            r1 = r2 = 0.0;
            for (k = 0; k < 30; k++) {
               x2 = x1/2.0;
               tanxi = sqrt(1.0-x2*x2)/x2;
               r2 = -(log(x2)/tanxi + prj->w[1]*tanxi);

               if (r2 >= r) break;
               x1 = x2;
               r1 = r2;
            }
            if (k == 30) {
               *thetap = 0.0;
               *(statp++) = 1;
               status = 3;
               continue;
            }

            for (k = 0; k < 100; k++) {
               /* Weighted division of the interval. */
               lambda = (r2-r)/(r2-r1);
               if (lambda < 0.1) {
                  lambda = 0.1;
               } else if (lambda > 0.9) {
                  lambda = 0.9;
               }
               cosxi = x2 - lambda*(x2-x1);

               tanxi = sqrt(1.0-cosxi*cosxi)/cosxi;
               rt = -(log(cosxi)/tanxi + prj->w[1]*tanxi);

               if (rt < r) {
                  if (r-rt < tol) break;
                  r1 = rt;
                  x1 = cosxi;
               } else {
                  if (rt-r < tol) break;
                  r2 = rt;
                  x2 = cosxi;
               }
            }
            if (k == 100) {
               *thetap = 0.0;
               *(statp++) = 1;
               status = 3;
               continue;
            }

            xi = acosd(cosxi);
         }

         *thetap = 90.0 - 2.0*xi;
         *(statp++) = 0;
      }
   }

   return status;
}

/*--------------------------------------------------------------------------*/

int airs2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int mphi, mtheta, rowlen, rowoff, status;
   double cosphi, cosxi, r, tanxi, xi, sinphi;
   register int iphi, itheta, istat, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != AIR) {
      if (airset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }

   status = 0;


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      sinphi = sind(*phip);
      cosphi = cosd(*phip);

      xp = x + rowoff;
      yp = y + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = sinphi;
         *yp = cosphi;
         xp += rowlen;
         yp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   xp = x;
   yp = y;
   statp = stat;
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      istat = 0;

      if (*thetap == 90.0) {
         r = 0.0;
      } else if (*thetap > -90.0) {
         xi = D2R*(90.0 - *thetap)/2.0;
         if (xi < prj->w[4]) {
            r = xi*prj->w[3];
         } else {
            cosxi = cosd((90.0 - *thetap)/2.0);
            tanxi = sqrt(1.0-cosxi*cosxi)/cosxi;
            r = -prj->w[0]*(log(cosxi)/tanxi + prj->w[1]*tanxi);
         }
      } else {
         r = 0.0;
         istat  = 1;
         status = 4;
      }

      for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
         *xp =  r*(*xp) - prj->x0;
         *yp = -r*(*yp) - prj->y0;
         *(statp++) = istat;
      }
   }

   return status;
}

/*============================================================================
*   CYP: cylindrical perspective projection.
*
*   Given:
*      prj->pv[1]   Distance of point of projection from the centre of the
*                   generating sphere, mu, in units of r0.
*      prj->pv[2]   Radius of the cylinder of projection, lambda, in units of
*                   r0.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to 0.0 if undefined.
*      prj->theta0  Reset to 0.0 if undefined.
*
*   Returned:
*      prj->flag     CYP
*      prj->code    "CYP"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->w[0]    r0*lambda*(pi/180)
*      prj->w[1]    (180/pi)/(r0*lambda)
*      prj->w[2]    r0*(mu + lambda)
*      prj->w[3]    1/(r0*(mu + lambda))
*      prj->prjx2s  Pointer to cypx2s().
*      prj->prjs2x  Pointer to cyps2x().
*===========================================================================*/

int cypset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   prj->flag = CYP;
   strcpy(prj->code, "CYP");

   if (undefined(prj->pv[1])) prj->pv[1] = 1.0;
   if (undefined(prj->pv[2])) prj->pv[2] = 1.0;

   strcpy(prj->name, "Cylindrical perspective");
   prj->category  = CYLINDRICAL;
   prj->pvrange   = 102;
   prj->simplezen = 0;
   prj->equiareal = 0;
   prj->conformal = 0;
   prj->global    = prj->pv[1] < -1.0 || 0.0 < prj->pv[1];
   prj->divergent = !prj->global;

   if (prj->r0 == 0.0) {
      prj->r0 = R2D;

      prj->w[0] = prj->pv[2];
      if (prj->w[0] == 0.0) {
         return 2;
      }

      prj->w[1] = 1.0/prj->w[0];

      prj->w[2] = R2D*(prj->pv[1] + prj->pv[2]);
      if (prj->w[2] == 0.0) {
         return 2;
      }

      prj->w[3] = 1.0/prj->w[2];
   } else {
      prj->w[0] = prj->r0*prj->pv[2]*D2R;
      if (prj->w[0] == 0.0) {
         return 2;
      }

      prj->w[1] = 1.0/prj->w[0];

      prj->w[2] = prj->r0*(prj->pv[1] + prj->pv[2]);
      if (prj->w[2] == 0.0) {
         return 2;
      }

      prj->w[3] = 1.0/prj->w[2];
   }

   prj->prjx2s = cypx2s;
   prj->prjs2x = cyps2x;

   return prjoff(prj, 0.0, 0.0);
}

/*--------------------------------------------------------------------------*/

int cypx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int mx, my, rowlen, rowoff;
   double eta, s, t;
   register int ix, iy, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != CYP) {
      if (cypset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      s = prj->w[1]*(*xp + prj->x0);

      phip = phi + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip = s;
         phip += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   thetap = theta;
   statp = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      eta = prj->w[3]*(*yp + prj->y0);
      t = atan2d(eta,1.0) + asind(eta*prj->pv[1]/sqrt(eta*eta+1.0));

      for (ix = 0; ix < mx; ix++, thetap += spt) {
         *thetap = t;
         *(statp++) = 0;
      }
   }

   return 0;
}

/*--------------------------------------------------------------------------*/

int cyps2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int mphi, mtheta, rowlen, rowoff, status;
   double eta, xi;
   register int iphi, itheta, istat, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != CYP) {
      if (cypset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }

   status = 0;


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      xi = prj->w[0]*(*phip) - prj->x0;

      xp = x + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = xi;
         xp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   yp = y;
   statp = stat;
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      eta = prj->pv[1] + cosd(*thetap);

      istat = 0;
      if (eta == 0.0) {
         istat  = 1;
         status = 4;

      } else {
         eta = prj->w[2]*sind(*thetap)/eta;
      }

      eta -= prj->y0;
      for (iphi = 0; iphi < mphi; iphi++, yp += sxy) {
         *yp = eta;
         *(statp++) = istat;
      }
   }

   return status;
}

/*============================================================================
*   CEA: cylindrical equal area projection.
*
*   Given:
*      prj->pv[1]   Square of the cosine of the latitude at which the
*                   projection is conformal, lambda.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to 0.0 if undefined.
*      prj->theta0  Reset to 0.0 if undefined.
*
*   Returned:
*      prj->flag     CEA
*      prj->code    "CEA"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->w[0]    r0*(pi/180)
*      prj->w[1]    (180/pi)/r0
*      prj->w[2]    r0/lambda
*      prj->w[3]    lambda/r0
*      prj->prjx2s  Pointer to ceax2s().
*      prj->prjs2x  Pointer to ceas2x().
*===========================================================================*/

int ceaset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   prj->flag = CEA;
   strcpy(prj->code, "CEA");

   if (undefined(prj->pv[1])) prj->pv[1] = 1.0;

   strcpy(prj->name, "Cylindrical equal area");
   prj->category  = CYLINDRICAL;
   prj->pvrange   = 101;
   prj->simplezen = 0;
   prj->equiareal = 1;
   prj->conformal = 0;
   prj->global    = 1;
   prj->divergent = 0;

   if (prj->r0 == 0.0) {
      prj->r0 = R2D;
      prj->w[0] = 1.0;
      prj->w[1] = 1.0;
      if (prj->pv[1] <= 0.0 || prj->pv[1] > 1.0) {
         return 2;
      }
      prj->w[2] = prj->r0/prj->pv[1];
      prj->w[3] = prj->pv[1]/prj->r0;
   } else {
      prj->w[0] = prj->r0*D2R;
      prj->w[1] = R2D/prj->r0;
      if (prj->pv[1] <= 0.0 || prj->pv[1] > 1.0) {
         return 2;
      }
      prj->w[2] = prj->r0/prj->pv[1];
      prj->w[3] = prj->pv[1]/prj->r0;
   }

   prj->prjx2s = ceax2s;
   prj->prjs2x = ceas2x;

   return prjoff(prj, 0.0, 0.0);
}

/*--------------------------------------------------------------------------*/

int ceax2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int mx, my, rowlen, rowoff, status;
   double s;
   const double tol = 1.0e-13;
   register int istat, ix, iy, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != CEA) {
      if (ceaset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }

   status = 0;


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      s = prj->w[1]*(*xp + prj->x0);

      phip = phi + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip = s;
         phip += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   thetap = theta;
   statp = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      s = prj->w[3]*(*yp + prj->y0);

      istat = 0;
      if (fabs(s) > 1.0) {
         if (fabs(s) > 1.0+tol) {
            s = 0.0;
            istat  = 1;
            status = 3;
         } else {
            s = copysign(90.0, s);
         }
      } else {
         s = asind(s);
      }

      for (ix = 0; ix < mx; ix++, thetap += spt) {
         *thetap = s;
         *(statp++) = istat;
      }
   }

   return status;
}

/*--------------------------------------------------------------------------*/

int ceas2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int mphi, mtheta, rowlen, rowoff;
   double eta, xi;
   register int iphi, itheta, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != CEA) {
      if (ceaset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      xi = prj->w[0]*(*phip) - prj->x0;

      xp = x + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = xi;
         xp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   yp = y;
   statp = stat;
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      eta = prj->w[2]*sind(*thetap) - prj->y0;

      for (iphi = 0; iphi < mphi; iphi++, yp += sxy) {
         *yp = eta;
         *(statp++) = 0;
      }
   }

   return 0;
}

/*============================================================================
*   CAR: Plate carree projection.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to 0.0 if undefined.
*      prj->theta0  Reset to 0.0 if undefined.
*
*   Returned:
*      prj->flag     CAR
*      prj->code    "CAR"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->w[0]    r0*(pi/180)
*      prj->w[1]    (180/pi)/r0
*      prj->prjx2s  Pointer to carx2s().
*      prj->prjs2x  Pointer to cars2x().
*===========================================================================*/

int carset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   prj->flag = CAR;
   strcpy(prj->code, "CAR");

   strcpy(prj->name, "Plate caree");
   prj->category  = CYLINDRICAL;
   prj->pvrange   = 0;
   prj->simplezen = 0;
   prj->equiareal = 0;
   prj->conformal = 0;
   prj->global    = 1;
   prj->divergent = 0;

   if (prj->r0 == 0.0) {
      prj->r0 = R2D;
      prj->w[0] = 1.0;
      prj->w[1] = 1.0;
   } else {
      prj->w[0] = prj->r0*D2R;
      prj->w[1] = 1.0/prj->w[0];
   }

   prj->prjx2s = carx2s;
   prj->prjs2x = cars2x;

   return prjoff(prj, 0.0, 0.0);
}

/*--------------------------------------------------------------------------*/

int carx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int mx, my, rowlen, rowoff;
   double s, t;
   register int ix, iy, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != CAR) {
      if (carset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      s = prj->w[1]*(*xp + prj->x0);

      phip = phi + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip = s;
         phip += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   thetap = theta;
   statp = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      t = prj->w[1]*(*yp + prj->y0);

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         *thetap = t;
         *(statp++) = 0;
      }
   }

   return 0;
}

/*--------------------------------------------------------------------------*/

int cars2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int mphi, mtheta, rowlen, rowoff;
   double eta, xi;
   register int iphi, itheta, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != CAR) {
      if (carset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      xi = prj->w[0]*(*phip) - prj->x0;

      xp = x + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = xi;
         xp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   yp = y;
   statp = stat;
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      eta = prj->w[0]*(*thetap) - prj->y0;

      for (iphi = 0; iphi < mphi; iphi++, yp += sxy) {
         *yp = eta;
         *(statp++) = 0;
      }
   }

   return 0;
}

/*============================================================================
*   MER: Mercator's projection.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to 0.0 if undefined.
*      prj->theta0  Reset to 0.0 if undefined.
*
*   Returned:
*      prj->flag     MER
*      prj->code    "MER"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->w[0]    r0*(pi/180)
*      prj->w[1]    (180/pi)/r0
*      prj->prjx2s  Pointer to merx2s().
*      prj->prjs2x  Pointer to mers2x().
*===========================================================================*/

int merset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   prj->flag = MER;
   strcpy(prj->code, "MER");

   strcpy(prj->name, "Mercator's");
   prj->category  = CYLINDRICAL;
   prj->pvrange   = 0;
   prj->simplezen = 0;
   prj->equiareal = 0;
   prj->conformal = 1;
   prj->global    = 0;
   prj->divergent = 1;

   if (prj->r0 == 0.0) {
      prj->r0 = R2D;
      prj->w[0] = 1.0;
      prj->w[1] = 1.0;
   } else {
      prj->w[0] = prj->r0*D2R;
      prj->w[1] = 1.0/prj->w[0];
   }

   prj->prjx2s = merx2s;
   prj->prjs2x = mers2x;

   return prjoff(prj, 0.0, 0.0);
}

/*--------------------------------------------------------------------------*/

int merx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int mx, my, rowlen, rowoff;
   double s, t;
   register int ix, iy, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != MER) {
      if (merset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      s = prj->w[1]*(*xp + prj->x0);

      phip = phi + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip = s;
         phip += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      t = 2.0*atand(exp((*yp + prj->y0)/prj->r0)) - 90.0;

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         *thetap = t;
         *(statp++) = 0;
      }
   }

   return 0;
}

/*--------------------------------------------------------------------------*/

int mers2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int mphi, mtheta, rowlen, rowoff, status;
   double eta, xi;
   register int iphi, itheta, istat, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != MER) {
      if (merset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }

   status = 0;


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      xi  = prj->w[0]*(*phip) - prj->x0;

      xp = x + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = xi;
         xp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   yp = y;
   statp = stat;
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      istat = 0;

      if (*thetap <= -90.0 || *thetap >= 90.0) {
         eta = 0.0;
         istat  = 1;
         status = 4;
      } else {
         eta = prj->r0*log(tand((*thetap+90.0)/2.0)) - prj->y0;
      }

      for (iphi = 0; iphi < mphi; iphi++, yp += sxy) {
         *yp = eta;
         *(statp++) = istat;
      }
   }

   return status;
}

/*============================================================================
*   SFL: Sanson-Flamsteed ("global sinusoid") projection.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to 0.0 if undefined.
*      prj->theta0  Reset to 0.0 if undefined.
*
*   Returned:
*      prj->flag     SFL
*      prj->code    "SFL"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->w[0]    r0*(pi/180)
*      prj->w[1]    (180/pi)/r0
*      prj->prjx2s  Pointer to sflx2s().
*      prj->prjs2x  Pointer to sfls2x().
*===========================================================================*/

int sflset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   prj->flag = SFL;
   strcpy(prj->code, "SFL");

   strcpy(prj->name, "Sanson-Flamsteed");
   prj->category  = PSEUDOCYLINDRICAL;
   prj->pvrange   = 0;
   prj->simplezen = 0;
   prj->equiareal = 1;
   prj->conformal = 0;
   prj->global    = 1;
   prj->divergent = 0;

   if (prj->r0 == 0.0) {
      prj->r0 = R2D;
      prj->w[0] = 1.0;
      prj->w[1] = 1.0;
   } else {
      prj->w[0] = prj->r0*D2R;
      prj->w[1] = 1.0/prj->w[0];
   }

   prj->prjx2s = sflx2s;
   prj->prjs2x = sfls2x;

   return prjoff(prj, 0.0, 0.0);
}

/*--------------------------------------------------------------------------*/

int sflx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int mx, my, rowlen, rowoff, status;
   double s, t, yj;
   register int istat, ix, iy, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != SFL) {
      if (sflset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      s = prj->w[1]*(*xp + prj->x0);

      phip = phi + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip = s;
         phip += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   phip   = phi;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      yj = *yp + prj->y0;
      s = cos(yj/prj->r0);

      istat = 0;
      if (s == 0.0) {
         istat  = 1;
         status = 3;
      } else {
         s = 1.0/s;
      }

      t = prj->w[1]*yj;

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         *phip  *= s;
         *thetap = t;
         *(statp++) = 0;
      }
   }

   return 0;
}

/*--------------------------------------------------------------------------*/

int sfls2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int mphi, mtheta, rowlen, rowoff;
   double eta, xi;
   register int iphi, itheta, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != SFL) {
      if (sflset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      xi = prj->w[0]*(*phip);

      xp = x + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = xi;
         xp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   xp = x;
   yp = y;
   statp = stat;
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      xi  = cosd(*thetap);
      eta = prj->w[0]*(*thetap) - prj->y0;

      for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
         *xp = xi*(*xp) - prj->x0;
         *yp = eta;
         *(statp++) = 0;
      }
   }

   return 0;
}

/*============================================================================
*   PAR: parabolic projection.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to 0.0 if undefined.
*      prj->theta0  Reset to 0.0 if undefined.
*
*   Returned:
*      prj->flag     PAR
*      prj->code    "PAR"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->w[0]    r0*(pi/180)
*      prj->w[1]    (180/pi)/r0
*      prj->w[2]    pi*r0
*      prj->w[3]    1/(pi*r0)
*      prj->prjx2s  Pointer to parx2s().
*      prj->prjs2x  Pointer to pars2x().
*===========================================================================*/

int parset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   prj->flag = PAR;
   strcpy(prj->code, "PAR");

   strcpy(prj->name, "Parabolic");
   prj->category  = PSEUDOCYLINDRICAL;
   prj->pvrange   = 0;
   prj->simplezen = 0;
   prj->equiareal = 1;
   prj->conformal = 0;
   prj->global    = 1;
   prj->divergent = 0;

   if (prj->r0 == 0.0) {
      prj->r0 = R2D;
      prj->w[0] = 1.0;
      prj->w[1] = 1.0;
      prj->w[2] = 180.0;
      prj->w[3] = 1.0/prj->w[2];
   } else {
      prj->w[0] = prj->r0*D2R;
      prj->w[1] = 1.0/prj->w[0];
      prj->w[2] = PI*prj->r0;
      prj->w[3] = 1.0/prj->w[2];
   }

   prj->prjx2s = parx2s;
   prj->prjs2x = pars2x;

   return prjoff(prj, 0.0, 0.0);
}

/*--------------------------------------------------------------------------*/

int parx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int mx, my, rowlen, rowoff, status;
   double r, s, t, xj;
   const double tol = 1.0e-13;
   register int istat, ix, iy, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != PAR) {
      if (parset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }

   status = 0;


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      xj = *xp + prj->x0;
      s = prj->w[1]*xj;
      t = fabs(xj) - tol;

      phip   = phi   + rowoff;
      thetap = theta + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip   = s;
         *thetap = t;
         phip   += rowlen;
         thetap += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   phip   = phi;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      r = prj->w[3]*(*yp + prj->y0);

      istat = 0;
      if (r > 1.0 || r < -1.0) {
         s = 0.0;
         t = 0.0;
         istat  = 1;
         status = 3;

      } else {
         s = 1.0 - 4.0*r*r;
         if (s == 0.0) {
            /* Deferred test. */
            istat = -1;
         } else {
            s = 1.0/s;
         }

         t = 3.0*asind(r);
      }

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         if (istat < 0) {
            if (*thetap < 0.0) {
               *(statp++) = 0;
            } else {
               *(statp++) = 1;
               status = 3;
            }
         }

         *phip  *= s;
         *thetap = t;
      }
   }

   return status;
}

/*--------------------------------------------------------------------------*/

int pars2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int mphi, mtheta, rowlen, rowoff;
   double eta, s, xi;
   register int iphi, itheta, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != PAR) {
      if (parset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      xi = prj->w[0]*(*phip);

      xp = x + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = xi;
         xp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   xp = x;
   yp = y;
   statp = stat;
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      s = sind((*thetap)/3.0);
      xi = (1.0 - 4.0*s*s);
      eta = prj->w[2]*s - prj->y0;

      for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
         *xp = xi*(*xp) - prj->x0;
         *yp = eta;
         *(statp++) = 0;
      }
   }

   return 0;
}

/*============================================================================
*   MOL: Mollweide's projection.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to 0.0 if undefined.
*      prj->theta0  Reset to 0.0 if undefined.
*
*   Returned:
*      prj->flag     MOL
*      prj->code    "MOL"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->w[0]    sqrt(2)*r0
*      prj->w[1]    sqrt(2)*r0/90
*      prj->w[2]    1/(sqrt(2)*r0)
*      prj->w[3]    90/r0
*      prj->prjx2s  Pointer to molx2s().
*      prj->prjs2x  Pointer to mols2x().
*===========================================================================*/

int molset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   prj->flag = MOL;
   strcpy(prj->code, "MOL");

   if (prj->r0 == 0.0) prj->r0 = R2D;

   strcpy(prj->name, "Mollweide's");
   prj->category  = PSEUDOCYLINDRICAL;
   prj->pvrange   = 0;
   prj->simplezen = 0;
   prj->equiareal = 1;
   prj->conformal = 0;
   prj->global    = 1;
   prj->divergent = 0;

   prj->w[0] = SQRT2*prj->r0;
   prj->w[1] = prj->w[0]/90.0;
   prj->w[2] = 1.0/prj->w[0];
   prj->w[3] = 90.0/prj->r0;
   prj->w[4] = 2.0/PI;

   prj->prjx2s = molx2s;
   prj->prjs2x = mols2x;

   return prjoff(prj, 0.0, 0.0);
}

/*--------------------------------------------------------------------------*/

int molx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int mx, my, rowlen, rowoff, status;
   double r, s, t, xj, y0, yj, z;
   const double tol = 1.0e-12;
   register int istat, ix, iy, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != MOL) {
      if (molset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }

   status = 0;


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      xj = *xp + prj->x0;
      s = prj->w[3]*xj;
      t = fabs(xj) - tol;

      phip   = phi   + rowoff;
      thetap = theta + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip   = s;
         *thetap = t;
         phip   += rowlen;
         thetap += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   phip   = phi;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      yj = *yp + prj->y0;
      y0 = yj/prj->r0;
      r  = 2.0 - y0*y0;

      istat = 0;
      if (r <= tol) {
         if (r < -tol) {
            istat  = 1;
            status = 3;
         } else {
            /* OK if fabs(x) < tol whence phi = 0.0. */
            istat = -1;
         }

         r = 0.0;
         s = 0.0;

      } else {
         r = sqrt(r);
         s = 1.0/r;
      }

      z = yj*prj->w[2];
      if (fabs(z) > 1.0) {
         if (fabs(z) > 1.0+tol) {
            z = 0.0;
            istat  = 1;
            status = 3;
         } else {
            z = copysign(1.0, z) + y0*r/PI;
         }
      } else {
         z = asin(z)*prj->w[4] + y0*r/PI;
      }

      if (fabs(z) > 1.0) {
         if (fabs(z) > 1.0+tol) {
            z = 0.0;
            istat  = 1;
            status = 3;
         } else {
            z = copysign(1.0, z);
         }
      }

      t = asind(z);

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         if (istat < 0) {
            if (*thetap < 0.0) {
               *(statp++) = 0;
            } else {
               *(statp++) = 1;
               status = 3;
            }
         }

         *phip  *= s;
         *thetap = t;
      }
   }

   return status;
}

/*--------------------------------------------------------------------------*/

int mols2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int k, mphi, mtheta, rowlen, rowoff;
   double eta, gamma, resid, u, v, v0, v1, xi;
   const double tol = 1.0e-13;
   register int iphi, itheta, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != MOL) {
      if (molset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      xi = prj->w[1]*(*phip);

      xp = x + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = xi;
         xp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   xp = x;
   yp = y;
   statp = stat;
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      if (fabs(*thetap) == 90.0) {
         xi  = 0.0;
         eta = copysign(prj->w[0], *thetap);
      } else if (*thetap == 0.0) {
         xi  = 1.0;
         eta = 0.0;
      } else {
         u  = PI*sind(*thetap);
         v0 = -PI;
         v1 =  PI;
         v  = u;
         for (k = 0; k < 100; k++) {
            resid = (v - u) + sin(v);
            if (resid < 0.0) {
               if (resid > -tol) break;
               v0 = v;
            } else {
               if (resid < tol) break;
               v1 = v;
            }
            v = (v0 + v1)/2.0;
         }

         gamma = v/2.0;
         xi  = cos(gamma);
         eta = prj->w[0]*sin(gamma);
      }

      eta -= prj->y0;
      for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
         *xp = xi*(*xp) - prj->x0;
         *yp = eta;
         *(statp++) = 0;
      }
   }

   return 0;
}

/*============================================================================
*   AIT: Hammer-Aitoff projection.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to 0.0 if undefined.
*      prj->theta0  Reset to 0.0 if undefined.
*
*   Returned:
*      prj->flag     AIT
*      prj->code    "AIT"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->w[0]    2*r0**2
*      prj->w[1]    1/(2*r0)**2
*      prj->w[2]    1/(4*r0)**2
*      prj->w[3]    1/(2*r0)
*      prj->prjx2s  Pointer to aitx2s().
*      prj->prjs2x  Pointer to aits2x().
*===========================================================================*/

int aitset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   prj->flag = AIT;
   strcpy(prj->code, "AIT");

   if (prj->r0 == 0.0) prj->r0 = R2D;

   strcpy(prj->name, "Hammer-Aitoff");
   prj->category  = CONVENTIONAL;
   prj->pvrange   = 0;
   prj->simplezen = 0;
   prj->equiareal = 1;
   prj->conformal = 0;
   prj->global    = 1;
   prj->divergent = 0;

   prj->w[0] = 2.0*prj->r0*prj->r0;
   prj->w[1] = 1.0/(2.0*prj->w[0]);
   prj->w[2] = prj->w[1]/4.0;
   prj->w[3] = 1.0/(2.0*prj->r0);

   prj->prjx2s = aitx2s;
   prj->prjs2x = aits2x;

   return prjoff(prj, 0.0, 0.0);
}

/*--------------------------------------------------------------------------*/

int aitx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int mx, my, rowlen, rowoff, status;
   double s, t, x0, xj, y0, yj, yj2, z;
   const double tol = 1.0e-13;
   register int ix, iy, istat, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != AIT) {
      if (aitset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }

   status = 0;


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      xj = *xp + prj->x0;
      s  = 1.0 - xj*xj*prj->w[2];
      t  = xj*prj->w[3];

      phip   = phi   + rowoff;
      thetap = theta + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip   = s;
         *thetap = t;
         phip   += rowlen;
         thetap += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   phip   = phi;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      yj  = *yp + prj->y0;
      yj2 = yj*yj*prj->w[1];

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         s = *phip - yj2;

         istat = 0;
         if (s < 0.5) {
            if (s < 0.5-tol) {
               istat  = 1;
               status = 3;
            }

            s = 0.5;
         }

         z = sqrt(s);
         x0 = 2.0*z*z - 1.0;
         y0 = z*(*thetap);
         if (x0 == 0.0 && y0 == 0.0) {
            *phip = 0.0;
         } else {
            *phip = 2.0*atan2d(y0, x0);
         }

         t = z*yj/prj->r0;
         if (fabs(t) > 1.0) {
            if (fabs(t) > 1.0+tol) {
               istat  = 1;
               status = 3;
            }
            t = copysign(90.0, t);

         } else {
            t = asind(t);
         }

         *thetap = t;
         *(statp++) = istat;
      }
   }

   return status;
}

/*--------------------------------------------------------------------------*/

int aits2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int mphi, mtheta, rowlen, rowoff;
   double cosphi, costhe, sinphi, sinthe, w;
   register int iphi, itheta, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != AIT) {
      if (aitset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      w = (*phip)/2.0;
      sinphi = sind(w);
      cosphi = cosd(w);

      xp = x + rowoff;
      yp = y + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = sinphi;
         *yp = cosphi;
         xp += rowlen;
         yp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   xp = x;
   yp = y;
   statp = stat;
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      costhe = cosd(*thetap);
      sinthe = sind(*thetap);

      for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
         w = sqrt(prj->w[0]/(1.0 + costhe*(*yp)));
         *xp = 2.0*w*costhe*(*xp) - prj->x0;
         *yp = w*sinthe - prj->y0;
         *(statp++) = 0;
      }
   }

   return 0;
}

/*============================================================================
*   COP: conic perspective projection.
*
*   Given:
*      prj->pv[1]   sigma = (theta2+theta1)/2
*      prj->pv[2]   delta = (theta2-theta1)/2, where theta1 and theta2 are the
*                   latitudes of the standard parallels, in degrees.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to sigma if undefined.
*      prj->theta0  Reset to sigma if undefined.
*
*   Returned:
*      prj->flag     COP
*      prj->code    "COP"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->w[0]    C  = sin(sigma)
*      prj->w[1]    1/C
*      prj->w[2]    Y0 = r0*cos(delta)*cot(sigma)
*      prj->w[3]    r0*cos(delta)
*      prj->w[4]    1/(r0*cos(delta)
*      prj->w[5]    cot(sigma)
*      prj->prjx2s  Pointer to copx2s().
*      prj->prjs2x  Pointer to cops2x().
*===========================================================================*/

int copset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   prj->flag = COP;
   strcpy(prj->code, "COP");

   if (undefined(prj->pv[1])) {
      return 2;
   }
   if (undefined(prj->pv[2])) prj->pv[2] = 0.0;
   if (prj->r0 == 0.0) prj->r0 = R2D;

   strcpy(prj->name, "Conic perspective");
   prj->category  = CONIC;
   prj->pvrange   = 102;
   prj->simplezen = 0;
   prj->equiareal = 0;
   prj->conformal = 0;
   prj->global    = 0;
   prj->divergent = 1;

   prj->w[0] = sind(prj->pv[1]);
   if (prj->w[0] == 0.0) {
      return 2;
   }

   prj->w[1] = 1.0/prj->w[0];

   prj->w[3] = prj->r0*cosd(prj->pv[2]);
   if (prj->w[3] == 0.0) {
      return 2;
   }

   prj->w[4] = 1.0/prj->w[3];
   prj->w[5] = 1.0/tand(prj->pv[1]);

   prj->w[2] = prj->w[3]*prj->w[5];

   prj->prjx2s = copx2s;
   prj->prjs2x = cops2x;

   return prjoff(prj, 0.0, prj->pv[1]);
}

/*--------------------------------------------------------------------------*/

int copx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int mx, my, rowlen, rowoff;
   double alpha, dy, dy2, r, xj;
   register int ix, iy, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;

   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != COP) {
      if (copset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      xj = *xp + prj->x0;

      phip = phi + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip = xj;
         phip += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   phip   = phi;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      dy  = prj->w[2] - (*yp + prj->y0);
      dy2 = dy*dy;

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         xj = *phip;

         r = sqrt(xj*xj + dy2);
         if (prj->pv[1] < 0.0) r = -r;

         if (r == 0.0) {
            alpha = 0.0;
         } else {
            alpha = atan2d(xj/r, dy/r);
         }

         *phip = alpha*prj->w[1];
         *thetap = prj->pv[1] + atand(prj->w[5] - r*prj->w[4]);
         *(statp++) = 0;
      }
   }

   return 0;
}

/*--------------------------------------------------------------------------*/

int cops2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int mphi, mtheta, rowlen, rowoff, status;
   double alpha, cosalpha, r, s, t, sinalpha, y0;
   register int iphi, itheta, istat, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;

   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != COP) {
      if (copset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }

   status = 0;


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      alpha = prj->w[0]*(*phip);
      sinalpha = sind(alpha);
      cosalpha = cosd(alpha);

      xp = x + rowoff;
      yp = y + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = sinalpha;
         *yp = cosalpha;
         xp += rowlen;
         yp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   xp = x;
   yp = y;
   statp = stat;
   y0 = prj->y0 - prj->w[2];
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      t = *thetap - prj->pv[1];
      s = cosd(t);

      istat = 0;
      if (s == 0.0) {
         r = 0.0;
         istat  = 1;
         status = 4;

      } else {
         r = prj->w[2] - prj->w[3]*sind(t)/s;

         if (prj->bounds && r*prj->w[0] < 0.0) {
            istat  = 1;
            status = 4;
         }
      }

      for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
         *xp =  r*(*xp) - prj->x0;
         *yp = -r*(*yp) - y0;
         *(statp++) = istat;
      }
   }

   return status;
}

/*============================================================================
*   COE: conic equal area projection.
*
*   Given:
*      prj->pv[1]   sigma = (theta2+theta1)/2
*      prj->pv[2]   delta = (theta2-theta1)/2, where theta1 and theta2 are the
*                   latitudes of the standard parallels, in degrees.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to sigma if undefined.
*      prj->theta0  Reset to sigma if undefined.
*
*   Returned:
*      prj->flag     COE
*      prj->code    "COE"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->w[0]    C = (sin(theta1) + sin(theta2))/2
*      prj->w[1]    1/C
*      prj->w[2]    Y0 = chi*sqrt(psi - 2C*sind(sigma))
*      prj->w[3]    chi = r0/C
*      prj->w[4]    psi = 1 + sin(theta1)*sin(theta2)
*      prj->w[5]    2C
*      prj->w[6]    (1 + sin(theta1)*sin(theta2))*(r0/C)**2
*      prj->w[7]    C/(2*r0**2)
*      prj->w[8]    chi*sqrt(psi + 2C)
*      prj->prjx2s  Pointer to coex2s().
*      prj->prjs2x  Pointer to coes2x().
*===========================================================================*/

int coeset(prj)

struct prjprm *prj;

{
   double theta1, theta2;

   if (prj == 0) return 1;

   prj->flag = COE;
   strcpy(prj->code, "COE");

   if (undefined(prj->pv[1])) {
      return 2;
   }
   if (undefined(prj->pv[2])) prj->pv[2] = 0.0;
   if (prj->r0 == 0.0) prj->r0 = R2D;

   strcpy(prj->name, "Conic equal area");
   prj->category  = CONIC;
   prj->pvrange   = 102;
   prj->simplezen = 0;
   prj->equiareal = 1;
   prj->conformal = 0;
   prj->global    = 1;
   prj->divergent = 0;

   theta1 = prj->pv[1] - prj->pv[2];
   theta2 = prj->pv[1] + prj->pv[2];

   prj->w[0] = (sind(theta1) + sind(theta2))/2.0;
   if (prj->w[0] == 0.0) {
      return 2;
   }

   prj->w[1] = 1.0/prj->w[0];

   prj->w[3] = prj->r0/prj->w[0];
   prj->w[4] = 1.0 + sind(theta1)*sind(theta2);
   prj->w[5] = 2.0*prj->w[0];
   prj->w[6] = prj->w[3]*prj->w[3]*prj->w[4];
   prj->w[7] = 1.0/(2.0*prj->r0*prj->w[3]);
   prj->w[8] = prj->w[3]*sqrt(prj->w[4] + prj->w[5]);

   prj->w[2] = prj->w[3]*sqrt(prj->w[4] - prj->w[5]*sind(prj->pv[1]));

   prj->prjx2s = coex2s;
   prj->prjs2x = coes2x;

   return prjoff(prj, 0.0, prj->pv[1]);
}

/*--------------------------------------------------------------------------*/

int coex2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int mx, my, rowlen, rowoff, status;
   double alpha, dy, dy2, r, t, w, xj;
   const double tol = 1.0e-12;
   register int ix, iy, istat, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;

   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != COE) {
      if (coeset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }

   status = 0;


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      xj = *xp + prj->x0;

      phip = phi + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip = xj;
         phip += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   phip   = phi;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      dy  = prj->w[2] - (*yp + prj->y0);
      dy2 = dy*dy;

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         xj = *phip;

         r = sqrt(xj*xj + dy2);
         if (prj->pv[1] < 0.0) r = -r;

         if (r == 0.0) {
            alpha = 0.0;
         } else {
            alpha = atan2d(xj/r, dy/r);
         }

         istat = 0;
         if (fabs(r - prj->w[8]) < tol) {
            t = -90.0;
         } else {
            w = (prj->w[6] - r*r)*prj->w[7];
            if (fabs(w) > 1.0) {
               if (fabs(w-1.0) < tol) {
                  t = 90.0;
               } else if (fabs(w+1.0) < tol) {
                  t = -90.0;
               } else {
                  t = 0.0;
                  istat  = 1;
                  status = 3;
               }
            } else {
               t = asind(w);
            }
         }

         *phip = alpha*prj->w[1];
         *thetap = t;
         *(statp++) = istat;
      }
   }

   return status;
}

/*--------------------------------------------------------------------------*/

int coes2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int mphi, mtheta, rowlen, rowoff;
   double alpha, cosalpha, r, sinalpha, y0;
   register int iphi, itheta, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;

   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != COE) {
      if (coeset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      alpha = prj->w[0]*(*phip);
      sinalpha = sind(alpha);
      cosalpha = cosd(alpha);

      xp = x + rowoff;
      yp = y + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = sinalpha;
         *yp = cosalpha;
         xp += rowlen;
         yp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   xp = x;
   yp = y;
   statp = stat;
   y0 = prj->y0 - prj->w[2];
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      if (*thetap == -90.0) {
         r = prj->w[8];
      } else {
         r = prj->w[3]*sqrt(prj->w[4] - prj->w[5]*sind(*thetap));
      }

      for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
         *xp =  r*(*xp) - prj->x0;
         *yp = -r*(*yp) - y0;
         *(statp++) = 0;
      }
   }

   return 0;
}

/*============================================================================
*   COD: conic equidistant projection.
*
*   Given:
*      prj->pv[1]   sigma = (theta2+theta1)/2
*      prj->pv[2]   delta = (theta2-theta1)/2, where theta1 and theta2 are the
*                   latitudes of the standard parallels, in degrees.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to sigma if undefined.
*      prj->theta0  Reset to sigma if undefined.
*
*   Returned:
*      prj->flag     COD
*      prj->code    "COD"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->w[0]    C = r0*sin(sigma)*sin(delta)/delta
*      prj->w[1]    1/C
*      prj->w[2]    Y0 = delta*cot(delta)*cot(sigma)
*      prj->w[3]    Y0 + sigma
*      prj->prjx2s  Pointer to codx2s().
*      prj->prjs2x  Pointer to cods2x().
*===========================================================================*/

int codset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   prj->flag = COD;
   strcpy(prj->code, "COD");

   if (undefined(prj->pv[1])) {
      return 2;
   }
   if (undefined(prj->pv[2])) prj->pv[2] = 0.0;
   if (prj->r0 == 0.0) prj->r0 = R2D;

   strcpy(prj->name, "Conic equidistant");
   prj->category  = CONIC;
   prj->pvrange   = 102;
   prj->simplezen = 0;
   prj->equiareal = 0;
   prj->conformal = 0;
   prj->global    = 1;
   prj->divergent = 0;

   if (prj->pv[2] == 0.0) {
      prj->w[0] = prj->r0*sind(prj->pv[1])*D2R;
   } else {
      prj->w[0] = prj->r0*sind(prj->pv[1])*sind(prj->pv[2])/prj->pv[2];
   }

   if (prj->w[0] == 0.0) {
      return 2;
   }

   prj->w[1] = 1.0/prj->w[0];
   prj->w[2] = prj->r0*cosd(prj->pv[2])*cosd(prj->pv[1])/prj->w[0];
   prj->w[3] = prj->w[2] + prj->pv[1];

   prj->prjx2s = codx2s;
   prj->prjs2x = cods2x;

   return prjoff(prj, 0.0, prj->pv[1]);
}

/*--------------------------------------------------------------------------*/

int codx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int mx, my, rowlen, rowoff;
   double alpha, dy, dy2, r, xj;
   register int ix, iy, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;

   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != COD) {
      if (codset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      xj = *xp + prj->x0;

      phip = phi + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip = xj;
         phip += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   phip   = phi;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      dy  = prj->w[2] - (*yp + prj->y0);
      dy2 = dy*dy;

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         xj = *phip;

         r = sqrt(xj*xj + dy2);
         if (prj->pv[1] < 0.0) r = -r;

         if (r == 0.0) {
            alpha = 0.0;
         } else {
            alpha = atan2d(xj/r, dy/r);
         }

         *phip = alpha*prj->w[1];
         *thetap = prj->w[3] - r;
         *(statp++) = 0;
      }
   }

   return 0;
}

/*--------------------------------------------------------------------------*/

int cods2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int mphi, mtheta, rowlen, rowoff;
   double alpha, cosalpha, r, sinalpha, y0;
   register int iphi, itheta, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;

   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != COD) {
      if (codset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      alpha = prj->w[0]*(*phip);
      sinalpha = sind(alpha);
      cosalpha = cosd(alpha);

      xp = x + rowoff;
      yp = y + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = sinalpha;
         *yp = cosalpha;
         xp += rowlen;
         yp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   xp = x;
   yp = y;
   statp = stat;
   y0 = prj->y0 - prj->w[2];
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      r = prj->w[3] - *thetap;

      for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
         *xp =  r*(*xp) - prj->x0;
         *yp = -r*(*yp) - y0;
         *(statp++) = 0;
      }
   }

   return 0;
}

/*============================================================================
*   COO: conic orthomorphic projection.
*
*   Given:
*      prj->pv[1]   sigma = (theta2+theta1)/2
*      prj->pv[2]   delta = (theta2-theta1)/2, where theta1 and theta2 are the
*                   latitudes of the standard parallels, in degrees.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to sigma if undefined.
*      prj->theta0  Reset to sigma if undefined.
*
*   Returned:
*      prj->flag     COO
*      prj->code    "COO"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->w[0]    C = ln(cos(theta2)/cos(theta1))/ln(tan(tau2)/tan(tau1))
*                       where tau1 = (90 - theta1)/2
*                             tau2 = (90 - theta2)/2
*      prj->w[1]    1/C
*      prj->w[2]    Y0 = psi*tan((90-sigma)/2)**C
*      prj->w[3]    psi = (r0*cos(theta1)/C)/tan(tau1)**C
*      prj->w[4]    1/psi
*      prj->prjx2s  Pointer to coox2s().
*      prj->prjs2x  Pointer to coos2x().
*===========================================================================*/

int cooset(prj)

struct prjprm *prj;

{
   double cos1, cos2, tan1, tan2, theta1, theta2;

   if (prj == 0) return 1;

   prj->flag = COO;
   strcpy(prj->code, "COO");

   if (undefined(prj->pv[1])) {
      return 2;
   }
   if (undefined(prj->pv[2])) prj->pv[2] = 0.0;
   if (prj->r0 == 0.0) prj->r0 = R2D;

   strcpy(prj->name, "Conic orthomorphic");
   prj->category  = CONIC;
   prj->pvrange   = 102;
   prj->simplezen = 0;
   prj->equiareal = 0;
   prj->conformal = 1;
   prj->global    = 0;
   prj->divergent = 1;

   theta1 = prj->pv[1] - prj->pv[2];
   theta2 = prj->pv[1] + prj->pv[2];

   tan1 = tand((90.0 - theta1)/2.0);
   cos1 = cosd(theta1);

   if (theta1 == theta2) {
      prj->w[0] = sind(theta1);
   } else {
      tan2 = tand((90.0 - theta2)/2.0);
      cos2 = cosd(theta2);
      prj->w[0] = log(cos2/cos1)/log(tan2/tan1);
   }
   if (prj->w[0] == 0.0) {
      return 2;
   }

   prj->w[1] = 1.0/prj->w[0];

   prj->w[3] = prj->r0*(cos1/prj->w[0])/pow(tan1,prj->w[0]);
   if (prj->w[3] == 0.0) {
      return 2;
   }
   prj->w[2] = prj->w[3]*pow(tand((90.0 - prj->pv[1])/2.0),prj->w[0]);
   prj->w[4] = 1.0/prj->w[3];

   prj->prjx2s = coox2s;
   prj->prjs2x = coos2x;

   return prjoff(prj, 0.0, prj->pv[1]);
}

/*--------------------------------------------------------------------------*/

int coox2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int mx, my, rowlen, rowoff, status;
   double alpha, dy, dy2, r, t, xj;
   register int ix, iy, istat, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;

   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != COO) {
      if (cooset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }

   status = 0;


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      xj = *xp + prj->x0;

      phip = phi + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip = xj;
         phip += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   phip   = phi;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      dy  = prj->w[2] - (*yp + prj->y0);
      dy2 = dy*dy;

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         xj = *phip;

         r = sqrt(xj*xj + dy2);
         if (prj->pv[1] < 0.0) r = -r;

         if (r == 0.0) {
            alpha = 0.0;
         } else {
            alpha = atan2d(xj/r, dy/r);
         }

         istat = 0;
         if (r == 0.0) {
            if (prj->w[0] < 0.0) {
               t = -90.0;
            } else {
               t = 0.0;
               istat  = 1;
               status = 3;
            }
         } else {
            t = 90.0 - 2.0*atand(pow(r*prj->w[4],prj->w[1]));
         }

         *phip = alpha*prj->w[1];
         *thetap = t;
         *(statp++) = istat;
      }
   }

   return status;
}

/*--------------------------------------------------------------------------*/

int coos2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int mphi, mtheta, rowlen, rowoff, status;
   double alpha, cosalpha, r, sinalpha, y0;
   register int iphi, itheta, istat, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;

   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != COO) {
      if (cooset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }

   status = 0;


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      alpha = prj->w[0]*(*phip);
      sinalpha = sind(alpha);
      cosalpha = cosd(alpha);

      xp = x + rowoff;
      yp = y + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = sinalpha;
         *yp = cosalpha;
         xp += rowlen;
         yp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   xp = x;
   yp = y;
   statp = stat;
   y0 = prj->y0 - prj->w[2];
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      istat = 0;

      if (*thetap == -90.0) {
         r = 0.0;
         if (prj->w[0] >= 0.0) {
            istat  = 1;
            status = 4;
         }
      } else {
         r = prj->w[3]*pow(tand((90.0 - *thetap)/2.0),prj->w[0]);
      }

      for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
         *xp =  r*(*xp) - prj->x0;
         *yp = -r*(*yp) - y0;
         *(statp++) = istat;
      }
   }

   return status;
}

/*============================================================================
*   BON: Bonne's projection.
*
*   Given:
*      prj->pv[1]   Bonne conformal latitude, theta1, in degrees.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to 0.0 if undefined.
*      prj->theta0  Reset to 0.0 if undefined.
*
*   Returned:
*      prj->flag     BON
*      prj->code    "BON"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->w[1]    r0*pi/180
*      prj->w[2]    Y0 = r0*(cot(theta1) + theta1*pi/180)
*      prj->prjx2s  Pointer to bonx2s().
*      prj->prjs2x  Pointer to bons2x().
*===========================================================================*/

int bonset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   prj->flag = BON;
   strcpy(prj->code, "BON");

   if (undefined(prj->pv[1])) {
      return 2;
   }

   if (prj->pv[1] == 0.0) {
      /* Sanson-Flamsteed. */
      return sflset(prj);
   }

   strcpy(prj->name, "Bonne's");
   prj->category  = POLYCONIC;
   prj->pvrange   = 101;
   prj->simplezen = 0;
   prj->equiareal = 1;
   prj->conformal = 0;
   prj->global    = 1;
   prj->divergent = 0;

   if (prj->r0 == 0.0) {
      prj->r0 = R2D;
      prj->w[1] = 1.0;
      prj->w[2] = prj->r0*cosd(prj->pv[1])/sind(prj->pv[1]) + prj->pv[1];
   } else {
      prj->w[1] = prj->r0*D2R;
      prj->w[2] = prj->r0*(cosd(prj->pv[1])/sind(prj->pv[1]) +
                           prj->pv[1]*D2R);
   }

   prj->prjx2s = bonx2s;
   prj->prjs2x = bons2x;

   return prjoff(prj, 0.0, 0.0);
}

/*--------------------------------------------------------------------------*/

int bonx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int mx, my, rowlen, rowoff;
   double alpha, dy, dy2, costhe, r, s, t, xj;
   register int ix, iy, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->pv[1] == 0.0) {
      /* Sanson-Flamsteed. */
      return sflx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat);
   }

   if (prj->flag != BON) {
      if (bonset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      xj = *xp + prj->x0;

      phip = phi + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip = xj;
         phip += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   phip   = phi;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      dy  = prj->w[2] - (*yp + prj->y0);
      dy2 = dy*dy;

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         xj = *phip;

         r = sqrt(xj*xj + dy2);
         if (prj->pv[1] < 0.0) r = -r;

         if (r == 0.0) {
            alpha = 0.0;
         } else {
            alpha = atan2d(xj/r, dy/r);
         }

         t = (prj->w[2] - r)/prj->w[1];
         costhe = cosd(t);
         if (costhe == 0.0) {
            s = 0.0;
         } else {
            s = alpha*(r/prj->r0)/costhe;
         }

         *phip = s;
         *thetap = t;
         *(statp++) = 0;
      }
   }

   return 0;
}

/*--------------------------------------------------------------------------*/

int bons2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int mphi, mtheta, rowlen, rowoff;
   double alpha, r, s, y0;
   register int iphi, itheta, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;

   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->pv[1] == 0.0) {
      /* Sanson-Flamsteed. */
      return sfls2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat);
   }

   if (prj->flag != BON) {
      if (bonset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }

   y0 = prj->y0 - prj->w[2];


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      s = prj->r0*(*phip);

      xp = x + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = s;
         xp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   xp = x;
   yp = y;
   statp = stat;
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      r = prj->w[2] - prj->w[1]*(*thetap);
      s = cosd(*thetap)/r;

      for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
         alpha = s*(*xp);
         *xp =  r*sind(alpha) - prj->x0;
         *yp = -r*cosd(alpha) - y0;
         *(statp++) = 0;
      }
   }

   return 0;
}

/*============================================================================
*   PCO: polyconic projection.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to 0.0 if undefined.
*      prj->theta0  Reset to 0.0 if undefined.
*
*   Returned:
*      prj->flag     PCO
*      prj->code    "PCO"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->w[0]    r0*(pi/180)
*      prj->w[1]    1/r0
*      prj->w[2]    2*r0
*      prj->prjx2s  Pointer to pcox2s().
*      prj->prjs2x  Pointer to pcos2x().
*===========================================================================*/

int pcoset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   prj->flag = PCO;
   strcpy(prj->code, "PCO");

   strcpy(prj->name, "Polyconic");
   prj->category  = POLYCONIC;
   prj->pvrange   = 0;
   prj->simplezen = 0;
   prj->equiareal = 0;
   prj->conformal = 0;
   prj->global    = 1;
   prj->divergent = 0;

   if (prj->r0 == 0.0) {
      prj->r0 = R2D;
      prj->w[0] = 1.0;
      prj->w[1] = 1.0;
      prj->w[2] = 360.0/PI;
   } else {
      prj->w[0] = prj->r0*D2R;
      prj->w[1] = 1.0/prj->w[0];
      prj->w[2] = 2.0*prj->r0;
   }

   prj->prjx2s = pcox2s;
   prj->prjs2x = pcos2x;

   return prjoff(prj, 0.0, 0.0);
}

/*--------------------------------------------------------------------------*/

int pcox2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int mx, my, rowlen, rowoff;
   double f, fneg, fpos, lambda, tanthe, the, theneg, thepos, w, x1, xj, xx,
          yj, ymthe, y1;
   const double tol = 1.0e-12;
   register int ix, iy, k, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != PCO) {
      if (pcoset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      xj = *xp + prj->x0;

      phip = phi + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip = xj;
         phip += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   phip   = phi;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      yj = *yp + prj->y0;
      w  = fabs(yj*prj->w[1]);

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         xj = *phip;

         if (w < tol) {
            *phip = xj*prj->w[1];
            *thetap = 0.0;

         } else if (fabs(w-90.0) < tol) {
            *phip = 0.0;
            *thetap = copysign(90.0, yj);

         } else {
            /* Iterative solution using weighted division of the interval. */
            if (yj > 0.0) {
               thepos =  90.0;
            } else {
               thepos = -90.0;
            }
            theneg = 0.0;

            xx = xj*xj;
            ymthe = yj - prj->w[0]*thepos;
            fpos = xx + ymthe*ymthe;
            fneg = -999.0;

            for (k = 0; k < 64; k++) {
               if (fneg < -100.0) {
                  /* Equal division of the interval. */
                  the = (thepos+theneg)/2.0;
               } else {
                  /* Weighted division of the interval. */
                  lambda = fpos/(fpos-fneg);
                  if (lambda < 0.1) {
                     lambda = 0.1;
                  } else if (lambda > 0.9) {
                     lambda = 0.9;
                  }
                  the = thepos - lambda*(thepos-theneg);
               }

               /* Compute the residue. */
               ymthe = yj - prj->w[0]*(the);
               tanthe = tand(the);
               f = xx + ymthe*(ymthe - prj->w[2]/tanthe);

               /* Check for convergence. */
               if (fabs(f) < tol) break;
               if (fabs(thepos-theneg) < tol) break;

               /* Redefine the interval. */
               if (f > 0.0) {
                  thepos = the;
                  fpos = f;
               } else {
                  theneg = the;
                  fneg = f;
               }
            }

            x1 = prj->r0 - ymthe*tanthe;
            y1 = xj*tanthe;
            if (x1 == 0.0 && y1 == 0.0) {
               *phip = 0.0;
            } else {
               *phip = atan2d(y1, x1)/sind(the);
            }

            *thetap = the;
         }

         *(statp++) = 0;
      }
   }

   return 0;
}

/*--------------------------------------------------------------------------*/

int pcos2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int mphi, mtheta, rowlen, rowoff;
   double alpha, costhe, cotthe, sinthe, therad;
   register int iphi, itheta, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;

   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != PCO) {
      if (pcoset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      xp = x + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = *phip;
         xp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   xp = x;
   yp = y;
   statp = stat;
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      therad = (*thetap)*D2R;
      costhe = cosd(*thetap);
      sinthe = sind(*thetap);

      for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
         if (sinthe == 0.0) {
            *xp =  prj->w[0]*(*xp) - prj->x0;
            *yp = -prj->y0;
         } else {
            alpha  = (*xp)*sinthe;
            cotthe = costhe/sinthe;
            *xp = prj->r0*cotthe*sind(alpha) - prj->x0;
            *yp = prj->r0*(cotthe*(1.0 - cosd(alpha)) + therad) - prj->y0;
         }

         *(statp++) = 0;
      }
   }

   return 0;
}

/*============================================================================
*   TSC: tangential spherical cube projection.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to 0.0 if undefined.
*      prj->theta0  Reset to 0.0 if undefined.
*
*   Returned:
*      prj->flag     TSC
*      prj->code    "TSC"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->w[0]    r0*(pi/4)
*      prj->w[1]    (4/pi)/r0
*      prj->prjx2s  Pointer to tscx2s().
*      prj->prjs2x  Pointer to tscs2x().
*===========================================================================*/

int tscset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   prj->flag = TSC;
   strcpy(prj->code, "TSC");

   strcpy(prj->name, "Tangential spherical cube");
   prj->category  = QUADCUBE;
   prj->pvrange   = 0;
   prj->simplezen = 0;
   prj->equiareal = 0;
   prj->conformal = 0;
   prj->global    = 1;
   prj->divergent = 0;

   if (prj->r0 == 0.0) {
      prj->r0 = R2D;
      prj->w[0] = 45.0;
      prj->w[1] = 1.0/45.0;
   } else {
      prj->w[0] = prj->r0*PI/4.0;
      prj->w[1] = 1.0/prj->w[0];
   }

   prj->prjx2s = tscx2s;
   prj->prjs2x = tscs2x;

   return prjoff(prj, 0.0, 0.0);
}

/*--------------------------------------------------------------------------*/

int tscx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int mx, my, rowlen, rowoff, status;
   double l, m, n, xf, yf;
   register int ix, iy, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != TSC) {
      if (tscset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }

   status = 0;


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      xf = (*xp + prj->x0)*prj->w[1];

      phip = phi + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip = xf;
         phip += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   phip   = phi;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      yf = (*yp + prj->y0)*prj->w[1];

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         xf = *phip;

         /* Check bounds. */
         if (fabs(xf) <= 1.0) {
            if (fabs(yf) > 3.0) {
               *phip = 0.0;
               *thetap = 0.0;
               *(statp++) = 1;
               status = 3;
               continue;
            }
         } else {
            if (fabs(xf) > 7.0 || fabs(yf) > 1.0) {
               *phip = 0.0;
               *thetap = 0.0;
               *(statp++) = 1;
               status = 3;
               continue;
            }
         }

         /* Map negative faces to the other side. */
         if (xf < -1.0) xf += 8.0;

         /* Determine the face. */
         if (xf > 5.0) {
            /* face = 4 */
            xf = xf - 6.0;
            m  = -1.0/sqrt(1.0 + xf*xf + yf*yf);
            l  = -m*xf;
            n  = -m*yf;
         } else if (xf > 3.0) {
            /* face = 3 */
            xf = xf - 4.0;
            l  = -1.0/sqrt(1.0 + xf*xf + yf*yf);
            m  =  l*xf;
            n  = -l*yf;
         } else if (xf > 1.0) {
            /* face = 2 */
            xf = xf - 2.0;
            m  =  1.0/sqrt(1.0 + xf*xf + yf*yf);
            l  = -m*xf;
            n  =  m*yf;
         } else if (yf > 1.0) {
            /* face = 0 */
            yf = yf - 2.0;
            n  = 1.0/sqrt(1.0 + xf*xf + yf*yf);
            l  = -n*yf;
            m  =  n*xf;
         } else if (yf < -1.0) {
            /* face = 5 */
            yf = yf + 2.0;
            n  = -1.0/sqrt(1.0 + xf*xf + yf*yf);
            l  = -n*yf;
            m  = -n*xf;
         } else {
            /* face = 1 */
            l  =  1.0/sqrt(1.0 + xf*xf + yf*yf);
            m  =  l*xf;
            n  =  l*yf;
         }

         if (l == 0.0 && m == 0.0) {
            *phip = 0.0;
         } else {
            *phip = atan2d(m, l);
         }

         *thetap = asind(n);
         *(statp++) = 0;
      }
   }

   return status;
}

/*--------------------------------------------------------------------------*/

int tscs2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int face, mphi, mtheta, rowlen, rowoff, status;
   double cosphi, costhe, l, m, n, sinphi, sinthe, x0, xf, y0, yf, zeta;
   const double tol = 1.0e-12;
   register int iphi, istat, itheta, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;

   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != TSC) {
      if (tscset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }

   status = 0;


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      cosphi = cosd(*phip);
      sinphi = sind(*phip);

      xp = x + rowoff;
      yp = y + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = cosphi;
         *yp = sinphi;
         xp += rowlen;
         yp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   xp = x;
   yp = y;
   statp = stat;
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      costhe = cosd(*thetap);
      sinthe = sind(*thetap);

      for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
         l = costhe*(*xp);
         m = costhe*(*yp);
         n = sinthe;

         face = 0;
         zeta = n;
         if (l > zeta) {
            face = 1;
            zeta = l;
         }
         if (m > zeta) {
            face = 2;
            zeta = m;
         }
         if (-l > zeta) {
            face = 3;
            zeta = -l;
         }
         if (-m > zeta) {
            face = 4;
            zeta = -m;
         }
         if (-n > zeta) {
            face = 5;
            zeta = -n;
         }

         switch (face) {
         case 1:
            xf =  m/zeta;
            yf =  n/zeta;
            x0 =  0.0;
            y0 =  0.0;
            break;
         case 2:
            xf = -l/zeta;
            yf =  n/zeta;
            x0 =  2.0;
            y0 =  0.0;
            break;
         case 3:
            xf = -m/zeta;
            yf =  n/zeta;
            x0 =  4.0;
            y0 =  0.0;
            break;
         case 4:
            xf =  l/zeta;
            yf =  n/zeta;
            x0 =  6.0;
            y0 =  0.0;
            break;
         case 5:
            xf =  m/zeta;
            yf =  l/zeta;
            x0 =  0.0;
            y0 = -2.0;
            break;
         default:
            /* face == 0 */
            xf =  m/zeta;
            yf = -l/zeta;
            x0 =  0.0;
            y0 =  2.0;
            break;
         }

         istat = 0;
         if (fabs(xf) > 1.0) {
            if (fabs(xf) > 1.0+tol) {
               istat  = 1;
               status = 4;
            }
            xf = copysign(1.0, xf);
         }
         if (fabs(yf) > 1.0) {
            if (fabs(yf) > 1.0+tol) {
               istat  = 1;
               status = 4;
            }
            yf = copysign(1.0, yf);
         }

         *xp = prj->w[0]*(xf + x0) - prj->x0;
         *yp = prj->w[0]*(yf + y0) - prj->y0;
         *(statp++) = istat;
      }
   }

   return status;
}

/*============================================================================
*   CSC: COBE quadrilateralized spherical cube projection.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to 0.0 if undefined.
*      prj->theta0  Reset to 0.0 if undefined.
*
*   Returned:
*      prj->flag     CSC
*      prj->code    "CSC"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->w[0]    r0*(pi/4)
*      prj->w[1]    (4/pi)/r0
*      prj->prjx2s  Pointer to cscx2s().
*      prj->prjs2x  Pointer to cscs2x().
*===========================================================================*/

int cscset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   prj->flag = CSC;
   strcpy(prj->code, "CSC");

   strcpy(prj->name, "COBE quadrilateralized spherical cube");
   prj->category  = QUADCUBE;
   prj->pvrange   = 0;
   prj->simplezen = 0;
   prj->equiareal = 0;
   prj->conformal = 0;
   prj->global    = 1;
   prj->divergent = 0;

   if (prj->r0 == 0.0) {
      prj->r0 = R2D;
      prj->w[0] = 45.0;
      prj->w[1] = 1.0/45.0;
   } else {
      prj->w[0] = prj->r0*PI/4.0;
      prj->w[1] = 1.0/prj->w[0];
   }

   prj->prjx2s = cscx2s;
   prj->prjs2x = cscs2x;

   return prjoff(prj, 0.0, 0.0);
}

/*--------------------------------------------------------------------------*/

int cscx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int face, mx, my, rowlen, rowoff, status;
   double l, m, n;
   register int ix, iy, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;

   float     chi, psi, xf, xx, yf, yy, z0, z1, z2, z3, z4, z5, z6;
   const float p00 = -0.27292696;
   const float p10 = -0.07629969;
   const float p20 = -0.22797056;
   const float p30 =  0.54852384;
   const float p40 = -0.62930065;
   const float p50 =  0.25795794;
   const float p60 =  0.02584375;
   const float p01 = -0.02819452;
   const float p11 = -0.01471565;
   const float p21 =  0.48051509;
   const float p31 = -1.74114454;
   const float p41 =  1.71547508;
   const float p51 = -0.53022337;
   const float p02 =  0.27058160;
   const float p12 = -0.56800938;
   const float p22 =  0.30803317;
   const float p32 =  0.98938102;
   const float p42 = -0.83180469;
   const float p03 = -0.60441560;
   const float p13 =  1.50880086;
   const float p23 = -0.93678576;
   const float p33 =  0.08693841;
   const float p04 =  0.93412077;
   const float p14 = -1.41601920;
   const float p24 =  0.33887446;
   const float p05 = -0.63915306;
   const float p15 =  0.52032238;
   const float p06 =  0.14381585;

   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != CSC) {
      if (cscset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }

   status = 0;


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      xf = (*xp + prj->x0)*prj->w[1];

      phip = phi + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip = xf;
         phip += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   phip   = phi;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      yf = (*yp + prj->y0)*prj->w[1];

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         xf = *phip;

         /* Check bounds. */
         if (fabs(xf) <= 1.0) {
            if (fabs(yf) > 3.0) {
               *phip = 0.0;
               *thetap = 0.0;
               *(statp++) = 1;
               status = 3;
               continue;
            }
         } else {
            if (fabs(xf) > 7.0 || fabs(yf) > 1.0) {
               *phip = 0.0;
               *thetap = 0.0;
               *(statp++) = 1;
               status = 3;
               continue;
            }
         }

         /* Map negative faces to the other side. */
         if (xf < -1.0) xf += 8.0;

         /* Determine the face. */
         if (xf > 5.0) {
            face = 4;
            xf = xf - 6.0;
         } else if (xf > 3.0) {
            face = 3;
            xf = xf - 4.0;
         } else if (xf > 1.0) {
            face = 2;
            xf = xf - 2.0;
         } else if (yf > 1.0) {
            face = 0;
            yf = yf - 2.0;
         } else if (yf < -1.0) {
            face = 5;
            yf = yf + 2.0;
         } else {
            face = 1;
         }

         xx  =  xf*xf;
         yy  =  yf*yf;

         z0 = p00 + xx*(p10 + xx*(p20 + xx*(p30 + xx*(p40 + xx*(p50 +
                    xx*(p60))))));
         z1 = p01 + xx*(p11 + xx*(p21 + xx*(p31 + xx*(p41 + xx*(p51)))));
         z2 = p02 + xx*(p12 + xx*(p22 + xx*(p32 + xx*(p42))));
         z3 = p03 + xx*(p13 + xx*(p23 + xx*(p33)));
         z4 = p04 + xx*(p14 + xx*(p24));
         z5 = p05 + xx*(p15);
         z6 = p06;

         chi = z0 + yy*(z1 + yy*(z2 + yy*(z3 + yy*(z4 + yy*(z5 + yy*z6)))));
         chi = xf + xf*(1.0 - xx)*chi;

         z0 = p00 + yy*(p10 + yy*(p20 + yy*(p30 + yy*(p40 + yy*(p50 +
                    yy*(p60))))));
         z1 = p01 + yy*(p11 + yy*(p21 + yy*(p31 + yy*(p41 + yy*(p51)))));
         z2 = p02 + yy*(p12 + yy*(p22 + yy*(p32 + yy*(p42))));
         z3 = p03 + yy*(p13 + yy*(p23 + yy*(p33)));
         z4 = p04 + yy*(p14 + yy*(p24));
         z5 = p05 + yy*(p15);
         z6 = p06;

         psi = z0 + xx*(z1 + xx*(z2 + xx*(z3 + xx*(z4 + xx*(z5 + xx*z6)))));
         psi = yf + yf*(1.0 - yy)*psi;

         switch (face) {
         case 1:
            l =  1.0/sqrt(chi*chi + psi*psi + 1.0);
            m =  chi*l;
            n =  psi*l;
            break;
         case 2:
            m =  1.0/sqrt(chi*chi + psi*psi + 1.0);
            l = -chi*m;
            n =  psi*m;
            break;
         case 3:
            l = -1.0/sqrt(chi*chi + psi*psi + 1.0);
            m =  chi*l;
            n = -psi*l;
            break;
         case 4:
            m = -1.0/sqrt(chi*chi + psi*psi + 1.0);
            l = -chi*m;
            n = -psi*m;
            break;
         case 5:
            n = -1.0/sqrt(chi*chi + psi*psi + 1.0);
            l = -psi*n;
            m = -chi*n;
            break;
         default:
            /* face == 0 */
            n =  1.0/sqrt(chi*chi + psi*psi + 1.0);
            l = -psi*n;
            m =  chi*n;
            break;
         }

         if (l == 0.0 && m == 0.0) {
            *phip = 0.0;
         } else {
            *phip = atan2d(m, l);
         }

         *thetap = asind(n);
         *(statp++) = 0;
      }
   }

   return status;
}

/*--------------------------------------------------------------------------*/

int cscs2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int face, mphi, mtheta, rowlen, rowoff, status;
   double cosphi, costhe, eta, l, m, n, sinphi, sinthe, xi, zeta;
   const float tol = 1.0e-7;
   register int iphi, istat, itheta, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;

   float chi, chi2, chi2psi2, chi4, chipsi, psi, psi2, psi4, chi2co, psi2co, x0, xf, y0, yf;
   const float gstar  =  1.37484847732;
   const float mm     =  0.004869491981;
   const float gamma  = -0.13161671474;
   const float omega1 = -0.159596235474;
   const float d0  =  0.0759196200467;
   const float d1  = -0.0217762490699;
   const float c00 =  0.141189631152;
   const float c10 =  0.0809701286525;
   const float c01 = -0.281528535557;
   const float c11 =  0.15384112876;
   const float c20 = -0.178251207466;
   const float c02 =  0.106959469314;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != CSC) {
      if (cscset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }

   status = 0;


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      cosphi = cosd(*phip);
      sinphi = sind(*phip);

      xp = x + rowoff;
      yp = y + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = cosphi;
         *yp = sinphi;
         xp += rowlen;
         yp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   xp = x;
   yp = y;
   statp = stat;
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      costhe = cosd(*thetap);
      sinthe = sind(*thetap);

      for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
         l = costhe*(*xp);
         m = costhe*(*yp);
         n = sinthe;

         face = 0;
         zeta = n;
         if (l > zeta) {
            face = 1;
            zeta = l;
         }
         if (m > zeta) {
            face = 2;
            zeta = m;
         }
         if (-l > zeta) {
            face = 3;
            zeta = -l;
         }
         if (-m > zeta) {
            face = 4;
            zeta = -m;
         }
         if (-n > zeta) {
            face = 5;
            zeta = -n;
         }

         switch (face) {
         case 1:
            xi  =  m;
            eta =  n;
            x0  =  0.0;
            y0  =  0.0;
            break;
         case 2:
            xi  = -l;
            eta =  n;
            x0  =  2.0;
            y0  =  0.0;
            break;
         case 3:
            xi  = -m;
            eta =  n;
            x0  =  4.0;
            y0  =  0.0;
            break;
         case 4:
            xi  =  l;
            eta =  n;
            x0  =  6.0;
            y0  =  0.0;
            break;
         case 5:
            xi  =  m;
            eta =  l;
            x0  =  0.0;
            y0  = -2.0;
            break;
         default:
            /* face == 0 */
            xi  =  m;
            eta = -l;
            x0  =  0.0;
            y0  =  2.0;
            break;
         }

         chi =  xi/zeta;
         psi = eta/zeta;

         chi2 = chi*chi;
         psi2 = psi*psi;
         chi2co = 1.0 - chi2;
         psi2co = 1.0 - psi2;

         /* Avoid floating underflows. */
         chipsi = fabs(chi*psi);
         chi4   = (chi2 > 1.0e-16) ? chi2*chi2 : 0.0;
         psi4   = (psi2 > 1.0e-16) ? psi2*psi2 : 0.0;
         chi2psi2 = (chipsi > 1.0e-16) ? chi2*psi2 : 0.0;

         xf = chi*(chi2 + chi2co*(gstar + psi2*(gamma*chi2co + mm*chi2 +
                psi2co*(c00 + c10*chi2 + c01*psi2 + c11*chi2psi2 + c20*chi4 +
                c02*psi4)) + chi2*(omega1 - chi2co*(d0 + d1*chi2))));
         yf = psi*(psi2 + psi2co*(gstar + chi2*(gamma*psi2co + mm*psi2 +
                chi2co*(c00 + c10*psi2 + c01*chi2 + c11*chi2psi2 + c20*psi4 +
                c02*chi4)) + psi2*(omega1 - psi2co*(d0 + d1*psi2))));

         istat = 0;
         if (fabs(xf) > 1.0) {
            if (fabs(xf) > 1.0+tol) {
               istat  = 1;
               status = 4;
            }
            xf = copysign(1.0, xf);
         }
         if (fabs(yf) > 1.0) {
            if (fabs(yf) > 1.0+tol) {
               istat  = 1;
               status = 4;
            }
            yf = copysign(1.0, yf);
         }

         *xp = prj->w[0]*(xf + x0) - prj->x0;
         *yp = prj->w[0]*(yf + y0) - prj->y0;
         *(statp++) = istat;
      }
   }

   return status;
}

/*============================================================================
*   QSC: quadrilaterilized spherical cube projection.
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to 0.0 if undefined.
*      prj->theta0  Reset to 0.0 if undefined.
*
*   Returned:
*      prj->flag     QSC
*      prj->code    "QSC"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->w[0]    r0*(pi/4)
*      prj->w[1]    (4/pi)/r0
*      prj->prjx2s  Pointer to qscx2s().
*      prj->prjs2x  Pointer to qscs2x().
*===========================================================================*/

int qscset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   prj->flag = QSC;
   strcpy(prj->code, "QSC");

   strcpy(prj->name, "Quadrilateralized spherical cube");
   prj->category  = QUADCUBE;
   prj->pvrange   = 0;
   prj->simplezen = 0;
   prj->equiareal = 1;
   prj->conformal = 0;
   prj->global    = 1;
   prj->divergent = 0;

   if (prj->r0 == 0.0) {
      prj->r0 = R2D;
      prj->w[0] = 45.0;
      prj->w[1] = 1.0/45.0;
   } else {
      prj->w[0] = prj->r0*PI/4.0;
      prj->w[1] = 1.0/prj->w[0];
   }

   prj->prjx2s = qscx2s;
   prj->prjs2x = qscs2x;

   return prjoff(prj, 0.0, 0.0);
}

/*--------------------------------------------------------------------------*/

int qscx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int direct, face, mx, my, rowlen, rowoff, status;
   double l, m, n, omega, tau, xf, yf, w, zeco, zeta;
   const double tol = 1.0e-12;
   register int ix, iy, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != QSC) {
      if (qscset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }

   status = 0;


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      xf = (*xp + prj->x0)*prj->w[1];

      phip = phi + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip = xf;
         phip += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   phip   = phi;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      yf = (*yp + prj->y0)*prj->w[1];

      for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
         xf = *phip;

         /* Check bounds. */
         if (fabs(xf) <= 1.0) {
            if (fabs(yf) > 3.0) {
               *phip = 0.0;
               *thetap = 0.0;
               *(statp++) = 1;
               status = 3;
               continue;
            }
         } else {
            if (fabs(xf) > 7.0 || fabs(yf) > 1.0) {
               *phip = 0.0;
               *thetap = 0.0;
               *(statp++) = 1;
               status = 3;
               continue;
            }
         }

         /* Map negative faces to the other side. */
         if (xf < -1.0) xf += 8.0;

         /* Determine the face. */
         if (xf > 5.0) {
            face = 4;
            xf -= 6.0;
         } else if (xf > 3.0) {
            face = 3;
            xf -= 4.0;
         } else if (xf > 1.0) {
            face = 2;
            xf -= 2.0;
         } else if (yf > 1.0) {
            face = 0;
            yf -= 2.0;
         } else if (yf < -1.0) {
            face = 5;
            yf += 2.0;
         } else {
            face = 1;
         }

         direct = (fabs(xf) > fabs(yf));
         if (direct) {
            if (xf == 0.0) {
               omega = 0.0;
               tau  = 1.0;
               zeta = 1.0;
               zeco = 0.0;
            } else {
               w = 15.0*yf/xf;
               omega = sind(w)/(cosd(w) - SQRT2INV);
               tau  = 1.0 + omega*omega;
               zeco = xf*xf*(1.0 - 1.0/sqrt(1.0 + tau));
               zeta = 1.0 - zeco;
            }
         } else {
            if (yf == 0.0) {
               omega = 0.0;
               tau  = 1.0;
               zeta = 1.0;
               zeco = 0.0;
            } else {
               w = 15.0*xf/yf;
               omega = sind(w)/(cosd(w) - SQRT2INV);
               tau  = 1.0 + omega*omega;
               zeco = yf*yf*(1.0 - 1.0/sqrt(1.0 + tau));
               zeta = 1.0 - zeco;
            }
         }

         if (zeta < -1.0) {
            if (zeta < -1.0-tol) {
               *phip = 0.0;
               *thetap = 0.0;
               *(statp++) = 1;
               status = 3;
               continue;
            }

            zeta = -1.0;
            zeco =  2.0;
            w    =  0.0;
         } else {
            w = sqrt(zeco*(2.0-zeco)/tau);
         }

         switch (face) {
         case 1:
            l = zeta;
            if (direct) {
               m = w;
               if (xf < 0.0) m = -m;
               n = m*omega;
            } else {
               n = w;
               if (yf < 0.0) n = -n;
               m = n*omega;
            }
            break;
         case 2:
            m = zeta;
            if (direct) {
               l = w;
               if (xf > 0.0) l = -l;
               n = -l*omega;
            } else {
               n = w;
               if (yf < 0.0) n = -n;
               l = -n*omega;
            }
            break;
         case 3:
            l = -zeta;
            if (direct) {
               m = w;
               if (xf > 0.0) m = -m;
               n = -m*omega;
            } else {
               n = w;
               if (yf < 0.0) n = -n;
               m = -n*omega;
            }
            break;
         case 4:
            m = -zeta;
            if (direct) {
               l = w;
               if (xf < 0.0) l = -l;
               n = l*omega;
            } else {
               n = w;
               if (yf < 0.0) n = -n;
               l = n*omega;
            }
            break;
         case 5:
            n = -zeta;
            if (direct) {
               m = w;
               if (xf < 0.0) m = -m;
               l = m*omega;
            } else {
               l = w;
               if (yf < 0.0) l = -l;
               m = l*omega;
            }
            break;
         default:
            /* face == 0 */
            n = zeta;
            if (direct) {
               m = w;
               if (xf < 0.0) m = -m;
               l = -m*omega;
            } else {
               l = w;
               if (yf > 0.0) l = -l;
               m = -l*omega;
            }
            break;
         }

         if (l == 0.0 && m == 0.0) {
            *phip = 0.0;
         } else {
            *phip = atan2d(m, l);
         }

         *thetap = asind(n);
         *(statp++) = 0;
      }
   }

   return status;
}

/*--------------------------------------------------------------------------*/

int qscs2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int face, mphi, mtheta, rowlen, rowoff, status;
   double cosphi, costhe, eta, l, m, n, omega, p, sinphi, sinthe, t, tau, x0,
          xf, xi, y0, yf, zeco, zeta;
   const double tol = 1.0e-12;
   register int iphi, istat, itheta, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != QSC) {
      if (qscset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }

   status = 0;


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      cosphi = cosd(*phip);
      sinphi = sind(*phip);

      xp = x + rowoff;
      yp = y + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = cosphi;
         *yp = sinphi;
         xp += rowlen;
         yp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   xp = x;
   yp = y;
   statp = stat;
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      costhe = cosd(*thetap);
      sinthe = sind(*thetap);

      for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
         if (fabs(*thetap) == 90.0) {
            *xp = -prj->x0;
            *yp = copysign(2.0*prj->w[0], *thetap) - prj->y0;
            *(statp++) = 0;
            continue;
         }

         l = costhe*(*xp);
         m = costhe*(*yp);
         n = sinthe;

         face = 0;
         zeta = n;
         if (l > zeta) {
            face = 1;
            zeta = l;
         }
         if (m > zeta) {
            face = 2;
            zeta = m;
         }
         if (-l > zeta) {
            face = 3;
            zeta = -l;
         }
         if (-m > zeta) {
            face = 4;
            zeta = -m;
         }
         if (-n > zeta) {
            face = 5;
            zeta = -n;
         }

         zeco = 1.0 - zeta;

         switch (face) {
         case 1:
            xi  = m;
            eta = n;
            if (zeco < 1.0e-8) {
               /* Small angle formula. */
               t = (*thetap)*D2R;
               p = atan2(*yp, *xp);
               zeco = (p*p + t*t)/2.0;
            }
            x0 = 0.0;
            y0 = 0.0;
            break;
         case 2:
            xi  = -l;
            eta =  n;
            if (zeco < 1.0e-8) {
               /* Small angle formula. */
               t = (*thetap)*D2R;
               p = atan2(*yp, *xp) - PI/2.0;
               zeco = (p*p + t*t)/2.0;
            }
            x0 = 2.0;
            y0 = 0.0;
            break;
         case 3:
            xi  = -m;
            eta =  n;
            if (zeco < 1.0e-8) {
               /* Small angle formula. */
               t = (*thetap)*D2R;
               p = atan2(*yp, *xp);
               p -= copysign(PI, p);
               zeco = (p*p + t*t)/2.0;
            }
            x0 = 4.0;
            y0 = 0.0;
            break;
         case 4:
            xi  = l;
            eta = n;
            if (zeco < 1.0e-8) {
               /* Small angle formula. */
               t = (*thetap)*D2R;
               p = atan2(*yp, *xp) + PI/2.0;
               zeco = (p*p + t*t)/2.0;
            }
            x0 = 6;
            y0 = 0.0;
            break;
         case 5:
            xi  =  m;
            eta =  l;
            if (zeco < 1.0e-8) {
               /* Small angle formula. */
               t = (*thetap + 90.0)*D2R;
               zeco = t*t/2.0;
            }
            x0 =  0.0;
            y0 = -2;
            break;
         default:
            /* face == 0 */
            xi  =  m;
            eta = -l;
            if (zeco < 1.0e-8) {
               /* Small angle formula. */
               t = (90.0 - *thetap)*D2R;
               zeco = t*t/2.0;
            }
            x0 = 0.0;
            y0 = 2.0;
            break;
         }

         xf = 0.0;
         yf = 0.0;
         if (xi != 0.0 || eta != 0.0) {
            if (-xi > fabs(eta)) {
               omega = eta/xi;
               tau = 1.0 + omega*omega;
               xf  = -sqrt(zeco/(1.0 - 1.0/sqrt(1.0+tau)));
               yf  = (xf/15.0)*(atand(omega) - asind(omega/sqrt(tau+tau)));
            } else if (xi > fabs(eta)) {
               omega = eta/xi;
               tau = 1.0 + omega*omega;
               xf  =  sqrt(zeco/(1.0 - 1.0/sqrt(1.0+tau)));
               yf  = (xf/15.0)*(atand(omega) - asind(omega/sqrt(tau+tau)));
            } else if (-eta >= fabs(xi)) {
               omega = xi/eta;
               tau = 1.0 + omega*omega;
               yf  = -sqrt(zeco/(1.0 - 1.0/sqrt(1.0+tau)));
               xf  = (yf/15.0)*(atand(omega) - asind(omega/sqrt(tau+tau)));
            } else if (eta >= fabs(xi)) {
               omega = xi/eta;
               tau = 1.0 + omega*omega;
               yf  =  sqrt(zeco/(1.0 - 1.0/sqrt(1.0+tau)));
               xf  = (yf/15.0)*(atand(omega) - asind(omega/sqrt(tau+tau)));
            }
         }

         istat = 0;
         if (fabs(xf) > 1.0) {
            if (fabs(xf) > 1.0+tol) {
               istat  = 1;
               status = 4;
            }
            xf = copysign(1.0, xf);
         }
         if (fabs(yf) > 1.0) {
            if (fabs(yf) > 1.0+tol) {
               istat  = 1;
               status = 4;
            }
            yf = copysign(1.0, yf);
         }

         *xp = prj->w[0]*(xf + x0) - prj->x0;
         *yp = prj->w[0]*(yf + y0) - prj->y0;
         *(statp++) = istat;
      }
   }

   return status;
}

/*============================================================================
*   HPX: HEALPix projection.
*
*   Given:
*      prj->pv[1]   H - the number of facets in longitude.
*      prj->pv[2]   K - the number of facets in latitude
*
*   Given and/or returned:
*      prj->r0      Reset to 180/pi if 0.
*      prj->phi0    Reset to 0.0 if undefined.
*      prj->theta0  Reset to 0.0 if undefined.
*
*   Returned:
*      prj->flag     HPX
*      prj->code    "HPX"
*      prj->x0      Fiducial offset in x.
*      prj->y0      Fiducial offset in y.
*      prj->n       True if K is odd.
*      prj->w[0]    r0*(pi/180)
*      prj->w[1]    (180/pi)/r0
*      prj->w[2]    (K-1)/K
*      prj->w[3]    90*K/H
*      prj->w[4]    (K+1)/2
*      prj->w[5]    90*(K-1)/H
*      prj->w[6]    180/H
*      prj->w[7]    H/360
*      prj->w[8]    (90*K/H)*r0*(pi/180)
*      prj->w[9]     (180/H)*r0*(pi/180)
*      prj->prjx2s  Pointer to hpxx2s().
*      prj->prjs2x  Pointer to hpxs2x().
*===========================================================================*/

int hpxset(prj)

struct prjprm *prj;

{
   if (prj == 0) return 1;

   prj->flag = HPX;
   strcpy(prj->code, "HPX");

   if (undefined(prj->pv[1])) prj->pv[1] = 4.0;
   if (undefined(prj->pv[2])) prj->pv[2] = 3.0;

   strcpy(prj->name, "HEALPix");
   prj->category  = HEALPIX;
   prj->pvrange   = 102;
   prj->simplezen = 0;
   prj->equiareal = 1;
   prj->conformal = 0;
   prj->global    = 1;
   prj->divergent = 0;

   if (prj->pv[1] <= 0.0 || prj->pv[2] <= 0.0) {
      return 2;
   }

   prj->n = ((int)prj->pv[2])%2;

   if (prj->r0 == 0.0) {
      prj->r0 = R2D;
      prj->w[0] = 1.0;
      prj->w[1] = 1.0;
   } else {
      prj->w[0] = prj->r0*D2R;
      prj->w[1] = R2D/prj->r0;
   }

   prj->w[2] = (prj->pv[2] - 1.0) / prj->pv[2];
   prj->w[3] = 90.0 * prj->pv[2] / prj->pv[1];
   prj->w[4] = (prj->pv[2] + 1.0) / 2.0;
   prj->w[5] = 90.0 * (prj->pv[2] - 1.0) / prj->pv[1];
   prj->w[6] = 180.0 / prj->pv[1];
   prj->w[7] = prj->pv[1] / 360.0;
   prj->w[8] = prj->w[3] * prj->w[0];
   prj->w[9] = prj->w[6] * prj->w[0];

   prj->prjx2s = hpxx2s;
   prj->prjs2x = hpxs2x;

   return prjoff(prj, 0.0, 0.0);
}

/*--------------------------------------------------------------------------*/

int hpxx2s(prj, nx, ny, sxy, spt, x, y, phi, theta, stat)

struct prjprm *prj;
int nx, ny, sxy, spt;
const double x[], y[];
double phi[], theta[];
int stat[];

{
   int h, mx, my, offset, rowlen, rowoff, status;
   double absy, s, sigma, t, yr;
   register int istat, ix, iy, *statp;
   register const double *xp, *yp;
   register double *phip, *thetap;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != HPX) {
      if (hpxset(prj)) return 2;
   }

   if (ny > 0) {
      mx = nx;
      my = ny;
   } else {
      mx = 1;
      my = 1;
      ny = nx;
   }

   status = 0;


   /* Do x dependence. */
   xp = x;
   rowoff = 0;
   rowlen = nx*spt;
   for (ix = 0; ix < nx; ix++, rowoff += spt, xp += sxy) {
      s = prj->w[1] * (*xp + prj->x0);
      t = -180.0 + (2.0 * floor((*xp + 180.0) * prj->w[7]) + 1.0) * prj->w[6];
      t = prj->w[1] * (*xp - t);

      phip   = phi + rowoff;
      thetap = theta + rowoff;
      for (iy = 0; iy < my; iy++) {
         *phip   = s;
         *thetap = t;
         phip   += rowlen;
         thetap += rowlen;
      }
   }


   /* Do y dependence. */
   yp = y;
   phip   = phi;
   thetap = theta;
   statp  = stat;
   for (iy = 0; iy < ny; iy++, yp += sxy) {
      yr = prj->w[1]*(*yp + prj->y0);
      absy = fabs(yr);

      istat = 0;
      if (absy <= prj->w[5]) {
         t = asind(yr/prj->w[3]);
         for (ix = 0; ix < mx; ix++, thetap += spt) {
            *thetap = t;
            *(statp++) = 0;
         }

      } else {
         offset = (prj->n || *yp > 0.0) ? 0 : 1;

         sigma = prj->w[4] - absy / prj->w[6];

         if (sigma == 0.0) {
            s = 0.0;
            t = 90.0;

         } else {
            t = 1.0 - sigma*sigma/prj->pv[2];
            if (t < -1.0) {
               s = 0.0;
               t = 0.0;
               istat  = 1;
               status = 3;
            } else {
               s = 1.0/sigma - 1.0;
               t = asind(t);
            }
         }
         if (*yp < 0.0) t = -t;

         for (ix = 0; ix < mx; ix++, phip += spt, thetap += spt) {
            if (offset) {
               /* Offset the southern polar half-facets for even K. */
               h = (int)floor(*phip / prj->w[6]);
               if (h%2) {
                  *thetap -= prj->w[6];
               } else {
                  *thetap += prj->w[6];
               }
            }
            *phip += *thetap * s;
            *thetap = t;
            *(statp++) = istat;
         }
      }
   }

   return status;
}

/*--------------------------------------------------------------------------*/

int hpxs2x(prj, nphi, ntheta, spt, sxy, phi, theta, x, y, stat)

struct prjprm *prj;
int nphi, ntheta, spt, sxy;
const double phi[], theta[];
double x[], y[];
int stat[];

{
   int h, mphi, mtheta, offset, rowlen, rowoff;
   double abssin, eta, sigma, sinthe, t, xi;
   register int iphi, itheta, *statp;
   register const double *phip, *thetap;
   register double *xp, *yp;


   /* Initialize. */
   if (prj == 0) return 1;
   if (prj->flag != HPX) {
      if (hpxset(prj)) return 2;
   }

   if (ntheta > 0) {
      mphi   = nphi;
      mtheta = ntheta;
   } else {
      mphi   = 1;
      mtheta = 1;
      ntheta = nphi;
   }


   /* Do phi dependence. */
   phip = phi;
   rowoff = 0;
   rowlen = nphi*sxy;
   for (iphi = 0; iphi < nphi; iphi++, rowoff += sxy, phip += spt) {
      xi = prj->w[0] * (*phip) - prj->x0;
      t  = -180.0 + (2.0*floor((*phip+180.0) * prj->w[7]) + 1.0) * prj->w[6];
      t  = prj->w[0] * (*phip - t);

      xp = x + rowoff;
      yp = y + rowoff;
      for (itheta = 0; itheta < mtheta; itheta++) {
         *xp = xi;
         *yp = t;
         xp += rowlen;
         yp += rowlen;
      }
   }


   /* Do theta dependence. */
   thetap = theta;
   xp = x;
   yp = y;
   statp = stat;
   for (itheta = 0; itheta < ntheta; itheta++, thetap += spt) {
      sinthe = sind(*thetap);
      abssin = fabs(sinthe);

      if (abssin <= prj->w[2]) {
         eta = prj->w[8] * sinthe - prj->y0;
         for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
            *yp = eta;
            *(statp++) = 0;
         }

      } else {
         offset = (prj->n || *thetap > 0.0) ? 0 : 1;

         sigma = sqrt(prj->pv[2]*(1.0 - abssin));
         xi = sigma - 1.0;

         eta = prj->w[9] * (prj->w[4] - sigma);
         if (*thetap < 0) eta = -eta;
         eta -= prj->y0;

         for (iphi = 0; iphi < mphi; iphi++, xp += sxy, yp += sxy) {
            if (offset) {
               /* Offset the southern polar half-facets for even K. */
               h = (int)floor((*xp + prj->x0) / prj->w[9]);
               if (h%2) {
                  *yp -= prj->w[9];
               } else {
                  *yp += prj->w[9];
               }
            }
            *xp += *yp * xi;
            *yp = eta;
            *(statp++) = 0;
         }
      }
   }

   return 0;
}
