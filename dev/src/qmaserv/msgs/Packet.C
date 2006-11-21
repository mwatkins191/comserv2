/*
 *
 * File     :
 *   Packet.C
 *
 * Purpose  :
 *   A packet is a bitstring that contains a QDP header and data.
 *   The data can be either a msg, or a cmd. The bit string can
 *   be used to construct or dissabmle packet bit string.s
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
#include <iostream>
#include <iomanip>
#include "Field.h"
#include "Packet.h"
#include "CRC.h"

//
// constructors
//
Packet::Packet()
{
  p_packet_length_in_bytes = 0;
}


//
// Packet get/set methods
//

unsigned char* Packet::getBitString() const
{
  return (unsigned char*)&p_bits[0];
}

void Packet::setBitString(unsigned char* p,const int len)
{
  memcpy(p_bits,p,len);
  p_packet_length_in_bytes = len;
  //p_packet_length_in_bytes = len + BYTES_IN_QDP_HEADER;
}

int Packet::getLengthInBytes() const
{
  return p_packet_length_in_bytes;
}

//
// Special Packet routines
//

void Packet::setCRC()
{
  long crc  = p_crc.gcrccalc((char *)p_bits+4,p_packet_length_in_bytes-4);
  memcpy(p_bits,(char*)&crc,BYTES_IN_CRC);
}

bool Packet::checkCRC()
{
  qma_uint32 rxd_crc;
  memcpy((char*)&rxd_crc,(char*)&p_bits[0],BYTES_IN_CRC);
  qma_uint32 expected_crc  = (qma_uint32)
     p_crc.gcrccalc((char *)p_bits+4,p_packet_length_in_bytes-4);

  bool retVal = false;
  if(rxd_crc == expected_crc)
  {
    retVal = true;
  }
  return retVal;
}


//
// QDPHeader get/set routines
//


unsigned char* Packet::getQDPHeaderBitString() const
{
  return (unsigned char *)&p_bits[0];
}

void Packet::setQDPHeaderBitString(unsigned char* buf)
{
  
  memcpy(p_bits,buf,BYTES_IN_QDP_HEADER);
  if (p_packet_length_in_bytes == 0)
  {
    p_packet_length_in_bytes =  BYTES_IN_QDP_HEADER;
  }
}

int Packet::getQDPHeaderLengthInBytes() const
{
  return BYTES_IN_QDP_HEADER;
}


//
// Data get/set routines
//

unsigned char* Packet::getDataBitString() const
{
  return (unsigned char*)p_bits + BYTES_IN_QDP_HEADER;
}


void Packet::setDataBitString(unsigned char* buf,const int len_in_bytes)
{
  memcpy((unsigned char*)p_bits+12,buf,len_in_bytes);
  p_packet_length_in_bytes = len_in_bytes + BYTES_IN_QDP_HEADER;
}


int Packet::getDataLengthInBytes() const
{
  return (p_packet_length_in_bytes - BYTES_IN_QDP_HEADER);
}


void Packet::printPacketContents()
{
  std::cout << "--- Packet Contents " << std::endl;
  for(int x = 0;x<p_packet_length_in_bytes;(x=x+4))
  {
    qma_uint16 b1;
    qma_uint16 b2;

    memcpy(&b1,&p_bits[x],2);
    memcpy(&b2,&p_bits[x+2],2);
    std::cout << "--- " << std::hex << std::setw(4) << std::setiosflags(std::ios::right) << b1 << " "
	      <<  std::hex << std::setw(4) << std::setiosflags(std::ios::right) << b2 << std::endl;
  }
}
