/*
 * File     :
 *  InputPacket.h
 *
 * Purpose  :
 *  Define the structure containing a packet and a full flag.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  10 Aug 2002
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
#ifndef INPUT_PACKET
#define INPUT_PACKET

#include "Packet.h"

typedef struct inputpacket
{
  Packet packet;
  bool   full;
} InputPacket;

#endif
