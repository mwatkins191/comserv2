/*
 * File     :
 *   ProcessDCMult.C
 *
 * Purpose  :
 *   Process a DCMult packet
 *
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *  8 June 2002
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
#include "ProcessDCMult.h"

#include "QmaTypes.h"
#include "QmaLimits.h"
#include "QmaDiag.h"
#include "global.h"
#include "msgs.h"
#include "Blockette.h"
#include "Verbose.h"

extern Verbose g_verbosity;

qma_uint16 processDCMult(const int       curposition,
		         const qma_uint8 chan,
	                 dt_data&        dt,
			 const BTI&      curtime)
{
  qma_uint8 chanindex = (chan & 0x07);
  qma_uint8 freqseg;
  qma_uint8 freq;
  qma_uint8 seg;
  qma_uint16 len = 0;

  //
  // Find the frequency and segment numbers. They share a byte in
  // this message.
  //

  memcpy((char*)&freqseg,dt.getOffsetBitString(curposition+1),1);
  freq = freqseg & 0x07;
  seg  = freqseg & 0xf8;
  seg = seg >> 3; // Rotate the segment number into the lsb bit position.

  //
  // Get the length, and last segment flag from the same short int.
  //

  memcpy((char*)&len,dt.getOffsetBitString(curposition+2),2);
  qma_uint16 lastb;
  lastb = len & 0x8000;
  len = len & DMSZ; // mask out last seg bit.


  //
  // All of the data fields have been found in the message. Summarize
  // them then find the LCQ, then construct a QMBLOCK and insert it
  // into the Queue.
  //
  //

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
    // Fill in fixed values in dcblock for dc_mult blockette
    //
    dcblock.blocketteType = (qma_uint32) DC_MULT;
    dcblock.numberOfSamples =
       g_digLCQ_list[Queuepos].getLCQVO().getSamplesPerBlockette();
    dcblock.segmentNumber = seg;
    if(lastb == 0)
    {
      if(g_verbosity.show(D_EVERYTHING,D_MULTS))
      {
        g_log << "--- Rx'd Mult Segnum: " << (qma_uint16) seg << 
          " not final" << std::endl;
      }
      dcblock.finalSegment = false;
    }
    else
    {
      if(g_verbosity.show(D_EVERYTHING,D_MULTS))
      {
        g_log << "--- Tx'd Mult Segnum: " << (qma_uint16) seg << 
           " final" << std::endl;
      }
      dcblock.finalSegment = true;
    }
    
    //
    // The blockette format differs. Segment 0 has a map and
    // a DC_Comp format. Segment 1 and up has a data only format.
    //
    //
    
    if(seg == 0)
    {
      //
      // Now extra info for blockette structure from blockette
      // Find blockette info : previous sample and data offset
      // len was read above with segment number
      //

      qma_int32  prevsample;
      qma_uint16 dataoffset;

      memcpy((char*)&prevsample,dt.getOffsetBitString(curposition+4),4);
      memcpy((char*)&dataoffset,dt.getOffsetBitString(curposition+8),2);
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
      // Construct the decompression data
      // 
      dcblock.previousSample = prevsample;
    }
    else
    {
      dcblock.mapLengthInBytes = 0;
      dcblock.dataLengthInBytes = len - 4; // 4 is the 4 bytes which contains 
                                           // chan/seg/freq/size/lastsegflag
      memcpy((char*)&dcblock.w[0],
         dt.getOffsetBitString(curposition+4),dcblock.dataLengthInBytes);

    }

    //
    // Regardless of segment number, dcblock is now complete.
    // Add it to the LCQ
    //

    curBlock.setBlockette(dcblock);
    curBlock.setBlocketteTime(curtime);
    if(!g_startingDRSNNeeded)
    {
      g_digLCQ_list[Queuepos].insertBlockette(curBlock);
    }
    if(g_verbosity.show(D_EVERYTHING,D_MULTS))
    {
      g_log << "--- Processed DC_MULT Packet: " << 
	g_digLCQ_list[Queuepos].getLCQVO().getSEEDName() <<  
      " Hertz : " << g_digLCQ_list[Queuepos].getLCQVO().getSamplesPerBlockette()
	<< " : " << curtime.drsn << std::endl;
      g_log << "--- Segment Number: " 
        << ((qma_uint16) dcblock.segmentNumber) << std::endl;
      g_log << "--- Last Segment Flag : " 
        << (qma_uint16) dcblock.finalSegment << std::endl;
      g_log << "--- Data Length : " << dcblock.dataLengthInBytes
        << std::endl;
      g_log << "--- Map Length : " << dcblock.mapLengthInBytes 
        << std::endl;
    }
  }
  //
  // Return length of packet to allow parsing of next blockette
  //
  return len;
}
