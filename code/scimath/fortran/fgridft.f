C
C Grid a number of visibility records
C
      subroutine ggridft (uvw, dphase, values, nvispol, nvischan,
     $   dopsf, flag, weight, nrow, irow,
     $   scale, offset, grid, nx, ny, npol, nchan, freq, c,
     $   support, sampling, convFunc, chanmap, polmap, sumwt)

      implicit none
      integer nx, ny, npol, nchan, nvispol, nvischan, nrow
      complex values(nvispol, nvischan, nrow)
      complex grid(nx, ny, npol, nchan)
      double precision uvw(3, nrow), freq(nvischan), c, scale(2),
     $     offset(2)
      double precision dphase(nrow)
      complex phasor
      logical flag(nvispol, nvischan, nrow)
      real weight(nvischan, nrow), sumwt(npol, nchan)
      integer irow
      integer support, sampling
      integer chanmap(nvischan), polmap(nvischan)
      logical dopsf

      complex nvalue

      double precision convFunc(*)
      double precision norm, wt

      logical ogridft

      double precision pos(2)
      integer loc(2), off(2), iloc(2)
      integer rbeg, rend
      integer ix, iy, ipol, ichan
      integer apol, achan

      if(irow.ge.0) then
         rbeg=irow+1
         rend=irow+1
      else 
         rbeg=1
         rend=nrow
      end if

      do irow=rbeg, rend
         do ichan=1, nvischan
            achan=chanmap(ichan)+1
            if((achan.ge.1).and.(achan.le.nchan).and.
     $           (weight(ichan,irow).gt.0.0)) then
               call sgridft(uvw(1,irow), dphase(irow), freq(ichan), c, 
     $              scale, offset, sampling, pos, loc, off, phasor)
               if (ogridft(nx, ny, loc, support)) then
                  do ipol=1, nvispol
                     apol=polmap(ipol)+1
                     if((.not.flag(ipol,ichan,irow)).and.
     $                    (apol.ge.1).and.(apol.le.npol)) then
C If we are making a PSF then we don't want to phase
C rotate but we do want to reproject uvw
                        if(dopsf) then
                           nvalue=cmplx(weight(ichan,irow))
                        else
                           nvalue=weight(ichan,irow)*
     $                        conjg(values(ipol,ichan,irow)*phasor)
                        end if
                        norm=0.0
                        do iy=-support,support
                           iloc(2)=abs(sampling*iy+off(2))+1
                           do ix=-support,support
                              iloc(1)=abs(sampling*ix+off(1))+1
                              wt=convFunc(iloc(1))*convFunc(iloc(2))
                              grid(loc(1)+ix,loc(2)+iy,apol,achan)=
     $                             grid(loc(1)+ix,loc(2)+iy,apol,achan)+
     $                             nvalue*wt
                              norm=norm+wt
                           end do
                        end do
                        sumwt(apol,achan)=sumwt(apol,achan)+
     $                       weight(ichan,irow)*norm
                     end if
                  end do
               end if
            end if
         end do
      end do
      return
      end
C
C Degrid a number of visibility records
C
      subroutine dgridft (uvw, dphase, values, nvispol, nvischan, flag, 
     $     nrow, irow, scale, offset, grid, nx, ny, npol, nchan, freq,
     $     c, support, sampling, convFunc, chanmap, polmap)

      implicit none
      integer nx, ny, npol, nchan, nvispol, nvischan, nrow
      complex values(nvispol, nvischan, nrow)
      complex grid(nx, ny, npol, nchan)
      double precision uvw(3, nrow), freq(nvischan), c, scale(2),
     $     offset(2)
      double precision dphase(nrow)
      complex phasor
      logical flag(nvispol, nvischan, nrow)
      integer irow
      integer support, sampling
      integer chanmap(*), polmap(*)

      complex nvalue

      double precision convFunc(*)
      double precision norm

      logical ogridft

      double precision pos(2)
      integer loc(2), off(2), iloc(2)
      integer rbeg, rend
      integer ix, iy, ipol, ichan
      integer apol, achan
      double precision wt

      if(irow.ge.0) then
         rbeg=irow+1
         rend=irow+1
      else 
         rbeg=1
         rend=nrow
      end if

      do irow=rbeg, rend
         do ichan=1, nvischan
            achan=chanmap(ichan)+1
            if((achan.ge.1).and.(achan.le.nchan)) then
               call sgridft(uvw(1,irow), dphase(irow), freq(ichan), c,
     $              scale, offset, sampling, pos, loc, off, phasor)
               if (ogridft(nx, ny, loc, support)) then
                  do ipol=1, nvispol
                     apol=polmap(ipol)+1
                     if((.not.flag(ipol,ichan,irow)).and.
     $                    (apol.ge.1).and.(apol.le.npol)) then
                        nvalue=0.0
                        norm=0.0
                        do iy=-support,support
                           iloc(2)=abs(sampling*iy+off(2))+1
                           do ix=-support,support
                              iloc(1)=abs(sampling*ix+off(1))+1
                              wt=convFunc(iloc(1))*convFunc(iloc(2))
                              norm=norm+wt
                              nvalue=nvalue+wt*
     $                             grid(loc(1)+ix,loc(2)+iy,apol,achan)
                           end do
                        end do
                        values(ipol,ichan,irow)=conjg(nvalue*phasor)
     $                       /norm
                     end if
                  end do
               end if
            end if
         end do
      end do
      return
      end
C
C Calculate gridded coordinates and the phasor needed for
C phase rotation.
C
      subroutine sgridft (uvw, dphase, freq, c, scale, offset, sampling, 
     $     pos, loc, off, phasor)
      implicit none
      integer sampling
      integer loc(2), off(2)
      double precision uvw(3), freq, c, scale(2), offset(2), pos(2)
      double precision dphase, phase
      complex phasor
      integer idim
      double precision pi
      data pi/3.14159265358979323846/

      do idim=1,2
         pos(idim)=scale(idim)*uvw(idim)*freq/c+(offset(idim)+1.0)
         loc(idim)=nint(pos(idim))
         off(idim)=nint((loc(idim)-pos(idim))*sampling)
      end do
      phase=-2.0D0*pi*dphase*freq/c
      phasor=cmplx(cos(phase), sin(phase))
      return 
      end
C
C Is this on the grid?
C
      logical function ogridft (nx, ny, loc, support)
      implicit none
      integer nx, ny, loc(2), support
      ogridft=(loc(1)-support.ge.1).and.(loc(1)+support.le.nx).and.
     $        (loc(2)-support.ge.1).and.(loc(2)+support.le.ny)
      return
      end
