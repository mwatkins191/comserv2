/*
 * File     :
 *  ReceiveMsgs.C
 *
 * Purpose  :
 *  Collection of routines to process Received msgs from the Q330. 
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  27 July 2002
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
#include "ReceiveMsgs.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "QmaDiag.h"
#include "msgs.h"
#include "TimeServer.h"
#include "global.h"
#include "ClockUtils.h"
#include "Cleanup.h"
#include "qmaswap.h"

#ifdef Q3302EW
#include "externs.h"
#include "heart.h"
#include "q3302ew_error.h"
#include "config.h"
#endif

bool processStatusResponse(const c1_stat& s1)
{
  bool retValue = false;
  if(s1.globalStatus())
  {
    g_log << "--- Initializing TimeStamp with Offsets : " 
		    << std::endl;
    g_log << "--- Seconds Offset : " << s1.getSecondsOffset() << 
	    std::endl;
    g_log << "--- Usecs Offset : " << s1.getUsecsOffset() <<
	    std::endl;
    g_timeServer.setSecondsOffset(s1.getSecondsOffset());
    g_timeServer.setUsecsOffset(s1.getUsecsOffset());
    g_timeServer.setMinutesSinceLoss(s1.getMinutesSinceLoss());
    g_timeServer.setClockQuality(s1.getClockQuality());
    g_timeServer.setCalibrationStatus(s1.getCalibrationStatus());
    retValue = true;
  }
  return retValue;
}


bool processFlagsResponse(const c1_flgs& f1)
{
  bool retValue = false;

  // Flag contains 4 variable length msgs. Create the
  // flags then move them to their own message.
  //
  c1_fix fix1;
  fix1.setBitString(f1.getOffsetBitString(
		    f1.getOffsetToFixedValues()));

  if(g_verbosity.show(D_MINOR,D_FIXED_FLAGS))
  { 
    g_log << "--- Fix field 1 Time of reboot : " << 
		fix1.getTimeOfLastReboot() << std::endl;
    g_log << "--- Total number of reboots : " << 
		fix1.getTotalNumberOfReboots() << std::endl;
  }    

  qma_uint32 dpacketmem = 0;
  int pnum = g_cvo.getQ330DataPortNumber();

  switch (pnum)
  {
    case 1:
    {
      dpacketmem = fix1.getDataPort1PacketMemorySize();
      break;
    }
    case 2:
    {
      dpacketmem = fix1.getDataPort2PacketMemorySize();
      break;
    }
    case 3:
    {
      dpacketmem = fix1.getDataPort3PacketMemorySize();
      break;
    }
    case 4:
    {
      dpacketmem = fix1.getDataPort4PacketMemorySize();
      break;
    }
    default:
    {
      g_log << "xxx Unexpected DataPortNumber: " << 
        g_cvo.getQ330DataPortNumber() << std::endl;
    }
  }

  g_dataPort.setPacketMemorySize(dpacketmem);

  g_Q330Version.setVersion(fix1.getSystemVersion());

  g_timeServer.setChan13FreqBitDelay(0,fix1.getChannels13Bit0FreqDelay());
  g_timeServer.setChan13FreqBitDelay(1,fix1.getChannels13Bit1FreqDelay());
  g_timeServer.setChan13FreqBitDelay(2,fix1.getChannels13Bit2FreqDelay());
  g_timeServer.setChan13FreqBitDelay(3,fix1.getChannels13Bit3FreqDelay());
  g_timeServer.setChan13FreqBitDelay(4,fix1.getChannels13Bit4FreqDelay());
  g_timeServer.setChan13FreqBitDelay(5,fix1.getChannels13Bit5FreqDelay());
  g_timeServer.setChan13FreqBitDelay(6,fix1.getChannels13Bit6FreqDelay());
  g_timeServer.setChan13FreqBitDelay(7,fix1.getChannels13Bit7FreqDelay());

  g_timeServer.setChan46FreqBitDelay(0,fix1.getChannels46Bit0FreqDelay());
  g_timeServer.setChan46FreqBitDelay(1,fix1.getChannels46Bit1FreqDelay());
  g_timeServer.setChan46FreqBitDelay(2,fix1.getChannels46Bit2FreqDelay());
  g_timeServer.setChan46FreqBitDelay(3,fix1.getChannels46Bit3FreqDelay());
  g_timeServer.setChan46FreqBitDelay(4,fix1.getChannels46Bit4FreqDelay());
  g_timeServer.setChan46FreqBitDelay(5,fix1.getChannels46Bit5FreqDelay());
  g_timeServer.setChan46FreqBitDelay(6,fix1.getChannels46Bit6FreqDelay());
  g_timeServer.setChan46FreqBitDelay(7,fix1.getChannels46Bit7FreqDelay());

  c1_sglob sglob;
  sglob.setBitString(f1.getOffsetBitString(
		f1.getOffsetToGlobalProgramming()));

  if(g_verbosity.show(D_MINOR,D_FIXED_FLAGS))
  {
	   g_log << "--- glob field 1 : clock timeout : " 
		<< sglob.getClockTimeout() << std::endl;
  }

  c1_sc sc;
  sc.setBitString(f1.getOffsetBitString(f1.getOffsetToSensorControl()));

  if(g_verbosity.show(D_MINOR,D_SENSORCONTROL_FLAGS))
  {
	 g_log << "--- sc field 1 : Sensor Output 1 : " 
		<< sc.getSensorOutput1() << std::endl;
  }

  c1_log l1;
  l1.setBitString(f1.getOffsetBitString(f1.getOffsetToDataPort()));

  if(g_verbosity.show(D_MINOR,D_LOGFLAGS_FLAGS))
  {
    g_log << "--- Data Port Info : Port Number : " << 
	      l1.getDataPortNumber() << " Seqno : " << 
	      l1.getPacketSequenceNumber() << " Flags : " 
	       << l1.getManualFilters() << std::endl;
  }
  //
  // Get the ackCount out of this packet
  //   
  //if(g_verbosity.show(D_MAJOR,D_FIXED_FLAGS))
  if(true)
  {
    g_log << "<-- Got LogMsg : " << std::endl;
    g_log << "<-- AckCount : " << l1.getAckCount() << std::endl;
    g_log << "<-- WindowSize : " << l1.getWindowSize() << std::endl;
    g_log << "<-- LastPacket seqn : " << l1.getPacketSequenceNumber() 
		<< std::endl;
    g_log << "<-- AckTimeout : " << 
	    l1.getAckTimeout() << std::endl;
  }

  //
  // Initialize ackCounters for both data and Cmds
  //
  g_ackCounter.initializeCounter(l1.getWindowSize(),
			             l1.getPacketSequenceNumber());
  g_nextPacketSeqno = l1.getPacketSequenceNumber();
  g_ackCounter.setAckGroupSize(l1.getAckCount()); 
  g_ackCounter.setAckTimeout(l1.getAckTimeout());
  g_ackCounter.resetAckCounter(); 
  retValue = true;
  return retValue;
}


bool processTokenResponse(const c1_mem& mm)
{
  bool retVal = false;
  c1_rqmem rmm;
  char reqHdr[SEQNUM_LEN]; 

  mm.getRequestHeader((qma_char*)&reqHdr[0]);

  /*
   * reqHdr was previously treated as a 64 bit int, so it was
   * swapped improperly (it's really a 32 followed by 2 16s)
   * We'll "unswap" it here, and then deal with it
   */
  qma_uint64 tmp;
  memcpy(&tmp, &reqHdr[0], 8);
  tmp = qma_htonll(tmp);
  memcpy(&reqHdr[0], &tmp, 8);

  rmm.setBitString((unsigned char*)&reqHdr[0]);

  if(g_verbosity.show(D_MINOR,D_RECEIVE_TOKENS))
  {
    g_log << "<-- Request returned addr : " <<
    rmm.getStartingAddress() << std::endl;
    g_log << "<-- ByteCount : " <<
    rmm.getByteCount() << std::endl;
    g_log << "<-- MemoryType : " << 
    rmm.getMemoryType() << std::endl;
    }

  g_totalSegments = mm.getTotalSegments();

  if(g_verbosity.show(D_MINOR,D_RECEIVE_TOKENS))
  {
    g_log << "<-- Segment Number : " <<
	    mm.getSegmentNumber() << std::endl;
     g_log << "<-- Total Segments : " <<
	  g_totalSegments << std::endl;
  }

  //
  // The assumption is that getByteCount returns number of
  // bytes in segment buffer plus 4 bytes segment totals.
  // The geSegmentBuffer should return just the segment buffer
  // and not the segment counts.
  //
	
  char tempbuf[C1_MAXSEG];
  mm.getSegmentBuffer((char*)&tempbuf[0]);
  int len = rmm.getByteCount();
  len = len - 4; // remove len of segment numbers

  if(g_verbosity.show(D_MINOR,D_RECEIVE_TOKENS))
  {
    g_log << "--- Found token buffer len of " << len << std::endl;
  }

  memcpy((char*)&g_tokenBuffer[g_bytesInBuffer],(char*)&tempbuf[0],len);
  g_bytesInBuffer = g_bytesInBuffer + len;
	
  //
  // add overhead to total to find addresss of next segments
  //
  g_nextTokenAddress = g_nextTokenAddress + len + C1_OVERHEAD;
  retVal = true;
  return retVal;
}

