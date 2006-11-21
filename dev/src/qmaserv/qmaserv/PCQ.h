/*
 * File     :
 *   Packet Compression Queue
 *
 * Purpose  :
 *  This is the program that returns the compressed packets.
 *  Once returned, the packets are handed off to comserv.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  25 May 2002
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
#ifndef PCQ_H
#define PCQ_H
#include "LCQVO.h"
#include "SecondOfData.h"

#ifdef Q3302EW
extern "C" {
# include "trace_buf.h"
};
#endif

class PCQ
{
  public:

    PCQ();
    ~PCQ();

    bool  setLCQVO(LCQVO vo);
    LCQVO getLCQVO() const;

    //
    // Establish ID for this channel. All channel info is set with
    // initialize, so we provide only a get.
    //
    bool addDataToQueue(SecondOfData& ds);    

    //
    // Reset the queue indicating there was no previous sample;
    //
    void resetQueue();

    //
    // Here are routines to check if a packet is ready,
    // then get it if it is. 
    // Also, if there is a time gap, you can get the system
    // to packetize what it has, regardless of whether its a
    // a full packet or not. This would be done in the case of
    // a time gap, or if the min_samples per packet was met.
    //

    bool  packetReady();
    bool  packetRemaining();
    bool  compressPacket();
    char* getPacket();
    int   getSecondsInList();

  private:

    int                     p_seconds_to_packetize;
    LCQVO                   p_lcqvo;
    SecondOfData 	    p_list[MAX_SECONDS_IN_COMPRESSION_QUEUE]; 
                                      /* See QmaLimits.h for sizing info */
    int                     p_seconds_in_list;
#ifdef Q3302EW
    TracePacket             *p_ewtrace_packet;
#else 
    char   		    p_miniseed_packet[512];
#endif
    qma_int32               p_previous_last_sample_in_packet;
    qma_uint16		    p_seqno;
};

#endif
