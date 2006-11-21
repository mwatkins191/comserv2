/*
 *
 * File     :
 *   c1_dsrv.C
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
#include "c1_dsrv.h"


c1_dsrv::c1_dsrv()
{
  p_fieldListPtr = &p_fieldList[0];
  Field f1;
  f1.p_start_position = 0;
  f1.p_number_bytes = 8;
  f1.p_data_type = UnsignedInt64;
  strcpy(f1.p_name,"SerialNumber");
  p_fieldList[0] = f1;
  p_number_of_fields = 1;
  if(!true)
  {
    std::cout << "Creating c1_dsrv with length : " << 
 	p_fieldList[0].p_start_position  << " " <<
    	p_fieldList[0].p_number_bytes << std::endl;	
  }

  p_length_in_bytes =
    p_fieldList[0].p_start_position + 
    p_fieldList[0].p_number_bytes;

  memset((char*)&p_bits[0],0,p_length_in_bytes);
}

//
// Operations on fields
//

qma_uint64  c1_dsrv::getSerialNumber()
{
  qma_uint64 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[0].p_start_position],
         (unsigned int)p_fieldList[0].p_number_bytes);
  return val;
}


void  c1_dsrv::setSerialNumber(qma_uint64 val)
{
  memcpy((char*)&p_bits[p_fieldList[0].p_start_position],
         (char*)&val,
         p_fieldList[0].p_number_bytes);
}
