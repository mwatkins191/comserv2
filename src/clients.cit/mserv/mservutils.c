/***********************************************************

File Name : 
        mservutils.c


Programmer:
        Phil Maechling

Description:
        These are mserv utilities. They should isolate certain routines
        which may change from the primary mserv and mcomlink code.

Creation Date:
        22 Feb 1999


Usage Notes:



Modification History:
Ver   Date       WHO	What
----------------------------------------------------------------------
   2 24 Aug 2007 DSN	Ported to little_endian systems.
   3 04 Dec 2010 DSN	Added function to generate nepoch from SEED time.

**********************************************************/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>
#include "quanstrc.h"
#include "mservutils.h"

int classify_packet(seed_fixed_data_record_header* sh)
{

  /*  WARNING - ASSUMPTION - ASSUME RECORD IS BIG_ENDIAN */

  if (! (sh->header.seed_record_type == 'D' || 
	 sh->header.seed_record_type == 'Q' || 
	 sh->header.seed_record_type == 'R' ) )
  {
    /* I'm guessing on this one. I don't know what type the blockettes */
    /* will be flagged as, but I expect everything else to be = 'D' */
    /* so I'll through all ananomlys into the blockette buffer */

    return(BLOCKETTE);
  }
  else /* Must be a SEED "D', 'R', or 'Q' record type */
  {

    /* Test for the data only blockette encoding */
    /* A value of 0 indicates a non-waveform block */

    if( (sh->dob.encoding_format == 10) ||
        (sh->dob.encoding_format == 11) ||
        (sh->dob.encoding_format == 19) )
    {
      /* I will only return a single blockette type */
      /* all types go into the DATAQ */

      return(RECORD_HEADER_1);
    }
    else if (sh->dob.encoding_format == 0)
    {

      if ( (ntohs(sh->dob.blockette_type) == 1000) && 
           (strncmp(sh->header.channel_id,"LOG",3) == 0))
      {
        return(COMMENTS);
      }
      else if(ntohs(sh->deb.blockette_type) == 500)
      {
        return(CLOCK_CORRECTION);
      }
      else if ( (ntohs(sh->deb.blockette_type) == 200) ||
		(ntohs(sh->deb.blockette_type) == 201))
      {
        return(DETECTION_RESULT);
      }
      else if( (ntohs(sh->deb.blockette_type) == 300) ||
	       (ntohs(sh->deb.blockette_type) == 310) ||
		(ntohs(sh->deb.blockette_type) == 320) ||
		(ntohs(sh->deb.blockette_type) == 395))
      {
        return(CALIBRATION);
      }
      else if ( (ntohs(sh->deb.blockette_type) == 2000))
      {
        return(BLOCKETTE);
      }

      else if((sh->header.activity_flags && 
		SEED_ACTIVITY_FLAG_END_EVENT) > 0)
      {
        return(END_OF_DETECTION);
      }
      else
      {
        fprintf(stderr,"Unknown packet type. Encoding format blockette_type = %d\n", sh->deb.blockette_type);
        return(BLOCKETTE);
      }
    }
    else
    {
      fprintf(stderr,"Unknown data only blockette encoding d%\n",
			sh->dob.encoding_format);
      return(BLOCKETTE);
    } 
  }
}


#define	IS_LEAP(yr)	( yr%400==0 || (yr%4==0 && yr%100!=0) )
double header_to_double_time(seed_fixed_data_record_header* sh)
{
    seed_time_struc *seedtime = &(sh->header.starting_time);
    seed_time_struc st;
    int seconds;
    double dseconds;
    int year = 1970;
    int days = 0;
    
    /* Convert SEED time from network byte order to computer byte order. */
    st.yr = ntohs(seedtime->yr);
    st.jday = ntohs(seedtime->jday);
    st.hr = seedtime->hr;
    st.minute = seedtime->minute;
    st.seconds = seedtime->seconds;
    st.tenth_millisec = ntohs(seedtime->tenth_millisec);

    /* WARNING - ASSUMPTION - ASSUME RECORD IS BIG_ENDIAN */
    /* WARNING- Assume SEED time is >= 1980. This is OK for real-time code. */
    while (year < st.yr) {
	days += 365 + (IS_LEAP(year));
	year++;
    }
    days += (st.jday-1);
    seconds = (days*86400) + (st.hr*3600) + (st.minute*60) + st.seconds;
    dseconds = seconds + ((double)st.tenth_millisec)/10000;
    return (dseconds);
}
