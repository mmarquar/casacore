//# ImageFITSConverter.h: Interconvert between AIPS++ Images and FITS files
//# Copyright (C) 1996
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


#if !defined(AIPS_IMAGE_FITS_CONVERTER_H)
#define AIPS_IMAGE_FITS_CONVERTER_H

#include <aips/aips.h>

template<class T> class PagedImage;
class IPosition;
class String;

// <summary>
// Interconvert between AIPS++ Images and FITS files.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
// <li> <linkto class="PagedImage">PagedImage</linkto>
// <li> <linkto class="PrimaryArray">PrimaryArray</linkto> (and FITS concepts in
//      general).
// </prerequisite>
//
// <synopsis>
// This class is a helper class that is used to interconvert between AIPS++
// images and FITS files. This adds no functionality over the general abilities
// available in the underlying FITS classes, however it is a useful higher-level
// packaging.
//
// There are two fundamental member functions in this class.
// <src>FITSToImage</src> which turns a FITS file into an AIPS++ image, and
// <src>ImageToFITS</src> which does the opposite.
//
// We can read images from any HDU inside the FITS file (although this isn't
// well tested). However at the moment we always write to the first HDU, i.e.
// to the primary array, not an image extension.
// </synopsis>
//
// <example>
// A FITS to image conversion may be accomplished as follows:
// <srcBlock>
//    PagedImage<Float> *image = 0;
//    String fitsName = "exists.fits";
//    String imageName = "new.image";
//    String error;
//    ImageFITSConverter::FITSToImage(image, error, imageName, fitsName);
//    if (!image) ... error ...
// </srcBlock>
// A couple of things to note:
// <ul>
//    <li> The pointer "image" is set if the conversion succeeds. If it is
//         zero the conversion failed and <src>error</src> will contain an
//         error message.
//    <li> The caller is responsible for deleting the pointer <src>image</src>
//         when the conversion is successful.
//    <li> The conversion will not overwrite an existing image.
// </ul>
// Similarly, an image to FITS conversion may be accomplised as follows:
// <srcBlock>
//    String imageName = argv[1];
//    PagedImage<Float> image = ...; // An existing image from somewhere
//    String fitsName = "new.fits";
//    String error;
//    Bool ok = ImageFITSConverter::ImageToFITS(error, image, fitsName);
// </srcBlock>
// A couple of similar remarks can be made about this example:
// <ul>
//    <li> If <src>ok</src> is False, the conversion failed and <src>error</src>
//         will be set.
//    <li> The conversion will not overwrite an existing image.
// </ul>
// </example>
//
// <motivation>
// FITS files are the fundamental transport format for images in Astronomy.
// </motivation>
//
// <todo asof="1996/11/19">
//   <li> It might be useful to have functions that convert between FITS
//        and general lattices.
//   <li> Add support for PagedImage<Complex>
//   <li> At the moment we always write BITPIX=-32 images. Should we offer
//        support for other types? (We read all types).
//   <li> Convert multiple images at once?
//   <li> Allow existing images to be overwritten?
// </todo>

class ImageFITSConverter
{
public:
    // Convert a FITS file to an AIPS++ image.
    // <ul>
    //   <li> <src>newImage</src> will be zero if the conversion fail. If the 
    //        conversion succeeds, the caller is responsible for deleting this
    //        pointer.
    //   <li> <src>error</src> will be set if the conversion fails.
    //   <li> <src>imageName</src> cannot already exit.
    //   <li> <src>fitsName</src> must already exist (and have an image at the
    //        indicated HDU).
    //   <li> <src>whichHDU</src> Zero-relative hdu. The default is correct for
    //        a primary array, set it for an image extension. Only zero has been
    //        tested.
    //   <li> <src>memoryInMB</src>. Setting this to zero will result in
    //        row-by-row copying, otherwise it will attempt to with as large
    //        a chunk-size as possible, while fitting in the desired memory.
    // </ul>
    static void FITSToImage(PagedImage<Float> *&newImage,
			    String &error,
			    const String &imageName,
			    const String &fitsName, 
			    uInt whichHDU = 0,
			    uInt memoryInMB = 64);
    // Convert an AIPS++ image to a FITS file.
    // <ul>
    //   <li> <src>return</src> True if the conversion succeeds, False otherwise.
    //   <li> <src>error</src> will be set if the conversion fails.
    //   <li> <src>image</src> The image to convert.
    //   <li> <src>fitsName</src> Will not overwrite an existing FITS file.
    //        Always writes to the primary array, and writes with BITPIX=-32,
    //        i.e. floating point.
    //   <li> <src>memoryInMB</src>. Setting this to zero will result in
    //        row-by-row copying, otherwise it will attempt to with as large
    //        a chunk-size as possible, while fitting in the desired memory.
    // </ul>
    static Bool ImageToFITS(String &error,
			    const PagedImage<Float> &image,
			    const String &fitsName, 
			    uInt memoryInMB = 64,
			    Bool preferVelocity = True,
			    Bool opticalVelocity = True);

    // Helper function - used to calculate a cursor appropriate for the desired
    // memory use. It's not intended that application programmers call this, but
    // you may if it's useful to you.
    static IPosition copyCursorShape(String &report,
				     const IPosition &shape, 
				     uInt imagePixelSize,
				     uInt fitsPixelSize,
				     uInt memoryInMB);
};

// <synopsis>
// This class is an internal class used to implement 
// ImageFitsConverter::FITSToImage - in particular, it has the code which
// is dependent on the various types (BITPIX values).
// </synopsis>
template<class HDUType> class ImageFITSConverterImpl
{
public:
    static void FITSToImage(PagedImage<Float> *&newImage,
			    String &error,
			    const String &imageName,
			    HDUType &fitsImage,
			    uInt memoryInMB = 64);
};

#endif
