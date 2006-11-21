/*
 * File     :
 *   ChanFreqMap.C
 *
 * Purpose  :
 *   In order to maintain a sparse array of LCQVO's, LCQs, and CompressionQs,
 *   we use a channel,freq to array index mapper.
 *   Based strictly on the order in request, a channelfreq pair is convert
 *   to a index. When the channel,freq are new, they get assigned a new
 *   index. When they are established, they returned the assigned index.
 * 
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *  20 May 2002
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
#include "ChanFreqMap.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "global.h"
ChanFreqMap::ChanFreqMap()
{

  p_current_index = 0;

  //
  // Initialize the array of positions
  //
  qma_uint8 pos = 0;
  for (int i=0;i<MAX_CHANNELS;i++)
  {
    for(int x=0;x<MAX_FREQS;x++)
    {
      p_position_array[i][x] = pos;
      ++pos;
    }
  }
  p_map = NULL;
}

ChanFreqMap::~ChanFreqMap()
{
  //
  // Freq heap memory on exit
  //
  delete [] p_map;
}

void ChanFreqMap::reset()
{

  delete [] p_map;

  p_current_index = 0;

  //
  // Initialize the array of positions
  //
  qma_uint8 pos = 0;
  for (int i=0;i<MAX_CHANNELS;i++)
  {
    for(int x=0;x<MAX_FREQS;x++)
    {
      p_position_array[i][x] = pos;
      ++pos;
    }
  }
  p_map = NULL;
}

bool ChanFreqMap::ChanFreqInQueue(qma_uint8 chan, qma_uint8 freq)
{

  if(!true)
  {
    g_log << "--- Checking if chanfreq already exists : " <<
	(qma_uint16) chan << "  " << (qma_uint16) freq << std::endl;
  }
  //
  // Error return value is 255 for this routine
  //
  qma_uint8 pos = mapChanFreqToID(chan,freq);

  if(!true)
  { 
    g_log << "--- Found list position of " << (qma_uint16) pos << std::endl;
  }
  if(pos == 255)
  {
    return false;
  }
  else
  {
    int retval = retrieveQueuePosition(chan,freq);
    if(retval < 0)
    {
        return false;
    }
    else
    {
	return true;
    }
  }
}

bool ChanFreqMap::validChanFreqValues(qma_uint8 chan, qma_uint8 freq)
{

  if(!true)
  {
    g_log << "--- Checking if chanfreq values are in valid range : " <<
	(qma_uint16) chan << "  " << (qma_uint16) freq << std::endl;
  }
  //
  // Error return value is 255 for this routine
  //
  qma_uint8 pos = mapChanFreqToID(chan,freq);

  if(!true)
  { 
    g_log << "--- Found list position of " <<
	(qma_uint16) pos << std::endl;
  }

  if(pos == 255)
  {
    return false;
  }
  else
  {
    return true;
  }
}

int  ChanFreqMap::assignQueuePosition(qma_uint8 chan, qma_uint8 freq)
{

  int retval = p_current_index;

  //
  // Use this to create a dynamically sized array of indexes.
  // The array will grow only during initialization, and not
  // during running of the program.
  //
  qma_uint8* tempptr;
  tempptr  = new qma_uint8[p_current_index+1];

  //
  // 
  if(p_current_index == 0)
  {
    p_map = tempptr; 
  }
  else
  {
    for(int i=0;i<p_current_index;i++)
    {
      tempptr[i] = p_map[i];
    }
    delete [] p_map; // free previous array
    p_map = tempptr;
  }
  qma_uint8 pos = mapChanFreqToID(chan,freq);
  p_map[p_current_index] = pos;
  if(!true)
  {
    g_log << "--- Found chan " <<
	(qma_uint16) chan << " freq " <<
	(qma_uint16) freq << " ID " <<
	(qma_uint16) pos  << " index " <<
	(qma_uint16) p_current_index << std::endl;
  }
  ++p_current_index;
  return retval; // returns index assigned to this chanfreq
}

int  ChanFreqMap::retrieveQueuePosition(qma_uint8 chan, qma_uint8 freq)
{
  int retval = -1;
  qma_uint8 pos = mapChanFreqToID(chan,freq);
  
  for(int i=0;i<p_current_index;i++)
  {
    if(p_map[i] == pos)
    {
      retval = i;
      break;
    }
  }
  if(retval == -1)
  {
    if(!true)
    {
     g_log << "xxx Error Searched for chan freq queue position and did " <<
	" not find it " << (qma_uint16) chan << 
	(qma_uint16) freq << std::endl;
    }
  }
  return retval;
}

qma_uint8 ChanFreqMap::mapChanFreqToID(qma_uint8 chan, qma_uint8 freq)
{

  qma_uint8 retVal;
  //
  // Due to the data type, it is always 0 or greater. So test for
  // negative numbers will not be done here.
  // I will use value 255 as error return value
  //
  if( (chan <= MAX_CHANNELS) &&
      (freq <= MAX_FREQS))
  {
        retVal = p_position_array[chan][freq];
  }
  else
  {
    g_log << "xxx Error - Invalid chan or freq in list " <<
        (qma_uint16) chan << " " << (qma_uint16) freq << std::endl;
    retVal = 255;
  }
  return retVal;
}
