/*
 * File     :
 *   ClockUtils.C
 *
 * Purpose  :
 *  This contains the utilies that translate clock measurements into
 *  a clock quality value for the SEED Header.
 *
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *  14 October 2002
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
#include "QmaTypes.h"
#include "ClockUtils.h"
#include "ClockProcVO.h"
#include <iostream>

qma_uint8 translate_clock(const ClockProcVO& cVO,
                          const qma_uint16 clockQuality,
                          const qma_uint16 minutesSinceLoss)
{
  qma_uint16 calcVal = 0;
  qma_uint8 retVal = 0;

  if ( (clockQuality >= PLL_TRACK) ||
      (clockQuality & (CQ_3D | CQ_2D | CQ_1D)) != 0)
  {
    qma_uint16 val = clockQuality & PLL_LOCK;
    switch(val)
    {
      case PLL_LOCK:
      {
        calcVal = cVO.getPLLLockQuality();
        break;
      }
      case PLL_TRACK:
      {
        calcVal = cVO.getPLLTrackingQuality();
        break;
      }
      case PLL_HOLD:
      {
        calcVal = cVO.getPLLHoldQuality();
        break;
      }
      case PLL_OFF:
      {
        calcVal = cVO.getPLLOffQuality();
        break;
      }
      default:
      {
        calcVal = cVO.getPLLOffQuality();
        break;
      }
    }
    retVal = (qma_uint8) calcVal;
  }
  else if((clockQuality & CQ_LOCK) != 0)
  {
    int tval = 0; 
    if(cVO.getLossInMinutes() != 0)
    {
      tval = minutesSinceLoss /cVO.getLossInMinutes();
      tval = cVO.getHighestHasBeenLocked() - tval;
    }
    else
    {
      tval = cVO.getHighestHasBeenLocked();
    }

    if(tval < cVO.getLowestHasBeenLocked())
    {
      tval = cVO.getLowestHasBeenLocked();
    }
    retVal = (qma_uint8) tval;
  }
  else
  {
    retVal = cVO.getNeverHasBeenLocked();
  }  
  return retVal;
}
