/*
 *
 * File     :
 *   dt_open.h
 *
 * Purpose  :
 *   This is a open request for data.
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

#ifndef DT_OPEN_H
#define DT_OPEN_H

#include "Field.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "PacketElement.h"

class dt_open : public PacketElement
{
  public:

    dt_open();
    ~dt_open() {};

    //
    // Operations on fields
    //
    // This command has no parameters
    //
};

#endif
