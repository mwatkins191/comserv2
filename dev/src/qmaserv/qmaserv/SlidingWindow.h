/*
 *
 * File     :
 *   SlidingWindow.h
 *
 * Purpose  :
 *
 *   This implements a sliding window class which contains a list
 *    of packets in the current window, along with a marker indicating
 *    whether the packet has been acked.
 *
 * Author   :
 *  Phil Maechling
 *
 *
 * Mod Date :
 *   12 March 2002
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
#ifndef SLIDINGWINDOW_H
#define SLIDINGWINDOW_H
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "PacketMarker.h"

class SlidingWindow
{
  public:
    SlidingWindow() {};
    ~SlidingWindow() {};

    //
    // We use lastUnAckedPacket because this is what the q330
    // sends us in the c1_log flags packet.
    //
    void       initializeWindow(qma_uint16 windowSize,
			        qma_uint16 lastUnAckedPacket);
  
    bool       seqnoInWindow(qma_uint16 val);
    bool       markAsAcked(qma_uint16 val);

    qma_uint16 getLastUnAckedSeqno();
    void       moveWindow();
    qma_uint16 getWindowSize();
	  
  private:

   qma_uint16              nextFirstSeqno();
   qma_uint16		   firstPosition();
   qma_uint16		   p_position_of_last_unacked;
   qma_uint16              p_window_size;
   PacketMarker		   p_seqno_list[MAX_SLIDING_WINDOW_SIZE];
};

#endif
