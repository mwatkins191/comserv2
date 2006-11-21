/*
 *
 * File     :
 *   PacketMarker.h
 *
 *
 * Purpose  :
 *   This class tracks a seqno, and a ack'd flag. If the seqno
 *    has been ack'd, the acked flag is set to true.
 *
 *
 * Author   :
 *  Phil Maechling
 *
 *
 * Mod Date :
 *   12 March 2002
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
#ifndef PACKETMARKER_H
#define PACKETMARKER_H
#include "QmaTypes.h"

class PacketMarker
{

  public:
    PacketMarker();
    ~PacketMarker() {};

  void       setWasAcked(bool wasAcked);
  bool       wasAcked();
  void       setSeqno(qma_uint32 val);
  qma_uint32 getSeqno();
 
  private:

    bool       p_wasAcked;
    qma_uint32 p_seqno;
};

#endif
