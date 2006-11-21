/*
 *
 * File     :
 *  AckCounter.C
 *
 * Purpose  :
 *  This class counts up the recieve packets, and indicates
 *   when it is time to send an Ack.
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

#include <iostream>
#include "AckCounter.h"
#include "QmaLimits.h"
#include "QmaDiag.h"
#include "TimeOfDay.h"
#include "SeqnoList.h"
#include "Verbose.h"
#include "global.h"

extern Verbose g_verbosity;

//
// Assume that the user must call initialize
// before using this Ackcounter. Null constructor
//
AckCounter::AckCounter()
{
  p_ack_timeout_tenthsofseconds = 50.0; // wait time to ack in seconds
  p_ack_group_size  = 1;
  p_rx_seqno_list.clearList();
}

void AckCounter::initializeCounter(qma_uint16 windowSize,
				   qma_uint16 lastUnAckedSeqno)
{
  p_time_of_last_ack = p_clock.getCurrentTime();
  p_sliding_window.initializeWindow(windowSize,
			            lastUnAckedSeqno);
  p_rx_seqno_list.clearList();
}

qma_uint16 AckCounter::getLastUnAckedSeqno()
{
  return p_sliding_window.getLastUnAckedSeqno();
}

qma_uint16 AckCounter::getLastAckedSeqno()
{
  qma_uint32 lastAckedSeqno;
  qma_uint32 temp = p_sliding_window.getLastUnAckedSeqno();
  if(temp == 0) 
  {
    lastAckedSeqno = PACKET_MODULUS - 1;
  }
  else {
    lastAckedSeqno = temp -1;
  }
  return lastAckedSeqno;
}

//
// Timeout get and set
//

void AckCounter::setAckTimeout(qma_uint16 tov)
{
  double tenths = (double) tov;
  p_ack_timeout_tenthsofseconds = tenths; 
}

double AckCounter::getAckTimeout()
{
  return p_ack_timeout_tenthsofseconds;
}

qma_uint16 AckCounter::getSlidingWindowSize()
{
  return p_sliding_window.getWindowSize();
}

// 
// Group size get and set
//

void AckCounter::setAckGroupSize(qma_uint16 num)
{
  p_ack_group_size = num;
}

qma_uint16 AckCounter::getAckGroupSize()
{
  return p_ack_group_size;
}

bool AckCounter::inCurrentWindow(qma_uint16 trySeqno)
{
  return p_sliding_window.seqnoInWindow(trySeqno);
}

//
// Add sequence numbers into list for future acks.
// returns false if packet is outside current sliding window,
// or if the packet is a duplicate of one we have processed.
//
//

bool AckCounter::addRxSeqno(qma_uint16 seqno)
{
  if(p_sliding_window.seqnoInWindow(seqno)) 
   {
     if(!seqnoInList(seqno)) // test so we don't add the same number twice
     {
       p_rx_seqno_list.addSeqnoToList(seqno);  
       bool res = p_sliding_window.markAsAcked(seqno);
       if(res == false)
       {
         //
         // markAsAcked is miss named. This is really set when a seqno
         // is put into a list ready to be acked. It not really sent yet.
         //
         // This needs to be re-examined. Should not find dups here.
         // They should have been weeded out in the test above.
         // It's possible that the seqnoInList() test fails for 
         // seqno's outside of window.
         // Still needs more consideration. Why does slding window
         // contain any packets marked as acked.
         //
         if(!true)
         {
           g_log << "--- Found dup in markAsAcked : " << seqno  
		<< std::endl;
         }
         return false;
       }
       return true;
     }
     else
     {
       if(!true)
       {
         g_log << "xxx Tried to process same packet twice."
           << " Dropping packet :" << 
		seqno << std::endl;
       }
       return false;
     }
  } 
  else 
  {
    if(!true) 
    {
      g_log << "Dropping packet because it's outside sliding window :" 
	<< std::endl; 
    }
    return false;
  }
}

//
//  This has the logic to determine if (a) enough new packets have
// been recieved to fill the ack group, or (b) enough time has passed
// to timeout and send and ack
//

bool AckCounter::ackNow()
{
  if (p_rx_seqno_list.numberInList() >= p_ack_group_size) 
  {
    if(g_verbosity.show(D_EVERYTHING,D_ACK_COUNTER))
    {
       g_log << "--- Ack now due to group size : " << 
	p_rx_seqno_list.numberInList() << std::endl;
    }
   return true;
  }

  //
  // Test for the AckTimeout. qmaserv sends ack on timeout regardless
  // of whether any messages have been received.
  //

  double cur_time = p_clock.getCurrentTime();
  double tdiff = p_clock.getTimeDifference(p_time_of_last_ack,cur_time);
  //
  // Compare tdiff to timeout in tenths of seconds
  //
  if((tdiff*10.0) > p_ack_timeout_tenthsofseconds)
  {
    if(g_verbosity.show(D_EVERYTHING,D_ACK_COUNTER))
    {
        g_log << "--- Ack now due to ack_timeout tdiff : " 
	<< tdiff << "  timeout: " << p_ack_timeout_tenthsofseconds
	  << " packet in queue: " << p_rx_seqno_list.numberInList() 
	    << std::endl;
      if(p_rx_seqno_list.numberInList() < 1)
      {
        g_log << "--- Sending Ack Position on Timeout."  << std::endl;
      }
    }
    return true;
  }
  return false;
}

//
//
//

qma_uint16 AckCounter::getNumberInAckList()
{
  qma_uint16 res = (qma_uint16) p_rx_seqno_list.numberInList();
  return res;
}

SeqnoList AckCounter::getAckList()
{
  p_rx_seqno_list.sortList(); // Sort before returning list
  return p_rx_seqno_list;
}

void AckCounter::resetAckCounter()
{ 
  p_time_of_last_ack = p_clock.getCurrentTime();
  p_sliding_window.moveWindow();
  p_rx_seqno_list.clearList();
  return;
}

bool AckCounter::seqnoInList(qma_uint16 seqno)
{
  bool result = false;
  for(int i=0;i < p_rx_seqno_list.numberInList();i++)
  {
    if(p_rx_seqno_list.p_seqno_list[i] == seqno)
    {
      result = true;
      break;
    }
  }
  return result;
}
