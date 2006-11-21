/*
 * File     :
 *  DC.h
 *
 * Purpose  :
 *  Define data structures used by q330 blockettes
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  8 June 2002
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
#ifndef DC_H
#define DC_H

#include "QmaTypes.h"
#include "QmaLimits.h"

//
// Use the same format for either a DC_COMP, or DC_MULT
// DC_Comp will have segment 0 and secondComplete true;
// DC_Mult will have segment 0 and second complete false;
//
typedef struct qma_DCBlockette
{
 qma_uint32 blocketteType;
 qma_uint32 numberOfSamples;
 qma_int32  previousSample;
 qma_uint32 segmentNumber;
 bool       finalSegment;
 qma_uint16 mapLengthInBytes; 
 qma_uint8  map[MAX_BYTES_IN_BLOCKETTE_MAP]; // 200/4 currently
 qma_uint16 dataLengthInBytes;
 qma_int32  w[MAX_SAMPLES_IN_BLOCKETTE];    // set to 200 currently
} QMABLOCK;

#endif
