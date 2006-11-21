/*
 *
 * File     :
 *   c1_rqmem.C
 *
 * Purpose  :
 *   This message is used to request memory segments (tokens) from the
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
#include "c1_rqmem.h"


c1_rqmem::c1_rqmem()
{
  p_fieldListPtr = &p_fieldList[0];
  p_number_of_fields = 0;
  Field f1;
  f1.p_start_position = 0;
  f1.p_number_bytes = 4;
  f1.p_data_type = UnsignedInt32;
  strcpy(f1.p_name,"Starting Address");
  p_fieldList[p_number_of_fields] = f1;
  ++p_number_of_fields;
 

  Field f2;
  f2.p_start_position = 
	f1.p_start_position + f1.p_number_bytes;
  f2.p_number_bytes = 2;
  f2.p_data_type = UnsignedInt16;
  strcpy(f2.p_name,"Byte Count");
  p_fieldList[p_number_of_fields] = f2;
  ++p_number_of_fields;
 

  Field f3;
  f3.p_start_position = 
	f2.p_start_position + f2.p_number_bytes;
  f3.p_number_bytes = 2;
  f3.p_data_type = UnsignedInt16;
  strcpy(f3.p_name,"Memory Type");
  p_fieldList[p_number_of_fields] = f3;
  ++p_number_of_fields;
 

  Field f4;
  f4.p_start_position = 
	f3.p_start_position + f3.p_number_bytes;
  f4.p_number_bytes = 16;
  f4.p_data_type = UnsignedInt128;
  strcpy(f4.p_name,"Password");
  p_fieldList[p_number_of_fields] = f4;

  if(!true)
  {
    std::cout << "Creating c1_rqmem with length : " << 
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

qma_uint32  c1_rqmem::getStartingAddress()
{
  qma_uint32 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[0].p_start_position],
         (unsigned int)p_fieldList[0].p_number_bytes);
  return val;
}


void  c1_rqmem::setStartingAddress(qma_uint32 val)
{

  memcpy((char*)&p_bits[p_fieldList[0].p_start_position],
         (char*)&val,
         p_fieldList[0].p_number_bytes);
}

//
//
//

qma_uint16 c1_rqmem::getByteCount()
{
  qma_uint16 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[1].p_start_position],
         (unsigned int)p_fieldList[1].p_number_bytes);
  return val;
}

void  c1_rqmem::setByteCount(qma_uint16 val)
{

  memcpy((char*)&p_bits[p_fieldList[1].p_start_position],
         (char*)&val,
         p_fieldList[1].p_number_bytes);
}

//
//
//

qma_uint16  c1_rqmem::getMemoryType()
{
  qma_uint16 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[2].p_start_position],
         (unsigned int)p_fieldList[2].p_number_bytes);
  return val;
}


void  c1_rqmem::setMemoryType(qma_uint16 val)
{
  memcpy((char*)&p_bits[p_fieldList[2].p_start_position],
         (char*)&val,
         p_fieldList[2].p_number_bytes);
}

//
// This must be called with 32 bytes of storage available
//

void c1_rqmem::getPassword(qma_char* pwd32bytes)
{
  memcpy((char*)pwd32bytes,
         (char*)&p_bits[p_fieldList[3].p_start_position],
         (unsigned int)p_fieldList[3].p_number_bytes);
}


void  c1_rqmem::setPassword(qma_char* pwd32bytes)
{
  memcpy((char*)&p_bits[p_fieldList[3].p_start_position],
         (char*)pwd32bytes,
	 p_fieldList[3].p_number_bytes);
}

qma_uint16 c1_rqmem::getDataPortToken(qma_uint16 PortNumber)
{
  qma_uint16 retval = 0;
  int val = PortNumber;
  switch(val)
  {
    case 1:
      {
	retval = DataPort1;
	break;
      }
    case 2:
      {
	retval = DataPort2;
	break;
      }
    case 3:
      {
	retval = DataPort3;
	break;
      }
    case 4:
      {
	retval = DataPort4;
	break;
      }
    default :
      {
	std::cout << "Unknown Data Port in send for tokens" << std::endl;
	break;
      }
  }
  return retval;
     
}
