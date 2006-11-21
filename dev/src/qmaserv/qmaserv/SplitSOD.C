/*
 * File     :
 *  SplitSOD.C
 *
 * Purpose  :
 *   Split a Second of data containing more than a packet of
 *   data into two SODs. The second one will be flagged as
 *   continution packet.
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
#include <iostream>
#include <string>
#include "SplitSOD.h"
#include "QmaLimits.h"
#include "QmaTypes.h"
#include "CharUtils.h"
#include "Cleanup.h"
#include "UnpackComp.h"
#include "TimeServer.h"
#include "FrameUtils.h"
#include "global.h"

SecondOfData splitSOD(double freqHertz,
                      SecondOfData& sod)
{
  int currentWordsInPacket = 0;
  int currentWordsInFrame = 0;
  int totalWordsInPacket = 0;
  SecondOfData tempSod = sod;
  SecondOfData secondsod = sod;
  secondsod.setCompletionPacket(false);

  totalWordsInPacket =  sod.getNumberOfDataWords();
 
  if(totalWordsInPacket <= MAX_WORDS_IN_PACKET)
  {
    g_log << 
      "xxx Called splitSOD with less 103 data words." << 
        std::endl;
    return secondsod;
  }

  qma_int32 packetType = sod.getBlocketteType();
  if(packetType == DC_MULT)
  {
    g_log << "xxx Called Split with DC_MULT: " << std::endl;
    return secondsod;
  }

  qma_int32 diffbuf[532],*diffptr;
  qma_int32 databuf[532],*dataptr;
  diffptr = diffbuf;
  dataptr = databuf;

  //
  // Find the data point at which the SOD exceeds
  // the 103 data words allowed in a packet.
  //

  int startSecondData = 0;
  int totalWordsInSecond = sod.getNumberOfDataWords();	
  int currentWordsInSecond = 0;
  QMABLOCK dblock;
  qma_int32 x0,xn;
  int num;

  if (packetType == DC_COMP)
  {
    dblock = sod.p_blockette_array[0].getBlockette();

    // Model is that this returns the number of samples that starts the
    // the first sample of the
    // in the continuation packet

    startSecondData = 
      positionOfCompWord(100,
                         &dblock,
                         diffptr,
                         dataptr);

    if( (startSecondData <= 0) || 
        (startSecondData >= dblock.numberOfSamples))
    {
      g_log << "xxx Invalid (too high or low) sample Number in SplitSOD: " 
	<< startSecondData << std::endl;
      return secondsod;
    }

    num = sod.p_blockette_array[0].getBlockette().numberOfSamples;

    if(!true)
    {
      g_log << "--- Found startSecond as: " << startSecondData << 
        " with number of data words: " << totalWordsInSecond << 
        " and numberOfSamples: " << num << std::endl;
    }

    dblock = sod.p_blockette_array[0].getBlockette();
    int res = unpack_DCComp(&dblock,
                             diffptr,
                             dataptr);
    if(res > 0)
    {
      x0 = databuf[0];
      xn = databuf[(num-1)]; /* -1 give position of last sample */
    }
    else
    {
      g_log << "xxx Error unpacking blockette" << std::endl;
    }
  }
  else
  {
    g_log << 
      "xxx Called splitSOD with invalid packet type of: " 
        << packetType << std::endl;
    return secondsod;
  }

  //
  // We have determined the data we need to split the packet.
  // Leave the first 100 data words in the first packet, and
  // copy the remainder to the second packet.
  // Then update the fields in SOD so it can be packetized.
  //

  //
  // Checks before we begin. 
  // mapLengthInBytes *4 should be equal to the number
  // of data words. This is because each MapByte gives the maps for 
  // 4 data words.
  //
  if((sod.p_blockette_array[0].p_blockette.mapLengthInBytes*4) < 
      MAX_WORDS_IN_PACKET)
  {
    g_log << "xxx Invalid length for map words in Split: " <<
      sod.p_blockette_array[0].p_blockette.mapLengthInBytes << std::endl;
  }
  else
  {
    if(!true)
    {
      g_log << "--- Starting length for map words in Split: " <<
      sod.p_blockette_array[0].p_blockette.mapLengthInBytes << std::endl;
    }
  }

  // Change the original blockette to new status

  sod.p_blockette_array[0].p_blockette.blocketteType = DC_COMP;
  sod.p_blockette_array[0].p_blockette.numberOfSamples = startSecondData;
  sod.p_blockette_array[0].p_blockette.segmentNumber = 0;
  sod.p_blockette_array[0].p_blockette.finalSegment = true;
  sod.p_blockette_array[0].p_blockette.mapLengthInBytes = 25; 
   // 25 is an byte aligned number of maps which will give us
   // 100 data words.

  sod.p_blockette_array[0].p_blockette.dataLengthInBytes = 400;

  sod.setNumberOfDataWords(100);

  //
  // Update the fields in Completion pacaket.
  //

  secondsod.setCompletionPacket(true);
  secondsod.p_blockette_array[0].p_blockette.blocketteType = DC_COMP;
  secondsod.p_blockette_array[0].p_blockette.numberOfSamples = 
    (num - startSecondData);
  secondsod.p_blockette_array[0].p_blockette.segmentNumber = 0;
  secondsod.p_blockette_array[0].p_blockette.finalSegment = true;

  secondsod.p_blockette_array[0].p_blockette.mapLengthInBytes = 
    secondsod.p_blockette_array[0].p_blockette.mapLengthInBytes - 25;

  int mblen = secondsod.p_blockette_array[0].p_blockette.mapLengthInBytes;

  if(!true)
  {
  g_log << "--- Set mapWordLength in completion packet as: " <<
    mblen << " with samples: " << 
	secondsod.p_blockette_array[0].getBlockette().numberOfSamples << 
	std::endl;
  }
  memcpy((char*)&secondsod.p_blockette_array[0].p_blockette.map[0],
    (char*)&tempSod.p_blockette_array[0].p_blockette.map[25],mblen);

  //
  // Now update the data length
  secondsod.p_blockette_array[0].p_blockette.dataLengthInBytes = 
    secondsod.p_blockette_array[0].p_blockette.dataLengthInBytes - 400;
  secondsod.p_blockette_array[0].p_blockette.previousSample =
    databuf[startSecondData-1]; 

  int blen = secondsod.p_blockette_array[0].p_blockette.dataLengthInBytes;
  int wlen = blen/4;
  if (blen%4 != 0)
  {
    g_log << 
      "xxx Invalid num of dataWords in completion packet - bytelen: "
              << blen << " with words: " << wlen << std::endl;
  }

  secondsod.setNumberOfDataWords(wlen);

  memcpy((char*)&secondsod.p_blockette_array[0].p_blockette.w[0],
    (char*)&tempSod.p_blockette_array[0].p_blockette.w[100],blen);

  //
  // Assign new start time, and new last sample
  //
  
  BTI newStamp = 
    g_timeServer.addSamplesToBTI(startSecondData,
                                 freqHertz,
                         secondsod.p_blockette_array[0].getBlocketteTime());

  secondsod.p_blockette_array[0].setBlocketteTime(newStamp);

  return secondsod;
}
