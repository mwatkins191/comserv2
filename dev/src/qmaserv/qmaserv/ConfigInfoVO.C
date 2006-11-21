/*
 * File     :
 *   ConfigInfoVO.C
 *
 * Purpose  :
 *   This encapsulates Configuration Request information
 *     that we receive from the Tokens.
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
#include <iostream>
#include "ConfigInfoVO.h"
#include <string.h>
#include "global.h"

ConfigInfoVO::ConfigInfoVO()
{
  memset((char*)&p_stream_SEED_Location[0],0,(LOCATION_CODE_LEN+1));
  memset((char*)&p_stream_SEED_Name[0],0,(SEED_NAME_LEN+1));
  p_config_option = Beginning;
  p_config_interval = 0;
}

//
// This converts from the Token bit string to
// the fields in the token
//
bool ConfigInfoVO::initialize(char* buf)
{
  memset((char*)&p_stream_SEED_Location[0],0,(LOCATION_CODE_LEN+1));
  memcpy((char*)&p_stream_SEED_Location[0],&buf[0],2);

  memset((char*)&p_stream_SEED_Name[0],0,(SEED_NAME_LEN+1));
  memcpy((char*)&p_stream_SEED_Name[0],&buf[2],3);

  p_config_interval = 0;
  qma_uint8 flags;
  memcpy(&flags,&buf[5],1);
  
  if((flags & 0x01) != 0)
  {
    p_config_option = Beginning;
  }
  else if((flags & 0x02) != 0)
  {
    p_config_option = End;
  }
  else if ((flags & 0x04) != 0)
  {
     p_config_option = Periodic;
     memcpy(&p_config_interval,(char*)&buf[6],2);
  }
  else
  {
    g_log << "xxx Unexpected flags in ConfigInfo Token : " 
    << flags << std::endl;
  }

  return true;
}
//
// Stream Location
//
char* ConfigInfoVO::getStreamSEEDLocation()
{
  return ((char*)&p_stream_SEED_Location[0]);
}

void ConfigInfoVO::setStreamSEEDLocation(char* sta)
{
  strcpy((char*)&p_stream_SEED_Location[0],sta);
}

//
// Stream Name
//
char* ConfigInfoVO::getStreamSEEDName()
{
  return ((char*)&p_stream_SEED_Name[0]);
}


void ConfigInfoVO::setStreamSEEDName(char* sta)
{
  strcpy((char*)&p_stream_SEED_Name[0],sta);
}

//
// Config Interval option methods
// 

ConfigRequestOption ConfigInfoVO::getConfigOption()
{
  return p_config_option;
}

void ConfigInfoVO::setConfigOption(ConfigRequestOption cro)
{
  p_config_option = cro;
}

qma_uint16 ConfigInfoVO::getConfigInterval()
{
  return p_config_interval;
}

void ConfigInfoVO::setConfigInterval(qma_uint16 ci)
{
  p_config_interval = ci;
}
