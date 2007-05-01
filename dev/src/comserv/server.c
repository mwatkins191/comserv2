/*$Id: server.c,v 1.2 2005/03/30 22:30:59 isti Exp $*/
/*   Server Main file
     Copyright 1994-1998 Quanterra, Inc.
     Written by Woodrow H. Owens
     Linux porting by ISTI  (www.isti.com)
     with modifications by KIM GEUNYOUNG fo Linux UDP 	

Edit History:
   Ed Date      By  Changes
   -- --------- --- ---------------------------------------------------
    0 11 Mar 94 WHO First created.
    1 23 Mar 94 WHO First testing done, start splitting code of into modules.
    2 27 Mar 94 WHO Merged blockette ring changed to separate rings.
    3  7 Apr 94 WHO Individual fields for link statistics removed in favor
                    linkstat_rec. 
    4 16 Apr 94 WHO Define seed format and extension level. Show station
                    description.
    5 30 May 94 WHO Add global comm_mask variable.
    6  6 Jun 94 WHO client_wait field changed to microseconds. add setting
                    of privusec and nonusec fields. Don't send SIGALRM if
                    not same UID as client. Add user privilege variables
                    (handled by cscfg and commands).
    7  9 Jun 94 WHO Add support to remove foreign clients after timeout.
                    Cleanup to avoid warnings.
    8 16 Jun 94 WHO Fix calculation of foreign_to in procedure check_clients.
    9  9 Aug 94 WHO Fix initialization of linkstat record.
   10 11 Aug 94 WHO Add network support.
   11 18 Aug 94 WHO Fix = instead of == in detach_client.
   12 25 Sep 94 WHO Add REUSEADDR and KEEPALIVE options to socket.
   13  3 Nov 94 WHO Add SOLARIS2 definition to use nanosleep call instead of usleep.
                    SOLARIS2 also makes slight syntax differences in socket calls.
   14 13 Dec 94 WHO Add size of data only blockette to ring buffers allocations.
                    Change SEED version extension from A to B.
   15 14 Jun 95 WHO Add ignoring the SIGPIPE signal so program doesn't terminate when
                    the socket is disconnected.
   16 20 Jun 95 WHO Process netto and netdly parameters. Allows giving up on network
                    connection and trying again. Check for timeout of ack grouping.
   17  2 Oct 95 WHO Don't do automatic 60 second link_req polling if sync set.
   18  8 Oct 95 WHO Allow enabling RTS/CTS flow control.
   19  2 Jun 96 WHO Start of conversion to run on OS9. Don't compare
                    the result of "kill" with zero, use ERROR.
   20 10 Jun 96 WHO Simplify file upload, just send packet every second.
   21 19 Jun 96 WHO Allocate cal ring buffer room based on random_calibration
                    instead of sine_calibration, which is 4 bytes shorter.
   22 30 Jun 96 WHO Don't set active flag to FALSE in detach_client if this
                    is a reserved client that hasn't timed out yet, or else
                    the timeout will never occur.
   23  4 Aug 96 WHO If noultra flag is on, don't poll for ultra packet.
   24  3 Dec 96 WHO Add support for Blockette Records.
   25  7 Dec 96 WHO Add support for UDP.
   26 16 Oct 97 WHO Change in cscfg. Add reporting of all module versions.
   27 22 Oct 97 WHO Add vopttable access for OS9 version.
   28 29 Nov 97 DSN Added optional lockfile directive again.
   29  8 Jan 98 WHO lockfile stuff doesn't apply to OS9 version.
   30 23 Jan 98 WHO Add link_retry variable to use with linkpoll.
   31  9 Nov 99 IGD Porting to SuSE 6.1 LINUX begins
      10 Nov 99 IGD A third parameter in semop set to 1 instead of 0
       6 Dec 00 IGD The above change is #ifdef'ed for Linux only for full compatibility
			with the Solaris version with the version 30	
       8 Sep 00 KIM Change timespec definition struct timespec var
       8 Sep 00 KIM Change ipaddr, ipaddr type conversion
       7 Dec 00 IGD Changes are incorporated into the root Linux version
   32 21 Apr 04 DSN Added myipaddr config directive.
   33 18 Apr 07 DSN Test for failure when shmat() to client shared memory.
*/           
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#ifndef _OSK
#include <unistd.h>
#include <termio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/file.h>
#else
#include <stdlib.h>
#include <sgstat.h>
#include <module.h>
#include <types.h>
#include <sg_codes.h>
#include <modes.h>
#include "os9inet.h"
#endif
#include "quanstrc.h"
#include "stuff.h"
#include "service.h"
#include "cfgutil.h"
#include "server.h"
#include "timeutil.h"
#ifdef _OSK
#include "os9stuff.h"
#endif
#ifdef	MYIPADDR
#include <arpa/inet.h>
#endif

#ifdef SOLARIS2
#include <time.h>

     typedef struct {
          time_t        tv_sec; /* seconds */
          long          tv_nsec;/* and nanoseconds */
     } timespec ;
#endif
/* Added by Kim 2000 SEP 08          */
#ifdef LINUX
#include <time.h>
#endif
/* LINUX supports timespec structure */


#define MAXPROC 2 /* maximum number of service requests to process before checking serial port */
#define MAXWAIT 10 /* maximum number of seconds for clients to wait */
#define PRIVILEGED_WAIT 1000000 /* 1 second */
#define NON_PRIVILEGED_WAIT 100000 /* 0.1 second */
#define NON_PRIVILEGED_TO 60.0
#define EDITION 32

char seedformat[4] = { 'V', '2', '.', '3' } ;
char seedext = 'B' ;
seed_net_type network = {' ', ' '} ;
complong station ;
 
tring rings[NUMQ] ;                /* Access structure for rings */

