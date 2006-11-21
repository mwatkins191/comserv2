/*
 * File     :
 *  LCQVO.h
 *
 * Purpose  :
 *  Logical Channel Queue value objects. Once the tokens have been read int,
 *  a series of tokens were recorded. These VO's contain the information
 *  about the LCQ that was in the token.
 *
 *  In addtion, these objects will be used to initialize the Packet Compression
 *  Queues. In order to provide complete information about the Packet, this
 *  LCQVO also contains configuration information, such as min_samples in
 *  a packet, which is a user configurable data item.
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
#ifndef LCQVO_H
#define LCQVO_H

#include "QmaLimits.h"
#include "QmaTypes.h"
#include "OptionBitsVO.h"

class LCQVO
{
  public:
    LCQVO();
    ~LCQVO() {};

   //
   // All of the token values are set here, through this
   // initialize function. User setable values have both
   // get and set methods.

   void         initialize(char* buf,int len);

   char*        getLocationCode() const;
   char*        getSEEDName() const;
   qma_uint8    getLCQReferenceNumber() const;
   qma_uint8    getChannel() const;
   qma_uint8    getChannelByte() const;
   qma_uint8    getFrequencyBit() const;
   qma_uint8    getCNPPort() const;

   OptionBitsVO getOptionBits() const;

   //void         setOptionBits(OptionBitsVO obv);
   //
   // Unprocessed fields for diagnostics
   //
   qma_uint8    getSource() const;
   qma_uint8    getParameterNumber() const;
   qma_uint16   getSourceParameter() const;

   qma_uint32   getOptionBitsFlag() const;
   qma_int16    getRate() const;
   qma_uint32   getSamplesPerBlockette() const;

   qma_int32    getFilterDelay() const;
   void         setFilterDelay(const qma_int32 aDelay);

   //
   // User settable data values, from the configuration file.
   // These are retrieved from the fixed flags and stored
   // here with other information about this LCQ for convienence.
   //
   //

   void     setFrequencyHertz(const double hrtz);
   double   getFrequencyHertz() const;

   //
   // These are from globabl Tokens
   //
   void setStationCode(char* sta);
   char* getStationCode() const;

   void setNetworkCode(char* nc);
   char* getNetworkCode() const;

   int  getMinimumSamplesPerPacket() const;
   void setMinimumSamplesPerPacket(int minsamps);


  private:
   char         p_location_code[LOCATION_CODE_LEN + 1]; 
   char         p_seed_name[SEED_NAME_LEN + 1];

   char         p_network_code[NETWORK_CODE_LEN + 1]; 
   char         p_station_code[STATION_CODE_LEN + 1];

   qma_uint8    p_reference_number;
   //
   // This is the raw data word. It is decomposed into the following.
   //
   qma_uint8    p_channel_number;
   qma_uint8    p_channel_byte;
   //
   // These three are stored in the same byte and only one is populated
   // at any one time.
   //
   qma_uint8    p_frequency_bit;
   qma_uint8    p_cnp_port;

   qma_uint16   p_source_parameter;
   qma_uint8    p_source;
   qma_uint8    p_parameter_number;

   qma_int32    p_filter_delay;
   //
   // This is the raw representations without interpretation.
   //
   qma_uint32   p_optionbits;
   OptionBitsVO p_option_bits;
   qma_int16    p_rate;
   int          p_minimum_samples_per_packet;
   double       p_frequency_hertz;
};
#endif
