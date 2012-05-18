/*
 * Program: Mountainair
 *
 * File:
 *  qmaserv
 *
 * Purpose:
 *  This is the top level program that runs the Mountainair program.
 *  Invoke this and it receives data from a Q330.
 *
 * Author:
 *   Phil Maechling, Hal Schechner
 *
 * Created:
 *   2 February 2002
 *
 * Modifications:
 *   13 Nov 2006 - HJS - Ripped out all QMA related code
 *   24 Aug 2007 - DSN - Change from SIG_IGN to signal handler for SIGALRM.
 *    6 Feb 2012 - DSN - Limit max number of open files (RLIMIT_NOFILE) 
 *                       to be no more than compile-time limie FD_SETSIZE.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it with the sole restriction that:
 * You must cause any work that you distribute or publish, that in
 * whole or in part contains or is derived from the Program or any
 * part thereof, to be licensed as a whole at no charge to all third parties.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *
 */
#include <iostream>
#include <cstdlib>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>

// our includes
#define	DEFINE_EXTERNAL
#include "global.h"
#include "Logger.h"
#include "Verbose.h"
#include "ConfigVO.h"
#include "qmaserv.h"
#include "ReadConfig.h"
#include "lib330Interface.h"

// comserv includes
extern "C" {

#include "qmacfg.h"
#include "cserv.h"
#include "clink.h"
#include "srvc.h"
void cs_sig_alrm (int signo);
}

#ifdef linux
#include "portingtools.h"
#endif

//
// These are the instantiations of the variable declared in the global.h
//
Verbose          g_verbosity;
ConfigVO         g_cvo;
bool             g_reset;
bool             g_done;
Verbose*         g_verbList; // Not needed. Just for testing


Logger		 g_log;
Lib330Interface  *g_libInterface = NULL;

//
//*****************************************************************************
// This is the main routine which receives data from the q330.
//*****************************************************************************
//


int main(int argc, char *argv[]) {
  g_log.logToStdout(true);
  g_log.logToFile(false);

  showVersion();
#ifdef	ENDIAN_LITTLE
  g_log << "Compiled with ENDIAN_LITTLE" << std::endl;
#endif

  if(argc < 2) {
    Usage();
    return(0);
  }

  char station_code[MAX_CHARS_IN_STATION_CODE+1];
  time_t nextStatusUpdate;

  if( (strcmp(argv[1],"-v") == 0) || (strcmp(argv[1],"-V") == 0) ) {
//    showVersion();
    exit(12);
  } else {
    strlcpy(station_code,argv[1],MAX_CHARS_IN_STATION_CODE+1);
  }

  // Limit the max number of open file descriptors to the compliation value 
  // FD_SETSIZE, since this is used to create the fd_set options used by select().
  struct rlimit rlp;
  int status = getrlimit (RLIMIT_NOFILE, &rlp);
  if (status != 0) {
    g_log << "XXX Program unable to query max_open_file_limit RLIMIT_NOFILE" << std::endl;
    return (1);
  }
  if (rlp.rlim_cur > FD_SETSIZE) {
    rlp.rlim_cur = FD_SETSIZE;
    status = setrlimit (RLIMIT_NOFILE, &rlp);
    if (status != 0) {
	g_log << "XXX Program unable to set max_open_file_limit RLIMIT_NOFILE" << std::endl;
	return (1);
    }
  }
  g_log << "XXX Program max_open_file_limit = " << rlp.rlim_cur << std::endl;

  initializeSignalHandlers();
  g_done  = false;
  g_reset = false;
  g_verbosity.setVerbosity(0);
  g_verbosity.setDiagnostic(0);
  initialize(station_code);
  readOrRereadConfigFile(station_code);
  nextStatusUpdate = time(NULL) + g_cvo.getStatusInterval();
  while(!g_done) {
    g_reset = 0;

    // Make sure we have a lib330 interface, create one if not
    if(!g_libInterface) {
      g_libInterface = new Lib330Interface(station_code, g_cvo);
    }

    // We may have gotten here if a client blocked, if so, wait until that
    // condition clears
    if(comlink_dataQueueBlocking()) {
      g_log << "XXX Client still blocking...  Sleeping for 5 seconds" << std::endl;
      struct timespec t;
      t.tv_sec = 0;
      t.tv_nsec = 250000000;
      for(int i=0; i <= 20; i++) {
	nanosleep(&t, NULL);
	scan_comserv_clients();
      }
      continue;
    }

    // Process the packet queue if there's anything in it
    if(!g_libInterface->processPacketQueue()) {
      // not only was there something in the queue, but we started blocking
      // again before we finished emptying it.  back to square 1, waiting for
      // the blocking to stop, then we'll try running the queue again
      continue;
    }

    // register and wait for data to flow
    g_libInterface->startRegistration();
    if(!g_libInterface->waitForState(LIBSTATE_RUN, 120, scan_comserv_clients)) {
      // we'll go from RUNWAIT to RUN automatically.  If we don't get there, we'll reset and try again.
      delete g_libInterface;
      g_libInterface = NULL;
      continue;
    } 

    // this is where we live for most of the life of the process.  Scan the clients
    // and do what we're told
    while(!g_reset) {
      int packetQueueEmptied;
      scan_comserv_clients();
      if(time(NULL) >= nextStatusUpdate) {
	g_libInterface->displayStatusUpdate();
	nextStatusUpdate = time(NULL) + g_cvo.getStatusInterval();
      }
      packetQueueEmptied = g_libInterface->processPacketQueue();
      if(!packetQueueEmptied) {
	g_log << "--- client blocking, halting dataflow." << std::endl;
	g_reset = 1;
      }
    }

    // we get here when g_reset is set to 1
    g_libInterface->startDeregistration();
    
  }
  cleanup(0);
  return 0;
}

