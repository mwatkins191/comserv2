/*
 *
 * File     :
 *   dt_dack.C
 *
 * Purpose  :
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
#include <iostream>
#include <string.h>
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "QmaDiag.h"
#include "dt_dack.h"
#include "md5.h"
#include "findMD5.h"
#include "Field.h"
#include "SeqnoList.h"

dt_dack::dt_dack()
{
  p_fieldListPtr = &p_fieldList[0];
  p_number_of_fields =0;
  Field f1;
  f1.p_start_position = 0;
  f1.p_number_bytes = 2;
  f1.p_data_type = UnsignedInt16;
  strcpy(f1.p_name,"ThrottleValue");
  p_fieldList[p_number_of_fields] = f1;
  ++p_number_of_fields;

  Field f2;
  f2.p_start_position = f1.p_start_position + f1.p_number_bytes;
  f2.p_number_bytes = 2;
  f2.p_data_type = UnsignedInt16;
  strcpy(f2.p_name,"Spare");
  p_fieldList[p_number_of_fields] = f2;
  ++p_number_of_fields;

  /*
   * For purposes of byteswapping, we need to treat the ack bitmap as
   * seperate 32 bit values, since the protocol specifies it as 4 32
   * bit values rather than a 128 bit
   */
  /*
  Field f3;
  f3.p_start_position = f2.p_start_position + f2.p_number_bytes;
  f3.p_number_bytes = 16;
  f3.p_data_type = UnsignedInt128;
  strcpy(f3.p_name,"AcknowledgementBitmap"); 
  p_fieldList[p_number_of_fields] = f3;
  ++p_number_of_fields; 
  */
  Field f3;
  f3.p_start_position = f2.p_start_position + f2.p_number_bytes;
  f3.p_number_bytes = 4;
  f3.p_data_type = UnsignedInt32;
  strcpy(f3.p_name,"AcknowledgementBitmap0"); 
  p_fieldList[p_number_of_fields] = f3;
  ++p_number_of_fields; 

  Field f4;
  f4.p_start_position = f3.p_start_position + f3.p_number_bytes;
  f4.p_number_bytes = 4;
  f4.p_data_type = UnsignedInt32;
  strcpy(f4.p_name,"AcknowledgementBitmap1"); 
  p_fieldList[p_number_of_fields] = f4;
  ++p_number_of_fields; 

  Field f5;
  f5.p_start_position = f4.p_start_position + f4.p_number_bytes;
  f5.p_number_bytes = 4;
  f5.p_data_type = UnsignedInt32;
  strcpy(f5.p_name,"AcknowledgementBitmap2"); 
  p_fieldList[p_number_of_fields] = f5;
  ++p_number_of_fields; 

  Field f6;
  f6.p_start_position = f5.p_start_position + f5.p_number_bytes;
  f6.p_number_bytes = 4;
  f6.p_data_type = UnsignedInt32;
  strcpy(f6.p_name,"AcknowledgementBitmap3"); 
  p_fieldList[p_number_of_fields] = f6;
  ++p_number_of_fields; 

  Field f7;
  f7.p_start_position = f6.p_start_position + f6.p_number_bytes;
  f7.p_number_bytes = 4;
  f7.p_data_type = UnsignedInt32;
  strcpy(f7.p_name,"Spare");
  p_fieldList[p_number_of_fields] = f7;


  if(!true)
  {
    std::cout << "Creating dt_dack with length : " << 
 	p_fieldList[p_number_of_fields].p_start_position  << " " <<
    	p_fieldList[p_number_of_fields].p_number_bytes << std::endl;	
  }

  p_length_in_bytes =
    p_fieldList[p_number_of_fields].p_start_position + 
    p_fieldList[p_number_of_fields].p_number_bytes;

  p_number_of_fields++;
  memset((char*)&p_bits[0],0,p_length_in_bytes);
}

//
// Operations on fields
//

