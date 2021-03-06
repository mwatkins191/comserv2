/*   Server comlink protocol file
     Copyright 1994-1999 Quanterra, Inc.
     Written by Woodrow H. Owens

Edit History:
   Ed Date      By  Changes
   -- --------- --- ---------------------------------------------------
    0 23 Mar 94 WHO First created.
    1  9 Apr 94 WHO Command Echo processing added.
    2 15 Apr 94 WHO Commands done.
    3 30 May 94 WHO If rev 1 or higher ultra record received, set comm_mask.
    4  9 Jun 94 WHO Consider a "busy" client still active if it is a
                    foreign client, without trying to do a Kill (0).
                    Cleanup to avoid warnings.
    5 11 Jun 94 WHO Martin-Marietta support added.
    6  9 Aug 94 WHO Set last_good and last_bad fields in linkstat.
    7 11 Aug 94 WHO Add support for network.
    8 25 Sep 94 WHO Allow receiving RECORD_HEADER_3.
    9  3 Nov 94 WHO Use SOLARIS2 definition to alias socket parameter.
   10 13 Dec 94 WHO Remove record size in seedheader function, COMSERV always
                    uses 512 byte blocks.
   11 16 Jun 95 WHO Don't try to open closed network connection until
                    netdly_cnt reaches netdly. Clear netto_cnt timeout
                    counter when a packet is received.
   12 20 Jun 95 WHO Updates due to link adj and link record packets. Transmitted
                    packet circular buffer added to accomodate grouping.
   13 15 Aug 95 WHO Set frame_count for Q512 and MM256 data packets.
   14  2 Oct 95 WHO Implement new link_pkt/ultra_req handshaking protocol.
   15 28 Jan 96 DSN Update check_input to handle unexpected characters better.
                    Correctly assign state when unexpected character is found.
   16  3 Jun 96 WHO Start of conversion to OS9
   17  4 Jun 96 WHO Comparison for dbuf.seq being positive removed, seq
                    is unsigned. cli_addr, network, and station made external.
   18  7 Jun 96 WHO Check result of "kill" with ERROR, not zero.
   19 13 Jun 96 WHO Adjust seedname and location fields for COMMENTS.
   20  3 Aug 96 WHO If anystation flag is on, don't check station, and show what
                    station came from. If noultra is on, don't poll for ultra
                    packets.
   21  7 Dec 96 WHO Add support for Blockettes and UDP.
   22 11 Jun 97 WHO Clear out remainder of packets that are received with
                    less than 512 bytes. Convert equivalent of header_flag
                    in blockette packet to seed sequence number.
   23 27 Jul 97 WHO Handle FLOOD_PKT.
   24 17 Oct 97 WHO Add VER_COMLINK
   25 23 Dec 98 WHO Use link_retry instead of a fixed 10 seconds for
                    polling for link packets (VSAT).
   26 22 Feb 99 PJM Modified comlink to create mcomlink.c. This allows
                    multicast reception by comlink.
   27 09 Aug 02 PAF cleaned up verbosity messages
   28 01 Dec 05 PAF changed printf to LogMessage() calls
   29 24 Aug 07 DSN Ported to little_endian systems.
		    Removed unused code.
   30 12 Mar 09 DSN Another fix for reference through NULL pointer for dead client.

    The changes from comlink.c to mcomlink.c were done to support reception
    of packets on a multicast interface by comserv. The packets are to be
    the 512byte SEED packets that are available to comserv clients. The
    conversion done by the standard comlink from qsl format to SEED is not
    done here. Various parts of the protocal done by comserv are removed,
    because in the multicast configuration, no acknowledgements are used.
    The def MSERV is used to comment out unused blocks. This should be
    defined near the start of this file.
    The intention what to minimize the number of modification to the original
    comlink.c to minimize the size effects of changes to the code.

*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#ifndef _OSK
#include <termio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#else
#include <stdlib.h>
#include <sgstat.h>
#include <sg_codes.h>
#include "os9inet.h"
#endif
#include "quanstrc.h"
#include "stuff.h"
#include "service.h"
#include "cfgutil.h"
#include "server.h"
#include "timeutil.h"
#include "logging.h"
#ifdef _OSK
#include "os9stuff.h"
#endif

#ifdef	LINUX
#include "unistd.h"
#ifndef FIRSTDATA       /* IGD 03/05/01 Add */
#define FIRSTDATA 56
#endif
#endif

