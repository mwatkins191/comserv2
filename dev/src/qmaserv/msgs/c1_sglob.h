/*
 *
 * File     :
 *   c1_sglog.h
 *
 * Purpose  :
 *   This is a Global Programming message
 *
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   4 March 2002
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

#ifndef C1_SGLOB_H
#define C1_SGLOB_H

#include "Field.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "PacketElement.h"

class c1_sglob : public PacketElement
{
  public:

    c1_sglob();
    ~c1_sglob() {};

    //
    // Operations on fields
    //
    
    qma_uint16 getClockTimeout();
    qma_uint16 getInitialVCO();
    qma_uint16 getGPSBackupPower();
    qma_uint16 getAuxStatusSampRate();
    qma_uint16 getGainBitmap();
    qma_uint16 getFilterBitmap();
    qma_uint16 getInputBitmap();
    qma_uint16 getWebPort();
    qma_uint16 getServerTimeout();
    qma_uint16 getDriftTolerance();
    qma_uint16 getJumpFilter();
    qma_uint16 getJumpThreshold();
    qma_uint16 getCalibratorOffset();
    qma_uint16 getSensorControlBitmap();
    qma_uint16 getSamplePhase();
    qma_uint16 getGPSColdStartSeconds();
    qma_uint32 getUsetTag();
    qma_uint16 getChan1FreqBit7();
    qma_uint32 getMessageBitmap();

    Field  p_fieldList[FIELDS_IN_C1_SGLOB];

};

#endif