pserver_struc base = NULL ;        /* Base address of server memory segment */
pclient_struc cursvc = NULL ;      /* Current client being processed */
pclient_station curclient = NULL ; /* Offset into client's memory for that station */

config_struc cfg ;
tuser_privilege user_privilege ;
char str1[CFGWIDTH] ;
char str2[CFGWIDTH] ;
char stemp[CFGWIDTH] ;
char port[SECWIDTH] = "" ;    /* assume not an serial port */
char ipport [SECWIDTH] = "" ;
char ipaddr [SECWIDTH] = "" ;
#ifdef  MYIPADDR
char myipaddr [SECWIDTH] = "" ;
#endif
#ifndef _OSK
char lockfile[CFGWIDTH] = "" ;
#endif
long baud = 19200 ;
char parity = 'N' ; 
long polltime = 50000 ;
long blockmask = 0 ;
long noackmask = 0 ;
long oldackmask = 0 ;
long comm_mask = 0 ;
long grpsize = 1 ;
long grptime = 5 ;
long link_retry = 10 ;
int segkey = 0 ;
int upmemid = NOCLIENT ;
tclients clients[MAXCLIENTS] ;
byte inphase = SOHWAIT ;
byte upphase = UP_IDLE ;
short highclient = 0 ;
short resclient = 0 ;
short uids = 0 ;
short txwin = 0 ;
int path = -1 ;
int sockfd = -1 ;
#ifndef _OSK
int lockfd = -1 ;
#endif
struct sockaddr_in cli_addr, serv_addr ;
      
typedef struct sockaddr *psockaddr ;

/* These pointers are used to access the input source buffer "sbuf"
   and the destination buffer, which is "dbuf"
*/
pchar src = NULL ;
pchar srcend = NULL ;
pchar dest = NULL ;
pchar destend = NULL ;
pchar term = NULL ;
unsigned char sbuf[BLOB] ;
DA_to_DP_buffer dbuf ;

byte last_packet_received = 0 ;
byte lastchar = NUL ;

DP_to_DA_buffer temp_pkt ;
tcrc_table crctable ;

long seq = 0 ;
long start_time = 0 ;      /* For seconds in operation */
boolean verbose = TRUE ;   /* normal, client on/off etc. */
boolean rambling = FALSE ; /* Incoming packets display a line */
boolean insane = FALSE ;   /* Client commands are shown */
boolean override = FALSE ; /* Override station/component */
boolean detavail_ok = FALSE ;
boolean detavail_loaded = FALSE ;
boolean first = TRUE ;
boolean firstpacket = TRUE ;
boolean seq_valid = FALSE ;
boolean xfer_down_ok = FALSE ;
boolean xfer_up_ok = FALSE ;
boolean map_wait = FALSE ;
boolean ultra_seg_empty = TRUE ;
boolean stop = FALSE ;
boolean follow_up = FALSE ;
boolean serial = FALSE ;         /* serial port not enabled */
boolean notify = FALSE ;        /* Ultra_req polling not disabled */
boolean flow = FALSE ;
boolean udplink = FALSE ;
boolean noultra = FALSE ;
short combusy = NOCLIENT ;           /* <>NOCLIENT if processing a command for a station */
short ultra_percent = 0 ;
unsigned short lowest_seq = 300 ;
short linkpoll = 0 ;
long con_seq = 0 ;
long netto = 120 ; /* network timeout */
long netdly = 30 ; /* network reconnect delay */
long netto_cnt = 0 ; /* timeout counter (seconds) */
long netdly_cnt = 0 ; /* reconnect delay */
byte cmd_seq = 1 ;
link_record curlink = { 0, 0, 0, 0, 0, 0, CSF_QSL, 0, 0, 0, 0, 0, 0, 0, 0, 0 } ;
linkstat_rec linkstat = { TRUE, FALSE, FALSE, FALSE, 0, 0, 0, 0, 0, 0, 0, 0, 0.0, 0.0, "V2.3", 'A', CSF_QSL, "" } ;
string59 xfer_destination = "" ;
string59 xfer_source = "" ;
char station_desc[CFGWIDTH] = "" ;
byte ultra_seg[14] ;
byte detavail_seg[14] ;
seg_map_type xfer_seg ;
unsigned short seg_size = 0 ;
unsigned short xfer_size = 0 ;
unsigned short xfer_up_curseg = 0 ;
unsigned short xfer_bytes = 0 ;
unsigned short xfer_total = 0 ;
unsigned short xfer_last = 0 ;
unsigned short xfer_segments = 0 ;
unsigned short cal_size = 0 ;
unsigned short used_size = 0 ;
unsigned short ultra_size = 0 ;
cal_record *pcal = NULL ;
short sequence_mod = 8 ;
short vcovalue = -1 ; 
short xfer_resends = -1 ;
short mappoll = 0 ;
short sincemap = 0 ;
short minctr = 0 ;
short clientpoll = 0 ;
short down_count = 0 ;
download_struc *pdownload = NULL ;     
ultra_type *pultra = NULL ;
tupbuf *pupbuf = NULL ;
double curtime, lastsec ;
double last_sent = 0.0 ;
DP_to_DA_msg_type gmsg ;

#ifdef _OSK
mh_com *vopthdr ;
voptentry *pvopt ;
#endif

