/*
 * File     :
 *   TokenVO.h
 *
 * Purpose  :
 *   This class encapsulates the configuration information received
 *   from the logical port memory read.
 *
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *  20 April 2002
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
#ifndef TokenVO_H
#define TokenVO_H

#include "QmaLimits.h"
#include "NetStationVO.h"
#include "ClockProcVO.h"
#include "LogTimingVO.h"
#include "ConfigInfoVO.h"

class TokenVO 
{
  public:

    TokenVO() {};
    ~TokenVO() {};

    int processTokenBuffer(char* buf, int len);
    NetStationVO getNetStationVO();
    int          getVersion();
    qma_uint16   getDPNetServerPortNumber();
    qma_uint16   getDPWebServerPortNumber();
    ClockProcVO  getClockProcVO();
    LogTimingVO  getLogTimingVO(); 
    ConfigInfoVO getConfigInfoVO();
    qma_uint16   getDataServerPort();
   
 private:
    int 	 p_version;
    NetStationVO p_netStation;
    qma_uint16 	 p_dpNetServerPort;
    qma_uint16 	 p_dpWebServerPort;
    ClockProcVO  p_clockProcessing;
    LogTimingVO  p_logTimingInfo;
    ConfigInfoVO p_configInfo;
    qma_uint16   p_dataServerPort;
};

#endif
