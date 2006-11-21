/*
 * File     :
 *  qmautils.C
 *
 * Purpose  :
 *  Simple utility routines for use in qmaserv.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  26 July 2002
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
#ifndef _WIN32
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <sys/types.h>

#include "qmautils.h"
#include "QmaTypes.h"
#include "QmaDiag.h"
#include "Verbose.h"
#include "global.h"
#include "QMA_Port.h"
#include "Cleanup.h"
#include "QMA_Version.h"

#include "Logger.h"

#ifdef Q3302EW
extern "C" {
# include <local_earthworm_complex_funcs.h>
# include <earthworm.h>
};
#endif

void Usage()
{
  g_log << "Usage   : qmaserv station_abbr" << std::endl;
  g_log << "Example : qmaserv LGB" << std::endl;
  return;
}

//
// Routine to initialize the data and cmd ports
//

bool openLocalIPPorts()
{  

  if(g_stateMachine.getState() != OpeningLocalPorts)
  {
    CleanQMA(12);
  }

  bool initialized = false;
  bool result = false;
  if(g_verbosity.show(D_MINOR,D_OPEN_LOCAL_PORT))
  {
    g_log << "--- QMA IPPort : " << 
      (qma_uint32) g_cvo.getQMAIPPort() << std::endl;
    g_log << "--- Q330 Data Port : " <<
      (qma_uint32) g_cvo.getQ330DataPortAddress() << std::endl;
    struct sockaddr_in temp;
    temp.sin_family = AF_INET;
    temp.sin_addr.s_addr = g_cvo.getQ330UdpAddr();
    memset(&(temp.sin_zero),'\0',8); 
    g_log << "--- UdpAddr : " <<
	    inet_ntoa(temp.sin_addr) << std::endl;
    g_log << "--- Serial Number : " << std::hex <<  
	g_cvo.getQ330SerialNumber() << std::endl;
  }

  int retryLimit = 100; // Just a large number of retries before exiting
  while(!initialized)
  {
      --retryLimit;
       bool result = g_cmdPort.initialize(g_cvo.getQMAIPPort(),
		                        g_cvo.getQ330DataPortAddress(),
                                        g_cvo.getQ330UdpAddr());
       if(result)
       {
         initialized = true;
         if(g_verbosity.show(D_MAJOR,D_OPEN_LOCAL_PORT))
         {
           g_log << "+++ DP CmdPort Initialized: " << 
		g_cvo.getQMAIPPort() << std::endl;
         }
       }
       else if(retryLimit < 1)
       {
          g_log << "xxx Exiting after 100 retries initializing cmdPort" << 
		std::endl;
          initialized = false;
       }
       else
       {
         g_log << "--- Retrying cmdPort Initializaion" << std::endl;
#if defined(_WIN32) && defined(Q3302EW)
	 sleep_ew(10 * 1000);
#else
         sleep(10); // sleep in seconds
#endif
       }
    } // end while

    initialized = false;
    result = false;
    retryLimit = 100;
    while(!initialized)
    {
      --retryLimit;
      result = g_dataPort.initialize((g_cvo.getQMAIPPort()+1),
                                    (g_cvo.getQ330DataPortAddress()+1),
                                     g_cvo.getQ330UdpAddr(),
				     g_cvo.getStatusInterval(),
                                     g_cvo.getDataRateInterval());
      if(result)
      {
	    initialized = true;

        if(g_verbosity.show(D_MAJOR,D_OPEN_LOCAL_PORT))
        {
            g_log << "+++ DP DataPort Initialized: " << 
		(g_cvo.getQMAIPPort() + 1) << std::endl;
        }
      }
      else if (retryLimit < 1)
      {
            g_log << "xxx Exiting after 100 retries initializing dataPort" 
		<< std::endl;
	return false;
      }
      else
      {
#if defined(Q3302EW) && defined(_WIN32)
	sleep_ew(10 * 1000);
#else
	sleep(10);
#endif
	g_log << "xxx Retrying dataPort Initializaion" << std::endl;
      }
  }
  g_stateMachine.setState(RequestingServerChallenge);
  return true;
}

//
// Validation current consist of only two checks.
// (a) Version. A new version will generate a warning only.
// (b) Station Code of more than 4 chars. Currently, on this error,
//     Mountainair exits.
void validateTokens()
{
  char staCode[STATION_CODE_LEN +1];
  memset(staCode,0,STATION_CODE_LEN +1);
  memcpy((char*)&staCode[0],
         g_tvo.getNetStationVO().getStationCode(),
         strlen(g_tvo.getNetStationVO().getStationCode()));
  memset((char*)&staCode[STATION_CODE_LEN],0,1);
  int i;
  //
  // This test assumes that a station code is left justified and
  // blank padded. If the 5 char is not a blank, then the station
  // code is 5 chars in length, which mountainair does not support.
  //
  int res = strncmp((char*)&staCode[4]," ",1);
  if(res != 0)
  {
    g_log << "xxx Mountainair supports Station codes with a maximum of" 
	<< std::endl;
    g_log << "xxx 4 characters. Please reconfigure the Q330 port with" 
        << std::endl;
    g_log << "xxx a shorter Station Code." << std::endl;
    g_log << "xxx Station Code received was: " << 
	g_tvo.getNetStationVO().getStationCode() << std::endl;
    g_log << "xxx Mountainair Exiting" << std::endl;
    CleanQMA(12);
  }

  //
  // There is a Token Version which is not useful
  // More importantly there is a system version. This version
  // Info is in the fixed flags, not the tokens. None the less.
  // check the System Version here before starting acquisition.
  //
  if(g_Q330Version.getVersionRelease() > QMA_VERSION_RELEASE)
  {
    g_log << "+++ Important. This Q330 is using a new System Version." 
     << std::endl;
    g_log << "+++ Mountainair may not be tested with this System Version."
     << std::endl;
  }
          g_log << std::endl;
          g_log << "+++ Summary of Token Information : " << std::endl;
	  g_log << "+++ Q330 System Version : " 
                << std::dec << g_Q330Version.getVersionRelease() 
		<< "." << std::dec << g_Q330Version.getVersionFeature() 
                   << std::endl;
          NetStationVO nsvo = g_tvo.getNetStationVO(); 
          g_log << "+++ Token Version : " << g_tvo.getVersion() 
                << std::endl;
          g_log << "+++ Network : " << nsvo.getNetworkCode() << std::endl;
          g_log << "+++ Station : " << nsvo.getStationCode() << std::endl;
          g_log << "+++ DPNetserver : " << g_tvo.getDPNetServerPortNumber() 
		<< std::endl;
          g_log << "+++ DPWebserver : " << g_tvo.getDPWebServerPortNumber()
		<< std::endl;

          //
          // ClockProc info
          //
          qma_uint16 temp;
	  ClockProcVO cpvo = g_tvo.getClockProcVO();

	  temp = cpvo.getTimeZoneOffset(); 
	  g_log << "+++ TimeZone offset in Minutes : " <<
		temp << std::endl;

	  temp = cpvo.getLossInMinutes();
	  g_log << "+++ Loss in Minutes : " <<
		temp << std::endl;

	  temp = cpvo.getPLLLockQuality();
	  g_log << "+++ PLL Lock Quality : " << std::hex <<
		temp << std::endl;

          temp = cpvo.getPLLTrackingQuality();
	  g_log << "+++ PLL Track Quality : " << std::hex <<
		temp << std::endl;

	  temp = cpvo.getPLLHoldQuality();
	  g_log << "+++ PLL Hold Quality : " << std::hex <<
		temp << std::endl;

          temp = cpvo.getPLLOffQuality();
	  g_log << "+++ PLL Off Quality : " << std::hex <<
		temp << std::endl;

	  temp = cpvo.getHighestHasBeenLocked();
	  g_log << "+++ Highest Has Been Locked Clock Quality : " <<
		temp << std::endl;

	  temp = cpvo.getLowestHasBeenLocked();
	  g_log << "+++ Lowest Has Been Locked Clock Quality : " <<
		temp << std::endl;

	  temp = cpvo.getClockQualityFilter();
	  g_log << "+++ Clock Quality Filter : " <<
		temp << std::endl;

	  //
          // Now Timing log info
          //
	  LogTimingVO ltvo = g_tvo.getLogTimingVO();
	  g_log << "+++ MessageLogLocation : " << 
		ltvo.getMessageLogLocation() << std::endl;
	  g_log << "+++ MessageLogSEEDName : " << 
		ltvo.getMessageLogName() << std::endl;
	  g_log << "+++ TimingLogLocation : " << 
		ltvo.getTimingLogLocation() << std::endl;
	  g_log << "+++ TimingLogSEEDName : " << 
		ltvo.getTimingLogName() << std::endl;

	  //
          //
          //
	  ConfigInfoVO civo = g_tvo.getConfigInfoVO();
	  g_log << "+++ ConfigStreamLocation : " <<
	    civo.getStreamSEEDLocation() << std::endl;
	  g_log << "+++ ConfigStreamName : " <<
	    civo.getStreamSEEDName() << std::endl;
	  g_log << "+++ ConfigStreamRequestOption : " <<
	    civo.getConfigOption() << std::endl;
	  if(civo.getConfigOption() == Periodic)
	    {
	      g_log << "+++ ConfigRequestInterval : " <<
		civo.getConfigInterval() << std::endl;
	    }
         
           g_log << "+++ DataServerPort : " <<
		g_tvo.getDataServerPort() << std::endl;

 	   g_log << "+++ Number of 24 bit digitizer LCQ's" 
		<< " for this Data port : " <<
		std::dec << g_number_of_diglcqs << std::endl;
           for(i=0;i<g_number_of_diglcqs;i++)
           {
             LCQVO temp = g_digLCQ_list[i].getLCQVO();

             g_log << "+++ LCQ Network Code : " << temp.getNetworkCode() 
	        <<	std::endl;
             g_log << "+++ LCQ Station Code : " << temp.getStationCode()
		<<	std::endl;
             g_log << "+++ LCQ Location Code : " << temp.getLocationCode() 
	        <<	std::endl;
             g_log << "+++ LCQ SEED Name : " << temp.getSEEDName()
		<<	std::endl;

             g_log << "+++ LCQ Ref Number : " << 
		(qma_uint16) temp.getLCQReferenceNumber() << std::endl;

             g_log << "+++ LCQ Channel Number : " <<
		(qma_uint16) temp.getChannel() << std::endl;

             g_log << "+++ LCQ Channel Byte   : " <<
		(qma_uint16) temp.getChannelByte() << std::endl;

             g_log << "+++ LCQ Frequency (Hertz) : " <<
		 temp.getFrequencyHertz() << std::endl;

             g_log << "+++ LCQ FreqBit Number : " <<
		(qma_uint16) temp.getFrequencyBit() << std::endl;

             g_log << "+++ LCQ Filter Delay : " <<
		(qma_int32) temp.getFilterDelay() << std::endl;

             g_log << "+++ LCQ Source Parameter : " << 
		(qma_uint16) temp.getSourceParameter() << std::endl;

             g_log << "+++ LCQ Source : " << (qma_uint16) temp.getSource()
                << std::endl;

             g_log << "+++ LCQ Parameter : " << 
		(qma_uint16) temp.getParameterNumber() << std::endl;

             g_log << "+++ LCQ Option Bits : " << 
			temp.getOptionBitsFlag() << std::endl;
             g_log << "+++ LCQ Samples per Blockette : " <<
                        temp.getSamplesPerBlockette() << std::endl;
             g_log << "+++ LCQ Rate : " << temp.getRate() <<
			std::endl << std::endl;
	   }


 	   g_log << "+++ Number of main and analog LCQ's" 
		<< " for this Data port : " <<
		std::dec << g_number_of_mainlcqs << std::endl;

           for(i=0;i<g_number_of_mainlcqs;i++)
           {

             LCQVO temp = g_mainLCQ_list[i].getLCQVO();

             g_log << "+++ LCQ Network Code : " << temp.getNetworkCode() 
	        <<	std::endl;
             g_log << "+++ LCQ Station Code : " << temp.getStationCode()
		<<	std::endl;
             g_log << "+++ LCQ Location Code : " << temp.getLocationCode() 
	        <<	std::endl;
             g_log << "+++ LCQ SEED Name : " << temp.getSEEDName()
		<<	std::endl;

             g_log << "+++ LCQ Ref Number : " << 
		(qma_uint16) temp.getLCQReferenceNumber() << std::endl;

             g_log << "+++ LCQ Channel Number : " <<
		(qma_uint16) temp.getChannel() << std::endl;

             g_log << "+++ LCQ Channel Byte   : " <<
		(qma_uint16) temp.getChannelByte() << std::endl;

             g_log << "+++ LCQ Frequency (Hertz) : " <<
		 temp.getFrequencyHertz() << std::endl;

             g_log << "+++ LCQ FreqBit Number : " <<
		(qma_uint16) temp.getFrequencyBit() << std::endl;

             g_log << "+++ LCQ Filter Delay : " <<
		(qma_int32) temp.getFilterDelay() << std::endl;

             g_log << "+++ LCQ Source Parameter : " << 
		(qma_uint16) temp.getSourceParameter() << std::endl;

             g_log << "+++ LCQ Source : " << (qma_uint16) temp.getSource()
                << std::endl;

             g_log << "+++ LCQ Parameter : " << 
		(qma_uint16) temp.getParameterNumber() << std::endl;

             g_log << "+++ LCQ Option Bits : " << 
			temp.getOptionBitsFlag() << std::endl;

             g_log << "+++ LCQ Samples per Blockette : " <<
                        temp.getSamplesPerBlockette() << std::endl;

             g_log << "+++ LCQ Rate : " << temp.getRate() <<
			std::endl << std::endl;
	   }
}

void showVersion()
{
 g_log << "+++ QMASERV version : " <<
        QMA_VERSION_RELEASE << "." <<
        QMA_VERSION_FEATURE << "." <<
        QMA_VERSION_FIX << std::endl;
}
