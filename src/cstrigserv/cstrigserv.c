#ifndef lint
static char sccsid[] = "@(#) $Id: cstrigserv.c,v 1.2 2003/02/01 19:03:33 lombard Exp $";
#endif

/*  Cstrigserv - ComServ Trigger Server program.			*/
/*  Douglas Neuhauser, UC Berkeley Seismological Laboratory		*/
/*	Copyright (c) 2000 The Regents of the University of California.	*/
/*  Based on sample client program written by:				*/
/*	Woodrow H. Owens, Quanterra, Inc.				*/
/*	Copyright 1994 Quanterra, Inc.					*/
/************************************************************************/

/************************************************************************/
/*  Valid trigger message formats:					*/
/*									*/
/*  1.  Explicitly set one or more commdet flags.			*/
/*  seq_no SET commdet1 value1 commdet2 value2 ... commdetN valueN	*/
/*	Set the specified commdet flags to the specified values.	*/
/*	1 = enable, 0 = disable.					*/
/*									*/
/*  2.  Trigger one or more commdet flags for a specified duration.	*/
/*  seq_no TRIGGER duration commdet1 commdet2 ... commdetN		*/
/*	Set the specified commdet flags to trigger for the specified	*/
/*	duration (in seconds).						*/
/*									*/
/*  3.	Reset server.							*/
/*  seq_no  RESET							*/
/*									*/
/*  seq_no: unsigned integer						*/
/*  commdet1 ... commdetN:  Comm detector names (eg HON, BON, etc).	*/
/*	    At most 32 commdet strings per message.			*/
/*  duration:	duration time for trigger in seconds.			*/
/*									*/
/*									*/
/*  Each UNIQUE message should have a UNIQUE sequence number, and 	*/
/*  should be monotonically increasing.					*/
/************************************************************************/

#define	VERSION		"1.0beta (2000.074)"

#include <stdio.h>

char *syntax[] = {
"%s version " VERSION,
"%s    [-d n] [-v n] [-h] station",
"    where:",
"	-d n	    Debug setting",
"		    1 = debug msg",
"		    2 = debug ip",
"		    4 = debug queue",
"		    8 = no execute",
"	-v n	    Verbose setting",
"		    1 = print receipt line for each packet.",
"		    2 = display polling info.",
"	-h	    Print brief help message for syntax.",
"	station	    Comserv station name",
NULL };

#include <errno.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#ifndef	_OSK
#include <unistd.h>
#include <termio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/file.h>
#else
#include <stdlib.h>
#include <sgstat.h>
#include <module.h>
#include <types.h>
#include <sg_codes.h>
#include <modes.h>
#include "os9inet.h"
char *strdup();
#endif

#include <time.h>
#include <math.h>
double log2(double);
#ifndef	_OSK
#include <sys/time.h>
#include <arpa/inet.h>
#else
/*#include <inet/in.h>*/
#include <inet/netdb.h>
#endif

#include "dpstruc.h"
#include "seedstrc.h"
#include "stuff.h"
#include "timeutil.h"
#include "service.h"
#include "cfgutil.h"

#define	CLIENT_NAME	"CSTR"
#define	TIMESTRLEN  	40
#define	CFGSTRLEN	160
#define	DEFAULT_DATA_MASK \
	(CSIM_MSG)
#define	MAX_SELECTORS	CHAN+2

#define info stderr

#define DAT_INDEX	DATAQ
#define DET_INDEX	DETQ
#define	CAL_INDEX	CALQ
#define	CLK_INDEX	TIMQ
#define	LOG_INDEX	MSGQ
#define	BLK_INDEX	BLKQ

#define	MAXADDR	    16
char *remaddrstr[MAXADDR];
char myaddrstr[CFGSTRLEN];
char ipportstr[CFGSTRLEN];
int naddr = 0;
int path = -1 ;
int sockfd = -1 ;
struct sockaddr_in serv_addr, rem_addr;
typedef struct sockaddr *psockaddr ;

#define	MAXMSGLEN	128
#define	ERROR_PARSE	-1
#define WAIT_SECONDS 30

#define	COMMDET_RESET	1
#define	COMMDET_SET	2
#define	COMMDET_TRIG	3

typedef struct commdet {
    char name[COMMLENGTH+1];
    int value;
} COMMDET;

typedef struct msg {
    int seq;		/* sequence number.				*/
    int op;		/* operation: COMMDET_SET or COMMDET_TRIG	*/
    int duration;	/* duration in seconds (0 = no duration)	*/
    unsigned int etime;	/* time trigger should end (computed).		*/
    COMMDET commdet[CE_MAX];
} TRIGGER;

