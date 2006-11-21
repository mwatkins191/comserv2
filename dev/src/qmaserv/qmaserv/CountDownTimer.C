/*
 * File     :
 *   CountDownTimer.C
 *
 * Purpose  :
 *   This counts down and will indicate expired if time is up.
 *
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *  23 July 2002
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
#include "CountDownTimer.h"
#include "global.h"

CountDownTimer::CountDownTimer()
{
  p_started = false;
  p_end_time = 0;
  p_seconds_in_interval = 0;
  p_retryCount = 0;
  p_currentCount = 0;
}

void CountDownTimer::start()
{
  p_started = true;
  restartInterval();
  restartCount();
}

void CountDownTimer::stop()
{
  p_started = false;
}

bool CountDownTimer::started()
{
  return p_started;
}


void CountDownTimer::setRetryInterval(long secs)
{
  p_seconds_in_interval = secs;
  long curTime = getCurrentTime();
  p_end_time = curTime + p_seconds_in_interval; 
}

void CountDownTimer::setRetryCount(long aRetryNumber)
{
  p_retryCount = aRetryNumber;
  p_currentCount = p_currentCount;
}

void CountDownTimer::restartInterval()
{
  long curTime = getCurrentTime();
  p_end_time = curTime + p_seconds_in_interval;
}

void CountDownTimer::restartCount()
{
  p_currentCount = p_retryCount;
}


long CountDownTimer::secondsRemaining()
{
  long seconds_remaining = -1;
  long curTime = getCurrentTime();
  if(curTime > p_end_time)
  {
    seconds_remaining = 0;
  }
  else
  {
    seconds_remaining = p_end_time - curTime;
  }
  return seconds_remaining;
}


bool CountDownTimer::elapsed()
{
  bool res;
  long curTime = getCurrentTime();
  if(curTime > p_end_time)
  {
    --p_currentCount;
    res = true;
  }
  else
  {
    res = false;
  }
  return res;
}

bool CountDownTimer::retryLimitReached()
{

  if(p_currentCount < 1)
  {
    return true;
  }
  else
  {
    return false;
  }
}

long CountDownTimer::getCurrentTime()
{
  long retSecs = 0;

#ifdef WIN32
  // this seems logical, is there a reason for doing it the roundabout way?
  retSecs = time(NULL);
#else
# if defined(LINUX)
  struct timezone *ptr = NULL;
# else
  long *ptr = NULL;
# endif

  struct timeval t;

# if defined(LINUX)
  int res = gettimeofday(&t, ptr);
# else
  int res = gettimeofday(&t,(void *) ptr);
# endif


 if(res !=0)
 {
    g_log << "xxx Unable to get time of day" << std::endl;
 }
 else
 {
     retSecs = t.tv_sec;
 }
#endif
 return retSecs;
}
