/*
 * File     :
 *   CheckPCQS.h
 *
 * Purpose  :
 *  This is the routines which checks to see if packet queues
 *   have current data in them, and if so, compresss and send it.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   26 May 2002
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
#ifndef CHECKPCQS_H
#define CHECKPCQS_H

#include "QmaTypes.h"

bool check_pcqs();
void empty_pcqs();

#endif
