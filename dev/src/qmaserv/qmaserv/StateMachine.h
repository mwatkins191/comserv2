/*
 *
 * File     :
 *  StateMachine.h
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

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

enum MainStates {OpeningLocalPorts,
		 RequestingServerChallenge,
	         RequestingChallengeResponse,
		 RequestingStatus,
		 RequestingFlags,
		 RequestingTokens,
		 SendingUserMessage,
		 AcquiringData,
                 Resetting, 
		 Exitting};

class StateMachine
{

  public:
    
    StateMachine();
   ~StateMachine() {};
   
   void setState(MainStates state);
   MainStates getState();
   
  private:

   MainStates p_state;

};

#endif
