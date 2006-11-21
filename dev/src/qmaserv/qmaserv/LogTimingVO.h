/*
 * File     :
 *   LogTimingVO.h
 *
 * Purpose  :
 *   This contains information about Log and Timing names based ont
 *     the tokens
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
#ifndef LOGTIMING_H
#define LOGTIMING_H

#include "QmaLimits.h"

class LogTimingVO
{
  public:
    LogTimingVO();
    ~LogTimingVO() {} ;

    bool initialize(char* buf);


    char*  getMessageLogLocation();
    void   setMessageLogLocation(char* loc);

    char*  getMessageLogName();
    void   setMessageLogName(char* name);

    char*  getTimingLogLocation();
    void   setTimingLogLocation(char* loc);
    
    char*  getTimingLogName();
    void   setTimingLogName(char* name);
    
  private:

    char p_message_log_location[LOCATION_CODE_LEN + 1];
    char p_message_log_name[SEED_NAME_LEN + 1];

    char p_timing_log_location[LOCATION_CODE_LEN + 1];
    char p_timing_log_name[SEED_NAME_LEN + 1];

};

#endif
