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
#include "ConfigVO.h"
#include "QmaDiag.h"
#include "global.h"

#if defined(linux) || defined(_WINNT)
#include "portingtools.h"
#endif



ConfigVO::ConfigVO(char* q330_udpaddr,
		   char* q330_base_port,
		   char* q330_data_port,
		   char* q330_serial_number,
		   char* q330_auth_code,
		   char* qma_ipport,
		   char* verbosity,
		   char* diagnostic,
		   char* startmsg,
                   char* statusinterval)
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
  p_configured = true;
}

ConfigVO::ConfigVO(qma_cfg cfg) {
  setQ330UdpAddr(cfg.udpaddr);
  setQ330BasePort(cfg.baseport);
  setQ330DataPortNumber(cfg.dataport);
  setQ330SerialNumber(cfg.serialnumber);
  setQ330AuthCode(cfg.authcode);
  setQMAIPPort(cfg.ipport);
  setVerbosity(cfg.verbosity);
  setDiagnostic(cfg.diagnostic);
  setStartMsg(cfg.startmsg);
  setStatusInterval(cfg.statusinterval);
  setLogLevel(cfg.loglevel);
  setContinuityFileDir(cfg.contFileDir);
  setSourcePortControl(cfg.sourceport_control);
  setSourcePortData(cfg.sourceport_data);
  setFailedRegistrationsBeforeSleep(cfg.failedRegistrationsBeforeSleep);
  setMinutesToSleepBeforeRetry(cfg.minutesToSleepBeforeRetry);
  setDutyCycle_MaxConnectTime(cfg.dutycycle_maxConnectTime);
  setDutyCycle_BufferLevel(cfg.dutycycle_bufferLevel);
  setDutyCycle_SleepTime(cfg.dutycycle_sleepTime);
  p_configured = true;
}