string3 seed_names[20][7] =
          /* VBB   VSP    LG    MP    LP   VLP   ULP */
  { /*V1*/ {"BHZ","EHZ","HLZ","MHZ","LHZ","VHZ","UHZ"},
    /*V2*/ {"BHN","EHN","HLN","MHN","LHN","VHN","UHN"},
    /*V3*/ {"BHE","EHE","HLE","MHE","LHE","VHE","UHE"},
    /*V4*/ {"BXZ","EXZ","HXZ","MXZ","LXZ","VXZ","UXZ"},
    /*V5*/ {"BXN","EXN","HXN","MXN","LXN","VXN","UXN"},
    /*V6*/ {"BXE","EXE","HXE","MXE","LXE","VXE","UXE"},
    /*ZM*/ {"BMZ","EMZ","HMZ","MMZ","LMZ","VMZ","UMZ"},
    /*NM*/ {"BMN","EMN","HMN","MMN","LMN","VMN","UMN"},
    /*EM*/ {"BME","EME","HME","MME","LME","VME","UME"},
    /*L1*/ {"BCI","ECI","HCI","MCI","LCI","VCI","UCI"},
    /*L2*/ {"BTI","ETI","HTI","MTI","LTI","VTI","UTI"},
    /*L3*/ {"BPI","EPI","HPI","MPI","LPI","VPI","UPI"},
    /*L4*/ {"BX1","EX1","HX1","MX1","LX1","VX1","UX1"},
    /*L5*/ {"BX2","EX2","HX2","MX2","LX2","VX2","UX2"},
    /*L6*/ {"BX3","EX3","HX3","MX3","LX3","VX3","UX3"},
    /*L7*/ {"BX4","EX4","HX4","MX4","LX4","VX4","UX4"},
    /*L8*/ {"BX5","EX5","HX5","MX5","LX5","VX5","UX5"},
    /*L9*/ {"BX6","EX6","HX6","MX6","LX6","VX6","UX6"},
    /*R1*/ {"BX7","EX7","HX7","MX7","LX7","VX7","UX7"},
    /*R2*/ {"BX8","EX8","HX8","MX8","LX8","VX8","UX8"}} ;

location_type seed_locs[20][7] ;

string15 queue_names[STOPACK+1] =
  { "Data", "Detection", "Calibration", "Timing", "Message", "Every" } ;

#ifdef _OSK
extern short VER_OS9STUFF ;
#endif
extern short VER_TIMEUTIL ;
extern short VER_CFGUTIL ;
extern short VER_SEEDUTIL ;
extern short VER_STUFF ;
extern short VER_COMLINK ;
extern short VER_CSCFG ;
extern short VER_BUFFERS ;
extern short VER_COMMANDS ;

