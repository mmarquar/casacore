#include <aips/Inputs/Input.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Arrays/Cube.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Containers/Block.h>
#include <trial/Coordinates/CoordinateSystem.h>
#include <trial/Coordinates/CoordinateUtil.h>
#include <trial/Coordinates/DirectionCoordinate.h>
#include <trial/Images/TempImage.h>
#include <trial/Images/SubImage.h>
#include <trial/Images/PagedImage.h>
#include <trial/Images/ImageRegrid.h>
#include <trial/Images/ImageRegion.h>
#include <trial/Lattices/MaskedLattice.h> 
#include <trial/Lattices/LCPagedMask.h> 
#include <aips/Lattices/TempLattice.h>
#include <aips/Lattices/TiledShape.h>
#include <trial/Mathematics/Interpolate2D.h>
#include <aips/Logging/LogIO.h>
#include <aips/Utilities/Assert.h>
#include <aips/strstream.h>
#include <aips/iostream.h>



main (int argc, char **argv)
{
try {

   Input inputs(1);
   inputs.version ("$Revision$");

// Get inputs

   inputs.create("in", "", "Input image name");
   inputs.create("axes", "-10", "axes");
   inputs.create("method", "linear", "Method");
   inputs.create("save", "False", "Save output ?");
   inputs.create("shape", "-10", "Shape");
   inputs.create("replicate", "False", "Replicate ?");
   inputs.create("decimate", "0", "Decimation factor");
   inputs.create("disk", "False", "Image on disk");
   inputs.create("reuse", "False", "Reuse coordinate grid");
   inputs.create("dbg", "0", "Debug level");
   inputs.create("double", "0", "Double size ?");
   inputs.create("force", "False", "Force regridding ?");
   inputs.readArguments(argc, argv);
   const String in = inputs.getString("in");
   const Bool save = inputs.getBool("save");
   const String method = inputs.getString("method");
   const Block<Int> axesU(inputs.getIntArray("axes"));
   const Block<Int> shapeU(inputs.getIntArray("shape"));
   const Bool replicate = inputs.getBool("replicate");
   const Int decimate = inputs.getInt("decimate");
   const Bool onDisk = inputs.getBool("disk");
   const Bool dbl = inputs.getBool("double");
   const Int dbg = inputs.getInt("dbg");
   const Bool force = inputs.getBool("force");
   const Bool reuse = inputs.getBool("reuse");
//
   Int maxMBInMemory = -1;
   if (onDisk) maxMBInMemory = 0;
//
   ImageInterface<Float>* pIm = 0;

   IPosition shapeIn;
   if (in.empty()) {
      if (shapeU.nelements()>0) {
         if (shapeU.nelements()==1 && shapeU[0]==-10) {
         } else {
            shapeIn.resize(shapeU.nelements());
            for (uInt i=0; i<shapeIn.nelements(); i++) shapeIn(i) = shapeU[i];
         }
      }
//
      TiledShape shape2(shapeIn);
      CoordinateSystem cSys = CoordinateUtil::makeCoordinateSystem(shapeIn, False);
//
      pIm = new TempImage<Float>(shape2, cSys, maxMBInMemory);
      pIm->set(1.0);
//
      TempLattice<Bool> inMask(shape2, maxMBInMemory);
      inMask.set(True);
      TempImage<Float>* pTemp = dynamic_cast<TempImage<Float>*>(pIm);
      pTemp->attachMask(inMask);
   } else {
      pIm = new PagedImage<Float>(in);
      shapeIn = pIm->shape();
   }
   IPosition shapeOut = pIm->shape();
//
   IPosition axes = IPosition::makeAxisPath(pIm->ndim());
   if (axesU.nelements()>0) {
      if (axesU.nelements()==1 && axesU[0]==-10) {
      } else {
         axes.resize(axesU.nelements());
         for (uInt i=0; i<axes.nelements(); i++) axes(i) = axesU[i];
      }
   }
//
   CoordinateSystem cSysOut = pIm->coordinates();
   if (dbl) {
      Vector<Double> incr = cSysOut.increment().copy();
      Vector<Double> refp  = cSysOut.referencePixel().copy();
      Vector<Double> refv  = cSysOut.referenceValue().copy();
      for (uInt i=0; i<axes.nelements(); i++) {
         uInt j = axes(i);
         shapeOut(j) = 2 * shapeIn(j);
         incr(j) = incr(j) / 2.0;
         refp(j) = shapeOut(j) / 2.0;              // Center
      }
      cSysOut.setReferencePixel(refp);
      cSysOut.setIncrement(incr);
   } else {
      if (shapeU.nelements()==1 && shapeU[0]==-10) {
      } else if (shapeU.nelements() > 0) {
         for (uInt i=0; i<shapeU.nelements(); i++) {
            shapeOut(i) = shapeU[i];
         }
      }
   }
   cerr << "shapeIn, shapeOut = " << shapeIn << shapeOut << endl;
//
   ImageRegrid<Float> regridder;
   {
      ImageInterface<Float>* pImOut = 0;
      if (save) {
         pImOut = new PagedImage<Float>(shapeOut, cSysOut, String("outFile"));
      } else {
         pImOut = new TempImage<Float>(shapeOut, cSysOut, maxMBInMemory);
      }
      String maskName = pImOut->makeUniqueRegionName(String("mask"), 0);    
      pImOut->makeMask(maskName, True, True, True, True);
//
      Interpolate2D::Method emethod = Interpolate2D::stringToMethod(method);
      regridder.showDebugInfo(dbg);
      regridder.regrid(*pImOut, emethod, axes, *pIm, replicate, decimate, False, force);
      delete pImOut;
    }
//
    if (reuse) {
      ImageInterface<Float>* pImOut = 0;
      if (save) {
         pImOut = new PagedImage<Float>(shapeOut, cSysOut, String("outFileReused"));
      } else {
         pImOut = new TempImage<Float>(shapeOut, cSysOut, maxMBInMemory);
      }
      String maskName = pImOut->makeUniqueRegionName(String("mask"), 0);    
      pImOut->makeMask(maskName, True, True, True, True);
//
      Interpolate2D::Method emethod = Interpolate2D::stringToMethod(method);
      Cube<Double> grid;
      Matrix<Bool> gridMask;
      regridder.get2DCoordinateGrid(grid, gridMask);
      regridder.set2DCoordinateGrid(grid, gridMask);
      regridder.regrid(*pImOut, emethod, axes, *pIm, replicate, decimate, False, force);
//
      grid.resize();
      gridMask.resize();
      regridder.set2DCoordinateGrid(grid, gridMask);
      regridder.regrid(*pImOut, emethod, axes, *pIm, replicate, decimate, False, force);
//
      delete pImOut;
    }
//
    delete pIm;

} catch (AipsError x) {
     cerr << "aipserror: error " << x.getMesg() << endl;
     return 1;
} 

return 0;

}


