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

#if defined(HOSTINFO_DO_IMPLEMENT) && ! defined(AIPS_HOSTINFO_IRIX_H)
#define AIPS_HOSTINFO_IRIX_H

/*
 *          AUTHOR:       Darrell Schiebel  <drs@nrao.edu>
 *
 * ORIGINAL AUTHORS:      Sandeep Cariapa   <cariapa@sgi.com>
 *                        Larry McVoy       <lm@sgi.com>
 *                        John Schimmel     <jes@sgi.com>
 *                        Ariel Faigon      <ariel@sgi.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/swap.h>
#include <sys/sysmp.h>
#include <sys/sysinfo.h>

#define pagetok(pages) ((((uint64_t) pages) * pagesize) >> 10)

class HostMachineInfo {
friend class HostInfo;

    HostMachineInfo( );
    void update_info( );

    int valid;
    int cpus;

    int swap_total;
    int swap_used;
    int swap_free;

    int memory_total;
    int memory_used;
    int memory_free;

    int pagesize;

};

HostMachineInfo::HostMachineInfo( )  : valid(1) {

	pagesize = getpagesize();

	if ((cpus = sysmp(MP_NPROCS)) == -1) {
		perror("sysmp(MP_NPROCS)");
		valid = 0;
		return;
	}

	struct rminfo	realmem;
	if (sysmp(MP_SAGET, MPSA_RMINFO, &realmem, sizeof(realmem)) == -1) {
		perror("sysmp(MP_SAGET,MPSA_RMINFO, ...)");
		valid = 0;
		return;
	}

	memory_total = pagetok(realmem.physmem);
}

void HostMachineInfo::update_info( ) {
	int		i;
	struct rminfo	realmem;
	struct sysinfo	sysinfo;
	off_t		fswap;		/* current free swap in blocks */
	off_t		tswap;		/* total swap in blocks */

	swapctl(SC_GETFREESWAP, &fswap);
	swapctl(SC_GETSWAPTOT, &tswap);

	if (sysmp(MP_SAGET, MPSA_RMINFO, &realmem, sizeof(realmem)) == -1) {
		perror("sysmp(MP_SAGET,MPSA_RMINFO, ...)");
		valid = 0;
		return;
	}

	memory_free = pagetok(realmem.freemem);
	memory_used =  memory_total - memory_free;
	swap_total = tswap / 2;
	swap_free = fswap / 2;
	swap_used = swap_total - swap_free;
}

#endif
