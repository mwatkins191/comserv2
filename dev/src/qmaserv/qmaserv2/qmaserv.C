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
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

// our includes
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
  if(argc < 2) {
    Usage();
    return(0);
  }

  char station_code[MAX_CHARS_IN_STATION_CODE+1];
  time_t nextStatusUpdate;

  if( (strcmp(argv[1],"-v") == 0) || (strcmp(argv[1],"-V") == 0) ) {
    showVersion();
    exit(12);
  } else {
    strlcpy(station_code,argv[1],MAX_CHARS_IN_STATION_CODE+1);
  }
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
    if(g_libInterface) {
      delete g_libInterface;
      g_libInterface = NULL;
    }
    g_libInterface = new Lib330Interface(station_code, g_cvo);
    g_libInterface->startRegistration();
    if(!g_libInterface->waitForState(LIBSTATE_RUN, 120)) {
      // we'll go from RUNWAIT to RUN automatically.  If we don't get there, we'll reset and try again.
      g_reset = 1;
    } 
    while(!g_reset) {
      scan_comserv_clients();
      if(time(NULL) >= nextStatusUpdate) {
	g_libInterface->displayStatusUpdate();
	nextStatusUpdate = time(NULL) + g_cvo.getStatusInterval();
      }
    }
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
    std::cout << "    qmaserv [stationName]" << std::endl;
}

//
// Initialize the signal handlers and the variables
//
void initializeSignalHandlers() {

  //
  // Client will send a SIGALRM signal when it puts it's segment ID into the
  // service queue. Make sure we don't die on it, just exit sleep.
  //

  signal (SIGALRM, SIG_IGN) ;
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
  int stat = comserv_scan();
  if(stat != 0) {
     g_log << "+++ Found signal from client: " << stat << std::endl;
	
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
