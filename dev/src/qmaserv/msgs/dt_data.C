/*
 *
 * File     :
 *   dt_data.C
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
#include "Field.h"
#include "dt_data.h"
#include "qmaswap.h"

dt_data::dt_data()
{
  p_fieldListPtr = &p_fieldList[0];
  p_number_of_fields=0;

  Field f1;
  f1.p_start_position = 0;
  f1.p_number_bytes = 4;
  f1.p_data_type = UnsignedInt32;
  strcpy(f1.p_name,"DataRecordSequenceNumber");
  p_fieldList[p_number_of_fields] = f1;
  ++p_number_of_fields;

  Field f2;
  f2.p_start_position = f1.p_start_position + f1.p_number_bytes;
  f2.p_number_bytes = (MAX_BYTES_IN_DATA - f1.p_number_bytes);
  f2.p_data_type = VariableLength;
  strcpy(f2.p_name,"DataBlockettes");
  p_fieldList[p_number_of_fields] = f2;

  if(!true)
  {
    std::cout << "Creating dt_data with length : " << 
 	p_fieldList[p_number_of_fields].p_start_position  << " + " <<
    	p_fieldList[p_number_of_fields].p_number_bytes << std::endl;	
  }

  p_length_in_bytes =
    p_fieldList[p_number_of_fields].p_start_position + 
    p_fieldList[p_number_of_fields].p_number_bytes;

  p_number_of_fields++;
  memset((char*)&p_bits[0],0,p_length_in_bytes);
}


void dt_data::setBitString(const unsigned char* buf, qma_uint32 len)
{
  //memcpy(p_bits,buf,len);
  p_length_in_bytes = len;
  PacketElement::setBitString((unsigned char *)buf);
}

unsigned char* dt_data::getOffsetBitString(const qma_uint16 len)
{
  return (unsigned char*)(p_bits + len);
}

//
// Operations on fields
//

qma_uint32  dt_data::getDataRecordSequenceNumber()
{
  qma_uint32 val;
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[0].p_start_position],
         (unsigned int)p_fieldList[0].p_number_bytes);
  return val;
}

bool dt_data::packetIsStartOfSecond()
{

  bool retValue = false;
  int totallen = getLengthInBytes();

  //
  // Length of minimum packet containint timestamp
  //
  if(totallen < 16)
  {
    return retValue;
  }

  int curposition = 4;    // Start past Data Record Sequence Number
  
  qma_uint8 chan;         // Raw channel from blockette
  qma_uint8 val;          // Place to put result of anding this with flags
 
  memcpy((char*)&chan,getOffsetBitString(curposition),1);
  val = (chan & DCM); // mask out channel stuff leaving packet type in val

  if(val == DC_MN232)
  {
    qma_uint8 chanindex = (chan & 0x07);
    qma_uint16 dchanindex;
    dchanindex = chanindex;
    if(chanindex == 0)
    {
      retValue = true;
    }
  }
  return retValue;
}

BTI dt_data::getCurrentTimeInfo()
{
  BTI retValue;
  int totallen = getLengthInBytes();

  if(!packetIsStartOfSecond())
  {
    std::cout << "xxx Error Called getCurrentTimeInfo on packet";
    std::cout << " which did not contain a DC_MN232 blockette." << std::endl;
    return retValue;
  }

  int curposition = 4;    // Start past Data Record Sequence Number
  qma_uint8 chan;         // Raw channel from blockette
  qma_uint8 val;          // Place to put result of anding this with flags
 
  memcpy((char*)&chan,getOffsetBitString(curposition),1);
  val = (chan & DCM); // mask out channel stuff leaving packet type in val

  if(val == DC_MN232)
  {
    qma_uint8 chanindex = (chan & 0x07);
    qma_uint16 dchanindex;
    dchanindex = chanindex;

    if(chanindex == 0)
    {
      //
      // Create a Blockette Time Info object for use as current
      // BTI time stamp
      //
      qma_uint32 curval32 = getDataRecordSequenceNumber();
      retValue.drsn = curval32;

      memcpy((char*)&curval32,getOffsetBitString(curposition+4),4);
      retValue.sec_offset = qma_ntohl(curval32);

      memcpy((char*)&curval32,getOffsetBitString(curposition+8),4);
      retValue.usec_offset = qma_ntohl(curval32);

      qma_uint8 curval8;
      memcpy((char*)&curval8,getOffsetBitString(curposition+1),1);
      retValue.clockQuality = curval8;

      qma_uint16 curval16;
      memcpy((char*)&curval16,getOffsetBitString(curposition+2),2);
      retValue.minutesSinceLock = qma_ntohs(curval16);
    }
    else
    {
     std::cout << "xxx Error Called getCurrentTimeInfo on packet";
     std::cout << " which did not contain a DC_MN232 blockette." << std::endl;
    }
  }
  return retValue;
}
