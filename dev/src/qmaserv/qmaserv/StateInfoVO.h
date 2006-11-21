/*
 * File     :
 *  StateInfoVO.h
 *
 * Purpose : This class encapsulates the state information that
 * is read from a Mountainair continutity. 
 *
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  5 October 2002
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
#ifndef StateInfoVO_H
#define StateInfoVO_H

#include "BTI.h"

class StateInfoVO 
{

  public:

    StateInfoVO() {};
    ~StateInfoVO() {};

  BTI   getTimeInfo() const;
  void  setTimeInfo(const BTI& aTime);
  BTI p_timeInfo;
};

#endif
