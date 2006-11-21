/*
 * File     :
 *  LCQVO.C
 *
 * Purpose  :
 *  Logical Channel Queue value objects. Once the tokens have been read int,
 *  a series of tokens were recorded. These VO's contain the information
 *  about the LCQ that was in the token.
 *
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *  22 April 2002
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
#include "LCQVO.h"
#include <string.h>
#include "qmaswap.h"
#include "global.h"
LCQVO::LCQVO()
{
  memset(p_location_code,0,(strlen(p_location_code)+1)); 
  memset(p_seed_name,0,(strlen(p_seed_name)+1));

  memset(p_station_code,0,(strlen(p_station_code)+1)); 
  memset(p_network_code,0,(strlen(p_network_code)+1));

  p_reference_number = 0;
  p_channel_number = 0;
  p_channel_byte = 0;
  p_frequency_bit = 0;
  p_cnp_port = 0;
 
  p_source_parameter = 0; 
  p_source = 0;
  p_parameter_number =0;
  p_optionbits = 0;
  p_rate = 0;
  p_minimum_samples_per_packet = 1000;
 
  p_filter_delay = 0;
  p_frequency_hertz = 0.0;
  //
  // Default constructor initializes the
  // OptionBitsVO and it's objects
  //
}


//
// LCQ is the one of the variable length Tokens.
// The intialize then requires a len byte.
//
void LCQVO::initialize(char* buf, int len)
{
  memset(p_location_code,0,(strlen(p_location_code)+1)); 
  memset(p_seed_name,0,(strlen(p_seed_name)+1));

  p_reference_number = 0;
  p_channel_number = 0;
  p_channel_byte = 0;
  p_frequency_bit =0;
  p_cnp_port = 0;

  p_source_parameter = 0;
  p_source = 0;
  p_parameter_number =0;
  p_optionbits = 0;
  p_rate = 0;
  
  p_filter_delay = 0;
  p_frequency_hertz = 0.0;
  //
  // This manually inserts a terminating null at the end of
  // the location and seed string. Should not be necessary
  //
  memcpy(&p_location_code[0],(char*)&buf[0],2);
  memset(&p_location_code[2],0,1);
  memcpy(&p_seed_name[0],(char*)&buf[2],3);
  memset(&p_seed_name[3],0,1);

  memcpy(&p_reference_number,(char*)&buf[5],1);
  memcpy(&p_channel_byte,(char*)&buf[6],1);
  p_channel_number = p_channel_byte & 0x07;
  //
  // Start with check for CNP Port config
  //
  if(p_channel_byte == 0xfa)
  {
    memcpy(&p_cnp_port,(char*)&buf[7],1);
  }
  else
  {
    memcpy(&p_frequency_bit,(char*)&buf[7],1);
  }
  memcpy(&p_source_parameter,(char*)&buf[6],2);
  memcpy(&p_source,(char*)&buf[6],1);
  memcpy(&p_parameter_number,(char*)&buf[7],1);
  memcpy(&p_optionbits,(char*)&buf[8],4);
  memcpy(&p_rate,(char*)&buf[12],2);

  // make sure multibyte values are in the proper byte order
  p_source_parameter = qma_ntohs(p_source_parameter);
  p_optionbits = qma_ntohl(p_optionbits);
  p_rate = qma_ntohs(p_rate);


  if(!true)
  {
    g_log << "--- SEED Name : " << p_seed_name << std::endl;
    g_log << "--- Location  : " << p_location_code << std::endl;
    g_log << "--- Get SEED Name : " << getSEEDName() << std::endl;
    g_log << "--- Get Location  : " << getLocationCode() << std::endl;
  }
}


//
// Getters and Setters
//

char* LCQVO::getLocationCode() const
{
  return (char*)&p_location_code[0];
}

char* LCQVO::getSEEDName() const
{
  return (char*)&p_seed_name[0];
}

qma_uint8 LCQVO::getLCQReferenceNumber() const
{
  return p_reference_number;  
}

qma_uint8 LCQVO::getChannel() const
{
  return (p_channel_number +1);
}

qma_uint8 LCQVO::getChannelByte() const
{
  return p_channel_byte;
}


/*
void LCQVO::setChannel(qma_uint8 chan)
{
  p_channel_number = chan;
}
*/

qma_uint8 LCQVO::getFrequencyBit() const
{
  return p_frequency_bit;
}

/*
void LCQVO::setFrequencyBit(qma_uint8 freq)
{
  p_frequency_bit = freq;
}
*/

qma_uint8 LCQVO::getCNPPort() const
{
  return p_cnp_port;
}

/*
void LCQVO::setCNPPort(qma_uint8 cnp)
{
  p_cnp_port = cnp;
}
*/

OptionBitsVO LCQVO::getOptionBits() const
{
  return p_option_bits;
}

/*
void LCQVO::setOptionBits(OptionBitsVO obv)
{
  p_option_bits = obv;
}
*/

qma_uint16 LCQVO::getSourceParameter() const
{
  return p_source_parameter;
}

qma_uint8 LCQVO::getSource() const
{
  //
  // Use the channel mask to remove packet type info,
  // and to just show source information.
  //
  qma_uint8 res = p_source;
  return res;
}

qma_uint8 LCQVO::getParameterNumber() const
{
  return p_parameter_number;
}


qma_uint32 LCQVO::getOptionBitsFlag() const
{
  return p_optionbits;
}

qma_int16 LCQVO::getRate() const
{
  return p_rate;
}

qma_uint32 LCQVO::getSamplesPerBlockette() const
{
  //
  // If blockette rate is more than 1, return rate of rate
  // as 32 bit unsigned. If rate is less than 1, return samples
  // per blockette as 1.
  //
  qma_uint32 res = 0;
  if( p_rate > 1) 
  {
    res = (qma_uint32) p_rate;
  }
  else
  {
    res = 1;
  }
  return res;
}


int LCQVO::getMinimumSamplesPerPacket() const
{
  return p_minimum_samples_per_packet;
}

void LCQVO::setMinimumSamplesPerPacket(int minsamps)
{
  p_minimum_samples_per_packet = minsamps;
}

void LCQVO::setStationCode(char* sta)
{
  strcpy(p_station_code,sta);
}

char* LCQVO::getStationCode() const
{
  return (char*)&p_station_code[0];
}
  
void LCQVO::setNetworkCode(char* nc)
{
  strcpy(p_network_code,nc);
}

char* LCQVO::getNetworkCode() const
{
  return (char*)&p_network_code[0];
}

qma_int32 LCQVO::getFilterDelay() const
{
  return p_filter_delay;
}

void LCQVO::setFilterDelay(const qma_int32 aDelay)
{
  p_filter_delay = aDelay;
}

double LCQVO::getFrequencyHertz() const
{
  return p_frequency_hertz;
}

void LCQVO::setFrequencyHertz(const double aHertz)
{
  //g_log << "--- (start) rate: " << aHertz << std::endl;
  double aval = aHertz;
  if (aHertz < 0)
  {
    aval = -1.0 * aHertz;
    //g_log << "--- (a) aval: " << aval << std::endl;
    p_frequency_hertz = 1.0/aval;
    //g_log << "--- (b) p_freq_hertz: " << p_frequency_hertz << std::endl;
  } 
  else
  {
    p_frequency_hertz = aval;
    //g_log << "--- (c) p_freq_hertz: " << p_frequency_hertz << std::endl;
  }
}
