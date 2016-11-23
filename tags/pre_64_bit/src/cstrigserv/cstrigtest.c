#ifndef lint
static char sccsid[] = "@(#) $Id: cstrigtest.c,v 1.2 2003/02/01 19:04:06 lombard Exp $";
#endif

/************************************************************************/
/*  cstrigger - send a trigger message to the comserv agent cstrigserv.
/************************************************************************/
/*
    Author:
	Doug Neuhauser
	UC Berkeley Seismological Laboratory
	doug@seismo.berkeley.edu

    Purpose:
	Send a commdet trigger message to the comserv agent cstrigserv
	to remotely trigger event channels on a Quanterra datalogger.

    Modification History:
    Date	Ver	Who	What
    ---------------------------------------------------------------------
    2000/03/15	1.0	DSN	Initial coding.
/************************************************************************/

#include    <stdio.h>
#define	    VERSION	"1.0beta (2000.075"

#ifndef DEFAULT_CONFIG_FILE
#define DEFAULT_CONFIG_FILE	"/home/redi/run/params/cstrigger.config"
#endif
#ifndef	MAXWAIT
#define	MAXWAIT	45
#endif
   
char *syntax[] = {
"%s version " VERSION " -- send trigger msg to the comserv agent cstrigserv.",
"%s  [-h] [-d n] datalogger msg",
"    where:",
"	-h	    Help - prints syntax message.",
"	-d n	    Debug flag",
"		    1 = debug msg",
"		    2 = debug ip",
"		    4 = no execute",
"	-C config   Name of config file.  Default config file is",
"		    " DEFAULT_CONFIG_FILE ,
"	datalogger  Name of comserv datalogger.",
"	msg	    cstrigserv message.",
NULL };

/************************************************************************/

#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <fcntl.h>
#include <sys/file.h>

#define	info		stdout
#define	MAXMSGLEN	512
#define	MAXLINELEN	80

/************************************************************************/
/*  External variables and symbols.					*/
/************************************************************************/
char *cmdname;			/* program name from command line.	*/
char *cfgfile = DEFAULT_CONFIG_FILE;
struct sockaddr_in serv_addr, rem_addr;
typedef struct sockaddr *psockaddr;
typedef char *pchar;
char myaddrstr[64];
int debug_flag;

#define debug(opt) (debug_flag & opt)
#define	DEBUG_COMM	1
#define	DEBUG_IP	2
#define	DEBUG_NOEXEC	4
#define	DEBUG_ANY (DEBUG_COMM | DEBUG_IP | DEBUG_NOEXEC)

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
/*  uppercase:								*/
/*	Convert a string to upper case in place.			*/
/*  return:								*/
/*	pointer to string.						*/
/************************************************************************/
char *uppercase
   (char	*string)	/* string to convert to upper case.	*/
{
    char *p = string;
    unsigned char c;
    while (c = *p) *(p++) = islower(c) ? toupper(c) : c;
    return (string);
}

