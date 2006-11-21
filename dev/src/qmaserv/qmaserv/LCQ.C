/*
 * File     :
 *  LCQ.C
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
#include <iostream>
#include "QmaTypes.h"
#include "QmaDiag.h"
#include "LCQVO.h"
#include "LCQ.h"
#include "Blockette.h"
#include "Verbose.h"
#include "Cleanup.h"
#include "StateMachine.h"
#include "global.h"

extern Verbose g_verbosity;
extern StateMachine g_stateMachine;

LCQ::LCQ()
{
  p_blocketteReadyToProcess = false;
  p_number_of_blockettes    = 0;
}

void LCQ::setStartTime(const BTI& startTime)
{
  p_pendingSecond = startTime;
  p_startingSecond = startTime;
  p_number_of_blockettes = 0;
  if(!true)
  {
    g_log << "--- Start time set as " <<
	startTime.drsn << " " <<
	getLCQVO().getSEEDName() << std::endl;
  }
}

void LCQ::setLCQVO(LCQVO aLCQVO)
{
  p_lcqvo = aLCQVO;
}

LCQVO LCQ::getLCQVO() const
{
  return p_lcqvo;
}

bool LCQ::insertBlockette(const Blockette bt)
{
  if(g_verbosity.show(D_EVERYTHING,D_INSERT_BLOCKETTE))
  {
    g_log << "--- Inserting blockette into LCQ : " << 
	getLCQVO().getSEEDName() <<  "  " <<
	bt.getBlocketteTime().drsn << " "
	<< bt.getBlockette().blocketteType << "  blockettes in queue : " <<
	p_number_of_blockettes << std::endl;
  }

  //
  // Check if DRSN is less than starting DRSN. If so, drop
  // the blockette.
  //
  if(bt.getBlocketteTime().drsn < p_startingSecond.drsn)
  {
    g_log << "xxx Exiting on LCQ Error - Tried to queue a blockette timestamped " <<
	"earlier than starting sample for " << getLCQVO().getSEEDName() 
	      << std::endl;
    g_log << "xxx LCQ - Current  : " << bt.getBlocketteTime().drsn 
	<< std::endl;
    g_log << "xxx LCQ - Starting : " << p_startingSecond.drsn 
	<< std::endl;
    g_stateMachine.setState(Exitting); 
    return false;
  }
  else if(p_number_of_blockettes >= MAX_BLOCKETTES_IN_LCQ)
  {
    g_log << "xxx LCQ - Too many blockettes in LCQ " <<
	getLCQVO().getSEEDName() <<
	" not transfering to Compression Queue : " <<
	p_number_of_blockettes << std::endl;
    g_log << "xxx Exiting Mountainair on this error" << std::endl;
    CleanQMA(12);
  }
  else
  {
    bool inserted = false;
    //
    // Assume blockettes are always sorted
    // 0 = earliest (smallest DSRN).

    if(p_number_of_blockettes == 0)
    {
      p_blockette_list[p_number_of_blockettes] = bt;
      ++p_number_of_blockettes;
    }
    else
    {
      for(int i=0;i<p_number_of_blockettes;i++)
      {
        if(FirstLessThanSecond(bt,p_blockette_list[i]))
        {
	  for(int x=p_number_of_blockettes;x>i;x--)
          {
            if(g_verbosity.show(D_EVERYTHING,D_MOVE_BLOCKETTE))
            {
              g_log << "--- Copied LCQ blockette " 
		<< (x-1) << " to " << x  << "  channel " <<
		getLCQVO().getSEEDName() << std::endl;
            }
            p_blockette_list[x] = p_blockette_list[x-1];
          }
          if(g_verbosity.show(D_EVERYTHING,D_INSERT_BLOCKETTE))
          {
              g_log << "--- Inserting at position : " << i << std::endl;
          }
          p_blockette_list[i] = bt;
          inserted = true;
          break;
        }
      }
      if(!inserted)
      {
        p_blockette_list[p_number_of_blockettes] = bt;
      }
      ++p_number_of_blockettes;
    }
  }

  if(g_verbosity.show(D_EVERYTHING,D_INSERT_BLOCKETTE))
  {
     for(int i = 0;i<p_number_of_blockettes;i++)
     {
        g_log << "--- DRSN : " << 
	p_blockette_list[i].getBlocketteTime().drsn << std::endl; 
     }
  }
  return true;
}

bool LCQ::pendingSecondReady() const
{

  bool result = false;

  if(p_number_of_blockettes > 0)
  {
    if(g_verbosity.show(D_EVERYTHING,D_CHECK_PENDING_BLOCKETTE))
    {
      g_log << "--- Test Pending Blckette - Lowest Rx'd " 
      << getLCQVO().getSEEDName() << " " 
      << p_blockette_list[0].getBlocketteTime().drsn << " - Pnding - " <<
          p_pendingSecond.drsn << std::endl;
    } 

    //
    // Test for invalid condition where queue packet 
    // was previously dequeued. 
    //	
    if(p_blockette_list[0].getBlocketteTime().drsn < 
	p_pendingSecond.drsn)
    {
      g_log << "xxx First DRSN less than pending : " 
	<< getLCQVO().getSEEDName() << std::endl;

      if(true)
      {
        g_log << "--- Test Pending Blckette - Last Rx'd " 
        << getLCQVO().getSEEDName() << " " 
        << p_blockette_list[0].getBlocketteTime().drsn << " - Pnding - " <<
          p_pendingSecond.drsn << std::endl;
      } 
      return result;
    }

    //
    // This is logic to determine if all packets for a Seconds are queued
    // and ready to be dequeued.
    //
    if (p_blockette_list[0].getBlocketteTime().drsn == p_pendingSecond.drsn)
    {
      //
      // Now we test for DCD32 and DCCOMP blockettes which are ready 
      // by them selves. If it's a DCMULT, we check to see that the
      // whole sequence is there.
      //

      qma_uint32 mtype = 
	p_blockette_list[0].getBlockette().blocketteType;

      if ((mtype == DC_D32) || (mtype == DC_COMP))
      {
	result = true;
      }
      else
      {
        //
        // Test to be sure all the MULT segments are there.
        //
        int  expectedSegNum = 0;
        //
        // If this for loop completes, without breaking on any conditions,
        // then result is still false and the DCMULT is not complete
        //
        for(int i=0;i<p_number_of_blockettes;i++)
        {
	    if( p_blockette_list[i].getBlocketteTime().drsn != 
		p_pendingSecond.drsn)
	    {
              g_log << "xxx Incorrect starting DRSN in mult LCQ. "
		<< std::endl;
	      break;
	    }
	    else if(p_blockette_list[i].getBlockette().blocketteType != 
		DC_MULT)
	    {
              g_log << "xxx Incorrect blockette type in mult LCQ. "
		<< std::endl;
	      break;
	    }	  
	    else
	    {
	      if(p_blockette_list[i].getBlockette().segmentNumber != 
                  expectedSegNum)
	      {
                // This case may occur because the mults are not sent
                // in order. So just break, and wait for next one to 
                // arrive. 
                if(!true)
                {
                  g_log << "xxx Out of sequence segNum in mult LCQ- Found: "
		  << p_blockette_list[i].getBlockette().segmentNumber << 
                  " expected " << expectedSegNum << std::endl;
                }
		break;
	      }
	      if(p_blockette_list[i].getBlockette().finalSegment == true)
	      {
	        result = true;
		break;
	      }
	    }
	    ++expectedSegNum;
	} // End of loop through all the segment in DCMULT

        if(!true)
        {
          g_log << "+++ Completed LCQ assemble of mults into Sec of Data "
           << " with total segments: " << expectedSegNum << " and result: " <<
		result << std::endl;
        }
      } // End of DC MUlt processing
    } // End of if current equals pending
    else
    {
      //
      // This is the standard and acceptable case when the current earliest
      // blockette is later than the blockette we want to send next. This is
      // caused when we receive packets out of order.
      // 
      if(g_verbosity.show(D_EVERYTHING,D_CHECK_PENDING_BLOCKETTE))
      {
        std:: cout << "--- New blockette is later than current time stamp : " 
	  << p_pendingSecond.drsn << std::endl;
        g_log << "xxx Blockette DRSN : " <<
	  p_blockette_list[0].getBlocketteTime().drsn << 
		" NextPacket : " <<
	  p_pendingSecond.drsn << std::endl;
      }
    }
  } // end of case where we have at least one blockette in queue
  else
  {
    if(g_verbosity.show(D_EVERYTHING,D_CHECK_PENDING_BLOCKETTE))
    {
      g_log << "--- Number of blockettes is 0 " << std::endl;
    }
  }
  return result;
}

Blockette LCQ::removeCurrentBlockette()
{

  Blockette temp;
  if(p_number_of_blockettes < 1)
  {
    g_log 
      << "xxx Error LCQ - Called removeBlockette on empty blockette list."
      << std::endl;
  }
  else
  {
    // 
    // Move blockette we are deleting out of array before
    // deleting it
    //
    temp = p_blockette_list[0];
    for(int i=0;i<p_number_of_blockettes;i++)
    {
      p_blockette_list[i] = p_blockette_list[i+1];
    }
    --p_number_of_blockettes;
    //
    // Increment to next second as current second
    //
    incrementPendingSecond();
 
    //
    // Check to see if a current blockette is the current second
    //

    p_blocketteReadyToProcess = false;
    if(p_number_of_blockettes > 0)
    { 
      if (p_blockette_list[0].getBlocketteTime().drsn == 
	    p_pendingSecond.drsn)
      {
        p_blocketteReadyToProcess = true;
      }
    }
  }
  return temp;
}








void LCQ::incrementPendingSecond()
{
  qma_uint32 sec = p_pendingSecond.drsn; 
  ++sec;
  p_pendingSecond.drsn = sec;
}



Blockette LCQ::removeAdditionalMultBlockette()
{

  Blockette temp;
  if(p_number_of_blockettes < 1)
  {
    g_log 
      << "xxx Error LCQ - Called removeBlockette on empty blockette list."
      << std::endl;
  }
  else
  {
    // 
    // Move blockette we are deleting out of array before
    // deleting it
    //
    temp = p_blockette_list[0];
    for(int i=0;i<p_number_of_blockettes;i++)
    {
      p_blockette_list[i] = p_blockette_list[i+1];
    }
    --p_number_of_blockettes;
  }
  return temp;
}





bool LCQ::FirstLessThanSecond(Blockette t1, Blockette t2)
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

int LCQ::getNumberOfBlockettesInQueue() const
{
  return p_number_of_blockettes;
}
