#!/bin/sh
#
#   Copyright (C) 1997
#   Associated Universities, Inc. Washington DC, USA.
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the Free
#   Software Foundation; either version 2 of the License, or (at your option)
#   any later version.
#
#   This program is distributed in the hope that it will be useful, but WITHOUT
#   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
#   more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program; if not, write to the Free Software Foundation, Inc.,
#   675 Massachusetts Ave, Cambridge, MA 02139, USA.
#
#   Correspondence concerning AIPS++ should be addressed as follows:
#          Internet email: aips2-request@nrao.edu.
#          Postal address: AIPS++ Project Office
#                          National Radio Astronomy Observatory
#                          520 Edgemont Road
#                          Charlottesville, VA 22903-2475 USA
#
#   $Id$
#
#
#  This here shell script replaces the iconserver string generated by
# latex2html with a relative path for the icons.  It gets called by the
# makefile.doc after latex2html is run.  It's really only needed for the
# distribution of documents, so the icons won't be served remotely.
#
#  This shell script takes three arguments
#  First argument is the docsarea
#  Second argument is the iconserver, generated by latex2html
#  Third argument is the subdirectory which contains the html files
#
echo Using relative icons.
ICONSERVER=`echo $2 | sed 's/\//./g'`
#
# cd into the directory containing the html files to munge
#
cd $3
#
# Find where the gifs are...
#
# this is dangerous cause it could cause an infinite loop, but it shouldn't
# be a problem as long as this script is only used inside the aips++ make
# system.
#
#echo $1
GIFPARENT=..
while(true)
do
   if [ -d $1/$GIFPARENT/gif ] ; then
      GIFPARENT=../$GIFPARENT
      break;  
   fi
   GIFPARENT=../$GIFPARENT
done
#echo $GIFPARENT
#
# Make the sed script,  how 'bout all them \\\\\'s, hey it works!
#
LOCALICONS=`echo $GIFPARENT/gif/latex2html | sed 's/\//\\\\\//g'`
cat sedfile
#
# Do the translation
#
for filename in *.html
do
   sed -f sedfile $filename > $filename.local
   mv $filename.local $filename
done
#
# Clean up
#
rm sedfile
