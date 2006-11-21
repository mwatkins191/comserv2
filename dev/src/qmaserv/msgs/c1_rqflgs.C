/*
 *
 * File     :
 *   c1_rqflgs.C
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
#include "c1_rqflgs.h"


c1_rqflgs::c1_rqflgs()
{
  p_fieldListPtr = &p_fieldList[0];
  Field f_lpn;
  p_number_of_fields = 1;
  f_lpn.p_start_position = 0;
  f_lpn.p_number_bytes = 2;
  f_lpn.p_data_type = UnsignedInt16;
  strcpy(f_lpn.p_name,"DataPortNumber");
  p_fieldList[0] = f_lpn;


  if(!true)
  {
    std::cout << "Creating c1_rqflgs with length : " << 
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

qma_uint16  c1_rqflgs::getDataPortNumber()
{
  qma_uint16 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[0].p_start_position],
         (unsigned int)p_fieldList[0].p_number_bytes);
  return val;
}


void  c1_rqflgs::setDataPortNumber(qma_uint16 val)
{
  //
  // We reduce the data port number by one, to match
  // the number of the system, and then copy it in.
  //
  if(val > 0)
  {
    if(!true)
    {
      std::cout << "In rqflgs - Setting local port number of : " << val << std::endl;
    }
    val= val-1;
  }
  else
  {
    std::cout << "Invalid data port used : " << val << std::endl;
  }

  memcpy((char*)&p_bits[p_fieldList[0].p_start_position],
         (char*)&val,
         p_fieldList[0].p_number_bytes);
}
