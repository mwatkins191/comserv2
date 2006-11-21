/*
 * File     :
 *   CountDownTimer.h
 *
 * Purpose  :
 *   This is a count down timer.
 *
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *  26 July 2002
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
#ifndef CountDownTimer_H
#define CountDownTimer_H

class CountDownTimer
{

  public:
    CountDownTimer();
    ~CountDownTimer() {};

    void setRetryInterval(long aTimerInterval);
    void setRetryCount(long aRetryLimit);

    //
    // Overload these methods. If we have several timers,
    // we can call the clock externally, and pass the results into
    // other timers to reduce the system call load.
    //
    
    void start();
    void stop();


    bool started();
    bool elapsed();
    bool retryLimitReached();
    void restartInterval();
    void restartCount();

    long secondsRemaining();
    long retriesRemaining();
    long getCurrentTime();

  private:

    bool p_started;
    long p_end_time;
    long p_seconds_in_interval;
    long p_retryCount;
    long p_currentCount;
};
#endif
