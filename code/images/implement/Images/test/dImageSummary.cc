//# imhead.cc: List image header
//# Copyright (C) 1996,1997,1998,1999,2000,2001
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
//# $Id$
//
#include <aips/aips.h>
#include <aips/Tasking/Aipsrc.h>
#include <aips/Exceptions/Error.h>
#include <aips/Inputs/Input.h>
#include <aips/Logging.h>
#include <aips/Utilities/DataType.h>
#include <aips/Utilities/String.h>
#include <trial/Images/ImageUtilities.h>
#include <trial/Images/ImageSummary.h>
#include <trial/Images/PagedImage.h>
#include <trial/Images/FITSImage.h>
#include <trial/Images/MIRIADImage.h>
#include <aips/Measures/MDoppler.h>


main (int argc, char **argv)
{
try {

   Input inputs(1);
   inputs.version ("$Revision$");

   String root = Aipsrc::aipsRoot();
   String name = root + "/data/demo/Images/test_image";
   inputs.create("in", name, "Input image name?");
   inputs.create("type", "RADIO","Velocity type ?");
   inputs.readArguments(argc, argv);
   const String in = inputs.getString("in");
   const String velocityType = inputs.getString("type");


// Open image, construct helper class object and list header

   if (in.empty()) {
      cout << "You must specify the image file name" << endl;
      return 1;
   }

   LogOrigin or("imhead", "main()", WHERE);
   LogIO os(or);

// Parse velocity type

   MDoppler::Types doppler;
   Bool ok = MDoppler::getType(doppler, velocityType);
   if (!ok) {
     os << "Invalid velocity type, using RADIO" << endl;
     doppler = MDoppler::RADIO;
   }     
//
   ImageUtilities::ImageTypes imageType = ImageUtilities::imageType(in);
   if (imageType==ImageUtilities::AIPSPP) {
      DataType pixelType = imagePixelType(in);
      if (pixelType==TpFloat) {    
         PagedImage<Float> im(in);
         ImageSummary<Float> header(im);
         header.list(os, doppler);
      } else {
         os << "Float images only" << LogIO::EXCEPTION;
      }
   } else if (imageType==ImageUtilities::FITS) {
      FITSImage im(in);
      ImageSummary<Float> header(im);
      header.list(os, doppler);
   } else if (imageType==ImageUtilities::MIRIAD) {  
      MIRIADImage im(in);
      ImageSummary<Float> header(im);
      header.list(os, doppler);
   } else {
     os << "Unrecognized image type" << LogIO::EXCEPTION;
   }


  } catch (AipsError x) {
     cerr << "aipserror: error " << x.getMesg() << endl;
     return 1;
  } 

return 0;
}
