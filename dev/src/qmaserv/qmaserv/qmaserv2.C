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
 *   Phil Maechling
 *
 * Created:
 *   2 February 2002
 *
 * Modifications:
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
#include "qmaserv.h"
#include "qmautils.h"
#include "Verbose.h"
#include "StateMachine.h"
#include "Cleanup.h"
#include "QMA_Port.h"
#include "ModuloCounter.h"
#include "QDPHeader.h"
#include "Packet.h"
#include "ConfigVO.h"
#include "ReadConfig.h"
#include "TxCmd.h"
#include "CountDownTimer.h"
#include "TimeServer.h"
#include "AckCounter.h"
#include "TokenVO.h"
#include "NetStationVO.h"
#include "LogTimingVO.h"
#include "ClockProcVO.h"
#include "LCQ.h"
#include "PCQ.h"
#include "Logger.h"
#include "ChanFreqMap.h"
#include "TypeChanFreqMap.h"
#include "QMA_Version.h"
#include "Q330Version.h"

#include "qmacfg.h"
#include "cserv.h"
#include "clink.h"
#include "ConfigInfoVO.h"
#include "LCQVO.h"
#include "CheckLCQS.h"
#include "CheckPCQS.h"
#include "msgs.h"
#include "InputPacket.h"

#ifdef LINUX
#include "portingtools.h"
#endif
//
// These are the instantiations of the variable declared in the global.h
//
Q330Version      g_Q330Version;
Verbose          g_verbosity;
StateMachine     g_stateMachine;
ConfigVO         g_cvo;
QMA_Port 	 g_cmdPort;
QMA_Port         g_dataPort;
ModuloCounter    g_cmdPacketSeq;
CountDownTimer   g_cmdTimer;
CountDownTimer   g_dataPortTimer;
CountDownTimer   g_statusTimer;
bool             g_reset;
bool             g_done;
Verbose*         g_verbList; // Not needed. Just for testing
Packet           g_packet_for_tx;
TimeServer 	 g_timeServer;
AckCounter       g_ackCounter;
TokenVO          g_tvo;
int              g_nextTokenAddress;
int              g_segmentsReceived;
int              g_bytesInBuffer;
char 		 g_tokenBuffer[C1_MAXCFG];
int              g_totalSegments;
qma_uint16       g_nextPacketSeqno;
int              g_totalFillPackets;
//
// These 4 are related to the number of digital LCQ's in the system.
// The LCQVO_List is used to moderate the startup grap for free memory.
//
int              g_number_of_diglcqs;
LCQ*             g_digLCQ_list;
PCQ* 		 g_digPCQ_list;
LCQVO*           g_digLCQVO_list;
bool             g_lcq_freed;
//
// These 4 are related to the number of main LCQ's in the system.
// The LCQVO_List is used to moderate the startup grap for free memory.
//
int              g_number_of_mainlcqs;
LCQ*             g_mainLCQ_list;
PCQ* 		 g_mainPCQ_list;
LCQVO*           g_mainLCQVO_list;
bool             g_main_lcq_freed;

ChanFreqMap 	 g_digMap_list;
TypeChanFreqMap  g_mainMap_list;

ModuloCounter    g_dataPacketSeq;
TimeOfDay        g_curTime;
bool             g_startingDRSNNeeded;
c1_srvch         g_c1; 
MainStates       g_curState;
bool             g_outputPacketsQueued;
BTI		 g_currentTimeInfo;
InputPacket      g_inputQueue[MAX_SLIDING_WINDOW_SIZE];
Logger		 g_log;
//
//*****************************************************************************
// This is the main routine which receives data from the q330.
//*****************************************************************************
//

int main(int argc, char *argv[])
{
  g_log.logToStdout(true);
  g_log.logToFile(false);
  if(argc < 2)
  {
    Usage();
    return(0);
  }

  char station_code[MAX_CHARS_IN_STATION_CODE+1];

  if( (strcmp(argv[1],"-v") == 0) || 
      (strcmp(argv[1],"-V") == 0)    )
  {
    showVersion();
    exit(12);
  }
  else
  {
    strlcpy(station_code,argv[1],MAX_CHARS_IN_STATION_CODE+1);
  }
  initializeSignalHandlers();
  g_done  = false;
  g_reset = false;
  g_lcq_freed = true;
  g_verbosity.setVerbosity(0);
  g_verbosity.setDiagnostic(0);

  while(!g_done)
  {
    readOrRereadConfigFile(station_code);
    ResetQMA();
    g_reset = false;
    openLocalIPPorts();
    while(!g_reset)
    {
      tx_cmd();
      rx_msg();
      assembleBlockettesIntoSecond();
      assembleSecondsIntoPacket();
      scan_comserv_clients();
      check_state();
    }
  }
  CleanQMA(12);
  return 0;
}
//
// Initialize the signal handlers and the variables
//

void initializeSignalHandlers()
{

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

  signal (SIGHUP ,CleanQMA) ;
  signal (SIGINT ,CleanQMA) ;
  signal (SIGQUIT,CleanQMA) ;

}


void readOrRereadConfigFile(char* stationcode)
{
  bool res = readConfigFile(stationcode);

  if(res)
  {
    g_verbosity.setVerbosity(g_cvo.getVerbosity());
    g_verbosity.setDiagnostic(g_cvo.getDiagnostic());
    g_stateMachine.setState(RequestingServerChallenge);
  }
  else
  {
     g_log << 
	"xxx Error reading Mountainair configuration values for station: " 
	<< stationcode << std::endl;
     CleanQMA(12);
  }
}

void assembleBlockettesIntoSecond()
{
  
  if(g_stateMachine.getState() == AcquiringData)
  {
    check_lcqs();
  }
  return;
}
    
void assembleSecondsIntoPacket()
{
  if(g_stateMachine.getState() == AcquiringData)
  {
    check_pcqs();
  }
  return;
}
    
void scan_comserv_clients()
{
  int stat = comserv_scan();
  if(stat != 0)
  {
     g_log << "+++ Found signal from client: " << stat << std::endl;
     ShutdownQMA();
  }
}

void check_state()
{

  MainStates mstate = g_stateMachine.getState();
  if(mstate == Resetting)
  {
    ResetQMA();
  }
  else if(mstate == Exitting)
  {
    CleanQMA(12);
  }
  return;
}