void unblock (short clientnum) ;
byte handler (pclient_struc svc, short clientnum) ;
void next_segment (void) ;
void request_map (void) ;
void send_window (void) ;
void readcfg (void) ;
void gcrcinit (void) ;
void check_input (void) ;
void setupbuffers (void) ;
void request_ultra (void) ;
void request_link (void) ;
void set_verb (int i) ;
  
  void cleanup (int sig)
    {
      if (!serial)
          {
            if (path >= 0)
                {
                  shutdown(path, 2) ;
                  close(path) ;
                }
            shutdown(sockfd, 2) ;
            close(sockfd) ;
          }
      fflush (stdout) ;
      fprintf(stderr, "%s Server terminated\n", localtime_string(dtime())) ;
      exit(12) ;
    }
    
  void terminate (pchar s)
    {
      fprintf(stderr, "%s %s", localtime_string(dtime()), s) ;
      exit(12) ;
    }

  void detach_client (short clientnum, boolean timedout)
    {
      short i, j ;
      tclients *pt ;
      boolean sameuid ;

      if (combusy == clientnum)
          combusy = NOCLIENT ;
      pt = &clients[clientnum] ;
      pt->client_pid = NOCLIENT ;
      pt->client_uid = NOCLIENT ;
      if (pt->client_address)
          {
            sameuid = clients[clientnum].client_address->client_uid == base->server_uid ;
            if (shmdt((pchar) pt->client_address) == 0)
                if (sameuid) /* Don't delete foreign client, we don't know for sure it is dead */
                    shmctl(pt->client_memid, IPC_RMID, NULL) ;
            pt->client_address = NULL ;
          }
      if (timedout)
          {
            pt->active = FALSE ;
            unblock (clientnum) ;
            clr_bit (&blockmask, clientnum) ;
          }
      if (clientnum >= resclient)
          { /* remove this client from list */
            pt->active = FALSE ;
            for (i = clientnum ; i < highclient - 1 ; i++)
              clients[i] = clients[i + 1] ;
            highclient-- ; /* contract the window */
          }
    }
         
  void check_clients (void)
    {
      short i ;
      tclients *pt ;
      boolean timedout, died, foreign_to, sameuid, alive ;

      if (++clientpoll >= highclient)
          clientpoll = 0 ;
      pt = &clients[clientpoll] ;
      if (!pt->active)
          return ;
      timedout = (pt->blocking) && ((curtime - pt->last_service) > pt->timeout) ;
      alive = (pt->client_address) && (pt->client_pid != NOCLIENT) ;
      sameuid = alive && (pt->client_uid == base->server_uid) ;
#ifdef _OSK
      died =  alive && sameuid && (kill(pt->client_pid, SIGWAKE) == ERROR) ;
#else
      died =  alive && sameuid && (kill(pt->client_pid, 0) == ERROR) ;
#endif
      foreign_to = alive && (!sameuid) && ((curtime - pt->last_service) > NON_PRIVILEGED_TO)
                         && (combusy != clientpoll) ;
      if (timedout && verbose)
          printf("Client %4.4s timed out at %s", &pt->client_name, time_string(dtime())) ;
      else if (died && verbose)
          printf("Client %4.4s has died at %s", &pt->client_name, time_string(dtime())) ;
      else if (foreign_to && verbose)
          printf("Foreign Client %4.4s presumed dead at %s", &pt->client_name, time_string(dtime())) ;
      if (died || timedout || foreign_to)
          {
            if (verbose)
                {
                  if (timedout)
                      printf(", unblocking") ;
                  if (clientpoll >= resclient)
                      printf(", Removed from client table\n") ;
                    else
                      printf(", Reserved client, not removed\n") ;
                }
            detach_client (clientpoll, timedout) ;
          }
      fflush (stdout) ;
    }
    
  int main (int argc, char *argv[], char **envp)
    {
#ifdef _OSK
      struct sgbuf sttynew ;
      struct sgbuf sttyold ;
#else
      struct termio sttynew ;
      struct termio sttyold ;
#endif
      int shmid ;
      int semid, clientid ;
      short i, j, k, l, zerocnt, found, first, uppoll ;
      long total, tscan, lasttotal, services, cflag ;
      short cur, did ;
      short min, max ;
      int err, outque ;
      char c ;
      long ct, ctmax, ctmin, ctcount ;
      float cttotal ;
      struct sembuf notbusy = { 0, 1, 0 } ;
      long bufsize, size, stemp ;
      int flags, ruflag ;
      int status ;
#ifdef SOLARIS2
      timespec_t rqtp, rmtp ;
#endif
#ifdef _OSK
      unsigned pollslp ;
#endif
#ifdef LINUX /*ADDED BY KIM */
      struct timespec rqtp, rmtp ;
#endif

       
      char filename[CFGWIDTH] ;
      char station_dir[CFGWIDTH] = "" ;
      char station_name[5] ;
      char source[SECWIDTH] = "" ;
      
/* must have at least one command line argument (the station) */
      if (argc < 2)
          {
            fprintf (stderr, "No station name specified\n") ;
            exit(12) ;
          }
          
      start_time = (long) dtime () ;

/* Copy the first argument into the station name, make sure it's null
   terminated.
*/
      strncpy (station_name, argv[1], 5) ;
      station_name[4] = '\0' ;
      station.l = str_long (station_name) ;

/* Report versions of all modules */
      printf ("%s Comserv Edition %d started for station %s\n", 
       localtime_string(dtime()), EDITION, &station_name) ;
      printf ("      Quanstrc Ver=%d, DPstruc Ver=%d, Seedstrc Ver=%d, Timeutil Ver=%d\n",
              VER_QUANSTRC, VER_DPSTRUC, VER_SEEDSTRC, VER_TIMEUTIL) ;
      printf ("      Cfgutil Ver=%d, Seedutil Ver=%d, Stuff Ver=%d, Comlink Ver=%d\n",
              VER_CFGUTIL, VER_SEEDUTIL, VER_STUFF, VER_COMLINK) ;
      printf ("      Cscfg Ver=%d, Buffers Ver=%d, Commands Ver=%d",
              VER_CSCFG, VER_BUFFERS, VER_COMMANDS) ;
#ifdef _OSK
      printf (", OS9stuff Ver=%d\n", VER_OS9STUFF) ;
#else
      printf ("\n") ;
#endif

/* for OS9, open a pointer into vopttable */
#ifdef _OSK
      err = vopt_open (&vopthdr, &pvopt) ;
      if (err)
          {
            fprintf (stderr, "Could not access VOPTTABLE module\n") ;
            exit(err) ;
          }
      pvopt->verbosity = 0 ;
      strcpy (str1, "comserv/") ;
      strcat (str1, station_name) ;
      strpas (pvopt->modname, str1) ;
      pvopt->option1 = 0 ;
      pvopt->option2 = 0 ;
      pvopt->option3 = 0 ;
#endif

/* open the stations list and look for that station */
#ifdef _OSK
      strcpy (filename, "/r0/stations.ini") ;
#else
      strcpy (filename, "/etc/stations.ini") ;
#endif
      if (open_cfg(&cfg, filename, station_name))
          terminate ("Could not find station in stations.ini file\n") ;

/* Try to find the station directory, source, and description */
      do
        {
          read_cfg(&cfg, str1, str2) ;
          if (str1[0] == '\0')
              break ;
          if (strcmp(str1, "DIR") == 0)
              strcpy(station_dir, str2) ;
          else if (strcmp(str1, "DESC") == 0)
              {
                strcpy(station_desc, str2) ;
                station_desc[59] = '\0' ;
                printf ("%s\n", station_desc) ;
              }
          else if (strcmp(str1, "SOURCE") == 0)
              strcpy(source, str2) ;
        }
      while (1) ;
      close_cfg(&cfg) ;
      
/* Check for that this is a comlink station */
      if (strcasecmp((pchar) &source, "comlink") != 0)
          terminate ("Not a comlink station\n") ;

/* Try to open the station.ini file in this station's directory */
      addslash (station_dir) ;
      strcpy (filename, station_dir) ;
      strcat (filename, "station.ini") ;
      if (open_cfg(&cfg, filename, "comlink"))
          terminate ("Could not find station.ini file\n") ;
          
/* Initialize client control structure */
      for (i = 0 ; i < MAXCLIENTS ; i++)
        {
          clients[i].client_memid = NOCLIENT ;
          clients[i].client_pid = NOCLIENT ;
          clients[i].client_uid = NOCLIENT ;
          clients[i].client_name.l = 0 ;
          clients[i].client_address = NULL ;
          clients[i].blocking = FALSE ;
          clients[i].timeout = 0 ; /* blocking not allowed */
          clients[i].active = FALSE ;
          for (j = DATAQ ; j < NUMQ ; j++)
            {
              clients[i].last[j].scan = NULL ;
              clients[i].last[j].packet = -1 ;
            }
        }
       
/* Initialize ring structure */
      for (i = DATAQ ; i < NUMQ ; i++)
        {
          rings[i].head = NULL ;
          rings[i].tail = NULL ;
          rings[i].count = 20 ;
          rings[i].spare = 0 ;
          rings[i].size = 0 ;
        }

/* call routine to parse config file */
      readcfg () ;
      fflush (stdout) ;

#ifndef _OSK
/* Open the lockfile for exclusive use if lockfile is specified.*/
/* This prevents more than one copy of the program running for */
/* a single station.      */
      if (lockfile[0] != '\0')
          {
            if ((lockfd = open (lockfile, O_RDWR | O_CREAT, 0644)) < 0)
                {
                  fprintf (stderr, "Unable to open lockfile: %s\n", lockfile) ;
                  exit (12) ;
                }
            if ((status = lockf (lockfd, F_TLOCK, 0)) < 0)
                {
                  fprintf (stderr, "Unable to lock lockfile: "
                           "%s status=%d errno=%d\n", lockfile, status, errno) ;
                  close (lockfd);
                  exit (12) ;
                }
          }
#endif      
      resclient = highclient ; /* reserved clients */
/* create access control semaphore */
      semid = semget(segkey, 1, IPC_CREAT |  PERM) ;
      if (semid == ERROR)
          {
            fprintf (stderr, "Could not create semaphore with key %d\n", segkey) ;
            exit (12) ;
          }

/* set the size of each ring buffer and add them up to get total size */
      bufsize = 0 ;
/* get base size for blockettes, including reception and header time */
      stemp = sizeof(seed_record_header) + sizeof(data_only_blockette) +
              (sizeof(tdata_user) - 512) ;
      for (i = DATAQ ; i < NUMQ ; i++)
        {
          switch (i)
            {
              case DATAQ : ;
              case BLKQ :
                {
                  size = sizeof(tdata_user) ;               /* whole enchilada */
                  break ;
                }
              case DETQ : 
                {
                  size = stemp + sizeof(murdock_detect) ;   /* just enough for detections */
                  break ;
                }
              case CALQ : 
                {
                  size = stemp + sizeof(random_calibration) ; /* Just enough for calibrations */
                  break ;
                }
              case TIMQ : 
                {
                  size = stemp + sizeof(timing) ;           /* just enough for timing */
                  break ;
                }
              case MSGQ :
                { 
                  size = stemp + COMMENT_STRING_LENGTH + 2 ; /* just enough for messages */
                  break ;
                }
            }
          rings[i].xfersize = size ; /* number of bytes to transfer to client */
          size = (size + 31) & 0xfffffff8 ; /* add in overhead and double word align */
          bufsize = bufsize + size * rings[i].count ;
          rings[i].size = size ;
        }
      bufsize = (bufsize + 15) & 0xfffffff8 ; /* double word align */

/* create shared memory segment and install my process id */
      shmid = shmget(segkey, sizeof(tserver_struc) + 
                     bufsize, IPC_CREAT |  PERM) ;
      if (shmid == ERROR)
          {
            fprintf (stderr, "Could not create server segment with key %d\n", segkey) ;
            exit (12) ;
          }
      base = (pserver_struc) shmat(shmid, NULL, 0) ;
      if (base == (pserver_struc) ERROR)
          {
            fprintf (stderr, "Could not attach to server segment ID %d\n", shmid) ;
            exit (12) ;
          }
      base->init = '\0' ;
      base->server_pid = getpid () ;
      base->server_uid = getuid () ;
      base->client_wait = MAXWAIT * 1000000 ; /* convert to microseconds */
      base->privusec = PRIVILEGED_WAIT ;
      base->nonusec = NON_PRIVILEGED_WAIT ;
      base->server_semid = semid ;
      base->servcode = dtime () ;

/* initialize service queue */
      for (i = 0 ; i < MAXCLIENTS ; i++)
        {
          base->svcreqs[i].clientseg = NOCLIENT ;
          base->svcreqs[i].clientname.l = clients[i].client_name.l ;
        }

/* initialize next packet numbers */
      base->next_data = 0 ;

/* Call routine to setup ring buffers for data and blockettes */
      setupbuffers () ;
   
/* Allow access to service queue */ 

	/*IGD -> was  if (semop(semid, &notbusy, 1) == ERROR)
	 * The third argument is the number of elements in the structure
	 *  of the third argument. Setting it to zero causes
	 * system EINVAL under Linux
	 **************************************************/    
#if defined (LINUX)     
	if (semop(semid, &notbusy, 1) == ERROR) 
#else  /* IGD 03/09/01 bug fixed : was elif */
	if (semop(semid, &notbusy, 0) == ERROR)
#endif
          {
            fprintf (stderr, "Could not set semaphore ID %d to not busy\n", semid) ;
            exit (12) ;
          }
          
/* Server shared memory segment is now initialized */
      base->init = 'I' ;

      if (port[0] != '\0')
          {
            udplink = FALSE ; /* no such thing as udp serial */
/* Open serial port, get it's settings (save them in sttyold) */
#ifdef _OSK
            path = open (port, FAM_READ |  FAM_WRITE) ;
#else
            path = open (port, O_RDWR |  O_NDELAY) ;
#endif
            if (path < 0)
                {
                  fprintf (stderr, "Could not open serial port %s\n", port) ;
                  exit (12) ;
                }
#ifdef _OSK
            if (_gs_opt(path, &sttyold) == ERROR)
#else
            if (ioctl(path, TCGETA, &sttyold) == ERROR)
#endif
                {
                  fprintf (stderr, "Could not obtain settings for %s\n", port) ;
                  exit (12) ;
                }
#ifdef _OSK
            memcpy ((pchar) &sttynew, (pchar) &sttyold, sizeof(struct sgbuf)) ;
#else
            memcpy ((pchar) &sttynew, (pchar) &sttyold, sizeof(struct termio)) ;
#endif

/* Setup new settings for Raw mode */
#ifdef _OSK
            memset((pchar) &sttynew.sg_case, '\0', 19) ; /* clear line editing */
            memset((pchar) &sttynew.sg_xon, '\0', 4) ; /* clear flow and tabs */
            switch (baud)
              {
                case 2400 : 
                  {
                    sttynew.sg_baud = 0xA ;
                    break ;
                  }
                case 4800 :
                  {
                    sttynew.sg_baud = 0xC ;
                    break ;
                  }
                case 9600 :
                  {
                    sttynew.sg_baud = 0xE ;
                    break ;
                  }
                case 19200 :
                  {
                    sttynew.sg_baud = 0xF ;
                    break ;
                  }
                case 38400 :
                  {
                    sttynew.sg_baud = 6 ;
                    break ;
                  }
                default :
                  {
                    fprintf(stderr, "Invalid baudrate %d\n", baud) ;
                    exit(12) ;
                  }
              }
            cflag = 0 ; /* assume no parity */
            switch (parity)
              {
                case 'E' :
                  {
                    cflag = 3 ;
                    break ;
                  }
                case 'O' :
                  {
                    cflag = 2 ;
                    break ;
                  }
              }
            sttynew.sg_parity = (sttynew.sg_parity & 0xfc) |  cflag ;

/* Configure port using new settings */
            if (_ss_opt(path, &sttynew) == ERROR)
                {
                  fprintf (stderr, "Cound not configure port %s\n", port) ;
                  exit (12) ;
                }
            if (resetp (path, 4096) != 0)
                fprintf (stderr, "Could not change serial input buffer size on port %s\n", port) ;
            if (flow)
                if (hardon (path) != 0)
                    {
                      fprintf (stderr, "Hardware flow control not supported \n") ;
                      exit (12) ;
                    }
#else
            sttynew.c_iflag = IGNBRK ;
            sttynew.c_oflag = 0 ;
            switch (baud)
              {
                case 2400 : 
                  {
                    cflag = B2400 ;
                    break ;
                  }
                case 4800 :
                  {
                    cflag = B4800 ;
                    break ;
                  }
                case 9600 :
                  {
                    cflag = B9600 ;
                    break ;
                  }
                case 19200 :
                  {
                    cflag = B19200 ;
                    break ;
                  }
                case 38400 :
                  {
                    cflag = B38400 ;
                    break ;
                  }
                default :
                  {
                    fprintf(stderr, "Invalid baudrate %d\n", baud) ;
                    exit(12) ;
                  }
              }
            switch (parity)
              {
                case 'E' :
                  {
                    cflag = cflag |  PARENB ;
                    break ;
                  }
                case 'O' :
                  {
                    cflag = cflag |  PARENB |  PARODD ;
                    break ;
                  }
              }
            if (flow)
                cflag = cflag |  CRTSCTS ;
            sttynew.c_cflag = CS8 |  CREAD |  CLOCAL |  cflag ;
            sttynew.c_lflag = NOFLSH ;
            sttynew.c_cc[VMIN] = 0 ;
            sttynew.c_cc[VTIME] = 0 ;

/* Configure port using new settings */
            if (ioctl(path, TCSETAW, &sttynew) == ERROR)
                {
                  fprintf (stderr, "Cound not configure port %s\n", port) ;
                  exit (12) ;
                }
#endif
            serial = TRUE ;
          }
      else if (ipport[0] == '\0')
          {
            fprintf (stderr, "No serial or IP port specified\n") ;
            exit (12) ;
          }
        else
          {
            if (udplink)
                {
                  if (ipaddr[0] == '\0')
                      {
                        fprintf (stderr, "No IP address specfied for UDP link\n") ;
                        exit (12) ;
                      }
                  sockfd = socket(AF_INET, SOCK_DGRAM, 0) ;
                }
              else
                sockfd = socket(AF_INET, SOCK_STREAM, 0) ;
            if (sockfd < 0)
                {
                  fprintf (stderr, "Could not open stream socket\n") ;
                  exit (12) ;
                }
            memset ((pchar) &serv_addr, sizeof(serv_addr), 0) ;
            serv_addr.sin_family = AF_INET ;
#ifdef  MYIPADDR
            if (myipaddr[0] != 0)
                {
                  /* Accept either ip address or hostname. */
                  char *ha = myipaddr ;
                  unsigned long mynetaddr ;
                  if ((int)(mynetaddr = inet_addr(myipaddr)) == -1)
                      {
                        struct hostent *hp;
                        if ((hp=gethostbyname(myipaddr)) == NULL)
                            {
                              fprintf (stderr, "Could not recognize ip address %s\n", myipaddr) ;
                              exit (12) ;
                            }
                          else
                            {
                              memcpy((char *)mynetaddr,hp->h_addr_list[0],sizeof(mynetaddr));
                            }
                      }
                  serv_addr.sin_addr.s_addr = mynetaddr ;
                }
              else
#endif
#if defined (LINUX)                                   /* CHANGED BY KIM */
            serv_addr.sin_addr.s_addr = INADDR_ANY;
            serv_addr.sin_port = flip2(atoi(ipport)) ;
#else
            serv_addr.sin_addr.s_addr = htonl(INADDR_ANY) ;
            serv_addr.sin_port = htons(atoi(ipport)) ;
#endif

#ifdef MYIPADDR
            printf ("Using myipaddr=%s\n",inet_ntoa(serv_addr.sin_addr)) ;
            printf ("Using ipport=%hu\n",ntohs(serv_addr.sin_port)) ;
            fflush(stdout);
#endif
            ruflag = 1 ; /* turn on REUSE option */
            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *) &ruflag, sizeof(ruflag)) < 0)
                {
                  fprintf (stderr, "Could not set REUSEADDR socket option\n") ;
                  exit (12) ;
                }
            ruflag = 1 ; /* turn on KEEPALIVE optin */
            if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *) &ruflag, sizeof(ruflag)) < 0)
                {
                  fprintf (stderr, "Could not set KEEPALIVE socket option\n") ;
                  exit (12) ;
                }
            if (bind(sockfd, (psockaddr) &serv_addr, sizeof(serv_addr)) < 0)
                {
                  fprintf (stderr, "Could not bind local address\n") ;
                  exit (12) ;
                }
            path = -1 ;
