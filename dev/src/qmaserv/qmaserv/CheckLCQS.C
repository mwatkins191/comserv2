/*
 * File     :
 *   CheckLCQS.C
 *
 * Purpose  :
 *  This is the routines which checks to see if logical queues
 *   have current data in them, and if so, compresss and send it.
 *
 * Author   :
 *  Phil Maechling
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
 *
 */
#include <iostream>
#include "QmaTypes.h"
#include "QmaDiag.h"
#include "CheckLCQS.h"

#ifndef Q3302EW
#  include "clink.h"
#endif

#include "global.h"
#include "Verbose.h"
#include "FrameUtils.h"
#include "SplitSOD.h"

extern Verbose g_verbosity;

bool check_lcqs()
{

  if(g_verbosity.show(D_MINOR,D_CHECK_LCQ))
  { 
    g_log << "--- CheckLogicalQueue" << std::endl;
    g_log << "--- StartTimeNeeded : " << g_startingDRSNNeeded << std::endl;
    g_log << "--- Size of Queues LCQ - PCQ " << 
	g_number_of_diglcqs << std::endl;
  }

  //
  // There is no point in checking these queues for processing before
  // we have received the first DRSN, so test for that and skip unless
  // it's set.
  //
  if(!g_startingDRSNNeeded)
  {
    bool queuesHavePendingData  = true;
    bool foundPendingQueue = false;
    int  numberOfQueuesChecked = 0;
    while(queuesHavePendingData)
    {
      foundPendingQueue = false;
      for(int i=0;i<g_number_of_diglcqs;i++)
      {
        if(g_digLCQ_list[i].pendingSecondReady())
        {
	  foundPendingQueue = true;
          if(g_verbosity.show(D_EVERYTHING,D_CHECK_LCQ))
          {
            g_log << "--- Extracting Second of data from LCQ : " << 
		g_digLCQ_list[i].getLCQVO().getSEEDName() << std::endl; 
          }
          
          SecondOfData sod;
          sod.setCompletionPacket(false);
          Blockette outb = g_digLCQ_list[i].removeCurrentBlockette();
	  qma_uint32 blktype = outb.getBlockette().blocketteType;

          if(g_verbosity.show(D_MINOR,D_LCQ_REMOVE))
          {
            g_log << "--- Removed blockette : " <<
	      g_digLCQ_list[i].getLCQVO().getSEEDName()<< "  " 
	      << outb.getBlocketteTime().drsn << 
		" type :" << outb.getBlockette().blocketteType << 
		std::endl;
          }

	  if(blktype == DC_MULT)
	  {
	     if(outb.getBlockette().segmentNumber != 0)
	     {
	      g_log << "xxx Error on MULT Packet. First segment not 0" 
		<< std::endl;
	     }
             int multBlockettes = 0;
	     bool moreBlockettesInSecond = true;
	     sod.addBlockette(outb);
	     ++multBlockettes;
             while(moreBlockettesInSecond)
	     {
                outb = g_digLCQ_list[i].removeAdditionalMultBlockette();
	        if (outb.getBlocketteTime().drsn != 
		    sod.getBlocketteTimeInfo().drsn)
	        {
		  g_log << "xxx Error with DC_MULT. DRSN not equal" 
			<< std::endl;
		  moreBlockettesInSecond = false;
	        }
	        else if(outb.getBlockette().finalSegment == true)
	        {
		  sod.addBlockette(outb);
		  moreBlockettesInSecond = false;
                  if(!true)
                  {
                    g_log << "--- SoD Mult blockettes on final: " 
		    << sod.getNumberOfBlockettes() << " data words " << 
                    sod.getNumberOfDataWords() << std::endl;
                  }
	        }
	        else
	        {
		  sod.addBlockette(outb);
                  if(!true)
                  {
                    g_log << "--- SoD Mult blockettes: " 
		    << sod.getNumberOfBlockettes() << " data words " << 
                    sod.getNumberOfDataWords() << std::endl;
                  }
	        }
	     } 
	  }
          else // Packet was DC_COMP or DC_D32
	  { 
	    sod.addBlockette(outb);
	  }

	  if(g_verbosity.show(D_MINOR,D_LCQ))
          {
	    g_log << "--- Source LCQ: " << 
	      g_digLCQ_list[i].getLCQVO().getSEEDName() << std::endl;
            g_log << "--- Dest   PCQ: " << 
	      g_digPCQ_list[i].getLCQVO().getSEEDName() << std::endl;
	    g_log << "--- Blockette Type : " << 
	      sod.getBlocketteType() << std::endl;
	    g_log << "--- Blockette 1 Type : " << 
	      sod.p_blockette_array[0].getBlockette().blocketteType << 
	      std::endl;
          }

          if(sod.getBlocketteType() == DC_MULT)
          {
            g_log << "+++ Creating Comp from Mult" << std::endl;
            moveMultSegsIntoFirstSeg(sod);
          }

          //
          // All seconds of data constructed. Now split those too large
          // to fit into a single packet.
          if(sod.getNumberOfDataWords() > MAX_WORDS_IN_PACKET)
          {
            if(g_verbosity.show(D_EVERYTHING,D_SPLITS))
            { 
              g_log 
              << "+++ Splitting Comp packet with dataWordsTotal: " 
		<< std::dec << sod.getNumberOfDataWords() << 
		" and mapBytes: " <<
	        std::dec << 
                sod.p_blockette_array[0].getBlockette().mapLengthInBytes
              << std::endl;
            }

            SecondOfData completionSOD =
              splitSOD(g_digPCQ_list[i].getLCQVO().getFrequencyHertz(),sod);

            if(!g_digPCQ_list[i].addDataToQueue(sod))
            {
              g_log << "xxx Resetting on Continuity Error 1" << std::endl;
              g_stateMachine.setState(Resetting);
            }

            if(completionSOD.completionPacket())
            {
              if(!g_digPCQ_list[i].addDataToQueue(completionSOD))
              {
                g_log << "xxx Resetting on Continuity Error 2" << std::endl;
                g_stateMachine.setState(Resetting); 
              }
            }
          }
          else
          {
            if(!g_digPCQ_list[i].addDataToQueue(sod))
            {
              g_log << "xxx Resetting on Continuity Error 3" << std::endl;
	      g_log << "xxx Resetting on Queue: " << i  << " of total:" 
                << g_number_of_diglcqs << std::endl;
              g_stateMachine.setState(Resetting);
            }
          }

	} // End of "if packet pending" 
	++numberOfQueuesChecked;
      } // End of loop through LCQs

      if(g_verbosity.show(D_MINOR,D_CHECK_LCQ))
      {
        g_log << "--- Checked number of LCQs : " << 
	  numberOfQueuesChecked << std::endl;
      }
      if(!foundPendingQueue)
      {
	queuesHavePendingData = false;
      }

    } // End of loop "While Queues have pending data

    // Now Repeat for mainLCQS

    queuesHavePendingData  = true;
    foundPendingQueue = false;
    numberOfQueuesChecked = 0;
    while(queuesHavePendingData)
    {
      foundPendingQueue = false;
      for(int i=0;i<g_number_of_mainlcqs;i++)
      {
        if(g_mainLCQ_list[i].getNumberOfBlockettesInQueue() > 0)
        {
	  foundPendingQueue = true;
          if(g_verbosity.show(D_EVERYTHING,D_CHECK_LCQ))
          {
            g_log << "--- Extracting Second of data from mainLCQ : " << 
		g_mainLCQ_list[i].getLCQVO().getSEEDName() << std::endl; 
          }
          
          SecondOfData sod;
          sod.setCompletionPacket(false);
          Blockette outb = g_mainLCQ_list[i].removeCurrentBlockette();
	  qma_uint32 blktype = outb.getBlockette().blocketteType;

          if(g_verbosity.show(D_EVERYTHING,D_CHECK_LCQ))
          {
            g_log << "--- Removed MainLCQ blockette : " <<
	      g_mainLCQ_list[i].getLCQVO().getSEEDName()<< "  " 
	      << outb.getBlocketteTime().drsn << 
		" type :" << outb.getBlockette().blocketteType << 
		std::endl;
          }

	  if(blktype == DC_MULT)
	  {
	     g_log << "xxx Unexpected Mult in Main Channels. Exit" 
               << std::endl;
             g_stateMachine.setState(Exitting);
             return NothingToDo;
	  }
          else // Packet was DC_COMP or DC_D32
	  { 
	    sod.addBlockette(outb);
	  }

	  if(g_verbosity.show(D_MINOR,D_LCQ))
          {
	    g_log << "--- Source LCQ: " << 
	      g_mainLCQ_list[i].getLCQVO().getSEEDName() << std::endl;
            g_log << "--- Dest   PCQ: " << 
	      g_mainPCQ_list[i].getLCQVO().getSEEDName() << std::endl;
	    g_log << "--- Blockette Type : " << 
	      sod.getBlocketteType() << std::endl;
	    g_log << "--- Blockette 1 Type : " << 
	      sod.p_blockette_array[0].getBlockette().blocketteType << 
	      std::endl;
          }

          if(!g_mainPCQ_list[i].addDataToQueue(sod))
          {
              g_log << "xxx Resetting on Continuity Error 3" << std::endl;
	      g_log << "xxx Resetting on Queue: " << i  << " of total:" 
                << g_number_of_mainlcqs << std::endl;
              g_stateMachine.setState(Resetting);
          }
	} // End of "if Main packet pending" 
	++numberOfQueuesChecked;
      } // End of loop through LCQs

      if(!foundPendingQueue)
      {
	queuesHavePendingData = false;
      }

    } // End of loop "While mainLCQ Queues have pending data

   // Done Repeat for mainLCQS
  } // End of if No Starting DRSN
  return NothingToDo;
}
