//# ImageProxy.h:  Proxy interface to images
//# Copyright (C) 1997-2007
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

#ifndef IMAGES_IMAGEPROXY_H
#define IMAGES_IMAGEPROXY_H

//# Includes
#include <images/Images/MaskSpecifier.h>
#include <lattices/Lattices/LatticeBase.h>
#include <lattices/Lattices/TiledShape.h>
#include <casa/Utilities/CountedPtr.h>
#include <casa/Containers/ValueHolder.h>
#include <casa/Containers/Record.h>

namespace casa {

  //# Forward Declarations.
  template<typename T> class ImageInterface;
  class LatticeExprNode;
  class CoordinateSystem;

  // <synopsis>
  // </synopsis>

  class ImageProxy
  {
  public:
    // Default constructor is needed for Boost-Python.
    ImageProxy();

    // Construct from the concatenation of the images along the axis.
    // The axis must be given as a Fortran-array axis.
    // All images must be of the same data type.
    ImageProxy (const Vector<String>& names, Int axis);

    // Construct from a string that contains an image name or image expression.
    // It is opened using ImageOpener.
    ImageProxy (const String& name, const String& mask,
                const vector<ImageProxy>& images);

    // Construct from the concatenation of the image objects along the axis.
    // The axis must be given as a Fortran-array axis.
    // All images must be of the same data type.
    //# The dummy arguments are needed to let all constructors have a
    //# different number of arguments (for Boost-Python).
    ImageProxy (const vector<ImageProxy>& images, Int axis,
                Int dummy1=0, Int dummy2=0);

    // Construct from a ValueHolder containing an Array of Float or Complex.
    // If the name is empty it is created as a temporary image, otherwise
    // as a PagedImage or HDF5Image.
    // If the coordinates record is empty, default coordinates are used.
    // A mask is created if the mask name or mask value is not empty.
    ImageProxy (const ValueHolder& values,
                const ValueHolder& mask,
                const Record& coordinates,
                const String& imageName = String(),
                Bool overwrite = True,
                Bool asHDF5 = False,
                const String& maskName = String(),
                const IPosition& tileShape = IPosition());

    // Construct from a shape.
    // If the name is empty it is created as a temporary image, otherwise
    // as a PagedImage or HDF5Image.
    // If the coordinates record is empty, default coordinates are used.
    // A mask is created if the mask name is not empty.
    ImageProxy (const IPosition& shape,
                const ValueHolder& value,
                const Record& coordinates,
                const String& imageName = String(),
                Bool overwrite = True,
                Bool asHDF5 = False,
                const String& maskName = String(),
                const IPosition& tileShape = IPosition(),
                Int dummy=0);

    // Copy constructor (reference semantics).
    ImageProxy (const ImageProxy&);

    // Assignment (reference semantics).
    ImageProxy& operator= (const ImageProxy&);

     ~ImageProxy();

    // Open an image in the file/table with the given name.
    // The specified mask will be applied (default is default mask).
    // A null pointer is returned for an unknown image type.
    // Non-AIPS++ image types must have been registered to be known.
    // If not successful, try to open it as an image expression.
    static LatticeBase* openImageOrExpr (const String& str,
                                         const MaskSpecifier&,
                                         const Block<LatticeExprNode>& nodes);

    // Turn the ImageProxy into a LatticeExprNode.
    LatticeExprNode makeNode() const;

    // Is the image persistent or temporary.
    Bool isPersistent() const;

    // Get the name of the image.
    String name (Bool stripPath=False) const;

    // Get the shape of the image.
    IPosition shape() const;

    // Get the dimensionality of the image.
    uInt ndim() const;

    // Get the size of the image (nr of pixels).
    uInt size() const;

    // Get the data type of the image.
    String dataType() const;

    // Get a chunk of data.
    ValueHolder getData (const IPosition& blc,
                         const IPosition& trc, 
                         const IPosition& inc);

