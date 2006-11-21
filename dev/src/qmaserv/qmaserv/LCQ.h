/*
 * File     :
 *  LCQ.h
 *
 * Purpose  :
 *   This is the logical channel queue object. For each Logical Channel
 *   queue, There will be one of these objects.
 *
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *   6 May 2002
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
#ifndef LCQ_H
#define LCQ_H

#include "QmaTypes.h"
#include "BTI.h"
#include "Blockette.h"
#include "CompareBlockettes.h"
#include "LCQVO.h"

class LCQ
{
  public:

    LCQ();
    ~LCQ() {};

   void      setStartTime(const BTI& startTime); 

   bool      insertBlockette(const Blockette blockette);
   bool      pendingSecondReady() const;
   Blockette removeCurrentBlockette();
   Blockette removeAdditionalMultBlockette();

   void      setLCQVO(LCQVO aLCQVO);
   LCQVO     getLCQVO() const;
   int       getNumberOfBlockettesInQueue() const;  
 
 private:

   qma_uint32 getSecondsPerSample();
   bool       FirstLessThanSecond(Blockette t1, Blockette t2);
   void       incrementPendingSecond();
   LCQVO      p_lcqvo;
   bool       p_blocketteReadyToProcess;
   BTI        p_pendingSecond;
   BTI        p_startingSecond;
   Blockette  p_blockette_list[MAX_BLOCKETTES_IN_LCQ]; 
				      // Size is based on how many
				     // blockettes are outstanding in
				     // a sliding window. Is set to
				     // 128 + 1;
   int       p_number_of_blockettes;
};
#endif
