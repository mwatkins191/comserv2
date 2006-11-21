/*
 * File     :
 *   LogTimingVO.C
 *
 * Purpose  :
 *   This encapsulates Log and Timing file naming information
 *     that we received from the Tokens.
 *
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *   27 April 2002
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
#include "LogTimingVO.h"
#include <string.h>
LogTimingVO::LogTimingVO()
{
  memset((char*)&p_message_log_location[0],0,(LOCATION_CODE_LEN+1));
  memset((char*)&p_message_log_name[0],0,(SEED_NAME_LEN+1));
  memset((char*)&p_timing_log_location[0],0,(LOCATION_CODE_LEN+1));
  memset((char*)&p_timing_log_name[0],0,(SEED_NAME_LEN+1));
}

//
// This converts from the Token bit string to
// the fields in the token
//
bool LogTimingVO::initialize(char* buf)
{
  memset((char*)&p_message_log_location[0],0,(LOCATION_CODE_LEN+1));
  memcpy((char*)&p_message_log_location[0],&buf[0],2);

  memset((char*)&p_message_log_name[0],0,(SEED_NAME_LEN+1));
  memcpy((char*)&p_message_log_name[0],&buf[2],3);

  memset((char*)&p_timing_log_location[0],0,(LOCATION_CODE_LEN+1));
  memcpy((char*)&p_timing_log_location[0],&buf[5],2);

  memset((char*)&p_timing_log_name[0],0,(SEED_NAME_LEN+1));
  memcpy((char*)&p_timing_log_name[0],&buf[7],3);

  return true; 
}

//
// MessageLocation
//
char* LogTimingVO::getMessageLogLocation()
{
  return ((char*)&p_message_log_location[0]);
}

void LogTimingVO::setMessageLogLocation(char* sta)
{
  strcpy((char*)&p_message_log_location[0],sta);
}

//
// MessageName
//
char* LogTimingVO::getMessageLogName()
{
  return ((char*)&p_message_log_name[0]);
}


void LogTimingVO::setMessageLogName(char* sta)
{
  strcpy((char*)&p_message_log_name[0],sta);
}

//
// TimingLogLocation
//
char* LogTimingVO::getTimingLogLocation()
{
  return ((char*)&p_timing_log_location[0]);
}


void LogTimingVO::setTimingLogLocation(char* sta)
{
  strcpy((char*)&p_timing_log_location[0],sta);
}

//
// TimingLogName
//
char* LogTimingVO::getTimingLogName()
{
  return ((char*)&p_timing_log_name[0]);
}

void LogTimingVO::setTimingLogName(char* sta)
{
  strcpy((char*)&p_timing_log_name[0],sta);
}
