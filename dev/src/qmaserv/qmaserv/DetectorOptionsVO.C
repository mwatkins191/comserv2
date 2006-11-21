/*
 * File     :
 *  DetectorOptionsVO.C
 *
 * Purpose  :
 *   This encapsulates the detector option information retreived
 *   from a Data Port Token.
 *
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *   22 April 2002
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
#include "DetectorOptionsVO.h"

DetectorOptionsVO::DetectorOptionsVO()
{    
  p_detector_base_number = 0;
  p_detector_invocation_number = 0;
  p_runsByDefault = false;
  p_loggingEnabled =false;
  p_textToMessageLog = false;
}

int DetectorOptionsVO::getDetectorBaseNumber() const
{
  return p_detector_base_number;
}

void DetectorOptionsVO::setDetectorBaseNumber(int detNum)
{
  p_detector_base_number = detNum;
}

int DetectorOptionsVO::getDetectorInvocationNumber() const
{
  return p_detector_invocation_number;
}
    
void DetectorOptionsVO::setDetectorInvocationNumber(int detNum)
{
  p_detector_invocation_number = detNum;
}
   
bool DetectorOptionsVO::getRunsByDefault() const
{
  return p_runsByDefault;
}

void DetectorOptionsVO::setRunsByDefault(bool torf)
{
  p_runsByDefault = torf;
}

bool DetectorOptionsVO::getLoggingEnabled() const
{
  return p_loggingEnabled;
}

void DetectorOptionsVO::setLoggingEnabled(bool torf)
{
  p_loggingEnabled = torf;
}


bool DetectorOptionsVO::getTextToMessageLog() const
{
  return p_textToMessageLog;
}

void DetectorOptionsVO::setTextToMessageLog(bool torf)
{
  p_textToMessageLog = torf;
}
