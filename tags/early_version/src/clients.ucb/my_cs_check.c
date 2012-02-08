/*   Client Server Access Library Routines
     Copyright 1994-1996 Quanterra, Inc.
     Written by Woodrow H. Owens

Edit History:
   Ed Date      By  Changes
   -- --------- --- ---------------------------------------------------
    0 14 Mar 94 WHO Derived from client.c test program.
    1 31 Mar 94 WHO Client data & blockettes merged.
    2  7 Apr 94 WHO Interface to servers tightened up.
    3  8 Apr 94 WHO Initialize command output buffers.
    4  9 Apr 94 WHO cs_all renamed to cs_setup and station name added.
    5 18 Apr 94 WHO "stuff.h" added since dtime was moved there from service.h
    6 30 May 94 WHO In cs_svc, set curclient->status to CSCR_DIED if that is
                    the status to be returned (DSN).
    7  9 Jun 94 WHO Cleanup to avoid warnings.
    8  9 Aug 94 WHO Put protection against improbable duplicate detach (DSN).
    9 14 oct 94 jms use "nanosleep" if NANOSLEEP
   10  3 Nov 94 WHO Use SOLARIS2 definition to use nanosleep/sleep instead of usleep.
   11 27 Feb 95 WHO Start of conversion to run on OS9.
   12 29 May 96 WHO Somebody slipped in a change from "getuid" to
                    "geteuid" without updating change list.
   13  7 Jun 96 WHO Don't assume a good return from "kill" is zero,
                    check against -1.
   14  4 Dec 96 WHO Fix sels[CHAN] not being initialized.
   15 11 Jun 97 WHO Fix Solaris2/OSK conditionals for sleeping.e
*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#ifndef _OSK
#include <termio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#else
#include <stdlib.h>
#endif
#include <signal.h>
#include "pascal.h"
#include "dpstruc.h"
#include "service.h"
#include "cfgutil.h"
#include "stuff.h"
#ifdef _OSK
#include "os9stuff.h"
#endif
#ifdef SOLARIS2
#include <time.h>
#endif

#define	debug(flag)	(flag & debug_flag)
#define	DEBUG_STATUS	1
#define	DEBUG_POLL	2
#define	DEBUG_CONFIG	4
#define	DEBUG_CHECK	8
#define	DEBUG_MALLOC	128

#define	PROC		"my_cs_check"
#define	WAIT		10

extern int debug_flag;			/* Debugging flag.		*/
extern FILE *info;			/* Output FILE for info.	*/

byte my_cs_check (pclient_struc client, short station_number, double now)
{
    pclient_station curclient ;
    pserver_struc srvr ;
      
    curclient = (pclient_station) ((long) client + client->offsets[station_number]) ;

    if (debug(DEBUG_CHECK)) {
	fprintf (info, "%s: status = %d\n", PROC, curclient->status);
	fprintf (info, "%s: now = %.2lf, last_attempt = %.2lf, diff = %.2lf\n",
		 PROC, now, curclient->last_attempt, now - curclient->last_attempt);
	fflush (info);
    }
    if (curclient->status != CSCR_GOOD)
    {
	if ((curclient->last_attempt + WAIT) < now)
	{
	    if (debug(DEBUG_CHECK)) {
		fprintf (info, "%s: performing link and attach\n", PROC);
		fflush (info);
	    }
	    curclient->last_attempt = now ;
	    if (curclient->base == (pserver_struc) NOCLIENT)
		cs_link (client, station_number, FALSE) ;
	    if (curclient->base != (pserver_struc) NOCLIENT)
		cs_attach (client, station_number) ;
	}
    }

    if ((curclient->status == CSCR_GOOD) land 
	(curclient->base != (pserver_struc) NOCLIENT))
    {
	if (debug(DEBUG_CHECK)) {
	    fprintf (info, "%s: have base\n", PROC);
	    fflush (info);
	}
	srvr = curclient->base ;
	if (curclient->servcode != srvr->servcode)
	{
	    curclient->servcode = srvr->servcode ;
	    curclient->next_data = 0 ;
	    curclient->seqdbuf = CSQ_FIRST ;
	    cs_attach (client, station_number) ;
	    curclient->status = CSCR_CHANGE ;
	}
	else
	{
	    if (curclient->next_data < srvr->next_data) {
		if (debug(DEBUG_CHECK)) {
		    fprintf (info, "%s: internal return 1 status = %d\n", PROC, CSCR_GOOD);
		    fflush (info);
		}
		return CSCR_GOOD ;
	    }
	    if ((curclient->last_attempt + WAIT) < now)
	    {
		curclient->last_attempt = now ;
		if ((client->client_uid == srvr->server_uid) 
		    land (kill(srvr->server_pid, 0) == ERROR))
		{
		    curclient->status = CSCR_DIED ;
		    shmdt ((pchar)srvr) ;
		    curclient->base = (pserver_struc) NOCLIENT ;
		}
		else {
		    if (debug(DEBUG_CHECK)) {
			fprintf (info, "%s: internal return 2 status = %d\n", PROC, CSCR_GOOD);
			fflush (info);
		    }
		    return CSCR_GOOD ;
		}
	    }
	    else {
		if (debug(DEBUG_CHECK)) {
		    fprintf (info, "%s: internal return 3 status = %d\n", PROC, CSCR_NODATA);
		    fflush (info);
		}
		return CSCR_NODATA ;
	    }
	}
    }

    if (debug(DEBUG_CHECK)) {
	fprintf (info, "%s: final return status = %d\n", PROC, curclient->status);
	fflush (info);
    }
    return curclient->status ;
}
