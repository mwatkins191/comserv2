/*
 * File     :
 *   CompareBlockette.h
 *
 * Purpose  :
 *   This is a comparison routine (a predicate) for use with STL
 *   containers. 
 *
 * Author   :
 *   Phil Maechling
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
 */
#ifndef COMPARE_BLOCKETTES_H
#define COMPARE_BLOCKETTES_H

#include "Blockette.h"
#include "BTI.h"

class CompareBlockettes
{
  public:

    CompareBlockettes();
    ~CompareBlockettes();

    bool operator()(Blockette t1, Blockette t2) const
    {
       // 
       // Do two level sort. If blockette DRSN are equal, sort on seg num.
       //
       qma_uint32 d1 = t1.getBlocketteTime().drsn;
       qma_uint32 d2 = t2.getBlocketteTime().drsn;
       if(d1 == d2)
       {
          qma_uint32 x1 = t1.getBlockette().segmentNumber;
          qma_uint32 x2 = t2.getBlockette().segmentNumber;
          return(x1 < x2);
       }
       else
       {
         return (d1 < d2);
       }
    }
};
#endif
