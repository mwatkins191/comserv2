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
 *
 */
#include <iostream>
#include <signal.h>
#include <unistd.h>
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
#include "ChanFreqMap.h"

#include "cserv.h"
#include "clink.h"
#include "ConfigInfoVO.h"
#include "LCQVO.h"
#include "CheckLCQS.h"
#include "CheckPCQS.h"
#include "msgs.h"
#include "InputPacket.h"

//
// These are the instantiations of the variable declared in the global.h
//
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
int              g_totalFillPackets;
char 		 g_tokenBuffer[C1_MAXCFG];
int              g_totalSegments;
qma_uint16       g_nextPacketSeqno;

//
// These 4 are related to the number of digital LCQ's in the system.
// The LCQVO_List is used to moderate the startup grap for free memory.
//
int              g_number_of_diglcqs;
LCQ*             g_digLCQ_list;
PCQ* 		 g_digPCQ_list;
LCQVO*           g_digLCQVO_list;

ChanFreqMap 	 g_digMap_list;
ModuloCounter    g_dataPacketSeq;
TimeOfDay        g_curTime;
bool             g_startingDRSNNeeded;
c1_srvch         g_c1; 
MainStates       g_curState;
bool             g_outputPacketsQueued;
BTI		 g_currentTimeInfo;
InputPacket      g_inputQueue[MAX_SLIDING_WINDOW_SIZE];

//
//*****************************************************************************
// This is the main routine which receives data from the q330.
//*****************************************************************************
//

int nonmain(int argc, char *argv[])
{
  if(argc < 2)
  {
    Usage();
    return(0);
  }


  bool initialized = initialize(argv);
  if(!initialized)
  {
    std::cout << "xxx Error during Mountainair initialization. Exitting." 
	<< std::endl;
    cleanup(12);
    return 0;
  }

  g_done  = false;
  g_reset = false;

  while(!g_done)
  {
    reset();
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
  cleanup(12);
  return 0;
}
//
// Initialize the signal handlers and the variables
//

bool initialize(char* argv[])
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

  signal (SIGHUP, cleanup) ;
  signal (SIGINT, cleanup) ;
  signal (SIGQUIT, cleanup) ;

  bool res = readConfigFile(argv);

  if(res)
  {
    g_verbosity.setVerbosity(g_cvo.getVerbosity());
    g_verbosity.setDiagnostic(g_cvo.getDiagnostic());
    g_stateMachine.setState(RequestingServerChallenge);
  }
  else
  {
     std::cout << 
	"xxx Error reading Mountainair configuration values for station: " 
	<< argv[1] << std::endl;
  }

  return res;
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
  if(g_stateMachine.getState() == AcquiringData)
  {
    char clientbuf[600];
    comserv_scan(clientbuf);
  }
  return;
}

void check_state()
{

  MainStates mstate = g_stateMachine.getState();
  if(mstate == Resetting)
  {
    reset();
  }
  else if(mstate == Exitting)
  {
    cleanup(12);
  }
  return;
}
