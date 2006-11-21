/*
 *
 * File     :
 *   SlidingWindow.C
 *
 * Purpose  :
 *
 *   This implements a sliding window class which contains a list
 *    of packets in the current window, along with a marker indicating
 *    whether the packet has been acked. It also has functionality that
 *    checks to see that a previously acked packet will not be sent further
 *    down the processing queue.
 *
 * Author   :
 *  Phil Maechling
 *
 *
 * Mod Date :
 *   12 March 2002
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
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "QmaDiag.h"
#include "Verbose.h"
#include "PacketMarker.h"
#include "SlidingWindow.h"
#include "StateMachine.h"
#include "global.h"
extern StateMachine g_stateMachine;
extern Verbose      g_verbosity;

//
// User must call initialize before the sliding window will work.
//
void SlidingWindow::initializeWindow(qma_uint16 windowSize,
				     qma_uint16 lastUnAckedPacket)
{
  if(g_verbosity.show(D_MINOR,D_SLIDING_WINDOW))
  {
    g_log << "--- Initializing Sliding Window Size : " << windowSize 
	<< std::endl;
    g_log << "--- Last UnAcked Packet : " <<
	lastUnAckedPacket << std::endl;
  }
  


  p_window_size = windowSize;
  p_position_of_last_unacked = 0;
  qma_uint16 startval = lastUnAckedPacket;
  PacketMarker ptemp;

  for(int i=0;i<p_window_size;i++)
  {
    ptemp.setSeqno(startval);
    ptemp.setWasAcked(false);
    p_seqno_list[i] = ptemp;
    ++startval;
    startval = startval % PACKET_MODULUS;
  }


  if(g_verbosity.show(D_MINOR,D_SLIDING_WINDOW))
  {
    g_log << "--- Sliding Window Init Complete Window Size : " 
	<< p_window_size << std::endl;
    g_log << "--- Last Unacked Seqno : " <<
	p_seqno_list[p_position_of_last_unacked].getSeqno() << std::endl;
    g_log <<  "--- First Seqno in Window : " << 
	p_seqno_list[firstPosition()].getSeqno() << std::endl;
    g_log << "--- Position of Last Unacked Packet : " << 
	p_position_of_last_unacked << std::endl;
    g_log << "--- Position of First Unacked Packet : " << 
	firstPosition()  << std::endl;
  }
}
  

bool SlidingWindow::seqnoInWindow(qma_uint16 trySeqno)
{
  qma_uint16 lastSeqno = p_seqno_list[p_position_of_last_unacked].getSeqno();
  qma_uint16 firstSeqno = p_seqno_list[firstPosition()].getSeqno();
  //
  // These conditionals handle the wraping of the seqno
  // 
  if(lastSeqno < firstSeqno) 
  {
    if((trySeqno >= lastSeqno) && (trySeqno <= firstSeqno)) 
    {
      return true;
    }
    else 
    {
      if(g_verbosity.show(D_EVERYTHING,D_SLIDING_WINDOW_ERRORS))
      {
        g_log << "xxx Seqno Outside Sliding window : Last seqno : "
        << lastSeqno << " First Seqno " << firstSeqno <<
        " Rx'd Seqno " << trySeqno << std::endl;
      }
      return false;
    }
  }
  else if (lastSeqno > firstSeqno) 
  {
    if((trySeqno >= lastSeqno) && (trySeqno < PACKET_MODULUS))
    {
      return true;
    }
    else if(trySeqno <= firstSeqno)
    {
      return true;
    }
    else 
    {
      if(g_verbosity.show(D_EVERYTHING,D_SLIDING_WINDOW_ERRORS))
      {
        g_log << "xxx Seqno Outside Sliding window : Last Seqno " <<
          lastSeqno << " First Seqno " << firstSeqno << "  Rx'd Seqno " 
	  << trySeqno << std::endl;
      }
      return false;
    }
  }
  else 
  {
    g_log << "xxx Unknown Sliding window state : last - first - trySeqno " 
      << lastSeqno << " " << firstSeqno << " " << trySeqno << std::endl;
    return false;
  }
}

bool SlidingWindow::markAsAcked(qma_uint16 ackedSeqno)
{
  if(seqnoInWindow(ackedSeqno)) 
  {
    bool found = false;
    for(int i = 0;i<p_window_size;i++)
    {
      if(p_seqno_list[i].getSeqno() == ackedSeqno) 
      {
          //
          // If it was acked. return false and don't pass it on
          //
          if(p_seqno_list[i].wasAcked())
          {
             return false;
          }
          else
          {
	    p_seqno_list[i].setWasAcked(true);
	    found = true;
	    break;
          }
      }
    }
    if (!found) 
    {
      g_log << "xxx - In Window True, but seqno not found" << std::endl;
      return false;
    }
    else 
    {
      return true;
    }
  } 
  else 
  {
    return false;
  }
}

  
qma_uint16 SlidingWindow::getLastUnAckedSeqno()
{
  return p_seqno_list[p_position_of_last_unacked].getSeqno();
}


qma_uint16 SlidingWindow::getWindowSize()
{
  return p_window_size;
}


void SlidingWindow::moveWindow()
{
  bool done = false;
  PacketMarker ptemp;
  qma_uint16 lastPos = p_position_of_last_unacked;
  while(!done) 
  {
    if(p_seqno_list[lastPos].wasAcked()) 
    { 
      //
      // order dependency here. Get the nextSeqno now, from the firstPacket
      // Then move the window creating a new FirstPacket.
      //
      ptemp.setSeqno(nextFirstSeqno());
      ptemp.setWasAcked(false);

      ++p_position_of_last_unacked;
      if(p_window_size < 1)
      {
        g_log << "xxx Restart Mountainair on call to move_window with p_window_size: " <<
         p_window_size << std::endl;
         g_stateMachine.setState(Exitting);
         return;
      }
      p_position_of_last_unacked = p_position_of_last_unacked % p_window_size;
      p_seqno_list[lastPos] = ptemp;

      if(g_verbosity.show(D_EVERYTHING,D_SLIDING_WINDOW_ERRORS))
      {
        g_log << "--- Window Position moved to : lastPosition " 
		<< p_position_of_last_unacked <<
		" last Seqno: " << 
		p_seqno_list[p_position_of_last_unacked].getSeqno() << 
		"  firstPosition : " << firstPosition() << " first seqno : " <<
	  p_seqno_list[firstPosition()].getSeqno() << "  nextFirstSeqno " <<
	  nextFirstSeqno() << std::endl;
      }
    }
    else // as soon as you find packet not acked, stop moving the window, and leave.
    {
	done = true;
	break;
    }
    lastPos = p_position_of_last_unacked;
  }
}

qma_uint16 SlidingWindow::nextFirstSeqno()
{
  qma_uint16 firstVal = p_seqno_list[firstPosition()].getSeqno();
  ++firstVal;
  firstVal = firstVal % PACKET_MODULUS;
  return firstVal;
}

//
// One less than window size to prevent wrapping.
//
qma_uint16 SlidingWindow::firstPosition()
{
  qma_uint16 retval = p_position_of_last_unacked + (p_window_size -1);

  if(p_window_size < 1)
  {
    g_log << 
      "xxx Restart Mountainair on call to firstPosition with p_window_size: " <<
         p_window_size << std::endl;
         g_stateMachine.setState(Exitting);
         return 0;
  }

  retval = retval % p_window_size;
  return retval;
}