    // Get a chunk of the mask.
    ValueHolder getMask (const IPosition& blc,
                         const IPosition& trc, 
                         const IPosition& inc);

    // Put a chunk of data.
    void putData (const ValueHolder&,
                  const IPosition& blc,
                  const IPosition& inc);

    // Put a chunk of the mask.
    // The mask will be created if not present yet.
    // That will not be done if the entire mask is True.
    void putMask (const ValueHolder& value,
                  const IPosition& blc,
                  const IPosition& inc);

    // Does the image have a read or write lock?
    Bool hasLock (Bool writeLock = False);

    // Try to acquire a read or write lock.
    // nattempts=0 means wait until acquired. Otherwise every second an
    // attempt is done.
    void lock (Bool writeLock=False, Int nattempts=0);

    // Release the lock acquired by lock().
    void unlock();

    // Form a new (virtual) image being a subset of the image.
    ImageProxy subImage (const IPosition& blc,
                         const IPosition& trc, 
                         const IPosition& inc,
                         Bool dropDegenerate=True);

    // Get the brightness unit.
    String unit() const;

    // Get the coordinate system.
    Record coordSys() const;

    // Get the image info.
    Record imageInfo() const;

    // Get the miscellaneous info.
    Record miscInfo() const;

    // Get the history.
    Vector<String> history() const;

    // Write the image in FITS format.
    // See class ImageFITSConverter for a description of the arguments.
    // Currently only a float image can be written to FITS.
    void toFits (const String& fitsfile, Bool overwrite=True,
                 Bool velocity=True, Bool optical=True, Int bitpix=-32,
                 Double minpix=1, Double maxpix=-1) const;

    // Write the image to an image file with the given name.
    // An exception is thrown if the name is the name of an already open image.
    void saveAs (const String& fileName, Bool overwrite=True,
                 Bool hdf5=False,
                 Bool copyMask=True, const String& newMaskName=String(),
                 const IPosition& newTileShape=IPosition()) const;

    // Return the statistics for the given axes.
    // E.g. fn axes 0,1 is given in a 3-dim image, the statistics are calculated
    // for each plane along the 3rd axis.
    // MinMaxValues can be given to include or exclude (4th argument) pixels
    // with values in the given range. If only one value is given, min=-abs(val)
    // and max=abs(val).
    // Robust statistics (Median, MedAbsDevMed, and Quartile) can be returned
    // too.
    Record statistics (const Vector<Int>& axes,
                       const String& mask,
                       const ValueHolder& minMaxValues,
                       Bool exclude = False,
                       Bool robust = False) const;

    // Regrid the image on the given axes to the given coordinate system.
    // The output is stored in the given file; it no file name is given a
    // temporary image is made.
    // If the output shape is empty, the old shape is used.
    // <src>replicate=True</src> means replication rather than regridding.
    ImageProxy regrid (const Vector<Int>& axes = Vector<Int>(),
                       const String& outfile = String(),
                       Bool overwriteOutFile = True,
                       const IPosition& outShape = IPosition(),
                       const Record& coordSys = Record(),
                       const String& method = "linear",
                       Int decimate = 10,
                       Bool replicate = False,
                       Bool doRefChange = True,
                       Bool forceRegrid = False);

    // Check and adjust blc, trc, or inc using the shape.
    // <group>
    static IPosition adjustBlc (const IPosition& blc, const IPosition& shp);
    static IPosition adjustTrc (const IPosition& trc, const IPosition& shp);
    static IPosition adjustInc (const IPosition& inc, const IPosition& shp);
    // </group>


