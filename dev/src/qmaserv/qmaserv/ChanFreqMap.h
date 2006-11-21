/*
 * File     :
 *   ChanFreqMap.h
 *
 * Purpose  :
 *   In order to maintain a sparse array of LCQVO's, LCQs, and CompressionQs,
 *   we use a channel,freq to array index mapper.
 *   Based strictly on the order in request, a channelfreq pair is converted
 *   to a index. When the channel,freq are new, they get assigned a new
 *   index. When they are established, they returned the assigned index.
 * 
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *  20 May 2002
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
#ifndef CHANFREQMAP_H
#define CHANFREQMAP_H

#include "QmaTypes.h"
#include "QmaLimits.h"

class ChanFreqMap
{

  public:
    ChanFreqMap();
    ~ChanFreqMap();

    //
    // All the routines input qma_uint8, qmauint8 because they will
    // use values from the Tokens, and Blockettes which are in qma_uint8 form.
    //

    bool validChanFreqValues(qma_uint8 chan, qma_uint8 freq);
    bool ChanFreqInQueue(qma_uint8 chan, qma_uint8 freq);
    int  assignQueuePosition(qma_uint8 chan, qma_uint8 freq);
    int  retrieveQueuePosition(qma_uint8 chan, qma_uint8 freq);

    void reset(); 
  private:

   qma_uint8 p_position_array[MAX_CHANNELS][MAX_FREQS];
   qma_uint8 mapChanFreqToID(qma_uint8 chan,qma_uint8 freq);
   int       p_current_index;
   qma_uint8 *p_map;
};
#endif
