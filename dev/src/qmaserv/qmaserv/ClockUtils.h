/*
 * File     :
 *   ClockUtils.h
 *
 * Purpose  :
 *  This contains the utilies that translate clock measurements into
 *  a clock quality value for the SEED Header.
 *
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *  14 October 2002
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
#ifndef CLOCK_UTILS
#define CLOCK_UTILS
#include "QmaTypes.h"
#include "ClockProcVO.h"

qma_uint8 translate_clock(const ClockProcVO& cVO,
                          const qma_uint16 clockQuality,
                          const qma_uint16 timeSinceLoss);

#endif
