/*
 * File     :
 *  SendCmds.h
 *
 * Purpose  :
 *  Collection of routines to send cmds to the Q330. Format and send
 *  the message.
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
#ifndef SENDCMDS_H
#define SENDCMDS_h

#include "QMA_Port.h"

bool sendChallenge(QMA_Port& outPort);
bool sendChallengeResponse(QMA_Port& outPort);
bool sendStatusRequest(QMA_Port& outPort);
bool sendFlagsRequest(QMA_Port& outPort);
bool sendTokenRequest(QMA_Port& cmdPort,int nextAddress);
bool sendUserMessage(QMA_Port& cmdPort);
bool sendOpenDataPort(QMA_Port& dataPort);
bool sendDisconnect(QMA_Port& outport);

#endif
