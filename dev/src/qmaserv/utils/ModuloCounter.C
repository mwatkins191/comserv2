/*
 *
 * File     :
 *   ModuloCounter.C
 *
 * Purpose  :
 *   Establish a simple packet sequence Counter
 *
 * Author   :
 *
 *
 *
 * Mod Date :
 *    
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
#include "ModuloCounter.h"

ModuloCounter::ModuloCounter()
{
  p_modulus = PACKET_MODULUS;
  p_modnumber = 0;
}

ModuloCounter::ModuloCounter(qma_uint32 modulus)
{
  p_modulus = modulus;
  p_modnumber = 0;

}

qma_uint32 ModuloCounter::next()
{
  qma_uint32 val = ++p_modnumber;
  p_modnumber = val%p_modulus;
  return (p_modnumber);
}

qma_uint32 ModuloCounter::last()
{
  return (p_modnumber);
}
