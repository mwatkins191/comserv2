/*
 * File     :
 *  CreatePacket.h
 *
 * Purpose  :
 *   Create a packet to hand to comserv
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  14 July 2002
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
#ifndef CREATE_PACKET_H
#define CREATE_PACKET_H

#include "QmaLimits.h"
#include "QmaTypes.h"
#include "LCQVO.h"
#include "SecondOfData.h"
#include "qma_mseed.h"

#ifdef Q3302EW
extern "C" {
# include "trace_buf.h"
};
#endif

bool createPacket(const LCQVO& vo,
                  const int    seconds_to_packetize,
                  qma_int32&   p_previous_last_sample_in_packet,
                  int&         p_seconds_in_list,
                  qma_uint16&  p_seqno,
                  char         p_miniseed_packet[],
                  SecondOfData p_list[]);

#ifdef Q3302EW
bool createEWTracePacket(const LCQVO& vo,
                  const int    seconds_to_packetize,
                  qma_int32&   p_previous_last_sample_in_packet,
                  int&         p_seconds_in_list,
                  qma_uint16&  p_seqno,
		  TracePacket  *p_ewtrace_packet,
                  SecondOfData p_list[]);
#endif

void createHeader(const       LCQVO& l,
                              Blockette& b,
                  const       int& p_seconds_to_packetize,
                  const       int& samplesInPacket,
                  const       int& framesInPacket,
                  qma_uint16& p_seqno,
                  char*       frame);


void getAllIntMap(int numberOfMaps,int* array);

void findFrameMapAndWord(const LCQVO& vo,
                         const int    currentWordInSecond,
                         const int    currentWordsInPacket,
                         qma_uint8    map[],
                         qma_uint16   mapLen,
                         int&         destFrameNumber,
                         int&         destWordPosition,
                         qma_uint32&  destMap);


bool packDC32(const LCQVO& vo,
              const int    seconds_to_packetize,
              qma_int32&   p_previous_last_sample_in_packet,
              int&         p_seconds_in_list,
              int&         SamplesInPacket,
              int&         FramesInPacket,
              qma_uint16&  p_seqno,
              char         p_miniseed_packet[],
              SecondOfData p_list[],
              qma_int32    mapArray[],
	      QMAMINISEED& packet);

bool packDCComp(const LCQVO& vo,
                const int    seconds_to_packetize,
                qma_int32&   p_previous_last_sample_in_packet,
                int&         p_seconds_in_list,
                int&         SamplesInPacket,
                int&         FramesInPacket,
                qma_uint16&  p_seqno,
                char         p_miniseed_packet[],
                SecondOfData p_list[],
                qma_int32    mapArray[],
	        QMAMINISEED& packet);

void packHeader(QMAMINISEED& packet,char header[],char p_miniseed_packet[]);


#endif
