/*
 *
 * File     :
 *   c1_srvrsp.C
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
#include <stdlib.h>
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "md5.h"
#include "findMD5.h"
#include "Field.h"
#include "c1_srvrsp.h"


c1_srvrsp::c1_srvrsp()
{
  p_fieldListPtr = &p_fieldList[0];
  p_number_of_fields =0;
  Field f_sn;
  f_sn.p_start_position = 0;
  f_sn.p_number_bytes = 8;
  f_sn.p_data_type = UnsignedInt64;
  strcpy(f_sn.p_name,"SerialNumber");
  p_fieldList[p_number_of_fields] = f_sn;
  ++p_number_of_fields;

  Field f_cv;
  f_cv.p_start_position = f_sn.p_start_position + f_sn.p_number_bytes;
  f_cv.p_number_bytes = 8;
  f_cv.p_data_type = UnsignedInt64;
  strcpy(f_cv.p_name,"ChallengeValue");
  strcpy(f_sn.p_name,"SerialNumber");
  p_fieldList[p_number_of_fields] = f_cv;
  ++p_number_of_fields;

  Field f_sip;
  f_sip.p_start_position = f_cv.p_start_position + f_cv.p_number_bytes;
  f_sip.p_number_bytes = 4;
  f_sip.p_data_type = UnsignedInt32;
  strcpy(f_sip.p_name,"ServerIPAddress");
  strcpy(f_sn.p_name,"SerialNumber");
  p_fieldList[p_number_of_fields] = f_sip;
  ++p_number_of_fields;

  Field f_sup;
  f_sup.p_start_position = f_sip.p_start_position + f_sip.p_number_bytes;
  f_sup.p_number_bytes = 2;
  f_sup.p_data_type = UnsignedInt16;
  strcpy(f_sup.p_name,"ServerUDPPort");
  strcpy(f_sn.p_name,"SerialNumber");
  p_fieldList[p_number_of_fields] = f_sup;
  ++p_number_of_fields;

  Field f_rn;
  f_rn.p_start_position = f_sup.p_start_position + f_sup.p_number_bytes;
  f_rn.p_number_bytes = 2;
  f_rn.p_data_type = UnsignedInt16;
  strcpy(f_cv.p_name,"RegistrationNumber");
  p_fieldList[p_number_of_fields] = f_rn;
  ++p_number_of_fields;

  Field f_rand;
  f_rand.p_start_position = f_rn.p_start_position + f_rn.p_number_bytes;
  f_rand.p_number_bytes = 8;
  f_rand.p_data_type = UnsignedInt64;
  strcpy(f_rand.p_name,"RandomNumber");
  p_fieldList[p_number_of_fields] = f_rand;
  ++p_number_of_fields;

  Field f_md5;
  f_md5.p_start_position = f_rand.p_start_position + f_rand.p_number_bytes;
  f_md5.p_number_bytes = 16;
  f_md5.p_data_type = ASCII;
  strcpy(f_md5.p_name,"MD5Result");
  p_fieldList[p_number_of_fields] = f_md5;

  if(!true)
  {
    std::cout << "Creating srvrsp with length : " << 
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

qma_uint64  c1_srvrsp::getSerialNumber()
{
  qma_uint64 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[0].p_start_position],
         (unsigned int)p_fieldList[0].p_number_bytes);
  return val;
}


void  c1_srvrsp::setSerialNumber(qma_uint64 val)
{
  memcpy((char*)&p_bits[p_fieldList[0].p_start_position],
         (char*)&val,
         p_fieldList[0].p_number_bytes);
}

//
// Operations on fields
//

qma_uint64  c1_srvrsp::getChallengeValue()
{
  qma_uint64 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[1].p_start_position],
         (unsigned int)p_fieldList[1].p_number_bytes);
  return val;
}


void  c1_srvrsp::setChallengeValue(qma_uint64 val)
{
  memcpy((char*)&p_bits[p_fieldList[1].p_start_position],
         (char*)&val,
         p_fieldList[1].p_number_bytes);
}

//
//
//

qma_uint32  c1_srvrsp::getServerIPAddress()
{
  qma_uint32 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[2].p_start_position],
         (unsigned int)p_fieldList[2].p_number_bytes);
  return val;
}


void  c1_srvrsp::setServerIPAddress(qma_uint32 val)
{
  memcpy((char*)&p_bits[p_fieldList[2].p_start_position],
         (char*)&val,
         p_fieldList[2].p_number_bytes);
}

//
//
//

qma_uint16  c1_srvrsp::getServerUDPPort()
{
  qma_uint16 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[3].p_start_position],
         (unsigned int)p_fieldList[3].p_number_bytes);
  return val;
}


void  c1_srvrsp::setServerUDPPort(qma_uint16 val)
{
  memcpy((char*)&p_bits[p_fieldList[3].p_start_position],
         (char*)&val,
         p_fieldList[3].p_number_bytes);
}

//
//
//

qma_uint16  c1_srvrsp::getRegistrationNumber()
{
  qma_uint16 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[4].p_start_position],
         (unsigned int)p_fieldList[4].p_number_bytes);
  return val;
}


void  c1_srvrsp::setRegistrationNumber(qma_uint16 val)
{
  memcpy((char*)&p_bits[p_fieldList[4].p_start_position],
         (char*)&val,
         p_fieldList[4].p_number_bytes);
}


//
//
//

qma_uint64  c1_srvrsp::getRandomNumber()
{    
  qma_uint64 val;
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[5].p_start_position],
         (unsigned int)p_fieldList[5].p_number_bytes);
  return val;
}


void  c1_srvrsp::setRandomNumber(qma_uint64 val)
{
  memcpy((char*)&p_bits[p_fieldList[5].p_start_position],
         (char*)&val,
         p_fieldList[5].p_number_bytes);
}

//
// This MD5 Result works only if the member fields were set already,
// including cv,sip,sup,rn, rand, and sn.
//

void  c1_srvrsp::setMD5Result(const qma_uint64 authCode)
{

    //
    // Assemble string to md5 on.
    //

    unsigned char signature[16];

    findMD5(getChallengeValue(),
            getServerIPAddress(),
            getServerUDPPort(),
            getRegistrationNumber(),
            authCode,
            getSerialNumber(),
            getRandomNumber(),
            signature);

  memcpy((char*)&p_bits[p_fieldList[6].p_start_position],
         (char*)&signature,
         p_fieldList[6].p_number_bytes);
}

qma_uint64 c1_srvrsp::generateRandomNumber()
{
  qma_uint64 val     = rand() % 0xffffffff;
  return val;
}
