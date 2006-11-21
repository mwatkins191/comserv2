/*
 *
 * File     :
 *   QDPHeader.h
 *
 * Purpose  :
 *   This is a bit oriented header definition. A packet will
 *   contain a list of header and a message.
 *
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   14 February 2002
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
#ifndef QDPHEADER_H
#define QDPHEADER_H

#include "Field.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "PacketElement.h"

class QDPHeader : public PacketElement
{
  public:

    QDPHeader();
    ~QDPHeader() {};

    //
    // Operations on fields
    //

    void setCRC(const qma_uint32 crc);
    qma_uint32 getCRC() const;

    qma_uint32 getCommand() const;
    void setCommand(const qma_uint8 cmd);

    qma_uint32 getVersion() const;
    void setVersion(const qma_uint8 ver);


    qma_uint16 getDataLengthInBytes() const;
    void setDataLengthInBytes(const qma_uint16 msgLen);


    qma_uint16 getPacketSequence() const;
    void setPacketSequence(const qma_uint16 seq);

    qma_uint16 getAckNumber() const;
    void setAckNumber(const qma_uint16 ackNum);

    Field p_fieldList[FIELDS_IN_QDPHEADER];
};

#endif