TRIGGER tmsg;
time_t now;

typedef struct commqueue {
    char name[COMMLENGTH+1];
    unsigned int etime;
} COMMQUEUE;

#define	MAX_ETIME   2147483647
COMMQUEUE pending[CE_MAX];
unsigned int next_etime = MAX_ETIME;

#define debug(opt) (debug_flag & opt)
#define	DEBUG_COMM	1
#define	DEBUG_IP	2
#define	DEBUG_QUEUE	4
#define	DEBUG_NOEXEC	8
#define	DEBUG_FLOW	16
#define	DEBUG_ANY (DEBUG_COMM | DEBUG_IP | DEBUG_NOEXEC | DEBUG_FLOW)

/************************************************************************/
/*  Externals required in multiple files or routines.			*/
/************************************************************************/
char pidfile[1024];			/* pid file.			*/
short data_mask = DEFAULT_DATA_MASK;	/* data mask for cs_setup.	*/

char *cmdname;				/* Name of this program.	*/
char station[8];			/* station name.		*/
char client_name[5] = CLIENT_NAME;	/* client name			*/
char lockfile[160];			/* Name of optional lock file.	*/
int lockfd;				/* Lockfile file desciptor.	*/
int verbosity;				/* verbosity setting.		*/
int debug_flag;				/* debug_flag			*/

static int terminate_proc;		/* flag to terminate program.	*/
static pclient_struc me = NULL;		/* comserv shared mem ptr.	*/
typedef char char23[24];
static char23 stats[11] =
    {	"Good", "Enqueue Timeout", "Service Timeout", "Init Error",
	"Attach Refused", "No Data", "Server Busy", "Invalid Command",
	"Server Dead", "Server Changed", "Segment Error" };

/* These logically belong in main, but are too large for the stack. */
tstations_struc tstations;
char msg[MAXMSGLEN];
char errmsg[256];
char str1[CFGSTRLEN], str2[CFGSTRLEN];
char station_dir[CFGSTRLEN], station_desc[CFGSTRLEN], source[CFGSTRLEN];
char configfile[256];
char time_str[TIMESTRLEN];

#ifdef	_OSK
char *__progname;		/* used in getopt()	*/
struct sgbuf sttynew ;
#define	EAGAIN	EWOULDBLOCK
#define	EIO 	-1
#define	STATIONS_FILE	"/r0/stations.ini"
#else
#define	STATIONS_FILE	"/etc/stations.ini"
#endif

/************************************************************************/
/*  Function declarations.						*/
/************************************************************************/
void finish_handler(int sig);
int terminate_program (int error);
char *cs_errmsg (short int err);
short int wait_finished (comstat_rec *pcomm);

/************************************************************************/
/*  print_syntax:							*/
/*	Print the syntax description of program.			*/
/************************************************************************/
int print_syntax
   (char	*cmd,		/* program name.			*/
    char	*syntax[],	/* syntax array.			*/
    FILE	*fp)		/* FILE ptr for output.			*/
{
    int i;
    for (i=0; syntax[i] != NULL; i++) {
	fprintf (fp, syntax[i], cmd);
	fprintf (fp, "\n");
    }
    return (0);
}

