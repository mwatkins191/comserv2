/*
 *
 * File     :
 *   Packet.h
 *
 * Purpose  :
 *   A Packet is a message containing a header and data.
 *
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   14 February 2002
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
#ifndef PACKET_H
#define PACKET_H

#include "QmaTypes.h"
#include "QmaLimits.h"
#include "PacketElement.h"
#include "CRC.h"

class Packet
{
  public:

    Packet();
    ~Packet() {} ;

    //
    // Packet Get/Set Routines
    //
    unsigned char* getBitString() const;
    void           setBitString(unsigned char*,const int);
    int            getLengthInBytes() const;



    //
    // Packet Specific Routines
    //
    void setCRC();
    bool checkCRC();

    //
    // Packet element Get/Set routines
    //
    unsigned char* getQDPHeaderBitString() const;
    void           setQDPHeaderBitString(unsigned char* header);
    int            getQDPHeaderLengthInBytes() const;


    unsigned char* getDataBitString() const;
    void           setDataBitString(unsigned char* data,
                           const int data_length_in_bytes);
    int            getDataLengthInBytes() const;

    void	   printPacketContents();  

  private:
    CRC		     p_crc; // Put this here so constructor is only
			    // called once.
    qma_uint32       p_packet_length_in_bytes;
    qma_uint8        p_bits[MAX_BYTES_IN_PACKET];
};

#endif
