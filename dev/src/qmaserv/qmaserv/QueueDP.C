/*
 * Program: Mountainair
 *
 * File:
 *  QueueDP.C
 *
 * Purpose:
 *  These queue the incoming data packets.
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
#include "QmaDiag.h"
#include "Verbose.h"
#include "InputPacket.h"
#include "Packet.h"
#include "msgs.h"
#include "global.h"
#include "qmaswap.h"
extern Verbose g_verbosity;


bool queuePacket(const Packet& in_p)
{
  //
  // This routine has a binary return. Continue(true) or exit(false)
  //

  bool retValue = false;
  QDPHeader in_qdp;
  in_qdp.setBitString(in_p.getQDPHeaderBitString());
  qma_uint16 cur_rxseq = in_qdp.getPacketSequence();

  if(g_verbosity.show(D_EVERYTHING,D_SEQNO))
  {
     g_log << "--- QueueDP processing seqno: " << cur_rxseq << std::endl;
  }
  if(g_ackCounter.inCurrentWindow(cur_rxseq))
  {
    qma_uint16 queuePosition = cur_rxseq & 0x007F;
    int winSize = g_ackCounter.getSlidingWindowSize();
    if ( (cur_rxseq >=  (g_nextPacketSeqno +winSize)) ||
         (cur_rxseq <= (g_nextPacketSeqno - winSize)) )
    {
      if(g_verbosity.show(D_EVERYTHING,D_INPUT_QUEUE))
       {
         g_log << "xxx Attempted to queue packet past end of Input Queue:"
         << cur_rxseq << " g_nextSeqno:" << g_nextPacketSeqno << std::endl;
       }
       return false;
    }
    else
    {
      if (g_inputQueue[queuePosition].full == false)
      {
        g_inputQueue[queuePosition].packet = in_p;
        g_inputQueue[queuePosition].full = true;

        //
        // For all data packets, processed or not, set
        // and ack flag.
        //
        if(!g_ackCounter.addRxSeqno(cur_rxseq))
        {
          if(g_verbosity.show(D_MINOR,D_RX_SEQUENCE))
          {
            g_log << "xxx Tried to set Ack for a packet: " << cur_rxseq <<
            "  seqno out of sliding window." << std::endl;
          }
        }
        retValue = true;
      }
      else
      {
        if(!true) 
        {
          g_log << "xxx Queue a packet into a full queue slot: " 
           << cur_rxseq << std::endl;
        }
        retValue = false;
      }
    }

    if(g_verbosity.show(D_MINOR,D_RX_SEQUENCE))
    {
        g_log << "<-- Rxed seqno : " << cur_rxseq << std::endl;
        g_log << "--- Queue Position of: " << queuePosition << std::endl;
    }
  }
  return retValue;
}