#ifdef _OSK
            _gs_opt(sockfd, &sttynew) ;
            sttynew.sg_noblock = 1 ;
            _ss_opt(sockfd, &sttynew) ;
#else
            flags = fcntl(sockfd, F_GETFL, 0) ;
            flags = flags |  FNDELAY ;
            fcntl(sockfd, F_SETFL, flags) ;
#endif
            if (udplink)
                { /* who to send packets to */
                  memset ((pchar) &cli_addr, sizeof(cli_addr), 0) ;
                  cli_addr.sin_family = AF_INET ;
 #ifdef LINUX                                    /* CHANGED BY KIM */
                  cli_addr.sin_addr.s_addr = (inet_addr(ipaddr)) ;
                  cli_addr.sin_port = flip2(atoi(ipport)) ;
#else
                  cli_addr.sin_addr.s_addr = htonl(inet_addr(ipaddr)) ;
                  cli_addr.sin_port = htons(atoi(ipport)) ;
#endif
                  path = sockfd ; /* fake it being opened */
                }
              else
                listen (sockfd, 1) ;
            netdly_cnt = netdly - 1 ; /* we can try to connect immediately */
          }
            

/* Client will send a SIGALRM signal to me when it puts it's segment ID into the
   service queue, So make sure we don't die on it, just exit sleep.
*/
      signal (SIGALRM, SIG_IGN) ;
      signal (SIGPIPE, SIG_IGN) ; 
