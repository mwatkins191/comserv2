/*
 *
 * File     :
 *  QMA_Port.C
 *
 * Purpose  :
 *  Define a class for input ports and output ports 
 *
 * Author   :
 *  Phil Maechling
 *
 *
 * Mod Date :
 *  4 March 2002
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
#include <string.h>
#include <ctime>
#include <sys/types.h>
#include <fcntl.h>

#ifndef _WIN32
# include <netdb.h>
# include <unistd.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <arpa/inet.h>
#else
# include <winsock2.h>
#endif

#include <errno.h>
#include "QmaLimits.h"
#include "QmaTypes.h"
#include "QmaDiag.h"
#include "QMA_Port.h"
#include "ConfigVO.h"
#include "Packet.h"
#include "QDPHeader.h"
#include "Verbose.h"
#include "Cleanup.h"
#include "global.h"

extern Verbose g_verbosity;

QMA_Port::QMA_Port()
{


  p_rx_current_packets = 0.0;
  p_rx_current_bytes = 0.0;

  p_rx_total_packets = 0.0;
  p_rx_total_bytes = 0.0;
  p_tx_total_packets = 0.0;
  p_tx_total_bytes = 0.0;
  p_calc_interval = 300;
  p_mem_size = 0;
  p_start_time = time(0); // current time
}

QMA_Port::~QMA_Port()
{
  if(p_isOpen)
  {
#ifdef Q3302EW
    SocketClose(p_sockfd);
#else
    close(p_sockfd);
#endif
  }
  p_isOpen = false;
  
}


bool QMA_Port::initialize(qma_uint32 dp_port,
		          qma_uint32 q330_port,
		          qma_uint32 q330_ip,
                          qma_uint32 status,
                          qma_uint32 datarate)
{

  setRateCalcInterval(status,datarate);
  return initialize(dp_port,
                    q330_port,
                    q330_ip);
}


bool QMA_Port::initialize(qma_uint32 dp_port,
		          qma_uint32 q330_port,
		          qma_uint32 q330_ip)
{

  p_isOpen = false;

  if ((p_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
  {
        g_log << "xxx Error getting socket. Error Number : " << 
		errno << std::endl;
        p_isOpen = false;
        return p_isOpen;
  }

/* Set the reuse flag. This will allow us to have */
/* multiple programs listening */
/* to the same IP address */
/* Allow reuse of socket addresses */

  unsigned int ruflag = 1 ; /* turn on REUSE option */
  if (setsockopt(p_sockfd, SOL_SOCKET,
                      SO_REUSEADDR, (char *) &ruflag,
                      sizeof(ruflag)) < 0)
  {
    g_log << "xxx Error setting socket reuse option. Error Number: " <<
		errno << std::endl;
    p_isOpen = false;
    return p_isOpen;
  }

  p_dp_addr.sin_family = AF_INET;
  p_dp_addr.sin_port = htons(dp_port);
  p_dp_addr.sin_addr.s_addr = INADDR_ANY;
  memset(&(p_dp_addr.sin_zero),'\0',8);
  p_dp2_addr= p_dp_addr;

  if( bind(p_sockfd,(sockaddr*)&p_dp_addr,sizeof(struct sockaddr)) == -1)
  {
    g_log << "xxx Error binding socket. Error Number: " << 
	errno <<std::endl;
    p_isOpen = false;
  }
  else
  {

    /* This was an important setting. The comserv code uses FNDELAY */
    /* which is phased out but is essentially the same option as O_NONBLOCK. */
    /* The idea is that qmaserv will not sit on a socket read. If no data */
    /* is available, it will exit the recev, with an errno, and continue. */
    /* If this option is not set, then the recv will block, and the comserv */
    /* clients will not receive service in a timely manner. */

#ifdef _WIN32
    /* windows does this a bit differently */
    qma_uint32 nonBlockingMode = 1;
    if(ioctlsocket(p_sockfd, FIONBIO, &nonBlockingMode) != 0)
#else
    int flags = fcntl(p_sockfd,F_GETFL,0);
    flags = flags | O_NONBLOCK;
    if(fcntl(p_sockfd,F_SETFL,flags) == -1) // -1 is setfl error return
#endif
    {
      g_log << "xxx Error setting socket option non-block. Error Number: " 
		<< errno <<std::endl;
      p_isOpen = false;
    }
    else
    {
      p_isOpen = true;
    }
  }

  /*
   * Destination Address
  */

  p_q330_addr.sin_family = AF_INET;
  p_q330_addr.sin_port = htons(q330_port);
  p_q330_addr.sin_addr.s_addr = q330_ip;
  memset(&(p_q330_addr.sin_zero),'\0',8);

  if(g_verbosity.show(D_MAJOR,D_NETIP))
  {
    g_log << "--- Configured to send packets to " << 
	inet_ntoa(p_q330_addr.sin_addr) << " on port " << 
	std::dec << p_q330_addr.sin_port << std::endl;
  }
  resetConnection();
  return p_isOpen;
}


