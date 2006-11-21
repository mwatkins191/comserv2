/*
 * Program: 
 *   ReadConfig.C
 *
 * Purpose:
 *   These is processing routine is used by the qmaserv main routine
 *   to read in configuration information. It uses the comserv configuration
 *   routines to read the config file 
 *
 * Author:
 *   Phil Maechling
 *
 * Created:
 *   4 April 2002
 *
 * Modifications:
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
#include <string.h>
#include "ReadConfig.h"
#include "Verbose.h"
#include "QmaTypes.h"
#include "QmaDiag.h"
#include "ConfigVO.h"
#include "qmacfg.h"
#include "clink.h"
#include "cserv.h"
#include "global.h"

extern Verbose      g_verbosity;
extern ConfigVO     g_cvo;


bool readConfigFile(char* stationcode)
{

  if(g_verbosity.show(D_MAJOR,D_SHOW_STATE))
  {
	  g_log << "+++ Configuring using Station Code : " << stationcode
		<< std::endl;
  }
  struct qma_cfg qmacfg;
  strcpy(qmacfg.station_code,stationcode);

  int res = getQmacfg(&qmacfg);    // This makes a call to comserv routines
                                   // to retrieve [mountainair] data in the
                                   // station.ini file

  if(res != QMA_SUCCESS)
  {
    g_log << "xxx Error initializing from config file for station : " << 
	stationcode << std::endl;
        return false;
  }


  //
  // Call to check that all required config values are set
  //
  res = validateQmaConfig(qmacfg);

  if(res == false)
  {
    return false;
  } 

  //
  // This will convert string values to int, IP, and other usable
  // values, and put them into a ConfigVO.
  //
  ConfigVO tcvo(qmacfg.udpaddr,
		qmacfg.baseport,
		qmacfg.dataport,
		qmacfg.serialnumber,
		qmacfg.authcode,
		qmacfg.ipport,
                qmacfg.verbosity,
	        qmacfg.diagnostic,
                qmacfg.startmsg,
		qmacfg.statusinterval);

  if(g_verbosity.show(D_MAJOR,D_QMA_CONFIG))
  {
    g_log << "--- Initialized configuration from file : " << stationcode
		<< std::endl;
  }

  g_cvo = tcvo; // Move local ConfigVO to global position

  //
  // When this is run, verbosity object not yet set. So specify
  // here the qma target
  //
  if((g_cvo.getVerbosity() >= D_MAJOR ) || 
      (g_cvo.getDiagnostic() == D_QMA_CONFIG))
  { 
            g_log << "--- Read config for " <<
	        qmacfg.station_code << std::endl;
            g_log << "--- Q330 UDPAddr : " <<
	        qmacfg.udpaddr << std::endl;
	    g_log << "--- Q330 control Port : " <<
		qmacfg.baseport << std::endl;
	    g_log << "--- Q330 Data Port : " <<
		qmacfg.dataport << std::endl;
	    g_log << "--- Q330 Serial number : " <<
	        qmacfg.serialnumber << std::endl;
	    g_log << "--- Authcode : " << 
	        qmacfg.authcode << std::endl;
	    g_log << "--- DP IPPort : " <<
                qmacfg.ipport << std::endl;
	    g_log << "--- Verbosity : " <<
                qmacfg.verbosity << std::endl;
	    g_log << "--- Diagnostic : " <<
                qmacfg.diagnostic << std::endl;
	    g_log << "--- Start Message : " <<
                qmacfg.startmsg << std::endl;
	    g_log << "--- Status Interval : " <<
                qmacfg.statusinterval << std::endl;
	    g_log << "--- Data Rate Interval : " <<
                qmacfg.datarateinterval << std::endl;
  }
  return true;
}

bool validateQmaConfig(const struct qma_cfg& aCfg)
{

 //
 // Required fields are 
 // udpaddr
 // ipport
 // baseport
 // dataport
 // serial number
 // authcode
 
 // Optional fields are
 // verbosity
 // diagnostic
 // startmsg
 // status interval
 // min clock quality

 int len = strlen(aCfg.udpaddr);
 if(len < 1)
 {
   g_log << 
     "xxx Configuration file is missing value for udpaddr:" << std::endl;
  return false;
 }

 len = strlen (aCfg.baseport);
 if(len < 1)
 {
   g_log << 
     "xxx - Configuration file is missing value for baseport:" << std::endl;
   return false;
 }

 len = strlen (aCfg.dataport);
 if(len < 1)
 {
   g_log << 
     "xxx - Configuration file is missing value for dataport:" << std::endl;
   return false;
 }

 len = strlen (aCfg.serialnumber);
 if(len < 1)
 {
   g_log << 
     "xxx - Configuration file is missing value for serialnumber:" 
	     << std::endl;
   return false;
 }

 len = strlen (aCfg.authcode);
 if(len < 1)
 {
   g_log << 
     "xxx - Configuration file is missing value for authcode:" << std::endl;
   return false;
 }

 return true;
}
