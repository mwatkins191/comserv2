/*
 *
 * File     :
 *   dt_fill.h
 *
 * Purpose  :
 *   This is a data fill packet.
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

#ifndef DT_FILL_H
#define DT_FILL_H

#include "Field.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "PacketElement.h"

class dt_fill : public PacketElement
{
  public:

    dt_fill();
    ~dt_fill() {};

    //
    // This message has variable length fields.
    // Add special set and get methods to deal with
    // length of message info as received.
    //
    void setBitString(const unsigned char* buf,qma_uint32 len);
    unsigned char* getOffsetBitString(const qma_uint16 start);


    //
    // Operations on fields
    //
    
    qma_uint32 getFillRecordSequenceNumber();

    Field p_fieldList[FIELDS_IN_DT_FILL];

};

#endif
