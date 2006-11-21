/*
 *
 * File     :
 *   c1_cack.C
 *
 * Purpose  :
 *   This is a message to ack cmd packets.
 *
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   4 April 2002
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

#include "c1_cack.h"
c1_cack::c1_cack()
{
  p_number_of_fields = 0;
  p_length_in_bytes = 0;
}

//
// Currently no paramenters for this message
//
