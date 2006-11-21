/*
 *
 * File     :
 *   c1_log.h
 *
 * Purpose  :
 *   This is a logical response message.
 *
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   4 March 2002
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

#ifndef C1_LOG_H
#define C1_LOG_H

#include "Field.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "PacketElement.h"

class c1_log : public PacketElement
{
  public:

    c1_log();
    ~c1_log() {};

    //
    // Operations on fields
    //
    
    qma_uint16 getDataPortNumber();
    qma_uint16 getFlags();
    qma_uint16 getPacketBufferSize();
    qma_uint16 getMTU();
    qma_uint16 getGroupCount();
    qma_uint16 getMaxResendTimeout();
    qma_uint16 getGroupTimeout();
    qma_uint16 getMinResendTimeout();
    qma_uint16 getWindowSize();
    qma_uint16 getPacketSequenceNumber();
    qma_uint16 getChannel1Freqs();
    qma_uint16 getChannel2Freqs();
    qma_uint16 getChannel3Freqs();
    qma_uint16 getChannel4Freqs();
    qma_uint16 getChannel5Freqs();
    qma_uint16 getChannel6Freqs();
    qma_uint16 getAckCount();
    qma_uint16 getAckTimeout();
    qma_uint16 getOldDataThresholdTime();
    qma_uint16 getEthernetThrottle();
    qma_uint16 getAlarmPercent();
    qma_uint16 getAutoFilters();
    qma_uint16 getManualFilters();

    Field p_fieldList[FIELDS_IN_C1_LOG];

};

#endif