/************************************************************************/
/*  main program.							*/
/************************************************************************/
main (int argc, char **argv)
{
    pclient_station this;
    config_struc cfg, station_cfg;
    short j, k, err;
    int status;
    boolean alert;
    pdata_user pdat;
    seed_record_header *pseed;
    int flags, ruflag ;
    int len;
    int i;

    int shared_command_flag = TRUE;
    int blocking_flag = FALSE;
    int n_selectors = 1;

    /* Variables needed for getopt. */
    extern char	*optarg;
    extern int	optind, opterr;
    int		c;

/*::
	printf ("hello, world\n");
	if (1) return(0);
::*/	
    cmdname = ((cmdname=strrchr(argv[0],'/'))) ? ++cmdname : argv[0];
#ifdef	OSK
	__progname = cmdname;
#endif
    configfile[0] = '\0';
    while ( (c = getopt(argc,argv,"hv:d:")) != -1)
      switch (c) {
	case '?':
        case 'h':   print_syntax(cmdname,syntax,info); exit(0);
	case 'v':   verbosity=atoi(optarg); break;
	case 'd':   debug_flag=atoi(optarg); break;
	case 'c':   strcpy(configfile,optarg); break;
      }

    /*	Skip over all options and their arguments.			*/
    argv = &(argv[optind]);
    argc -= optind;

    /* Allow override of station name on command line */
    if (argc > 0) {
	strncpy(station, argv[0], 4);
	station[4] = '\0';
    }
    else {
	fprintf (stderr, "Missing station name\n");
	exit(1);
    }
    upshift(station);

    if (configfile[0] == '\0') {
	/* Look for station entry in master station file.		*/
	strcpy (configfile, STATIONS_FILE);
	if (open_cfg(&cfg, configfile, station)) {
	    fprintf (stderr,"Could not find station entry for %s\n", station);
	    exit(1);
	}

	/* Look for station directory, source, and description.		*/
	while (1) {
	    read_cfg(&cfg, str1, str2);
	    if (str1[0] == '\0') break;
	    if (strcmp(str1, "DIR") == 0)
		strcpy(station_dir, str2);
	    else if (strcmp(str1, "SOURCE") == 0)
		strcpy(source, str2);
	    else if (strcmp(str1, "DESC") == 0) {
		strncpy(station_desc, str2, 60);
		station_desc[59] = '\0';
	    }
	}
	close_cfg(&cfg);
      
	/* Look for client entry in station's station.ini file.		*/
	addslash (station_dir);
	strcpy (configfile, station_dir);
	strcat (configfile, "station.ini");
    }
    else {
	station_desc[0] = '\0';
    }
    printf ("%s %s startup - %s\n", localtime_string(dtime()),
	    station, station_desc);

    /* Open and read station config file.				*/
    if (open_cfg(&cfg, configfile, client_name)) {
	fprintf (stderr, "Could not find section %s in station file %s\n", 
		 client_name, configfile);
	exit(1);
    }
	if (debug(DEBUG_FLOW)) {
		fprintf (info, "opening station config file\n");
		fflush (info);
	}
          
    /* Extract required info in source section of this station.	*/
    while (1) {
	read_cfg(&cfg,str1,str2);
	if (str1[0] == '\0') break;
	if (strcmp(str1,"IPPORT")==0) {		    /* ip port to listen to for msgs.	*/
	    strcpy(ipportstr,str2);
	}
	else if (strncmp(str1,"UDPADDR",7)==0) {    /* valid address to receive trigger msgs from. */
	    if (naddr < MAXADDR) {
		remaddrstr[naddr++] = strdup(str2);
	    }
	    else {
		fprintf (stderr, "Error: number of UDPADDR lines exceeds max of %d\n", MAXADDR);
		exit(1);
	    }
	}
	else if (strcmp(str1,"MYADDR")==0) {	    /* my ip address for responses (optional).	*/
	    strcpy(myaddrstr,str2);
	}
	else if (strcmp(str1,"LOCKFILE")==0) {
	    strcpy(lockfile,str2);
	}
    }
    close_cfg(&cfg);

#ifndef _OSK
    /* Open the lockfile for exclusive use if lockfile is specified.	*/
    /* This prevents more than one copy of the program running for	*/
    /* a single station.						*/
    if (strlen(lockfile) > 0) {
	if ((lockfd = open (lockfile, O_RDWR|O_CREAT,0644)) < 0) {
	    fprintf (info, "Unable to open lockfile: %s\n", lockfile);
	    exit(1);
	}
	if ((status=lockf (lockfd, F_TLOCK, 0)) < 0) {
	    fprintf (info, "Unable to lock daemon lockfile: %s status=%d errno=%d\n", 
		     lockfile, status, errno);
	    close (lockfd);
	    exit(1);
	}
    }
#endif

	if (debug(DEBUG_FLOW)) {
		fprintf (info, "Opening socket\n");
		fflush (info);
	}
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    memset ((pchar) &serv_addr, sizeof(serv_addr), 0) ;
    serv_addr.sin_family = AF_INET ;
    if (myaddrstr[0]) {
	serv_addr.sin_addr.s_addr = inet_addr(myaddrstr) ;
    }
    else {
	serv_addr.sin_addr.s_addr = INADDR_ANY ;
    }
    serv_addr.sin_port = htons((unsigned short)atoi(ipportstr)) ;
    ruflag = 1 ; /* turn on REUSE option */
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *) &ruflag, sizeof(ruflag)) < 0) {
	fprintf (stderr, "Could not set REUSEADDR socket option\n") ;
	exit (12) ;
    }
    ruflag = 1 ; /* turn on KEEPALIVE option */
    if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *) &ruflag, sizeof(ruflag)) < 0) {
	fprintf (stderr, "Could not set KEEPALIVE socket option\n") ;
	exit (12) ;
    }
    if (bind(sockfd, (psockaddr) &serv_addr, sizeof(serv_addr)) < 0) {
	fprintf (stderr, "Could not bind local address\n") ;
	exit (12) ;
    }
