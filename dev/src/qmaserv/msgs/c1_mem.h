/*
 *
 * File     :
 *   c1_mem.h
 *
 * Purpose  :
 *   This is a Memory (Tokens) message.
 *   Currently this is to interpret the response to a logical
 *    token request.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   20 April 2002
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

#ifndef C1_MEM_H
#define C1_MEM_H

#include "Field.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "PacketElement.h"


class c1_mem : public PacketElement
{
  public:

    c1_mem();
    ~c1_mem() {};

    //
    // This message has variable length fields.
    // Add special set and get methods to deal with
    // length of message info as received.
    //
    void setBitString(const unsigned char* buf,qma_uint32 len);

    //
    // Operations on fields
    //

    void       getRequestHeader(qma_char* reqHdr) const;
    void       setRequestHeader(qma_char* reqHdr);

    qma_uint16 getSegmentNumber() const;
    void       setSegmentNumber(qma_uint16 addr);

    qma_uint16 getTotalSegments() const;
    void       setTotalSegments(qma_uint16 totalSegments);
    
    void       getSegmentBuffer(qma_char* buf) const;
    void       setSegmentBuffer(qma_char* buf);

   Field p_fieldList[FIELDS_IN_C1_MEM];
};

#endif
