#ifdef LINUX

 /************************************************************************/
/*  wordorder_from_time_LINUX:						*/
/*	Determine wordorder from the fixed data header date_time field.	*/
/*  return:								*/
/*	wordorder of the record.                                        */
/* This version is modified from it's oroginal  wordoder_from_time      */
/* to run on little -endian comuters by Ilya Dricker ISTI        	*/
/************************************************************************/
int wordorder_from_time_LINUX
   (unsigned char *p)		/* ptr to fixed data time field.	*/
{
    int wordorder, reverse_order;
    unsigned char *cyear = p;
    /* This check ONLY works for dates in the range [1800, ..., 2054].	*/
    if (my_wordorder < 0) get_my_wordorder();
    reverse_order = (my_wordorder == SEED_BIG_ENDIAN)
	 ? SEED_LITTLE_ENDIAN : SEED_BIG_ENDIAN;
    if      ((cyear[0] == 0x07 && cyear[1] >= 0x08) ||
	     (cyear[0] == 0x08 && cyear[1] <  0x07)) wordorder = 1;
    else if ((cyear[1] == 0x07 && cyear[0] >= 0x08) ||
	     (cyear[1] == 0x08 && cyear[0] <  0x07)) wordorder = 0;
    else {
	fprintf (stderr, "Error: Unable to determine wordorder from time\n");
	exit(1);
    }
    return (wordorder);
}

