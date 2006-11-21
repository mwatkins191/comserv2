/*
 *
 * File     :
 *   c1_cerr.h
 *
 * Purpose  :
 *   This is a error response from the Q330
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

#ifndef C1_CERR
#define C1_CERR

#include "Field.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "PacketElement.h"

class c1_cerr :  public PacketElement
{
  public:

    c1_cerr();
    ~c1_cerr() {};

    //
    // Operations on fields
    //

    qma_uint16 getErrorCode() const;

    Field   p_fieldList[FIELDS_IN_C1_CERR]; 
};

#endif
