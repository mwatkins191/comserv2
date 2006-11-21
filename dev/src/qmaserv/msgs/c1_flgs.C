/*
 *
 * File     :
 *   c1_flgs.C
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
#include "c1_flgs.h"


c1_flgs::c1_flgs()
{
  p_fieldListPtr = &p_fieldList[0];
  p_number_of_fields = 0;
  Field f_ogp;
  f_ogp.p_start_position = 0;
  f_ogp.p_number_bytes = 2;
  f_ogp.p_data_type = UnsignedInt16;
  strcpy(f_ogp.p_name,"OffsetToGlobalProgramming");
  p_fieldList[p_number_of_fields] = f_ogp; 
  ++p_number_of_fields;

  Field f_osc;
  f_osc.p_start_position = f_ogp.p_start_position + f_ogp.p_number_bytes;
  f_osc.p_number_bytes = 2;
  f_osc.p_data_type = UnsignedInt16;
  strcpy(f_osc.p_name,"OffsetToSensorControl");
  p_fieldList[p_number_of_fields] = f_osc;
  ++p_number_of_fields;

  Field f_olp;
  f_olp.p_start_position = f_osc.p_start_position + f_osc.p_number_bytes;
  f_olp.p_number_bytes = 2;
  f_olp.p_data_type = UnsignedInt16;
  strcpy(f_olp.p_name,"OffsetToDataPort");
  p_fieldList[p_number_of_fields] = f_olp;
  ++p_number_of_fields;

  Field f4;
  f4.p_start_position = f_olp.p_start_position + f_olp.p_number_bytes;
  f4.p_number_bytes = 2;
  f4.p_data_type = UnsignedInt16;
  strcpy(f4.p_name,"Spare");
  p_fieldList[p_number_of_fields] = f4;
  ++p_number_of_fields;

  Field f5;
  f5.p_start_position = f4.p_start_position + f4.p_number_bytes;
  f5.p_number_bytes = MAX_BYTES_IN_DATA - (f4.p_start_position + f4.p_number_bytes);
  f5.p_data_type = VariableLength;
  strcpy(f5.p_name, "RemainingData");
  p_fieldList[p_number_of_fields] = f5;

  //
  // This is a variable length packet. Initialize it to
  // max size, and reset it when populated.
  //
  p_length_in_bytes = MAXDATASIZE;

  p_number_of_fields++;
  memset((char*)&p_bits[0],0,p_length_in_bytes);

  if(!true)
  {
    std::cout << "Creating c1_flgs with length : " << 
 	p_length_in_bytes << std::endl;
  }
}


void c1_flgs::setBitString(const unsigned char* buf, qma_uint32 len)
{
  //memcpy(p_bits,buf,len);
  p_length_in_bytes = len;
  PacketElement::setBitString( (unsigned char *) buf );
}

unsigned char* c1_flgs::getOffsetBitString(const qma_uint16 len) const
{
  return (unsigned char*)(p_bits + len);
}


//
// Operations on fields
//

qma_uint16  c1_flgs::getOffsetToFixedValues() const
{
  qma_uint16 val;
  val = p_fieldList[3].p_start_position + p_fieldList[3].p_number_bytes;
  return val;
}

qma_uint16  c1_flgs::getOffsetToGlobalProgramming() const
{
  qma_uint16 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[0].p_start_position],
         (unsigned int)p_fieldList[0].p_number_bytes);
  return val;
}

//
// Operations on fields
//

qma_uint16  c1_flgs::getOffsetToSensorControl() const
{
  qma_uint16 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[1].p_start_position],
         (unsigned int)p_fieldList[1].p_number_bytes);
  return val;
}

//
// Operations on fields
//

qma_uint16  c1_flgs::getOffsetToDataPort() const
{
  qma_uint16 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[2].p_start_position],
         (unsigned int)p_fieldList[2].p_number_bytes);
  return val;
}