/* Intercept various abort type signals so they close the socket, which the OS
   apparently doesn't do when the server is abnormally terminated
*/
      signal (SIGHUP, cleanup) ;
      signal (SIGINT, cleanup) ;
      signal (SIGQUIT, cleanup) ;
      
      stop = 0 ;
      uppoll = 0 ;
      total = 0 ;
      zerocnt = 0 ;
      tscan = 0 ;
      lasttotal = 0 ;
      services = 0 ;
      min = 32767 ;
      max = 0 ;
      first = TRUE ;
      ctmax = 0 ;
      ctmin = 2147483647 ;
      ctcount = 0 ;
      cttotal = 0 ;
      src = (pchar) &sbuf ;
      srcend = src ;
    /* destend must be past right end of packet */
      destend = (pchar) ((long) &dbuf.crc_low + sizeof(short)) ;
      dest = (pchar) &dbuf.seq ; /* skip "skipped" */
      gcrcinit () ;
      for (i = 0 ; i < 14 ; i++)
        {
          ultra_seg[i] = 0 ;
          detavail_seg[i] = 0 ;
        }
      if (linkstat.ultraon)
          request_link () ;
#ifdef SOLARIS2
      rqtp.tv_sec = (1000 * polltime) / 1000000000 ;
      rqtp.tv_nsec = (1000 * polltime) % 1000000000 ;
