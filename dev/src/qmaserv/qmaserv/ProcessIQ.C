/*
 * File:
 *  ProcessIQ.C
 *
 * Purpose:
 *  These routines process the incoming data packets.
 *
 * Author:
 *   Phil Maechling
 *
 * Created:
 *   27 April 2002
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
#include <errno.h>
#include "QmaDiag.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "ProcessIQ.h"
#include "Verbose.h"
#include "InputPacket.h"
#include "ProcessMN232.h"
#include "ProcessAG816.h"
#include "ProcessMN816.h"
#include "ProcessAG38.h"
#include "ProcessMN38.h"
#include "ProcessDCD32.h"
#include "ProcessDCComp.h"
#include "ProcessDCMult.h"
#include "CheckPCQS.h"
#include "msgs.h"
#include "global.h"
#include "Cleanup.h"
#include "qmaswap.h"


void processInputQueue()
{
  bool packetsToProcess = true;
  while(packetsToProcess)
  {
    qma_uint16 nextSeqno = g_nextPacketSeqno;
    qma_uint16 queuePosition = nextSeqno & 0x007F;
    if(g_inputQueue[queuePosition].full)
    {
      Packet in_p = g_inputQueue[queuePosition].packet;
      QDPHeader in_qdp;
      in_qdp.setBitString(in_p.getQDPHeaderBitString());
      qma_uint16 cur_rxseq = in_qdp.getPacketSequence();

      if(cur_rxseq != g_nextPacketSeqno)
      {
        if(g_verbosity.show(D_EVERYTHING,D_INPUT_QUEUE))
        {
          g_log << "xxx Input queue packet being processed "
	   << "is not equal to Sliding Window g_nextPacketSeqno."
           << std::endl;
          g_log << "xxx PacketSeqno in input queue: " << cur_rxseq 
           << std::endl;
          g_log << "xxx Next Unprocessed PacketSeqno in Sliding Window : " 
            << g_nextPacketSeqno << std::endl;
          g_log << "xxx Sliding Window Start: " <<
		g_ackCounter.getLastUnAckedSeqno() << std::endl;
        }
	g_inputQueue[queuePosition].full = false;
      }
      else if(!g_ackCounter.inCurrentWindow(cur_rxseq))
      {
        g_log << "xxx InputQueue packet being processed is " <<
          " not in current sliding window." << std::endl;
        g_log << "xxx Input queue PacketSeqno being processed: " << 
          cur_rxseq << std::endl;
        g_log << "xxx Next Sliding Window PacketSeqno : " << 
          g_nextPacketSeqno << std::endl;
        g_log << "xxx Sliding Window Start: " <<
		g_ackCounter.getLastUnAckedSeqno() << std::endl;
        g_log << "xxx Sliding Window size: " << 
            g_ackCounter.getSlidingWindowSize() << std::endl;

	g_inputQueue[queuePosition].full = false;
      }
      else
      {
	//
	// Mark the packet as processed in InputQueue
	//
	g_inputQueue[queuePosition].full = false;
        ++g_nextPacketSeqno;

	qma_uint32 msgType = in_qdp.getCommand();

        if((msgType != DT_DATA_VAL) && (msgType != DT_FILL_VAL)) 
	{
	  g_log << "xxx Error Unexpected msgType: " <<
	    msgType << " on data port." << std::endl;
	  return;
	}
        else if (msgType == DT_FILL_VAL)
        {
	  //
	  // Do fill processing by count Fill packets.
           ++g_totalFillPackets;
           if(g_verbosity.show(D_EVERYTHING,D_FILL_PACKETS))
           {
             g_log << "--- Got fill from q330. Current Fill Packet Total: " 
		<< g_totalFillPackets << std::endl;
           }
	}
	else // Is Data Packet
        {
          dt_data dt;
          dt.setBitString(in_p.getDataBitString(),
		          in_qdp.getDataLengthInBytes());

          if(g_verbosity.show(D_MINOR,D_PROCESS_IQ))
          {
            g_log << "--- Read data packet from InputQueue: " << cur_rxseq 
	      << " and drsn: " << dt.getDataRecordSequenceNumber() << std::endl;
          }

	  if(dt.packetIsStartOfSecond())
	  {
	      g_currentTimeInfo = dt.getCurrentTimeInfo();
              if(g_verbosity.show(D_EVERYTHING,D_PROCESS_IQ))
              {
                g_log << "--- Packet seqno: " <<
		  g_nextPacketSeqno -1 << " set curTime as: " 
                  << g_currentTimeInfo.drsn << std::endl;
              }
	      if(g_startingDRSNNeeded)
	      {
		setDRSNForAllLCQ(g_currentTimeInfo);
		g_startingDRSNNeeded = false;
	      }
	  }


          // Test to make sure DRSNs are contiguous. If not,
          // empty the packet compression queues, and start looking
          // for a starting number. 
          if(!g_startingDRSNNeeded)
          {
            qma_uint32  dsr = dt.getDataRecordSequenceNumber();
            if(g_currentTimeInfo.drsn != dsr)
            {
              g_log
                << "xxx g_currentTimeInfo.drsn does not match blockette.drsn."
                << std::endl;
              g_log << "xxx CurrentTime in dc_comp: "
                << g_currentTimeInfo.drsn <<
                " blockette drsn: " << dsr << std::endl;
              g_log << 
                "xxx Emptying Compression Queues, and resetting next drsn" 
               << std::endl;
              g_log << "xxx Mountainair Exitting on unexpected drsn." 
                << std::endl;
              CleanQMA(12); // This is an exit call
	    }
          }


	  if(!g_startingDRSNNeeded)
	  {
	    walkBlockettes(dt);
	  }

	} // End if is Data Packet.
      }// End if second check on is packet in sliding window.
    } // End of test for "is full" test on packet.
    else
    {
      //
      // We found that the next packet in InputQueue is not "full"
      // so stop processing packets.
      //
      packetsToProcess = false;
    }
  }
}

//
// This routine to walks through
// the blockettes in a data packet and queues the blockettes
// into the appropriate Logical Channel Queues.
//
bool walkBlockettes(dt_data& dt)
{
  int totallen = dt.getLengthInBytes();
  int curposition = 4;    // Start past Data Record Sequence Number
  
  qma_uint8 chan;         // Raw channel from blockette
  qma_uint8 val;          // Place to put result of anding this with flags
  qma_uint16 len;         // For variable length packet, copy len to here.
  int blockettes_in_packet = 0;

  while(curposition < totallen)
  {
    ++blockettes_in_packet;
    memcpy((char*)&chan,dt.getOffsetBitString(curposition),1);

    if(!true)
    {
      g_log << "<-- Blockette In Packet : " << blockettes_in_packet << 
      " Channel Byte : " << (qma_uint16) chan << std::endl;
    }

    val = (chan & DATA_BLOCKETTE_FLAG);
    if(val == 0)
    {
      //
      // This is a  Status blockette
      //
      val = (chan & DCM_ST); // mask out channel stuff

      if(g_verbosity.show(D_EVERYTHING,D_STATUS_PACKETS))
      {
        g_log << "--- Found Status Blockette of type: " 
	<< (qma_uint16) val << std::endl;
      }

      switch (val)
      {
        case DC_ST38:
	{
          qma_uint32  dsr = dt.getDataRecordSequenceNumber();
          if(g_currentTimeInfo.drsn != dsr)
	  {
            report_sequence_error("DRSN",g_currentTimeInfo.drsn,dsr);
	  }
          else
          {
          }
	  curposition = curposition + 4;
	  break;
	}
        case DC_ST816:
	{
	  curposition = curposition + 4;
	  break;
	}
        case DC_ST32:
	{
	  curposition = curposition + 8;
	  break;
	}
        case DC_ST232:
	{
	  curposition = curposition + 12;
	  break;
	}
        default:
	{
	  //
	  // If we get an unknown blockette type, we are in trouble because
	  // we don't know how far ahead to go to get the next blockkete.
	  // Write an error and return false
	  //
	  g_log << "xxx Error. Unknown Status packet Type 1 : " << 
		    (qma_uint16) val << std::endl;
		
          empty_pcqs();
	  return false;
	}
      }
    }
    else
    {

      val = (chan & DCM); // mask out channel stuff leaving packet type in val
      switch (val)
      {
        case DC_MN38:
	{
	  processMN38(curposition,chan,dt,g_currentTimeInfo);
	  curposition = curposition + 4;
	  break;
	}
        case DC_MN816:
	{
	  processMN816(curposition,chan,dt,g_currentTimeInfo);
	  curposition = curposition + 4;
	  break;
	}
        case DC_MN32:
	{
	  curposition = curposition + 8;
	  break;
	}
        case DC_MN232:
	{
          qma_uint32  dsr = dt.getDataRecordSequenceNumber();
          if(g_currentTimeInfo.drsn != dsr)
	  {
	    report_sequence_error("MN232",g_currentTimeInfo.drsn,dsr);
            return false;
	  }
          else
          {
	    processMN232(curposition,chan,dt,g_currentTimeInfo);
          }
	  curposition = curposition + 12;
	  break;
	}
        case DC_AG38:
	{
          qma_uint32  dsr = dt.getDataRecordSequenceNumber();
          if(g_currentTimeInfo.drsn != dsr)
	  {
	    report_sequence_error("AG38",g_currentTimeInfo.drsn,dsr);
            return false;
	  }
          else
          {
	    processAG38(curposition,chan,dt,g_currentTimeInfo);
          }
          curposition = curposition + 4;
	  break;
	}
        case DC_AG816:
	{
          qma_uint32  dsr = dt.getDataRecordSequenceNumber();
          if(g_currentTimeInfo.drsn != dsr)
	  {
	    report_sequence_error("AG816",g_currentTimeInfo.drsn,dsr);
            return false;
	  }
          else
          {
	    processAG816(curposition,chan,dt,g_currentTimeInfo);
          }
	  curposition = curposition + 4;
	  break;
	}
        case DC_AG32:
	{
	  curposition = curposition + 8;
	  break;
	}
        case DC_AG232:
	{
	  curposition = curposition + 12;
	  break;
	}
        case DC_CNP38:
	{
	  curposition = curposition + 4;
	  break;
	}
        case DC_CNP816:
	{
	  curposition = curposition + 4;
	  break;
	}
        case DC_CNP316:
	{
	  curposition = curposition + 8;
	  break;
	}
        case DC_CNP232:
	{
	  curposition = curposition + 12;
	  break;
	}
        case DC_D32:
	{
          // This is a fixed length blockette
       
          qma_uint32  dsr = dt.getDataRecordSequenceNumber();
          if(g_currentTimeInfo.drsn != dsr)
	  {
	    report_sequence_error("DCD32",g_currentTimeInfo.drsn,dsr);
            return false;
	  }
          else
          {
	    processDCD32(curposition,chan,dt,g_currentTimeInfo);
          }
	  curposition = curposition + 8;
	  break;
	}
        case DC_COMP:
	{
          // 
          // This is a variable length blockette. ProcessDCComp()
          // returns the appropriate length.  
          // 
          qma_uint32  dsr = dt.getDataRecordSequenceNumber();
          if(g_currentTimeInfo.drsn != dsr)
	  {
            report_sequence_error("DCComp",g_currentTimeInfo.drsn,dsr);
            return false;
	  }
          else
          {
            len = processDCComp(curposition,chan,dt,g_currentTimeInfo);
          }
	  curposition = curposition + len;
	  break;
	}
        case DC_MULT:
	{
	  //
	  // This is a variable length blockette. ProcessDCMult()
          // returns the appropriate length.
          //
          qma_uint32  dsr = dt.getDataRecordSequenceNumber();
          if(g_currentTimeInfo.drsn != dsr)
	  {
	    report_sequence_error("Mult",g_currentTimeInfo.drsn,dsr);
            return false;
	  }
          else
          {
	    len = processDCMult(curposition,chan,dt,g_currentTimeInfo);
          }
	  curposition = curposition + len;
	  break;
	}
        case DC_SPEC:
	{
	  // 
	  // variable length
	  //
	  qma_uint8 mytype;
	  mytype = (val & (!DCM));
	  switch (mytype)
	  {
	    case 0:
	    {
	      len = 28;
	      break;
	    }
	    case 1:
	    {
	      len = 8;
	      break;
	    }
	    case 2:
	    {
	      len = 12;
	      break;
	    }
	    case 3:
	    {
	      len = 8;
	      break;
	    }
	    default:
	    {
	      //
	      // If we get an unknown blockette type, we are in trouble because
	      // we don't know how far ahead to go to get the next blockkete.
	      // Write an error and return false
	      //
	       g_log << "xxx Error. Unknown Status packet Type 2 : " << 
		    (qma_uint16) val << std::endl;
              empty_pcqs();
	      return false;
	      break;
	    }
	  }
	  curposition = curposition + len;
	  break;
	}
        default:
	{
	  //
	  // If we get an unknown blockette type, we are in trouble because
	  // we don't know how far ahead to go to get the next blockkete.
	  // Write an error and return false
	  //
	  g_log << "xxx Error. Unknown Status packet Type 3 : " << 
		(qma_uint16) val << std::endl;
          empty_pcqs();
	  return false;
	}
      }
    }
    //
    // Completed both Status and Data processing for this blockette
    //
  } // While curposition < totallen

  if(!true)
  {
    g_log  << "--- Total blockettes in Packet : " << blockettes_in_packet 
	<< std::endl << std::endl;
  }
  return true;
}

void setDRSNForAllLCQ(const BTI& timeinfo)
{
  int i;

  for(i =0;i<g_number_of_diglcqs;i++)
  {
    g_digLCQ_list[i].setStartTime(timeinfo);
  }

  for(i=0;i<g_number_of_mainlcqs;i++)
  {
    g_mainLCQ_list[i].setStartTime(timeinfo);
  }

  if(true)
  {
    g_log << "--- Set all LCQs to start with DRSN: " <<
	timeinfo.drsn << std::endl;
  }
}

void sendAcksIfNeeded()
{
  if(g_ackCounter.ackNow())
  {
    if(g_verbosity.show(D_MINOR,D_ACK))
    { 
      g_log << "--- Time to Ack with Number Unack'd of : " << 
		g_ackCounter.getNumberInAckList() << std::endl;
    }

    // Acknowledgement packets

    dt_dack dtack;
    dtack.setThrottleValue(0);
    dtack.setAckBitmap(g_ackCounter.getAckList(),
		       g_ackCounter.getLastAckedSeqno());
    
    //
    // Declare the outgoing ACK packet
    //
    Packet    out_p;
    QDPHeader out_qdp;

    out_qdp.setCommand(DT_DACK_VAL);
    out_qdp.setVersion(QDP_VERSION);
    out_qdp.setDataLengthInBytes(dtack.getLengthInBytes());
    out_qdp.setPacketSequence(g_dataPacketSeq.next());
    out_qdp.setAckNumber(g_ackCounter.getLastAckedSeqno());
    out_p.setDataBitString(dtack.getBitString(),
				   dtack.getLengthInBytes());
    out_p.setQDPHeaderBitString(out_qdp.getBitString());
    out_p.setCRC();
	  
    if(g_verbosity.show(D_EVERYTHING,D_ACK_PACKET))
    {
      out_p.printPacketContents();
    }

    int send_ok = g_dataPort.write((char*)
			      out_p.getBitString(),
			      out_p.getLengthInBytes());
    if(!send_ok) 
    {
      g_log << "xxx Error Number on send in Ack Data Packet:" 
		<< (qma_uint16) errno << std::endl;
    }
    else
    {
      g_ackCounter.resetAckCounter();
    }
  } // End if AckNow()
  return;
}

void report_sequence_error(char* astring,qma_uint32 wasit, qma_uint32 oldit)
{
  g_log << "xxx TimeStampError " << astring << " - Was: " << wasit 
    << " expected: " << oldit << std::endl;
  empty_pcqs();
  g_startingDRSNNeeded = true;
  return;
}
