/*
 * File     :
 *   ConfigInfoVO.h
 *
 * Purpose  :
 *   This contains information about Configuration Streams from the
 *     
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *   20 April 2002 
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
#ifndef CONFIGINFOVO_H
#define CONFIGINFOVO_H

#include "QmaLimits.h"
#include "QmaTypes.h"

enum ConfigRequestOption {Beginning,End,Periodic};

class ConfigInfoVO
{
  public:
    ConfigInfoVO();
    ~ConfigInfoVO() {} ;

    bool initialize(char* buf);

    void  setStreamSEEDLocation(char* sta);
    char* getStreamSEEDLocation();

    void  setStreamSEEDName(char* sta);
    char*  getStreamSEEDName();

    ConfigRequestOption getConfigOption();
    void setConfigOption(ConfigRequestOption opt);

    qma_uint16 getConfigInterval();
    void  setConfigInterval(qma_uint16 ci);

   

  private:

    char p_stream_SEED_Location[LOCATION_CODE_LEN + 1];
    char p_stream_SEED_Name[SEED_NAME_LEN + 1];
    ConfigRequestOption p_config_option;
    qma_uint16 p_config_interval;

};

#endif