void initializeTokenBuffer()
{
   memset((char*)&g_tokenBuffer[0],0,C1_MAXCFG);
   g_totalSegments = 0;
   g_segmentsReceived = 0;
   g_bytesInBuffer = 0;
   g_nextTokenAddress = 0x00;
}

bool createLCQsAndPCQs()
{

  int i;

  // It is not defined operation in C++ to [] delete
  // twice in a row. Use the g_lcq_freed as flag to determine
  // it they should be deleted.
  if(!g_lcq_freed)
  {
    g_log << "xxx Unexpected need to free LCQ list in RecevieMsgs."
      << std::endl;
    delete [] g_digLCQ_list;
    delete [] g_digPCQ_list;
    delete [] g_mainLCQ_list;
    delete [] g_mainPCQ_list;
  }
  g_digLCQ_list = new LCQ[g_number_of_diglcqs];
  g_digPCQ_list = new PCQ[g_number_of_diglcqs];
  g_lcq_freed = false;

  for(i=0;i<g_number_of_diglcqs;i++)
  { 
    g_digLCQ_list[i].setLCQVO(g_digLCQVO_list[i]);
    g_digPCQ_list[i].setLCQVO(g_digLCQVO_list[i]);
  }

  if(g_verbosity.show(D_MAJOR,D_LCQ_CREATE))
  {
    for(int x=0;x<g_number_of_diglcqs;x++)
    {

      g_log << "--- Created Digital LCQ and PCQ - number : " <<
          x << "  " <<
          g_digPCQ_list[x].getLCQVO().getSEEDName() <<
                 std::endl;
    }
  }

  g_mainLCQ_list = new LCQ[g_number_of_mainlcqs];
  g_mainPCQ_list = new PCQ[g_number_of_mainlcqs];
  g_lcq_freed = false;

  for(i=0;i<g_number_of_mainlcqs;i++)
  { 
    g_mainLCQ_list[i].setLCQVO(g_mainLCQVO_list[i]);
    g_mainPCQ_list[i].setLCQVO(g_mainLCQVO_list[i]);
  }

  if(g_verbosity.show(D_MAJOR,D_LCQ_CREATE))
  {
    for(int x=0;x<g_number_of_mainlcqs;x++)
    {

      g_log << "--- Created main LCQ and PCQ - number : " <<
          x << "  " <<
          g_mainPCQ_list[x].getLCQVO().getSEEDName() <<
                 std::endl;
    }
  }
  //
  // LCQVO list used to initialization of LCQ's only. Safe to remove now.
  //
  delete [] g_digLCQVO_list;
  delete [] g_mainLCQVO_list;
  return true;
}


