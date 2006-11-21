/*
 * File     :
 *  RxMsg.h
 *
 * Purpose  :
 *  This routine checks the current input port and receives and packet it
 *  find on the ports.
 *
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  27 July 2002
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
#ifndef RXMSG_H
#define RXMSG_H

#include "Packet.h"

void rx_msg();
bool checkInCmdPort(Packet& in_p);
bool checkInDataPort(Packet& in_p);
bool processCmdPacket(const Packet& in_p);
bool processDataPacket(const Packet& in_p);

#endif
