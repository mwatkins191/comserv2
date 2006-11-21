/*
 * File     :
 *  ReceiveMsgs.h
 *
 * Purpose  :
 *  Collection of routines to process Received msgs from the Q330. 
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

#ifndef RECEIVEMSGS_H
#define RECEIVEMSGS_H

#include "msgs.h"

void initializeTokenBuffer();
bool createLCQsAndPCQs();
bool processStatusResponse(const c1_stat& s1);
bool processPeriodicStatusResponse(const c1_stat& s1);
bool processFlagsResponse(const c1_flgs& f1);
bool processTokenResponse(const c1_mem& mm);
bool processCmdErrorMessage(const c1_cerr& err);
#endif
