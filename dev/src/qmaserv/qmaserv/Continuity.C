/*
 * File     :
 *  Continuity.C
 *
 * Purpose  :
 *  Save and retreive Continuity infor during operation.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  4 October 2002
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
#include "QmaLimits.h"
#include "StateInfoVO.h"
#include "Continuity.h"
#include "global.h"
#include <fstream>
#include <iostream>

Continuity::Continuity()
{
  strcpy(p_fileName,"qmastate.dat"); 
}

StateInfoVO Continuity::getContinuityInfo() const
{

  StateInfoVO aState;
  std::ifstream pif(p_fileName);
  if(!pif)
  {
    g_log << "xxx Unable to open Continuity file: " <<
	(char *) p_fileName << std::endl;
  }
  else
  {
    pif >> aState.p_timeInfo.drsn;
    pif >> aState.p_timeInfo.sec_offset;
    pif >> aState.p_timeInfo.usec_offset;
    pif >> aState.p_timeInfo.clockQuality;
    pif >> aState.p_timeInfo.minutesSinceLock;
    pif >> aState.p_timeInfo.filter_delay;
    pif.close();
  }
  return aState;
}

bool Continuity::saveContinuityInfo(const StateInfoVO& aState)
{
  bool retValue = false;

  std::ofstream of(p_fileName);
  if(!of)
  {
    g_log << "xxx Unable to open Continuity file: " <<
	p_fileName << std::endl;
    retValue = false;
  }
  else
  {
    of << aState.getTimeInfo().drsn << std::endl;
    of << aState.getTimeInfo().sec_offset << std::endl;
    of << aState.getTimeInfo().usec_offset << std::endl;
    of << aState.getTimeInfo().clockQuality << std::endl;
    of << aState.getTimeInfo().minutesSinceLock << std::endl;
    of << aState.getTimeInfo().filter_delay << std::endl;
    of.flush();
    of.close();
    retValue = true;
  }
  return retValue;
}
