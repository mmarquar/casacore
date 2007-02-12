      SUBROUTINE iau_PVSTAR ( PV, RA, DEC, PMR, PMD, PX, RV, J )
*+
*  - - - - - - - - - - -
*   i a u _ P V S T A R
*  - - - - - - - - - - -
*
*  Convert star position+velocity vector to catalog coordinates.
*
*  This routine is part of the International Astronomical Union's
*  SOFA (Standards of Fundamental Astronomy) software collection.
*
*  Status:  support routine.
*
*  Given (Note 1):
*     PV       d(3,2)    pv-vector (AU, AU/day)
*
*  Returned (Note 2):
*     RA       d         right ascension (radians)
*     DEC      d         declination (radians)
*     PMR      d         RA proper motion (radians/year)
*     PMD      d         Dec proper motion (radians/year)
*     PX       d         parallax (arcsec)
*     RV       d         radial velocity (km/s, positive = receding)
*     J        i         status:
*                           0 = OK
*                          -1 = superluminal speed (Note 5)
*                          -2 = null position vector
*
*  Notes:
*
*  1) The specified pv-vector is the coordinate direction (and its rate
*     of change) for the epoch at which the light leaving the star
*     reached the solar-system barycenter.
*
*  2) The star data returned by this routine are "observables" for an
*     imaginary observer at the solar-system barycenter.  Proper motion
*     and radial velocity are, strictly, in terms of barycentric
*     coordinate time, TCB.  For most practical applications, it is
*     permissible to neglect the distinction between TCB and ordinary
*     "proper" time on Earth (TT/TAI).  The result will, as a rule, be
*     limited by the intrinsic accuracy of the proper-motion and radial-
*     velocity data;  moreover, the supplied pv-vector is likely to be
*     merely an intermediate result (for example generated by the
*     routine iau_STARPV), so that a change of time unit will cancel
*     out overall.
*
*     In accordance with normal star-catalog conventions, the object's
*     right ascension and declination are freed from the effects of
*     secular aberration.  The frame, which is aligned to the catalog
*     equator and equinox, is Lorentzian and centered on the SSB.
*
*     Summarizing, the specified pv-vector is for most stars almost
*     identical to the result of applying the standard geometrical
*     "space motion" transformation to the catalog data.  The
*     differences, which are the subject of the Stumpff paper cited
*     below, are:
*
*     (i) In stars with significant radial velocity and proper motion,
*     the constantly changing light-time distorts the apparent proper
*     motion.  Note that this is a classical, not a relativistic,
*     effect.
*
*     (ii) The transformation complies with special relativity.
*
*  3) Care is needed with units.  The star coordinates are in radians
*     and the proper motions in radians per Julian year, but the
*     parallax is in arcseconds; the radial velocity is in km/s, but
*     the pv-vector result is in AU and AU/day.
*
*  4) The proper motions are the rate of change of the right ascension
*     and declination at the catalog epoch and are in radians per Julian
*     year.  The RA proper motion is in terms of coordinate angle, not
*     true angle, and will thus be numerically larger at high
*     declinations.
*
*  5) Straight-line motion at constant speed in the inertial frame is
*     assumed.  If the speed is greater than or equal to the speed of
*     light, the routine aborts with an error status.
*
*  6) The inverse transformation is performed by the routine iau_STARPV.
*
*  Called:
*     iau_PN       normalize p-vector returning modulus
*     iau_PDP      scalar product
*     iau_SXP      multiply p-vector by scalar
*     iau_PMP      p-vector minus p-vector
*     iau_PM       modulus of p-vector
*     iau_PPP      p-vector plus p-vector
*     iau_PV2S     pv-vector to spherical coordinates
*     iau_ANP      normalize radians to range 0 to 2pi
*
*  Reference:
*
*     Stumpff, P., Astron.Astrophys. 144, 232-240 (1985).
*
*  This revision:  2005 August 26
*
*  Copyright (C) 2005 IAU SOFA Review Board.  See notes at end.
*
*-----------------------------------------------------------------------

      IMPLICIT NONE

      DOUBLE PRECISION PV(3,2), RA, DEC, PMR, PMD, PX, RV
      INTEGER J

*  Julian years to days
      DOUBLE PRECISION Y2D
      PARAMETER ( Y2D = 365.25D0 )

*  Radians to arc seconds
      DOUBLE PRECISION DR2AS
      PARAMETER ( DR2AS = 206264.8062470963551564734D0 )

*  Days to seconds
      DOUBLE PRECISION D2S
      PARAMETER ( D2S = 86400D0 )

*  AU (m)
      DOUBLE PRECISION AUM
      PARAMETER ( AUM = 149597870D3 )

*  Speed of light (AU per day)
      DOUBLE PRECISION C
      PARAMETER ( C = D2S/499.004782D0 )

      DOUBLE PRECISION R, X(3), VR, UR(3), VT, UT(3), BETT, BETR, D, W,
     :                 DEL, USR(3), UST(3), A, RAD, DECD, RD
      DOUBLE PRECISION iau_ANP

* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

*  Isolate the radial component of the velocity (AU/day, inertial).
      CALL iau_PN ( PV(1,1), R, X )
      CALL iau_PDP ( X, PV(1,2), VR )
      CALL iau_SXP ( VR, X, UR )

*  Isolate the transverse component of the velocity (AU/day, inertial).
      CALL iau_PMP ( PV(1,2), UR, UT )
      CALL iau_PM ( UT, VT )

*  Special-relativity dimensionless parameters.
      BETT = VT / C
      BETR = VR / C

*  The inertial-to-observed correction terms.
      D = 1D0 + BETR
      W = 1D0 - BETR*BETR - BETT*BETT
      IF ( D.EQ.0D0 .OR. W.LT.0D0 ) THEN
         J = -1
         GO TO 9
      END IF
      DEL = SQRT(W) - 1D0

*  Apply relativistic correction factor to radial velocity component.
      IF ( BETR.NE.0D0 ) THEN
         W = ( BETR-DEL ) / ( BETR*D )
      ELSE
         W = 1D0
      END IF
      CALL iau_SXP ( W, UR, USR )

*  Apply relativistic correction factor to tangential velocity component.
      CALL iau_SXP ( 1D0/D, UT, UST )

*  Combine the two to obtain the observed velocity vector (AU/day).
      CALL iau_PPP ( USR, UST, PV(1,2) )

*  Cartesian to spherical.
      CALL iau_PV2S ( PV, A, DEC, R, RAD, DECD, RD )
      IF ( R .EQ. 0D0 ) THEN
         J = -2
         GO TO 9
      END IF

*  Return RA in range 0 to 2pi.
      RA = iau_ANP ( A )

*  Return proper motions in radians per year.
      PMR = RAD * Y2D
      PMD = DECD * Y2D

*  Return parallax in arcsec.
      PX = DR2AS / R

*  Return radial velocity in km/s.
      RV = 1D-3 * RD * AUM / D2S

*  OK status.
      J = 0

*  Exit.
 9    CONTINUE

*  Finished.

*+----------------------------------------------------------------------
*
*  Copyright (C) 2005
*  Standards Of Fundamental Astronomy Review Board
*  of the International Astronomical Union.
*
*  =====================
*  SOFA Software License
*  =====================
*
*  NOTICE TO USER:
*
*  BY USING THIS SOFTWARE YOU ACCEPT THE FOLLOWING TERMS AND CONDITIONS
*  WHICH APPLY TO ITS USE.
*
*  1. The Software is owned by the IAU SOFA Review Board ("the Board").
*
*  2. The Software is made available free of charge for use by:
*
*     a) private individuals for non-profit research; and
*
*     b) non-profit educational, academic and research institutions.
*
*  3. Commercial use of the Software is specifically excluded from the
*     terms and conditions of this license.  Commercial use of the
*     Software is subject to the prior written agreement of the Board on
*     terms to be agreed.
*
*  4. The provision of any version of the Software under the terms and
*     conditions specified herein does not imply that future versions
*     will also be made available under the same terms and conditions.
*
*  5. The user may modify the Software for his/her own purposes.  The
*     user may distribute the modified software provided that the Board
*     is informed and that a copy of the modified software is made
*     available to the Board on request.  All modifications made by the
*     user shall be clearly identified to show how the modified software
*     differs from the original Software, and the name(s) of the
*     affected routine(s) shall be changed.  The original SOFA Software
*     License text must be present.
*
*  6. In any published work produced by the user and which includes
*     results achieved by using the Software, the user shall acknowledge
*     that the Software was used in producing the information contained
*     in such publication.
*
*  7. The user may incorporate or embed the Software into other software
*     products which he/she may then give away free of charge but not
*     sell provided the user makes due acknowledgement of the use which
*     he/she has made of the Software in creating such software
*     products.  Any redistribution of the Software in this way shall be
*     made under the same terms and conditions under which the user
*     received it from the SOFA Center.
*
*  8. The user shall not cause the Software to be brought into
*     disrepute, either by misuse, or use for inappropriate tasks, or by
*     inappropriate modification.
*
*  9. The Software is provided to the user "as is" and the Board makes
*     no warranty as to its use or performance.   The Board does not and
*     cannot warrant the performance or results which the user may
*     obtain by using the Software.  The Board makes no warranties,
*     express or implied, as to non-infringement of third party rights,
*     merchantability, or fitness for any particular purpose.  In no
*     event will the Board be liable to the user for any consequential,
*     incidental, or special damages, including any lost profits or lost
*     savings, even if a Board representative has been advised of such
*     damages, or for any claim by any third party.
*
*  Correspondence concerning SOFA software should be addressed as
*  follows:
*
*     Internet email: sofa@rl.ac.uk
*     Postal address: IAU SOFA Center
*                     Rutherford Appleton Laboratory
*                     Chilton, Didcot, Oxon OX11 0QX
*                     United Kingdom
*
*
*-----------------------------------------------------------------------

      END
