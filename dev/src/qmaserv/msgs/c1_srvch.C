/*
 *
 * File     :
 *   c1_srvch.C
 *
 * Purpose  :
 *
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   24 February 2002
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
#include "c1_srvch.h"


c1_srvch::c1_srvch()
{
  p_fieldListPtr = &p_fieldList[0];
  p_number_of_fields = 0;
  Field f_cv;
  f_cv.p_start_position = 0;
  f_cv.p_number_bytes = 8;
  f_cv.p_data_type = UnsignedInt64;
  strcpy(f_cv.p_name,"ChallengeValue");
  p_fieldList[p_number_of_fields] = f_cv;
  ++p_number_of_fields;  

  Field f_sip;
  f_sip.p_start_position = f_cv.p_start_position + f_cv.p_number_bytes;
  f_sip.p_number_bytes = 4;
  f_sip.p_data_type = UnsignedInt32;
  strcpy(f_sip.p_name,"ServerIPAddress");
    p_fieldList[p_number_of_fields] = f_sip;
  ++p_number_of_fields;  

  Field f_sup;
  f_sup.p_start_position = f_sip.p_start_position + f_sip.p_number_bytes;
  f_sup.p_number_bytes = 2;
  f_sup.p_data_type = UnsignedInt16;
  strcpy(f_sup.p_name,"ServerUDPPort");
  p_fieldList[p_number_of_fields] = f_sup;
  ++p_number_of_fields;  

  Field f_rn;
  f_rn.p_start_position = f_sup.p_start_position + f_sup.p_number_bytes;
  f_rn.p_number_bytes = 2;
  f_rn.p_data_type = UnsignedInt16;
  strcpy(f_cv.p_name,"RegistrationNumber");
  p_fieldList[p_number_of_fields] = f_rn;


  p_length_in_bytes =
    p_fieldList[p_number_of_fields].p_start_position +
	p_fieldList[p_number_of_fields].p_number_bytes;

  p_number_of_fields++;
  memset((char*)&p_bits[0],0,p_length_in_bytes);
}

//
// Operations on fields
//

qma_uint64  c1_srvch::getChallengeValue()
{
  qma_uint64 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[0].p_start_position],
         (unsigned int)p_fieldList[0].p_number_bytes);
  return val;
}


void  c1_srvch::setChallengeValue(qma_uint64 val)
{
  memcpy((char*)&p_bits[p_fieldList[0].p_start_position],
         (char*)&val,
         p_fieldList[0].p_number_bytes);
}

//
//
//

qma_uint32  c1_srvch::getServerIPAddress()
{
  qma_uint32 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[1].p_start_position],
         (unsigned int)p_fieldList[1].p_number_bytes);
  return val;
}


void  c1_srvch::setServerIPAddress(qma_uint32 val)
{
  memcpy((char*)&p_bits[p_fieldList[1].p_start_position],
         (char*)&val,
         p_fieldList[1].p_number_bytes);
}

//
//
//

qma_uint16  c1_srvch::getServerUDPPort()
{
  qma_uint16 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[2].p_start_position],
         (unsigned int)p_fieldList[2].p_number_bytes);
  return val;
}


void  c1_srvch::setServerUDPPort(qma_uint16 val)
{
  memcpy((char*)&p_bits[p_fieldList[2].p_start_position],
         (char*)&val,
         p_fieldList[2].p_number_bytes);
}

//
//
//

qma_uint16  c1_srvch::getRegistrationNumber()
{
  qma_uint16 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[3].p_start_position],
         (unsigned int)p_fieldList[3].p_number_bytes);
  return val;
}


void  c1_srvch::setRegistrationNumber(qma_uint16 val)
{
  memcpy((char*)&p_bits[p_fieldList[3].p_start_position],
         (char*)&val,
         p_fieldList[3].p_number_bytes);
}
