/*
 *
 * File     :
 *   PacketMarker.C
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
#include "QmaTypes.h"
#include "PacketMarker.h"

PacketMarker:: PacketMarker()
{
  p_wasAcked = false;
  p_seqno = 0;
}

void PacketMarker::setWasAcked(bool wasAcked)
{
  p_wasAcked = wasAcked;
}

bool PacketMarker::wasAcked()
{
  return p_wasAcked;
}

void PacketMarker::setSeqno(qma_uint32 val)
{
  p_seqno = val;

}

qma_uint32 PacketMarker::getSeqno()
{
  return p_seqno;
}
