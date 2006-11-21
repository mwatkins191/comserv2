/*
 * File     :
 *   StationVO.C
 *
 * Purpose  :
 *   This encapsulates a Station Object for the TokenVO
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
#include "NetStationVO.h"
#include <string.h>
NetStationVO::NetStationVO()
{
  memset((char*)&p_station_code[0],0,(STATION_CODE_LEN+1));
  memset((char*)&p_network_code[0],0,(NETWORK_CODE_LEN+1));
}

//
// This converts from the Token bit string to
// the fields in the token
//
bool NetStationVO::initialize(char* buf)
{
  memset((char*)&p_network_code[0],0,(NETWORK_CODE_LEN+1));
  memcpy((char*)&p_network_code[0],&buf[0],2);

  memset((char*)&p_station_code[0],0,(STATION_CODE_LEN+1));
  memcpy((char*)&p_station_code[0],&buf[2],5);
  return true;
}

char* NetStationVO::getStationCode() const
{
  return ((char*)&p_station_code[0]);
}

//
// Network Name functions
//

char* NetStationVO::getNetworkCode() const
{
  return ((char*)&p_network_code[0]);
}
