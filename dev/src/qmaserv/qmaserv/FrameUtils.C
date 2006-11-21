/*
 * File     :
 *  FrameUtils.h
 *
 * Purpose  :
 *   Utility methods used to pack Miniseed frames
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
#include "FrameUtils.h"
#include "QmaLimits.h"
#include "QmaTypes.h"
#include "CharUtils.h"
#include "qma_mseed.h"
#include "Cleanup.h"
#include "UnpackComp.h"
#include "TimeServer.h"
#include "global.h"


void getAllIntMap(const int numberOfWords,qma_int32 *array)
{
  qma_uint32 word = 0;
  qma_uint32 mask;
  int i;

  //
  // seven mask in packet
  //
  for(i=0;i<7;i++)
  {
    array[i] = word;
  }

  int index = 0;

  for(i=0;i<numberOfWords;i++)
  {
    if(i==0)
    {
      mask = 0x02000000;
      index = 0;
    }
    else if( (i==13) || (i==28) || (i==43) || (i==58) || (i == 73) || (i==88))
    {
      mask = 0x20000000;
      ++index;
    }
    array[index] = array[index] | mask;
    mask = mask >> 2;
  }
}


void findFrameMapAndWord(const LCQVO& vo,
                         const int currentWordsInSecond,
			 const int currentWordsInPacket,
			 qma_uint8 map[],
			 qma_uint16 mapLen,
			 int& destFrameNumber,
			 int& destWordPosition,
			 qma_uint32& destMask)
{

  //
  // We need both the current words in second - to determine the
  // map needed from the current blockette, and also the
  // current words in packet, to determine the output positioning
  // of the map and data.
  //
  int mapByteNum = currentWordsInSecond/4; //Integer division. drop remainder
  if((mapByteNum > 49) || (mapByteNum > (mapLen-1)))
  {
      g_log << "xxx Error Invalid Map position : " << mapByteNum <<
	" with wordsInSecond : " << currentWordsInSecond << " mapLen : "
		<< mapLen << std::endl;
  }   
  else
  {
    if(!true)
    {
      g_log << "--- Found Map position : " << mapByteNum <<
	" with wordsInSecond : " << currentWordsInSecond << " mapLen : " 
	<< mapLen << " " << vo.getSEEDName() << std::endl;
    }
  }

  qma_uint8 mb = map[mapByteNum];

  //
  // Find number of shifts needed to move current map into lsb position.
  // 4 maps per byte
  //
  int res = currentWordsInSecond % 4;
  switch(res)
  {
    case 0:
    {
       mb = mb & 0xc0;
       mb = mb >> 6;
       break;
    }
    case 1:
    {
       mb = mb & 0x30;
       mb = mb >> 4;
       break;
    }
    case 2:
    {
       mb = mb & 0x0c;
       mb = mb >> 2;
       break;
    }
    case 3:
    {
       mb = mb & 0x03;
       break;
    }
  }

  if(!true)
  {
    g_log << "--- Created map from map byte " <<
	(qma_uint16) map[mapByteNum] << "  " << (qma_uint16) mb << std::endl;
  }
  //
  // Based on current word in packet, move the current map byte to the
  // proper positon, and or it with current byte to set the byte bits.
  //

  //
  // Find the frame number
  //
  int rotateNum = 0;
  destMask = 0;
  qma_uint32 baseVal = (qma_uint32) mb;

  if(currentWordsInPacket < 13)
  {
	  destFrameNumber = 1;
	  destWordPosition = currentWordsInPacket + 3;
	  rotateNum = (12 - currentWordsInPacket) *2;
	  destMask = baseVal << rotateNum;
  }
  else if(currentWordsInPacket < 28)
  {	
 	  destFrameNumber = 2;
	  destWordPosition = ((currentWordsInPacket - 13)+1);
	  rotateNum = (27 - currentWordsInPacket)*2;
	  destMask = baseVal << rotateNum;
  }
  else if(currentWordsInPacket < 43)
  { 	
  	  destFrameNumber = 3;
	  destWordPosition = ((currentWordsInPacket - 28)+1);
	  rotateNum = (42 - currentWordsInPacket)*2;
	  destMask = baseVal << rotateNum;
  }
  else if(currentWordsInPacket < 58)
  {	
 	  destFrameNumber = 4;
	  destWordPosition = ((currentWordsInPacket - 43)+1);
	  rotateNum = (57 - currentWordsInPacket)*2;
	  destMask = baseVal << rotateNum;
  }
  else if(currentWordsInPacket < 73)
  {	
 	  destFrameNumber = 5;
	  destWordPosition = ((currentWordsInPacket - 58)+1);
	  rotateNum = (72 - currentWordsInPacket)*2;
	  destMask = baseVal << rotateNum;
  }
  else if(currentWordsInPacket < 88)
  {	
 	  destFrameNumber = 6;
	  destWordPosition = ((currentWordsInPacket - 73)+1);
	  rotateNum = (87 - currentWordsInPacket)*2;
	  destMask = baseVal << rotateNum;
  }
  else if(currentWordsInPacket < 103)
  {
	  destFrameNumber = 7;
	  destWordPosition = ((currentWordsInPacket - 88)+1);
	  rotateNum = (102 - currentWordsInPacket)*2;
	  destMask = baseVal << rotateNum;
  }
  else
  {
	  g_log << "xxx Error more than 103 words in packet" << std::endl;
  }
  return;
}

void moveMultSegsIntoFirstSeg(SecondOfData& sod)
{
  int numBlcks = sod.getNumberOfBlockettes();
  if(numBlcks < 2)
  {
    g_log << "xxx Called MoveMults with too few blockettes: " << 
      numBlcks << std::endl;
  }
  else
  {
    // Zero all un-used samples
   
    int startPos =  sod.p_blockette_array[0].getBlockette().dataLengthInBytes;
    int len = (MAX_SAMPLES_IN_BLOCKETTE*4) - startPos;

    if(startPos%4 !=0)
    {
      g_log << "xxx Data length in moveMults not a valid word length: " << 
        startPos << std::endl;
    }
    else
    {
      startPos = startPos/4;
    }

    memset((char*)&sod.p_blockette_array[0].p_blockette.w[startPos],0,len);

    for (int i = 1;i<numBlcks;i++)
    {
      qma_uint16 lastData = 
        sod.p_blockette_array[0].getBlockette().dataLengthInBytes;

      if(lastData%4 !=0)
      {
        g_log << "xxx Data length in moveMults not a valid word length: " 
	<< lastData << std::endl;
      }
      else
      {
        lastData = lastData/4;
      }

      qma_uint16 newData  = 
        sod.p_blockette_array[i].getBlockette().dataLengthInBytes;

     g_log << "--- Transferable data length found as: " << 
        newData << std::endl;

      memcpy((char*)&sod.p_blockette_array[0].p_blockette.w[lastData],
             (char*)&sod.p_blockette_array[i].p_blockette.w[0],
             newData);

      sod.p_blockette_array[0].p_blockette.dataLengthInBytes =
        sod.p_blockette_array[0].p_blockette.dataLengthInBytes + newData;

    }

    if((sod.p_blockette_array[0].getBlockette().mapLengthInBytes-2)%4 != 0)
    {
      g_log << "xxx Unexpected Map Length - not a factor of 4: " << 
	(sod.p_blockette_array[0].getBlockette().mapLengthInBytes-2) 
		<< std::endl;
    }

    if(sod.p_blockette_array[0].getBlockette().dataLengthInBytes%4 != 0)
    {
      g_log << "xxx Unexpected Blockette Length - not a factor of 4:" <<
	sod.p_blockette_array[0].getBlockette().dataLengthInBytes 
	 << std::endl;
    }

    int dlen = sod.p_blockette_array[0].getBlockette().dataLengthInBytes/4;

    sod.setNumberOfDataWords(dlen);

    if(!true)
    {
      g_log << "--- Final total via CountDataWords was: " <<
        sod.getNumberOfDataWords() << " and map size: " <<
	  sod.p_blockette_array[0].getBlockette().mapLengthInBytes << std::endl;
    }
    sod.setNumberOfBlockettes(1);
    sod.setBlocketteType(DC_COMP);
    sod.p_blockette_array[0].p_blockette.blocketteType = DC_COMP;
    sod.p_blockette_array[0].p_blockette.finalSegment = true;

/*
    g_log << "+++ Test in moveMUlt" << std::endl;
   
    qma_int32 diffbuf[532],*diffptr;
    qma_int32 databuf[532],*dataptr;
    diffptr = diffbuf;
    dataptr = databuf;

  //
  // Find the data point at which the SOD exceeds
  // the 103 data words allowed in a packet.
  //

   QMABLOCK dblock;
   qma_int32 x0,xn;
   int num;

   g_log << "--- Test DC_MULT decompress" << std::endl;
  
   dblock = sod.p_blockette_array[0].getBlockette();
   int res = unpack_DCComp(&dblock,
                             diffptr,
                              dataptr);

    if(res == 200)
    {
      g_log << "--- Unpack in moveMult ok" << std::endl;
    }
    else
    {
      g_log << "xxx Error unpacking after Move Mult " << std::endl;
    }
*/

  }
}
