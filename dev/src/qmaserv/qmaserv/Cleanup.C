/*
 * File     :
 *  Cleanup.C
 *
 * Purpose  :
 *  This is the cleanup routine which is called to exit the program.
 *  The signal number parameter is used when registering as a signal
 *  handler callback routine.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  8 July 2002
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
 *
 */
#include <iostream>
#include <errno.h>
#ifndef _WIN32
# include <unistd.h>
#endif
#include "Cleanup.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "StateMachine.h"
#include "global.h"
#include "CheckPCQS.h"
#include "QMA_Version.h"
#include "qmautils.h"
#include "qmaserv.h"

#ifdef Q3302EW
extern "C" {
# include <local_earthworm_complex_funcs.h>
# include <earthworm.h>
};
#endif

void ResetQMA()
{
  g_log << "+++ Setting QMASERV program to starting state." << std::endl;
  showVersion();
  MainStates curstate = g_stateMachine.getState();

  g_cmdTimer.stop();
  g_dataPortTimer.stop();
  g_statusTimer.stop();

  if(curstate > RequestingServerChallenge)
  {
    g_log << "+++ Writing Data in compression queues to disk with DRSN : " 
       << g_currentTimeInfo.drsn << std::endl;

    empty_pcqs();

    //
    // Send disconnect message
    //

    c1_dsrv   dsrv;
    QDPHeader out_qdp;
    Packet    out_p;

    dsrv.setSerialNumber(g_cvo.getQ330SerialNumber());

    out_qdp.setCommand(C1_DSRV_VAL);
    out_qdp.setVersion(QDP_VERSION);
    out_qdp.setDataLengthInBytes(dsrv.getLengthInBytes());
    out_qdp.setPacketSequence(g_cmdPacketSeq.next());
    out_qdp.setAckNumber(0x00);

    out_p.setDataBitString(dsrv.getBitString(),dsrv.getLengthInBytes());
    out_p.setQDPHeaderBitString(out_qdp.getBitString());
    out_p.setCRC();

    int send_ok = g_cmdPort.write((char*)
                                    out_p.getBitString(),
                                    out_p.getLengthInBytes());
    if(!send_ok)
    {
          g_log << "xxx Error sending exiting disconnect with errno :"
                << errno << std::endl;
    }
    g_log << "+++ Sent disconnect message while resetting." << std::endl;
  }
  g_cmdPort.closePort();
  g_dataPort.closePort();

  //
  // Reset the global variables to a starting value
  //
  g_segmentsReceived = 0;
  g_nextTokenAddress = 0;
  g_bytesInBuffer = 0;
  g_totalSegments = 0;
  g_totalFillPackets = 0;
  g_number_of_diglcqs = 0;
  g_number_of_mainlcqs = 0;
  g_startingDRSNNeeded = true;
  g_nextPacketSeqno = 0;
  g_reset = true;
  g_outputPacketsQueued = false;

  //
  // Free the dynamic array lists
  //
  if(!g_lcq_freed)
  {
    delete [] g_digLCQ_list;
    delete [] g_digPCQ_list;

    delete [] g_mainLCQ_list;
    delete [] g_mainPCQ_list;

    // These are freed on creation of LCQs. Don't
    // free them again here.
    // delete [] g_digLCQVO_list;
    // delete [] g_mainLCQVO_list; 

    g_lcq_freed = true;
  }
  g_digMap_list.reset();
  g_mainMap_list.reset();

  //
  // This statement will drop messages in the input queue on reset.
  // Believed to introduce datagaps becuase ack'd packets are dropped.
  //
  // clearInputQueue();

  g_stateMachine.setState(OpeningLocalPorts);
  return;
}

//
// Use cleanup to exit on fatal errors
//
extern "C" void CleanQMA(int sig)
{
  g_log << "+++ Exiting QMASERV" << std::endl;
  ResetQMA();
  time_t nowtime = time(0);
  g_log << "+++ Qmaserv Exitting at " << 
	asctime(gmtime(&nowtime));
  g_done = true;
#if defined(Q3302EW) && defined(_WIN32)
  sleep_ew(10 * 1000);
#else
  sleep(10);
#endif
  exit(sig);
}

//
// Use shutdown when you've received a signal from netmon
//
void ShutdownQMA()
{
 g_log << "+++ Shutting down QMASERV" << std::endl;
  empty_pcqs();
  ResetQMA();
  time_t nowtime = time(0);
  g_log << "+++ Qmaserv waiting for terminate signal at " << 
	asctime(gmtime(&nowtime));
  g_done = true;

  //
  // Sleep in order to give netmon a chance to kill clients and then
  // terminate qmaserv gracefully.
  //

  bool terminated = false;
  time_t termtime;
  while(!terminated)
  {

    termtime = time(0);
#ifndef Q3302EW
    if(comserv_scan() == QMA_TERMINATE)
    {
      terminated = true;
      g_log << "+++ Qmaserv exitting on netmon terminate signal at " << 
	std::asctime(gmtime(&nowtime)) << std::endl;
    }
    else 
#endif
    if(
       ((long) termtime - (long) nowtime) > MAX_TERMINATE_WAIT)
    {
      terminated = true;
      g_log << "+++ Qmaserv exitting on max terminate wait at " << 
	asctime(gmtime(&nowtime)) << std::endl;
    }
  }
  exit(12);
}

void clearInputQueue()
{
  for(int i=0;i<MAX_SLIDING_WINDOW_SIZE;i++)
  {
    g_inputQueue[i].full = false;
  }
}