bool processPeriodicStatusResponse(const c1_stat& s1)
{
  bool retValue = false;

  //
  // This is diagnostic code that turns on high verbosity on low data rate.
  //
  if(false)
  {
    if(g_dataPort.getRxPacketRate() < 0.5)
    {
      g_log << "+++ Turning Verbosity up due to low packet rate:" 
        << std::endl;
      g_verbosity.setVerbosity(D_EVERYTHING);
    }
    else
    {
      g_log << "+++ Turning Verbosity off due to reasonable packet rate:" 
       << std::endl;
      g_verbosity.setVerbosity(D_SILENT);
    }
  }

  if(g_verbosity.show(D_EVERYTHING,D_FILL_PACKETS))
  {
      g_log << "--- Fill Packets Received: " <<
	g_totalFillPackets << std::endl; 
  }

  retValue = true;

  if(s1.dpTokensChanged())
  {
    g_log << 
      "+++ Received status message indicating DP Tokens have changed.";
    g_log << " Resetting to read new tokens." <<  
	std::endl;
   CleanQMA(12); // This causes program to flush queues and exit.
  }

  if(s1.dataPortProgrammingChanged())
  {
    g_log << 
      "+++ Received status indicating Data Port Programming has changed." << 
	std::endl;
    g_log << "+++ Continuing with existing configuration." <<  
	std::endl;
    g_stateMachine.setState(Resetting);
  }

  if(s1.globalStatus())
  {
    g_timeServer.setDRSN(s1.getDRSN());
    g_timeServer.setMinutesSinceLoss(s1.getMinutesSinceLoss());
    g_timeServer.setClockQuality(s1.getClockQuality());
    g_timeServer.setCalibrationStatus(s1.getCalibrationStatus());
    g_timeServer.setSecondsOffset(s1.getSecondsOffset());
    g_timeServer.setUsecsOffset(s1.getUsecsOffset());

    g_log << "--- Q330 Time: " << 
         g_timeServer.currentSystemStatusTimeString() << std::endl;


    //
    // Display status info on packet rate
    //

    time_t nowtime = time(0);
    g_log << "--- Mountainair status for: " <<
		 g_tvo.getNetStationVO().getNetworkCode() << "." <<
                 g_tvo.getNetStationVO().getStationCode() << " rx'd at: " <<
                 asctime(gmtime(&nowtime));

    g_log << "--- Rx Data: "
      << g_dataPort.getRxCurrentPackets() << 
      " new packets, total Packets rx'd: " <<
        g_dataPort.getRxTotalPackets() << std::endl;

    g_log << "--- Rx Packet Rate is: "
      << g_dataPort.getRxDataRate()
      << " bytes/sec or " <<
         g_dataPort.getRxPacketRate()
      << " pkts/sec for last " <<
         g_dataPort.secondsOfData() << " secs." << std::endl;

    int portStat = s1.userDataPortStatusPresent();
    if(portStat != 0)
    {
    
      int queSize = s1.getDataPortQueue();
      g_log << "--- Q330 Data Queue Available: " << 
        (int) g_dataPort.getPacketBufferPercentFree(queSize) << "%" << 
        ", Timing Quality: " <<
         (qma_uint16) translate_clock(g_tvo.getClockProcVO(),
                                   g_timeServer.getClockQuality(),
                                   g_timeServer.getMinutesSinceLoss()) 
                      << "%" << std::endl;
    }
    else
    {

      g_log << "--- Timing Quality: " <<
         (qma_uint16) translate_clock(g_tvo.getClockProcVO(),
                                   g_timeServer.getClockQuality(),
                                   g_timeServer.getMinutesSinceLoss()) 
                      << "%" << std::endl;

    }
    g_dataPort.adjustCalcInterval();


    if(g_verbosity.show(D_EVERYTHING,D_CLOCKQUALITY))
    {
      g_log << "--- Current Clock Quality: " <<
        s1.getClockQuality() << std::endl;

      g_log << "--- MinutesSinceLoss: " << 
        s1.getMinutesSinceLoss() << std::endl;

      g_log << "--- Calibration Status: " <<
        (qma_uint16) s1.getCalibrationStatus()
         << std::endl;
     
      g_log << "+++ Current Timing Quality: " << (qma_uint16)
	translate_clock(g_tvo.getClockProcVO(),
                        g_timeServer.getClockQuality(),
                        g_timeServer.getMinutesSinceLoss()) << std::endl;
    }
  }

  if(s1.userMessagePresent())
  {
    g_log << "+++ Received User Message: " << s1.getUserMessage() << 
	std::endl;

    struct sockaddr_in temp;
    temp.sin_family = AF_INET;
    temp.sin_addr.s_addr = s1.getUserIPAddress();
    memset(&(temp.sin_zero),'\0',8);
    g_log << "+++ Received Message from IP: " << inet_ntoa(temp.sin_addr) 
	<<  std::endl;
  }

#ifdef Q3302EW
  /*
   * This is where we'll do our SOH checking, and send error messages if
   * needed
   */

  // check our clock quality, and serve an error if it's bad
  char errStr[100];
  if(g_currentClockQuality < gConfig.minclockquality) {
    sprintf(errStr, "%s (%d%%)", ERR_CLOCK_QUALITY, g_currentClockQuality);
    message_send(TypeErr, 0, errStr);
  }

  if(s1.globalStatus()) {
    qma_int16 boomPos;
    boomPos = s1.getBoomPosition(1);
    if(boomPos < gConfig.minmass1position) {
      sprintf(errStr, "%s mass 1 (%d)", ERR_BOOM_TOO_LOW, boomPos);
      message_send(TypeErr, 3, errStr);
    } else if (boomPos > gConfig.maxmass1position) {
      sprintf(errStr, "%s mass 1 (%d)", ERR_BOOM_TOO_HIGH, boomPos);
      message_send(TypeErr, 4, errStr);
    }

    boomPos = s1.getBoomPosition(2);
    if(boomPos < gConfig.minmass2position) {
      sprintf(errStr, "%s mass 2 (%d)", ERR_BOOM_TOO_LOW, boomPos);
      message_send(TypeErr, 3, errStr);
    } else if (boomPos > gConfig.maxmass2position) {
      sprintf(errStr, "%s mass 2 (%d)", ERR_BOOM_TOO_HIGH, boomPos);
      message_send(TypeErr, 4, errStr);
    }

    boomPos = s1.getBoomPosition(3);
    if(boomPos < gConfig.minmass3position) {
      sprintf(errStr, "%s mass 3 (%d)", ERR_BOOM_TOO_LOW, boomPos);
      message_send(TypeErr, 3, errStr);
    } else if (boomPos > gConfig.maxmass3position) {
      sprintf(errStr, "%s mass 3 (%d)", ERR_BOOM_TOO_HIGH, boomPos);
      message_send(TypeErr, 4, errStr);
    }

    boomPos = s1.getBoomPosition(4);
    if(boomPos < gConfig.minmass4position) {
      sprintf(errStr, "%s mass 4 (%d)", ERR_BOOM_TOO_LOW, boomPos);
      message_send(TypeErr, 3, errStr);
    } else if (boomPos > gConfig.maxmass4position) {
      sprintf(errStr, "%s mass 4 (%d)", ERR_BOOM_TOO_HIGH, boomPos);
      message_send(TypeErr, 4, errStr);
    }

    boomPos = s1.getBoomPosition(5);
    if(boomPos < gConfig.minmass5position) {
      sprintf(errStr, "%s mass 5 (%d)", ERR_BOOM_TOO_LOW, boomPos);
      message_send(TypeErr, 3, errStr);
    } else if (boomPos > gConfig.maxmass5position) {
      sprintf(errStr, "%s mass 5 (%d)", ERR_BOOM_TOO_HIGH, boomPos);
      message_send(TypeErr, 4, errStr);
    }

    boomPos = s1.getBoomPosition(6);
    if(boomPos < gConfig.minmass6position) {
      sprintf(errStr, "%s mass 6 (%d)", ERR_BOOM_TOO_LOW, boomPos);
      message_send(TypeErr, 3, errStr);
    } else if (boomPos > gConfig.maxmass6position) {
      sprintf(errStr, "%s mass 6 (%d)", ERR_BOOM_TOO_HIGH, boomPos);
      message_send(TypeErr, 4, errStr);
    }

    qma_uint16 inputPower = s1.getInputPower() * 150;
    if(inputPower < gConfig.mininputpower) {
      sprintf(errStr, "%s (%dmv)", ERR_INPUT_POWER_LOW, inputPower);
      message_send(TypeErr, 1, errStr);
    } else if(inputPower > gConfig.maxinputpower) {
      sprintf(errStr, "%s (%dmv)", ERR_INPUT_POWER_HIGH, inputPower);
      message_send(TypeErr, 2, errStr);
    }      
  }
#endif

  return retValue;
}

