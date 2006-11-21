/*
 * File     :
 *  Blockette.h
 *
 * Purpose  :
 *  Class containing raw blockette and Timestamp
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  26 May 2002
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
#ifndef BLOCKETTE_H
#define BLOCKETTE_H

#include "BTI.h"
#include "DC.h"

class Blockette
{
  public:
    Blockette() {};
    ~Blockette() {};

    void     setBlockette(QMABLOCK blockette);
    QMABLOCK getBlockette() const;

    void     setBlocketteTime(BTI timeStamp);
    BTI      getBlocketteTime() const;

    void     setFilterDelay(qma_int32 aDelay);

    // Made public to allow updating during multMoves.
    
    QMABLOCK p_blockette;

  private:

    BTI      p_time_info;
};
#endif
