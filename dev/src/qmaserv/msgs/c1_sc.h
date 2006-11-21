/*
 *
 * File     :
 *   c1_sc.h
 *
 * Purpose  :
 *   This is a Sensor Control message.
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

#ifndef C1_SC_H
#define C1_SC_H

#include "Field.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "PacketElement.h"

class c1_sc : public PacketElement
{
  public:

    c1_sc();
    ~c1_sc() {};

    //
    // Operations on fields
    //
    
    qma_uint32 getSensorOutput1();
    qma_uint32 getSensorOutput2();
    qma_uint32 getSensorOutput3();
    qma_uint32 getSensorOutput4();
    qma_uint32 getSensorOutput5();
    qma_uint32 getSensorOutput6();
    qma_uint32 getSensorOutput7();
    qma_uint32 getSensorOutput8();

    Field   p_fieldList[FIELDS_IN_C1_SC];

};

#endif
