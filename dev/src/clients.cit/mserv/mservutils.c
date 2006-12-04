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


**********************************************************/
#include <stdio.h>
#include <string.h>
#include "quanstrc.h"
#include "mservutils.h"

int classify_packet(seed_fixed_data_record_header* sh)
{

  if(sh->header.seed_record_type != 'D')
  {
    /* I'm guessing on this one. I don't know what type the blockettes */
    /* will be flagged as, but I expect everything else to be = 'D' */
    /* so I'll through all ananomlys into the blockette buffer */

    return(BLOCKETTE);
  }
  else /* Must be a SEED "D' record type */
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

      if ( (sh->dob.blockette_type == 1000) && 
           (strncmp(sh->header.channel_id,"LOG",3) == 0))
      {
        return(COMMENTS);
      }
      else if(sh->deb.blockette_type == 500)
      {
        return(CLOCK_CORRECTION);
      }
      else if ( (sh->deb.blockette_type == 200) ||
		(sh->deb.blockette_type == 201))
      {
        return(DETECTION_RESULT);
      }
      else if( (sh->deb.blockette_type == 300) ||
	       (sh->deb.blockette_type == 310) ||
		(sh->deb.blockette_type == 320) ||
		(sh->deb.blockette_type == 395))
      {
        return(CALIBRATION);
      }
      else if ( (sh->deb.blockette_type == 2000))
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
        fprintf(stderr,"Unknown packet type.\n");
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


int header_to_double_time(seed_fixed_data_record_header* sh,
			  double* out_time)
{
  return(FALSE);
}
