/*
 * File     :
 *   NetStationVO.h
 *
 * Purpose  :
 *   This contains information about a station and net based on the tokens
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
#ifndef NETSTATION_H
#define NETSTATION_H

#include "QmaLimits.h"

class NetStationVO
{
  public:

    NetStationVO();
    ~NetStationVO() {} ;

    bool initialize(char* buf);

    void  getStationName(char* sta) const;
    char* getStationCode() const;
    void  setStationName(char* sta);
    void  getNetworkName(char* net) const;
    char* getNetworkCode() const;
    void setNetworkName(char* net);

  private:

    char p_station_code[STATION_CODE_LEN + 1];
    char p_network_code[NETWORK_CODE_LEN + 1];
};

#endif