ConfigVO::ConfigVO()
{
  strcpy(p_q330_udpaddr, "");
  p_q330_base_port = 5330;
  p_q330_data_port = 1;
  p_q330_serial_number = 0x00;
  p_q330_auth_code = 0x00;
  p_qma_ipport = 6000;
  p_verbosity = D_SILENT;
  p_diagnostic = D_NO_TARGET;
  strcpy(p_startmsg,"");
  p_statusinterval = DEFAULT_STATUS_INTERVAL;
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

char * ConfigVO::getQ330UdpAddr() const
{
  return (char *)p_q330_udpaddr; 
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

qma_uint32 ConfigVO::getLogLevel() const {
  return p_logLevel;
}

qma_uint16 ConfigVO::getSourcePortControl() const {
  return p_sourcePortControl;
}

qma_uint16 ConfigVO::getSourcePortData() const {
  return p_sourcePortData;
}

qma_uint16 ConfigVO::getFailedRegistrationsBeforeSleep() const {
  return p_failedRegistrationsBeforeSleep;
}

qma_uint16 ConfigVO::getMinutesToSleepBeforeRetry() const {
  return p_failedRegistrationsBeforeSleep;
}

char * ConfigVO::getContinuityFileDir() const {
  return (char *)p_contFileDir;
}

qma_uint16 ConfigVO::getDutyCycle_MaxConnectTime() const {
  return p_dutycycle_maxConnectTime;
}
qma_uint16 ConfigVO::getDutyCycle_SleepTime() const {
  return p_dutycycle_sleepTime;
}
qma_uint16 ConfigVO::getDutyCycle_BufferLevel() const {
  return p_dutycycle_bufferLevel;
}

//
// Set values
// 

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

  strcpy(p_q330_udpaddr, input);
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
  // this whole function is useless, IPPort is not used at all anymore.  
  qma_uint16 port = atoi(input);
  if(port <= 0)
  {
    p_qma_ipport = 0;
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
  sprintf(p_startmsg,"Starting QMASERV v%d.%d.%d: ",MAJOR_VERSION,
                                	            MINOR_VERSION,
 						    RELEASE_VERSION);
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
  
void ConfigVO::setContinuityFileDir(char *input) {
  strcpy(this->p_contFileDir, input);
}

void ConfigVO::setSourcePortControl(char *input) {
  // make the default 0
  if(!strcmp(input, "")|| !strcmp(input, "0")) {
    p_sourcePortControl = 0;
    return;
  }

  qma_uint16 port = atoi(input);
  if(port <= 0)
  {
    g_log << "xxx Error converting input to port number : " <<  
      port << std::endl;
  }
  else
  {
    p_sourcePortControl = port;
  }

}
void ConfigVO::setSourcePortData(char *input) {
  // make the default 0
  if(!strcmp(input, "") || !strcmp(input, "0")) {
    p_sourcePortData = 0;
    return;
  }
  qma_uint16 port = atoi(input);
  if(port <= 0)
  {
    g_log << "xxx Error converting input to port number : " <<  
      port << std::endl;
  }
  else
  {
    p_sourcePortData = port;
  }
}

void ConfigVO::setFailedRegistrationsBeforeSleep(char *input) {
  if(!strcmp(input, "") || !strcmp(input, "0")) {
    p_failedRegistrationsBeforeSleep = 0;
    return;
  }
  p_failedRegistrationsBeforeSleep = atoi(input);
  if(p_failedRegistrationsBeforeSleep <= 0) {
    g_log << "xxx Error converting input to num registrations : " << input << std::endl;
  }
}

void ConfigVO::setMinutesToSleepBeforeRetry(char *input) {
  if(!strcmp(input, "") || !strcmp(input, "0")) {
    p_minutesToSleepBeforeRetry = 0;
    return;
  }
  p_minutesToSleepBeforeRetry = atoi(input);
  if(p_minutesToSleepBeforeRetry <= 0) {
    g_log << "xxx Error converting input to num minutes : " << input << std::endl;
  }
}

void ConfigVO::setLogLevel(char *input) {
  int logLevel = 0;
  char *tok;
  char localInput[255];
  strcpy(localInput, input);

  tok = strtok(localInput, ", ");

  while(tok != NULL) {
    if(!strncasecmp("SD", tok, 2)) {
      logLevel |= VERB_SDUMP;
    }
    if(!strncasecmp("CR", tok, 2)) {
      logLevel |= VERB_RETRY;
    }
    if(!strncasecmp("RM", tok, 2)) {
      logLevel |= VERB_REGMSG;
    }
    if(!strncasecmp("VB", tok, 2)) {
      logLevel |= VERB_LOGEXTRA;
    }
    if(!strncasecmp("SM", tok, 2)) {
      logLevel |= VERB_AUXMSG;
    }
    if(!strncasecmp("PD", tok, 2)) {
      logLevel |= VERB_PACKET;
    }
    tok = strtok(NULL, ", ");
  }
  
  p_logLevel = logLevel;
}

void ConfigVO::setDutyCycle_MaxConnectTime(char *input) {
  if(!strcmp(input, "") || !strcmp(input, "0")) {
    p_dutycycle_maxConnectTime = 0;
    return;
  }
  p_dutycycle_maxConnectTime = atoi(input);
  if(p_dutycycle_maxConnectTime <= 0) {
    g_log << "xxx Error converting input to num minutes : " << input << std::endl;
  }
}

void ConfigVO::setDutyCycle_SleepTime(char *input) {
  if(!strcmp(input, "") || !strcmp(input, "0")) {
    p_dutycycle_sleepTime = 0;
    return;
  }
  p_dutycycle_sleepTime = atoi(input);
  if(p_dutycycle_sleepTime <= 0) {
    g_log << "xxx Error converting input to num minutes : " << input << std::endl;
  }
}

void ConfigVO::setDutyCycle_BufferLevel(char *input) {
  if(!strcmp(input, "") || !strcmp(input, "0")) {
    p_dutycycle_bufferLevel = 0;
    return;
  }
  p_dutycycle_bufferLevel = atoi(input);
  if(p_dutycycle_bufferLevel <= 0) {
    g_log << "xxx Error converting input to buffer level : " << input << std::endl;
  }
}
