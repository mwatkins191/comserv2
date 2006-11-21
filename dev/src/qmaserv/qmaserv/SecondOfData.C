/*
 * File     :
 *  SecondOfData.C
 *
 * Purpose  :
 *  This is used to contain on second of data, encased in the blockette.
 *
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *  24 May 2002
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
#include "SecondOfData.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "qmaswap.h"
#include "global.h"

SecondOfData::SecondOfData() 
{
  p_number_of_data_words =0;
  p_number_of_blockettes = 0;
  p_completionPacket = false;
}
   
BTI SecondOfData::getBlocketteTimeInfo() const
{
  return p_blockette_array[0].getBlocketteTime();
}


void SecondOfData::addBlockette(Blockette blockette)
{
  if(p_number_of_blockettes > MAX_BLOCKETTES_IN_SECOND)
  {
    g_log << 
      "xxx Tried to add too many blockettes into a SecondOfData : " 
	      << p_number_of_blockettes << std::endl;
  }
  else
  {
    p_blockette_type = blockette.getBlockette().blocketteType;
    p_blockette_array[p_number_of_blockettes] = blockette;
    if(p_blockette_type == DC_D32)
    {
      p_number_of_data_words = 1;
    }
    else
    {
      countDataWords(p_number_of_blockettes);
    }
    ++p_number_of_blockettes;
  }
}

int SecondOfData::getBlocketteType() const
{
  return p_blockette_type;
} 

void SecondOfData::setBlocketteType(const int aType)
{
  p_blockette_type = aType;
} 


void SecondOfData::setNumberOfBlockettes(const int aNumber)
{
  p_number_of_blockettes = aNumber;
} 

int SecondOfData::getNumberOfBlockettes() const
{
  return p_number_of_blockettes;
} 


void SecondOfData::setNumberOfDataWords(const int aNumber)
{
  p_number_of_data_words = aNumber;
}

int SecondOfData::getNumberOfDataWords() const
{
  return p_number_of_data_words;
}

void SecondOfData::countDataWords(int bn)
{
  qma_uint16 len = p_blockette_array[bn].getBlockette().dataLengthInBytes;

  if(len%4 != 0) // This test to be sure we have a valid data length
  {
    g_log << "xxx Invalid data length found in SecondOfData : " 
	<< len << std::endl;
  }
  p_number_of_data_words = p_number_of_data_words + (len/4);
  if(!true)
  {
    g_log << "--- Counted length: " << len << " as number of Datawords: " <<
    (len/4) << " for total len: " << p_number_of_data_words << std::endl;
  }
}

bool SecondOfData::completionPacket() const
{
  return p_completionPacket;
}

void SecondOfData::setCompletionPacket(const bool val)
{
  p_completionPacket = val;
}
