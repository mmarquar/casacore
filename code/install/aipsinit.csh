#!/bin/echo Usage: source
#-----------------------------------------------------------------------------
# aipsinit.csh: Define the AIPS++ environment for C-like shells
#-----------------------------------------------------------------------------
#
#   Copyright (C) 1992-1999,2000
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
#    LD_LIBRARY_PATH   If defined, it will be modified for AIPS++ in the same
#              manner as for PATH except using the "aips_lib" keyword.
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
  set a_temp = ""
  if (-f "$a_root/aipshosts") then
     set a_temp = `egrep "^[ 	]*${a_host}[ 	]" "$a_root/aipshosts"`
     if ("$a_temp" == "") then
#       Look for a DEFAULT entry.
        set a_temp = `egrep "^[ 	]*DEFAULT[ 	]" "$a_root/aipshosts"`
     endif
  endif

# Request an update to aipshosts.
  if ("$a_temp" == "") then
     set a_temp = "DEFAULT"
  endif

  setenv GLISHROOT "$a_root"
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
#    Remove aips_bin, aips_lib, and aips_doc from PATH, LD_LIBRARY_PATH,
#    and MANPATH.
     setenv PATH `echo ":${PATH}:" | sed -e '{s#:aips_bin:#:#g;s#^:##;s#:$##;}'`
     if ($?LD_LIBRARY_PATH) then
        setenv LD_LIBRARY_PATH `echo ":${LD_LIBRARY_PATH}:" | sed -e '{s#:aips_lib:#:#g;s#^:##;s#:$##;}'`
     endif
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
        case HP-UX:
           if (`uname -r` =~ *.*) then
              set a_arch = hpux
           else
              set a_arch = hpux10
           endif
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

     set a_nonomatch = 0
     if ($?nonomatch) set a_nonomatch = 1
     set nonomatch

     if ("$a_site" == "") then
        # Sometimes the host name is used as the architecture (eg. dop03_egcs).
        # So look if such a directory with files exists.
        set a_temp = $a_root/${a_host}_*/*
        if ("$a_temp" != "$a_root/${a_host}_*/*") then
	   set a_arch = $a_host
	endif

#       Try to deduce the site name by looking for aipsrc or makedefs file
#       in a subdirectory of the architecture (with possible extension).
#       The first subdirectory is the site name.
        set a_temp = $a_root/$a_arch/*/aipsrc
        if ("$a_temp" == "$a_root/$a_arch/*/aipsrc") then
           set a_temp = $a_root/$a_arch*/*/aipsrc
           if ("$a_temp" == "$a_root/$a_arch*/*/aipsrc") then
              set a_temp = $a_root/$a_arch/*/makedefs
              if ("$a_temp" == "$a_root/$a_arch/*/makedefs") then
                 set a_temp = $a_root/$a_arch*/*/makedefs
              endif
           endif
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
	else
           set a_arch=`echo ${a_arch}_$aips_ext | sed -e '{s/ .*//;s/_.*_/_/;}'`
        endif
        unset aips_ext
        unsetenv aips_ext
     else
        set a_ext = `echo $a_arch | sed -e 's/.*_//'`
        if ("$a_ext" == "$a_arch") then
           set a_temp = $a_root/${a_arch}_*/bin
           set noglob
           set a_temp = `echo $a_temp[1] | awk '{ print $1 }' | awk -F/ '{ print $(NF-1) }'`
           set a_ext = `echo $a_temp | sed -e 's/.*_//'`
           unset noglob
           if ("$a_ext" != "*") set a_arch = ${a_arch}_$a_ext
        endif
     endif

     if ("$a_nonomatch" == "1") unset nonomatch

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
        echo "Warning: $a_root/$a_arch/$a_site does not exist."
     endif

#    Define AIPSPATH.
     setenv AIPSPATH "$a_root $a_arch $a_site $a_host"

#    Update the prompt string.
     cd .

#    Escape pound sign
     set a_old = `echo $a_old | sed -e 's/#/\\#/g'`
     set a_och = `echo $a_och | sed -e 's/#/\\#/g'`
     set a_arch_t = `echo $a_arch | sed -e 's/#/\\#/g'`
     set a_root_t = `echo $a_root | sed -e 's/#/\\#/g'`

#    Reset PATH.
     set a_new = `echo " $PATH " | sed -e 's#::*# #g' -e "s# $a_old/bin # aips_bin #g" -e "s#/aips++/$a_och/#/aips++/$a_arch_t/#g" -e "s# aips_bin # $a_root_t/$a_arch_t/bin #g"`

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


#    Reset LD_LIBRARY_PATH.
     if ("$?LD_LIBRARY_PATH") then
        set a_new = `echo " $LD_LIBRARY_PATH " | sed -e 's#::*# #g' -e "s# $a_old/lib # aips_lib #g" -e "s# aips_lib # $a_root_t/$a_arch_t/lib #g"`

#       Ensure that some AIPS++ lib area got into LD_LIBRARY_PATH.
        echo " $a_new " | grep " $a_root/$a_arch/lib " >& /dev/null
        if ("$status" != 0) set a_new = "$a_root/$a_arch/lib $a_new"

#       Reset it, with sanity check!
        set a_new = `echo $a_new | sed -e 's# #:#g'`
        if ("$a_new" != "") setenv LD_LIBRARY_PATH "$a_new"
     else
        setenv LD_LIBRARY_PATH "$a_root/$a_arch/lib"
     endif


#    Reset MANPATH.
     if ("$?MANPATH") then
        set a_new = `echo " $MANPATH " | sed -e 's#::*# #g' -e "s# $a_old/doc # aips_doc #g" -e "s# aips_doc # $a_root_t/$a_arch_t/doc #g"`

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

# Source possible local AIPS++ initialization files.
  if (-r $a_root/aips++local.csh) then
    source $a_root/aips++local.csh
  endif
  if (-r $a_root/$a_arch/aips++local.csh) then
    source $a_root/$a_arch/aips++local.csh
  endif
  if (-r $a_root/$a_arch/$a_site/aips++local.csh) then
    source $a_root/$a_arch/$a_site/aips++local.csh
  endif
  if (-r $a_root/$a_arch/$a_site/$a_host/aips++local.csh) then
    source $a_root/$a_arch/$a_site/$a_host/aips++local.csh
  endif
  if (-r $HOME/.aips++local.csh) then
     echo "Sourcing personal .aips++local.csh file."
     source $HOME/.aips++local.csh
  endif

# Clean up.
  unset a_arch a_host a_new a_nonomatch a_och a_old a_root a_site a_temp a_arch_t a_root_t
