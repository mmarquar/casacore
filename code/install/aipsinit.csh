#!/bin/echo Usage: source
#-----------------------------------------------------------------------------
# aipsinit.csh: Define the AIPS++ environment for C-like shells
#-----------------------------------------------------------------------------
#
#   Copyright (C) 1992-1997
#   Associated Universities, Inc. Washington DC, USA.
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
#   Correspondence concerning AIPS++ should be addressed as follows:
#          Internet email: aips2-request@nrao.edu.
#          Postal address: AIPS++ Project Office
#                          National Radio Astronomy Observatory
#                          520 Edgemont Road
#                          Charlottesville, VA 22903-2475 USA
#
#-----------------------------------------------------------------------------
# Usage: source aipsinit.csh
#-----------------------------------------------------------------------------
# aipsinit.csh bootstraps the AIPS++ user environment for C-like shells.
# It sets or modifies the following environment variables based partly on
# information contained in the "aipshosts" file:
#
#    AIPSPATH  A sequence of four character strings separated by spaces
#              comprised of the following:
#
#                 1) Root of the AIPS++ directory tree.
#                 2) Host "architecture".
#                 3) Local site name.
#                 4) Name of the machine.
#
#    PATH      The PATH modified for AIPS++.  Any prior AIPS++ path
#              definitions will be removed.
#
#              If the keyword "aips_bin" is encoded into PATH prior to
#              invoking this script it will be replaced with a fully
#              qualified pathname.
#
#              If the "aips_bin" keyword is not present, the AIPS++ bin
#              directory will be added to the start of PATH, following
#              "." if it comes first.
#
#    MANPATH   If defined, MANPATH will be modified for AIPS++ in the same
#              manner as for PATH except using the "aips_doc" keyword.
#
# The host "architecture", a string of the form "<base>[_<ext>]", corresponds
# to a subdirectory of the AIPS++ root directory in which an AIPS++ system
# resides (object libraries, executables,...).  The base part of the name is a
# mnemonic predefined by AIPS++ which describes the innate binary format of a
# machine and reflects the combination of machine architecture and operating
# system.  An optional extension of the architecture name can select between
# alternative systems of the same binary format, for example as may have been
# generated by different compilers.
#
# aipsinit recognizes a shell or environment variable "aips_ext" which, if
# defined, overrides the architecture extension defined in the "aipshosts"
# file for a particular machine.  By convention, setting "aips_ext" to "_"
# signals no extension, that is, any architecture extension specified in
# "aipshosts" is stripped off.  It also defines an alias called 'aipsinit'
# which takes a value for "aips_ext" as an argument and reinvokes aipsinit.
# This provides a convenient mechanism for interactively switching between
# architecture extensions.  Note that aipsinit always undefines "aips_ext".
#
# The "aipshosts" file may contain a catch-all entry, "DEFAULT", which applies
# for hosts which are not specifically listed.  An architecture type of "NONE"
# signals that the particular host does not have an AIPS++ installation and
# causes AIPS++ initialization to be skipped.  Thus an "aipshosts" entry of
#
#    DEFAULT	NONE
#
# defeats AIPS++ initialization for all hosts other than those specifically
# listed.
#
# Notes:
#    1) There are some subtleties in the way the spacing of PATH
#       elements is handled.  In particular, advantage is made of the
#       fact that if x=" a  b   c    ", then
#
#         `echo $x`     returns   "a b c",
#
#       whereas
#
#         `echo "$x"`   returns   " a  b   c    ".
#
#    2) The "exit" command does not work as expected here.
#
#    3) Shell variables used as temporaries are prefixed with "a_".
#
# Original: 1992/03/05 by Mark Calabretta, ATNF.
# $Id$
#=============================================================================
# The AIPS++ root directory is hardcoded here.
  if (! $?a_root) set a_root = __AIPSROOT

#------------------ Do not change anything below this line. ------------------

# Rederive the host name.
  set a_host = `uname -n | awk -F. '{ print $1 }'`

# Look for this host in the AIPS++ hosts database.
  if (-f "$a_root/aipshosts") then
     set a_temp = `egrep "^[ 	]*${a_host}[ 	]" "$a_root/aipshosts"`
     if ("$a_temp" == "") then
#       Look for a DEFAULT entry.
        set a_temp = `egrep "^[ 	]*DEFAULT[ 	]" "$a_root/aipshosts"`
     endif
  else
     echo "aipsinit: Please create the $a_root/aipshosts database."
     set a_temp = ""
  endif

# Request an update to aipshosts.
  if (! $?AIPSPATH && "$a_temp" == "") then
     echo "aipsinit: Please add an entry for $a_host to $a_root/aipshosts."
  endif

# Set the architecture and site.
  set a_arch = ""
  set a_site = ""
  if ("$#a_temp" > 1) then
     set a_arch = $a_temp[2]
  endif
  if ("$#a_temp" > 2) then
     set a_site = $a_temp[3]
  endif

  if ("$a_arch" == NONE) then
