/*
 * File     :
 *  TypeChanFreqMap.C
 *
 * Purpose  :
 *   In order to maintain a sparse array of LCQVO's, LCQs, and CompressionQs,
 *   we use a type/channel/freq to array index mapper.
 *   Based strictly on the order in request, a type/channel/freq is convert
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
#include "TypeChanFreqMap.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "global.h"

TypeChanFreqMap::TypeChanFreqMap()
{
  p_current_index = 0;

  //
  // Initialize the array of positions
  //
  qma_uint8 pos = 0;
  for (int i=0;i<MAX_TYPES;i++)
  {
    for(int x=0;x<MAX_CHANNELS;x++)
    {
       for(int z=0;z<MAX_MAIN_FREQS;z++)
       {
         p_position_array[i][x][z] = pos;
         ++pos;
       }
    }
  }
}

void TypeChanFreqMap::reset()
{
  p_current_index = 0;
  //
  // Initialize the array of positions
  //
  qma_uint8 pos = 0;
  for (int i=0;i<MAX_TYPES;i++)
  {
    for(int x=0;x<MAX_CHANNELS;x++)
    {
      for(int z=0;z<MAX_MAIN_FREQS;z++)
      {
        p_position_array[i][x][z] = pos;
        ++pos;
      }
    }
  }
}

bool TypeChanFreqMap::TypeChanFreqInQueue(qma_uint8 atype, 
                                          qma_uint8 chan,
                                          qma_uint8 freq)
{

  if(!true)
  {
    g_log << "--- Checking if type/chan/freq already exists : " <<
	(qma_uint16) atype << " " << (qma_uint16) chan << "  " 
         << (qma_uint16) freq << std::endl;
  }
  //
  // Error return value is 255 for this routine
  //
  qma_uint8 pos = mapTypeChanFreqToID(atype,chan,freq);

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
    int retval = retrieveQueuePosition(atype,chan,freq);
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

bool TypeChanFreqMap::validTypeChanFreqValues(qma_uint8 atype,
                                              qma_uint8 chan,
                                              qma_uint8 freq)
{

  if(!true)
  {
    g_log << "--- Checking if type/chan/freq values are valid range : " <<
	(qma_uint16) atype << " " << (qma_uint16) chan 
        << "  " << (qma_uint16) freq << std::endl;
  }
  //
  // Error return value is 255 for this routine
  //
  qma_uint8 pos = mapTypeChanFreqToID(atype,chan,freq);

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

int  TypeChanFreqMap::assignQueuePosition(qma_uint8 atype, 
                                          qma_uint8 chan,
                                          qma_uint8 freq)
{
  int retval = p_current_index;
  qma_uint8 pos = mapTypeChanFreqToID(atype,chan,freq);
  p_map[p_current_index] = pos;
  if(!true)
  {
    g_log << "--- Assigned type: " <<
	(qma_uint16) atype << " chan: " <<
	(qma_uint16) chan << " freq: " <<
        (qma_uint16) freq << " at position: " <<
	(qma_uint16) pos  << " with index: " <<
	(qma_uint16) p_current_index << std::endl;
  }
  ++p_current_index;
  return retval; // returns index assigned to this chanfreq
}

int  TypeChanFreqMap::retrieveQueuePosition(qma_uint8 atype, 
                                            qma_uint8 chan,
                                            qma_uint8 freq)
{
  int retval = -1;
  qma_uint8 pos = mapTypeChanFreqToID(atype,chan,freq);
  
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
     g_log << "xxx Searched for type/chan/freq queue position and did " <<
	" not find it. Type: " << (qma_uint16) atype << " chan: " << 
	(qma_uint16) chan << " freq: " << (qma_uint16) freq << std::endl;
    }
  }
  return retval;
}

qma_uint8 TypeChanFreqMap::mapTypeChanFreqToID(qma_uint8 atype, 
                                               qma_uint8 chan,
                                               qma_uint8 freq)
{
  qma_uint8 retVal;
  //
  // Due to the data type, it is always 0 or greater. So test for
  // negative numbers will not be done here.
  // I will use value 255 as error return value
  //
  if( (atype <= MAX_TYPES) &&
      (chan <= MAX_CHANNELS) &&
      (freq <= MAX_MAIN_FREQS))
  {
        retVal = p_position_array[atype][chan][freq];
  }
  else
  {
    g_log << "xxx Invalid type/chan/freq in list. Type: " <<
        (qma_uint16) atype << " chan: " << (qma_uint16) chan << " freq: " << 
        (qma_uint16) freq << std::endl;
    retVal = 255;
  }
  return retVal;
}
