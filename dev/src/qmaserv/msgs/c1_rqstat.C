/*
 *
 * File     :
 *   c1_rqstat.C
 *
 * Purpose  :
 *
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   28 April 2002
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
#include "c1_rqstat.h"


c1_rqstat::c1_rqstat()
{
  p_fieldListPtr = &p_fieldList[0];
  p_number_of_fields = 1;

  Field f1;
  f1.p_start_position = 0;
  f1.p_number_bytes = 4;
  f1.p_data_type = UnsignedInt32;
  strcpy(f1.p_name,"StatusRequestBitmap");
  p_fieldList[0] = f1;


  if(!true)
  {
    std::cout << "Creating c1_rqstat with length : " << 
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

void  c1_rqstat::requestAllStatus()
{
  qma_uint32 mask = 0x00000ffff;
  memcpy((char*)&p_bits[0],(char*)&mask,4);
}


void  c1_rqstat::requestGlobalAndBoomPositionStatus(int portNumber)
{
  qma_uint32 mask = 0x00000009;
  qma_uint32 temp = 0x00000000;
  if(portNumber == 1)
  {
      temp = 0x00000100;
  }
  else if (portNumber == 2)
  {
    temp = 0x00000200;
  }
  else if(portNumber == 3)
  {
    temp = 0x00000400;
  }
  else if(portNumber == 4)
  {
    temp = 0x00000800;
  }
  else
  {
    std::cout << "xxx Invalid port specified in Request Data Port Status" 
	 << std::endl;
  }

  mask = temp | mask;
  memcpy((char*)&p_bits[0],(char*)&mask,4);
}

void  c1_rqstat::requestDataPortStatus(int portNumber)
{
  qma_uint32 mask = 0x00000000;
  if(portNumber == 1)
  {
      mask = 0x00000100;
  }
  else if (portNumber == 2)
  {
    mask = 0x00000200;
  }
  else if(portNumber == 3)
  {
    mask = 0x00000400;
  }
  else if(portNumber == 4)
  {
    mask = 0x00000800;
  }
  else
  {
    std::cout << "xxx Invalid port specified in Request Data Port Status" 
	 << std::endl;
  }
  memcpy((char*)&p_bits[0],(char*)&mask,4);
}

void c1_rqstat::requestSerialInterfaceStatus(int portNumber)
{
  qma_uint32 mask = 0x00000800;
  qma_uint32 temp;
  memcpy((char*)&temp,(char*)&p_bits[0],4);
  if(portNumber == 1)
  {
      mask = 0x00000800;
  }
  else if (portNumber == 2)
  {
    mask = 0x00001000;
  }
  else if(portNumber == 3)
  {
    mask = 0x00002000;
  }
  else
  {
    std::cout << "xxx Invalid port specified in Request Port Status" 
	 << std::endl;
  }
  temp = temp | mask;
  memcpy((char*)&p_bits[0],(char*)&mask,4);
}