/* Include routines which extract information from the seed header */

#include "mservutils.h"

/* Define MSERV to use in excluding code in this file */

#define MSERV 1



short VER_COMLINK = 30 ;

extern seed_net_type network ;
extern complong station ;
long reconfig_on_err = 25 ;
boolean anystation = FALSE ;

extern boolean noultra ;
extern pchar src, srcend, dest, destend, term ;
extern unsigned char sbuf[BLOB] ;
extern DA_to_DP_buffer dbuf ;
extern byte last_packet_received ;
extern byte lastchar ;
extern int path ;
extern int sockfd ;
extern struct sockaddr_in cli_addr ;
extern int upmemid ;
extern DP_to_DA_buffer temp_pkt ;
extern tcrc_table crctable ;
extern tclients clients[MAXCLIENTS] ;
extern pserver_struc base ;

extern byte inphase ;
extern byte upphase ;
extern long seq ;
extern long comm_mask ;
extern boolean verbose ;
extern boolean rambling ;
extern boolean insane ;
extern linkstat_rec linkstat ;
extern boolean detavail_ok ;
extern boolean detavail_loaded ;
extern boolean first ;
extern boolean firstpacket ;
extern boolean seq_valid ;
extern boolean xfer_down ;
extern boolean xfer_down_ok ;
extern boolean xfer_up_ok ;
extern boolean map_wait ;
extern boolean ultra_seg_empty ;
extern boolean override ;
extern boolean follow_up ;
extern boolean serial ;
extern boolean udplink ;
extern boolean notify ;
extern short ultra_percent ;
extern unsigned short lowest_seq ;
extern short linkpoll ;
extern long con_seq ;
extern long netdly_cnt ;
extern long netdly ;
extern long netto_cnt ;
extern long grpsize ;
extern long grptime ;
extern long link_retry ;
extern double last_sent ;
extern short combusy ;
extern byte cmd_seq ;
extern link_record curlink ;
extern string59 xfer_destination ;
extern string59 xfer_source ;
extern byte ultra_seg[14] ;
extern byte detavail_seg[14] ;
extern seg_map_type xfer_seg ;
extern unsigned short seg_size ;
extern unsigned short xfer_size ;
extern unsigned short xfer_up_curseg ;
extern unsigned short xfer_bytes ;
extern unsigned short xfer_total ;
extern unsigned short xfer_last ;
extern unsigned short xfer_segments ;
extern unsigned short cal_size ;
extern unsigned short used_size ;
extern unsigned short ultra_size ;
extern unsigned short detavail_size ;
extern cal_record *pcal ;
extern short sequence_mod ;
extern short vcovalue ; 
extern short xfer_resends ;
extern short mappoll ;
extern short sincemap ;
extern short down_count ;
extern short minctr ;
extern short txwin ;
extern download_struc *pdownload ;     
extern ultra_type *pultra ;
extern tupbuf *pupbuf ;
DP_to_DA_msg_type replybuf ;
extern DP_to_DA_msg_type gmsg ;
 
extern string3 seed_names[20][7] ;
extern location_type seed_locs[20][7] ;
 
static byte this_packet ;

static int maxbytes = 0 ;

long julian (time_array *gt) ;
pchar time_string (double jul) ;
tring_elem *getbuffer (short qnum) ;
boolean bufavail (short qnum) ;
boolean checkmask (short qnum) ;
double seedheader (header_type *h, seed_fixed_data_record_header *sh) ;
double seedblocks (seed_record_header *sl, commo_record *log) ;
signed char encode_rate (short rate) ;
void seedsequence (seed_record_header *seed, long seq) ;

static short seq_seen[DEFAULT_WINDOW_SIZE] = { 0,0,0,0,0,0,0,0};

typedef struct sockaddr *psockaddr ;

typedef struct
  {
    byte cmd ;              /* command number */
    byte ack ;              /* acknowledgement of packet n */
    byte dpseq ;            /* sequence for DP to DA commands */
    char leadin ;           /* leadin character */
    short len ;             /* Length of message */
    DP_to_DA_msg_type buf ; /* buffer for the raw message */
  } txbuf_type ;
  
static short nextin = 0 ;
static short nextout = 0 ;
static txbuf_type txbuf[64] ;