void showVersion() {
  std::cout << APP_VERSION_STRING << std::endl;
}

void Usage() {
    showVersion();
    std::cout << " Usage: " << std::endl; 
    std::cout << "    qmaserv -v | -V | stationName" << std::endl;
}

//
// Initialize the signal handlers and the variables
//
void initializeSignalHandlers() {

  //
  // Client will send a SIGALRM signal when it puts it's segment ID into the
  // service queue. Make sure we don't die on it, just exit sleep.
  //

      struct sigaction action;
      /* Set up a permanently installed signal handler for SIG_ALRM. */
      action.sa_handler = cs_sig_alrm;
      action.sa_flags = 0;
      sigemptyset (&(action.sa_mask));
      sigaction (SIGALRM, &action, NULL);

  signal (SIGPIPE, SIG_IGN) ;

  //
  // Intercept various abort type signals so they send a disconnect message
  // to the Q330 if we control C out of this program. Netmon may signal this
  // program to cause it to exit also.
  //

  signal (SIGHUP ,cleanupAndExit) ;
  signal (SIGINT ,cleanupAndExit) ;
  signal (SIGQUIT,cleanupAndExit) ;
  signal (SIGTERM,cleanupAndExit) ;

}


void readOrRereadConfigFile(char* stationcode) {
  bool res = readConfigFile(stationcode);

  if(res) {
    g_verbosity.setVerbosity(g_cvo.getVerbosity());
    g_verbosity.setDiagnostic(g_cvo.getDiagnostic());
  } else {
     g_log << 
	"xxx Error reading Mountainair configuration values for station: " 
	<< stationcode << std::endl;
     cleanup(0);
  }
}

void scan_comserv_clients() {
  int clientCmd = comserv_scan();
  static int areSuspended = 0;

  if(clientCmd != 0) {
    //g_log << "+++ Found signal from client: " << clientCmd << std::endl;
     switch(clientCmd) {
     case CSCM_SUSPEND:
       //cleanup(1);
       g_log << "--- Suspending link (Requested)" << std::endl;
       g_libInterface->startDeregistration();
       areSuspended = 1;
       break;
     case CSCM_TERMINATE:
       g_log << "--- Terminating server (Requested)" << std::endl;
       cleanupAndExit(1);
       break;
     case CSCM_RESUME:
       if(areSuspended) {
	 g_log << "--- Resuming link (Requested)" << std::endl;
	 g_reset = 1;
	 areSuspended = 0;
       } else {
	 g_log << "--- Asked to resume unsuspended link.  Ignoring." << std::endl;
       }
     }
	
  }
}

void initialize(char *stationCode) {
    g_log << "+++ Initializing comserv subsystem (Station: " << stationCode << ")." << std::endl;
    comserv_init(stationCode);
    g_log << "+++ Comserv subsystem initialized." << std::endl;
}

void cleanup(int arg) {
    // perform any cleanup that's required
    if(g_libInterface) {
      delete g_libInterface;
    }
}

void cleanupAndExit(int arg) {
  g_log << "+++ Shutdown started" << std::endl;
  cleanup(arg);
  exit(1);
  g_log << "+++ Application ended" << std::endl;
}
