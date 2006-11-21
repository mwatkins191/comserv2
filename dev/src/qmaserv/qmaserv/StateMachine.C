/*
 *
 * File     :
 *  StateMachine.C
 *
 * Purpose  :
 *  This is a simple class to maintain states for use as a state machine.
 *
 * Author   :
 *  Phil Maechling 
 *
 *
 * Mod Date :
 *  7 March 2002
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
 */

#include <iostream>
#include "StateMachine.h"
#include "ConfigVO.h"
#include "QmaLimits.h"
#include "QmaDiag.h"
#include "Verbose.h"
#include "global.h"

extern Verbose g_verbosity;

StateMachine::StateMachine()
{
  p_state = OpeningLocalPorts;
}
    
void StateMachine::setState(MainStates state)
{
  p_state = state;
  if(g_verbosity.show(D_MAJOR,D_SET_STATE))
  {
    g_log << "+++ State set to : " << p_state << std::endl;
  }
}

MainStates StateMachine::getState()
{
  return p_state;
}
