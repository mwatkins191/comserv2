/*
 *
 * File     :
 *   QDPHeader.C
 *
 * Purpose  :
 *   This is a bit oriented header definition. A packet will
 *   contain a list of header and a message.
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
#include <string.h>
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "QDPHeader.h"
#include "Field.h"


QDPHeader::QDPHeader()
{
  p_fieldListPtr = &p_fieldList[0];

  p_number_of_fields =0;
  Field f_crc;
  f_crc.p_start_position = 0;
  f_crc.p_number_bytes = 4;
  f_crc.p_data_type = UnsignedInt32;
  strcpy(f_crc.p_name,"CRC");
  p_fieldList[p_number_of_fields] = f_crc;
  ++p_number_of_fields;
 
  Field f_cmd;
  f_cmd.p_start_position = f_crc.p_start_position + 
					(f_crc.p_number_bytes);
  f_cmd.p_number_bytes = 1;
  f_cmd.p_data_type = UnsignedInt8;
  strcpy(f_cmd.p_name,"Command");
  p_fieldList[p_number_of_fields] = f_cmd;
  ++p_number_of_fields;

  Field f_version;
  f_version.p_start_position = f_cmd.p_start_position + 
					(f_cmd.p_number_bytes);
  f_version.p_number_bytes = 1;
  f_version.p_data_type = UnsignedInt8;
  strcpy(f_version.p_name,"Version");
  p_fieldList[p_number_of_fields] = f_version;
  ++p_number_of_fields;

  Field f_length;
  f_length.p_start_position = f_version.p_start_position + 
					(f_version.p_number_bytes);
  f_length.p_number_bytes = 2;
  f_length.p_data_type = UnsignedInt16;
  strcpy(f_length.p_name,"Length");
  p_fieldList[p_number_of_fields] = f_length;
  ++p_number_of_fields;

  Field f_seqNum;
  f_seqNum.p_start_position = f_length.p_start_position + 
					(f_length.p_number_bytes);
  f_seqNum.p_number_bytes = 2;
  f_seqNum.p_data_type = UnsignedInt16;
  strcpy(f_seqNum.p_name,"SequenceNumber");
  p_fieldList[p_number_of_fields] = f_seqNum;
  ++p_number_of_fields;

  Field f_ackNum;
  f_ackNum.p_start_position = f_seqNum.p_start_position + 
					(f_seqNum.p_number_bytes);
  f_ackNum.p_number_bytes = 2;
  f_ackNum.p_data_type = UnsignedInt16;
  strcpy(f_ackNum.p_name,"AcknowledgeNumber");
  p_fieldList[p_number_of_fields] = f_ackNum;

  p_length_in_bytes =
    p_fieldList[p_number_of_fields].p_start_position +
    p_fieldList[p_number_of_fields].p_number_bytes;

  p_number_of_fields++;
  memset((char*)&p_bits[0],0,p_length_in_bytes);
  
}

//
// Operations on fields
//

void  QDPHeader::setCRC(const qma_uint32 val)
{
  memcpy((char*)&p_bits[p_fieldList[0].p_start_position],
         (char*)&val,
         p_fieldList[0].p_number_bytes);
}


void  QDPHeader::setCommand(const qma_uint8 cmd)
{

 memcpy((char*)&p_bits[p_fieldList[1].p_start_position],
        (char*)&cmd,
        p_fieldList[1].p_number_bytes);
}

void  QDPHeader::setVersion(const qma_uint8 val)
{
  memcpy((char*)&p_bits[p_fieldList[2].p_start_position],
         (char*)&val,
         p_fieldList[2].p_number_bytes);
}

void  QDPHeader::setDataLengthInBytes(const qma_uint16 val)
{

  memcpy((char*)&p_bits[p_fieldList[3].p_start_position],
         (char*)&val,
         p_fieldList[3].p_number_bytes);
}

void  QDPHeader::setPacketSequence(const qma_uint16 val)
{
  memcpy((char*)&p_bits[p_fieldList[4].p_start_position],
         (char*)&val,
         p_fieldList[4].p_number_bytes);
}

void  QDPHeader::setAckNumber(const qma_uint16 val)
{
  memcpy((char*)&p_bits[p_fieldList[5].p_start_position],
         (char*)&val,
         p_fieldList[5].p_number_bytes);
}
    
//
// Operations on fields
//

qma_uint32  QDPHeader::getCRC() const
{
  qma_uint32 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[0].p_start_position],
         (unsigned int)p_fieldList[0].p_number_bytes);
  return val;
}

qma_uint32   QDPHeader::getCommand() const
{
  qma_uint8 val;
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[1].p_start_position],
         (unsigned int)p_fieldList[1].p_number_bytes);
  qma_uint32 intval = (qma_uint32) val;
  return intval;
}
    
qma_uint32   QDPHeader::getVersion() const
{
  qma_uint8 val;
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[2].p_start_position],
         (unsigned int)p_fieldList[2].p_number_bytes);
  qma_uint32 intval = (qma_uint32) val; 
  return intval;
}
    
    
qma_uint16  QDPHeader::getPacketSequence() const
{
  qma_uint16 val;
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[4].p_start_position],
         (unsigned int)p_fieldList[4].p_number_bytes);
  return val;

}
    
qma_uint16  QDPHeader::getAckNumber() const
{
  qma_uint16 val;
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[5].p_start_position],
         (unsigned int)p_fieldList[5].p_number_bytes);
  return val;

}


qma_uint16 QDPHeader::getDataLengthInBytes() const
{
  qma_uint16 val;
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[3].p_start_position],
         (unsigned int)p_fieldList[3].p_number_bytes);
  return val;
}
