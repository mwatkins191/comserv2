/*
 * File     :
 *   ProcessDCComp.C
 *
 * Purpose  :
 *   Process a DCComp packet
 *
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *  8 June 2002K
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
#include <string>
#include "ProcessDCComp.h"

#include "QmaTypes.h"
#include "QmaLimits.h"
#include "QmaDiag.h"
#include "global.h"
#include "msgs.h"
#include "Blockette.h"
#include "Verbose.h"
#include "qmaswap.h"

extern Verbose g_verbosity;

qma_uint16 processDCComp(const int       curposition,
		         const qma_uint8 chan,
	                 dt_data&        dt,
			 const BTI&	 curtime)
{
  //
  // variable length find length here
  //
  qma_uint8 freq;
  qma_uint16 len = 0;
  
  qma_uint8 chanindex = (chan & 0x07);

  memcpy((char*)&freq,dt.getOffsetBitString(curposition+1),1);

  int Queuepos = g_digMap_list.retrieveQueuePosition(chanindex,freq);
  if(Queuepos >= g_number_of_diglcqs)
  {
    g_log << "xxx Unable to find Logical Queue " << 
		((qma_uint16) chanindex) << " " << 
		((qma_uint16) freq) << std::endl;
    return len;
  }
  else
  {
   
    Blockette curBlock;
    QMABLOCK  dcblock;

    //
    // Fill in fixed values in dcblock for dc_comp blockette
    //
    dcblock.blocketteType = (qma_uint32) DC_COMP;
    dcblock.numberOfSamples = 
       g_digLCQ_list[Queuepos].getLCQVO().getSamplesPerBlockette();
    dcblock.segmentNumber = 0;
    dcblock.finalSegment = true;
    
    //
    // Now extra info for blockette structure from blockette
    // Find blockette info : previous sample and data offset
    //

    qma_int32  prevsample;
    qma_uint16 dataoffset;
    memcpy((char*)&len,dt.getOffsetBitString(curposition+2),2);
    memcpy((char*)&prevsample,dt.getOffsetBitString(curposition+4),4);
    memcpy((char*)&dataoffset,dt.getOffsetBitString(curposition+8),2);
    len = qma_ntohs(len);
    prevsample = qma_ntohl(prevsample);
    dataoffset = qma_ntohs(dataoffset);

    int flaglen = dataoffset - 10; // 10 is number of bytes to start of
				   // flags after start of packet
    int datalen = len - dataoffset;
    if((flaglen < 0) || (datalen < 0))
    {
      g_log << "xxx Data or Flag length found to be less than 0" 
	<< std::endl;
    }
   
    //
    // Fill in map length, and map data
    //
    dcblock.mapLengthInBytes = flaglen;
    memcpy((char*)&dcblock.map[0],
	   dt.getOffsetBitString(curposition+10),flaglen);

    //
    // Fill in data length, and data values
    //
    dcblock.dataLengthInBytes = datalen;
    memcpy((char*)&dcblock.w[0],
       dt.getOffsetBitString(curposition+dataoffset),datalen);

    //
    // The following statements indicate how data length and offset 
    // are related, and how length of flag values are derived.
    // len = offset + datalen ;
    // flaglen = offset - fixed;
    // 
    if(g_verbosity.show(D_MINOR,D_COMP_PACKET))
    {
      g_log << "--- Comp Packet Length : " << len << std::endl;
      g_log << "--- Data Offset length : " << dataoffset << std::endl;
      g_log << "--- Data  length       : " << 
	len - dataoffset << std::endl;
      g_log << "--- Map   length       : " << dataoffset - 10 << std::endl;
      g_log << "--- Last sample        : " << prevsample << std::endl;
    }

    //
    // Construct the decompression data
    // 
    dcblock.previousSample = prevsample;
    curBlock.setBlockette(dcblock);
    curBlock.setBlocketteTime(curtime);
    if(!g_startingDRSNNeeded)
    {
      g_digLCQ_list[Queuepos].insertBlockette(curBlock);
    }
    if(g_verbosity.show(D_MINOR,D_COMP_PACKET))
    {
      g_log << "<-- DC_COMP Packet Queued - Chan : " << 
	g_digLCQ_list[Queuepos].getLCQVO().getSEEDName() <<  
      " Hertz : " << g_digLCQ_list[Queuepos].getLCQVO().getFrequencyHertz()
		<< " : " << curtime.drsn << std::endl;
     }
   }
   //
   // Return length of packet to allow parsing of next blockette
   //
   return len;
}
