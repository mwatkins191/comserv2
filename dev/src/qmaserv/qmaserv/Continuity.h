/*
 * File     :
 *   Continuity.h
 *
 * Purpose  :
 *  Save the 
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  4 October 2002
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

#include "QmaTypes.h"
#include "QmaLimits.h"
#include "StateInfoVO.h"

class Continuity
{
  public:
    Continuity();
    ~Continuity() {};

    StateInfoVO getContinuityInfo() const;
    bool saveContinuityInfo(const StateInfoVO& aState);

  private:
   char p_fileName[100];

};