#    Remove aips_bin and aips_doc from PATH and MANPATH.
     setenv PATH `echo ":${PATH}:" | sed -e '{s#:aips_bin:#:#g;s#^:##;s#:$##;}'`
     if ($?MANPATH) then
        setenv MANPATH `echo ":${MANPATH}:" | sed -e '{s#:aips_doc:#:#g;s#^:##;s#:$##;}'`
     endif

  else
     if ("$a_arch" == "") then
#       Try to deduce the architecture.
        switch (`uname -s`)
        case SunOS:
           if (`uname -r` =~ 5.*) then
              set a_arch = sun4sol
           else
              set a_arch = sun4
           endif
           breaksw
        case HPUX:
           set a_arch = hpux
           breaksw
        case Linux:
           set a_arch = linux
           breaksw
        IRIX*:
           set a_arch = sgi
           breaksw
        default:
           if (`uname -m` == alpha) then
              set a_arch = alpha
           else
              set a_arch = UNKNOWN_ARCH
           endif
        endsw
     endif

     if ("$a_site" == "") then
#       Try to deduce the site name.
        if ($?nonomatch) then
           set a_temp = $a_root/$a_arch/*/makedefs
        else
           set nonomatch
           set a_temp = $a_root/$a_arch/*/makedefs
           unset nonomatch
        endif

        set noglob
        set a_site = `echo "$a_temp[1]" | awk -F/ '{ print $(NF-1) }'`
        unset noglob
        if ("$a_site" == "*") set a_site = UNKNOWN_SITE
     endif

#    Reset the architecture extension if required.
     if ($?aips_ext) then
        if ("$aips_ext" == "_" || "$aips_ext" == " ") then
           set a_arch=`echo $a_arch | sed -e 's/_.*//'`
        else if ("$aips_ext" != "") then
           set a_arch=`echo ${a_arch}_$aips_ext | sed -e '{s/ .*//;s/_.*_/_/;}'`
        endif

        unset aips_ext
        unsetenv aips_ext
     endif

#    Is AIPSPATH already defined?
     if ($?AIPSPATH) then
        set a_och = `echo $AIPSPATH | awk '{print $2}'`
        set a_old = `echo $AIPSPATH | awk '{printf("%s/%s",$1,$2)}'`
     else
        set a_och = "$a_arch"
        set a_old = "$a_root/$a_arch"
     endif

     if (! -d "$a_root/$a_arch") then
        echo "Warning: $a_root/$a_arch does not exist."
     else if (! -d "$a_root/$a_arch/$a_site") then
        echo "Warning: $a_root/$a_arch/$site does not exist."
     endif

#    Define AIPSPATH.
     setenv AIPSPATH "$a_root $a_arch $a_site $a_host"

#    Update the prompt string.
     cd .


#    Reset PATH.
     set a_new = `echo " $PATH " | sed -e 's#::*# #g' -e "s# $a_old/bin # aips_bin #g" -e "s#/aips++/$a_och/#/aips++/$a_arch/#g" -e "s# aips_bin # $a_root/$a_arch/bin #g"`

#    Ensure that some AIPS++ bin area got into PATH.
     echo " $a_new " | grep " $a_root/$a_arch/bin " >& /dev/null
     if ("$status" != 0) then
#       Leave "." first, and put the AIPS++ areas next.
        if ("$a_new[1]" == ".") then
           set a_new = ". $a_root/$a_arch/bin $a_new[2-]"
        else
           set a_new = "$a_root/$a_arch/bin $a_new"
        endif
     endif

#    Reset it, with sanity check!
     set a_new = `echo $a_new | sed -e 's# #:#g'`
     if ("$a_new" != "") setenv PATH "$a_new"


#    Reset MANPATH.
     if ("$?MANPATH") then
        set a_new = `echo " $MANPATH " | sed -e 's#::*# #g' -e "s# $a_old/doc # aips_doc #g" -e "s# aips_doc # $a_root/$a_arch/doc #g"`

#       Ensure that some AIPS++ man area got into MANPATH.
        echo " $a_new " | grep " $a_root/$a_arch/doc " >& /dev/null
        if ("$status" != 0) set a_new = "$a_root/$a_arch/doc $a_new"

#       Reset it, with sanity check!
        set a_new = `echo $a_new | sed -e 's# #:#g'`
        if ("$a_new" != "") setenv MANPATH "$a_new"
     endif

#    Alias which invokes aipsinit with "aips_ext" as a command line argument.
     alias aipsinit "set aips_ext = (\!*) ; set a_root = $a_root ; source $a_root/aipsinit.csh"
  endif

# Clean up.
  unset a_arch a_host a_new a_och a_old a_root a_site a_temp
