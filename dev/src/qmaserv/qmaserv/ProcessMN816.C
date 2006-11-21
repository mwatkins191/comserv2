/*
 * File     :
 *   processMN816.C
 *
 * Purpose  :
 *   Process an DC_MN 8 - 16 packet
 *
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *  3 August 2003
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

#include "ProcessMN816.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "QmaDiag.h"
#include "global.h"
#include "DC.h"
#include "msgs.h"
#include "Blockette.h"
#include "Verbose.h"

extern Verbose g_verbosity;

void processMN816(const int       curposition,
                 const qma_uint8 chan,
	         dt_data         &dt,
	         const BTI&      curtime)
{

  if(!g_startingDRSNNeeded)
  {
    qma_uint8 mainType = (chan & 0x7f); // mask out the max bit
    mainType = mainType >> 3;  // Rotate right to get number 0-15
    qma_uint8 chanindex = (chan & 0x07);

    if(chanindex == 0)
    {
    //
    // Process First byte (16 bit byte) in packet
    //
    qma_uint8 queuepos = g_mainMap_list.retrieveQueuePosition(mainType,
                                                    chanindex,
                                                    0);

    if(queuepos >= g_number_of_mainlcqs)
    {
       return;
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
        g_mainLCQ_list[queuepos].getLCQVO().getSamplesPerBlockette();
      //
      // Some values in a QMABLOCK are not valid for a DC_D32 blockette
      // Fill them in with placeholder, default values
      //
      dcblock.previousSample = 0;
      dcblock.segmentNumber = 0;
      dcblock.finalSegment = true;
      dcblock.mapLengthInBytes = 0;
      dcblock.dataLengthInBytes = 4;
      qma_uint16 curval16 = 0;
      qma_uint32 uintval = 0;
      //
      // copy data into first word of data structure
      //
      memcpy((char*)&curval16,(char*)dt.getOffsetBitString(curposition+2),2);
      uintval = (qma_uint32) curval16;
      memcpy((char*)&dcblock.w[0],(char*)&uintval,4);
      //
      // Finally, data and time complete, insert them into blockette
      //    
      curBlock.setBlockette(dcblock);
      curBlock.setBlocketteTime(curtime);

      g_mainLCQ_list[queuepos].insertBlockette(curBlock);
    }
  }
  else if(chanindex == 1)
  {
    //
    // Process First byte (16 bit byte) in packet
    //
    qma_uint8 queuepos = g_mainMap_list.retrieveQueuePosition(mainType,
                                                    chanindex,
                                                    1);

    if(queuepos >= g_number_of_mainlcqs)
    {
       return;
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
        g_mainLCQ_list[queuepos].getLCQVO().getSamplesPerBlockette();
      //
      // Some values in a QMABLOCK are not valid for a DC_D32 blockette
      // Fill them in with placeholder, default values
      //
      dcblock.previousSample = 0;
      dcblock.segmentNumber = 0;
      dcblock.finalSegment = true;
      dcblock.mapLengthInBytes = 0;
      dcblock.dataLengthInBytes = 4;
      qma_uint16 curval16 = 0;
      qma_uint32 uintval = 0;
      //
      // copy data into first word of data structure
      //
      memcpy((char*)&curval16,(char*)dt.getOffsetBitString(curposition+2),2);
      uintval = (qma_uint32) curval16;
      memcpy((char*)&dcblock.w[0],(char*)&uintval,4);
      //
      // Finally, data and time complete, insert them into blockette
      //    
      curBlock.setBlockette(dcblock);
      curBlock.setBlocketteTime(curtime);

      g_mainLCQ_list[queuepos].insertBlockette(curBlock);
    }
  }
  else
  {
       g_log << "xxx Error. Invalid chanNum in Main 816 LCQ : " <<
                      chanindex << std::endl;
  }
 }
}
