/*
 * File     :
 *  FrameUtils.h
 *
 * Purpose  :
 *   Utility Methods used in the CreatePacket routines.
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
#ifndef FRAME_UTILS_H
#define FRAME_UTILS_H

#include "QmaLimits.h"
#include "QmaTypes.h"
#include "LCQVO.h"
#include "SecondOfData.h"
#include "qma_mseed.h"


void getAllIntMap(const int numberOfMaps,qma_int32 *array);


void findFrameMapAndWord(const LCQVO& vo,
                         const int    currentWordInSecond,
                         const int    currentWordsInPacket,
                         qma_uint8    map[],
                         qma_uint16   mapLen,
                         int&         destFrameNumber,
                         int&         destWordPosition,
                         qma_uint32&  destMap);

void moveMultSegsIntoFirstSeg(SecondOfData& sod);
#endif
