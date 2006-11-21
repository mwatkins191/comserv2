/*
 *
 * File     :
 *  AckCounter.h
 *
 * Purpose  :
 *  This class counts up the recieve packets, and indicates
 *   when it is time to send an Ack.
 *
 *  This routines assumes that once you call getAckList, the Acks are
 *    sent. It removes the pending acks from the queue.
 *    If the ack send fails, the system must re-call - get flags,
 *     get a new PacketSeqno, and start the ack processing again.
 *
 * Author   :
 *  Phil Maechling
 *
 *
 * Mod Date :
 *  9 March 2002
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

#ifndef ACKCOUNTER_H
#define ACKCOUNTER_H

#include "QmaTypes.h"
#include "SlidingWindow.h"
#include "TimeOfDay.h"
#include "SeqnoList.h"

class AckCounter
{
  public:

    AckCounter();
    ~AckCounter () {};

    //
    // This number, the last unacked seqno is what we get from
    // the Q330 in the flags packet.
    //
    qma_uint16 getLastUnAckedSeqno();

    double     getAckTimeout();
    void       setAckTimeout(qma_uint16 at);

    qma_uint16 getSlidingWindowSize();

    qma_uint16 getAckGroupSize();
    void       setAckGroupSize(qma_uint16 group);

    //
    // Use this initialize function to set the window size, 
    // and the lastUnacked Packet
    // which we got from the c1_flags packet.
    //

    void       initializeCounter(qma_uint16 windowSize,
			         qma_uint16 lastUnAckedPacket);

    qma_uint16 getLastAckedSeqno();
    bool       addRxSeqno(qma_uint16 seqno);
    bool       ackNow();            // does group count and timeout checks
    void       resetAckCounter();
    qma_uint16 getNumberInAckList();
    SeqnoList  getAckList();

    bool                  inCurrentWindow(qma_uint16 seqno);

  private: 

   bool                  seqnoInList(qma_uint16 seqno);
   qma_uint32		 p_ack_group_size;
   double  	 	 p_ack_timeout_tenthsofseconds;
   double                p_time_of_last_ack;

   SeqnoList             p_rx_seqno_list;
   SlidingWindow         p_sliding_window;
   TimeOfDay		 p_clock;
};
#endif
