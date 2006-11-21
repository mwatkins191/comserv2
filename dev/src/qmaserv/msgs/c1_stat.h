/*
 *
 * File     :
 *   c1_stat.h
 *
 * Purpose  :
 *   This is a flags response message.
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

#ifndef C1_STAT_H
#define C1_STAT_H

#include "Field.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "PacketElement.h"

void strpcopy (char* outstring, char* instring);

#define STAT_BITMAP_SIZE 4
#define STAT_GLOBAL_SIZE 52
#define STAT_MISC_SIZE 32
#define STAT_USERMSG_SIZE 84

class c1_stat : public PacketElement
{
  public:

    c1_stat();
    ~c1_stat() {};

    //
    // This message has variable length fields.
    // Add special set and get methods to deal with
    // length of message info as received.
    //
    void setBitString(const unsigned char* buf,qma_uint32 len);
    unsigned char* getOffsetBitString(const qma_uint16 start) const;
    unsigned char* getOffsetRawBitString(const qma_uint16 start) const;

    //
    // The following are clock quaility values
    //
    bool globalStatus() const;

    bool logicalPortStatus1();
    bool logicalPortStatus2();
    bool logicalPortStatus3();
    bool logicalPortStatus4();

    bool  dataPortProgrammingChanged() const;
    bool  dpTokensChanged() const;

    bool       userMessagePresent() const;
    int        userDataPortStatusPresent() const;
    char*      getUserMessage() const;
    qma_uint32 getUserIPAddress() const;

    qma_uint16        getClockQuality() const;
    qma_uint16        getMinutesSinceLoss() const;
    qma_uint32        getSecondsOffset() const;
    qma_uint32        getUsecsOffset() const;
    qma_uint16        getCalibrationStatus() const;
    qma_uint32        getDRSN() const;
    qma_uint32        getDataPortQueue() const;
    qma_uint16	      getInputPower() const;
    qma_int16        getBoomPosition(int) const;
    Field   p_fieldList[FIELDS_IN_C1_STAT];

   private:
     char p_user_message[120];
     qma_uint32 p_userIPAddress;
     qma_uint8        p_raw_bits[MAX_BYTES_IN_PACKET];
};

#endif
