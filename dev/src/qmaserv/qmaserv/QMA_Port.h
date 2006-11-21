/*
 *
 * File     :
 *  QMA_Port.h
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
#ifndef QMA_PORT_H
#define QMA_PORT_H

#include <sys/types.h>

#ifndef _WINNT
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#ifdef Q3302EW
 extern "C" {
# include <local_earthworm_complex_funcs.h>
# include <earthworm.h>
 };
#endif

#include "QmaTypes.h"
#include "QmaLimits.h"

class QMA_Port
{
  public:

    QMA_Port();
    ~QMA_Port();

    bool initialize(qma_uint32 dp_port,
		    qma_uint32 q330_port,
		    qma_uint32 q330_ip);

    bool initialize(qma_uint32 dp_port,
		    qma_uint32 q330_port,
		    qma_uint32 q330_ip,
                    qma_uint32 statusinterval,
                    qma_uint32 datarateinterval);

    bool   isOpen();
    int    write(char* buf,int length);
    int    read(unsigned char*);
    void   closePort();
    void   resetConnection();
    void   adjustCalcInterval();
    void   setRateCalcInterval(qma_uint32 status,qma_uint32 datarate);
    int    getRateCalcInterval();
    void   setPacketMemorySize(const qma_uint32 aSize);
    float  getPacketBufferPercentFree(qma_uint32 aReading);

    double getRxCurrentPackets() const;
    double getRxCurrentBytes() const;
    double getRxTotalPackets() const;
    double getRxTotalBytes() const;
    double getRxDataRate() const;
    double getRxPacketRate() const;
 
    double getTxTotalPackets() const;
    double getTxTotalBytes() const;
    double getTxDataRate() const;
    double getTxPacketRate() const;

    double secondsOfData() const;

 private:
    bool 	       p_isOpen;
    char 	       p_buf[MAX_BYTES_IN_PACKET];
    int  	       p_sockfd;
    int  	       p_numbytes;
    struct sockaddr_in p_dp_addr;   // connector's address information
    struct sockaddr_in p_dp2_addr;  // connector's address information
    struct sockaddr_in p_q330_addr; //
    time_t             p_start_time;

    double             p_rx_total_packets;
    double             p_rx_total_bytes;

    double             p_tx_total_packets;
    double             p_tx_total_bytes;

    double             p_rx_current_packets;
    double             p_rx_current_bytes;
 
    qma_uint32         p_calc_interval;
    qma_uint32         p_mem_size;
}; 
#endif
