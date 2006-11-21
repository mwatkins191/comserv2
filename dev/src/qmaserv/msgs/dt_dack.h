/*
 *
 * File     :
 *   dt_dack.h
 *
 * Purpose  :
 *   This is a Data packet acknowledgment packet.
 *
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   5 March 2002
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

#ifndef DT_DACK_H
#define DT_DACK_H

#include "QmaTypes.h"
#include "QmaLimits.h"
#include "Field.h"
#include "PacketElement.h"
#include "SeqnoList.h"

class dt_dack : public PacketElement
{
  public:

    dt_dack();
    ~dt_dack() {};

    //
    // Operations on fields. Set only for this output message
    //
    qma_uint16 findPosition(qma_uint16 lastUnAckedSeqno,
			    qma_uint16 currentSeqno);
    void       setPosition(qma_uint16 pos);
    qma_uint16 getThrottleValue();
    void       setThrottleValue(qma_uint16 tv);
    void       setAckBitmap(SeqnoList seqnos,qma_uint16 lastAckedSeqno);

    Field p_fieldList[FIELDS_IN_DT_DACK];

};

#endif