#ifdef _OSK
    _gs_opt(sockfd, &sttynew) ;
    sttynew.sg_noblock = 1 ;
    _ss_opt(sockfd, &sttynew) ;
#else
    flags = fcntl(sockfd, F_GETFL, 0) ;
    flags = flags | FNDELAY ;
    fcntl(sockfd, F_SETFL, flags) ;
#endif
	if (debug(DEBUG_FLOW)) {
		fprintf (info, "Socket opened\n");
		fflush (info);
	}

    path = sockfd ; /* fake it being opened */

    terminate_proc = 0;
    signal (SIGINT,finish_handler);
    signal (SIGTERM,finish_handler);

	if (debug(DEBUG_FLOW)) {
		fprintf (info, "CS setup\n");
		fflush (info);
	}
    /* Generate an entry for all available stations */      
    cs_setup (&tstations, client_name, station, shared_command_flag, 
	      blocking_flag, 10, n_selectors, data_mask, 6000);

    /* Create my segment and attach to all stations */      
    me = cs_gen (&tstations);

	if (debug(DEBUG_FLOW)) {
		fprintf (info, "Done CS setup\n");
		fflush (info);
	}
    /* Show beginning status of all stations */
    strcpy(time_str, localtime_string(dtime()));
    for (j = 0; j < me->maxstation; j++) {
	this = (pclient_station) ((long) me + me->offsets[j]);
	printf ("%s - [%s] Status=%s\n", time_str, long_str(this->name.l), 
		(stats[this->status]));
/*::
	printf ("%s\n", time_str);
	fflush (stdout);
	printf ("[%s]\n", long_str(this->name.l));
	fflush (stdout);
	printf ("status=%d\n", this->status);
	fflush (stdout);
	printf ("string=%s\n", stats[this->status]);
::*/
	fflush (stdout);
    }
	
    fflush (stdout);
	if (debug(DEBUG_FLOW)) {
		fprintf (info, "Done all setup\n");
		fflush (info);
	}


    /* Loop until we are terminated by outside signal.			*/
    while (! terminate_proc) {
	status = get_msg (msg, &len, &rem_addr);
	if (debug(DEBUG_FLOW)) {
		fprintf (info, "Done get_msg\n");
		fflush (info);
	}
	if (status == OK) {
	    status = parse_msg (msg, &tmsg, errmsg);
	    if (status == OK) {
		set_detectors (&tmsg, errmsg);
		send_ack(&rem_addr, tmsg.seq, OK, errmsg);
	    }
	    else {
		send_ack(&rem_addr, tmsg.seq, ERROR_PARSE, errmsg);
	    }
	}
	else if (status == EAGAIN) {
	    now = time(NULL);
	    if (now > next_etime) {
		set_detectors (NULL, errmsg);
	    }
	}
	else {
	    /* Unknown error - abort */
	    terminate_proc = 1;
	    continue;
	}
	j = cs_scan (me, &alert);
	if (j != NOCLIENT) {
	    this = (pclient_station) ((long) me + me->offsets[j]);
	    if (alert) {
		strcpy(time_str, localtime_string(dtime()));
		printf("%s - New status on station %s is %s\n", time_str,
		       long_str(this->name.l), (stats[this->status]));
		fflush (stdout);
	    }
	}
	else {
	    if (verbosity & 2) {
		printf ("sleeping...");
		fflush (stdout);
	    }
	    sleep (1);		/* Bother the server once every second */
	    if (verbosity & 2) {
		printf ("awake\n");
		fflush (stdout);
	    }
	}
    }
    terminate_program (0);
    return(0);
}

void finish_handler(int sig)
{
    terminate_proc = 1;
    signal (sig,finish_handler);    /* Re-install handler (for SVR4)	*/
}