/************************************************************************/
/*  decode_hdr_sdr_LINUX		*/
/*	Decode SDR header stored with each SDR data block,		*/
/*	and return ptr to dynamically allocated DATA_HDR structure.	*/
/*	Fill in structure with the information in a easy-to-use format.	*/
/*	Skip over vol_hdr record, which may be on Quanterra Ultra-Shear	*/
/*	tapes.	                                                        */
/* NOTE: this version uses wordorder_from_time_LINUX (IGD)              */
/*  returns:								*/
/*	DATA_HDR structure.						*/
/************************************************************************/
DATA_HDR *decode_hdr_sdr_LINUX
   (SDR_HDR	*ihdr,		/* input SDR header.			*/
    int		maxbytes)	/* max # bytes in buffer.		*/
{
    char tmp[80];
    DATA_HDR *ohdr;
    BS *bs;			/* ptr to blockette structure.		*/
    char *p;
    char *pc;
    int i, next_seq;
    int seconds, usecs;
    int swapflag;
    int		itmp[2];
    short int	stmp[2];
    unsigned short int ustmp[2];

    herrno=0;
    if (my_wordorder < 0) get_my_wordorder();

    /* Perform data integrity check, and pick out pertinent header info.*/
    if (!(ihdr->data_hdr_ind == DATA_HDR_IND || ihdr->data_hdr_ind == VOL_HDR_IND)) {
	/*  Don't have a DATA_HDR_IND.  See if the entire header is	*/
	/*  composed of NULLS.  If so, print warning and return NULL.	*/
	/*  Some early Quanterras output a spurious block with null	*/
	/*  header info every 16 blocks.  That block should be ignored.	*/
	if (allnull((char *)ihdr, sizeof(SDR_HDR))) {
	    return ((DATA_HDR *)NULL);
	}
	else {
	    herrno = 1;
	    return ((DATA_HDR *)NULL);
	}
    }

    if ((ohdr = new_data_hdr()) == NULL) return (NULL);
    ohdr->record_type = ihdr->data_hdr_ind;
    ohdr->seq_no = atoi (charncpy (tmp, ihdr->seq_no, 6) );

    /* Handle volume header.					    */
    /* Return a pointer to a DATA_HDR structure containing blksize. */
    /* Save actual blockette for later use.			    */
    if (ihdr->data_hdr_ind == VOL_HDR_IND) {
	/* Get blksize from volume header.			    */
	p = (char *)ihdr+8;
	ohdr->blksize = 4096;	/* default tape blksize.	    */
	/* Put volume blockette number in data_type field.	    */
	ohdr->data_type = atoi (charncpy (tmp, p, 3));
	switch (ohdr->data_type) {
	  case 5:
	  case 8:
	  case 10:
	    ohdr->blksize = (int)pow(2.0,atoi(charncpy(tmp,p+11,2)));
	    add_blockette (ohdr, p, ohdr->data_type, 
			   atoi(charncpy(tmp,p+3,4)), my_wordorder, 0);
	    break;
	  default:
	    break;
	}
	return (ohdr);
    }

    /* Determine word order of the fixed record header.			*/
    ohdr->hdr_wordorder = wordorder_from_time_LINUX ((unsigned char *)&(ihdr->time)); /*IGD*/
    ohdr->data_wordorder = ohdr->hdr_wordorder;
    swapflag = (ohdr->hdr_wordorder != my_wordorder);
    charncpy (ohdr->station_id, ihdr->station_id, 5);
    charncpy (ohdr->location_id, ihdr->location_id, 2);
    charncpy (ohdr->channel_id, ihdr->channel_id, 3);
    charncpy (ohdr->network_id, ihdr->network_id, 2);
    trim (ohdr->station_id);
    trim (ohdr->location_id);
    trim (ohdr->channel_id);
    trim (ohdr->network_id);
    ohdr->hdrtime = decode_time_sdr(ihdr->time, ohdr->hdr_wordorder);
    if (swapflag) {
	/* num_samples.	*/
	ustmp[0] = ihdr->num_samples;
	swab2 ((short int *)&ustmp[0]);
	ohdr->num_samples = ustmp[0];
	/* data_rate	*/
	stmp[0] = ihdr->sample_rate_factor;
	stmp[1] = ihdr->sample_rate_mult;
	swab2 ((short int *)&stmp[0]);
	swab2 ((short int *)&stmp[1]);
	ohdr->sample_rate = stmp[0];
	ohdr->sample_rate_mult = stmp[1];
	/* num_ticks_correction. */
	itmp[0] = ihdr->num_ticks_correction;
	swab4 (&itmp[0]);
	ohdr->num_ticks_correction = itmp[0];
	/* first_data	*/
	ustmp[0] = ihdr->first_data;
	swab2 ((short int *)&ustmp[0]); 
	ohdr->first_data = ustmp[0];
	/* first_blockette */
	ustmp[1] = ihdr->first_blockette;
	swab2 ((short int *)&ustmp[1]);
	ohdr->first_blockette = ustmp[1];
    }
    else {
	ohdr->num_samples = ihdr->num_samples;
	ohdr->sample_rate = ihdr->sample_rate_factor;
	ohdr->sample_rate_mult = ihdr->sample_rate_mult;
	ohdr->num_ticks_correction = ihdr->num_ticks_correction;
	ohdr->first_data = ihdr->first_data;
	ohdr->first_blockette = ihdr->first_blockette;
    }

    /*	WARNING - may need to convert flags to independent format	*/
    /*	if we ever choose a different flag format for the DATA_HDR.	*/
    ohdr->activity_flags = ihdr->activity_flags;
    ohdr->io_flags = ihdr->io_flags;
    ohdr->data_quality_flags = ihdr->data_quality_flags;

    ohdr->num_blockettes = ihdr->num_blockettes;
    ohdr->data_type = 0;		/* assume unknown datatype.	*/
    ohdr->pblockettes = (BS *)NULL;	/* Do not parse blockettes here.*/

    if (ohdr->num_blockettes == 0) ohdr->pblockettes = (BS *)NULL;
    else {
	if (! read_blockettes (ohdr, (char *)ihdr)) {
	    free ((char *)ohdr);
	    return ((DATA_HDR *)NULL);
	}
    }

    /*	Process any blockettes that follow the fixed data header.	*/
    /*	If a blockette 1000 exists, fill in the datatype.		*/
    /*	Otherwise, leave the datatype as unknown.			*/
    ohdr->data_type = UNKNOWN_DATATYPE;
    ohdr->num_data_frames = -1;
    if ((bs=find_blockette(ohdr, 1000))) {
	/* Ensure we have proper output blocksize in the blockette.	*/
	BLOCKETTE_1000 *b1000 = (BLOCKETTE_1000 *) bs->pb;
	ohdr->data_type = b1000->format;
	ohdr->blksize = (int)pow(2.0,b1000->data_rec_len);
	ohdr->data_wordorder = b1000->word_order;
    }
    if ((bs=find_blockette(ohdr, 1001))) {
	/* Add in the usec99 field to the hdrtime.			*/
	BLOCKETTE_1001 *b1001 = (BLOCKETTE_1001 *) bs->pb;
	ohdr->hdrtime = add_time (ohdr->hdrtime, 0, b1001->usec99);
	ohdr->num_data_frames = b1001->frame_count;
    }

    /*	If the time correction has not already been added, we should	*/
    /*	add it to the begtime.  Do NOT change the ACTIVITY flag, since	*/
    /*	it refers to the hdrtime, NOT the begtime/endtime.		*/
    ohdr->begtime = ohdr->hdrtime;
    if ( ohdr->num_ticks_correction != 0 && 
	((ohdr->activity_flags & ACTIVITY_TIME_CORR_APPLIED) == 0) ) {
	ohdr->begtime = add_dtime (ohdr->begtime,
				   (double)ohdr->num_ticks_correction * USECS_PER_TICK);
    }
    time_interval2(ohdr->num_samples - 1, ohdr->sample_rate, ohdr->sample_rate_mult,
		  &seconds, &usecs);
    ohdr->endtime = add_time(ohdr->begtime, seconds, usecs);

    /*	Attempt to determine blocksize if current setting is 0.		*/
    /*	We can detect files of either 512 byte or 4K byte blocks.	*/
    if (ohdr->blksize == 0) {
	for (i=1; i< 4; i++) {
	    pc = ((char *)(ihdr)) + (i*512);
	    if (pc - (char *)(ihdr) >= maxbytes) break;
	    if ( allnull ( pc,sizeof(SDR_HDR)) ) continue;
	    next_seq = atoi (charncpy (tmp, ((SDR_HDR *)pc)->seq_no, 6) );
	    if (next_seq == ohdr->seq_no + i) {
		ohdr->blksize = 512;
		break;
	    }
	}
	/* Can't determine the blocksize.  Assume default.		*/
	/* Assume all non-MiniSEED SDR data is in STEIM1 format.	*/
	/* Assume data_wordorder == hdr_wordorder.			*/
	if (ohdr->blksize == 0) ohdr->blksize = (maxbytes >= 1024) ? 4096 : 512;
	if (ohdr->num_samples > 0 && ohdr->sample_rate != 0) {
	    ohdr->data_type = STEIM1;
	    ohdr->num_data_frames = (ohdr->blksize-ohdr->first_data)/sizeof(FRAME);
	    ohdr->data_wordorder = ohdr->hdr_wordorder;
	}
    }

    /* Fill in num_data_frames, since there may not be a blockette 1001.*/
    if (IS_STEIM_COMP(ohdr->data_type) && ohdr->num_samples > 0 && 
	ohdr->sample_rate != 0 && ohdr->num_data_frames < 0) {
	ohdr->num_data_frames = (ohdr->blksize-ohdr->first_data)/sizeof(FRAME);
    }
	
    return (ohdr);
}


