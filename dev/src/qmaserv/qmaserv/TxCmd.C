/*
 * File     :
 *  TxCmd.C
 *
 * Purpose  :
 *  This routine checks the current state and transmits any required messages.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  27 July 2002
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
#include "TxCmd.h"
#include "QMA_Port.h"
#include "StateMachine.h"
#include "SendCmds.h"

#include "global.h"


void tx_cmd()
{
  MainStates curState = g_stateMachine.getState();
  
  switch(curState)
  {
    //****************************
    // RequestingServerChallenge indicates we have not received a
    // registration challenge from the Q330. Our action is to
    // send in a challenge.
    //***************************
    case RequestingServerChallenge:
    {
      if(g_cmdTimer.started())
      {
	if(g_cmdTimer.elapsed())
	{
          // Put no limit on the number of Sever Challenge requests.
          //
          //if(g_cmdTimer.retryLimitReached())
          //{
          //  if(true)
          //  {
          //    g_log 
          //		<< "--- Challenge Timer retry limit reached : " 
          //		<< std::endl;
          //  }
	  //  g_stateMachine.setState(Exitting);
          //}
	  //else
          //{
	    if(!true)
            {
              g_log << "--- No response from Q330 to Challenge Request " <<
		std::endl;
            }
            g_cmdTimer.restartInterval();
	    sendChallenge(g_cmdPort);
            time_t nowtime = time(0);
            g_log << "+++ TxCmd RequestingChallenge at: " <<
               asctime(gmtime(&nowtime));
          //}
	}
	else
	{
	  // continue without taking any action.
	}
      }
      else
      {
	sendChallenge(g_cmdPort);
        time_t nowtime = time(0);
        g_log << "+++ TxCmd RequestingChallenge at: " << 
               asctime(gmtime(&nowtime));
	g_cmdTimer.setRetryInterval(CHALLENGE_RETRY_INTERVAL);
	g_cmdTimer.setRetryCount(CHALLENGE_RETRY_COUNT);
	g_cmdTimer.start();
        if(true)
        {
          g_log << "--- Started Challenge Timer: " << std::endl;
        }
      }
      break;
    }

    //****************************
    // SendingChallengeResponse indicates we have received a challenge
    // from the Q330. Our action is to send in a challenge response.
    //***************************
    case RequestingChallengeResponse:
    {
      if(g_cmdTimer.started())
      {
	if(g_cmdTimer.elapsed())
	{
          if(g_cmdTimer.retryLimitReached())
          {
            if(true)
            {
              g_log << 
		"--- Started Challenge Response Timer retry limit reached : " 
		<< std::endl;
            }
	    g_stateMachine.setState(Exitting);
          }
	  else
          {
	    if(true)
            {
              g_log << 
		"--- Challenge Response Timer elapsed. Restarting : " 
			<< std::endl;
            }
            g_cmdTimer.restartInterval();
	    sendChallengeResponse(g_cmdPort);
            g_log << "+++ TxCmd RequestingChallengeReponse." << std::endl;
          }
	}
	else
	{
	  // continue without taking any action.
	}
      }
      else
      {
	sendChallengeResponse(g_cmdPort);
        g_log << "+++ TxCmd RequestingChallengeReponse." << std::endl;
	g_cmdTimer.setRetryInterval(CHALLENGE_RETRY_INTERVAL);
	g_cmdTimer.setRetryCount(CHALLENGE_RETRY_COUNT);
	g_cmdTimer.start();
        if(true)
        {
          g_log << "--- Started Challenge Response Timer: " << std::endl;
        }
      }
      break;
    }

    //****************************
    // Requesting Status indicates the Q330 has acked us as a valid
    // Registered DP, and we are ready to request status and tokens.
    //***************************
    case RequestingStatus:
    {

      if(g_cmdTimer.started())
      {
	if(g_cmdTimer.elapsed())
	{
    
          if(g_cmdTimer.retryLimitReached())
          {
            if(true)
            {
              g_log << 
		"--- Status Request Retry Timer retry limit reached : " 
		<< std::endl;
            }
	    g_stateMachine.setState(Exitting);
          }
	  else
          {
            if(true)
            {
              g_log << "--- Status Request Timer elapsed. Restarting: " 
			<< std::endl;
            }
            g_cmdTimer.restartInterval();
	    sendStatusRequest(g_cmdPort);
            g_log << "+++ TxCmd Sending Status Request." << std::endl;
          }
	}
	else
	{
	  // continue without taking any action.
	}
      }
      else
      {
	sendStatusRequest(g_cmdPort);
        g_log << "+++ TxCmd Sending Status Request." << std::endl;
	g_cmdTimer.setRetryInterval(STATUS_RETRY_INTERVAL);
	g_cmdTimer.setRetryCount(STATUS_RETRY_COUNT);
	g_cmdTimer.start();
        if(true)
        {
          g_log << "--- Started Status Request Timer: " << std::endl;
        }
      }
      break;
    }


    //****************************
    // Requesting Status indicates the Q330 has acked us as a valid
    // Registered DP, and we are ready to request status and tokens.
    //***************************
    case RequestingFlags:
    {
      if(g_cmdTimer.started())
      {
	if(g_cmdTimer.elapsed())
	{

          if(g_cmdTimer.retryLimitReached())
          {
            if(true)
            {
              g_log << 
		"--- Flags Request Retry Timer retry limit reached : " 
		<< std::endl;
            }
	    g_stateMachine.setState(Exitting);
          }
	  else
          {
            if(true)
            {
              g_log << "--- Flags Request Timer elapsed. Resetting: " 
			<< std::endl;
            }
            g_cmdTimer.restartInterval();
	    sendFlagsRequest(g_cmdPort);
            g_log << "+++ TxCmd Sending Flags Request." << std::endl;
          }
	}
	else
	{
	  // continue without taking any action.
	}
      }
      else
      {
	sendFlagsRequest(g_cmdPort);
        g_log << "+++ TxCmd Sending Flags Request." << std::endl;
	g_cmdTimer.setRetryInterval(FLAGS_RETRY_INTERVAL);
	g_cmdTimer.setRetryCount(FLAGS_RETRY_COUNT);
	g_cmdTimer.start();
        if(true)
        {
          g_log << "--- Started Flags Request Timer: " << std::endl;
        }
      }
      break;
    }

    //****************************
    // Requesting Token indicates we have valid status from the Q330
    // and now we are requesting the tokens to configure our LCQs.
    //***************************
    case RequestingTokens:
    {
   
      if(g_cmdTimer.started())
      {
	if(g_cmdTimer.elapsed())
	{

          if(g_cmdTimer.retryLimitReached())
          {
            if(true)
            {
              g_log << 
		"--- Requesting Tokens Retry Timer retry limit reached : " 
		<< std::endl;
            }
	    g_stateMachine.setState(Exitting);
          }
	  else
          {
            if(true)
            {
              g_log << "--- Requesting Tokens Timer elapsed: " << 
		std::endl;
            }
            g_cmdTimer.restartInterval();
	    sendTokenRequest(g_cmdPort,g_nextTokenAddress);
	    g_log << "+++ TxCmd Requesting Tokens." << std::endl;
          }
	}
	else
	{
	  // continue without taking any action.
	}
      }
      else
      {
	sendTokenRequest(g_cmdPort,g_nextTokenAddress);
        g_log << "+++ TxCmd Requesting Tokens." << std::endl;
	g_cmdTimer.setRetryInterval(TOKEN_RETRY_INTERVAL);
	g_cmdTimer.setRetryCount(TOKEN_RETRY_COUNT);
	g_cmdTimer.start();
        if(true)
        {
          g_log << "--- Requesting Tokens Timer started: " << std::endl;
        }
      }
      break;
    }


    //****************************
    // Requesting Token indicates we have valid status from the Q330
    // and now we are requesting the tokens to configure our LCQs.
    //***************************
    case SendingUserMessage:
    {
      int res = strlen(g_cvo.getStartMessage());
      if(res > 0)
      {

        if(g_cmdTimer.started())
        {
	  if(g_cmdTimer.elapsed())
	  {
           
            if(g_cmdTimer.retryLimitReached())
            {
              if(true)
              {
                g_log << 
		  "--- Send User Message Timer retry limit reached : " 
		  << std::endl;
              }
	      g_stateMachine.setState(Exitting);
            }
	    else
            {
              if(true)
              {
                g_log << "--- Send User Message Timer elapsed: " 
			  << std::endl;
              }
              g_cmdTimer.restartInterval();
	      sendUserMessage(g_cmdPort);
              g_log << "+++ TxCmd SendUser Message." << std::endl;
            }
	  }
	  else
	  {
	    // continue without taking any action.
	  }
        }
        else
        {
	  sendUserMessage(g_cmdPort);
          g_log << "+++ TxCmd SendUser Message." << std::endl;
	  g_cmdTimer.setRetryInterval(USER_MESSAGE_RETRY_INTERVAL);
	  g_cmdTimer.setRetryCount(USER_MESSAGE_RETRY_COUNT);
	  g_cmdTimer.start();
          if(true)
          {
            g_log << "--- User Message Timer started: " << std::endl;
          }
        }
      }
      else
      {
	g_stateMachine.setState(AcquiringData);
      }
      break;
    }

    //****************************
    // While acquiring data, reset if we don't get any data for
    // TIMEOUT period of time. Also, periodically request Status,
    // and repeat the data port open request.
    //***************************
    case AcquiringData:
    {
     if(g_dataPortTimer.started())
      {
	if(g_dataPortTimer.elapsed())
	{
          if(g_dataPortTimer.retryLimitReached())
          {
            if(true)
            {
              g_log << 
	  "--- Data Port DT_OPEN retry limit reached. Resetting Mountainair : " 
		<< std::endl;
            }
	    g_stateMachine.setState(Resetting);
          }
	  else
          {
            if(true)
            {
              g_log << "--- No packets rx'd on data port for seconds: " 
			<< OPEN_DATA_PORT_RETRY_INTERVAL << std::endl;
            }
	    sendOpenDataPort(g_dataPort);
            g_log << "+++ TxCmd Sending DT_OPEN" << std::endl;
            g_dataPortTimer.restartInterval();
          }
	}
	else
	{
	  // continue without taking any action.
	}
      }
      else
      {
	sendOpenDataPort(g_dataPort);
        g_log << "+++ TxCmd Sending DT_OPEN" << std::endl;
	g_dataPortTimer.setRetryInterval(OPEN_DATA_PORT_RETRY_INTERVAL);
	g_dataPortTimer.setRetryCount(OPEN_DATA_PORT_RETRY_COUNT);
	g_dataPortTimer.start();
        if(true)
        {
          g_log << "--- Started Data Port Open Timer: " << std::endl;
        }
      }

     //
     // Status retry interval. No max retry counts for this status timer.
     //

     if(g_statusTimer.started())
      {
	if(g_statusTimer.elapsed())
	{ 
          if(!true)
	  {
            g_log << "--- Sent Timed Status Request: " << std::endl;
          }
	  sendStatusRequest(g_cmdPort);
          g_statusTimer.restartInterval();
	   
	}
	else
	{
	  // continue without taking any action.
	}
      }
      else
      {
	sendStatusRequest(g_cmdPort);
        if(!true)
        {
          g_log << "--- Sent Timed Status Request: " << std::endl;
        }
	g_statusTimer.setRetryInterval(g_cvo.getStatusInterval());
	g_statusTimer.start();
        if(true)
        {
          g_log << "--- Status Request Timer Started: " << std::endl;
        }
      }
      break;
    }

    //****************************
    // Requesting Status indicates the Q330 has acked us as a valid
    // Registered DP, and we are ready to request status and tokens.
    //***************************
    case Resetting:
    {
   
      if(g_cmdTimer.started())
      {
	if(g_cmdTimer.elapsed())
	{

          if(g_cmdTimer.retryLimitReached())
          {
            if(true)
            {
              g_log << 
		"--- Disconnect Request Retry Timer retry limit reached : " 
		<< std::endl;
            }
	    g_stateMachine.setState(Exitting);
          }
	  else
          {
            if(true)
            {
              g_log << "--- Disconnect Timer elapsed. Restarting: " 
			<< std::endl;
            }
            g_cmdTimer.restartInterval();
	    sendDisconnect(g_cmdPort);
            g_log << "+++ TxCmd Sending Disconnect Request." << std::endl;
          }
	}
	else
	{
	  // continue without taking any action.
	}
      }
      else
      {
	sendDisconnect(g_cmdPort);
        g_log << "+++ TxCmd Sending Disconnect Request." << std::endl;
	g_cmdTimer.setRetryInterval(DISCONNECT_RETRY_INTERVAL);
	g_cmdTimer.setRetryCount(DISCONNECT_RETRY_COUNT);
	g_cmdTimer.start();
        if(true)
        {
          g_log << "--- Started Disconnect Request Timer: " << std::endl;
        }
      }
      break;
    }

    //****************************
    //Exiting indicates an unrecoverable error has occured. This
    // may mean we tried a command it's max retry number of times.
    //***************************
    case Exitting:
    {
      g_reset = true;
      g_done = true; 
      break;
    }
    default:
    {
      g_log << "xxx Error in TxCmd. Unknown state found: " <<
	curState << " Exitting " << std::endl;
      g_stateMachine.setState(Exitting);
      break;
    }
  }
  return;
}
