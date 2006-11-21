/*
 * File     :
 *  BTI.h
 *
 * Purpose  :
 *  Blockette Time Infomation (BTI). This encapsulates the essential time 
 *  info about a blockette. Some of the information is from the
 *  the COMP98 blockette, but other is specific to the channel this 
 *  such as the channel represented by the blockette.
 *
 * Author   :
 *  Phil Maechling.
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

#ifndef BTI_H
#define BTI_H

#include "QmaTypes.h"

typedef struct blockette_time_info
{  
    qma_uint32 drsn;
    qma_uint32 sec_offset;
    qma_uint32 usec_offset;
    qma_uint8  clockQuality;
    qma_uint32 minutesSinceLock;
    qma_int32 filter_delay;
} BTI;

#endif
