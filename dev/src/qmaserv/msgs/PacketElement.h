/*
 *
 * File     :
 *   PacketElement.h
 *
 * Purpose  :
 *   A packet element is a header, or a data portion of a packet.
 *   All packet elements contains field lists.
 *
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   28 February 2002
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

#ifndef PACKET_ELEMENT_H
#define PACKET_ELEMENT_H

#include "Field.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include <vector>

class PacketElement
{
  public:

    PacketElement();
    ~PacketElement() {};

    //
    // Bit String Transfer routines
    //

    unsigned char* getBitString() const;
    void           setBitString(unsigned char* buf);
    int            getLengthInBytes() const;

    qma_uint32	       p_number_of_fields;	
    qma_uint32 	       p_length_in_bytes;
    qma_uint8          p_bits[MAX_BYTES_IN_PACKET];
#ifdef QMA_LITTLE_ENDIAN
    qma_uint8          p_bits_swapped[MAX_BYTES_IN_PACKET];
#endif
	
	Field *p_fieldListPtr;
};
#endif
