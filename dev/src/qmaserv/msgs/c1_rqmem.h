/*
 *
 * File     :
 *   c1_rqmem.h
 *
 * Purpose  :
 *   This is a Request Memory (Tokens) message.
 *   Currently this is used to request logical port tokens
 *   but could be expanded to request other types of tokens.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   20 April 2002
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

#ifndef C1_RQMEM_H
#define C1_RQMEM_H

#include "Field.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "PacketElement.h"

//
// Define the memory types used in the rqmem message
//
const qma_uint16 Flash = 0;
const qma_uint16 DataPort1 = 1;
const qma_uint16 DataPort2 = 2;
const qma_uint16 DataPort3 = 3;
const qma_uint16 DataPort4 = 4;
const qma_uint16 WebPage = 5;
const qma_uint16 NA1 = 6;
const qma_uint16 NA2 = 7;
const qma_uint16 NA3 = 8;
const qma_uint16 NA4 = 9;
const qma_uint16 SlaveEEPROM = 10;
const qma_uint16 SlavePIC = 11;
const qma_uint16 ClockChipRAM = 12;
const qma_uint16 CalibratorPIC = 13;
const qma_uint16 QAPCHPEEPROM = 14;
const qma_uint16 PacketBufferMem = 15;
const qma_uint16 DSPProgramMemory = 16;
const qma_uint16 DSPDataMemory = 17;

class c1_rqmem : public PacketElement
{
  public:

    c1_rqmem();
    ~c1_rqmem() {};

    //
    // Operations on fields
    //

    qma_uint32 getStartingAddress();
    void       setStartingAddress(qma_uint32 addr);

    qma_uint16 getByteCount();
    void       setByteCount(qma_uint16 count);

    qma_uint16 getMemoryType();
    void       setMemoryType(qma_uint16 memtpye);

    void 	getPassword(qma_char* pwd32bytes);
    void        setPassword(qma_char* pwd32bytes);

    qma_uint16  getDataPortToken(qma_uint16 PortNumber);

    Field      p_fieldList[FIELDS_IN_C1_RQMEM];
};

#endif