/************************************************************************/
/*  terminate_program							*/
/*	Terminate prog and return error code.  Clean up on the way out.	*/
/************************************************************************/
int terminate_program (int error) 
{
    pclient_station this;
    char time_str[TIMESTRLEN];
    int j;
    boolean alert;

    strcpy(time_str, localtime_string(dtime()));
    if (verbosity & 2) {
	printf ("%s - Terminating program.\n", time_str);
	fflush (stdout);
    }

    /* Perform final cs_scan for 0 records to ack previous records.	*/
    /* Detach from all stations and delete my segment.			*/
    if (me != NULL) {
	for (j=0; j< me->maxstation; j++) {
	    this = (pclient_station) ((long) me + me->offsets[0]);
	    this->reqdbuf = 0;
	}
	strcpy(time_str, localtime_string(dtime()));
	printf ("%s - Final scan to ack all received packets\n", time_str);
	fflush (stdout);
	cs_scan (me, &alert);
	cs_off (me);
    }

    shutdown(path, 2) ;
    close(path) ;
    shutdown(sockfd, 2) ;
    close(sockfd) ;
#ifdef _OSK
/*::      vopt_close (vopthdr) ;*/
#endif

    if (strlen(pidfile)) unlink(pidfile);
    if (lockfd) close(lockfd);
    strcpy(time_str, localtime_string(dtime()));
    printf ("%s - Terminated\n", time_str);
    exit(error);
}

/************************************************************************/
/*  get_msg - read a UDP trigger message.				*/
/*	Return:	OK when msg read, EAGAIN when no msg, EIO on error.	*/
/************************************************************************/
int get_msg (char *msg, int *len, struct sockaddr_in *rem_addr)
{
    static char str[MAXMSGLEN];
    int n;
    int l = sizeof(struct sockaddr_in);
    n = recvfrom (path, str, MAXMSGLEN, 0, (psockaddr)rem_addr, &l);
    if (n == -1 && errno == EAGAIN) {
	*len = 0;
	if (debug(DEBUG_IP)) {
	    if (verbosity) printf ("no message\n");
	}
	return (errno);
    }
    if (n > 0) {
	memcpy (msg, str, n);
	msg[n] = '\0';
	*len = n;
	if (debug(DEBUG_IP)) {
	    printf ("recv msg from %s:%d len=%d msg=%s\n",
		    inet_ntoa(rem_addr->sin_addr),
		    (int)rem_addr->sin_port, *len, msg);
	}
	return (OK);
    }
    /* Error of some sort. */
    if (debug(DEBUG_IP)) {
	printf ("recv msg error %d\n", errno);
    }
    return (EIO);
}

/************************************************************************/
/*  send_ack - send acknowledge message with status return.		*/
/*	Return:	0 on success, -1 on error.				*/
/************************************************************************/
int send_ack (struct sockaddr_in *rem_addr, int seq, int status, char *errmsg)
{
    static char ack[MAXMSGLEN];
    /* who to send packets to */
    sprintf (ack, "%d %d %s", seq, status, errmsg);
    status = sendto (path, ack, strlen(ack), 0, (psockaddr)rem_addr, sizeof(struct sockaddr_in));
    if (debug(DEBUG_IP)) {
	printf ("send ack to %s:%d status=%d ack=%s\n", inet_ntoa(rem_addr->sin_addr),
		(int)rem_addr->sin_port, status, ack);
    }
    return (status);
}

#define	WHITESPACE  " \t"
/************************************************************************/
/*  parse_msg - parse a UDP trigger message.				*/
/*	Return:	OK for valid msg, ERROR for invalid msg.		*/
/************************************************************************/
int parse_msg (char *msg, TRIGGER *tmsg, char *errmsg) 
{
    static char str[MAXMSGLEN];
    char str1[32];
    char *p;
    int ival1, ival2, n;

    errmsg[0] = '\0';
    memset (tmsg, 0, sizeof(TRIGGER));
    strcpy (str, msg);

    p = strtok(str,WHITESPACE);
    tmsg->seq = 0;
    if (p) tmsg->seq = atoi(p);
    else {
	strcpy (errmsg, "Missing sequence number");
	return (ERROR);
    }

    p = strtok(NULL,WHITESPACE);
    upshift (p);
    if (strcmp(p,"SET")==0) {
	tmsg->op = COMMDET_SET;
    }
    else if (strcmp(p,"TRIGGER")==0) {
	tmsg->op = COMMDET_TRIG;
    }
    else if (strcmp(p,"RESET")==0) {
	tmsg->op = COMMDET_RESET;
    }
    else {
	sprintf (errmsg, "Unknown command: %s", p);
	return (ERROR);
    }

    n = 0;
    switch (tmsg->op) {
      case COMMDET_SET:
	while ((p=strtok(NULL,WHITESPACE)) && n<CE_MAX) {
	    strcpy(tmsg->commdet[n].name,p);
	    upshift(tmsg->commdet[n].name);
	    p = strtok(NULL,WHITESPACE);
	    if (p) tmsg->commdet[n].value = atoi(p);
	    else {
		sprintf (errmsg, "Missing SET value for detector %s", tmsg->commdet[n].name);
		return (ERROR);
	    }
	    n++;
	}
	break;
      case COMMDET_TRIG:
	p = strtok(NULL,WHITESPACE);
	if (p) tmsg->duration = atoi(p);
	else {
	    sprintf (errmsg, "Missing duration for TRIGGER msg");
	    return (ERROR);
	}
	while ((p=strtok(NULL,WHITESPACE)) && n<CE_MAX) {
	    strcpy(tmsg->commdet[n].name,p);
	    upshift(tmsg->commdet[n].name);
	    tmsg->commdet[n].value = 1;
	    n++;
	}
	if (p) {
	    sprintf (errmsg, "Number of TRIGGER comm detectors > %d", CE_MAX);
	    return (ERROR);
	}
	now = time(NULL);
	tmsg->etime = now + tmsg->duration;
	break;
      case COMMDET_RESET:
	p = strtok(NULL,WHITESPACE);
	if (p) {
	    sprintf (errmsg, "Extraneous tokens on RESET msg");
	    return (ERROR);
	}
	break;
      default:
	sprintf (errmsg, "Unknown command");
	return (ERROR);
    }
    return (OK);
}

