/*
 * File     :
 *   TokenVO.C
 *
 * Purpose  :
 *   This program encapsulates the Token information received from
 *   the Q330. It presents the token information in usable format.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  20 April 2002
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

#include <string.h>
#include <iostream>
#include "TokenVO.h"
#include "TokenBuffer.h"
#include "QmaDiag.h"
#include "msgs.h"
#include "global.h"
#include "qmaswap.h"

int TokenVO::processTokenBuffer(char* buf, int len)
{
  g_log << "--- Starting TokenVO processing. " << std::endl;
  //
  // It's important to use the TokenBuffer properly. (1) you must initialize
  // it with the token memory area. (2) The getTokenBuffer method is stateful
  // that is, it depends on the last thing you did with TokenBuffer.
  // basically it returns a pointer to the start of the last token. (3)
  // you can use the token to copy memory, but don't store the pointer itself
  // because that store will soon go out of scope.
  //
  TokenBuffer tb;
  tb.initialize(buf,len);  
  while(tb.hasMoreTokens())
  {
    qma_uint8 tok = tb.nextToken();
    switch (tok)
    {
       case 0:
       {
         g_log << "+++ Token 0 - Nop" << std::endl;
         break;
       }
       case 1:
       {
         g_log << "+++ Token 1 - Version" << std::endl;
         p_version = 0;
         memcpy(&p_version,(char*)tb.getTokenBitString(),1);
         break;
       }
       case 2:
       {
         g_log << "+++ Token 2 - Network and Station ID" << std::endl;
         p_netStation.initialize((char*)tb.getTokenBitString());
         break;
       }
       case 3:
       {
         g_log << "+++ Token 3 - DP NetServer" << std::endl;
	 p_dpNetServerPort = 0;
         memcpy(&p_dpNetServerPort,(char*)tb.getTokenBitString(),2);
	 p_dpNetServerPort = qma_ntohs(p_dpNetServerPort);
         break;
       }
       case 4:
       {
         g_log << "+++ Token 4 - DSS Params" << std::endl;
	 //g_log << "DSS Param Tokens not implemented" << std::endl;
         break;
       }
       case 5:
       {
         g_log << "+++ Token 5 - DP Webserver" << std::endl;
	 p_dpWebServerPort = 0;
         memcpy(&p_dpWebServerPort,(char*)tb.getTokenBitString(),2);
	 p_dpWebServerPort = qma_ntohs(p_dpWebServerPort);
         break;
       }
       case 6:
       {
         g_log << "+++ Token 6 - Clock Processing" << std::endl;
         p_clockProcessing.initialize((char*)tb.getTokenBitString());
         break;
       }
       case 7:
       {
         g_log << "+++ Token 7 - Log and Timing Info" << std::endl;
         p_logTimingInfo.initialize((char*)tb.getTokenBitString());
         break;
       }
       case 8:
       {
         g_log << "+++ Token 8 - Config Identification" << std::endl;
         p_configInfo.initialize((char*)tb.getTokenBitString());
         break;
       }
       case 9:
       {
         g_log << "+++ Token 9 - DataServer Port" << std::endl;
         memcpy(&p_dataServerPort,(char*)&buf[0],2);
	 p_dataServerPort = qma_ntohs(p_dataServerPort);
         break;
       }
       case 128:
       {
         // g_log << "+++ Token 128 - LCQ" << std::endl;
         //
         // Take two steps (1) Store the lcq in the TokenVO
	 //	lcqlist. (2) Add a LCQ to the LCQ

         LCQVO lcqvo;
         lcqvo.initialize((char*)tb.getTokenBitString(),
			       tb.getTokenBitStringLength());
         
         lcqvo.setStationCode(g_tvo.getNetStationVO().getStationCode());
         lcqvo.setNetworkCode(g_tvo.getNetStationVO().getNetworkCode());


         //
         // Add the LCQVO to the LCQ
         // Steps are: 
         //     (a) Determine which type of LCQ it is
	 //         (1) status, (2) main , (3) 24bit digitizer
	 //	(b) mask out the type field and add it in as a channel.
 
         qma_uint8 chanType;
         qma_uint8 chanNum;
         chanType = lcqvo.getChannelByte();

         if(chanType < DATA_BLOCKETTE_FLAG) // Status types are < than 0x80
         {
           qma_uint8 statType = chanType & 0xe0;
	   chanNum = chanType & 0x1f; 

           switch (statType)
           {
              case 0:
              {
                break;
              }
              case 0x20:
              {
                break;
              }
              case 0x40:
              {
                break;
              }
              case 0x60:
              {
		break;
              }
              default:
	      {
	        g_log << "xxx Unexpected Status Type : " <<
			statType << std::endl;
                break;
              }
           }
         }
         else if(chanType  < DCM_ST) // Data Types from main
         {
           //
           // Insert LCQVO into main lcq
           // Key variable are:
           // typeNum = High order bits indicating the packet type
           // mainTypeNum = High order bits with no max bit shifted to right
           //   to produce a number 0-15
           // chanNum = Channel number indicating
           // The results of this are a valid chanPosition which is used
           // to create the mainLCQ list
           //

           int chanPosition;
           //
           // Isolate the LCQ Type using the upper bits of the channel byte
           qma_uint8 typeNum = chanType & DCM; // clear chan bits

           // For main and analog channels find mainTypeNum
           // This will represent the type without the max bit set. 
           //
           qma_uint8 mainTypeNum = typeNum & 0x7f; // clear max bit
           mainTypeNum = mainTypeNum >> 3; // rotate to get rid of chann bits

           // Isolate the low three bits of channel number used here
           //
           qma_uint8 chanNum = chanType & 0x07; //clear all but low three bits


           qma_uint8 tempFreqByte = lcqvo.getFrequencyBit();
           qma_uint8 freqByte = tempFreqByte & 0x07; // Clear all but 3 bits

           if(!g_mainMap_list.validTypeChanFreqValues(mainTypeNum,
                                                      chanNum,
                                                      freqByte))
           {
	     g_log << 
	       "xxx Found invalid type/chan/freq LCQVO map: " 	
		<< (qma_uint16) mainTypeNum << "/" << 
                 (qma_uint16) chanNum << "/" << (qma_uint16) freqByte 
                 << std::endl;
           }
           else if(g_mainMap_list.TypeChanFreqInQueue(mainTypeNum,
                                                      chanNum,
                                                      freqByte))
           {
	     g_log 
	       << "xxx Found duplicate type/chan/freq LCQVO maps: " <<
	         (qma_uint16) mainTypeNum << "/" << 
                 (qma_uint16) chanNum << "/" << (qma_uint16) freqByte 
                   << std::endl;
           }
           else
           {
             chanPosition = g_mainMap_list.assignQueuePosition(mainTypeNum,
                                                               chanNum,
                                                               freqByte);
              //g_log << "--- Found pos. for Type/Chan/Freq in LCQVO map: "
              //  << (qma_uint16) mainTypeNum << "/" << (qma_uint16) chanNum 
              //  << "/" << (qma_uint16) freqByte << std::endl;
           }

	   //
           // Find channel frequency in hertz
           //

           double freq = ((double) lcqvo.getRate());
           lcqvo.setFrequencyHertz(freq);

           // Switch and case here to determine the appropriate
           // index number for this LCQ.
           //
           bool validChannel = true;
	   switch (typeNum)
           {
	     case DC_MN38:
	     {
		if(chanNum == 0)
                {
                   //g_log << "--- Found System Power Token" << std::endl;
                }
                else if(chanNum == 1)
                {
                  //g_log << "--- Found Main Current Token" << std::endl;
                }
                else if(chanNum == 2)
                {
                  //g_log << "--- Found Status Port Token" << std::endl;
                }
                else if(chanNum == 3)
                {
                  //g_log << "--- Found Opto Inputs Token" << std::endl;
                }
                else
                {
		  g_log << "xxx Invalid chanNum in DC_MN38  LCQ : " <<
			typeNum << " : " << chanNum << std::endl;
                  validChannel = false;
                }
		break;
             }
             case DC_MN816:
             {
		if(chanNum == 0)
                {
                   //g_log << "--- Found VCO Token" << std::endl;
                }
                else if(chanNum == 1)
                {
                  //g_log << "--- Found Percentage of packet buffer full" 
                  //   << std::endl;
                }
                else
                {
		  g_log << "xxx Invalid chanNum in DC_MN816 LCQ : " <<
			typeNum << " : " << chanNum << std::endl;
                  validChannel = false;
                }
		break;
             }
	     case DC_MN32:
             {
                //g_log << "xxx Unknown MN32 Token: " << std::endl;
                validChannel = false;
		break;
             }
             case DC_MN232:
             {
                if(chanNum == 0)
                {
                  //g_log << "--- Clock Channel Token" << std::endl;
                }
                else if(chanNum == 1)
                {
                  //g_log << "--- Clock phase error Token" << std::endl;
                }
                else if(chanNum == 2)
                {
                  //g_log << "--- Clock quality Token" << std::endl;
                }
                else
                {
		  g_log << "xxx Invalid chanNum in DC_MN232 LCQ : " <<
			(int) typeNum << " : " << (int) chanNum << std::endl;
			break;
                  validChannel = false;
                }            
	       break;
             }
             case DC_AG38:
             {
		if(chanNum == 0)
                {
                   //g_log << "--- Boom Position channels 0-2 Token" << std::endl;
                }
                else if(chanNum == 1)
                {
                  //g_log << "--- Boom Position channels 3-5 Token" << std::endl;
                }
                else if(chanNum == 2)
                {
                  //g_log << "--- Temperature Celsius Token" << std::endl;
                }
                else
                {
		  g_log << "xxx Invalid chanNum in DC_AG38 : " <<
			(int) typeNum << " : " << (int) chanNum << std::endl;
			break;
                  validChannel = false;
                }            
                break;
             }
             case DC_AG816:
             {
		if(chanNum == 0)
                {
                   //g_log << "--- Calibration Status Token" << std::endl;
                }
                else if(chanNum == 1)
                {
                  //g_log << "--- Analog positive voltage Token" << std::endl;
                }
                else if(chanNum == 2)
                {
                  //g_log << "--- Analog negative voltage Token" << std::endl;
                }
                else
                {
		  //g_log << "xxx Invalid chanNum in DC_AG816: " <<
		  //	(int) typeNum << " : " << (int) chanNum << std::endl;
                  validChannel = false;
                }
               break;
             }
             case DC_AG32:
             {
               g_log << "xxx Unexpected AG32 Token" << std::endl;
               validChannel = false;
               break;
             }
             case DC_AG232:
             {
               g_log << "xxx Unexpected AG232 Token" << std::endl;
               validChannel = false;
               break;
             }
             case DC_CNP38:
             {
               if(chanNum == 0)
               {
                 //g_log << "--- CNP Battery Temperature Token" << std::endl;
               }
               else
               {
		  g_log << "xxx Invalid chanNum in DC_CNP38 LCQ : " <<
	          	(int) typeNum << " : " << (int) chanNum << std::endl;
                  validChannel = false;
               }
               break;
             }
             case DC_CNP816:
             {
               g_log << "xxx Unexpected CNP816 Token" << std::endl;
               validChannel = false;
               break;
             }
             case DC_CNP316:
             {
               if(chanNum == 0)
               {
                 //g_log << "--- Battery Voltage Token" << std::endl;
               }
               else
               {
		  g_log << "xxx Invalid chanNum in DC_CNP316 LCQ : " <<
			(int) typeNum << " : " << (int) chanNum << std::endl;
                  validChannel = false;
               }      
               break;
             }
             case DC_CNP232:
             {
              g_log << "xxx Unexpected CNP232 Token" << std::endl;
              validChannel = false;
               break;
             }
             default:
             {
              g_log << "xxx Unknown channel ID: " << (int) chanType 
                << " num: " <<
                (int) chanNum << std::endl;
              g_log << "--- " << lcqvo.getSEEDName() << std::endl;
              validChannel = false;
             }
           } // End Switch on Channel Type


           if(validChannel)
           {
             //
             // Construct new LCQ and PCQ for this channel
             //
             LCQVO* lptr;
             lptr = new LCQVO[chanPosition+1];
             if(chanPosition == 0)
             {
               g_mainLCQVO_list = lptr;
             }
             else
             {
               for(int i = 0;i<chanPosition;i++)
               {
                 lptr[i] = g_mainLCQVO_list[i];
               }
               delete [] g_mainLCQVO_list;
               g_mainLCQVO_list = lptr;
             }
	     g_mainLCQVO_list[chanPosition] = lcqvo;
	     ++g_number_of_mainlcqs;
           }
           else
           {
             g_log << "xxx Invalid channel found in mainLCQ processing: " <<
                 std::endl;
           }
         } // Else if Not a main type
         else if(chanType < DCM) // These are the 24 bit data types
         {
           int pbyte = chanType;
           int chanPosition;
	   chanNum = chanType & 0x07; //Clear all but chan bits
           qma_uint8 freqByte = lcqvo.getFrequencyBit();

           if(!g_digMap_list.validChanFreqValues(chanNum,freqByte))
           {
	     g_log << 
	       "xxx Error - Found invalid Chan/Freq LCQVO maps" 	
		<< std::endl;
           }
           else if(g_digMap_list.ChanFreqInQueue(chanNum,freqByte))
           {
	     g_log 
	       << "xxx Error - Found duplicate Chan/Freq LCQVO maps" 	
		<< std::endl;
           }
           else
           {
             chanPosition = g_digMap_list.assignQueuePosition(chanNum,freqByte);
           }

           if(g_verbosity.show(D_MINOR,D_READ_TOKENS))
           {
             g_log << "--- Creating 24 bit digitizer LCQ" << std::endl;
	     g_log << "--- Digitizer LCQ : Chan / ChanNum / Freq " <<
	       (qma_uint16) chanType << " " << (qma_uint16) chanNum <<
	       " " << (qma_uint16) freqByte <<
		" returned position : " << chanPosition << std::endl;
             g_log << "--- Number LCQs in diglcq = " << 
		g_number_of_diglcqs << std::endl;
	     g_log << "--- Starting dynamic LCQ creation " << std::endl;
           }
         
	   //
	   // Find ChannelDelay for these chan,freq bytes
	   //
	   qma_int32 cfdelay = g_timeServer.getChanFreqDelay(chanNum,freqByte);
	   if(g_verbosity.show(D_MINOR,D_LCQ_CREATE))
	   {
	     g_log << "--- Found delay of: " <<
	       cfdelay << " for channel " << lcqvo.getSEEDName() << 
	       std::endl;
	   }
	   lcqvo.setFilterDelay(cfdelay);
	
	   //
           // Find channel frequency in hertz
           //

           double freq = ((double) lcqvo.getRate());
           lcqvo.setFrequencyHertz(freq);

           //
           // Construct new LCQ and PCQ for this channel
           //

           LCQVO* lptr;
           lptr = new LCQVO[chanPosition+1];
           if(chanPosition == 0)
           {
             g_digLCQVO_list = lptr;
           }
           else
           {
             for(int i = 0;i<chanPosition;i++)
             {
               lptr[i] = g_digLCQVO_list[i];
             }
             delete [] g_digLCQVO_list;
             g_digLCQVO_list = lptr;
           }
	   g_digLCQVO_list[chanPosition] = lcqvo;
	   ++g_number_of_diglcqs;
         }
         else // These are the special blockettes . Not LCQ's
         {
	   g_log << "--- LCQ for Special Blockettes " << std::endl;
	   g_log << "--- No LCQ Constructed "  << std::endl;
         }
         break;
       }
       case 129:
       {
         g_log << "+++ Token 129 - IIR Specification" << std::endl;
         break;
       }
       case 130:
       {
         g_log << "+++ Token 130 - FIR Specification" << std::endl;
         break;
       }
       case 131:
       {
         g_log << "+++ Token 131 - Control Detector Specification" 
		   << std::endl;
         break;
       }
       case 132:
       {
         g_log << "+++ Token 132 - Murdock Hut Detector Specification" 
		   << std::endl;
         break;
       }
       case 133:
       {
         g_log << "+++ Token 133 - Threshold Detector Specification" 
		   << std::endl;
         break;
       }
       case 192:
       {
         g_log << "+++ Token 192 - Comm Event Names" << std::endl;
         break;
       }
       case 193:
       {
         g_log << "+++ Token 193 - Email Alert Configuration" << std::endl;
         break;
       }
       default:
       {
	 g_log << "xxx Unknown Token type : " << tok << std::endl;
       }
    }    
  }
  return true;
}

int TokenVO::getVersion()
{
  int val = (int) p_version;
  return val;
}

NetStationVO TokenVO::getNetStationVO()
{
  return p_netStation;
}

LogTimingVO TokenVO::getLogTimingVO()
{
  return p_logTimingInfo;
}

qma_uint16 TokenVO::getDPNetServerPortNumber()
{
  return p_dpNetServerPort;
}

qma_uint16 TokenVO::getDPWebServerPortNumber()
{
  return p_dpWebServerPort;
}

ClockProcVO TokenVO::getClockProcVO()
{
  return p_clockProcessing;
}

ConfigInfoVO TokenVO::getConfigInfoVO()
{
  return p_configInfo;
}

qma_uint16 TokenVO::getDataServerPort()
{
  return p_dataServerPort;
}