#endif

/**********************************************************************************/
/* check_or_swap() swaps the fixed MSEED header and every blockette known by      */
/*  swab_blockettes() from qlib2 into the byte order defined by                   */
/* header_byte_order_suggested  		                                  */
/* If header_byte_order_suggested  is not set to 0 or 1,                          */
/* no byte swapping is conducted     		                                  */
/* If header_byte_order_suggested is the same as the existing hdr_order,          */
/* no byte swapping is done                                                       */
/* In all other cases, the byte order in the header is changed to the opposite	  */
/* check_or_swap() swaps the fixed blockette() itself calls recursive procedure   */
/*    do_blockettes()  to handle recursively the blockettes                       */
/* Ilya Dricker ISTI 03/31/00						          */ 	
/*--------------------------------------------------------------------------------*/
/* In addition to the byteswapping I added a nasty deal of renaming       */
/* station names for Infrasound array IS26 in German y to this routine    */
/* Renaming only done if the compile flaf IS26 is defined in the Makefile */
/*		Oct. 19, 2000	IGD	                                  */						
/**************************************************************************/

int check_or_swap (char *block,  int header_byte_order_suggested,  int hdr_order,  int comp_order)	{

SDR_HDR* sdr_hdr;
BLOCKETTE_HDR* bl_hdr;
short next_blockette, next_blockette_type, next_blockette_next;

sdr_hdr = (SDR_HDR *) (block);
#ifdef IS26  /* We are renaming I26x stations into I26hx */

  if (sdr_hdr->station_id[4] != 'H')	{
	sdr_hdr->station_id[4]=sdr_hdr->station_id[3];
	sdr_hdr->station_id[3]='H';
  }

#endif
	if (header_byte_order_suggested < 0 ||  header_byte_order_suggested > 1) /* We are not asked for any byte checking */
		return 0;
	if (header_byte_order_suggested == hdr_order)	/*We are all set, the byte order is already correct */
		return 0;

/* If we are here, we need to swap the byte order in the MSEED header */



if (hdr_order == comp_order) 		/* We understand the variables in sdr_hdr */
	next_blockette = sdr_hdr->first_blockette;
else					/*we do not understand the variables in sdr_hdr */
	next_blockette = flip2(sdr_hdr->first_blockette);

/* Now swap bytes in a fixed header */
sdr_hdr->time.day=flp2(sdr_hdr->time.day);
sdr_hdr->time.year=flp2(sdr_hdr->time.year);
sdr_hdr->time.ticks=flp2(sdr_hdr->time.ticks);
sdr_hdr->num_samples  =  flp2(sdr_hdr->num_samples);
sdr_hdr->sample_rate_factor = flp2(sdr_hdr->sample_rate_factor);
sdr_hdr->sample_rate_mult = flp2(sdr_hdr->sample_rate_mult);
sdr_hdr->num_ticks_correction = flp4(sdr_hdr->num_ticks_correction);
sdr_hdr->first_data   = flp2(sdr_hdr->first_data);
sdr_hdr->first_blockette  = flp2(sdr_hdr->first_blockette);

/* and switch to the blockettes */
do_blockettes(block, next_blockette, comp_order, hdr_order);
return 0;
}


