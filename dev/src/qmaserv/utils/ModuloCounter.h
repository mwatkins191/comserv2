/*
 *
 * File     :
 *   ModuloCounter.h
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

#ifndef MODULO_COUNTER_H
#define MODULO_COUNTER_H

#include "QmaTypes.h"
#include "QmaLimits.h"

class ModuloCounter 
{
  public:
    ModuloCounter();
    ModuloCounter(qma_uint32);
    ~ModuloCounter() {};
    qma_uint32 next();
    qma_uint32 last();

 private :

  qma_uint32 p_modnumber;
  qma_uint32 p_modulus;
};
#endif
