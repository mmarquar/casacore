      real function maxeig(i,q,u,v)
      real i, q, u, v
      maxeig=i+sqrt(q**2+u**2+v**2)
      return 
      end
      real function mineig(i,q,u,v)
      real i, q, u, v
      mineig=i-sqrt(q**2+u**2+v**2)
      return 
      end
      real function eigprod(i,q,u,v)
      real i, q, u, v
      eigprod=i**2-(q**2+u**2+v**2)
      return 
      end
      real function l2norm(i,q,u,v)
      real i, q, u, v
      l2norm=i**2+q**2+u**2+v**2
      return 
      end
      subroutine hclean(limage, limagestep, lpsf, lmask, nx, ny, npol,
     $     xbeg, xend, ybeg, yend, niter, gain, thres, msgput, stopnow)
      
      implicit none
      integer nx, ny, npol, xbeg, xend, ybeg, yend, niter, yes
      real limage(nx, ny, npol)
      real limagestep(nx, ny, npol)
      real lpsf(nx, ny), lmask(nx, ny)
      real gain, thres
      external msgput
      external stopnow

      real peak, mev, maxeig
      real maxVal(4), pv(4)
      integer px, py, pol, iter, ix, iy, cycle
      
      do iter=1,niter
         
c     Now find peak in residual image 
         peak=0.0
         px=xbeg
         py=ybeg
         do iy=ybeg,yend
            do ix=xbeg,xend
               if(lmask(ix,iy).GT.0.000001) then
                  if(npol.EQ.4) then
                     mev=maxeig(limageStep(ix,iy,1),
     $                    limageStep(ix,iy,2), limageStep(ix,iy,3),
     $                    limageStep(ix,iy,4))
                  else if(npol.EQ.2) then
                     mev=limageStep(ix,iy,1)+ abs(limageStep(ix,iy,2))
                  else 
                     mev=limageStep(ix,iy,1)
                  end if
                  if(abs(mev).GT.peak) then
                     px=ix
                     py=iy
                     peak=abs(mev)
                  end if
               end if
            end do
         end do
      
         if(npol.EQ.4) then
            maxVal(1)=limageStep(px,py,1)
            maxVal(2)=limageStep(px,py,2)
            maxVal(3)=limageStep(px,py,3)
            maxVal(4)=limageStep(px,py,4)
         else if(npol.EQ.2) then
            maxVal(1)=limageStep(px,py,1)
            maxVal(2)=0.0
            maxVal(3)=0.0
            maxVal(4)=limageStep(px,py,2)
         else 
            maxVal(1)=limageStep(px,py,1)
            maxVal(2)=0.0
            maxVal(3)=0.0
            maxVal(4)=0.0
         end if
         
c     // Output ten lines of information if run to the end
         cycle=niter/10
         if((iter.EQ.1).OR.(mod(iter,cycle)).EQ.1) then
            call msgput(iter, px, py, maxVal)
            call stopnow(yes)
         endif
         if((yes.EQ.1).OR.(peak.LT.thres)) then
            call msgput(-iter, px, py, maxVal)
            return
         endif
         
c  Add the scaled peak to the current image
         if(npol.EQ.4) then
            do pol=1,npol
               pv(pol)=gain*maxVal(pol)
            end do
            do pol=1,npol
               limage(px,py,pol)=limage(px,py,pol)+pv(pol)
            end do
            do pol=1,npol
               do iy=ybeg,yend
                  do ix=xbeg,xend
                     limageStep(ix,iy,pol)=limageStep(ix,iy,pol)
     $                    -pv(pol) * lpsf(nx/2+ix-px+1,ny/2+iy-py+1)
                  end do
               end do
            end do
         else if(npol.EQ.2) then
            pv(1)=gain*maxVal(1)
            pv(4)=gain*maxVal(4)
            limage(px,py,1)=limage(px,py,1)+pv(1)
            limage(px,py,2)=limage(px,py,2)+pv(4)
            do iy=ybeg,yend
               do ix=xbeg,xend
                  limageStep(ix,iy,1)=limageStep(ix,iy,1)
     $              - pv(1)*lpsf(nx/2+ix-px+1,ny/2+iy-py+1)
                  limageStep(ix,iy,2)=limageStep(ix,iy,2)
     $              - pv(4)*lpsf(nx/2+ix-px+1,ny/2+iy-py+1)
               end do
            end do
         else 
            pv(1)=gain*maxVal(1)
            limage(px,py,1)=limage(px,py,1)+pv(1)
            do iy=ybeg,yend
               do ix=xbeg,xend
                  limageStep(ix,iy,1)=limageStep(ix,iy,1)
     $                 -pv(1)*lpsf(nx/2+ix-px+1,ny/2+iy-py+1)
               end do
            end do
         end if
         
      end do
 100  continue
      call msgput(-iter, px, py, maxVal)
      return
      end

