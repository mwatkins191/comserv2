/*
 * File     :
 *   CheckPCQS.C
 *
 * Purpose  :
 *  This is the routines which checks to see if packet queues
 *   have current data in them, and if so, compresss and send it.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   26 May 2002
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
 *
 */
#include <iostream>
#include <stdio.h>
#include "QmaTypes.h"
#include "QmaDiag.h"
#include "CheckPCQS.h"
#include "global.h"
#include "PCQ.h"

#ifndef Q3302EW
#  include "clink.h"
#else
# include "externs.h"
extern "C" {
# include "transport.h"
};

#endif

#include "Verbose.h"

extern Verbose g_verbosity;

bool check_pcqs()
{
  if(g_outputPacketsQueued)
  {
    //
    // Walk through the digPCSs
    //  
    bool packetFound = true;
    while(packetFound)
    {
      packetFound = false;
      for(int i=0;i<g_number_of_diglcqs;i++)
      {
         if(g_digPCQ_list[i].packetReady())
         {
           g_digPCQ_list[i].compressPacket();
           char* pkt_ptr = g_digPCQ_list[i].getPacket();
#ifdef Q3302EW
	   int msgSize = (((TRACE_HEADER *)pkt_ptr)->nsamp * sizeof(qma_uint32)) + sizeof(TRACE_HEADER);
	   time_t msgTime = (time_t) ((TRACE_HEADER *)pkt_ptr)->starttime;
	   //fprintf(stderr, "MsgSize: %d.  Time: %s\n", msgSize, ctime(&msgTime));
	   if(tport_putmsg(&Region, &DataLogo, (long)msgSize, pkt_ptr) != PUT_OK) {
	     fprintf(stderr, "Error sending packet to EW\n");
	   }
	   if(g_verbosity.show(D_MINOR,D_COMSERV))
	   {
	     g_log << "--- Data packet sent to earthworm : " 
		<< g_digPCQ_list[i].getLCQVO().getSEEDName() << std::endl;
	   }
#else
           comlink_send(pkt_ptr,
		      BYTES_IN_MINISEED_PACKET,
		      DATA_PACKET);
	   if(g_verbosity.show(D_MINOR,D_COMSERV))
	   {
	     g_log << "--- Data packet sent to comserv : " 
		<< g_digPCQ_list[i].getLCQVO().getSEEDName() << std::endl;
	   }
#endif
           packetFound = true;
	 }
      }
    }

    //
    // Now work through the mainPCQS
    //
    packetFound = true;
    while(packetFound)
    {
      packetFound = false;
      for(int i=0;i<g_number_of_mainlcqs;i++)
      {
         if(g_mainPCQ_list[i].packetReady())
         {
           g_mainPCQ_list[i].compressPacket();
           char* pkt_ptr = g_mainPCQ_list[i].getPacket();

#ifdef Q3302EW
	   int msgSize = (((TRACE_HEADER *)pkt_ptr)->nsamp * sizeof(qma_uint32)) + sizeof(TRACE_HEADER);
	   time_t msgTime = (time_t) ((TRACE_HEADER *)pkt_ptr)->starttime;
	   //fprintf(stderr, "MsgSize: %d.  Time: %s\n", msgSize, ctime(&msgTime));
	   if(tport_putmsg(&Region, &DataLogo, (long)msgSize, pkt_ptr) != PUT_OK) {
	     fprintf(stderr, "Error sending packet to EW\n");
	   }
	   if(g_verbosity.show(D_MINOR,D_COMSERV))
	   {
	     g_log << "--- Data packet sent to earthworm : " 
		<< g_digPCQ_list[i].getLCQVO().getSEEDName() << std::endl;
	   }
#else
           comlink_send(pkt_ptr,
		      BYTES_IN_MINISEED_PACKET,
		      DATA_PACKET);
	   if(g_verbosity.show(D_MINOR,D_COMSERV))
	   {
	     g_log << "--- Data packet sent to comserv : " 
		<< g_digPCQ_list[i].getLCQVO().getSEEDName() << std::endl;
	   }
#endif      

           packetFound = true;
	 }
      }
    }
  }
  g_outputPacketsQueued = false;
  return true;
}


void empty_pcqs()
{
  bool res = check_pcqs();
  bool packetFound = true;
  while(packetFound)
  {
      packetFound = false;
      for(int i=0;i<g_number_of_diglcqs;i++)
      {
         if(g_digPCQ_list[i].packetRemaining())
         {
           g_digPCQ_list[i].compressPacket();
           char* pkt_ptr = g_digPCQ_list[i].getPacket();
#ifdef Q3302EW
	   int msgSize = (((TRACE_HEADER *)pkt_ptr)->nsamp * sizeof(qma_uint32)) + sizeof(TRACE_HEADER);
	   if(tport_putmsg(&Region, &DataLogo, (long)msgSize, pkt_ptr) != PUT_OK) {
	     fprintf(stderr, "Error sending packet to EW\n");
	   }
	   if(g_verbosity.show(D_MINOR,D_PARTIALPACKETS))
	   {
	     g_log << "--- Partial Data packet sent to earthworm : " 
		<< g_digPCQ_list[i].getLCQVO().getSEEDName() << std::endl;
	   }
#else
           comlink_send(pkt_ptr,
		      BYTES_IN_MINISEED_PACKET,
		      DATA_PACKET);
	   if(g_verbosity.show(D_MINOR,D_PARTIALPACKETS))
	   {
	     g_log << "--- Partial Data packet sent to comserv : " 
		<< g_digPCQ_list[i].getLCQVO().getSEEDName() << std::endl;
	   }
#endif
           packetFound = true;
	 }
      }
  }

  //
  // Now write out the mainPCQs
  //
  packetFound = true;
  while(packetFound)
  {
      packetFound = false;
      for(int i=0;i<g_number_of_mainlcqs;i++)
      {
         if(g_mainPCQ_list[i].packetRemaining())
         {
           g_mainPCQ_list[i].compressPacket();
           char* pkt_ptr = g_mainPCQ_list[i].getPacket();

#ifdef Q3302EW
	   int msgSize = (((TRACE_HEADER *)pkt_ptr)->nsamp * sizeof(qma_uint32)) + sizeof(TRACE_HEADER);
	   if(tport_putmsg(&Region, &DataLogo, (long)msgSize, pkt_ptr) != PUT_OK) {
	     fprintf(stderr, "Error sending packet to EW\n");
	   }
	   if(g_verbosity.show(D_MINOR,D_PARTIALPACKETS))
	   {
	     g_log << "--- Partial Data packet sent to earthworm : " 
		<< g_digPCQ_list[i].getLCQVO().getSEEDName() << std::endl;
	   }
#else
           comlink_send(pkt_ptr,
		      BYTES_IN_MINISEED_PACKET,
		      DATA_PACKET);
	   if(g_verbosity.show(D_MINOR,D_PARTIALPACKETS))
	   {
	     g_log << "--- Partial Data packet sent to comserv : " 
		<< g_digPCQ_list[i].getLCQVO().getSEEDName() << std::endl;
	   }
#endif
           packetFound = true;
	 }
      }
  }
}
