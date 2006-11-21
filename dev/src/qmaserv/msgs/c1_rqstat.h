/*
 *
 * File     :
 *   c1_rqstat.h
 *
 * Purpose  :
 *   This is a Request Status message.
 *
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   28 April 2002
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

#ifndef C1_RQSTAT_H
#define C1_RQSTAT_H

#include "Field.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "PacketElement.h"

class c1_rqstat : public PacketElement
{
  public:

    c1_rqstat();
    ~c1_rqstat() {};

    //
    // Operations on fields
    //

    void requestAllStatus();

    void requestGlobalAndBoomPositionStatus(int portNumber);
    void requestGPSStatus();
    void requestPowerSupplyStatus();
    void requestBoomPosition();
    void requestThreadStatus();
    void requestPLLStatus();
    void requestGPSSatellites();
    void requestARPStatus();
    void requestDataPortStatus(int portNumber);
    void requestSerialInterfaceStatus(int portNumber);
    void requestEthernetStatus();
    void requestBalerStatus();

    Field p_fieldList[FIELDS_IN_C1_RQSTAT];
};

#endif
