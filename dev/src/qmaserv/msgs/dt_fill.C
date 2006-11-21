/*
 *
 * File     :
 *   dt_fill.C
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
#include "dt_fill.h"

dt_fill::dt_fill()
{
  p_fieldListPtr = &p_fieldList[0];
  p_number_of_fields = 1;
  Field f1;
  f1.p_start_position = 0;
  f1.p_number_bytes = 4;
  f1.p_data_type = UnsignedInt32;
  strcpy(f1.p_name,"FillRecordSequenceNumber");
  p_fieldList[0] = f1;

  if(!true)
  {
    std::cout << "Creating dt_fill with length : " << 
 	p_fieldList[0].p_start_position  << " + " <<
    	p_fieldList[0].p_number_bytes << std::endl;	
  }

  p_length_in_bytes =
    p_fieldList[0].p_start_position + 
    p_fieldList[0].p_number_bytes;

  memset((char*)&p_bits[0],0,p_length_in_bytes);
}


void dt_fill::setBitString(const unsigned char* buf, qma_uint32 len)
{
  memcpy(p_bits,buf,len);
  p_length_in_bytes = len;
}

unsigned char* dt_fill::getOffsetBitString(const qma_uint16 len)
{
  return (unsigned char*)(p_bits + len);
}

//
// Operations on fields
//

qma_uint32  dt_fill::getFillRecordSequenceNumber()
{
  qma_uint32 val;
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[0].p_start_position],
         (unsigned int)p_fieldList[0].p_number_bytes);
  return val;
}
