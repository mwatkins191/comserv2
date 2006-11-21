/*
 * File: ConfigVO.C
 *
 * Purpose : This class encapsulates the configuration information that
 * is read from a Mountainair configuration file. It has constructors for both
 * data and string types. There are get and set methods to access the
 * data once it is set.
 *
 *
 *
 *
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
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <sys/types.h>

#ifndef _WINNT
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif


#include "QmaTypes.h"
#include "QmaLimits.h"
#include "ConfigVO.h"
#include "QmaDiag.h"
#include "QMA_Version.h"
#include "global.h"

#if defined(LINUX) || defined(_WINNT)
#include "portingtools.h"
#endif

ConfigVO::ConfigVO(
	   qma_uint32 q330_udpaddr,
	   qma_uint16 q330_base_port,
	   qma_uint16 q330_data_port,
	   qma_uint64 q330_serial_number,
	   qma_uint64 q330_auth_code,
	   qma_uint32 qma_ipport,
	   qma_uint32 aVerbosity,
	   qma_uint32 aDiagnostic,
           char       *startmsg,
           qma_uint32 statusinterval,
           qma_uint32 datarateinterval)
{
  p_q330_udpaddr = q330_udpaddr;
  p_q330_base_port = q330_base_port;
  p_q330_data_port = q330_data_port;
  p_q330_serial_number = q330_serial_number;
  p_q330_auth_code = q330_auth_code;
  p_qma_ipport = qma_ipport;
  p_verbosity  = aVerbosity;
  p_diagnostic = aDiagnostic;
  strcpy(p_startmsg,startmsg);
  p_statusinterval = statusinterval;
  p_datarateinterval = datarateinterval;
  p_configured = true;
}


ConfigVO::ConfigVO(char* q330_udpaddr,
		   char* q330_base_port,
		   char* q330_data_port,
		   char* q330_serial_number,
		   char* q330_auth_code,
		   char* qma_ipport,
		   char* verbosity,
		   char* diagnostic,
		   char* startmsg,
                   char* statusinterval,
                   char* datarateinterval)
{
  setQ330UdpAddr(q330_udpaddr);
  setQ330BasePort(q330_base_port);
  setQ330DataPortNumber(q330_data_port);
  setQ330SerialNumber(q330_serial_number);
  setQ330AuthCode(q330_auth_code);
  setQMAIPPort(qma_ipport);
  setVerbosity(verbosity);
  setDiagnostic(diagnostic);
  setStartMsg(startmsg);
  setStatusInterval(statusinterval);
  setDataRateInterval(datarateinterval);
  p_configured = true;
}

ConfigVO::ConfigVO()
{
  p_q330_udpaddr = 0x0000;
  p_q330_base_port = 5330;
  p_q330_data_port = 1;
  p_q330_serial_number = 0x00;
  p_q330_auth_code = 0x00;
  p_qma_ipport = 6000;
  p_verbosity = D_SILENT;
  p_diagnostic = D_NO_TARGET;
  strcpy(p_startmsg,"");
  p_statusinterval = DEFAULT_STATUS_INTERVAL;
  p_datarateinterval = DEFAULT_DATA_RATE_INTERVAL;
  p_configured = false;
}
  
int ConfigVO::initialize(char* infile)
{

// reading a text file

   char buffer[256];
   std::ifstream examplefile (infile);
   if (! examplefile.is_open())
   { 
     g_log << "Error opening file"; 
     return(QMA_FAILURE); 
   }

   int result = QMA_SUCCESS;
   int i = 0;
   while (! examplefile.eof() )
   {
     examplefile.getline (buffer,100);
     if((strncmp(buffer,"#",1) == 0) || (strncmp(buffer," ",1) == 0)
        || (strncmp(buffer,"",1) == 0))
     {
       continue;
     }
     else
     {
       if(i==0)
       {
	 setQ330UdpAddr(buffer);
         //g_log << buffer << " " << i << std::endl;
       }
       else if(i==1)
       {
	 setQ330BasePort(buffer);
         //g_log << buffer << " " << i << std::endl;
       }
       else if(i==2)
       {
	 setQ330DataPortNumber(buffer);
         //g_log << buffer << " " << i << std::endl;
       }
       else if(i==3)
       {
	 setQ330SerialNumber(buffer);
         //g_log << buffer << " " << i << std::endl;
       }
       else if(i==4)
       {
	 setQ330AuthCode(buffer);
         //g_log << buffer << " " << i << std::endl;
       }
       else if(i==5)
       {
	 setQMAIPPort(buffer);
         //g_log << buffer << " " << i << std::endl;
       }
       else if(i==6)
       {
	 setVerbosity(buffer);
         //g_log << buffer << " " << i << std::endl;
       }
       else if(i==7)
       {
	 setDiagnostic(buffer);
         //g_log << buffer << " " << i << std::endl;
       }
       else if(i==8)
       {
         setStatusInterval(buffer);
       }
       else if(i==9)
       {
         setDataRateInterval(buffer);
       }
       else
       {
	 g_log << "Error on reading configuraton file." << std::endl;
         g_log << buffer << " " << i << std::endl;
	 result = QMA_FAILURE;
       }
       ++i;
     }
   }
  return result;
}

qma_uint32 ConfigVO::getQ330UdpAddr() const
{
  return p_q330_udpaddr; 
}

qma_uint32 ConfigVO::getQ330BasePort() const
{
  return p_q330_base_port;
}

qma_uint32 ConfigVO::getQ330DataPortNumber() const
{
  return p_q330_data_port;
}

qma_uint32 ConfigVO::getQ330DataPortAddress() const
{

  return (p_q330_data_port *2) + p_q330_base_port;
}

qma_uint64 ConfigVO::getQ330SerialNumber() const
{
  return p_q330_serial_number;
}

qma_uint64 ConfigVO::getQ330AuthCode() const
{
  return p_q330_auth_code;
}

qma_uint32 ConfigVO::getQMAIPPort() const
{
  return p_qma_ipport;
}

qma_uint32 ConfigVO::getVerbosity() const
{
  return p_verbosity;
}

qma_uint32 ConfigVO::getDiagnostic() const
{
  return p_diagnostic;
}

char* ConfigVO::getStartMessage() const
{
  return (char*)&p_startmsg[0];
}

qma_uint32 ConfigVO::getStatusInterval() const
{
  return p_statusinterval;
}

qma_uint32 ConfigVO::getDataRateInterval() const
{
  return p_datarateinterval;
}


//
// Set values
// 

void ConfigVO::setQ330UdpAddr(qma_uint32 a)
{
  p_q330_udpaddr = a;
}

void ConfigVO::setQ330BasePort(qma_uint32 a)
{
  p_q330_base_port = a;
}

void ConfigVO::setQ330DataPortNumber(qma_uint32 a)
{
 p_q330_data_port = a;
}
  
void ConfigVO::setQ330SerialNumber(qma_uint64 a)
{
 p_q330_serial_number = a;
}

void ConfigVO::setQ330AuthCode(qma_uint64 a)
{
 p_q330_auth_code = a;
}

void ConfigVO::setQMAIPPort(qma_uint32 a)
{
  p_qma_ipport = a;
}

void ConfigVO::setVerbosity(qma_uint32 a)
{
  p_verbosity = a;
}

void ConfigVO::setDiagnostic(qma_uint32 a) 
{
  p_diagnostic = a;
}

void ConfigVO::setStatusInterval(qma_uint32 a) 
{
  p_statusinterval = a;
}

void ConfigVO::setQ330UdpAddr(char* input)
{

  qma_uint32 ip = inet_addr(input);
  if(ip <= 0)
  {
    g_log <<"xxx Error converting input to IP. Input : " << 
	input << " :  result : " << ip << std::endl;

    p_q330_udpaddr = 0;
  }
  else
  {
    p_q330_udpaddr = inet_addr(input);
  }
}
 
void ConfigVO::setQ330BasePort(char* input)
{
  qma_uint16 port = atoi(input);
  if(port <= 0)
  {
    g_log << "xxx Error converting input to Q330 base port number : " <<  
      port << std::endl;
  }
  else
  {
    p_q330_base_port = port;
  }
}

void ConfigVO::setQ330DataPortNumber(char* input)
{
  qma_uint16 port = atoi(input);
  if(port <= 0)
  {
    g_log << "xxx Error converting input to Q330 data port number : " <<  
      port << std::endl;
  }
  else
  {
    p_q330_data_port = port;
  }
}
  
void ConfigVO::setQ330SerialNumber(char* input)
{
#ifdef _WIN32
  char ls32[9];
  char ms32[9];
  int inputLen = strlen(input);
  int ls32Start = inputLen-8;
  int ms32Start = 0;
  if(!strncmp(input, "0x", 2)) {
	ms32Start = 2;
  }
  memcpy(ls32, &(input[ls32Start]), 8);
  memcpy(ms32, &(input[ms32Start]), ls32Start);
  ls32[8] = '\0';
  ms32[8] = '\0';
  my64 serNo;
  serNo.highAndLow.lowVal = strtoul(ls32, 0, 16);
  serNo.highAndLow.highVal = strtoul(ms32, 0, 16);
  qma_uint64 port = serNo.longVal;
#else
  qma_uint64 port = strtoull(input,0,0);
#endif
  if(port <= 0)
  {
    fprintf(stderr, "xxx Error converting input to serial number : %s (%d)\n", input, port);
  }
  else
  {
    p_q330_serial_number = port;
  }
}

void ConfigVO::setQ330AuthCode(char* input)
{
#ifdef _WINNT
  qma_uint64 port = _atoi64(input);
#else
  qma_uint64 port = strtoull(input,0,0);
#endif
  if(port < 0)
  {
    g_log << "xxx Error converting input to auth code : " <<  
      port << std::endl;
  }
  else
  {
    p_q330_auth_code = port;
  }
}
  
void ConfigVO::setQMAIPPort(char* input)
{
  qma_uint16 port = atoi(input);
  if(port <= 0)
  {
    g_log << "xxx Error converting input : " << input << 
	" to QMA IP port number : " 
	      <<  port << std::endl;
  }
  else
  {
    p_qma_ipport = port;
  }
}


void ConfigVO::setVerbosity(char* input)
{
  int len = strlen (input);
  if(len > 0)
  {
    qma_uint32 level = atoi(input);

    if(level > D_EVERYTHING)
    {
      p_verbosity = D_EVERYTHING;
    }
    else
    {
      p_verbosity = level;
    }
  }
  else
  {
     p_verbosity = D_SILENT;
  }
}

void ConfigVO::setDiagnostic(char* input)
{
  int len = strlen (input);
  if(len > 0)
  {
    qma_uint32 target = atoi(input);
    p_diagnostic = target;
  }
  else
  {
    p_diagnostic = D_NO_TARGET;
  }
}

void ConfigVO::setStartMsg(char* input)
{
  strcpy(p_startmsg,"");
  sprintf(p_startmsg,"Starting QMASERV v%d.%d.%d: ",QMA_VERSION_RELEASE,
                                	            QMA_VERSION_FEATURE,
 						    QMA_VERSION_FIX);
  int res = strlen(input);
  if(res > 1)
  {
    strlcat(p_startmsg,input,76); // always null terminates the string
  }
}

void ConfigVO::setStatusInterval(char* input)
{
  int len = strlen (input);
  if(len > 0)
  {
    qma_uint32 interval = atoi(input);
    if(interval < MIN_STATUS_INTERVAL)
    {
    g_log << 
    "+++ StatusInterval in station.ini is too short." << std::endl;
    g_log << "+++ Mountainair minimum StatusInterval is: " 
      << MIN_STATUS_INTERVAL << " seconds. " << std::endl;
    g_log << "+++ Setting StatusInterval to " 
       << MIN_STATUS_INTERVAL
      << " rather than : " << interval << std::endl;
    interval = MIN_STATUS_INTERVAL;
    }
    else if(interval > MAX_STATUS_INTERVAL)
    {
    g_log << 
    "+++ StatusInterval in station.ini is too long." << std::endl;
    g_log << "+++ Mountainair maximum StatusInterval is: " 
      << MAX_STATUS_INTERVAL << " seconds. " << std::endl;
    g_log << "+++ Setting StatusInterval to " << MAX_STATUS_INTERVAL << 
      " rather than : " << interval << std::endl;
    interval = MAX_STATUS_INTERVAL;
    }
    g_log << "+++ Setting StatusInterval to: " << interval 
	<< std::endl;
    p_statusinterval = interval;
  }
  else
  {
    g_log << "+++ Setting StatusInterval to default value: "
      << DEFAULT_STATUS_INTERVAL 
      << std::endl;
    p_statusinterval = DEFAULT_STATUS_INTERVAL;
  }
}
  
void ConfigVO::setDataRateInterval(char* input)
{
  int len = strlen (input);
  if(len > 0)
  {
    qma_uint32 interval = atoi(input);
    if(interval < MIN_DATA_RATE_INTERVAL)
    {
      g_log << 
      "+++ DataRateInterval in station.ini is too small." << std::endl;
      g_log << "+++ Mountainair minimum DataRateInterval is: " <<
        MIN_DATA_RATE_INTERVAL << std::endl;
      g_log << "+++ Setting DataRateInterval to " 
        << MIN_DATA_RATE_INTERVAL
        << " rather than : " << interval << std::endl;
      interval = MIN_DATA_RATE_INTERVAL;
    }
    else if(interval > MAX_DATA_RATE_INTERVAL)
    {
      g_log << 
      "+++ DataRateInterval in station.ini is too large." << std::endl;
      g_log << "+++ Mountainair Maximum DataRateInterval is:" <<
        MAX_DATA_RATE_INTERVAL << std::endl;
      g_log << "+++ Setting DataRateInterval to " << 
        MAX_DATA_RATE_INTERVAL 
      << " rather than : " << interval << std::endl;
      interval = MAX_DATA_RATE_INTERVAL;
    }
    g_log << "+++ Setting DataRateInterval to: " << interval 
	<< std::endl;
    p_datarateinterval = interval;
  }
  else
  {
    g_log << "+++ Setting DataRateInterval to default value: " 
      << DEFAULT_DATA_RATE_INTERVAL 
      << std::endl;
    p_datarateinterval = DEFAULT_DATA_RATE_INTERVAL;
  }
}
