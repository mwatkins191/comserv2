/*
 *
 * File     :
 *   c1_sglog.C
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
#include "c1_sglob.h"


c1_sglob::c1_sglob()
{
  p_fieldListPtr = &p_fieldList[0];
  p_number_of_fields =0;
  Field f1;
  f1.p_start_position = 0;
  f1.p_number_bytes = 2;
  f1.p_data_type = UnsignedInt16;
  strcpy(f1.p_name,"ClockTimeout");
  p_fieldList[p_number_of_fields] = f1;
  ++p_number_of_fields;

  Field f2;
  f2.p_start_position = f1.p_start_position + f1.p_number_bytes;
  f2.p_number_bytes = 2;
  f2.p_data_type = UnsignedInt16;
  strcpy(f2.p_name,"InitialVCO");
  p_fieldList[p_number_of_fields] = f2;
  ++p_number_of_fields;

  Field f3;
  f3.p_start_position = f2.p_start_position + f2.p_number_bytes;
  f3.p_number_bytes = 2;
  f3.p_data_type = UnsignedInt16;
  strcpy(f3.p_name,"GPSBackupPower");
  p_fieldList[p_number_of_fields] = f3;
  ++p_number_of_fields;

  Field f4;
  f4.p_start_position = f3.p_start_position + f3.p_number_bytes;
  f4.p_number_bytes = 2;
  f4.p_data_type = UnsignedInt16;
  strcpy(f4.p_name,"AuxStatusSamplingRate");
  p_fieldList[p_number_of_fields] = f4;
  ++p_number_of_fields;

  Field f5;
  f5.p_start_position = f4.p_start_position + f4.p_number_bytes;
  f5.p_number_bytes = 2;
  f5.p_data_type = UnsignedInt16;
  strcpy(f5.p_name,"GainBitmap");
  p_fieldList[p_number_of_fields] = f5;
  ++p_number_of_fields;

  Field f6;
  f6.p_start_position = f5.p_start_position + f5.p_number_bytes;
  f6.p_number_bytes = 2;
  f6.p_data_type = UnsignedInt16;
  strcpy(f6.p_name,"FilterBitmap");
  p_fieldList[p_number_of_fields] = f6;
  ++p_number_of_fields;

  Field f7;
  f7.p_start_position = f6.p_start_position + f6.p_number_bytes;
  f7.p_number_bytes = 2;
  f7.p_data_type = UnsignedInt16;
  strcpy(f7.p_name,"InputBitmap");
  p_fieldList[p_number_of_fields] = f7;
  ++p_number_of_fields;

  Field f8;
  f8.p_start_position = f7.p_start_position + f7.p_number_bytes;
  f8.p_number_bytes = 2;
  f8.p_data_type = UnsignedInt16;
  strcpy(f8.p_name,"WebPort");
  p_fieldList[p_number_of_fields] = f8;
  ++p_number_of_fields;

  Field f9;
  f9.p_start_position = f8.p_start_position + f8.p_number_bytes;
  f9.p_number_bytes = 2;
  f9.p_data_type = UnsignedInt16;
  strcpy(f9.p_name,"ServerTimeout");
  p_fieldList[p_number_of_fields] = f9;
  ++p_number_of_fields;

  Field f10;
  f10.p_start_position = f9.p_start_position + f9.p_number_bytes;
  f10.p_number_bytes = 2;
  f10.p_data_type = UnsignedInt16;
  strcpy(f10.p_name,"DriftTolerance");
  p_fieldList[p_number_of_fields] = f10;
  ++p_number_of_fields;

  Field f11;
  f11.p_start_position = f10.p_start_position + f10.p_number_bytes;
  f11.p_number_bytes = 2;
  f11.p_data_type = UnsignedInt16;
  strcpy(f11.p_name,"JumpFilter");
  p_fieldList[p_number_of_fields] = f11;
  ++p_number_of_fields;

  Field f12;
  f12.p_start_position = f11.p_start_position + f11.p_number_bytes;
  f12.p_number_bytes = 2;
  f12.p_data_type = UnsignedInt16;
  strcpy(f2.p_name,"JumpThreshold");
  p_fieldList[p_number_of_fields] = f12;
  ++p_number_of_fields;

  Field f13;
  f13.p_start_position = f12.p_start_position + f12.p_number_bytes;
  f13.p_number_bytes = 2;
  f13.p_data_type = UnsignedInt16;
  strcpy(f13.p_name,"CalibratorOffset");
  p_fieldList[p_number_of_fields] = f13;
  ++p_number_of_fields;

  Field f14;
  f14.p_start_position = f13.p_start_position + f13.p_number_bytes;
  f14.p_number_bytes = 2;
  f14.p_data_type = UnsignedInt16;
  strcpy(f14.p_name,"SensorControlBitmap");
  p_fieldList[p_number_of_fields] = f14;
  ++p_number_of_fields;

  Field f15;
  f15.p_start_position = f14.p_start_position + f14.p_number_bytes;
  f15.p_number_bytes = 2;
  f15.p_data_type = UnsignedInt16;
  strcpy(f15.p_name,"SamplingPhase");
  p_fieldList[p_number_of_fields] = f15;
  ++p_number_of_fields;

  Field f16;
  f16.p_start_position = f15.p_start_position + f15.p_number_bytes;
  f16.p_number_bytes = 2;
  f16.p_data_type = UnsignedInt16;
  strcpy(f16.p_name,"GPSColdStartSeconds");
  p_fieldList[p_number_of_fields] = f16;
  ++p_number_of_fields;

  Field f17;
  f17.p_start_position = f16.p_start_position + f16.p_number_bytes;
  f17.p_number_bytes = 4;
  f17.p_data_type = UnsignedInt32;
  strcpy(f17.p_name,"UserTage");
  p_fieldList[p_number_of_fields] = f17;
  ++p_number_of_fields;

 Field f18;
  f18.p_start_position = f17.p_start_position + f17.p_number_bytes;
  f18.p_number_bytes = 124;
  //f18.p_data_type = UnsignedInt16;
  f18.p_data_type = VariableLength;
  strcpy(f18.p_name,"FrequencyBits");
  p_fieldList[p_number_of_fields] = f18;
  ++p_number_of_fields;

/*
  Field f19;
  f19.p_start_position = f18.p_start_position + f18.p_number_bytes;
  f19.p_number_bytes = 4;
  f19.p_data_type = UnsignedInt16;
  strcpy(f19.p_name,"OldDataThresholdTime");

  Field f20;
  f20.p_start_position = f19.p_start_position + f19.p_number_bytes;
  f20.p_number_bytes = 2;
  f20.p_data_type = UnsignedInt16;
  strcpy(f20.p_name,"EthernetThrottle");

  Field f21;
  f21.p_start_position = f20.p_start_position + f20.p_number_bytes;
  f21.p_number_bytes = 2;
  f21.p_data_type = UnsignedInt16;
  strcpy(f21.p_name,"AlarmPercent");

  Field f22;
  f22.p_start_position = f21.p_start_position + f21.p_number_bytes;
  f22.p_number_bytes = 2;
  f22.p_data_type = UnsignedInt16;
  strcpy(f22.p_name,"AutomaticFilters");

  Field f23;
  f23.p_start_position = f22.p_start_position + f22.p_number_bytes;
  f23.p_number_bytes = 2;
  f23.p_data_type = UnsignedInt16;
  strcpy(f23.p_name,"ManualFilters");
*/

  Field f19;
  f19.p_start_position = f18.p_start_position + f18.p_number_bytes;
  f19.p_number_bytes = 4;
  f19.p_data_type = UnsignedInt16;
  strcpy(f19.p_name,"MessageBitmap");
  p_fieldList[p_number_of_fields] = f19;


  if(!true)
  {
    std::cout << "Creating c1_sglob with length : " << 
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

qma_uint16  c1_sglob::getClockTimeout()
{

  qma_uint16 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[0].p_start_position],
         (unsigned int)p_fieldList[0].p_number_bytes);
  return val;
}

//
// Skip some here
//

//
// Operations on fields
//

qma_uint16  c1_sglob::getInitialVCO()
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

qma_uint32  c1_sglob::getMessageBitmap()
{
  qma_uint32 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[18].p_start_position],
         (unsigned int)p_fieldList[18].p_number_bytes);
  return val;
}
