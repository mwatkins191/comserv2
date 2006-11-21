/*
 * File     :
 *   processDC32.C
 *
 * Purpose  :
 *   Process a DC32 packet
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

#include "ProcessDCD32.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "QmaDiag.h"
#include "global.h"
#include "DC.h"
#include "msgs.h"
#include "Blockette.h"
#include "Verbose.h"

extern Verbose g_verbosity;

void processDCD32(const int       curposition,
                  const qma_uint8 chan,
	          dt_data         &dt,
		  const BTI&      curtime)
{

  if(!g_startingDRSNNeeded)
  {
  //
  // It appears that in the LCQ tokens,
  // 1Hz data is given as freq bit 0. But in the blockettes
  // 1Hz data is given as freq bit 1. 
  //
  qma_uint8 chanindex = (chan & 0x07);
  qma_uint8 freq;

  memcpy((char*)&freq,dt.getOffsetBitString(curposition+1),1);
  //
  // Here's the adjustment to adjust for the diff uses of freq bits.
  // that freq bits are different in tokens versus blockettes
  //
  if(freq == 1)
  {
    freq = 0;
  }
  else
  {
    //
    // Some references in the DP writes guide suggest that DCD32 data is
    // only used for 1 HRZ data. Assume this is true. If not, print a
    // error message.
    //
    g_log << "xxx Found DC without 1Hz data type" << std::endl;
  }

  int Queuepos = g_digMap_list.retrieveQueuePosition(chanindex,freq);

  if(Queuepos >= g_number_of_diglcqs)
  {
    g_log << "xxx Unable to find Logical Queue " << 
      ((qma_int16) chanindex) << " " << ((qma_int16) freq) << 
	" QueuePosition " << Queuepos << " dig_lcqs " << 
          g_number_of_diglcqs << std::endl;
  }
  else
  {
    Blockette curBlock;
    QMABLOCK  dcblock;

    //
    // Fill in fixed values in dcblock for dc_comp blockette
    //
    dcblock.blocketteType = (qma_uint32) DC_D32;
    dcblock.numberOfSamples = 
       g_digLCQ_list[Queuepos].getLCQVO().getSamplesPerBlockette();
    //
    // Some values in a QMABLOCK are not valid for a DC_D32 blockette
    // Fill them in with placeholder, default values
    //
    dcblock.previousSample = 0;
    dcblock.segmentNumber = 0;
    dcblock.finalSegment = true;
    dcblock.mapLengthInBytes = 0;
    dcblock.dataLengthInBytes = 4;
    //
    // copy data into first word of data structure
    //
    memcpy((char*)&dcblock.w[0],(char*)dt.getOffsetBitString(curposition+4),4);

    //
    // Finally, data and time complete, insert them into blockette
    //    
    curBlock.setBlockette(dcblock);
    curBlock.setBlocketteTime(curtime);

    g_digLCQ_list[Queuepos].insertBlockette(curBlock);
    if(g_verbosity.show(D_MINOR,D_LCQ))
    {
      g_log << "<-- DC_D32 - Chan : " << 
	g_digLCQ_list[Queuepos].getLCQVO().getSEEDName() << 
	" Hertz : " << g_digLCQ_list[Queuepos].getLCQVO().getFrequencyHertz() 
		<< " : " << curtime.drsn << 
	    " data value: " << dcblock.w[0] << std::endl;
    }
  }
  }
}