#endif
#ifdef LINUX
      rqtp.tv_sec = (1000 * polltime) / 1000000000 ;
      rqtp.tv_nsec = (1000 * polltime) % 1000000000 ;
#endif
#ifdef _OSK
      if (polltime < 11000)
          pollslp = 1 ;
        else
          pollslp = 0x80000000 |  (polltime / 3906) ;
#endif
      lastsec = dtime () ;
      do
        {
          check_input () ;
#ifdef _OSK
          set_verb (longhex(pvopt->verbosity)) ;
#endif
          tscan++ ;
          did = 0 ;
/* Look for service requests */
          for (cur = 0 ; cur < MAXCLIENTS ; cur++)
            {
              clientid = base->svcreqs[cur].clientseg ;
              if (clientid != NOCLIENT)
                  {
/* Search in table to find previously registered client */
                    cursvc = NULL ;
                    for (i = 0 ; i < highclient ; i++)
                      if (clients[i].client_memid == clientid)
                          {
                            cursvc = clients[i].client_address ;
                            if (cursvc->client_pid != clients[i].client_pid)
                                cursvc = NULL ; /* not a complete match */
                              else
                                break ; /* found a complete match of segment ID and PID */
                          }
                    if (cursvc == NULL)
                        {
                          cursvc = (pclient_struc) shmat(clientid, NULL, 0) ;
/*:: DSN start add */
			  /* If we cannot attach to client's shared memory, skip client. */
			  if (cursvc == (pclient_struc) ERROR)
			      {
				clients[i].client_address = NULL;
				continue;
			      }
/*:: DSN end add */
                          found = FALSE ;
                          for (i = 0 ; i < highclient ; i++)
                            if ((clients[i].client_name.l == cursvc->myname.l) &&
                               ((clients[i].client_pid == cursvc->client_pid) ||
                                (clients[i].client_pid == NOCLIENT) || (i < resclient)))
                                {
                                  found = TRUE ; /* Name and PID match, new segment */
                                  clients[i].client_memid = clientid ;
                                  clients[i].client_pid = cursvc->client_pid ;
                                  clients[i].client_uid = cursvc->client_uid ;
                                  clients[i].client_address = cursvc ;
                                  if (verbose)
                                      {
                                        if (i < resclient)
                                            if (clients[i].timeout)
                                                printf("Blocking") ;
                                              else
                                                printf("Reserved") ;
                                          else
                                            printf("Returning") ;
                                        printf (" client %4.4s, with PID of %d with memory ID %d\n",
                                                &clients[i].client_name,
                                                cursvc->client_pid, clientid) ;
                                        fflush (stdout) ;
                                      }
                                  if (clients[i].timeout) /* client can be blocking */
                                      {
                                        curclient = (pclient_station) ((long) cursvc +
                                                     cursvc->offsets[cursvc->curstation]) ;
                                        if (curclient->blocking)
                                            { /* client is taking blocking option */
                                              clients[i].blocking = TRUE ;
                                              set_bit (&blockmask, i) ;
                                            }
                                          else
                                            { /* client elected not to block */
                                              clients[i].blocking = FALSE ;
                                              unblock(i) ;
                                              clr_bit (&blockmask, i) ;
                                            }
                                      }
                                  break ;
                                }
                          if (!found)
                              {
                                i = highclient++ ;
                                if (i >= MAXCLIENTS)
                                    {
                                      cursvc->error = CSCR_REFUSE ;
                                      highclient-- ;
                                    }
                                  else
                                    {
                                      clients[i].client_memid = clientid ;
                                      clients[i].client_pid = cursvc->client_pid ;
                                      clients[i].client_uid = cursvc->client_uid ;
                                      clients[i].client_address = cursvc ;
                                      clients[i].client_name.l = cursvc->myname.l ;
                                      if (verbose)
                                          {
                                            if (cursvc->client_uid == base->server_uid)
                                                printf ("New client %4.4s, with PID %d and memory ID %d\n", 
                                                         &clients[i].client_name,
                                                         cursvc->client_pid, clientid) ;
                                              else
                                                printf ("New Foreign client %4.4s, with PID %d, UID %d, and memory ID %d\n", 
                                                         &clients[i].client_name, cursvc->client_pid,
                                                         cursvc->client_uid, clientid) ;
                                            fflush (stdout) ;
                                          }
                                    }
                              }
                        }
                    if (cursvc->error == OK)
                        {
                          did++ ;
                          services++ ;
                          if ((services % 1000) == 0)
                              {
                                ct = cttotal / ctcount ;
                                if (rambling)
                                    {
                                      printf("%d CLient services, %d read calls, %d avg. time\n",
                                             services, ctcount, ct) ;
                                      fflush (stdout) ;
                                    }
                              }
                          clients[i].last_service = dtime () ;
                          clients[i].active = TRUE ;
                          cursvc->error = handler(cursvc, i) ;
                        }
                    cursvc->done = TRUE ;
                    base->svcreqs[cur].clientseg = NOCLIENT ;
                    if (cursvc->client_uid == base->server_uid)
#ifdef _OSK
                        kill (cursvc->client_pid, SIGWAKE) ;
#else
                        kill (cursvc->client_pid, SIGALRM) ;
#endif
                    if (did >= MAXPROC)
                        break ;
                  }
            }
          check_input () ;
          curtime = dtime () ;
          if ((curtime - lastsec) >= 1.0)
              {
                uppoll++ ;
                lastsec = curtime ;
                check_clients () ;
                if (!serial)
                    if ((path >= 0) && ! udplink)
                        {
                          if (netto_cnt++ >= netto)
                              {
                                if (verbose)
                                    printf("%d Second network timeout, closing connection to DA\n", netto) ;
                                shutdown (path, 2) ;
                                close (path) ;
                                path = -1 ;
                                netto_cnt = 0 ;
                                netdly_cnt = 0 ;
                                seq_valid = FALSE ;
                              }
                        }
                    else if ((++netdly_cnt == netdly) && (rambling)
                             && !udplink)
                        printf("Checking for pending network connection request from DA\n") ;
                if (path >= 0)
                    {
                      if (linkstat.ultraon && (!linkstat.linkrecv))
                          if (++linkpoll >= link_retry)
                              request_link () ;
                      if (mappoll != 0)
                          if (--mappoll == 0)
                              request_map () ;
                      if (linkstat.ultraon && linkstat.linkrecv && (!linkstat.ultrarecv)
                          && (!notify) && (!noultra) && (++minctr >= 60))
                          {
                            minctr = 0 ;
                            request_ultra () ;
                          }
                      if ((mappoll != 0) && (--mappoll <= 0))
                          request_map () ;
                    }                
              }
          if ((txwin > 0) && (curtime > last_sent + grptime))
              send_window () ; 
          check_input () ;
          if (xfer_up_ok)
              {
                if ((path >= 0) && (uppoll >= 0))
                    {
                      uppoll = 0 ;
                      next_segment () ;
                    }
              }
          if (verbose && (noackmask != oldackmask))
              {
                for (i = DATAQ ; i <= STOPACK ; i++)
                  if (verbose && (test_bit(noackmask ^ oldackmask, i)))
                      {
                        if (test_bit(noackmask, i))
                            printf("%s queue blocked at %s\n", queue_names[i], time_string(dtime())) ;
                          else
                            printf("%s queue unblocked at %s\n", queue_names[i], time_string(dtime())) ;
                        fflush (stdout) ;
                      }
                oldackmask = noackmask ;
              }
#ifdef SOLARIS2
          nanosleep (&rqtp, &rmtp) ;
/* ADDED BY KIM	*/
#elif defined LINUX
	  nanosleep (&rqtp, &rmtp) ;
#elif defined _OSK
          tsleep (pollslp) ;
#else
          usleep (polltime) ;
#endif
 
        }
      while (!stop) ;
      if (serial)
          {
            if (path >= 0)
                close(path) ;
          }
        else
          {
            if (path >= 0)
                {
                  shutdown(path, 2) ;
                  close(path) ;
                }
            shutdown(sockfd, 2) ;
            close(sockfd) ;
          }
#ifdef _OSK
      vopt_close (vopthdr) ;
#endif
      exit (12) ;
    }