/********************************************************************************************/
/* WARNING: this is a recursive procedure                                                                                                             */
/* do blockettes() recursively reads blockettes in the block until the last blockette: (blockette->next == 0 )  */
/* The second condition to leave the recursive procedure is if the blockette type is not in the list 	       */
/* On the way do_blockette() callse swab_blockettes() from qlib2 which does byte swapping                           */
/* Note that if you call do_blockettes, you already know that you want to byte-swap the header                       */
/* Ilya Dricker ISTI 03/31/00                                                                                                                                       */
/*******************************************************************************************/

int do_blockettes(char *block, short next_blockette, int comp_order, int hdr_order)	{
short next_blockette_type;
short next_blockette_next;
short blockette_length;
short nb;
BLOCKETTE_HDR* bl_hdr;
BLOCKETTE_1000* b1000;

	if (next_blockette == 0)	
		return(0);

	bl_hdr = (BLOCKETTE_HDR*)(block+next_blockette) 	; /*the header of our current blockette */
	if (comp_order != hdr_order)	{   /* If we cannot understand the header */
		next_blockette_type = flp2 (bl_hdr->type);
		next_blockette_next =  flp2 (bl_hdr->next);
	}
	else	{ 
		next_blockette_type =  bl_hdr->type;
		next_blockette_next =  bl_hdr->next;   
	}                   
	nb = next_blockette_type;  /* If it is not a blockette in the list below , get out ASAP ! */
	if  (!(nb == 1000 || nb == 1001  || nb == 100 || nb == 200 ||  nb == 201 ||  nb == 300 ||  nb == 310  ||  nb == 320 \
		 ||  nb == 390 ||  nb == 395 ||  nb == 405 ||  nb == 500 || nb == 2000) ){
		fprintf(stderr, "WARNING: unsupported blockette %d is not byte-swapped\n", nb);
		return(-1);
	}
	if (next_blockette_next == 0)   /*no next blockette and there is a good chance the blockette is not 300,310,320, which require exact length*/
		blockette_length = 8;
	else
		blockette_length = next_blockette_next - next_blockette;   
	if(next_blockette_type == 1000)	{ /* set the correct byte order flag*/
		b1000 = (BLOCKETTE_1000*)(block+next_blockette );
                  	if (hdr_order == 0) /*Linux order */
		           b1000->word_order = '\001';
		else 
		            b1000->word_order = '\000';
	}	
	if (swab_blockette (next_blockette_type, (char *) (block + next_blockette), blockette_length) < 0)	{
		fprintf(stderr, "WARNING: blockette %d was not byte-swapped properly\n", next_blockette_type);
		return(-1);
	}
	do_blockettes(block, next_blockette_next, comp_order, hdr_order);
return(0);
}


/* flip short and integer */

short flp2( short shToFlip ) {

 short shSave1, shSave2;

        shSave1 = ((shToFlip & 0xFF00) >> 8);
        shSave2 = ((shToFlip & 0x00FF) << 8);
        return( shSave1 | shSave2 );
}

int flp4( int iToFlip ) {
int iSave1, iSave2, iSave3, iSave4;

        iSave1 = ((iToFlip & 0xFF000000) >> 24);
        iSave2 = ((iToFlip & 0x00FF0000) >> 8);
        iSave3 = ((iToFlip & 0x0000FF00) << 8);
        iSave4 = ((iToFlip & 0x000000FF) << 24);
        return( iSave1 | iSave2 | iSave3 | iSave4 );
}
