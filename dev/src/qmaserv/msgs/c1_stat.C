/*
 *
 * File     :
 *   c1_stat.C
 *
 * Purpose  :
 *   This is a status message. It may contain many types of status.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   28 April 2002
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
#include "c1_stat.h"
#include "qmaswap.h"

// temp
//
#include <ctime>

#ifndef _WIN32
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>



c1_stat::c1_stat()
{
  p_fieldListPtr = &p_fieldList[0];
  p_number_of_fields = 0;

  Field f1;
  f1.p_start_position = 0;
  f1.p_number_bytes = 4;
  f1.p_data_type = UnsignedInt32;
  strcpy(f1.p_name,"StatusBitmap");
  p_fieldList[p_number_of_fields] = f1;
  ++p_number_of_fields;

  Field f2;
  f2.p_start_position = f1.p_start_position + f1.p_number_bytes;
  f2.p_number_bytes = 2;
  f2.p_data_type = UnsignedInt16;
  strcpy(f2.p_name,"AcquisitionControl");
  p_fieldList[p_number_of_fields] = f2;
  ++p_number_of_fields;

  Field f3;
  f3.p_start_position = f2.p_start_position + f2.p_number_bytes;
  f3.p_number_bytes = 2;
  f3.p_data_type = UnsignedInt16;
  strcpy(f3.p_name,"ClockQuality");
  p_fieldList[p_number_of_fields] = f3;
  ++p_number_of_fields;

  Field f4;
  f4.p_start_position = f3.p_start_position + f3.p_number_bytes;
  f4.p_number_bytes = 2;
  f4.p_data_type = UnsignedInt16;
  strcpy(f4.p_name,"MinutesSinceLoss");
  p_fieldList[p_number_of_fields] = f4;
  ++p_number_of_fields;

  Field f5;
  f5.p_start_position = f4.p_start_position + f4.p_number_bytes;
  f5.p_number_bytes = 2;
  f5.p_data_type = UnsignedInt16;
  strcpy(f5.p_name,"AnalogVoltageControlValue");
  p_fieldList[p_number_of_fields] = f5;
  ++p_number_of_fields;

  Field f6;
  f6.p_start_position = f5.p_start_position + f5.p_number_bytes;
  f6.p_number_bytes = 4;
  f6.p_data_type = UnsignedInt32;
  strcpy(f6.p_name,"SecondsOffset");
  p_fieldList[p_number_of_fields] = f6;
  ++p_number_of_fields;

  Field f7;
  f7.p_start_position = f6.p_start_position + f6.p_number_bytes;
  f7.p_number_bytes = 4;
  f7.p_data_type = UnsignedInt32;
  strcpy(f7.p_name,"UsecsOffset");
  p_fieldList[p_number_of_fields] = f7;
  ++p_number_of_fields;


  Field f8;
  f8.p_start_position = f7.p_start_position + f7.p_number_bytes;
  f8.p_number_bytes = (MAX_BYTES_IN_DATA - (f7.p_start_position +
  f7.p_number_bytes));
  f8.p_data_type = VariableLength;
  strcpy(f8.p_name,"StatusFields");
  p_fieldList[p_number_of_fields] = f8;


  p_length_in_bytes =
    p_fieldList[p_number_of_fields].p_start_position + 
    p_fieldList[p_number_of_fields].p_number_bytes;

  if(!true)
  {
    std::cout << "--- Creating c1_stat with length : " << 
 	p_fieldList[p_number_of_fields].p_start_position  << " " <<
    	p_fieldList[p_number_of_fields].p_number_bytes << std::endl;	
  }

  p_number_of_fields++;
  memset((char*)&p_bits[0],0,p_length_in_bytes);
}


void c1_stat::setBitString(const unsigned char* buf, qma_uint32 len)
{
  memcpy((char*)&p_raw_bits[0],(char*)buf,len);
  //if(!userMessagePresent()) {
    p_length_in_bytes = len;
    PacketElement::setBitString((unsigned char *)buf);
    //}
}


unsigned char* c1_stat::getOffsetBitString(const qma_uint16 len) const
{
  return (unsigned char*)(p_bits + len);
}

unsigned char* c1_stat::getOffsetRawBitString(const qma_uint16 len) const
{
  return (unsigned char*)(p_raw_bits + len);
}

//
// Test for received status types
//

bool c1_stat::globalStatus() const
{
  qma_uint32 mask = 0x00000001;
  qma_uint32 bitmask;
  memcpy((char*)&bitmask,
         (char*)&p_bits[p_fieldList[0].p_start_position],
         (unsigned int)p_fieldList[0].p_number_bytes);

  if((mask & bitmask) != 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool c1_stat::dataPortProgrammingChanged() const
{
  qma_uint32 mask = 0x40000000;
  qma_uint32 bitmask;
  memcpy((char*)&bitmask,
         (char*)&p_bits[p_fieldList[0].p_start_position],
         (unsigned int)p_fieldList[0].p_number_bytes);

  if((mask & bitmask) != 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool c1_stat::dpTokensChanged() const
{
  qma_uint32 mask = 0x20000000;
  qma_uint32 bitmask;
  memcpy((char*)&bitmask,
         (char*)&p_bits[p_fieldList[0].p_start_position],
         (unsigned int)p_fieldList[0].p_number_bytes);

  if((mask & bitmask) != 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

//
// Test for received status types
//

int c1_stat::userDataPortStatusPresent() const
{
  int res = 0;
  qma_uint32 mask = 0x00000f00;
  qma_uint32 bitmask;
  memcpy((char*)&bitmask,
         (char*)&p_bits[p_fieldList[0].p_start_position],
         (unsigned int)p_fieldList[0].p_number_bytes);

  qma_uint32 bres = 0;
  bres = mask & bitmask;

  if(bres != 0)
  {
    if(bres == 0x00000100)
    {
      res = 1;
    }
    else if (bres == 0x00000200)
    {
       res = 2;
    }
    else if(bres == 0x00000400)
    {
       res = 3;
    }
    else if (bres == 0x00000800)
    {
       res = 4;
    }
    else 
    {
      std::cout << "xxx Unknown Port status received: " 
       << (qma_uint32) bres << std::endl;
    }
    return res;
  }
  else
  {
    return res;
  }
}


bool c1_stat::userMessagePresent() const
{
  qma_uint32 mask = 0x80000000;
  qma_uint32 bitmask;
  //  memcpy((char*)&bitmask,
  //       (char*)&p_bits[p_fieldList[0].p_start_position],
  //       (unsigned int)p_fieldList[0].p_number_bytes);

  memcpy( (char *)&bitmask, &p_bits[0], 4);

  if(false)
  {
    std::cout << "--- Status Flags Bitmap : " << std::hex << bitmask << 
	std::endl;
  }

  if((mask & bitmask) != 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}


char* c1_stat::getUserMessage() const
{

  strcpy((char*)&p_user_message[0],"");

  if(!userMessagePresent())
  {
     return (char*)&p_user_message[0];
  }

  //
  // This assumes that User Message is always offset 4 chars.
  // This may not be true if more than global status is requested
  //
  int offsetToUserMessage = 4;   
  char passtring[120];

    
  memcpy((char*)&p_userIPAddress,
	 (char*)getOffsetRawBitString(offsetToUserMessage),4);
  memcpy((char*)&passtring[0],
	(char*)getOffsetRawBitString(offsetToUserMessage+4),80);

  
  strpcopy((char*)&p_user_message[0],(char*)&passtring[0]);

  
  return (char*)&p_user_message[0];
}




qma_uint32 c1_stat::getUserIPAddress() const
{
  return p_userIPAddress;
}

qma_uint16 c1_stat::getClockQuality() const
{
  qma_uint16 qual = CQ_LOCK;
  qma_uint16 val;  

  //
  // The clock quality values are returned as a 2 bytes because
  // the Timing Quality algorithm is written to use the Q330
  // 2 byte word in its calcutations.

  if(userMessagePresent())
  {
    int offsetToClockQuality = 4 + 84 +2;
    memcpy((char*)&val,
	   (char*)getOffsetBitString(offsetToClockQuality),
           (unsigned int)2);
    val = qma_ntohs(val);
  }
  else
  {
 
    memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[2].p_start_position],
         (unsigned int)p_fieldList[2].p_number_bytes);
  }
  return val;
}


qma_uint16 c1_stat::getCalibrationStatus() const
{
  qma_uint16 val;  
  if (userMessagePresent())
  {
    int offsetToCalibration = 4 + 84 + 34;

    memcpy((char*)&val,
	   (char*)getOffsetBitString(offsetToCalibration),
           (unsigned int)2);
    val = qma_ntohs(val);
  }
  else
  {

   int offsetToCalibration = 4 + 34;
    memcpy((char*)&val,
           (char*)getOffsetBitString(offsetToCalibration),
           (unsigned int)2);
  }
  return qma_htons(val);
}


qma_uint16 c1_stat::getMinutesSinceLoss() const
{

  qma_uint16 val;  
  if (userMessagePresent())
  {
    int offsetToMinutes = 4 + 84 + 4;

    memcpy((char*)&val,
	   (char*)getOffsetBitString(offsetToMinutes),
           (unsigned int)2);
    val = qma_ntohs(val);
  }
  else
  {
    memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[3].p_start_position],
         (unsigned int)p_fieldList[3].p_number_bytes);
  }
  return val;
}

qma_uint32 c1_stat::getSecondsOffset() const
{

  qma_uint32 val;
  if(userMessagePresent())
  {
    int offsetToSeconds = 4 + 84 + 8;

    memcpy((char*)&val,
	   (char*)getOffsetBitString(offsetToSeconds),
           (unsigned int)4);
    val = qma_ntohl(val);
    if(!true)
    { 
      std::cout << "+++ Seconds offset when userMessagePresent() :"
	      << val << std::endl;
    }
  }
  else
  {
    memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[5].p_start_position],
         (unsigned int)p_fieldList[5].p_number_bytes);
   if(!true)
   {
     std::cout << "+++ Seconds offset when no userMessagePresent() :"
	      << val << std::endl;
   }
  }
  return val;
}

qma_uint32 c1_stat::getUsecsOffset() const
{

  qma_uint32 val;
  if(userMessagePresent())
  {
      int offsetToUsecs = 4 + 84 + 16;

      memcpy((char*)&val,
	   (char*)getOffsetBitString(offsetToUsecs),
           (unsigned int)4);
    val = qma_ntohl(val);
  }
  else
  {
    memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[6].p_start_position],
         (unsigned int)p_fieldList[6].p_number_bytes);
  }
  return val;
}


qma_uint32 c1_stat::getDRSN() const
{

  qma_uint32 val;
  if(userMessagePresent())
  {
      int offsetToDRSN = 4 + 84 + 48;

      memcpy((char*)&val,
	   (char*)getOffsetBitString(offsetToDRSN),
           (unsigned int)4);
  }
  else
  {

    int offsetToDRSN = 4 + 48;
    memcpy((char*)&val,
        (char*)getOffsetBitString(offsetToDRSN),
        (unsigned int)4);
  }
  return qma_htonl(val);
}

qma_int16 c1_stat::getBoomPosition(int whichBoom) const {
  // this will always be following the global
  qma_int16 val;
  int offsetToThisBoom = 0;
  if(userMessagePresent()) {
    offsetToThisBoom = STAT_BITMAP_SIZE + STAT_USERMSG_SIZE + STAT_GLOBAL_SIZE + ((whichBoom-1) * 2);
  } else {
    offsetToThisBoom = STAT_BITMAP_SIZE + STAT_GLOBAL_SIZE + ((whichBoom-1) * 2);
  }

  memcpy((char*)&val,
	 (char*)getOffsetBitString(offsetToThisBoom),
	 (unsigned int)2);

   return qma_ntohs(val);
}

qma_uint16 c1_stat::getInputPower() const {
  qma_uint16 val;
  int offsetToInputPower = 0;
  if(userMessagePresent()) {
    offsetToInputPower = STAT_BITMAP_SIZE + STAT_USERMSG_SIZE + STAT_GLOBAL_SIZE + 16;
  } else {
    offsetToInputPower = STAT_BITMAP_SIZE + STAT_GLOBAL_SIZE + 16;
  }

  memcpy((char *)&val,
	 (char*)getOffsetBitString(offsetToInputPower),
	 (unsigned int)2);
  return qma_ntohs(val);
}

qma_uint32 c1_stat::getDataPortQueue() const
{
  // 
  // This assumes that Port Status, boom status and Global Status are always
  // requested together.
  //
  qma_uint32 val;
  int offsetToDataPort = 0;
  if(userMessagePresent())
  {
      offsetToDataPort = STAT_BITMAP_SIZE + STAT_USERMSG_SIZE + STAT_GLOBAL_SIZE + STAT_MISC_SIZE + 16;

      memcpy((char*)&val,
	   (char*)getOffsetBitString(offsetToDataPort),
           (unsigned int)4);
  }
  else
  {

    offsetToDataPort = STAT_BITMAP_SIZE + STAT_GLOBAL_SIZE + STAT_MISC_SIZE + 16;
    memcpy((char*)&val,
        (char*)getOffsetBitString(offsetToDataPort),
        (unsigned int)4);
  }

  return qma_ntohl(val);
}



/* Convert Pascal string to C string */

void strpcopy (char* outstring, char* instring)
{
  qma_uint8 len = instring[0];
  int i;
  for (i = 0 ; i < len ; i++)
  {
    outstring[i] = instring[i + 1] ;
  }
  outstring[i] = '\0' ;
}
