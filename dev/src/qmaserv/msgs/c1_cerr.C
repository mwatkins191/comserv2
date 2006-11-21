/*
 *
 * File     :
 *   c1_cerr.C
 *
 * Purpose  :
 *  This implements a c1_err packet.
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
#include "c1_cerr.h"

c1_cerr::c1_cerr()
{
  p_fieldListPtr = &p_fieldList[0];
  Field f_err;
  f_err.p_start_position = 0;
  f_err.p_number_bytes = 2;
  f_err.p_data_type = UnsignedInt16;
  strcpy(f_err.p_name,"ErrorCode");
  p_fieldList[0] = f_err;
  p_number_of_fields = 1;
  if(!true)
  {
    std::cout << "Creating c1_err with length : " << 
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

qma_uint16  c1_cerr::getErrorCode() const
{
  qma_uint16 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[0].p_start_position],
         (unsigned int)p_fieldList[0].p_number_bytes);
  return val;
}
