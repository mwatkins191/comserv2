/*
 * File     :
 *  TimeServer.h
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
#ifndef TIMESERVER_H
#define TIMESERVER_H

#ifdef _WINNT
#include <time.h>
#else
#include <sys/time.h>
#endif

#include "QmaTypes.h"
#include "QmaLimits.h"
#include "BTI.h"


class TimeServer
{
  public: 
    TimeServer();
    ~TimeServer() {};

    //
    // Storage for global status items
    //
    void       setDRSN(qma_uint32 aDRSN); 
    void       setSecondsOffset(qma_uint32 so);
    void       setUsecsOffset(qma_uint32 useco);
    void       setMinutesSinceLoss(const qma_uint16 msl);
    qma_uint16 getMinutesSinceLoss() const;
    void       setClockQuality(const qma_uint16 cq);
    qma_uint16 getClockQuality() const;
    void       setCalibrationStatus(const qma_uint16 cs);
    qma_uint16 getCalibrationStatus() const;
    //
    // Returns string using current time set above.
    char*      currentSystemStatusTimeString();

    void setChan13FreqBitDelay(int freqbit,qma_int32 delay);
    void setChan46FreqBitDelay(int freqbit, qma_int32 delay);
    qma_int32 getChanFreqDelay(int chan, int freq);

    //
    // Convert Q330 time to more usual time formats
    //
    double    getRootTimeTag(qma_uint32 dsr,
			     qma_uint32 secOffset,
			     qma_uint32 usecOffset);

    double    getTimeTag(qma_uint32 dsr,int chan,int freq);

    double    getDigitizerTimeTag(qma_uint32 dsr,
				  qma_uint32 secOffset,
				  qma_uint32 usecOffset,
				  qma_int32  chanDelay);

    struct    timeval convertDouble2Timeval(double tt);
 
    struct timeval  convertBTI2Timeval(BTI digTime);

    void timeval2sdr(timeval& tt,char* btime);

    BTI  addSamplesToBTI(const int    numSamples,
                         const double sampFreq,
                         const BTI&   startTime);

    qma_uint8 getUsecsFromTimeval(const timeval atval);

  private:

    qma_uint32 p_drsn;
    qma_uint16 p_calibrationStatus;
    qma_uint16 p_minutesSinceLoss;
    qma_uint16 p_clockQuality;
    qma_uint32 p_seconds_offset;
    qma_uint32 p_usec_offset;
    qma_uint32 p_epoch_offset;  // seconds from 1970 to 2000
    //
    // Note these delays, in usec may be negative. This is one
    // of the few negative integer types used so far.
    //
    qma_int32  p_chan_freq_delay[MAX_CHANNELS][MAX_FREQS];     
    //
    // This size is based on the 26 byte ctime string.
    // An addition byte was added to allow for bracket.
    // Routine will remove terminate \n from ctime value.
    char       p_timeString[27];
};
#endif
