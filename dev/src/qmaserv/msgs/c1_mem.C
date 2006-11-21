/*
 *
 * File     :
 *   c1_mem.C
 *
 * Purpose  :
 *   This message is used to interpret memory segments (tokens) from the
 *    Q330. A series of thes will be send, requesting a series of 
 *    token segments, until all segments are read.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   20 April 2002
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
#include <iostream>
#include <string.h>
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "Field.h"
#include "c1_mem.h"

c1_mem::c1_mem()
{
  p_fieldListPtr = &p_fieldList[0];
  p_number_of_fields =0;
  Field f1;
  f1.p_start_position = 0;
  f1.p_number_bytes = 8;
  f1.p_data_type = UnsignedInt64;
  strcpy(f1.p_name,"Memory Request");
  p_fieldList[p_number_of_fields] = f1;
  ++p_number_of_fields;


  Field f2;
  f2.p_start_position = 
	f1.p_start_position + f1.p_number_bytes;
  f2.p_number_bytes = 2;
  f2.p_data_type = UnsignedInt16;
  strcpy(f2.p_name,"Segment Number");
  p_fieldList[p_number_of_fields] = f2;
  ++p_number_of_fields;


  Field f3;
  f3.p_start_position = 
	f2.p_start_position + f2.p_number_bytes;
  f3.p_number_bytes = 2;
  f3.p_data_type = UnsignedInt16;
  strcpy(f3.p_name,"Total Segements");
  p_fieldList[p_number_of_fields] = f3;
  ++p_number_of_fields;

  Field f4;
  f4.p_start_position = 
	f3.p_start_position + f3.p_number_bytes;
  f4.p_number_bytes = C1_MAXSEG;
  f4.p_data_type = SegmentBuffer;
  strcpy(f4.p_name,"Segement Buffer");
  p_fieldList[p_number_of_fields] = f4;

  p_length_in_bytes =
    p_fieldList[p_number_of_fields].p_start_position + 
    p_fieldList[p_number_of_fields].p_number_bytes;


  if(!true)
  {
    std::cout << "Creating c1_mem with length : " << 
       p_length_in_bytes << std::endl;
  }

  p_number_of_fields++;
  memset((char*)&p_bits[0],0,p_length_in_bytes);
}

void c1_mem::setBitString(const unsigned char* buf, qma_uint32 len)
{
  //memcpy(p_bits,buf,len);
  p_length_in_bytes = len;
  PacketElement::setBitString((unsigned char *)buf);
}


//
// Operations on fields
//

void c1_mem::getRequestHeader(qma_char* reqHdr) const
{
  memcpy((char*)reqHdr,
         (char*)&p_bits[p_fieldList[0].p_start_position],
         (unsigned int)p_fieldList[0].p_number_bytes);
  return;
}


void  c1_mem::setRequestHeader(qma_char* val)
{

  memcpy((char*)&p_bits[p_fieldList[0].p_start_position],
         (char*)&val,
         p_fieldList[0].p_number_bytes);
}

//
//
//

qma_uint16 c1_mem::getSegmentNumber() const
{
  qma_uint16 val;
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[1].p_start_position],
         (unsigned int)p_fieldList[1].p_number_bytes);
  return val;
}

void  c1_mem::setSegmentNumber(qma_uint16 val)
{

  memcpy((char*)&p_bits[p_fieldList[1].p_start_position],
         (char*)&val,
         p_fieldList[1].p_number_bytes);
}

//
//
//

qma_uint16  c1_mem::getTotalSegments() const
{
  qma_uint16 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[2].p_start_position],
         (unsigned int)p_fieldList[2].p_number_bytes);
  return val;
}


void  c1_mem::setTotalSegments(qma_uint16 val)
{
  memcpy((char*)&p_bits[p_fieldList[2].p_start_position],
         (char*)&val,
         p_fieldList[2].p_number_bytes);
}

//
// This must be called with 438 bytes of storage available
//

void c1_mem::getSegmentBuffer(qma_char* buf438bytes) const
{
  memcpy((char*)buf438bytes,
         (char*)&p_bits[p_fieldList[3].p_start_position],
         (unsigned int)p_fieldList[3].p_number_bytes);
}


void  c1_mem::setSegmentBuffer(qma_char* buf438bytes)
{
  memcpy((char*)&p_bits[p_fieldList[3].p_start_position],
         (char*)buf438bytes,
	 p_fieldList[3].p_number_bytes);
}