qma_uint16  dt_dack::getThrottleValue()
{
  qma_uint16 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[0].p_start_position],
         (unsigned int)p_fieldList[0].p_number_bytes);
  return val;
}


void  dt_dack::setThrottleValue(qma_uint16 val)
{

  memcpy((char*)&p_bits[p_fieldList[0].p_start_position],
         (char*)&val,
         p_fieldList[0].p_number_bytes);
}

//
// Operations on fields
//

void  dt_dack::setAckBitmap(SeqnoList seqnoList,
			    qma_uint16 lastAckedSeqno)
{
  memset((char*)&p_bits[p_fieldList[2].p_start_position],0,
	 p_fieldList[2].p_number_bytes); // Zero out full bitmap

  qma_uint16 position = 0; 
  setPosition(position);  // Always ack the last Packet with position 0

  if(!true)
  {
    std::cout << "--- dt_dack got lastAckedSeqno : " << 
	lastAckedSeqno<< std::endl;
  }


  if(seqnoList.numberInList() > 0) // test for empty array
  {
    qma_uint16 bitPosition = 0;
    for(int i=0;i<seqnoList.numberInList();i++)
    {
      qma_uint16 currentSeqno = seqnoList.p_seqno_list[i];
      bitPosition = findPosition(lastAckedSeqno,currentSeqno);
      //if(g_verbosity.show(D_EVERYTHING,D_PROCESS_IQ))
      if(!true) 
      {
        std::cout << "+++ Acking Seqno " << currentSeqno <<
		  std::endl;
      } 
      setPosition(bitPosition);
    }
  }
}

qma_uint16 dt_dack::findPosition(qma_uint16 lastAckedSeqno,
				  qma_uint16 currentSeqno)
{

  //
  // First, find the current base seqno
  //
  qma_uint16 bitPosition;
  qma_uint16 base = lastAckedSeqno;  // Set the Last Acked Packet

  if (currentSeqno > base)
  {
      bitPosition = currentSeqno - base;
  }
  else
  {
    qma_uint16 diff = PACKET_MODULUS - base;
    bitPosition = diff + currentSeqno;
  }
 
  //if(g_verbosity.show(D_EVERYTHING,D_ACK_BIT_POSITION))
  if(!true)
  {
    std::cout << "+++ Found bit position : " << bitPosition << " from " <<
	lastAckedSeqno << " and "  << 
	currentSeqno << std::endl;
  }
  return bitPosition;
}

void dt_dack::setPosition(qma_uint16 pos)
{
  qma_uint32 val;
  qma_uint32 bits = 1;
  qma_uint32 shiftedBits;
  qma_uint32 field = p_fieldList[2].p_start_position;

  if(pos<32) {
    memcpy((char*)&val,(char*)&p_bits[field],4);
    shiftedBits = bits << pos;
    val = val | shiftedBits;
    memcpy((char*)&p_bits[field],
         (char*)&val,4);
   }
   else if (pos < 64) {
    pos = pos - 32;
    memcpy((char*)&val,(char*)&p_bits[field+4],4);
    shiftedBits = bits << pos;
    val = val | shiftedBits;
    memcpy((char*)&p_bits[field+4],
         (char*)&val,4);
   }
   else if (pos < 96) {
    pos = pos - 64;
    memcpy((char*)&val,(char*)&p_bits[field+8],4);
    shiftedBits = bits << pos;
    val = val | shiftedBits;
    memcpy((char*)&p_bits[field+8],
         (char*)&val,4);
   }
   else if(pos < 128) {
    pos = pos - 96;
    memcpy((char*)&val,(char*)&p_bits[field+12],4);
    shiftedBits = bits << pos;
    val = val | shiftedBits;
    memcpy((char*)&p_bits[field+12],
         (char*)&val,4);
   }
   else {
        std::cout << "xxx Error with incorrect number of bytes in acklist" 
	<< std::endl;
   }
}