bool QMA_Port::isOpen()
{
  return p_isOpen;
}

int QMA_Port::write(char* buf,int length) 
{
  bool result = false;

  if(g_verbosity.show(D_MINOR,D_PORT_ON_WRITE))
  {
    g_log << "--- Sending packet to " << 
      inet_ntoa(p_q330_addr.sin_addr) << " on port " << 
      p_q330_addr.sin_port << std::endl;
    g_log << "--- Tx packet length: " 
      << length << std::endl;
  }

  if(g_verbosity.show(D_EVERYTHING,D_PRINT_PACKET_ON_TX))
  {
    Packet p;
    p.setBitString((unsigned char *)buf,length);
    g_log << "--- Tx Packet: " << std::endl;
    p.printPacketContents();
  }

  int bytes_sent = sendto(p_sockfd,
                            buf,
                            length,0,
			    (const struct sockaddr *)&p_q330_addr,
			    sizeof(struct sockaddr));


  if(g_verbosity.show(D_MINOR,D_PORT_ON_WRITE))
  {
    g_log << "--- Sent packets to " << inet_ntoa(p_q330_addr.sin_addr) << 
	" on port " << 
         p_q330_addr.sin_port << 
        " of size : " << bytes_sent << std::endl;
  }

  if(bytes_sent == -1)
  {
        g_log << "xxx Error on packet write. Error Number: " << 
		errno << std::endl;
        result = false;
  }
  else
  {
    ++p_tx_total_packets;
    p_tx_total_bytes = p_tx_total_bytes + bytes_sent;
    result = true;
  } 
  return result;
}


int  QMA_Port::read(unsigned char* outbuf)
{
  int result = -1;
  int len = MAX_BYTES_IN_PACKET;
  int numbytes = 0;
  numbytes = recv(p_sockfd,p_buf,MAX_BYTES_IN_PACKET,0); 
  if(numbytes <0)
  {
#ifdef _WIN32
	if(WSAGetLastError() != WSAEWOULDBLOCK)
#else
    if(errno != EWOULDBLOCK)
#endif
    {
        g_log << "xxx Error on port read. Error number: " <<
		errno << std::endl;
        return(-1);
    }
    else
    {
      numbytes = 0;
      return numbytes;
    }
  }
  else
  {
    memcpy(outbuf,p_buf,numbytes);
    ++p_rx_total_packets;
    ++p_rx_current_packets;
    p_rx_total_bytes = p_rx_total_bytes + numbytes;
    p_rx_current_bytes = p_rx_current_bytes + numbytes;
  }

  if(g_verbosity.show(D_EVERYTHING,D_PORT_ON_READ))
  {
      g_log << "--- Got packet from of length : " << 
	  std::dec << numbytes << std::endl;
  }

  if(numbytes < 0)
  {
	g_log << "xxx Error reading port with Error Number: " << 
	  errno << std::endl;
  }
  else if(numbytes < QDP_HEADER_SIZE_IN_BYTES)
  {
      g_log << "xxx Error Received data less than QPD header size: " 
		<< numbytes << std::endl;
  }

  if(g_verbosity.show(D_EVERYTHING,D_PRINT_PACKET_ON_RX))
  {
    Packet p;
    p.setBitString((unsigned char *)p_buf,numbytes);
    g_log << "--- Rx Packet: ";
    p.printPacketContents();
    QDPHeader in_qdp;
    in_qdp.setBitString(p.getQDPHeaderBitString());

    g_log << "Rx'd command  : " << (qma_uint32) in_qdp.getCommand() 
	      << std::endl;
    g_log << "Rx'd version  : " << (qma_uint32) in_qdp.getVersion()  
	      << std::endl;
    g_log << "Rx'd length   : " << (qma_uint32) in_qdp.getLengthInBytes() 
	      << std::endl;
    g_log << "Rx'd seqno    : " << 
	(qma_uint32) in_qdp.getPacketSequence() << std::endl;
    g_log << "Rx'd acknumber: " << (qma_uint32) in_qdp.getAckNumber() 
	      << std::endl;
  }
  return numbytes;
}

void QMA_Port::closePort()
{
#ifdef Q3302EW
  SocketClose(p_sockfd);
#else
  int res = close(p_sockfd);
#endif
  p_isOpen = false;
}

double QMA_Port::getRxCurrentPackets() const
{
  return p_rx_current_packets;
}

double QMA_Port::getRxCurrentBytes() const
{
  return p_rx_current_bytes;
}

