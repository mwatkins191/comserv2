/*
 * File     :
 *  DigitizerSegment.h
 *
 * Purpose  :
 *  This is used to contain on second of uncompressed data.
 *  For sub one sample per second data, we use scan of data class
 *
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *  24 May 2002
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
#ifndef SECONDOFDATA_H
#define SECONDOFDATA_H

#include "BTI.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "Blockette.h"

class SecondOfData
{

  public:

   SecondOfData();
   ~SecondOfData() {};

   void addBlockette(Blockette block);

   void setNumberOfBlockettes(const int aNumber);
   int  getNumberOfBlockettes() const; 

   int  getNumberOfDataWords() const;
   void setNumberOfDataWords(const int aNumber);
 
   void setBlocketteTimeInfo(BTI dbti);
   BTI  getBlocketteTimeInfo() const;

   int  getBlocketteType() const;
   void setBlocketteType(const int aType);

   bool completionPacket() const;
   void setCompletionPacket(const bool val);

   Blockette p_blockette_array[MAX_BLOCKETTES_IN_SECOND];

   void      countDataWords(int aBlocketteNumber);
  
  private:


   bool      p_completionPacket;
   int       p_number_of_data_words;
   int       p_number_of_blockettes;
   int       p_blockette_type;
};
#endif