    /*

    ImageProxy rotate(const String& outfile, 
                      const IPosition& shape, 
                      const Quantity& pa, Record& region, 
                      const String& mask, 
                      const String& method = "cubic", 
                      Int decimate = 0, 
                      Bool replicate = False, 
                      Bool dropdeg = False,
                      Bool overwrite = False);


    Bool setbrightnessunit (const String& unit);

    Bool setcoordsys (const Record& csys);

    Bool sethistory (const String& origin, const Vector<String>& history);

    Bool setmiscinfo (const Record& info);

    ImageProxy subimage(const String& outfile, Record& region, 
                        const String& mask, 
                        Bool dropdeg = False, 
                        Bool overwrite = False, 
                        Bool list = True);

    Vector<Double> topixel(Record& value);

    Record toworld(const Vector<double>& value, const String& format = "n");
    */

    LatticeBase* getLattice() const
      { return itsLattice.operator->(); }

 private:
    // Form an ImageProxy object from an existing image object.
    explicit ImageProxy (LatticeBase*);

    // Open the image (which can also be an expression.
    // Throw an exception if not succeeded.
    void openImage (const String& name, const String& mask,
                    const vector<ImageProxy>& images);

    // Make an image from an array or shape.
    template<typename T>
    void makeImage (const Array<T>& array, const Array<Bool>& mask,
                    const IPosition& shape, const Record& coordinates,
                    const String& fileName, Bool asHDF5,
                    const String& maskName,
                    const IPosition& tileShape);

    // Form a concatenated image.
    // <group>
    void concatImages         (const std::vector<ImageProxy>& images, Int axis);
    void concatImagesFloat    (const std::vector<ImageProxy>& images, Int axis);
    void concatImagesDouble   (const std::vector<ImageProxy>& images, Int axis);
    void concatImagesComplex  (const std::vector<ImageProxy>& images, Int axis);
    void concatImagesDComplex (const std::vector<ImageProxy>& images, Int axis);
    // </group>

    // Setup the pointers for the various image data types.
    void setup (LatticeBase* lattice);

    // Centre all axes except the Stokes one.
    void centreRefPix (CoordinateSystem& cSys, 
                       const IPosition& shape) const;

    // Put the mask and create it if needed.
    template<typename T>
    void doPutMask (ImageInterface<T>& image,
                    const ValueHolder& value,
                    const IPosition& blc,
                    const IPosition& inc);

    // Copy the image to an image (PagedImage or HDF5Image) with the given name.
    // A new tile shape can be given.
    // If the image is masked, the mask can be copied as well.
    template <typename T>
    void saveImage (const String& fileName,
                    Bool hdf5, Bool copyMask,
                    const String& newMaskName, 
                    const IPosition& newTileShape,
                    const ImageInterface<T>& image) const;

    // Form a tiled shape from the current shape and a possible new tile shape.
    TiledShape makeTiledShape (const IPosition& newTileShape,
                               const IPosition& shape,
                               const IPosition& oldTileShape=IPosition()) const;

    // Calculate the statistics.
    template<typename T>
    Record makeStatistics (const ImageInterface<T>& image,
                           const Vector<Int>& axes,
                           const String& mask,
                           const ValueHolder& minMaxValues,
                           Bool exclude,
                           Bool robust) const;

    // Do the actual regridding.
    template<typename T>
    ImageProxy doRegrid (const ImageInterface<T>& image,
                         const Vector<Int>& axes,
                         const String& outfile,
                         Bool overwrite,
                         const IPosition& shape,
                         const Record& coordSys,
                         const String& method,
                         Int decimate,
                         Bool replicate, 
                         Bool doRefChange,
                         Bool force);

    // Make a coordinate system from the Record.
    // The cylindrical fix is applied if needed.
    CoordinateSystem makeCoordinateSystem (const Record& coordinates,
                                           const IPosition& shape) const;


    //# Data members.
    //# itsLattice is the real data; the pointers are for type convenience only.
    CountedPtr<LatticeBase>   itsLattice;
    ImageInterface<Float>*    itsImageFloat;
    ImageInterface<Double>*   itsImageDouble;
    ImageInterface<Complex>*  itsImageComplex;
    ImageInterface<DComplex>* itsImageDComplex;
    const CoordinateSystem*   itsCoordSys;
  };

} // end namespace casa

#endif