/************************************************************************/
/*  main:   main program.
/************************************************************************/
main (argc, argv)
    int		argc;
    char	**argv;
{
    char site[6];
    char msg[MAXMSGLEN];
    char ack[MAXMSGLEN];
    char line[MAXLINELEN];
    char sitename[8], csname[8], remhost[64], remhostaddr[64];
    struct hostent *hostent;
    int remport, myport;
    int msglen;
    unsigned int seq;
    int i, l, n, found;
    FILE *fp;
    int sockfd, path;
    int ruflag, flags;
    int maxwait;
    int status;

    /* Variables needed for getopt. */
    extern char	*optarg;
    extern int	optind, opterr;
    int		c;
    char	*p;

    cmdname = ((p = strrchr(*argv,'/')) != NULL) ? ++p : *argv;
    /*	Parse command line options.					*/
    while ( (c = getopt(argc,argv,"hC:d:")) != -1) switch (c) {
	case '?':
	case 'h':   print_syntax(cmdname,syntax,info); exit(0); break;
	case 'C':   cfgfile = optarg; break;
	case 'd':   debug_flag = atoi(optarg); break;
	default:    fprintf (info, "Unknown option: -%c\n", c); exit(1);
    }
    /*	Skip over all options and their arguments.			*/
    argv = &(argv[optind]);
    argc -= optind;

    if (argc-- < 1) {
	fprintf (info, "Error: missing datalogger name\n");
	exit(1);
    }
    strncpy (site, *argv++, 5);
    site[5] = '\0';
    uppercase (site);

    seq = time(NULL);

    msg[0] = '\0';
    sprintf (msg, "%ld", seq);
    l = strlen(msg);

    while (argc-- > 0) {
	i = strlen(*argv);
	if (l > 0 && l < MAXMSGLEN) {
	    strcat (&msg[l], " ");
	    l++;
	}
	if (i+l < MAXMSGLEN) {
	    strcat (&msg[l], *argv++);
	    l += i;
	}
    }
    uppercase(msg);

    found = 0;
    /* Find host, address, and port for datalogger's comserv in config file.	*/
    if ((fp=fopen(cfgfile,"r")) == NULL) {
	fprintf (info, "Error: opening config file %s\n", cfgfile);
	exit(1);
    }
    while (fgets(line,MAXLINELEN,fp)) {
	if (line[0] == '#') continue;
	n = sscanf (line, "%s %s %s %d", sitename, csname, remhost, &remport, &myport);
	if (n != 4) {
	    fprintf (info, "Error in config file line: %s", line);
	    exit(1);
	}
	if (strcasecmp (site, sitename) == 0) {
	    found = 1;
	    break;
	}
    }
    fclose(fp);
    if (! found) {
	fprintf (info, "Error: datalogger %s not found in config file %s\n", site, cfgfile);
	exit(1);
    }

    /* Connect to comserv host for the datalogger and send message. */
    
    if (strspn(remhost,"0123457890.") == strlen(remhost)) {
	strcpy (remhostaddr, remhost);
    }
    else {
	if ((hostent=gethostbyname(remhost))) {
	    strcpy (remhostaddr, inet_ntoa(*(struct in_addr *)hostent->h_addr));
	}
	else {
	    fprintf (info, "Error: Unable to get ipaddr for %s\n", remhost);
	    exit(1);
	}
    }
    if (debug(DEBUG_IP)) {
	fprintf (info, "Opening connection to: %s:%d\n", remhostaddr, remport);
    }

    myaddrstr[0] = '\0';
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    memset ((pchar) &serv_addr, sizeof(serv_addr), 0) ;
    serv_addr.sin_family = AF_INET ;
    if (myaddrstr[0]) {
	serv_addr.sin_addr.s_addr = inet_addr(myaddrstr) ;
    }
    else {
	serv_addr.sin_addr.s_addr = INADDR_ANY ;
    }
    serv_addr.sin_port = htons((unsigned short)atoi(myport)) ;
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

    flags = fcntl(sockfd, F_GETFL, 0) ;
    flags = flags | FNDELAY ;
    fcntl(sockfd, F_SETFL, flags) ;

    path = sockfd ; /* fake it being opened */

    rem_addr.sin_family = AF_INET ;
    rem_addr.sin_addr.s_addr = inet_addr(remhostaddr) ;
    rem_addr.sin_port = htons((unsigned short)remport) ;
    if (debug(DEBUG_COMM)) {
	fprintf (info, "Sending msg: %s\n", msg);
    }
    status = 0;
    msglen = strlen(msg);
    if (! debug(DEBUG_NOEXEC)) {
	status = sendto (path, msg, msglen, 0, (psockaddr)&rem_addr, sizeof(struct sockaddr_in));
    }
    if (status != msglen) {
	fprintf (info, "Error: send status = %d, errno = %d\n", status, errno);
	exit(1);
    }

    n = 0;
    ack[0] = '\0';
    maxwait = 0;
    if (! debug(DEBUG_NOEXEC)) {
	while ((n = recvfrom (path, ack, MAXMSGLEN, 0, (psockaddr)&rem_addr, &l)) < 0 && errno == EAGAIN && maxwait++ < MAXWAIT) {
	    sleep(1);
	}
    }
    if (n == MAXMSGLEN) n--;
    if (n >= 0) ack[n] = '\0';
    if (debug(DEBUG_COMM)) {
	printf ("recv %d chars, ack = %s\n", n, ack);
    }
    if (n <= 0 && maxwait >= MAXWAIT) {
	printf ("No response\n");
    }
    close (path);
    close (sockfd);
    return (0);
}
