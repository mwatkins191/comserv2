/*
 *
 * File     :
 *   c1_umsg.h
 *
 * Purpose  :
 *   This is a User Message from DP to Q330.
 *
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   28 July 2002
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

#ifndef C1_UMSG
#define C1_UMSG

#include "Field.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "PacketElement.h"

void strpas(char*, char*);

class c1_umsg :  public PacketElement
{
  public:

    c1_umsg();
    ~c1_umsg() {};

    //
    // Operations on fields
    //

    void  setUserMessage(char msg[80]);
    char* getUserMessage();
    Field   p_fieldList[FIELDS_IN_C1_UMSG]; 
};

#endif
