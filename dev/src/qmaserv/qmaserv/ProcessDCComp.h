/*
 * File     :
 *   ProcessDCComp.h
 *
 * Purpose  :
 *   Process a DCComp packet
 *
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *  8 June 2002
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
#ifndef PROCESS_DCCOMP
#define PROCESS_DCCOMP
#include "BTI.h"
#include "msgs.h"
//
// return the number of bytes (len) in blockette
//
qma_uint16 processDCComp(const int       curposition,
                         const qma_uint8 chan,
	                 dt_data&        blockette,
			 const BTI&	 curtime);

#endif