double QMA_Port::getRxTotalPackets() const
{
  return p_rx_total_packets;
}

double QMA_Port::getRxTotalBytes() const
{
  return p_rx_total_bytes;
}

double QMA_Port::getTxTotalPackets() const
{
  return p_tx_total_packets;
}

double QMA_Port::getTxTotalBytes() const
{
  return p_tx_total_bytes;
}

double QMA_Port::getRxDataRate() const
{
  double retval = 0.0;
  double elapsed = secondsOfData();
  if( (elapsed > 0) && (p_rx_current_bytes > 0) )
  {
    retval = p_rx_current_bytes/elapsed;
  }
  return retval;
}

double QMA_Port::getRxPacketRate() const
{
  double retval = 0.0;
  double elapsed = secondsOfData();
  if( (elapsed > 0) && (p_rx_current_packets > 0) )
  {
    retval = p_rx_current_packets/elapsed;
  }
  return retval;
}

double QMA_Port::getTxDataRate() const
{
  double retval = 0.0;
  double elapsed = secondsOfData();
  if ( (elapsed > 0) && (p_tx_total_bytes > 0) )
  {
    retval =  p_tx_total_bytes/elapsed;
  }
  return retval;
}

double QMA_Port::getTxPacketRate() const
{
  double retval = 0.0;
  double elapsed = secondsOfData();
  if ( (elapsed > 0) && (p_tx_total_packets) )
  {
    retval =  p_tx_total_packets/elapsed;
  }
  return retval;
}

void QMA_Port::adjustCalcInterval() 
{
  double elapsed = secondsOfData();
  double interval = getRateCalcInterval();
  if(elapsed > interval)
  {
    //
    // Here we set the p_start_time to now.
    // The possibility of an elapsed time of
    // 0 seconds is handled in secondsOfData os
    // we don't handle it here. Seconds of Data
    // of 0 would cause a divide by zero.
    //
    time_t now = time(0);
    p_start_time = now;
    p_rx_current_bytes = 0;
    p_rx_current_packets = 0;
  }
}

void QMA_Port::resetConnection()
{

  p_rx_current_packets = 0.0;
  p_rx_current_bytes = 0.0;

  p_rx_total_packets = 0.0;
  p_rx_total_bytes = 0.0;
  p_tx_total_packets = 0.0;
  p_tx_total_bytes = 0.0;
  p_start_time = time(0); // current time
  if(g_verbosity.show(D_MAJOR,D_PORT_RESET))
  {
    g_log << "+++ Port connection reset at : " 
	  << asctime(gmtime(&p_start_time));
  }
}

int QMA_Port::getRateCalcInterval()
{
  return p_calc_interval;
}

void QMA_Port::setRateCalcInterval(qma_uint32 status, qma_uint32 datarate)
{

  //
  // These range errors are treated as fatal errors. This is because
  // they should have been checked prior to arriving here, and if they
  // weren't we are in an invalid state, and we want to log the error 
  // and restart.
  //
  if( (status < MIN_STATUS_INTERVAL) ||
      (status > MAX_STATUS_INTERVAL) )
  {
    g_log << "xxx Invalid status rate in QMA_Port :" << status <<
      " Exitting." << std::endl;
    CleanQMA(12);
  }
  
  if( (datarate < MIN_DATA_RATE_INTERVAL) ||
      (datarate > MAX_DATA_RATE_INTERVAL) )
  {
    g_log << "xxx Invalid data rate in QMA_Port: " << datarate <<
     " Exitting." << std::endl;
    CleanQMA(12);
  }
  p_calc_interval = status * datarate;
}

double QMA_Port::secondsOfData() const
{
  time_t now = time(0);
  //
  // Make sure we don't return 0 for a divide by 0 error.
  //
  double elapsed = now - p_start_time;
  if(elapsed <= 0)
  {
    elapsed = 1.0; //
  }
  return elapsed;
}

void QMA_Port::setPacketMemorySize(const qma_uint32 aSize)
{
  g_log << "--- Setting Packet Mem size to: " << aSize << std::endl;
  p_mem_size = aSize;
}


float QMA_Port::getPacketBufferPercentFree(qma_uint32 aReading)
{
  float res = 0.0;
  if(!true)
  {
    g_log << "--- MemSize: " << p_mem_size << " Buffer Usage: " << 
      aReading << std::endl;
  }
  if(p_mem_size > 0)
  {
    if(p_mem_size >= aReading)
    {
      float diff = (float) (p_mem_size - aReading);
      float avail = (float) p_mem_size;
      res = diff/avail * 100.0;
    }
    else
    {
      g_log << "xxx Packet Buffer Usage greater than Packet Buffer: " <<
	p_mem_size << " " << aReading << std::endl;
    }
  }
  return res;
}
