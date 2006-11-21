/*
 * File     :
 *  RxMsg.C
 *
 * Purpose  :
 *  This routine checks the input ports, receives and processes any messages
 *  that it finds.
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
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "QmaDiag.h"
#include "RxMsg.h"
#include "QMA_Port.h"
#include "StateMachine.h"
#include "ReceiveMsgs.h"
#include "qmautils.h"
#include "QueueDP.h"
#include "ProcessIQ.h"
#include "global.h"

void rx_msg()
{
  Packet  receivedPacket;

  if(checkInCmdPort(receivedPacket))
  {
    processCmdPacket(receivedPacket);
  }

  bool packetQueued = false;

  if(checkInDataPort(receivedPacket))
  {
     packetQueued = processDataPacket(receivedPacket);
  }

  if(packetQueued)
  {
    processInputQueue();
  }

  sendAcksIfNeeded();
   
  return;
} 


bool checkInCmdPort(Packet& in_p)
{

  char            buf[MAX_BYTES_IN_PACKET];
  bool retValue = false;

  int numbytes=g_cmdPort.read((unsigned char*)buf);
  if(numbytes < 0)
  {
    //
    // Socket Read error. Exit with error
    //
    retValue = false;
    g_stateMachine.setState(Resetting);
  }
  else if(numbytes == 0)
  {
    //
    // We read the port, and no data was available, so we continue on to poll
    // other parts of the system that need attention.
    //
    retValue = false;
  }
  else if(numbytes < QDP_HEADER_SIZE_IN_BYTES)
  {
    //
    // Receiving a message with less that header amount of data is always
    // considered an error. We should get all or nothing with UDP packets.
    //
    retValue = false;
    g_stateMachine.setState(Resetting);
  }
  else
  {
    //
    // In this section, more the process data to
    //
    in_p.setBitString((unsigned char*)buf,numbytes);
    if(!in_p.checkCRC())
    {
      //if(g_verbosity.show(D_MAJOR,D_CRC_CHECK))
      if(true)
      {
        g_log << "xxx Error. Dropped packet due to CRC failure." 
	  " numbytes " << numbytes << std::endl;
        QDPHeader in_qdp;
        in_qdp.setBitString(in_p.getQDPHeaderBitString());

        g_log << "--- Rx'd command  : " << (qma_uint32) in_qdp.getCommand()
              << std::endl;
        g_log << "--- Rx'd version  : " << (qma_uint32) in_qdp.getVersion()
              << std::endl;
        g_log << "--- Rx'd length   : " << (qma_uint32) 
           in_qdp.getLengthInBytes()
              << std::endl;
        g_log << "--- Rx'd seqno    : " <<
          (qma_uint32) in_qdp.getPacketSequence() << std::endl;
        g_log << "--- Rx'd acknumber: " << (qma_uint32) in_qdp.getAckNumber()
              << std::endl;

        in_p.printPacketContents();
      }
      retValue = false;
    }
    else
    {
      if(g_verbosity.show(D_MINOR,D_RX_CMD_PACKET))
      {
        QDPHeader in_qdp;
        in_qdp.setBitString(in_p.getQDPHeaderBitString());
        g_log << "<-- Received a cmd Packet. Type is : " <<
          in_qdp.getCommand() << std::endl;
      }
      retValue = true;
    }
  }
  return retValue;
}


bool checkInDataPort(Packet& in_p)
{
  char            buf[MAX_BYTES_IN_PACKET];
  bool retValue = false;

  int numbytes=g_dataPort.read((unsigned char*)buf);
  if(numbytes < 0)
  {
    //
    // Socket Read error. Exit with error
    //
    retValue = false;
  }
  else if(numbytes == 0)
  {
    //
    // We read the port, and no data was available, so we continue on to poll
    // other parts of the system that need attention.
    //
    retValue = false;
  }
  else if(numbytes < QDP_HEADER_SIZE_IN_BYTES)
  {
    //
    // Receiving a message with less that header amount of data is always
    // considered an error. We should get all or nothing with UDP packets.
    //
    retValue = false;
  }
  else
  {
    //
    // In this section, move the process data to
    //
    in_p.setBitString((unsigned char*)buf,numbytes);
    if(!in_p.checkCRC())
    {
      //if(g_verbosity.show(D_MAJOR,D_CRC_CHECK))
      if(true)
      {
        g_log << "xxx Error. Dropped Data packet due to CRC failure." 
	  <<std::endl;
        QDPHeader in_qdp;
        in_qdp.setBitString(in_p.getQDPHeaderBitString());

        g_log << "--- Rx'd command  : " << (qma_uint32) in_qdp.getCommand()
              << std::endl;
        g_log << "--- Rx'd version  : " << (qma_uint32) in_qdp.getVersion()
              << std::endl;
        g_log << "--- Rx'd length   : " << (qma_uint32) in_qdp.getLengthInBytes()
              << std::endl;
        g_log << "--- Rx'd seqno    : " <<
           (qma_uint32) in_qdp.getPacketSequence() << std::endl;
        g_log << "--- Rx'd acknumber: " << (qma_uint32) in_qdp.getAckNumber()
              << std::endl;

         in_p.printPacketContents();
      }
      retValue = false;
    }
    else
    {
      retValue = true;
    }
  }
  return retValue;
}

//
// In the process routines. we know we have a valid message, with
// enough bytes for a header in the in_p var. Test for errors, then
// do state processing.
//
bool processCmdPacket(const Packet& in_p)
{
  bool retValue = false;
  QDPHeader in_qdp;

  in_qdp.setBitString(in_p.getQDPHeaderBitString());


  if (in_qdp.getCommand() == C1_CERR_VAL)
  {
    c1_cerr err;
    err.setBitString(in_p.getDataBitString());
    processCmdErrorMessage(err);
    return retValue;
  }

  MainStates curState = g_stateMachine.getState();
  switch(curState)
  {
    //*******************************
    // In this state we are looking to receive a Challenge from the server.
    //
    //***********************
    case RequestingServerChallenge:
    {
      if (in_qdp.getCommand() == C1_SRVCH_VAL)
      {
        if(true)
        {
	    g_log << "<-- Got challenge from q330" << std::endl;
        }
	g_packet_for_tx.setBitString(in_p.getBitString(),
				     in_p.getLengthInBytes());
	g_stateMachine.setState(RequestingChallengeResponse);
	g_cmdTimer.stop();
      }
      else
      {
	g_log << "xxx Received unexpected message type while expecting "<<
	  "a challenge response."<< std::endl;
	g_log << "xxx Receive message Type : " << 
	  in_qdp.getCommand() << " of length: " << in_qdp.getLengthInBytes() <<
	    std::endl;
      }
      break;
    }
    
    //****************************
    // We have sent a server challenge and are expecing a server response.
    // The expected response is an ACK.
    //******************************
    case RequestingChallengeResponse:
    {   
      if (in_qdp.getCommand() == C1_CACK_VAL)
      {
        g_log << "<-- Got challenge acceptance from q330." << std::endl;;
        g_log << "+++ Registered with Q330. Ready to request Q330 Status." 
	  << std::endl;;
	g_cmdTimer.stop();
	g_stateMachine.setState(RequestingStatus);
      }
    }

    //****************************
    // We have sent a status request and are expecing a status response.
    // The expected response is an STAT_VAL
    //******************************
    case RequestingStatus:
    {   
      if (in_qdp.getCommand() == C1_STAT_VAL)
      {
	c1_stat s1;
	s1.setBitString(in_p.getDataBitString(),in_p.getDataLengthInBytes());
        if(processStatusResponse(s1) == true)
	{
          g_log << "<-- Got global status from q330." << std::endl;;
          g_log << "+++ Ready to request flags from Q330." << std::endl;;
	  g_cmdTimer.stop();
	  g_stateMachine.setState(RequestingFlags);
	}
      }
    }

    //****************************
    // We have sent a Flags request and are expecing a flags response.
    // The expected response is an C1_FLGS.
    //******************************
    case RequestingFlags:
    {   
      if (in_qdp.getCommand() == C1_FLGS_VAL)
      {
	c1_flgs f1;
        f1.setBitString(in_p.getDataBitString(),in_p.getDataLengthInBytes());

        if(processFlagsResponse(f1) == true)
	{
            g_log << "<-- Got flags from q330." << std::endl;;
            g_log << "+++ Ready to request Tokens from Q330." << std::endl;
	    g_cmdTimer.stop();
	    initializeTokenBuffer();
	    g_stateMachine.setState(RequestingTokens);
	}
      }
      break;
    }

    //****************************
    // We have sent a Token request and are expecting a memory response.
    // The expected response is an C1_MEM
    //******************************
    case RequestingTokens:
    {   
      if (in_qdp.getCommand() == C1_MEM_VAL)
      {
	c1_mem mm;
        mm.setBitString(in_p.getDataBitString(),
			in_p.getDataLengthInBytes());
        if(processTokenResponse(mm) == true)
	{
	  ++g_segmentsReceived;
          if(g_segmentsReceived == g_totalSegments)
          {
            g_tvo.processTokenBuffer((char*)&g_tokenBuffer[0],g_bytesInBuffer);
            g_log << "<-- Received and processed all Tokens from q330." 
		      << std::endl;
	    g_cmdTimer.stop();
	    if(createLCQsAndPCQs() != true)
            {
	      g_log << "xxx Unable to create LCQ's. Exitting on this error"
			<< std::endl;
	      g_stateMachine.setState(Exitting);
	    }
	    else
	    {
              // Validate and display the tokens
              validateTokens();
	      g_stateMachine.setState(SendingUserMessage);
	    }
	  }
	  else
	  {
	    // For next segment, restart counters
	    g_cmdTimer.restartInterval();
	    g_cmdTimer.restartCount();
	  }
	}
      }
      break;
    }

    //****************************
    // We have sent an User Message Cmd.
    // We are expecting a C1_CACK_VAL response
    //******************************
    case SendingUserMessage:
    {   
      if (in_qdp.getCommand() == C1_CACK_VAL)
      {
	g_log << "+++ Received User Message Acknowledgement." << std::endl;
	g_cmdTimer.stop();
	g_stateMachine.setState(AcquiringData);
      }
      break;
    }

    case AcquiringData:
    {   
      //
      // While acquiring data, several messages need to be handled.
      //
      // (a) A "Tokens have changed" message
      // (b) periodic status messages
      // (c) 
      //
      if(g_verbosity.show(D_EVERYTHING,D_CMD_MSG))
      {
        g_log << "--- Received msg on cmd port while acquiring data: " <<
	in_qdp.getCommand() << std::endl;
      }
      if(in_qdp.getCommand() == C1_STAT_VAL)
      {
	c1_stat s1;
        if(g_verbosity.show(D_EVERYTHING,D_CMD_MSG))
        {
          g_log << "--- Got Status response of length : " <<
	  in_p.getDataLengthInBytes() << std::endl;
        }
	g_statusTimer.restartInterval();
	s1.setBitString(in_p.getDataBitString(),in_p.getDataLengthInBytes());
        processPeriodicStatusResponse(s1);  
      }	
      break;
    }

    //****************************
    // We have sent a Disconnect request and are expecing an Ack response.
    // The expected response is an C1_ACK
    //******************************
    case Resetting:
    {   
      if (in_qdp.getCommand() == C1_CACK_VAL)
      {
	  g_cmdTimer.stop();
	  g_stateMachine.setState(Exitting);
      }
      break;
    }
  }
  return retValue;
}

bool processDataPacket(const Packet& in_p)
{

  bool retValue = false;
  QDPHeader in_qdp;

  in_qdp.setBitString(in_p.getQDPHeaderBitString());
  if(g_verbosity.show(D_MINOR,D_DATA_PACKETS))
  {
    g_log << "--- Received packet on data port of type: " <<
      in_qdp.getCommand() << std::endl;
  }

  if((in_qdp.getCommand() == DT_DATA_VAL) ||
	 (in_qdp.getCommand() == DT_FILL_VAL))
  {
        g_dataPortTimer.restartInterval();
        g_dataPortTimer.restartCount();
	retValue = queuePacket(in_p);
  }
  else
  {
        g_log << "xxx Received unexpected packet of type: " <<
	   in_qdp.getCommand() << " on data port." << std::endl;
  }
  return retValue;
}
