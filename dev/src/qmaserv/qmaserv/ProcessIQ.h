/*
 * File     :
 *  ProecessIQ.h
 *
 * Purpose  :
 *  Process Input Queue (ProcesIQ). The input queue is the packet 
 *  sequence order queue of packets. When the packets are contiguous, 
 *  this routine will read them from the Input queue, and put the 
 *  individual blockettes onto Logical Queues.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  10 August 2002
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
#ifndef PROCESSIQ_H
#define PROCESSIQ_H

#include "BTI.h"
#include "dt_data.h"

void processInputQueue();
bool walkBlockettes(dt_data& dt);
void setDRSNForAllLCQ(const BTI& timeInfo);
void sendAcksIfNeeded();
void report_sequence_error(char* astring,qma_uint32 wasit, qma_uint32 notit);
#endif