bool processCmdErrorMessage(const c1_cerr& err)
{
  bool retValue = false;
  g_log << "xxx Received error response : " << err.getErrorCode()
        << " while in state : " << g_stateMachine.getState() << std::endl;

  qma_uint16 errval = err.getErrorCode();

  switch(errval)
  {
    case 0:
    {
      g_log << 
       "xxx Error Type : No Permission - Invalid Password." << std::endl;
      g_stateMachine.setState(Exitting);
      break;
    }
    case 1:
    {
      g_log <<  "xxx Error Type : Too Many Configuration or ";
      g_log << "Special Functions Servers Registered, cannot add you."
        << std::endl;
      g_stateMachine.setState(Exitting);
      break;
    }
    case 2:
    {
      g_log << 
        "xxx Error Type : You are not registered. Please Register." 
           << std::endl;
      // Only reset to this if we are in the acquiring data stage.
      if( (g_stateMachine.getState() == OpeningLocalPorts) ||
          (g_stateMachine.getState() == RequestingServerChallenge) ||
          (g_stateMachine.getState() == RequestingChallengeResponse) )
      {
        // Don't reset in these states as we aren't registered yet.
      }
      else
      { 
        g_stateMachine.setState(Resetting);
      }
      break;
    }
    case 3:
    {
      g_log << 
        "xxx Error Type : ";
      g_log << "Invalid Registration Response, cannot honor your request." 
        << std::endl;
      g_stateMachine.setState(Exitting);
      break;
    }
    case 4:
    {
      g_log << "xxx Error Type : Parameter Error." << std::endl;
      break;
    }
    case 5:
    {
      g_log << "xxx Error Type : Structure Not Valid, Tried to read an ";
      g_log << "EEPROM structure that is not valid." << std::endl;
      g_stateMachine.setState(Exitting);
      break;
    }
    case 6:
    {
      g_log << "xxx Error Type : Configuration Only, you are not ";
      g_log << "allowed to use this command except on the ";
      g_log << "configuration port." 
		<< std::endl;
      g_stateMachine.setState(Exitting);
      break;
    }
    case 7:
    {
      g_log << "xxx Error Type : Special Functions Port Only, you are ";
      g_log << "not allowed to use this command except on the special ";
      g_log << "functions port." << std::endl;
      g_stateMachine.setState(Exitting);
      break;
    }
    case 8:
    {
      g_log << "xxx Error Type : Memory Operation in Progress, try your ";
      g_log << "memory access command again later." << std::endl;
      break;
    }
    case 9:
    {
      g_log << "xxx Error Type : Calibration in Progress, try your ";
      g_log << " calibration start of mass re-centering command later."
	<< std::endl;
      break;
    }
    case 10:
    {
      g_log << "xxx Error Type : Data not yet available for QuickView." 
      << std::endl;
      g_stateMachine.setState(Exitting);
      break;
    }
    case 11:
    {
      g_log << "xxx Error Type : Console (virtual) Interface Only, you ";
      g_log << "are not allowed to use this command except on the front ";
      g_log << "panel connector." << std::endl;
      g_stateMachine.setState(Exitting);
      break;
    }
    case 12:
    {
      g_log << "xxx Error Type : Flash Write or Erase Error." << std::endl;
      g_stateMachine.setState(Exitting);
      break;
    }
    default:
    {
      g_log << "xxx Error Type : Unknown Error Number: " << errval 
       << std::endl;
      g_stateMachine.setState(Exitting);
      break;
    }
  }
  return retValue;
}