pchar seednamestring (seed_name_type *sd, location_type *loc) ;

  void gcrcinit (void)
    {
      short count, bits ;
      long tdata, accum ;

      for (count = 0 ; count < 256 ; count++)
        {
          tdata = ((long) count) << 24 ;
          accum = 0 ;
          for (bits = 1 ; bits <= 8 ; bits++)
            {
              if ((tdata ^ accum) < 0)
                  accum = (accum << 1) ^ CRC_POLYNOMIAL ;
                else
                  accum = (accum << 1) ;
              tdata = tdata << 1 ;
            }
          crctable[count] = accum ;
        }
    }

  long gcrccalc (pchar b, short len)
    {
      complong crc ;

      crc.l = 0 ;
      while (len-- > 0)
        crc.l = (crc.l << 8) ^ crctable[(crc.b[0] ^ *b++) && 255] ;
      return crc.l ;
    }

  unsigned short checksum (pchar addr, short size)
    {
      unsigned short ck ;

      ck = 0 ;
      while (size-- > 0)
          ck = ck + ((short) *addr++ & 255) ;
      return ck ;
    }

  void send_window (void)
    {
      short i, len ;
      int numwrit ;
      pchar ta, tstr ;
      char transmit_buf[2 * sizeof(DP_to_DA_buffer) + 2] ;
      complong ltemp ;
      byte b ;
      txbuf_type *cur ;
      return;
    }

  void send_tx_packet (byte nr, byte cmd_var, DP_to_DA_msg_type *msg)
    {
      txbuf_type *cur ;
      return; 
    }
    
  void send_ack (void)
    {
      DP_to_DA_msg_type msg ;
      return;
    }

  void request_ultra (void)
    {
      DP_to_DA_msg_type mmsg ;
      return;
    }

  void request_link (void)
    {
      DP_to_DA_msg_type mmsg ;
      return;
    }

  void request_map (void)
    {
      DP_to_DA_msg_type mmsg ;
      return;
    }

  void reconfigure (boolean full)
    {
      short i ;
    }

  void clearmsg (DP_to_DA_msg_type *m)
    {
      m->mmc.dp_seq = 0 ;
      m->mmc.cmd_parms.param0 = 0 ;
      m->mmc.cmd_parms.param1 = 0 ;
      m->mmc.cmd_parms.param2 = 0 ;
      m->mmc.cmd_parms.param3 = 0 ;
      m->mmc.cmd_parms.param4 = 0 ;
      m->mmc.cmd_parms.param5 = 0 ;
      m->mmc.cmd_parms.param6 = 0 ;
      m->mmc.cmd_parms.param7 = 0 ;
    }
    
  boolean checkbusy (void) /* returns TRUE if client is foreign or alive */
    {
      return (combusy != NOCLIENT) && 
         ((clients[combusy].client_address->client_uid != base->server_uid) ||
#ifdef _OSK
          (kill(clients[combusy].client_pid, SIGWAKE) != ERROR)) ;
#else
          (kill(clients[combusy].client_pid, 0) != ERROR)) ;
