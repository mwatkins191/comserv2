/*
 *
 * File     :
 *   c1_rqflgs.h
 *
 * Purpose  :
 *   This is a Request Flags message.
 *
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   4 March 2002
 *
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

#ifndef C1_RQFLGS_H
#define C1_RQFLGS_H

#include "Field.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "PacketElement.h"

class c1_rqflgs : public PacketElement
{
  public:

    c1_rqflgs();
    ~c1_rqflgs() {};

    //
    // Operations on fields
    //

    qma_uint16 getDataPortNumber();
    void       setDataPortNumber(qma_uint16 port);

    Field      p_fieldList[FIELDS_IN_C1_RQFLGS];

};

#endif