/************************************************************************/
/*  set_detectors - set comm detector flag(s).				*/
/*	Add detector to pending table with ending time if message is	*/
/*	type TRIGGER.							*/
/*	Return:	OK for success, ERROR for failure.			*/
/************************************************************************/
int set_detectors (TRIGGER *tmsg, char *msg)
{
    pclient_station this;
    comstat_rec *pcomm ;
    ultra_rec *pur ;
    comm_event_com *pcec ;
    int n, i, found, enable;
    short int err;
    char s1[32];
    char s2[32];
    pchar pc1;
    long int ltemp, mtemp ;
    int save_err = 0;
    int avail;

    msg[0] = '\0';
    this = (pclient_station) ((long) me + me->offsets[0]) ;
    pcomm = (pvoid) ((long) me + this->comoutoffset) ;
    pcomm->completion_status = CSCS_IDLE ;

    this->command = CSCM_ULTRA ;
    err = cs_svc(me, 0) ;
    if (err != CSCR_GOOD) {
	strcpy (msg, cs_errmsg(err)) ;
	return (err);
    }
    pcomm->completion_status = CSCS_IDLE ;
    pur = (pvoid) &pcomm->moreinfo ;
    ltemp = 0 ;
    mtemp = 0 ;

    /* Process server reset.	*/
    if (tmsg != NULL && tmsg->op == COMMDET_RESET) {
	memset (&pending, 0, sizeof(pending));
	next_etime = MAX_ETIME;
	return (OK);
    }

    /* Set comm detectors to the specified values.  */
    if (tmsg != NULL) {
	/* Interate over the detector names in the message. */
	for (n=0; n<CE_MAX; n++) {
	    found = TRUE;
	    if (strlen(tmsg->commdet[n].name) == 0) continue;
	    found = FALSE ;
	    i = strlen(s1) - 1 ;
	    enable = tmsg->commdet[n].value;
	    pc1 = (pvoid) &pur->commnames ;
	    /* Iterate over the station's comm detector flags (pascal strings) */
	    for (i = 0 ; i < CE_MAX ; i++) {
		strpcopy (s2, pc1) ;
		if (strcmp(s2, tmsg->commdet[n].name) == 0) {
		    set_bit (&mtemp, i) ; /* change this bit */
		    if (enable) {
			set_bit (&ltemp, i) ;
			if (debug(DEBUG_COMM)) printf ("Setting %s\n",tmsg->commdet[n].name);
		    }
		    else {
			clr_bit (&ltemp, i) ;
			if (debug(DEBUG_COMM)) printf ("Clearing %s\n",tmsg->commdet[n].name);
		    }
		    found = TRUE ;
		    break ;
		    }
		else {
		    pc1 = (pchar) ((long) pc1 + *pc1 + 1) ;
		    }
		}
	    if (! found) {
		sprintf (msg, "Comm detector %s not found", tmsg->commdet[n].name);
		if (debug(DEBUG_COMM)) printf ("%s\n", msg);
		return (ERROR);
		}
	    }
    }
    else {
	/* Interate over pending list.  */
	if (debug(DEBUG_QUEUE)) {
	    if (verbosity) printf ("Checking queue\n");
	}
	for (n=0; n<CE_MAX; n++) {
	    found = TRUE;
	    if (strlen(pending[n].name) == 0) continue;
	    if (pending[n].etime > now) continue;
	    found = FALSE ;
	    i = strlen(s1) - 1 ;
	    enable = 0;
	    pc1 = (pvoid) &pur->commnames ;
	    /* Iterate over the station's comm detector flags (pascal strings) */
	    for (i = 0 ; i < CE_MAX ; i++) {
		strpcopy (s2, pc1) ;
		if (strcmp(s2, pending[n].name) == 0) {
		    set_bit (&mtemp, i) ; /* change this bit */
		    if (enable) {
			set_bit (&ltemp, i) ;
			if (debug(DEBUG_COMM)) printf ("Setting %s\n",pending[n].name);
		    }
		    else {
			clr_bit (&ltemp, i) ;
			if (debug(DEBUG_COMM)) printf ("Clearing %s\n",pending[n].name);
		    }
		    found = TRUE ;
		    break ;
		    }
		else {
		    pc1 = (pchar) ((long) pc1 + *pc1 + 1) ;
		    }
		}
	    if (! found) {
		/* Remove it from the pending list. */
		pending[n].name[0] = '\0';
		pending[n].etime = 0;
		sprintf (msg, "Comm detector %s not found - removed from pending list", pending[n].name);
		if (debug(DEBUG_COMM)) printf ("%s\n", msg);
		return (ERROR);
	    }
	}
    }
	
    /* Send this operation to comserv.	*/
    pcec = (pvoid) ((long) me + this->cominoffset) ;
    pcec->remote_map = ltemp ;
    pcec->remote_mask = mtemp ;
    this->command = CSCM_COMM_EVENT ;
    err = CSCR_GOOD;
    if (debug(DEBUG_COMM)) printf ("Issuing comserv command\n");
    if (! debug(DEBUG_NOEXEC)) err = cs_svc(me, 0) ;
    if (err == CSCR_GOOD) {
	err = CSCS_FINISHED;
	if (! debug(DEBUG_NOEXEC)) err = wait_finished (pcomm) ;
	if (err == CSCS_FINISHED) {
	    pcomm->completion_status = CSCS_IDLE ;
	    if (debug(DEBUG_COMM)) printf ("Comm Event Command completed\n") ;
	}
	else {
	    /* Return error unless this is a COMMDET_TRIG command and	*/
	    /* operation could still be pending.			*/
	    save_err = err;
	    strcpy (msg, cs_errmsg(err)) ;
	    if (! (tmsg != NULL && tmsg->op == COMMDET_TRIG)) return (err);
	}
    }
    else {
	strcpy (msg, cs_errmsg(err)) ;
	return (err);
    }

    /*	Operation was successful OR operation was a COMMDET_TRIG.	*/
    /*  Update pending records if operation operation was COMM_TRIGGER	*/
    /*	OR operation was from pending requests.				*/

    if (tmsg != NULL && tmsg->op == COMMDET_TRIG) {
	/* For TRIGGER message save the expiration time.	*/
	for (n=0; n<CE_MAX; n++) {
	    if (strlen(tmsg->commdet[n].name) == 0) continue;
	    avail = -1;
	    found = FALSE;
	    for (i=0; i<CE_MAX && ! found; i++) {
		if (avail < 0 && pending[i].name[0] == '\0') avail = i;
		if (strcmp(tmsg->commdet[n].name, pending[i].name) == 0) {
		    /* Update pending table entry for this detector if new etime is later. */
		    if (pending[i].etime < tmsg->etime) {
			pending[i].etime = tmsg->etime;
			if (debug(DEBUG_QUEUE)) {
			    printf ("Queue time for %s is %u < new etime %u.  Queue entry %d updated.\n",
				    tmsg->commdet[n].name, pending[i].etime, tmsg->etime, i);
			}
		    }
		    else {
			if (debug(DEBUG_QUEUE)) {
			    printf ("Queue time for %s is %u > new etime %u.  Queue entry %d not updated.\n",
				    tmsg->commdet[n].name, pending[i].etime, tmsg->etime, i);
			}
		    }
		    found = TRUE;
		}
	    }
	    if (! found) {
		/* Create new entry in pending table for this detector.	*/
		if (avail < 0) {
		    if (save_err == 0) {
			sprintf (msg, "Pending table full - unable to cache %s", tmsg->commdet[n].name);
			save_err = err;
		    }
		}
		else {
		    strcpy(pending[avail].name,tmsg->commdet[n].name);
		    pending[avail].etime = tmsg->etime;
		    if (debug(DEBUG_QUEUE)) {
			printf ("Adding %s to queue for time %u - queue entry %d\n", 
				tmsg->commdet[n].name, tmsg->etime, avail);
		    }
		}
	    }
	}

	/* Now find the earliest etime in the list. */
	next_etime = MAX_ETIME;
	for (i=0; i<CE_MAX; i++) {
	    if (pending[i].name[0] && pending[i].etime < next_etime)
		next_etime = pending[i].etime;
	}
	if (debug(DEBUG_QUEUE)) {
	    printf ("Next queued event time is %u\n", next_etime);
	}
    }

    /* We processed a pending request.  Remove it from the queue.   */
    if (tmsg == NULL) {
	for (n=0; n<CE_MAX; n++) {
	    found = TRUE;
	    if (strlen(pending[n].name) == 0) continue;
	    if (pending[n].etime > now) continue;
	    if (debug(DEBUG_QUEUE)) {
		printf ("Removing %s %u from queue entry %d\n", pending[n].name, pending[n].etime, n);
	    }
	    pending[n].etime = MAX_ETIME;
	    pending[n].name[0] = '\0';
	}
	/* Now find the earliest etime in the list. */
	next_etime = MAX_ETIME;
	for (i=0; i<CE_MAX; i++) {
	    if (pending[i].name[0] && pending[i].etime < next_etime)
		next_etime = pending[i].etime;
	}
	if (debug(DEBUG_QUEUE)) {
	    printf ("Next queued event time is %u\n", next_etime);
	}
    }

    err = (save_err) ? save_err : OK;
    return (err);
}

