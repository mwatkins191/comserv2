/*
 *
 * File     :
 *   c1_srvrsp.h
 *
 * Purpose  :
 *   This is a Server Response message.
 *
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   24 February 2002
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

#ifndef C1_SRVRSP_H
#define C1_SRVRSP_H

#include "Field.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "PacketElement.h"

class c1_srvrsp : public PacketElement
{
  public:

    c1_srvrsp();
    ~c1_srvrsp() {};

    //
    // Operations on fields
    //

    qma_uint64 getSerialNumber();
    void       setSerialNumber(qma_uint64 cv);

    qma_uint64 getChallengeValue();
    void       setChallengeValue(qma_uint64 cv);

    qma_uint32 getServerIPAddress();
    void       setServerIPAddress(qma_uint32 sip);

    qma_uint16 getServerUDPPort();
    void       setServerUDPPort(qma_uint16 sup);

    qma_uint16 getRegistrationNumber();
    void       setRegistrationNumber(qma_uint16 regnum);

    qma_uint64 getRandomNumber();
    void       setRandomNumber(qma_uint64 randNum);
    qma_uint64 generateRandomNumber();

    void       setMD5Result(const qma_uint64 authCode);

    Field     p_fieldList[FIELDS_IN_C1_SRVRSP];

};

#endif
