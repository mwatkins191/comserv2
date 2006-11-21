/*
 * File     :
 *   ClockProc.C
 *
 * Purpose  :
 *   This contains information about clock processing based on the tokens.
 *
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *   24 April 2002 
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

#include <string.h>
#include "QmaLimits.h"
#include "ClockProcVO.h"
#include "qmaswap.h"

ClockProcVO::ClockProcVO()
{
  p_timezoneOffsetSeconds = 0;
  p_lossInMinutesBeforeDowngrading =0;
  p_pllLockedQuality =0;
  p_pllTrackingQuality=0;
  p_pllHoldQuality =0;
  p_pllOffQuality=0;
  p_highestHasBeenLockedQuality=0;
  p_lowestHasBeenLockedQuality=0;
  p_neverHasBeenLockedQuality=0;
  p_clockQualityFilter=0;
}


bool ClockProcVO::initialize(char* tbuf)
{
  memcpy(&p_timezoneOffsetSeconds,(char*)&tbuf[0],4);
  memcpy(&p_lossInMinutesBeforeDowngrading,(char*)&tbuf[4],2);
  memcpy(&p_pllLockedQuality,(char*)&tbuf[6],1);
  memcpy(&p_pllTrackingQuality,(char*)&tbuf[7],1);
  memcpy(&p_pllHoldQuality,(char*)&tbuf[8],1);
  memcpy(&p_pllOffQuality,(char*)&tbuf[9],1);
  //
  // Spare byte here for padding
  //
  memcpy(&p_highestHasBeenLockedQuality,(char*)&tbuf[11],1);
  memcpy(&p_lowestHasBeenLockedQuality,(char*)&tbuf[12],1);
  memcpy(&p_neverHasBeenLockedQuality,(char*)&tbuf[13],1);
  memcpy(&p_clockQualityFilter,(char*)&tbuf[14],2);

  p_timezoneOffsetSeconds = qma_ntohl(p_timezoneOffsetSeconds);
  p_lossInMinutesBeforeDowngrading = qma_ntohs(p_lossInMinutesBeforeDowngrading);
  p_clockQualityFilter = qma_ntohs(p_clockQualityFilter);
  return true;
}


qma_uint32 ClockProcVO::getTimeZoneOffset() const
{
  return  p_timezoneOffsetSeconds;
}
void ClockProcVO::setTimeZoneOffset(qma_uint32 ofset)
{
  p_timezoneOffsetSeconds = ofset;
}

    
qma_uint16 ClockProcVO::getLossInMinutes() const
{
  return p_lossInMinutesBeforeDowngrading;
}
void ClockProcVO::setLossInMinutes(qma_uint16 lossInMin)
{
  p_lossInMinutesBeforeDowngrading  = lossInMin;
}


qma_uint8 ClockProcVO::getPLLLockQuality() const
{
  return p_pllLockedQuality;
}
void ClockProcVO::setPLLLockQuality(qma_uint8 lq)
{
  p_pllLockedQuality = lq;
}

    
qma_uint8 ClockProcVO::getPLLTrackingQuality() const
{
  return p_pllTrackingQuality;
}
void  ClockProcVO::setPLLTrackingQuality(qma_uint8 hq)
{
  p_pllTrackingQuality = hq;
}


qma_uint8 ClockProcVO::getPLLHoldQuality() const
{
  return p_pllHoldQuality;
}
void ClockProcVO::setPLLHoldQuality(qma_uint8 hq)
{
  p_pllHoldQuality = hq;
}

qma_uint8 ClockProcVO::getPLLOffQuality() const
{
  return p_pllOffQuality;
}
void ClockProcVO::setPLLOffQuality(qma_uint8 oq)
{
  p_pllOffQuality = oq;
}

qma_uint8 ClockProcVO::getHighestHasBeenLocked() const
{
  return p_highestHasBeenLockedQuality;
}
void ClockProcVO::setHighsetHasBeenLocked(qma_uint8 hbl)
{
  p_highestHasBeenLockedQuality = hbl;
}


qma_uint8 ClockProcVO::getLowestHasBeenLocked() const
{
  return p_lowestHasBeenLockedQuality;
}
void ClockProcVO::setLowestHasBeenLocked(qma_uint8 lbl)
{
  p_lowestHasBeenLockedQuality = lbl;
}


qma_uint8 ClockProcVO::getNeverHasBeenLocked() const
{
  return p_neverHasBeenLockedQuality;
}
void ClockProcVO::setNeverHasBeenLocked(qma_uint8 nbl)
{
  p_neverHasBeenLockedQuality = nbl;
}




qma_uint16 ClockProcVO::getClockQualityFilter() const
{
  return p_clockQualityFilter;
}
void ClockProcVO::setClockQualityFilter(qma_uint16 cqf)
{
  p_clockQualityFilter = cqf;
}