/************************************************************************/
/*  cs_errmsg - return comserv error message string.			*/
/************************************************************************/
char *cs_errmsg (short int err)
{
    static char *msg;
    switch (err) {
      case CSCR_ENQUEUE :
	msg = "No Empty Service Queues";
	break ;
      case CSCR_TIMEOUT :
	msg = "Command not processed by server";
	break ;
      case CSCR_INIT :
	msg = "Server in initialization";
	break ;
      case CSCR_REFUSE :
	msg = "Could not attach to server";
	break ;
      case CSCR_NODATA :
	msg = "The requested data is not available";
	break ;
      case CSCR_BUSY :
	msg = "Command buffer in use";
	break ;
      case CSCR_INVALID :
	msg = "Command is not known by server or is invalid for this DA";
	break ;
      case CSCR_DIED :
	msg = "Server is dead";
	break ;
      case CSCR_CHANGE :
	msg = "Server has restarted since last service request";
	break ;
      case CSCR_PRIVATE :
	msg = "Could not create my shared memory segment";
	break ;
      case CSCR_SIZE :
	msg = "Command buffer is too small";
	break ;
      case CSCR_PRIVILEGE :
	msg = "That command is privileged";
	break ;
      case CSCS_IDLE :
	msg = "Command buffer is Idle";
	break ;
      case CSCS_INPROGRESS :
	msg = "Waiting for response from DA";
	break ;
      case CSCS_FINISHED :
	msg = "Command response available";
	break ;
      case CSCS_REJECTED :
	msg = "Command rejected by DA";
	break ;
      case CSCS_ABORTED :
	msg = "File transfer aborted";
	break ;
      case CSCS_NOTFOUND :
	msg = "File not found";
	break ;
      case CSCS_TOOBIG :
	msg = "File is larger than 65K bytes";
	break ;
      case CSCS_CANT :
	msg = "Cannot create file on DA";
	break ;
      default:
	msg = "Unknown error";
	break;
    }
    return (msg);
}

/* 
   If a command is in progress, wait up to MAXWAIT seconds for it to finish, else
   return actual status.
*/
short int wait_finished (comstat_rec *pcomm) 
{
#ifdef _OSK
    double finish ;
      
    finish = dtime () + (double)WAIT_SECONDS ;
    do {
	if (pcomm->completion_status == CSCS_FINISHED) 
	    return CSCS_FINISHED ;
	else if (pcomm->completion_status == CSCS_INPROGRESS) 
	    tsleep (0x80000100) ;
	else 
	    break ;
    } while (dtime() < finish) ;
#else
    short ct ;

    for (ct = 0 ; ct < WAIT_SECONDS ; ct++)
    if (pcomm->completion_status == CSCS_FINISHED) 
	return CSCS_FINISHED ;
    else if (pcomm->completion_status == CSCS_INPROGRESS) 
	sleep (1) ;
    else 
	break ;
#endif
    return pcomm->completion_status ;
}
