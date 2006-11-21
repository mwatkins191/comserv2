/*
 *
 * File     :
 *   c1_cack.h
 *
 * Purpose  :
 *   This is a cmd ack packet.
 *
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   4 April 2002
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

#ifndef C1_CACK_H
#define C1_CACK_H

#include "Field.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "PacketElement.h"

class c1_cack : public PacketElement
{
  public:

    c1_cack();
    ~c1_cack() {};

    //
    // Operations on fields
    //
    // This command has no parameters
    //
};

#endif
