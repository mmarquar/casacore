//# HostInfo.h: Information about the host that this process is running on.
//# Copyright (C) 1997,1999
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

#include <aips/OS/HostInfo.h>
#include <aips/Utilities/String.h>
#include <aips/Utilities/Assert.h>

#include <unistd.h>
#include <sys/utsname.h>

// Time related includes
#if defined(AIPS_SOLARIS) || defined(_AIX) || defined(AIPS_IRIX)
#include <sys/time.h>
#elif defined(AIPS_OSF)
#include <sys/timers.h>
#else
#include <sys/timeb.h>
#endif

#if defined(AIPS_SOLARIS) && !defined(__CLCC__)
extern "C" { int gettimeofday(struct timeval *tp, void*); };
#endif
#if defined(AIPS_OSF)
extern "C" { int getclock(int clock_type, struct timespec* tp); };
#endif


String HostInfo::hostName()
{
    String retval;
#if defined(AIPS_IRIX)
      // This is a kludge to get around a problem with
      // losing environment variable names on some IRIX machines
      // at NCSA in Urbana IL.
    Char buf[65];
    if (gethostname(buf, 64) >= 0) {
	retval = String(buf);
    }
#else
    struct utsname name;
    if (uname(&name) >= 0) {
	retval = name.nodename;
    }
#endif
    return retval;
}

Int HostInfo::processID()
{
    return getpid();
}


#if defined(AIPS_SOLARIS) && defined(__CLCC__)
Double HostInfo::secondsFrom1970()
{
    struct timeval  tp;
    AlwaysAssert(gettimeofday(&tp) >= 0, AipsError);
    double total = tp.tv_sec;
    total += tp.tv_usec * 0.000001;
    return total;
}
#elif defined(AIPS_SOLARIS) || defined(_AIX) || defined(AIPS_IRIX)
Double HostInfo::secondsFrom1970()
{
    struct timeval  tp;
    struct timezone tz;
    tz.tz_minuteswest = 0;
    AlwaysAssert(gettimeofday(&tp, &tz) >= 0, AipsError);
    double total = tp.tv_sec;
    total += tp.tv_usec * 0.000001;
    return total;
}
#elif defined(AIPS_OSF)
Double HostInfo::secondsFrom1970()
{
  struct timespec tp;
  AlwaysAssert(getclock(TIMEOFDAY,&tp) == 0, AipsError);
  double total = tp.tv_sec;
  total += tp.tv_nsec * 1.e-9;
  return total;
}
#else
Double HostInfo::secondsFrom1970()
{
    struct timeb ftm;
    AlwaysAssert(ftime(&ftm) >= 0, AipsError);
    double total = ftm.time;
    total += ftm.millitm*0.001;
    return total;
}
#endif