#endif
    }

  void do_abort (void)
    {
      DP_to_DA_msg_type mmsg ;
      comstat_rec *pcom ;
      download_result *pdr = NULL ;

      if (xfer_up_ok)
          {
            xfer_up_ok = FALSE ;
            clearmsg (&mmsg) ;
            mmsg.us.cmd_type = UPLOAD ;
            mmsg.us.dp_seq = cmd_seq ;
            mmsg.us.return_map = FALSE ;
            mmsg.us.upload_control = ABORT_UPLOAD ;
            send_tx_packet (0, UPLOAD, &mmsg) ;
            clearmsg (&mmsg) ;
            mmsg.us.cmd_type = UPLOAD ;
            mmsg.us.dp_seq = cmd_seq ;
            mmsg.us.return_map = FALSE ;
            mmsg.us.upload_control = ABORT_UPLOAD ;
            send_tx_packet (0, UPLOAD, &mmsg) ;
            shmdt((pchar)pupbuf) ;
            if (checkbusy ())
                {
                  pcom = (pvoid) clients[combusy].outbuf ;
                  if (upmemid != NOCLIENT)
                      {
                        shmctl(upmemid, IPC_RMID, NULL) ;
                        upmemid = NOCLIENT ;
                      }
                  if (pcom != NULL && pcom->completion_status == CSCS_INPROGRESS)
                      pcom->completion_status = CSCS_ABORTED ;
                  combusy = NOCLIENT ;
                }
            xfer_size = 0 ;
          } ;
      if (xfer_down_ok)
          {
            xfer_down_ok = FALSE ;
            clearmsg (&mmsg) ;
            mmsg.scs.cmd_type = DOWN_ABT ;
            send_tx_packet (0, DOWN_ABT, &mmsg) ;
            clearmsg (&mmsg) ;
            mmsg.scs.cmd_type = DOWN_ABT ;
            send_tx_packet (0, DOWN_ABT, &mmsg) ;
            if (pdownload != NULL)
                shmdt((pchar)pdownload) ;
            if (checkbusy ())
                {
                  pcom = (pvoid) clients[combusy].outbuf ;
                  if (pcom) pdr = (pvoid) &pcom->moreinfo ;
                  if (pdr->dpshmid != NOCLIENT)
                      {
                        shmctl(pdr->dpshmid, IPC_RMID, NULL) ;
                        pdr->dpshmid = NOCLIENT ;
                      }
                  if (pcom != NULL && pcom->completion_status == CSCS_INPROGRESS)
                      pcom->completion_status = CSCS_ABORTED ;
                  combusy = NOCLIENT ;
                }
          }
    }

  void next_segment (void)
    {
      DP_to_DA_msg_type mmsg ;
      unsigned short i ;
      unsigned short h ;
      pchar p1, p2 ;
      boolean allsent ;
      unsigned short off, cnt ; 
      comstat_rec *pcom ;
      upload_result *pupres ;

      clearmsg (&mmsg) ;
      mmsg.scs.cmd_type = UPLOAD ;
      mmsg.us.return_map = FALSE ;
      mmsg.us.upload_control = SEND_UPLOAD ;
      if (checkbusy ())
          {
            pcom = (pvoid) clients[combusy].outbuf ;
            pupres = (pvoid) &pcom->moreinfo ;
          }
        else
          {
            do_abort () ;
            return ;
          }
      if (upphase == SENDING_UPLOAD)
          {
            i = xfer_up_curseg ;
            allsent = TRUE ;
            while (i < xfer_segments)
              {
                if (((xfer_seg[i / 8]) & ((byte) (1 << (i % 8)))) == 0)
                    {
                      allsent = FALSE ;
                      off = seg_size * i ;
                      mmsg.us.up_union.up_send.byte_offset = off ;
                      xfer_up_curseg = i + 1 ;
                      mmsg.us.up_union.up_send.seg_num = xfer_up_curseg ;
                      if (((unsigned int) off + (unsigned int) seg_size) >= (unsigned int) xfer_size)
                          {
                            cnt = xfer_size - off ;
                            xfer_up_curseg = 0 ;
                            mmsg.us.return_map = TRUE ;
                            mappoll = 30 ;
                            sincemap = 0 ;
                            upphase = WAIT_MAP ;
                            if (xfer_resends < 0)
                                xfer_resends = 0 ;
                          }
                        else
                          {
                            cnt = seg_size ;
                            if (++sincemap >= 10)
                                {
                                  mmsg.us.return_map = TRUE ;
                                  mappoll = 30 ;
                                  sincemap = 0 ;
                                  upphase = WAIT_MAP ;
                                }
                          } ;
                      mmsg.us.up_union.up_send.byte_count = cnt ;
                      p1 = (pchar) ((long) pupbuf + off) ;
                      p2 = (pchar) &mmsg.us.up_union.up_send.bytes ;
                      memcpy(p2, p1, cnt) ;
                      xfer_bytes = xfer_bytes + cnt ;
                      if (xfer_bytes > xfer_total)
                          xfer_bytes = xfer_total ;
                      if (xfer_resends >= 0)
                          xfer_resends++ ;
                      pupres->bytecount = xfer_bytes ;
                      pupres->retries = xfer_resends ;
                      send_tx_packet (0, UPLOAD, &mmsg) ;
                      break ;
                    }
                i++ ;
              }
            if (allsent)
                for (i = 0 ; i < xfer_segments ; i++)
                  if (((xfer_seg[i / 8]) & ((byte)(1 << (i % 8)))) == 0)
                      {
                        xfer_up_curseg = 0 ;
                        upphase = WAIT_MAP ;
                        request_map () ;
                        return ;
                      }
            if (allsent)
                {
                  xfer_up_ok = FALSE ;
                  upphase = UP_IDLE ;
                  mmsg.us.upload_control = UPLOAD_DONE ;
                  xfer_size = 0 ;
                  shmdt((pchar)pupbuf) ;
                  pcom->completion_status = CSCS_FINISHED ;
                  combusy = NOCLIENT ;
                  send_tx_packet (0, UPLOAD, &mmsg) ;
                }
          }
    }

  void process_upmap (void)
    {
      comstat_rec *pcom ;
      
      mappoll = 0 ;
      switch (upphase)
        {
          case WAIT_CREATE_OK :
            {
              if (dbuf.data_buf.cu.upload_ok)
                  {
                    upphase = SENDING_UPLOAD ;
                    memcpy ((pchar) &xfer_seg, (pchar) &dbuf.data_buf.cu.segmap, sizeof(seg_map_type)) ;
                  }
                else
                  {
                    upphase = UP_IDLE ;
                    xfer_up_ok = FALSE ;
                    shmdt((pchar)pupbuf) ;
                    if (checkbusy ())
                        {
                          pcom = (pvoid) clients[combusy].outbuf ;
                          if (upmemid != NOCLIENT)
                              {
                                shmctl(upmemid, IPC_RMID, NULL) ;
                                upmemid = NOCLIENT ;
                              }
                          if (pcom->completion_status == CSCS_INPROGRESS)
                              pcom->completion_status = CSCS_CANT ;
                          combusy = NOCLIENT ;
                        }
                    xfer_size = 0 ;
                  }
              break ;
            }
          case WAIT_MAP :
            {
              upphase = SENDING_UPLOAD ;
              memcpy ((pchar) &xfer_seg, (pchar) &dbuf.data_buf.cu.segmap, sizeof(seg_map_type)) ;
            }
        }
    }

  void setseed (byte cmp, byte str, seed_name_type *seed, location_type *loc)
    {
      memcpy((pchar) seed, (pchar) &seed_names[cmp][str], 3) ;
      memcpy((pchar) loc, (pchar) &seed_locs[cmp][str], 2) ;
    }
    
  double seed_jul (seed_time_struc *st)
    {
      double t ;
      
      t = jconv (st->yr - 1900, st->jday) ;
      t = t + (double) st->hr * 3600.0 + (double) st->minute * 60.0 + 
          (double) st->seconds + (double) st->tenth_millisec * 0.0001 ;
      return t ;
    }

  void process (void)
    {
      typedef char string5[6] ;
      typedef block_record *tpbr ;
      
      char rcelu[2] = {'N', 'Y'} ;
      string5 lf[2] = {"QSL", "Q512"} ;
      short da_sum, dp_sum, i, j, l ;
      long da_crc, dp_crc ;
      boolean b, full ;
      byte k ;
      pchar ta ;
      seed_name_type sn ;
      seed_net_type sn2 ;
      header_type workheader ;
      short size ;
      unsigned short bc, xfer_offset ;
      typedef error_control_packet *tecp ;
      tecp pecp ;
      pchar p1, p2 ;
      time_quality_descriptor *ptqd ;
      calibration_result *pcal ;
      clock_exception *pce ;
      squeezed_event_detection_report *ppick ;
      commo_reply *preply ;
      complong ltemp ;
      tring_elem *freebuf ;
      seed_fixed_data_record_header *pseed ;
      comstat_rec *pcr, *pcom ;
      download_result *pdr ;
      det_request_rec *pdetavail ;
      download_struc *pds ;
      tpbr pbr ;
      long *pl ;
      char seedst[5] ;
      char s1[64], s2[64] ;
      char myseqno[7];

/* Create a temporary storage area for the received packets seed header */

      seed_fixed_data_record_header my_seed_header;

/* Classify the packets on an integer packet_type */
  
      int packet_type;

/* Determine a first sample_time from the seed packet */

      double packet_time;
      double recv_time;
      int res;


      linkstat.total_packets++ ;
      size = (short) ((long) term - (long) dest);

/* At this point, all known packets are 512 bytes in length, so reject */
/* any that are not this size. However, packets such as opaques blockettes */
/* have not been received and tested and this may reject packets which we */
/* want, but are not the expected size. Log an error to show that it is */
/* happening, so that we can make modifications to this if needed. */

      if (size != 512) 
          {
            LogMessage(CS_LOG_TYPE_ERROR, "Unknown packet size %d",size);
            return;
          }

/* In the mserv version, all 512 packets that we get are considered valid */
/* Here we process the packet, and put it into the next avaiable comserv */
/* Ring buffer slot. */

      else 
          {

            netto_cnt = 0 ; /* got a packet, reset timeout */
            linkstat.last_good = dtime () ;

/* Update the station name in the seed header if required. */
/* We won't worry about any station name embedded in blockettes or contents. */

/* Commented this portion out - paulf - 6/22/2004
	For some unknown reason, the SERVERID name from mserv was being mapped 
	into the MSEED HEADER even if override was not set!. */
	    if (station.l != 0 && override) {
		char *s = long_str (station.l);
		char *d = dest+8;
		memcpy (d, "     ", 5);
		for (i=0; i<5; i++) {
		    if (*s) *(d++) = *(s++);
		}
	    }

/* Copy the header portion of the received packet, and then decode it */
/* into packet types if possible. */

            memcpy(&my_seed_header,dest,64);

/* Extract information that is in the qsl header, but not in the SEED */
/* header so the comserv can use the information to label packets */

            packet_type = classify_packet(&my_seed_header);
            packet_time = header_to_double_time (&my_seed_header);
            recv_time = dtime();

/* Here is some planned logic to update the ring buffer element with */
/* a correct time of data sample. However I could not find a use for */
/* this time stamp, and the conversion from seed to time_array was */
/* non trivial, so I'll time stamp the packets with time of reception */
/* I thought dataspy used this header time of sample, but when I look */
/* I see it decodes the seed header itself. So until I find a need */
/* for an actual data time stamp in this header, I'll use reception time. */


/* The following commented out stuff is information on packets received. */
/* It was in the original comserv, and available if you turned */
/* verbosity up. Since the data structures changed, I need to reimplement */
/* this and I have not yet done it. */
	      if(rambling)
		{
		    	strncpy(myseqno,dest,6);
		    	memset(&myseqno[6],0,1);
     			LogMessage(CS_LOG_TYPE_ERROR, "Mcast Packet: %.2s %.5s %.3s Seq=%s Type=%d received at %s", 
					my_seed_header.header.seednet,
					my_seed_header.header.station_ID_call_letters,
					my_seed_header.header.channel_id,
					myseqno, packet_type,
					time_string(recv_time));
     			LogMessage(CS_LOG_TYPE_ERROR, "Mcast Packet: %.2s %.5s %.3s Seq=%s Type=%d header time %s", 
					my_seed_header.header.seednet,
					my_seed_header.header.station_ID_call_letters,
					my_seed_header.header.channel_id,
					myseqno, packet_type,
					time_string(packet_time));
		}

#ifndef MSERV
                    if (0)
                        {
                          if (anystation)

     LogMessage(CS_LOG_TYPE_INFO, "%4.4s %s Header time %s , received at %s", &my_seed_header.station
	seednamestring(&dbuf.data_buf.cr.h.seedname,
       &dbuf.data_buf.cr.h.location), 
	time_string(freebuf->user_data.header_time), time_string(freebuf->user_data.reception_time)) ;

                        }

#endif

            switch(packet_type)
            {
              case RECORD_HEADER_1 :
              {

                freebuf = getbuffer (DATAQ) ;    /* get free buffer */
	        if(freebuf == NULL)
                {
                  LogMessage(CS_LOG_TYPE_ERROR,"Station blocked.");
	          LogMessage(CS_LOG_TYPE_ERROR,"No blocking clients.Exiting");
/*::	          exit(12);*/
		  return;
                }
                break;
              }
              case BLOCKETTE :
              {

                freebuf = getbuffer (BLKQ) ;    /* get free buffer */
	        if(freebuf == NULL)
                {
                  LogMessage(CS_LOG_TYPE_ERROR,"Station blocked.");
	          LogMessage(CS_LOG_TYPE_ERROR,"No blocking clients.Exiting");
/*::	          exit(12);*/
		  return;
                }
                break;
              }
              case COMMENTS :
              {
                freebuf = getbuffer (MSGQ) ;    /* get free buffer */
	        if(freebuf == NULL)
                {
                  LogMessage(CS_LOG_TYPE_ERROR,"Station blocked.");
	          LogMessage(CS_LOG_TYPE_ERROR,"No blocking clients.Exiting");
/*::	          exit(12);*/
		  return;
                }
                break;
              }
              case CLOCK_CORRECTION :
              {
                freebuf = getbuffer (TIMQ) ;    /* get free buffer */
	        if(freebuf == NULL)
                {
                  LogMessage(CS_LOG_TYPE_ERROR,"Station blocked.");
	          LogMessage(CS_LOG_TYPE_ERROR,"No blocking clients.Exiting");
/*::	          exit(12);*/
		  return;
                }
                break;
              }
              case DETECTION_RESULT :
              {
                freebuf = getbuffer (DETQ) ;    /* get free buffer */
	        if(freebuf == NULL)
                {
                  LogMessage(CS_LOG_TYPE_ERROR,"Station blocked.");
	          LogMessage(CS_LOG_TYPE_ERROR,"No blocking clients.Exiting");
/*::	          exit(12);*/
		  return;
                }
                break;
              }
              case END_OF_DETECTION :
              {
                freebuf = getbuffer (DATAQ) ;    /* get free buffer */
	        if(freebuf == NULL)
                {
                  LogMessage(CS_LOG_TYPE_ERROR,"Station blocked.");
	          LogMessage(CS_LOG_TYPE_ERROR,"No blocking clients.Exiting");
/*::	          exit(12);*/
		  return;
                }
                break;
              }
              case CALIBRATION :
              {
                freebuf = getbuffer (CALQ) ;    /* get free buffer */
	        if(freebuf == NULL)
                {
                  LogMessage(CS_LOG_TYPE_ERROR,"Station blocked.");
	          LogMessage(CS_LOG_TYPE_ERROR,"No blocking clients.Exiting");
/*::	          exit(12);*/
		  return;
                }
                break;
              }
              default :
              {
                LogMessage(CS_LOG_TYPE_ERROR,"Unknown Packet Type %d",packet_type);
                return;
              }
            } /* End of Switch */


/* Now freebuf points to the proper comserv ring buffer. Now we */
/* put the data into the buffer. */


             pseed = (pvoid) &freebuf->user_data.data_bytes ;

             freebuf->user_data.reception_time = recv_time ;    
             freebuf->user_data.header_time = packet_time;

             memcpy (pseed,dest,512) ;
    
             return;

/* All the other packet handling logic here is to sort packets that have a */
/* QSL header. Mserv does not have the header, so this logic is stubbed out. */

          }
    }

  void fillbuf (void)
    {
      int numread, clilen ;
#ifdef _OSK
      u_int32 err, count ;
#endif

      src = (pchar) &sbuf ;
      srcend = src ;
      if (!serial)
          {
            if ((path < 0) && (netdly_cnt >= netdly))
                {
                  clilen = sizeof(cli_addr) ;
                  path = accept(sockfd, (psockaddr) &cli_addr, &clilen) ;
                  netdly_cnt = 0 ;
                  if ((verbose) && (path >= 0))
                      LogMessage (CS_LOG_TYPE_INFO, "Network connection with DA opened") ;
                  if (linkstat.ultraon)
                      {
                        linkstat.linkrecv = FALSE ;
                        linkstat.ultrarecv = FALSE ;
                        seq_valid = FALSE ;
                        linkpoll = link_retry ;
                      }
                }
            if (path < 0)
                return ;
          }
#ifdef _OSK
      if (serial) /* make sure we don't block here */
          {
            err = _os_gs_ready(path, &count) ;
            if ((err == EOS_NOTRDY) || (count == 0))
                return ;
            else if (err != 0)
                {
                  numread = -1 ;
                  errno = err ;
                }
              else
                {
                  if (count > BLOB)
                      count = BLOB ;
                  err = blockread (path, count, src) ;
                  if (err == 208)
                      numread = read(path, src, count) ;
                  else if (err == 0)
                      numread = count ;
                    else
                      {
                        numread = -1 ;
                        errno = err ;
                      }
                }
          }
        else
          numread = read(path, src, BLOB) ;  
#else
      numread = read(path, src, BLOB) ;
#endif
      if (numread > 0)
         {
           srcend = (pchar) ((long) srcend + numread) ;
           if ((insane) /* && (numread > maxbytes) */)
               {
                 maxbytes = numread ;
                 LogMessage (CS_LOG_TYPE_INFO, "%d bytes read", numread) ;
               }
         }
      else if (numread < 0)
          if (errno != EWOULDBLOCK)
              {
                linkstat.io_errors++ ;
                linkstat.lastio_error = errno ;
                if (serial)
                    {
                      if (verbose)
                          LogMessage (CS_LOG_TYPE_ERROR, "Error reading from port ") ;
                    }
                  else
                    {
                      if (verbose)
                          LogMessage (CS_LOG_TYPE_ERROR, "Network connection with DA closed") ;
                      shutdown(path, 2) ;
                      close(path) ;
                      seq_valid = FALSE ;
                      if (linkstat.ultraon)
                          {
                            linkstat.linkrecv = FALSE ;
                            linkstat.ultrarecv = FALSE ;
                          }
                       path = -1 ; /* signal not open */
                    }
              }
    }

  short inserial (pchar b)
    {
      int numread ;

      if (src == srcend)
          fillbuf () ;

      if (src != srcend)
          {
            *b = *src++ ;
            return 1 ;
          }
        else
          return 0 ;
    }

  void dlestrip (void)
    {
      boolean indle ;

      term = NULL ;
      indle = (lastchar == DLE) ;
      while ((src != srcend) && (dest != destend))
        {
          if (indle)
              {
                *dest++ = *src ;
                indle = FALSE ;
              }
          else if (*src == DLE)
              indle = TRUE ;
          else if (*src == ETX)
              {
                term = dest ;
                src++ ;
                lastchar = NUL ;
                return ;
              }
            else
              *dest++ = *src ;
          src++ ;
        }
      if (indle)
          lastchar = DLE ;
        else
          lastchar = NUL ;
    }

