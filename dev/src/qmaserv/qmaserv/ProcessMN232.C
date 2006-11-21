/*
 * File     :
 *   processMN232.C
 *
 * Purpose  :
 *   Process an DC_MN232 packet
 *
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *  9 August 2003
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

#include "ProcessMN232.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "QmaDiag.h"
#include "global.h"
#include "DC.h"
#include "msgs.h"
#include "Blockette.h"
#include "ClockUtils.h"
#include "Verbose.h"
#include "qmaswap.h"

extern Verbose g_verbosity;

void processMN232(const int       curposition,
                  const qma_uint8 chan,
	          dt_data         &dt,
	          const BTI&      curtime)
{


  qma_uint8 clockqual = 0;
  qma_uint16 minslock = 0;
  qma_uint32 soff = 0;    // seconds offset
  qma_uint32 usoff = 0;   // usec offset

  if(!g_startingDRSNNeeded)
  {
    qma_uint8 mainType = (chan & 0x7f); // mask out the max bit
    mainType = mainType >> 3;  // Rotate right to get number 0-15
    qma_uint8 chanindex = (chan & 0x07);

    if(chanindex == 0)
    { 
   
      memcpy((char*)&soff,dt.getOffsetBitString(curposition+4),4);
      //soff = qma_ntohl(soff);

      memcpy((char*)&usoff,dt.getOffsetBitString(curposition+8),4);
      //usoff = qma_ntohl(usoff);

      memcpy((char*)&clockqual,dt.getOffsetBitString(curposition+1),1);

      memcpy((char*)&minslock,dt.getOffsetBitString(curposition+2),2);
      //minslock = qma_ntohs(minslock);
    }
    else if(chanindex == 1)
    {
       g_log << "--- Rx'd Clock phase error blockette without index 0: " << std::endl;
    }
    else if (chanindex == 2)
    {
       g_log << "xxx Rx'd Clock Quality blockette without index 0: " << std::endl;
    }
    else
    {
       g_log << "xxx Unknown main data channel in MN232 packet : " <<
                (qma_uint16) chanindex << std::endl;
    }

    qma_uint8 tqual = translate_clock(g_tvo.getClockProcVO(),
                                      clockqual,
                                      minslock);

    if(!true)
    {
      g_log << "--- Found clock quality: " << (qma_uint16) tqual 
         << std::endl;
    }
    //
    // Process ChanIndex 0
    //
    qma_uint8 queuepos = g_mainMap_list.retrieveQueuePosition(mainType,
                                                    chanindex,
                                                    0);
    if(queuepos <= g_number_of_mainlcqs)
    {
      g_log << "xxx Unexpected Queue found for chanindex 0 position 0" 
         << std::endl;  
    }
    
    //
    // Process ChanIndex 1
    //
    queuepos = g_mainMap_list.retrieveQueuePosition(mainType,
                                                    chanindex,
                                                    1);
    if(queuepos <= g_number_of_mainlcqs)
    {

        // Calculate the data

        if(usoff >=500000)
        {
           usoff = usoff - 1000000;
        }

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

        memcpy((char*)&dcblock.w[0],(char*)&usoff,4);
        //
        // Finally, data and time complete, insert them into blockette
        //    
        curBlock.setBlockette(dcblock);
        curBlock.setBlocketteTime(curtime);

        g_mainLCQ_list[queuepos].insertBlockette(curBlock);
//        g_log << "--- MN232 index0 position 1 gives channel - sb LCE: " <<
//		g_mainLCQ_list[queuepos].getLCQVO().getSEEDName() << std::endl;
    }
//    else
//    {
//       g_log << "--- No queue found for chanindex 1 position 0" 
//         << std::endl;  
//    }


    queuepos = g_mainMap_list.retrieveQueuePosition(mainType,
                                                    chanindex,
                                                    2);

    if(queuepos < g_number_of_mainlcqs)
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
        qma_uint32 intval = 0;
        intval = (qma_uint32) tqual;
        memcpy((char*)&dcblock.w[0],(char*)&intval,4);
        //
        // Finally, data and time complete, insert them into blockette
        //    
        curBlock.setBlockette(dcblock);
        curBlock.setBlocketteTime(curtime);

        g_mainLCQ_list[queuepos].insertBlockette(curBlock);
//        g_log << "--- MN232 index0 position 2 gives channel- sb LCQ : " <<
//		g_mainLCQ_list[queuepos].getLCQVO().getSEEDName() << std::endl;

    }
//    else
//    {
//       g_log << "--- No queue found for chanindex 0 position 2" 
//         << std::endl;  
//    }


    queuepos = g_mainMap_list.retrieveQueuePosition(mainType,
                                                    chanindex,
                                                    3);

    if(queuepos < g_number_of_mainlcqs)
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

        qma_uint32 uintval = (qma_uint32) minslock;
        memcpy((char*)&dcblock.w[0],(char*)&uintval,4);
        //
        // Finally, data and time complete, insert them into blockette
        //    
        curBlock.setBlockette(dcblock);
        curBlock.setBlocketteTime(curtime);

        g_mainLCQ_list[queuepos].insertBlockette(curBlock);
//        g_log << "--- MN232 index0 position 1 gives channel-sb LCQ: " <<
//		g_mainLCQ_list[queuepos].getLCQVO().getSEEDName() << std::endl;

    }
//    else
//    {
//       g_log << "--- No queue found for chanindex 0 position 3" 
//         << std::endl;  
//    }

  }
}
