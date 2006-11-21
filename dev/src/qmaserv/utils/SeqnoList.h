/*
 * File     :
 *  SeqnoList.h
 *
 * Purpose  :
 *  This implements a sequence number routine to eliminate the use of
 *  stdlist in the processing. This is set to fixed max size of 128.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  6 July 2002
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
#ifndef SEQNOLIST_H
#define SEQNOLIST_H

#include "QmaTypes.h"
#include "QmaLimits.h"

class SeqnoList
{

  public:
    SeqnoList();
    ~SeqnoList();

    qma_uint32 numberInList();
    qma_uint32 getAndRemoveSeqno();
    void       addSeqnoToList(qma_uint32 seqno);  
    void       clearList();

    //
    // The size of this array is set by the maximum number of packets
    // that can be acked at once. This is set in the ack packet as
    // last acked plus 127.

    qma_uint16 p_seqno_list[MAX_SLIDING_WINDOW_SIZE];
    void       sortList();

  private:

    qma_uint16 p_number_in_list;
};

#endif
