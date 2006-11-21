/*
 *
 * File     :
 *   c1_sc.C
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
#include "c1_sc.h"


c1_sc::c1_sc()
{
  p_fieldListPtr = &p_fieldList[0];
  p_number_of_fields =0;

  Field f1;
  f1.p_start_position = 0;
  f1.p_number_bytes = 4;
  f1.p_data_type = UnsignedInt32;
  strcpy(f1.p_name,"SensorOutput1");
  p_fieldList[p_number_of_fields] = f1;
  ++p_number_of_fields;

  Field f2;
  f2.p_start_position = f1.p_start_position + f1.p_number_bytes;
  f2.p_number_bytes = 4;
  f2.p_data_type = UnsignedInt32;
  strcpy(f2.p_name,"SensorOutput2");
  p_fieldList[p_number_of_fields] = f2;
  ++p_number_of_fields;

  Field f3;
  f3.p_start_position = f2.p_start_position + f2.p_number_bytes;
  f3.p_number_bytes = 4;
  f3.p_data_type = UnsignedInt32;
  strcpy(f3.p_name,"SensorOutput3");
  p_fieldList[p_number_of_fields] = f3;
  ++p_number_of_fields;

  Field f4;
  f4.p_start_position = f3.p_start_position + f3.p_number_bytes;
  f4.p_number_bytes = 4;
  f4.p_data_type = UnsignedInt32;
  strcpy(f4.p_name,"SensorOutput4");
  p_fieldList[p_number_of_fields] = f4;
  ++p_number_of_fields;

  Field f5;
  f5.p_start_position = f4.p_start_position + f4.p_number_bytes;
  f5.p_number_bytes = 4;
  f5.p_data_type = UnsignedInt32;
  strcpy(f5.p_name,"SensorOutput5");
  p_fieldList[p_number_of_fields] = f5;
  ++p_number_of_fields;

  Field f6;
  f6.p_start_position = f5.p_start_position + f5.p_number_bytes;
  f6.p_number_bytes = 4;
  f6.p_data_type = UnsignedInt32;
  strcpy(f6.p_name,"SensorOutput6");
  p_fieldList[p_number_of_fields] = f6;
  ++p_number_of_fields;

  Field f7;
  f7.p_start_position = f6.p_start_position + f6.p_number_bytes;
  f7.p_number_bytes = 4;
  f7.p_data_type = UnsignedInt32;
  strcpy(f7.p_name,"SensorOutput7");
  p_fieldList[p_number_of_fields] = f7;
  ++p_number_of_fields;

  Field f8;
  f8.p_start_position = f7.p_start_position + f7.p_number_bytes;
  f8.p_number_bytes = 2;
  f8.p_data_type = UnsignedInt32;
  strcpy(f8.p_name,"SensorOutput8");
  p_fieldList[p_number_of_fields] = f8;

  if(!true)
  {
    std::cout << "Creating c1_log with length : " << 
 	p_fieldList[p_number_of_fields].p_start_position  << " + " <<
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

qma_uint32  c1_sc::getSensorOutput1()
{
  qma_uint32 val;
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[0].p_start_position],
         (unsigned int)p_fieldList[0].p_number_bytes);
  return val;
}

qma_uint32  c1_sc::getSensorOutput2()
{
  qma_uint32 val;
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[1].p_start_position],
         (unsigned int)p_fieldList[1].p_number_bytes);
  return val;
}

qma_uint32  c1_sc::getSensorOutput3()
{
  qma_uint32 val;
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[2].p_start_position],
         (unsigned int)p_fieldList[2].p_number_bytes);
  return val;
}

qma_uint32  c1_sc::getSensorOutput4()
{
  qma_uint32 val;
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[3].p_start_position],
         (unsigned int)p_fieldList[3].p_number_bytes);
  return val;
}

qma_uint32  c1_sc::getSensorOutput5()
{
  qma_uint32 val;
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[4].p_start_position],
         (unsigned int)p_fieldList[4].p_number_bytes);
  return val;
}

qma_uint32  c1_sc::getSensorOutput6()
{
  qma_uint32 val;
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[5].p_start_position],
         (unsigned int)p_fieldList[5].p_number_bytes);
  return val;
}

qma_uint32  c1_sc::getSensorOutput7()
{
  qma_uint32 val;
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[6].p_start_position],
         (unsigned int)p_fieldList[6].p_number_bytes);
  return val;
}

qma_uint32  c1_sc::getSensorOutput8()
{
  qma_uint32 val;
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[7].p_start_position],
         (unsigned int)p_fieldList[7].p_number_bytes);
  return val;
}
