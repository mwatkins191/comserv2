/*
 * File     :
 *  SendCmds.C
 *
 * Purpose  :
 *  Collection of routines to send messages to the Q330. Format and send
 *  the message.
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
#include <errno.h>
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "QmaDiag.h"
#include "Verbose.h"
#include "SendCmds.h"

#include "global.h"
#include "msgs.h"

bool sendChallenge(QMA_Port& cmdPort)
{
  //
  // Send a server challenge to the Q330.
  //
  //

  c1_rqsrv m1;
  QDPHeader out_qdp;
  Packet    out_p;

  //
  // First create the data portion of message
  // Only serial number is required in this initial packet
  //

  m1.setSerialNumber(g_cvo.getQ330SerialNumber());

  //
  // Set header information
  //

  out_qdp.setCommand(C1_RQSRV_VAL);
  out_qdp.setVersion(QDP_VERSION);
  out_qdp.setDataLengthInBytes(m1.getLengthInBytes());
  out_qdp.setPacketSequence(g_cmdPacketSeq.next());
  out_qdp.setAckNumber(0x00);

  //
  // Combine header and data into packet
  //

  out_p.setQDPHeaderBitString(out_qdp.getBitString());
  out_p.setDataBitString(m1.getBitString(),m1.getLengthInBytes());
  out_p.setCRC();

  //
  // Send a series of service requests. Once we get the challenge
  // response, then we exit.
  //
  // It looks to me like the Q330 only responses to the last 
  // challenge request. If you send more than one, you'll
  // have problems connecting.
  // 

  
  int send_ok = cmdPort.write((char*)out_p.getBitString(),
				     out_p.getLengthInBytes());

  if(!send_ok)
  {
      g_log << "xxx Error on serverChallenge packet send." << 
		std::endl;
    return false;
  }
  else
  {
    if(g_verbosity.show(D_MAJOR,D_SEND_SERVER_CHALLENGE))
    {
      g_log << "--> Sent request challenge request to Q330." << std::endl;
    }
    return true;
  }
}


bool sendChallengeResponse(QMA_Port& cmdPort)
{

  QDPHeader out_qdp;
  Packet    out_p;
  c1_srvch  c1;

  //
  // Copy input packet into message
  //
  c1.setBitString(g_packet_for_tx.getDataBitString());
  c1_srvrsp resp;

  //
  // Create data portion of challenge response message
  //

  resp.setSerialNumber(g_cvo.getQ330SerialNumber());
  resp.setChallengeValue(c1.getChallengeValue());
  resp.setServerIPAddress(c1.getServerIPAddress());
  resp.setServerUDPPort(c1.getServerUDPPort());
  resp.setRegistrationNumber(c1.getRegistrationNumber());
  resp.setRandomNumber(resp.generateRandomNumber());
  resp.setMD5Result(g_cvo.getQ330AuthCode());

  //
  // Create Header
  //
  out_qdp.setCommand(C1_SRVRSP_VAL);
  out_qdp.setVersion(QDP_VERSION);
  out_qdp.setDataLengthInBytes(resp.getLengthInBytes());
  out_qdp.setPacketSequence(g_cmdPacketSeq.next());
  out_qdp.setAckNumber(0x00);

  //
  // Add header then data to packet
  //
  out_p.setDataBitString(resp.getBitString(),resp.getLengthInBytes());
  out_p.setQDPHeaderBitString(out_qdp.getBitString());
  out_p.setCRC();
        
  if(g_verbosity.show(D_MAJOR,D_SEND_SERVER_CHALLENGE))
  {
      g_log << "--> Sending server challenge response." << std::endl;
  }
  
  int send_ok = cmdPort.write((char*)out_p.getBitString(),
				     out_p.getLengthInBytes());

  if(!send_ok)
  {
      g_log << "xxx Error sending Challenge Response packet." << 
	errno << std::endl;
    return false;
  }
  else
  {
    if(g_verbosity.show(D_MAJOR,D_SEND_SERVER_CHALLENGE))
    {
      g_log << "--> Sent request challenge request to Q330." << std::endl;
    }
    return true;
  }
}

bool sendUserMessage(QMA_Port& cmdPort)
{

  QDPHeader out_qdp;
  Packet    out_p;
  c1_umsg   m1;

  //
  // Create Header
  //
  out_qdp.setCommand(C1_UMSG_VAL);
  out_qdp.setVersion(QDP_VERSION);
  out_qdp.setDataLengthInBytes(m1.getLengthInBytes());
  out_qdp.setPacketSequence(g_cmdPacketSeq.next());
  out_qdp.setAckNumber(0x00);

  //
  // Add string to message
  //
  m1.setUserMessage(g_cvo.getStartMessage());
	
  //
  // Add header to packet
  //
  out_p.setDataBitString(m1.getBitString(),m1.getLengthInBytes());
  out_p.setQDPHeaderBitString(out_qdp.getBitString());
  out_p.setCRC();
        
  if(g_verbosity.show(D_MAJOR,D_SEND_USER_MESSAGE))
  {
      g_log << "--> Sending User Msg to Q330 response." << std::endl;
  }
  
  int send_ok = cmdPort.write((char*)out_p.getBitString(),
				     out_p.getLengthInBytes());

  if(!send_ok)
  {
      g_log << "xxx Error sending User Message packet." << 
	errno << std::endl;
    return false;
  }
  else
  {
    if(g_verbosity.show(D_MAJOR,D_SEND_USER_MESSAGE))
    {
      g_log << "--> Sent User Message to Q330." << std::endl;
    }
    return true;
  }
}


bool sendStatusRequest(QMA_Port& cmdPort)
{
  bool retValue = false;
  //
  // We have registered on a data port. Now ask for configuration
  // information such as buffers and blocks. Start by request
  // status.
  //
  QDPHeader out_qdp;
  Packet    out_p;
  c1_rqstat rs;

  //
  // Set port from configuration ionformation
  //

  rs.requestGlobalAndBoomPositionStatus(g_cvo.getQ330DataPortNumber());

  //
  // Then setup Header for request flag message
  //
  //
    
  out_qdp.setCommand(C1_RQSTAT_VAL);
  out_qdp.setVersion(QDP_VERSION);
  out_qdp.setDataLengthInBytes(rs.getLengthInBytes());
  out_qdp.setPacketSequence(g_cmdPacketSeq.next());
  out_qdp.setAckNumber(0x00);

  out_p.setDataBitString(rs.getBitString(),rs.getLengthInBytes());
  out_p.setQDPHeaderBitString(out_qdp.getBitString());
  out_p.setCRC();

  int send_ok = cmdPort.write(
                            (char*)out_p.getBitString(),
                            out_p.getLengthInBytes());

  if(g_verbosity.show(D_EVERYTHING,D_STATUS_PACKETS))
  {
    g_log << "--> Sent for Global Status" << std::endl;
  }

  if(!send_ok) 
  {
    g_log << "xxx Error on packet send. Reinitializing." << std::endl;
    retValue = false;
  }
  else
  {
      retValue = true;
  }
  return retValue;
}


bool sendFlagsRequest(QMA_Port& cmdPort)
{
  bool retValue = false;
  QDPHeader out_qdp;
  Packet    out_p;
  c1_rqflgs rf;

  //
  // Set port from configuration ionformation
  //

  rf.setDataPortNumber(g_cvo.getQ330DataPortNumber());

  //
  // Then setup Header for request flag message
  //
  //
    
  out_qdp.setCommand(C1_RQFLGS_VAL);
  out_qdp.setVersion(QDP_VERSION);
  out_qdp.setDataLengthInBytes(rf.getLengthInBytes());
  out_qdp.setPacketSequence(g_cmdPacketSeq.next());
  out_qdp.setAckNumber(0x00);

  out_p.setDataBitString(rf.getBitString(),rf.getLengthInBytes());
  out_p.setQDPHeaderBitString(out_qdp.getBitString());
  out_p.setCRC();

  int send_ok = cmdPort.write(
                            (char*)out_p.getBitString(),
                            out_p.getLengthInBytes());

  if(g_verbosity.show(D_EVERYTHING,D_STATUS_FLAGS))
  {
    g_log << "--> Sent for flags" << std::endl;
  }

  if(!send_ok) 
  {
    g_log << "xxx Error on packet send. Reinitializing." << std::endl;
    retValue = false;
  }
  else
  {
      retValue = true;
  }
  return retValue;
}


bool sendDisconnect(QMA_Port& outport)
{
  bool retValue = false;
  QDPHeader out_qdp;
  Packet    out_p;
  c1_dsrv dsrv;

  dsrv.setSerialNumber(g_cvo.getQ330SerialNumber());

  out_qdp.setCommand(C1_DSRV_VAL);
  out_qdp.setVersion(QDP_VERSION);
  out_qdp.setDataLengthInBytes(dsrv.getLengthInBytes());
  out_qdp.setPacketSequence(g_cmdPacketSeq.next());
  out_qdp.setAckNumber(0x00);
    
  //
  // Move to char structure
  //
    
  out_p.setDataBitString(dsrv.getBitString(),dsrv.getLengthInBytes());
  out_p.setQDPHeaderBitString(out_qdp.getBitString());
  out_p.setCRC();

  int send_ok = g_cmdPort.write((char*)
                                 out_p.getBitString(),
			         out_p.getLengthInBytes());

  g_log << "--> Sent for disconnect." << std::endl;

  if(!send_ok) 
  {
     g_log << "xxx Error sending disconnect. " <<
	" Error Number :" << errno << std::endl;
     retValue = false;
  }
  else
  {
      retValue = true;
  }
  return retValue;
}

bool sendOpenDataPort(QMA_Port& dataPort)
{
  QDPHeader out_qdp;
  Packet    out_p;
  bool retValue = false;
  dt_open dto;
   
  out_qdp.setCommand(DT_OPEN_VAL);
  out_qdp.setVersion(QDP_VERSION);
  out_qdp.setDataLengthInBytes(dto.getLengthInBytes());
  out_qdp.setPacketSequence(g_cmdPacketSeq.next());
  out_qdp.setAckNumber(0);

  //
  // Create Packet with this data
  //

  out_p.setDataBitString(dto.getBitString(),dto.getLengthInBytes());
  out_p.setQDPHeaderBitString(out_qdp.getBitString());
  out_p.setCRC();

  int send_ok = dataPort.write((char*)
                                 out_p.getBitString(),
			         out_p.getLengthInBytes());
  if(!send_ok)
  {
    g_log << "xxxx Error sending open request to data port. Error Number: "
	      << errno << std::endl;
    retValue = false;
  }
  else
  {
    retValue = true;
  }
  return retValue;
}

bool sendTokenRequest(QMA_Port& cmdPort,int nextAddress)
{

  QDPHeader out_qdp;
  Packet    out_p;
  if(g_verbosity.show(D_MINOR,D_TOKEN_REQUEST))
  {
    g_log << "--> Sending Token Request with Addr : " << 
	nextAddress << std::endl;
  }
  bool retValue = false;
  //
  // We have registered on a data port. Now ask for configuration
  // information such as buffers and blocks. Start by request
  // flags.
  //

  c1_rqmem rm;

  //
  // Set port from configuration ionformation
  //

  rm.setStartingAddress(nextAddress);
  rm.setByteCount(C1_MAXSEG); // max mem transfer is 438 bytes
  rm.setMemoryType(rm.getDataPortToken(g_cvo.getQ330DataPortNumber()));
  //
  // Then setup Header for request flag message
  //
  //
    
  out_qdp.setCommand(C1_RQMEM_VAL);
  out_qdp.setVersion(QDP_VERSION);
  out_qdp.setDataLengthInBytes(rm.getLengthInBytes());
  out_qdp.setPacketSequence(g_cmdPacketSeq.next());
  out_qdp.setAckNumber(0x00);

  out_p.setDataBitString(rm.getBitString(),rm.getLengthInBytes());
  out_p.setQDPHeaderBitString(out_qdp.getBitString());
  out_p.setCRC();

  if(g_verbosity.show(D_MINOR,D_TOKEN_REQUEST))
  {
    g_log << "--> Request byte count : " << rm.getByteCount() << 
	"  type : " << rm.getMemoryType() << " addr : " 
	<< rm.getStartingAddress() << 
	std::endl;
  }

  int send_ok = cmdPort.write((char*)out_p.getBitString(),
                                out_p.getLengthInBytes());

  if(!send_ok) 
  {
    g_log << "xxx Error sending token request." << std::endl;
    retValue = false;
  }
  else
  {
      retValue = true;
  }
  return retValue;
}