#define NUM_RECV_PACKETS 20 	/* the number of packets to pull out of the recv() buffer in one call to check_input() */

  void check_input (void)
    { 
      int numread, packets;
      short err;
      char b;
      packets = 0;

/* Define a SEED sequence Number var for debugging purposes. */


/* It should always be udplink in mserv */

      if (udplink)
      {

/* Use of recvfrom, versus recv. I went with recv because I don't */
/* need to know who sent the data to me. Recvfrom allows me to identify */
/* the ip address of the sender. I debated the size of the receive buffer. */
/* It is possible, use of a smaller recieve buffer preserves global system */
/* resources such as mbufs, but I can't confirm this, so I'll use a */
/* buffer with some extra room, which is the actual size of the sbuf var. */

/* In addition this is a nonblocking read. When we return we first check */
/* for a non-blocking return error code. This is the most common return. */
/* Next we check for a "good" read of 512 bytes. Then we will process */
/* this as a good message */


/* paulf - added in a check to read in at least NUM_RECV_PACKETS packets in a row if they are waiting */
       while(packets < NUM_RECV_PACKETS ) 
       {
	    numread = recv(path,dest,512,0);

            if (numread < 0)
            {
	      /* If the errno is EWOULDBLOCK, then we want to return without any error.*/
	      /* This is the standard path out of this routine. */
	      if (errno != EWOULDBLOCK)
              {
                LogMessage(CS_LOG_TYPE_ERROR,"Errno on socket recv: %s", strerror(errno));
              } else {
		if (rambling && packets > 0) 
                     LogMessage(CS_LOG_TYPE_ERROR, "EWOULDBLOCK on socket recv. after reading %d mcast packets", packets);
              	return;
	      }
            }
            else if(numread == 512)
            {
		/* This is a good read of a SEED packet*/
                term = (pchar) ((long) dest + numread) ;
                process () ;
		packets++;
             }
             else
             {
                  LogMessage(CS_LOG_TYPE_ERROR,"Uknown packet size %d",numread);
             }
         } /* end of while loop over packets */
      }
      else /* This is the else to if (udplink). Should never happen */
      {

/* On the exception condition (that is, it should never happen) that */
/* mserv is not in udplink mode, report an error and exit. This will */
/* for the operator to find out what's causing mserv to exit. */

           LogMessage(CS_LOG_TYPE_ERROR,"Mserv not is udplink mode.Exiting");
           exit(12);
       }

/* This should be the common return from all parts of the if statement. */

    return;

    }

