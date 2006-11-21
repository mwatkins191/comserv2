/*
 * File     :
 *  TimeServer.C
 *
 * Purpose  :
 *  The calculations for calculating timestamps from Q330 data packets
 *  and for converting into SEED timestamps are contained here.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  28 April 2002
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

#include <iostream>

#ifndef _WIN32
#include <sys/time.h>
#endif

#include <math.h>
#include <time.h>
#include <string.h>
#include "TimeServer.h"
#include "QmaLimits.h"
#include "QmaTypes.h"
#include "QmaDiag.h"
#include "BTI.h"
#include "global.h"

TimeServer::TimeServer()
{
  p_drsn = 0;
  p_seconds_offset    = 0;
  p_usec_offset       = 0;
  p_calibrationStatus = 0;
  p_clockQuality      = 0;
  p_minutesSinceLoss  = 0;

  //
  // Array's initialized to zero, so no array init here
  //

  p_epoch_offset = SECONDS_BETWEEN_1970_2000;
}


void TimeServer::setDRSN(qma_uint32 drsn)
{
  p_drsn = drsn;
}


void TimeServer::setSecondsOffset(qma_uint32 so)
{
  p_seconds_offset = so;
}

void TimeServer::setUsecsOffset(qma_uint32 useco)
{
  p_usec_offset = useco;
}

char* TimeServer::currentSystemStatusTimeString()
{
  double dtt = getDigitizerTimeTag(p_drsn,
				   p_seconds_offset,
				   p_usec_offset,
				   0);

  timeval tt = convertDouble2Timeval(dtt);
  time_t ptime = (time_t) tt.tv_sec;

 //
  // Given a time_t, conver to tm
  //
  struct tm mytime;
  
  time_t* lptime = &ptime;
  mytime = *gmtime(lptime);

  // add starting bracket and ending bracket.
  // remove terminating \n

  char bracket = '[';
  memset((char*)&p_timeString[0],bracket,1);
  memcpy((char*)&p_timeString[1],asctime(&mytime),26);
  bracket = ']';
  memset((char*)&p_timeString[25],bracket,1);
  return (char*)&p_timeString[0];
}



void TimeServer::setMinutesSinceLoss(const qma_uint16 msl)
{
  p_minutesSinceLoss = msl;
}

qma_uint16 TimeServer::getMinutesSinceLoss() const
{
  return p_minutesSinceLoss;
}

void TimeServer::setClockQuality(const qma_uint16 cq)
{
  p_clockQuality = cq;
}

qma_uint16 TimeServer::getClockQuality() const
{
  return p_clockQuality;
}


void TimeServer::setCalibrationStatus(const qma_uint16 cs)
{
  p_calibrationStatus = cs;
}

qma_uint16 TimeServer::getCalibrationStatus() const
{
  return p_calibrationStatus;
}

void TimeServer::setChan13FreqBitDelay(int freq,qma_int32 delay)
{
  p_chan_freq_delay[0][freq] = delay;
  p_chan_freq_delay[1][freq] = delay;
  p_chan_freq_delay[2][freq] = delay;
}

void TimeServer::setChan46FreqBitDelay(int freq,qma_int32 delay)
{
  p_chan_freq_delay[3][freq] = delay;
  p_chan_freq_delay[4][freq] = delay;
  p_chan_freq_delay[5][freq] = delay;
}

qma_int32 TimeServer::getChanFreqDelay(int chan, int freqbit)
{
  return p_chan_freq_delay[chan][freqbit];
}

double TimeServer::getRootTimeTag(qma_uint32 dsr,
					qma_uint32 secOffset,
					qma_uint32 usecOffset)
{
  double retval = (double) dsr + 
                  (double) secOffset + 
                 ((double) usecOffset/ONE_MILLION);
  return retval;
}

double TimeServer::getDigitizerTimeTag(qma_uint32 dsr,
				       qma_uint32 secOffset,
				       qma_uint32 usecOffset,
				       qma_int32 filterDelay)
{
  double roottt = getRootTimeTag(dsr,
			         secOffset,
				 usecOffset);

  double ddel = (double) filterDelay; // type conversion before division
  ddel = ddel / ONE_MILLION; // convert to usecs
  double retval = roottt - ddel;
  return retval;
}
 
struct timeval TimeServer::convertDouble2Timeval(double tt)
{
  double usec;
  double sec;
  usec = modf(tt,&sec);
  qma_uint32 isec = (qma_uint32) sec;

  usec = usec * ONE_MILLION;
  qma_uint32 iusec = (qma_uint32) usec;

  isec = isec + p_epoch_offset;

  struct timeval utime;
  utime.tv_sec = (long) isec;
  utime.tv_usec = (long) iusec;

  return utime;
}


timeval TimeServer::convertBTI2Timeval(BTI tinfo)
{

  double dtt = getDigitizerTimeTag(tinfo.drsn,
				   tinfo.sec_offset,
				   tinfo.usec_offset,
				   tinfo.filter_delay);

  timeval tt = convertDouble2Timeval(dtt);

  return tt;
}

void TimeServer::timeval2sdr(timeval& tt,char* btime)
{
  //
  // The input assumption is the system is set to UTC
  //

  time_t ptime = (time_t) tt.tv_sec;

  //
  // Given a time_t, conver to tm
  //
  struct tm mytime;
  qma_uint16 year;
  qma_uint16 day;
  qma_uint8 hour;
  qma_uint8 min;
  qma_uint8 second;
  qma_uint8 pad = 0;
  qma_uint16 ticks; // msecs * 10;

  time_t* lptime = &ptime;
  mytime = *gmtime(lptime);

  if(!true)
  {
     g_log << "-- Packet time_t (local): " << ctime(lptime);
     g_log << "--- Packet tm    (gmt)  : " << asctime(&mytime);
  }
  year = mytime.tm_year + 1900;
  day  = mytime.tm_yday +1;
  hour = mytime.tm_hour;
  min  = mytime.tm_min;
  second = mytime.tm_sec;
  int tmp = tt.tv_usec/100; //int division drops fractional part
  ticks = (qma_uint16) tmp;

  memcpy((char*)&btime[0],&year,2);
  memcpy((char*)&btime[2],&day,2);
  memcpy((char*)&btime[4],&hour,1);
  memcpy((char*)&btime[5],&min,1);
  memcpy((char*)&btime[6],&second,1);
  memcpy((char*)&btime[7],&pad,1);
  memcpy((char*)&btime[8],&ticks,2);
  return;
}

qma_uint8 TimeServer::getUsecsFromTimeval(const timeval atval)
{
  int tmp = atval.tv_usec/100; //int division drops fractional part
  tmp = tmp * 100;
  int ival = atval.tv_usec - tmp;
  qma_uint8 retVal = (qma_uint8) ival;
  return retVal;
}

BTI TimeServer::addSamplesToBTI(const int    numSamples,
                                const double freqHertz,
                                const BTI&   btime)
{

  BTI retBTI = btime;
  double freq = 1.0/freqHertz;
  double usecincrease = (((double)numSamples * freq)*ONE_MILLION);
  qma_uint32 intusecincrease = (qma_uint32) usecincrease;


  bool done = false;
  qma_uint32 intsecincrease = 0;

  intusecincrease = intusecincrease + btime.usec_offset;

  while(!done)
  {
    if(intusecincrease >= ONE_MILLION_INTEGER)
    {
      intusecincrease = intusecincrease - ONE_MILLION_INTEGER;
      ++intsecincrease;
    }
    else
    {
     done = true;
    }  
  }
  retBTI.sec_offset = retBTI.sec_offset + intsecincrease;
  retBTI.usec_offset = intusecincrease;
  return retBTI;
}   
