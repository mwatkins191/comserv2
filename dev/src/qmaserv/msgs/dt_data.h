/*
 *
 * File     :
 *   dt_data.h
 *
 * Purpose  :
 *   This is a data packet.
 *
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   4 March 2002
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

#ifndef DT_DATA
#define DT_DATA

#include "Field.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "PacketElement.h"
#include "BTI.h"

class dt_data : public PacketElement
{
  public:

    dt_data();
    ~dt_data() {};

    //
    // This message has variable length fields.
    // Add special set and get methods to deal with
    // length of message info as received.
    //
    void setBitString(const unsigned char* buf,qma_uint32 len);
    unsigned char* getOffsetBitString(const qma_uint16 start);

    bool packetIsStartOfSecond();
    BTI  getCurrentTimeInfo();

    //
    // Operations on fields
    //
    
    qma_uint32 getDataRecordSequenceNumber();

    Field p_fieldList[FIELDS_IN_DT_DATA];

};

#endif
