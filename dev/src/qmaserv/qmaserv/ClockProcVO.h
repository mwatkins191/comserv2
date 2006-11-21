/*
 * File     :
 *   ClockProc.h
 *
 * Purpose  :
 *   This contains information about clock processing based on the tokens.
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
#ifndef CLOCKPROCVO_H
#define CLOCKPROCVO_H

#include "QmaLimits.h"
#include "QmaTypes.h"

class ClockProcVO
{
  public:
    ClockProcVO();
    ~ClockProcVO() {};

    bool initialize(char* buf);

    qma_uint32 getTimeZoneOffset() const;
    void       setTimeZoneOffset(qma_uint32 ofset);
    
    qma_uint16 getLossInMinutes() const;
    void       setLossInMinutes(qma_uint16 lossInMin);
    
    qma_uint8  getPLLLockQuality() const;
    void       setPLLLockQuality(qma_uint8 lq);

    qma_uint8  getPLLTrackingQuality() const;
    void       setPLLTrackingQuality(qma_uint8 tq);

    qma_uint8  getPLLHoldQuality() const;
    void       setPLLHoldQuality(qma_uint8 hq);
  
    qma_uint8  getPLLOffQuality() const;
    void       setPLLOffQuality(qma_uint8 oq);

    qma_uint8  getHighestHasBeenLocked() const;
    void       setHighsetHasBeenLocked(qma_uint8 hbl);

    qma_uint8  getLowestHasBeenLocked() const;
    void       setLowestHasBeenLocked(qma_uint8 lbl);

    qma_uint8  getNeverHasBeenLocked() const;
    void       setNeverHasBeenLocked(qma_uint8 nbl);

    qma_uint16 getClockQualityFilter() const;
    void       setClockQualityFilter(qma_uint16 cqf); 

  private:

    qma_uint32 p_timezoneOffsetSeconds;
    qma_uint16 p_lossInMinutesBeforeDowngrading;
    qma_uint8  p_pllLockedQuality;
    qma_uint8  p_pllTrackingQuality;
    qma_uint8  p_pllHoldQuality;
    qma_uint8  p_pllOffQuality;
    qma_uint8  p_highestHasBeenLockedQuality;
    qma_uint8  p_lowestHasBeenLockedQuality;
    qma_uint8  p_neverHasBeenLockedQuality;
    qma_uint16 p_clockQualityFilter;

};

#endif
