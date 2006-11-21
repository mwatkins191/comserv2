/*
 * File     :
 *  Blockette.C
 *
 * Purpose  :
 *  Class containing raw blockette and Timestamp
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  26 May 2002
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
#include "Blockette.h"
#include "BTI.h"
#include "DC.h"
#include "qmaswap.h"

void  Blockette::setBlockette(QMABLOCK blockette)
{
  p_blockette = blockette;
}

QMABLOCK Blockette::getBlockette() const
{
  return p_blockette;
}


void  Blockette::setBlocketteTime(BTI timeStamp)
{
  p_time_info = timeStamp;
  //p_time_info.drsn = qma_ntohl(p_time_info.drsn);
  p_time_info.drsn = p_time_info.drsn;
}

void  Blockette::setFilterDelay(qma_int32 aDelay)
{
  p_time_info.filter_delay = aDelay;
}


BTI   Blockette::getBlocketteTime() const
{
   return p_time_info;
}
