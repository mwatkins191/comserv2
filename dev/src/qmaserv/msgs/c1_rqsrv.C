/*
 *
 * File     :
 *   c1_rqsrv.C
 *
 * Purpose  :
 *
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
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
 *
 */
#include <iostream>
#include <string.h>
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "md5.h"
#include "findMD5.h"
#include "Field.h"
#include "c1_rqsrv.h"

c1_rqsrv::c1_rqsrv()
{
  p_fieldListPtr = &p_fieldList[0];
  p_number_of_fields = 1;
  

  Field f_sn;
  f_sn.p_start_position = 0;
  f_sn.p_number_bytes = 8;
  f_sn.p_data_type = UnsignedInt64;
  strcpy(f_sn.p_name,"SerialNumber");
  this->p_fieldList[0] = f_sn;	

  if(!true)
  {
    std::cout << "Creating c1_rqsrv with length : " << 
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

qma_uint64  c1_rqsrv::getSerialNumber()
{
  qma_uint64 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[0].p_start_position],
         (unsigned int)p_fieldList[0].p_number_bytes);
  return val;
}


void  c1_rqsrv::setSerialNumber(qma_uint64 val)
{
  memcpy((char*)&p_bits[p_fieldList[0].p_start_position],
         (char*)&val,
         p_fieldList[0].p_number_bytes);
}
