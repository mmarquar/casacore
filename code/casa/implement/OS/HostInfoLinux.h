//# HostInfo_linux.h: Linux specific memory, swap, and CPU code.
//# $Id$

 /*
 **  This is a greatly MODIFIED version of a "top" machine dependent file.
 **  The only resemblance it bears to the original is with respect to the
 **  mechanics of finding various system details. The copyright details
 **  follow.
 **
 **  --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
 **
 **  Top users/processes display for Unix
 **  Version 3
 **
 **  This program may be freely redistributed,
 **  but this entire comment MUST remain intact.
 **
 **  Copyright (c) 1984, 1989, William LeFebvre, Rice University
 **  Copyright (c) 1989 - 1994, William LeFebvre, Northwestern University
 **  Copyright (c) 1994, 1995, William LeFebvre, Argonne National Laboratory
 **  Copyright (c) 1996, William LeFebvre, Group sys Consulting
 **  Copyright (c) 2002, Associated Universities Inc.
 */

#if defined(HOSTINFO_DO_IMPLEMENT) && ! defined(AIPS_HOSTINFO_LINUX_H)
#define AIPS_HOSTINFO_LINUX_H

/*
 *          AUTHOR:       Darrell Schiebel  <drs@nrao.edu>
 *
 * ORIGINAL AUTHORS:      Richard Henderson <rth@tamu.edu>
 *                        Alexey Klimkin    <kad@klon.tme.mcst.ru>
 *
 *
 */

#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#if 0
#include <linux/proc_fs.h>	/* for PROC_SUPER_MAGIC */
#else
#define PROC_SUPER_MAGIC 0x9fa0
#endif

#define PROCFS "/proc"
#define CPUINFO "/proc/cpuinfo"
#define MEMINFO "/proc/meminfo"

#define bytetok(x)	(((x) + 512) >> 10)

class HostMachineInfo {
friend class HostInfo;
  
    HostMachineInfo( );
    void update_info( );

    int valid;
    int cpus;

    int memory_total;
    int memory_used;
    int memory_free;

    int swap_total;
    int swap_used;
    int swap_free;
};

static inline char *
skip_ws(const char *p)
{
    while (isspace(*p)) p++;
    return (char *)p;
}
    
static inline char *
skip_token(const char *p)
{
    while (isspace(*p)) p++;
    while (*p && !isspace(*p)) p++;
    return (char *)p;
}

HostMachineInfo::HostMachineInfo( ) : valid(1)
{
    char buffer[4096+1];
    int fd, len;
    char *p;

    /* make sure the proc filesystem is mounted */
    {
	struct statfs sb;
	if (statfs(PROCFS, &sb) < 0 || sb.f_type != PROC_SUPER_MAGIC)
	{
	    fprintf( stderr, "proc filesystem not mounted on " PROCFS "\n" );
	    valid = 0;
	    return;
	}
    }

    /* get number of CPUs */
    {
	cpus = 0;
	fd = open(CPUINFO, O_RDONLY);
	len = read(fd, buffer, sizeof(buffer)-1);
	close(fd);
	buffer[len] = '\0';
	p = buffer;

	/* be prepared for extra columns to appear by seeking
	   to ends of lines */

	while ( *p ) 
	{
	    if ( ! strncmp( p, "processor", 9 ) ) ++cpus;
	    p = strchr(p, '\n');
	    if ( *p == '\n' ) ++p;
	}
    }

    /* get system total memory */
    {
	fd = open(MEMINFO, O_RDONLY);
	len = read(fd, buffer, sizeof(buffer)-1);
	close(fd);
	buffer[len] = '\0';

	/* be prepared for extra columns to appear be seeking
	   to ends of lines */

	p = strchr(buffer, '\n');
	p = skip_token(p);			/* "Mem:" */
	memory_total = bytetok(strtoul(p, &p, 10));
	p = strchr(p, '\n');
	p = skip_token(p);			/* "Swap:" */
	swap_total = bytetok(strtoul(p, &p, 10));
    }
}

void HostMachineInfo::update_info( )
{
    char buffer[4096+1];
    int fd, len;
    char *p;

    /* get system wide memory usage */
    {
	char *p;

	fd = open(MEMINFO, O_RDONLY);
	len = read(fd, buffer, sizeof(buffer)-1);
	close(fd);
	buffer[len] = '\0';

	/* be prepared for extra columns to appear be seeking
	   to ends of lines */

	p = strchr(buffer, '\n');
	p = skip_token(p);			/* "Mem:" */
	p = skip_token(p);			/* total memory */
	memory_used = bytetok(strtoul(p, &p, 10));
        memory_free = bytetok(strtoul(p, &p, 10));

	p = strchr(p, '\n');
	p = skip_token(p);			/* "Swap:" */
	swap_total = bytetok(strtoul(p, &p, 10));
	swap_used = bytetok(strtoul(p, &p, 10));
	swap_free = bytetok(strtoul(p, &p, 10));
    }
}

#endif
