*=======================================================================
*     Copyright (C) 1999
*     Associated Universities, Inc. Washington DC, USA.
*
*     This library is free software; you can redistribute it and/or
*     modify it under the terms of the GNU Library General Public
*     License as published by the Free Software Foundation; either
*     version 2 of the License, or (at your option) any later version.
*
*     This library is distributed in the hope that it will be useful,
*     but WITHOUT ANY WARRANTY; without even the implied warranty of
*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*     GNU Library General Public License for more details.
*
*     You should have received a copy of the GNU Library General Public
*     License along with this library; if not, write to the Free
*     Software Foundation, Inc., 675 Massachusetts Ave, Cambridge,
*     MA 02139, USA.
*
*     Correspondence concerning AIPS++ should be addressed as follows:
*            Internet email: aips2-request@nrao.edu.
*            Postal address: AIPS++ Project Office
*                            National Radio Astronomy Observatory
*                            520 Edgemont Road
*                            Charlottesville, VA 22903-2475 USA
*
*     $Id$
*-----------------------------------------------------------------------
C
C @(#)lawson.f	1.9 24 Feb 1995
C
C Code from Lawson & Hanson, _Solving Least Squares Problems_
C
C-----------------------------------------------------------------------
C
      SUBROUTINE HFTI (A,MDA,M,N,B,MDB,NB,TAU,KRANK,RNORM,H,G,IP)
C     C.L.LAWSON & R.J.HANSON, JET PROPULSION LABORATORY, 1973 JUN 12
C     TO APPEAR IN 'SOLVING LEAST SQUARES PROBLEMS', PRENTICE-HALL 1974
C     SOLVE LEAST SQUARES PROBLEM USING ALGORITHM HFTI.
C
      DIMENSION A(MDA,N),B(MDB,NB),H(N),G(N),RNORM(NB)
      INTEGER IP(N)
      DOUBLE PRECISION SM,DZERO
      DATA SZERO/ 0./, DZERO / 0.D0 /
      DATA FACTOR / .001 /
C
      K = 0
      LDIAG = MIN0(M,N)
      IF (LDIAG.LE.0) GOTO 270
        DO 80 J=1,LDIAG
          IF (J.EQ.1) GOTO 20
C
C     UPDATE SQUARED COLUMN LENGTHS AND FIND LMAX
C
          LMAX = J
            DO 10 L = J,N
              H(L) = H(L) - A(J-1,L)**2
              IF (H(L).GT.H(LMAX)) LMAX = L
   10       CONTINUE
          IF (DIFF(HMAX+FACTOR*H(LMAX),HMAX)) 20,20,50
C
C     COMPUTE SQUARED COLUMN LENGTHS AND FIND LMAX
C
   20     LMAX = J
            DO 40 L = J,N
              H(L) = SZERO
              DO 30 I = J,M
   30           H(L) = H(L) + A(I,L)**2
              IF (H(L).GT.H(LMAX)) LMAX = L
   40       CONTINUE
          HMAX = H(LMAX)
C
C     LMAX HAS BEEN DETERMINED
C
C     DO COLUMN INTERCHANGES IF NEEDED.
C
   50     CONTINUE
          IP(J) = LMAX
          IF (IP(J).EQ.J) GOTO 70
          DO 60 I = 1,M
            TMP = A(I,J)
            A(I,J) = A(I,LMAX)
   60       A(I,LMAX) = TMP
          H(LMAX) = H(J)
C
C     COMPUTE THE J-TH TRANSFORMATION AND APPLY IT TO A AND B.
C
   70     CALL H12 (1,J,J+1,M,A(1,J),1,H(J),A(1,J+1),1,MDA,N-J)
   80     CALL H12 (2,J,J+1,M,A(1,J),1,H(J),B,1,MDB,NB)
C
C     DETERMINE THE PSEUDORANK, K, USING THE TOLERANCE, TAU.
C
          DO 90 J = 1,LDIAG
            IF (ABS(A(J,J)).LE.TAU) GOTO 100
   90     CONTINUE
      K = LDIAG
      GOTO 110
  100 K = J - 1
  110 KP1 = K + 1
C
C     COMPUTE THE NORMS OF THE RESIDUAL VECTORS.
C
      IF (NB.LE.0) GOTO 140
        DO 130 JB = 1,NB
          TMP = SZERO
          IF (KP1.GT.M) GOTO 130
          DO 120 I = KP1,M
  120       TMP = TMP + B(I,JB)**2
  130     RNORM(JB) = SQRT(TMP)
  140 CONTINUE
C     SPECIAL FOR PSEUDORANK = 0
      IF (K.GT.0) GOTO 160
      IF (NB.LE.0) GOTO 270
        DO 150 JB = 1,NB
          DO 150 I = 1,N
  150       B(I,JB) = SZERO
      GOTO 270
C
C     IF THE PSEUDORANK IS LESS THAN N COMPUTE HOUSHOLDER
C     DECOMPOSITION OF FIRST K ROWS.
C
  160 IF (K.EQ.N) GOTO 180
        DO 170 II = 1,K
          I = KP1 - II
  170     CALL H12 (1,I,KP1,N,A(I,1),MDA,G(I),A,MDA,1,I-1)
  180 CONTINUE
C
C
      IF (NB.LE.0) GOTO 270
        DO 260 JB = 1,NB
C
C     SOLVE THE K BY K TRIANGULAR SYSTEM
C
          DO 210 L = 1,K
            SM = DZERO
            I = KP1 - L
            IF (I.EQ.K) GOTO 200
            IP1 = I + 1
            DO 190 J = IP1,K
  190         SM = SM + A(I,J)*DBLE(B(J,JB))
  200       SM1 = SM
  210       B(I,JB) = (B(I,JB)-SM1)/A(I,I)
C
          IF (K.EQ.N) GOTO 240
            DO 220 J = KP1,N
  220         B(J,JB) = SZERO
            DO 230 I = 1,K
  230         CALL H12 (2,I,KP1,N,A(I,1),MDA,G(I),B(1,JB),1,MDB,1)
C
C     RE-ORDER THE SOLUTION VECTOR TO COMPENSATE FOR THE
C     COLUMN INTERCHANGES.
C
C     COMPLETE COMPUTATION OF SOLUTION VECTOR
C
  240     DO 250 JJ = 1,LDIAG
            J = LDIAG + 1 - JJ
            IF (IP(J).EQ.J) GOTO 250
            L = IP(J)
            TMP = B(L,JB)
            B(L,JB) = B(J,JB)
            B(J,JB) = TMP
  250     CONTINUE
  260   CONTINUE
C
C     THE SOLUTION VECTORS, X, ARE NOW
C     IN THE FIRST N ROWS OF THE ARRAY B(.).
C
  270 KRANK = K
      RETURN
      END
C
      SUBROUTINE LDP (G,MDG,M,N,H,X,XNORM,W,INDEX,MODE)
C     C.L.LAWSON AND R.J.HANSON, JET PROPULSION LABORATORY, 1974 MAR 1
C     TO APPEAR IN @SOLVING LEAST SQUARES PROBLEMS@, PRENTICE-HALL, 1974
C
C            **********  LEAST DISTANCE PROGRAMMING  **********
C
      INTEGER INDEX(1)
      DIMENSION G(MDG,1), H(1), X(1), W(1)
      ZERO=0.
      ONE=1.
      IF (N.LE.0) GO TO 120
          DO 10 J=1,N
   10     X(J)=ZERO
      XNORM=ZERO
      IF (M.LE.0) GO TO 110
C
C     THE DECLARED DIMENSION OF W() MUST BE AT LEAST (N+1)*(M+2)+2*M.
C
C      FIRST (N+1)*M LOCS OF W()   =  MATRIX E FOR PROBLEM NNLS.
C       NEXT     N+1 LOCS OF W()   =  VECTOR F FOR PROBLEM NNLS.
C       NEXT     N+1 LOCS OF W()   =  VECTOR Z FOR PROBLEM NNLS.
C       NEXT       M LOCS OF W()   =  VECTOR Y FOR PROBLEM NNLS.
C       NEXT       M LOCS OF W()   =  VECTOR WDUAL FOR PROBLEM NNLS.
C     COPY G**T INTO FIRST N ROWS AND M COLUMNS OF E.
C     COPY H**T INTO ROW N+1 OF E.
C
      IW=0
          DO 30 J=1,M
              DO 20 I=1,N
              IW=IW+1
   20         W(IW)=G(J,I)
          IW=IW+1
   30     W(IW)=H(J)
      IF=IW+1
C                                STORE N ZEROS FOLLOWED BY A ONE INTO F.
          DO 40 I=1,N
          IW=IW+1
   40     W(IW)=ZERO
      W(IW+1)=ONE
C
      NP1=N+1
      IZ=IW+2
      IY=IZ+NP1
      IWDUAL=IY+M
C
      CALL NNLS (W,NP1,NP1,M,W(IF),W(IY),RNORM,W(IWDUAL),W(IZ),
     $   INDEX,3*NP1,MODE)

c      print *, rnorm, mode
c      write (20,*) (W(IY+I-1), I=1,N+1)

C                      USE THE FOLLOWING RETURN IF UNSUCCESSFUL IN NNLS.
      IF (MODE.NE.1) RETURN
      IF (RNORM) 130,130,50
   50 FAC=ONE
      IW=IY-1
          DO 60 I=1,M
          IW=IW+1
C                               HERE WE ARE USING THE SOLUTION VECTOR Y.
   60     FAC=FAC-H(I)*W(IW)
C
      IF (DIFF(ONE+FAC,ONE)) 130,130,70
   70 FAC=ONE/FAC
          DO 90 J=1,N
          IW=IY-1
              DO 80 I=1,M
              IW=IW+1
C                               HERE WE ARE USING THE SOLUTION VECTOR Y.
   80         X(J)=X(J)+G(I,J)*W(IW)
   90     X(J)=X(J)*FAC
          DO 100 J=1,N
  100     XNORM=XNORM+X(J)**2
      XNORM=SQRT(XNORM)
C                             SUCCESSFUL RETURN.
  110 MODE=1

c      do i = 1, m
c         acc = 0.0
c         do k = 1, n
c            acc = acc + g(i,k) * x(k)
c         end do
c         write (21, *) i, acc, h(i)
c      end do

      RETURN
C                             ERROR RETURN.       N .LE. 0.
  120 MODE=2
      RETURN
C                             RETURNING WITH CONSTRAINTS NOT COMPATIBLE.
  130 MODE=4
      RETURN
      END
C
C
C     SUBROUTINE NNLS  (A,MDA,M,N,B,X,RNORM,W,ZZ,INDEX,ITMAX,MODE)
C     C.L.LAWSON AND R.J.HANSON, JET PROPULSION LABORATORY, 1973 JUNE 15
C     TO APPEAR IN @SOLVING LEAST SQUARES PROBLEMS@, PRENTICE-HALL, 1974
C
C         **********   NONNEGATIVE LEAST SQUARES   **********
C
C     GIVEN AN M BY N MATRIX, A, AND AN M-VECTOR, B,  COMPUTE AN
C     N-VECTOR, X, WHICH SOLVES THE LEAST SQUARES PROBLEM
C
C                      A * X = B  SUBJECT TO X .GE. 0
C
C     A(),MDA,M,N     MDA IS THE FIRST DIMENSIONING PARAMETER FOR THE
C                     ARRAY, A().   ON ENTRY A() CONTAINS THE M BY N
C                     MATRIX, A.           ON EXIT A() CONTAINS
C                     THE PRODUCT MATRIX, Q*A , WHERE Q IS AN
C                     M BY M ORTHOGONAL MATRIX GENERATED IMPLICITLY BY
C                     THIS SUBROUTINE.
C     B()     ON ENTRY B() CONTAINS THE M-VECTOR, B.   ON EXIT B() CON-
C             TAINS Q*B.
C     X()     ON ENTRY X() NEED NOT BE INITIALIZED.  ON EXIT X() WILL
C             CONTAIN THE SOLUTION VECTOR.
C     RNORM   ON EXIT RNORM CONTAINS THE EUCLIDEAN NORM OF THE
C             RESIDUAL VECTOR.
C     W()     AN N-ARRAY OF WORKING SPACE.  ON EXIT W() WILL CONTAIN
C             THE DUAL SOLUTION VECTOR.   W WILL SATISFY W(I) = 0.
C             FOR ALL I IN SET P  AND W(I) .LE. 0. FOR ALL I IN SET Z
C     ZZ()     AN M-ARRAY OF WORKING SPACE.
C     INDEX()     AN INTEGER WORKING ARRAY OF LENGTH AT LEAST N.
C                 ON EXIT THE CONTENTS OF THIS ARRAY DEFINE THE SETS
C                 P AND Z AS FOLLOWS..
C
C                 INDEX(1)   THRU INDEX(NSETP) = SET P.
C                 INDEX(IZ1) THRU INDEX(IZ2)   = SET Z.
C                 IZ1 = NSETP + 1 = NPP1
C                 IZ2 = N
C     ITMAX   MAXIMUM NUMBER OF ITERATIONS  (WAS HARDED CODED AT 3*N IN
C             ORIGINAL LAWSON ROUTINE)
C     MODE    THIS IS A SUCCESS-FAILURE FLAG WITH THE FOLLOWING
C             MEANINGS.
C             1     THE SOLUTION HAS BEEN COMPUTED SUCCESSFULLY.
C             2     THE DIMENSIONS OF THE PROBLEM ARE BAD.
C                   EITHER M .LE. 0 OR N .LE. 0.
C             3    ITERATION COUNT EXCEEDED.  MORE THAN ITMAX ITERATIONS.
C
      SUBROUTINE NNLS (A,MDA,M,N,B,X,RNORM,W,ZZ,INDEX,ITMAX,MODE)
      DIMENSION A(MDA,N), B(1), X(1), W(1), ZZ(1)
      INTEGER INDEX(1)
      REAL      A, B, X, RNORM, W, ZZ, ZERO, ONE, TWO, FACTOR, SM,
     $   WMAX, ASAVE, UP, DUMMY, UNORM, ZTEST, ALPHA, T, CC, SS
      DOUBLE PRECISION DSVCMP, DSVTIM
      INTEGER   MDA, M, N, MODE, ITER, I, J, L, IZ1, IZ2, NSETP, NPP1,
     $   IZ, ITMAX, IZMAX, JZ, NEXT, IP, II, JJ
      REAL      DIFF
      ZERO=0.
      ONE=1.
      TWO=2.
      FACTOR=0.01
      DSVCMP=1.D0
      DSVTIM=1.D0
C
      MODE=1
      IF (M.GT.0.AND.N.GT.0) GO TO 10
      MODE=2
      RETURN
   10 ITER=0
C
C                    INITIALIZE THE ARRAYS INDEX() AND X().
C
          DO 20 I=1,N
          X(I)=ZERO
   20     INDEX(I)=I
C
      IZ2=N
      IZ1=1
      NSETP=0
      NPP1=1
C                             ******  MAIN LOOP BEGINS HERE  ******
   30 CONTINUE
C                  QUIT IF ALL COEFFICIENTS ARE ALREADY IN THE SOLUTION.
C                        OR IF M COLS OF A HAVE BEEN TRIANGULARIZED.
C
      IF (IZ1.GT.IZ2.OR.NSETP.GE.M) GO TO 350
C
C         COMPUTE COMPONENTS OF THE DUAL (NEGATIVE GRADIENT) VECTOR W().
C
          DO 50 IZ=IZ1,IZ2
          J=INDEX(IZ)
          SM=ZERO
              DO 40 L=NPP1,M
   40         SM=SM+A(L,J)*B(L)
   50     W(J)=SM
C                                   FIND LARGEST POSITIVE W(J).
   60 WMAX=ZERO
          DO 70 IZ=IZ1,IZ2
          J=INDEX(IZ)
          IF (W(J).LE.WMAX) GO TO 70
          WMAX=W(J)
          IZMAX=IZ
   70     CONTINUE
C
C             IF WMAX .LE. 0. GO TO TERMINATION.
C             THIS INDICATES SATISFACTION OF THE KUHN-TUCKER CONDITIONS.
C
      IF (WMAX) 350,350,80
   80 IZ=IZMAX
      J=INDEX(IZ)
C
C     THE SIGN OF W(J) IS OK FOR J TO BE MOVED TO SET P.
C     BEGIN THE TRANSFORMATION AND CHECK NEW DIAGONAL ELEMENT TO AVOID
C     NEAR LINEAR DEPENDENCE.
C
      ASAVE=A(NPP1,J)
      CALL H12 (1,NPP1,NPP1+1,M,A(1,J),1,UP,DUMMY,1,1,0)
      UNORM=ZERO
      IF (NSETP.EQ.0) GO TO 100
          DO 90 L=1,NSETP
   90     UNORM=UNORM+A(L,J)**2
  100 UNORM=SQRT(UNORM)
      IF (DIFF(UNORM+ABS(A(NPP1,J))*FACTOR,UNORM)) 130,130,110
C
C     COL J IS SUFFICIENTLY INDEPENDENT.  COPY B INTO ZZ, UPDATE ZZ AND
C   > SOLVE FOR ZTEST ( = PROPOSED NEW VALUE FOR X(J) ).
C
  110     DO 120 L=1,M
  120     ZZ(L)=B(L)
      CALL H12 (2,NPP1,NPP1+1,M,A(1,J),1,UP,ZZ,1,1,1)
      ZTEST=ZZ(NPP1)/A(NPP1,J)
C
C                                     SEE IF ZTEST IS POSITIVE
C     REJECT J AS A CANDIDATE TO BE MOVED FROM SET Z TO SET P.
C     RESTORE A(NPP1,J), SET W(J)=0., AND LOOP BACK TO TEST DUAL
C
      IF (ZTEST) 130,130,140
C
C     COEFFS AGAIN.
C
  130 A(NPP1,J)=ASAVE
      W(J)=ZERO
      GO TO 60
C
C     THE INDEX  J=INDEX(IZ)  HAS BEEN SELECTED TO BE MOVED FROM
C     SET Z TO SET P.    UPDATE B,  UPDATE INDICES,  APPLY HOUSEHOLDER
C     TRANSFORMATIONS TO COLS IN NEW SET Z,  ZERO SUBDIAGONAL ELTS IN
C     COL J,  SET W(J)=0.
C
  140     DO 150 L=1,M
  150     B(L)=ZZ(L)
C
      INDEX(IZ)=INDEX(IZ1)
      INDEX(IZ1)=J
      IZ1=IZ1+1
      NSETP=NPP1
      NPP1=NPP1+1
C
      IF (IZ1.GT.IZ2) GO TO 170
          DO 160 JZ=IZ1,IZ2
          JJ=INDEX(JZ)
  160     CALL H12 (2,NSETP,NPP1,M,A(1,J),1,UP,A(1,JJ),1,MDA,1)
  170 CONTINUE
C
      IF (NSETP.EQ.M) GO TO 190
          DO 180 L=NPP1,M
  180     A(L,J)=ZERO
  190 CONTINUE
C
      W(J)=ZERO
C                                SOLVE THE TRIANGULAR SYSTEM.
C                                STORE THE SOLUTION TEMPORARILY IN ZZ().
      ASSIGN 200 TO NEXT
      GO TO 400
  200 CONTINUE
C
C                       ******  SECONDARY LOOP BEGINS HERE ******
C
C                          ITERATION COUNTER.
C
  210 ITER=ITER+1
      IF (ITER.LE.ITMAX) GO TO 220
      MODE=3
      ITER = ITER - 1
      GO TO 350
  220 CONTINUE
C
C                    SEE IF ALL NEW CONSTRAINED COEFFS ARE FEASIBLE.
C                                  IF NOT COMPUTE ALPHA.
C
      ALPHA=TWO
          DO 240 IP=1,NSETP
          L=INDEX(IP)
          IF (ZZ(IP)) 230,230,240
C
  230     T=-X(L)/(ZZ(IP)-X(L))
          IF (ALPHA.LE.T) GO TO 240
          ALPHA=T
          JJ=IP
  240     CONTINUE
C
C          IF ALL NEW CONSTRAINED COEFFS ARE FEASIBLE THEN ALPHA WILL
C          STILL = 2.    IF SO EXIT FROM SECONDARY LOOP TO MAIN LOOP.
C
      IF (ALPHA.EQ.TWO) GO TO 330
C
C          OTHERWISE USE ALPHA WHICH WILL BE BETWEEN 0. AND 1. TO
C          INTERPOLATE BETWEEN THE OLD X AND THE NEW ZZ.
C
          DO 250 IP=1,NSETP
          L=INDEX(IP)
  250     X(L)=X(L)+ALPHA*(ZZ(IP)-X(L))
C
C        MODIFY A AND B AND THE INDEX ARRAYS TO MOVE COEFFICIENT I
C        FROM SET P TO SET Z.
C
      I=INDEX(JJ)
  260 X(I)=ZERO
C
      IF (JJ.EQ.NSETP) GO TO 290
      JJ=JJ+1
          DO 280 J=JJ,NSETP
          II=INDEX(J)
          INDEX(J-1)=II
          CALL G1 (A(J-1,II),A(J,II),CC,SS,A(J-1,II))
          A(J,II)=ZERO
              DO 270 L=1,N
              IF (L.NE.II) CALL G2 (CC,SS,A(J-1,L),A(J,L))
  270         CONTINUE
  280     CALL G2 (CC,SS,B(J-1),B(J))
  290 NPP1=NSETP
      NSETP=NSETP-1
      IZ1=IZ1-1
      INDEX(IZ1)=I
C
C        SEE IF THE REMAINING COEFFS IN SET P ARE FEASIBLE.  THEY SHOULD
C        BE BECAUSE OF THE WAY ALPHA WAS DETERMINED.
C        IF ANY ARE INFEASIBLE IT IS DUE TO ROUND-OFF ERROR.  ANY
C        THAT ARE NONPOSITIVE WILL BE SET TO ZERO
C        AND MOVED FROM SET P TO SET Z.
C
          DO 300 JJ=1,NSETP
          I=INDEX(JJ)
          IF (X(I)) 260,260,300
  300     CONTINUE
C
C         COPY B( ) INTO ZZ( ).  THEN SOLVE AGAIN AND LOOP BACK.
C

          DO 310 I=1,M
  310     ZZ(I)=B(I)
      ASSIGN 320 TO NEXT
      GO TO 400
  320 CONTINUE
      GO TO 210
C                      ******  END OF SECONDARY LOOP  ******
C
  330     DO 340 IP=1,NSETP
          I=INDEX(IP)
  340     X(I)=ZZ(IP)
C        ALL NEW COEFFS ARE POSITIVE.  LOOP BACK TO BEGINNING.
      GO TO 30
C
C                        ******  END OF MAIN LOOP  ******
C
C                        COME TO HERE FOR TERMINATION.
C                     COMPUTE THE NORM OF THE FINAL RESIDUAL VECTOR.
C
  350 SM=ZERO
      IF (NPP1.GT.M) GO TO 370
          DO 360 I=NPP1,M
  360     SM=SM+B(I)**2
      GO TO 390
  370     DO 380 J=1,N
  380     W(J)=ZERO
  390 RNORM=SQRT(SM)
      RETURN
C
C     THE FOLLOWING BLOCK OF CODE IS USED AS AN INTERNAL SUBROUTINE
C     TO SOLVE THE TRIANGULAR SYSTEM, PUTTING THE SOLUTION IN ZZ().
C
  400     DO 430 L=1,NSETP
          IP=NSETP+1-L
          IF (L.EQ.1) GO TO 420
              DO 410 II=1,IP
  410         ZZ(II)=ZZ(II)-A(II,JJ)*ZZ(IP+1)
  420     JJ=INDEX(IP)
  430     ZZ(IP)=ZZ(IP)/A(IP,JJ)
      GO TO NEXT, (200,320)
  440 FORMAT (35H0 NNLS QUITTING ON ITERATION COUNT.)
      END
C
C     SUBROUTINE H12 (MODE,LPIVOT,L1,M,U,IUE,UP,C,ICE,ICV,NCV)
C     C.L.LAWSON AND R.J.HANSON, JET PROPULSION LABORATORY, 1973 JUN 12
C     TO APPEAR IN @SOLVING LEAST SQUARES PROBLEMS@, PRENTICE-HALL, 1974
C
C     CONSTRUCTION AND/OR APPLICATION OF A SINGLE
C     HOUSEHOLDER TRANSFORMATION..     Q = I + U*(U**T)/B
C
C     MODE    = 1 OR 2   TO SELECT ALGORITHM  H1  OR  H2 .
C     LPIVOT IS THE INDEX OF THE PIVOT ELEMENT.
C     L1,M   IF L1 .LE. M   THE TRANSFORMATION WILL BE CONSTRUCTED TO
C            ZERO ELEMENTS INDEXED FROM L1 THROUGH M.   IF L1 GT. M
C            THE SUBROUTINE DOES AN IDENTITY TRANSFORMATION.
C     U(),IUE,UP    ON ENTRY TO H1 U() CONTAINS THE PIVOT VECTOR.
C                   IUE IS THE STORAGE INCREMENT BETWEEN ELEMENTS.
C                                       ON EXIT FROM H1 U() AND UP
C                   CONTAIN QUANTITIES DEFINING THE VECTOR U OF THE
C                   HOUSEHOLDER TRANSFORMATION.   ON ENTRY TO H2 U()
C                   AND UP SHOULD CONTAIN QUANTITIES PREVIOUSLY COMPUTED
C                   BY H1.  THESE WILL NOT BE MODIFIED BY H2.
C     C()    ON ENTRY TO H1 OR H2 C() CONTAINS A MATRIX WHICH WILL BE
C            REGARDED AS A SET OF VECTORS TO WHICH THE HOUSEHOLDER
C            TRANSFORMATION IS TO BE APPLIED.  ON EXIT C() CONTAINS THE
C            SET OF TRANSFORMED VECTORS.
C     ICE    STORAGE INCREMENT BETWEEN ELEMENTS OF VECTORS IN C().
C     ICV    STORAGE INCREMENT BETWEEN VECTORS IN C().
C     NCV    NUMBER OF VECTORS IN C() TO BE TRANSFORMED. IF NCV .LE. 0
C            NO OPERATIONS WILL BE DONE ON C().
C
      SUBROUTINE H12 (MODE,LPIVOT,L1,M,U,IUE,UP,C,ICE,ICV,NCV)
      DIMENSION U(IUE,1), C(1)
      DOUBLE PRECISION SM,B
      ONE=1.
C
      IF (0.GE.LPIVOT.OR.LPIVOT.GE.L1.OR.L1.GT.M) RETURN
      CL=ABS(U(1,LPIVOT))
      IF (MODE.EQ.2) GO TO 60
C                            ****** CONSTRUCT THE TRANSFORMATION. ******
          DO 10 J=L1,M
   10     CL=AMAX1(ABS(U(1,J)),CL)
      IF (CL) 130,130,20
   20 CLINV=ONE/CL
      SM=(DBLE(U(1,LPIVOT))*CLINV)**2
          DO 30 J=L1,M
   30     SM=SM+(DBLE(U(1,J))*CLINV)**2
C                              CONVERT DBLE. PREC. SM TO SNGL. PREC. SM1
      SM1=SM
      CL=CL*SQRT(SM1)
      IF (U(1,LPIVOT)) 50,50,40
   40 CL=-CL
   50 UP=U(1,LPIVOT)-CL
      U(1,LPIVOT)=CL
      GO TO 70
C            ****** APPLY THE TRANSFORMATION  I+U*(U**T)/B  TO C. ******
C
   60 IF (CL) 130,130,70
   70 IF (NCV.LE.0) RETURN
      B=DBLE(UP)*U(1,LPIVOT)
C                       B  MUST BE NONPOSITIVE HERE.  IF B = 0., RETURN.
C
      IF (B) 80,130,130
   80 B=ONE/B
      I2=1-ICV+ICE*(LPIVOT-1)
      INCR=ICE*(L1-LPIVOT)
          DO 120 J=1,NCV
          I2=I2+ICV
          I3=I2+INCR
          I4=I3
          SM=C(I2)*DBLE(UP)
              DO 90 I=L1,M
              SM=SM+C(I3)*DBLE(U(1,I))
   90         I3=I3+ICE
          IF (SM) 100,120,100
  100     SM=SM*B
          C(I2)=C(I2)+SM*DBLE(UP)
              DO 110 I=L1,M
              C(I4)=C(I4)+SM*DBLE(U(1,I))
  110         I4=I4+ICE
  120     CONTINUE
  130 RETURN
      END
C
      SUBROUTINE G1 (A,B,COS,SIN,SIG)
C     C.L.LAWSON AND R.J.HANSON, JET PROPULSION LABORATORY, 1973 JUN 12
C     TO APPEAR IN @SOLVING LEAST SQUARES PROBLEMS@, PRENTICE-HALL, 1974
C
C
C     COMPUTE ORTHOGONAL ROTATION MATRIX..
C     COMPUTE.. MATRIX   (C, S) SO THAT (C, S)(A) = (SQRT(A**2+B**2))
C                        (-S,C)         (-S,C)(B)   (   0          )
C     COMPUTE SIG = SQRT(A**2+B**2)
C        SIG IS COMPUTED LAST TO ALLOW FOR THE POSSIBILITY THAT
C        SIG MAY BE IN THE SAME LOCATION AS A OR B .
C
      ZERO=0.
      ONE=1.
      IF (ABS(A).LE.ABS(B)) GO TO 10
      XR=B/A
      YR=SQRT(ONE+XR**2)
      COS=SIGN(ONE/YR,A)
      SIN=COS*XR
      SIG=ABS(A)*YR
      RETURN
   10 IF (B) 20,30,20
   20 XR=A/B
      YR=SQRT(ONE+XR**2)
      SIN=SIGN(ONE/YR,B)
      COS=SIN*XR
      SIG=ABS(B)*YR
      RETURN
   30 SIG=ZERO
      COS=ZERO
      SIN=ONE
      RETURN
      END
C
      SUBROUTINE G2    (COS,SIN,X,Y)
C     C.L.LAWSON AND R.J.HANSON, JET PROPULSION LABORATORY, 1972 DEC 15
C     TO APPEAR IN @SOLVING LEAST SQUARES PROBLEMS@, PRENTICE-HALL, 1974
C          APPLY THE ROTATION COMPUTED BY G1 TO (X,Y).
      XR=COS*X+SIN*Y
      Y=-SIN*X+COS*Y
      X=XR
      RETURN
      END
C
      FUNCTION DIFF(X,Y)
C     C.L.LAWSON AND R.J.HANSON, JET PROPULSION LABORATORY, 1973 JUNE 7
C     TO APPEAR IN @SOLVING LEAST SQUARES PROBLEMS@, PRENTICE-HALL, 1974
      DIFF=X-Y
      RETURN
      END
C
C Double precision versions
C
      SUBROUTINE DLDP (G,MDG,M,N,H,X,XNORM,W,INDEX,MODE)
C     C.L.LAWSON AND R.J.HANSON, JET PROPULSION LABORATORY, 1974 MAR 1
C     TO APPEAR IN @SOLVING LEAST SQUARES PROBLEMS@, PRENTICE-HALL, 1974
C
C            **********  LEAST DISTANCE PROGRAMMING  **********
C
      implicit double precision (a-h,o-z)
      INTEGER INDEX(1)
      DIMENSION G(MDG,1), H(1), X(1), W(1)
      ZERO=0.D0
      ONE=1.D0
      IF (N.LE.0) GO TO 120
          DO 10 J=1,N
   10     X(J)=ZERO
      XNORM=ZERO
      IF (M.LE.0) GO TO 110
C
C     THE DECLARED DIMENSION OF W() MUST BE AT LEAST (N+1)*(M+2)+2*M.
C
C      FIRST (N+1)*M LOCS OF W()   =  MATRIX E FOR PROBLEM NNLS.
C       NEXT     N+1 LOCS OF W()   =  VECTOR F FOR PROBLEM NNLS.
C       NEXT     N+1 LOCS OF W()   =  VECTOR Z FOR PROBLEM NNLS.
C       NEXT       M LOCS OF W()   =  VECTOR Y FOR PROBLEM NNLS.
C       NEXT       M LOCS OF W()   =  VECTOR WDUAL FOR PROBLEM NNLS.
C     COPY G**T INTO FIRST N ROWS AND M COLUMNS OF E.
C     COPY H**T INTO ROW N+1 OF E.
C
      IW=0
          DO 30 J=1,M
              DO 20 I=1,N
              IW=IW+1
   20         W(IW)=G(J,I)
          IW=IW+1
   30     W(IW)=H(J)
      IF=IW+1
C                                STORE N ZEROS FOLLOWED BY A ONE INTO F.
          DO 40 I=1,N
          IW=IW+1
   40     W(IW)=ZERO
      W(IW+1)=ONE
C
      NP1=N+1
      IZ=IW+2
      IY=IZ+NP1
      IWDUAL=IY+M
C
      CALL DNNLS (W,NP1,NP1,M,W(IF),W(IY),RNORM,W(IWDUAL),W(IZ),
     $   INDEX, 3*NP1, MODE)

c      print *, rnorm, mode
c      write (20,*) (W(IY+I-1), I=1,N+1)

C                      USE THE FOLLOWING RETURN IF UNSUCCESSFUL IN NNLS.
      IF (MODE.NE.1) RETURN
      IF (RNORM) 130,130,50
   50 FAC=ONE
      IW=IY-1
          DO 60 I=1,M
          IW=IW+1
C                               HERE WE ARE USING THE SOLUTION VECTOR Y.
   60     FAC=FAC-H(I)*W(IW)
C
      IF (DDIFF(ONE+FAC,ONE)) 130,130,70
   70 FAC=ONE/FAC
          DO 90 J=1,N
          IW=IY-1
              DO 80 I=1,M
              IW=IW+1
C                               HERE WE ARE USING THE SOLUTION VECTOR Y.
   80         X(J)=X(J)+G(I,J)*W(IW)
   90     X(J)=X(J)*FAC
          DO 100 J=1,N
  100     XNORM=XNORM+X(J)**2
      XNORM=SQRT(XNORM)
C                             SUCCESSFUL RETURN.
  110 MODE=1

c      do i = 1, m
c         acc = 0.0
c         do k = 1, n
c            acc = acc + g(i,k) * x(k)
c         end do
c         write (21, *) i, acc, h(i)
c      end do

      RETURN
C                             ERROR RETURN.       N .LE. 0.
  120 MODE=2
      RETURN
C                             RETURNING WITH CONSTRAINTS NOT COMPATIBLE.
  130 MODE=4
      RETURN
      END
C
C
C     SUBROUTINE DNNLS  (A,MDA,M,N,B,X,RNORM,W,ZZ,INDEX,ITMAX,MODE)
C     C.L.LAWSON AND R.J.HANSON, JET PROPULSION LABORATORY, 1973 JUNE 15
C     TO APPEAR IN @SOLVING LEAST SQUARES PROBLEMS@, PRENTICE-HALL, 1974
C
C         **********   NONNEGATIVE LEAST SQUARES   **********
C
C     GIVEN AN M BY N MATRIX, A, AND AN M-VECTOR, B,  COMPUTE AN
C     N-VECTOR, X, WHICH SOLVES THE LEAST SQUARES PROBLEM
C
C                      A * X = B  SUBJECT TO X .GE. 0
C
C     A(),MDA,M,N     MDA IS THE FIRST DIMENSIONING PARAMETER FOR THE
C                     ARRAY, A().   ON ENTRY A() CONTAINS THE M BY N
C                     MATRIX, A.           ON EXIT A() CONTAINS
C                     THE PRODUCT MATRIX, Q*A , WHERE Q IS AN
C                     M BY M ORTHOGONAL MATRIX GENERATED IMPLICITLY BY
C                     THIS SUBROUTINE.
C     B()     ON ENTRY B() CONTAINS THE M-VECTOR, B.   ON EXIT B() CON-
C             TAINS Q*B.
C     X()     ON ENTRY X() NEED NOT BE INITIALIZED.  ON EXIT X() WILL
C             CONTAIN THE SOLUTION VECTOR.
C     RNORM   ON EXIT RNORM CONTAINS THE EUCLIDEAN NORM OF THE
C             RESIDUAL VECTOR.
C     W()     AN N-ARRAY OF WORKING SPACE.  ON EXIT W() WILL CONTAIN
C             THE DUAL SOLUTION VECTOR.   W WILL SATISFY W(I) = 0.
C             FOR ALL I IN SET P  AND W(I) .LE. 0. FOR ALL I IN SET Z
C     ZZ()     AN M-ARRAY OF WORKING SPACE.
C     INDEX()     AN INTEGER WORKING ARRAY OF LENGTH AT LEAST N.
C                 ON EXIT THE CONTENTS OF THIS ARRAY DEFINE THE SETS
C                 P AND Z AS FOLLOWS..
C
C                 INDEX(1)   THRU INDEX(NSETP) = SET P.
C                 INDEX(IZ1) THRU INDEX(IZ2)   = SET Z.
C                 IZ1 = NSETP + 1 = NPP1
C                 IZ2 = N
C     ITMAX   MAXIMUM NUMBER OF ITERATIONS  (WAS HARDED CODED AT 3*N IN
C             ORIGINAL LAWSON ROUTINE)
C     MODE    THIS IS A SUCCESS-FAILURE FLAG WITH THE FOLLOWING
C             MEANINGS.
C             1     THE SOLUTION HAS BEEN COMPUTED SUCCESSFULLY.
C             2     THE DIMENSIONS OF THE PROBLEM ARE BAD.
C                   EITHER M .LE. 0 OR N .LE. 0.
C             3    ITERATION COUNT EXCEEDED.  MORE THAN ITMAX ITERATIONS.
C
      SUBROUTINE DNNLS (A,MDA,M,N,B,X,RNORM,W,ZZ,INDEX,ITMAX,MODE)
      implicit double precision (a-h,o-z)
      DIMENSION A(MDA,N), B(1), X(1), W(1), ZZ(1)
      INTEGER INDEX(1)
      ZERO=0.D0
      ONE=1.D0
      TWO=2.D0
      FACTOR=0.01D0
C
      MODE=1
      IF (M.GT.0.AND.N.GT.0) GO TO 10
      MODE=2
      RETURN
   10 ITER=0
C
C                    INITIALIZE THE ARRAYS INDEX() AND X().
C
          DO 20 I=1,N
          X(I)=ZERO
   20     INDEX(I)=I
C
      IZ2=N
      IZ1=1
      NSETP=0
      NPP1=1
C                             ******  MAIN LOOP BEGINS HERE  ******
   30 CONTINUE
C                  QUIT IF ALL COEFFICIENTS ARE ALREADY IN THE SOLUTION.
C                        OR IF M COLS OF A HAVE BEEN TRIANGULARIZED.
C
      IF (IZ1.GT.IZ2.OR.NSETP.GE.M) GO TO 350
C
C         COMPUTE COMPONENTS OF THE DUAL (NEGATIVE GRADIENT) VECTOR W().
C
          DO 50 IZ=IZ1,IZ2
          J=INDEX(IZ)
          SM=ZERO
              DO 40 L=NPP1,M
   40         SM=SM+A(L,J)*B(L)
   50     W(J)=SM
C                                   FIND LARGEST POSITIVE W(J).
   60 WMAX=ZERO
          DO 70 IZ=IZ1,IZ2
          J=INDEX(IZ)
          IF (W(J).LE.WMAX) GO TO 70
          WMAX=W(J)
          IZMAX=IZ
   70     CONTINUE
C
C             IF WMAX .LE. 0. GO TO TERMINATION.
C             THIS INDICATES SATISFACTION OF THE KUHN-TUCKER CONDITIONS.
C
      IF (WMAX) 350,350,80
   80 IZ=IZMAX
      J=INDEX(IZ)
C
C     THE SIGN OF W(J) IS OK FOR J TO BE MOVED TO SET P.
C     BEGIN THE TRANSFORMATION AND CHECK NEW DIAGONAL ELEMENT TO AVOID
C     NEAR LINEAR DEPENDENCE.
C
      ASAVE=A(NPP1,J)
      CALL DH12 (1,NPP1,NPP1+1,M,A(1,J),1,UP,DUMMY,1,1,0)
      UNORM=ZERO
      IF (NSETP.EQ.0) GO TO 100
          DO 90 L=1,NSETP
   90     UNORM=UNORM+A(L,J)**2
  100 UNORM=SQRT(UNORM)
      IF (DDIFF(UNORM+ABS(A(NPP1,J))*FACTOR,UNORM)) 130,130,110
C
C     COL J IS SUFFICIENTLY INDEPENDENT.  COPY B INTO ZZ, UPDATE ZZ AND
C   > SOLVE FOR ZTEST ( = PROPOSED NEW VALUE FOR X(J) ).
C
  110     DO 120 L=1,M
  120     ZZ(L)=B(L)
      CALL DH12 (2,NPP1,NPP1+1,M,A(1,J),1,UP,ZZ,1,1,1)
      ZTEST=ZZ(NPP1)/A(NPP1,J)
C
C                                     SEE IF ZTEST IS POSITIVE
C     REJECT J AS A CANDIDATE TO BE MOVED FROM SET Z TO SET P.
C     RESTORE A(NPP1,J), SET W(J)=0., AND LOOP BACK TO TEST DUAL
C
      IF (ZTEST) 130,130,140
C
C     COEFFS AGAIN.
C
  130 A(NPP1,J)=ASAVE
      W(J)=ZERO
      GO TO 60
C
C     THE INDEX  J=INDEX(IZ)  HAS BEEN SELECTED TO BE MOVED FROM
C     SET Z TO SET P.    UPDATE B,  UPDATE INDICES,  APPLY HOUSEHOLDER
C     TRANSFORMATIONS TO COLS IN NEW SET Z,  ZERO SUBDIAGONAL ELTS IN
C     COL J,  SET W(J)=0.
C
  140     DO 150 L=1,M
  150     B(L)=ZZ(L)
C
      INDEX(IZ)=INDEX(IZ1)
      INDEX(IZ1)=J
      IZ1=IZ1+1
      NSETP=NPP1
      NPP1=NPP1+1
C
      IF (IZ1.GT.IZ2) GO TO 170
          DO 160 JZ=IZ1,IZ2
          JJ=INDEX(JZ)
  160     CALL DH12 (2,NSETP,NPP1,M,A(1,J),1,UP,A(1,JJ),1,MDA,1)
  170 CONTINUE
C
      IF (NSETP.EQ.M) GO TO 190
          DO 180 L=NPP1,M
  180     A(L,J)=ZERO
  190 CONTINUE
C
      W(J)=ZERO
C                                SOLVE THE TRIANGULAR SYSTEM.
C                                STORE THE SOLUTION TEMPORARILY IN ZZ().
      ASSIGN 200 TO NEXT
      GO TO 400
  200 CONTINUE
C
C                       ******  SECONDARY LOOP BEGINS HERE ******
C
C                          ITERATION COUNTER.
C
  210 ITER=ITER+1
      IF (ITER.LE.ITMAX) GO TO 220
      MODE=3
C Return number of iterations actually completed
      ITER = ITER - 1
      GO TO 350
  220 CONTINUE
C
C                    SEE IF ALL NEW CONSTRAINED COEFFS ARE FEASIBLE.
C                                  IF NOT COMPUTE ALPHA.
C
      ALPHA=TWO
          DO 240 IP=1,NSETP
          L=INDEX(IP)
          IF (ZZ(IP)) 230,230,240
C
  230     T=-X(L)/(ZZ(IP)-X(L))
          IF (ALPHA.LE.T) GO TO 240
          ALPHA=T
          JJ=IP
  240     CONTINUE
C
C          IF ALL NEW CONSTRAINED COEFFS ARE FEASIBLE THEN ALPHA WILL
C          STILL = 2.    IF SO EXIT FROM SECONDARY LOOP TO MAIN LOOP.
C
      IF (ALPHA.EQ.TWO) GO TO 330
C
C          OTHERWISE USE ALPHA WHICH WILL BE BETWEEN 0. AND 1. TO
C          INTERPOLATE BETWEEN THE OLD X AND THE NEW ZZ.
C
          DO 250 IP=1,NSETP
          L=INDEX(IP)
  250     X(L)=X(L)+ALPHA*(ZZ(IP)-X(L))
C
C        MODIFY A AND B AND THE INDEX ARRAYS TO MOVE COEFFICIENT I
C        FROM SET P TO SET Z.
C
      I=INDEX(JJ)
  260 X(I)=ZERO
C
      IF (JJ.EQ.NSETP) GO TO 290
      JJ=JJ+1
          DO 280 J=JJ,NSETP
          II=INDEX(J)
          INDEX(J-1)=II
          CALL DG1 (A(J-1,II),A(J,II),CC,SS,A(J-1,II))
          A(J,II)=ZERO
              DO 270 L=1,N
              IF (L.NE.II) CALL DG2 (CC,SS,A(J-1,L),A(J,L))
  270         CONTINUE
  280     CALL DG2 (CC,SS,B(J-1),B(J))
  290 NPP1=NSETP
      NSETP=NSETP-1
      IZ1=IZ1-1
      INDEX(IZ1)=I
C
C        SEE IF THE REMAINING COEFFS IN SET P ARE FEASIBLE.  THEY SHOULD
C        BE BECAUSE OF THE WAY ALPHA WAS DETERMINED.
C        IF ANY ARE INFEASIBLE IT IS DUE TO ROUND-OFF ERROR.  ANY
C        THAT ARE NONPOSITIVE WILL BE SET TO ZERO
C        AND MOVED FROM SET P TO SET Z.
C
          DO 300 JJ=1,NSETP
          I=INDEX(JJ)
          IF (X(I)) 260,260,300
  300     CONTINUE
C
C         COPY B( ) INTO ZZ( ).  THEN SOLVE AGAIN AND LOOP BACK.
C

          DO 310 I=1,M
  310     ZZ(I)=B(I)
      ASSIGN 320 TO NEXT
      GO TO 400
  320 CONTINUE
      GO TO 210
C                      ******  END OF SECONDARY LOOP  ******
C
  330     DO 340 IP=1,NSETP
          I=INDEX(IP)
  340     X(I)=ZZ(IP)
C        ALL NEW COEFFS ARE POSITIVE.  LOOP BACK TO BEGINNING.
      GO TO 30
C
C                        ******  END OF MAIN LOOP  ******
C
C                        COME TO HERE FOR TERMINATION.
C                     COMPUTE THE NORM OF THE FINAL RESIDUAL VECTOR.
C
  350 SM=ZERO
      IF (NPP1.GT.M) GO TO 370
          DO 360 I=NPP1,M
  360     SM=SM+B(I)**2
      GO TO 390
  370     DO 380 J=1,N
  380     W(J)=ZERO
  390 RNORM=SQRT(SM)
      RETURN
C
C     THE FOLLOWING BLOCK OF CODE IS USED AS AN INTERNAL SUBROUTINE
C     TO SOLVE THE TRIANGULAR SYSTEM, PUTTING THE SOLUTION IN ZZ().
C
  400     DO 430 L=1,NSETP
          IP=NSETP+1-L
          IF (L.EQ.1) GO TO 420
              DO 410 II=1,IP
  410         ZZ(II)=ZZ(II)-A(II,JJ)*ZZ(IP+1)
  420     JJ=INDEX(IP)
  430     ZZ(IP)=ZZ(IP)/A(IP,JJ)
      GO TO NEXT, (200,320)
  440 FORMAT (35H0 NNLS QUITTING ON ITERATION COUNT.)
      END
C
C     SUBROUTINE DH12 (MODE,LPIVOT,L1,M,U,IUE,UP,C,ICE,ICV,NCV)
C     C.L.LAWSON AND R.J.HANSON, JET PROPULSION LABORATORY, 1973 JUN 12
C     TO APPEAR IN @SOLVING LEAST SQUARES PROBLEMS@, PRENTICE-HALL, 1974
C
C     CONSTRUCTION AND/OR APPLICATION OF A SINGLE
C     HOUSEHOLDER TRANSFORMATION..     Q = I + U*(U**T)/B
C
C     MODE    = 1 OR 2   TO SELECT ALGORITHM  H1  OR  H2 .
C     LPIVOT IS THE INDEX OF THE PIVOT ELEMENT.
C     L1,M   IF L1 .LE. M   THE TRANSFORMATION WILL BE CONSTRUCTED TO
C            ZERO ELEMENTS INDEXED FROM L1 THROUGH M.   IF L1 GT. M
C            THE SUBROUTINE DOES AN IDENTITY TRANSFORMATION.
C     U(),IUE,UP    ON ENTRY TO H1 U() CONTAINS THE PIVOT VECTOR.
C                   IUE IS THE STORAGE INCREMENT BETWEEN ELEMENTS.
C                                       ON EXIT FROM H1 U() AND UP
C                   CONTAIN QUANTITIES DEFINING THE VECTOR U OF THE
C                   HOUSEHOLDER TRANSFORMATION.   ON ENTRY TO H2 U()
C                   AND UP SHOULD CONTAIN QUANTITIES PREVIOUSLY COMPUTED
C                   BY H1.  THESE WILL NOT BE MODIFIED BY H2.
C     C()    ON ENTRY TO H1 OR H2 C() CONTAINS A MATRIX WHICH WILL BE
C            REGARDED AS A SET OF VECTORS TO WHICH THE HOUSEHOLDER
C            TRANSFORMATION IS TO BE APPLIED.  ON EXIT C() CONTAINS THE
C            SET OF TRANSFORMED VECTORS.
C     ICE    STORAGE INCREMENT BETWEEN ELEMENTS OF VECTORS IN C().
C     ICV    STORAGE INCREMENT BETWEEN VECTORS IN C().
C     NCV    NUMBER OF VECTORS IN C() TO BE TRANSFORMED. IF NCV .LE. 0
C            NO OPERATIONS WILL BE DONE ON C().
C
      SUBROUTINE DH12 (MODE,LPIVOT,L1,M,U,IUE,UP,C,ICE,ICV,NCV)
      implicit double precision (a-h,o-z)
      DIMENSION U(IUE,1), C(1)
      DOUBLE PRECISION SM,B
      ONE=1.D0
C
      IF (0.GE.LPIVOT.OR.LPIVOT.GE.L1.OR.L1.GT.M) RETURN
      CL=ABS(U(1,LPIVOT))
      IF (MODE.EQ.2) GO TO 60
C                            ****** CONSTRUCT THE TRANSFORMATION. ******
          DO 10 J=L1,M
   10     CL=DMAX1(ABS(U(1,J)),CL)
      IF (CL) 130,130,20
   20 CLINV=ONE/CL
      SM=(DBLE(U(1,LPIVOT))*CLINV)**2
          DO 30 J=L1,M
   30     SM=SM+(DBLE(U(1,J))*CLINV)**2
C                              CONVERT DBLE. PREC. SM TO SNGL. PREC. SM1
      SM1=SM
      CL=CL*SQRT(SM1)
      IF (U(1,LPIVOT)) 50,50,40
   40 CL=-CL
   50 UP=U(1,LPIVOT)-CL
      U(1,LPIVOT)=CL
      GO TO 70
C            ****** APPLY THE TRANSFORMATION  I+U*(U**T)/B  TO C. ******
C
   60 IF (CL) 130,130,70
   70 IF (NCV.LE.0) RETURN
      B=DBLE(UP)*U(1,LPIVOT)
C                       B  MUST BE NONPOSITIVE HERE.  IF B = 0., RETURN.
C
      IF (B) 80,130,130
   80 B=ONE/B
      I2=1-ICV+ICE*(LPIVOT-1)
      INCR=ICE*(L1-LPIVOT)
          DO 120 J=1,NCV
          I2=I2+ICV
          I3=I2+INCR
          I4=I3
          SM=C(I2)*DBLE(UP)
              DO 90 I=L1,M
              SM=SM+C(I3)*DBLE(U(1,I))
   90         I3=I3+ICE
          IF (SM) 100,120,100
  100     SM=SM*B
          C(I2)=C(I2)+SM*DBLE(UP)
              DO 110 I=L1,M
              C(I4)=C(I4)+SM*DBLE(U(1,I))
  110         I4=I4+ICE
  120     CONTINUE
  130 RETURN
      END
C
      SUBROUTINE DG1 (A,B,COS,SIN,SIG)
      implicit double precision (a-h,o-z)
C     C.L.LAWSON AND R.J.HANSON, JET PROPULSION LABORATORY, 1973 JUN 12
C     TO APPEAR IN @SOLVING LEAST SQUARES PROBLEMS@, PRENTICE-HALL, 1974
C
C
C     COMPUTE ORTHOGONAL ROTATION MATRIX..
C     COMPUTE.. MATRIX   (C, S) SO THAT (C, S)(A) = (SQRT(A**2+B**2))
C                        (-S,C)         (-S,C)(B)   (   0          )
C     COMPUTE SIG = SQRT(A**2+B**2)
C        SIG IS COMPUTED LAST TO ALLOW FOR THE POSSIBILITY THAT
C        SIG MAY BE IN THE SAME LOCATION AS A OR B .
C
      ZERO=0.D0
      ONE=1.D0
      IF (ABS(A).LE.ABS(B)) GO TO 10
      XR=B/A
      YR=SQRT(ONE+XR**2)
      COS=SIGN(ONE/YR,A)
      SIN=COS*XR
      SIG=ABS(A)*YR
      RETURN
   10 IF (B) 20,30,20
   20 XR=A/B
      YR=SQRT(ONE+XR**2)
      SIN=SIGN(ONE/YR,B)
      COS=SIN*XR
      SIG=ABS(B)*YR
      RETURN
   30 SIG=ZERO
      COS=ZERO
      SIN=ONE
      RETURN
      END
C
      SUBROUTINE DG2    (COS,SIN,X,Y)
      implicit double precision (a-h,o-z)
C     C.L.LAWSON AND R.J.HANSON, JET PROPULSION LABORATORY, 1972 DEC 15
C     TO APPEAR IN @SOLVING LEAST SQUARES PROBLEMS@, PRENTICE-HALL, 1974
C          APPLY THE ROTATION COMPUTED BY G1 TO (X,Y).
      XR=COS*X+SIN*Y
      Y=-SIN*X+COS*Y
      X=XR
      RETURN
      END
C
      FUNCTION DDIFF(X,Y)
      implicit double precision (a-h,o-z)
C     C.L.LAWSON AND R.J.HANSON, JET PROPULSION LABORATORY, 1973 JUNE 7
C     TO APPEAR IN @SOLVING LEAST SQUARES PROBLEMS@, PRENTICE-HALL, 1974
      DDIFF=X-Y
      RETURN
      END
