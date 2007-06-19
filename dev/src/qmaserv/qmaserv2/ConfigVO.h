/*
 * File     :
 *  ConfigVO.h
 *
 * Purpose : This class encapsulates the configuration information that
 * is read from a Mountainair configuration file. It has constructors for both
 * data and string types. There are get and set methods to access the
 * data once it is set.
 *
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
#ifndef _ConfigVO_H
#define _ConfigVO_H

int read_config(char* configFileName);

#include "QmaTypes.h"
#include "qmacfg.h"

class ConfigVO {

  public:


  ConfigVO(char* q330_udpaddr,
	   char* q330_base_port,
	   char* q330_data_port,
	   char* q330_serial_number,
	   char* q330_auth_code,
	   char* qma_ipport,
	   char* verbosity,
	   char* diagnotic,	
	   char* startmsg,
           char* statusinterval);
  ConfigVO(qma_cfg);

  ConfigVO();
  ~ConfigVO() {};

  int initialize(char* configfile);

  char * getQ330UdpAddr() const;
  qma_uint32 getQ330BasePort() const;
  qma_uint32 getQ330DataPortNumber() const; // 1-4
  qma_uint32 getQ330DataPortAddress() const; // Base plus data port *2
  qma_uint64 getQ330SerialNumber() const;
  qma_uint64 getQ330AuthCode() const;
  qma_uint32 getQMAIPPort() const;
  qma_uint32 getVerbosity() const;
  qma_uint32 getDiagnostic() const;
  char*      getStartMessage() const;
  qma_uint32 getStatusInterval() const;
  qma_uint32 getLogLevel() const;
  char *     getContinuityFileDir() const;
  qma_uint16 getSourcePortControl() const;
  qma_uint16 getSourcePortData() const;
  qma_uint16 getFailedRegistrationsBeforeSleep() const;
  qma_uint16 getMinutesToSleepBeforeRetry() const;
  qma_uint16 getDutyCycle_MaxConnectTime() const;
  qma_uint16 getDutyCycle_SleepTime() const;
  qma_uint16 getDutyCycle_BufferLevel() const;
  qma_int8   getMulticastEnabled() const;
  qma_uint16 getMulticastPort() const;
  char *     getMulticastHost() const;

  void setQ330BasePort(qma_uint32);
  void setQ330DataPortNumber(qma_uint32);
  void setQ330SerialNumber(qma_uint64);
  void setQ330AuthCode( qma_uint64);
  void setQMAIPPort(qma_uint32);
  void setVerbosity(qma_uint32);
  void setDiagnostic(qma_uint32);
  void setStatusInterval(qma_uint32);

  void setQ330UdpAddr(char* input);
  void setQ330BasePort(char* input);
  void setQ330DataPortNumber(char* input);
  void setQ330SerialNumber(char* input);
  void setQ330AuthCode(char* input);
  void setQMAIPPort(char* input);
  void setVerbosity(char* input);
  void setDiagnostic(char* input);
  void setStartMsg(char* input);
  void setStatusInterval(char* input);
  void setLogLevel(char *input);
  void setContinuityFileDir(char *input);
  void setSourcePortControl(char *input);
  void setSourcePortData(char *input);
  void setFailedRegistrationsBeforeSleep(char *input);
  void setMinutesToSleepBeforeRetry(char *input);
  void setDutyCycle_MaxConnectTime(char *input);
  void setDutyCycle_SleepTime(char *input);
  void setDutyCycle_BufferLevel(char *input);
  void setMulticastEnabled(char * input);
  void setMulticastPort(char * input);
  void setMulticastHost(char *input);
 
 private:

  char       p_q330_udpaddr[255];
  qma_uint16 p_q330_base_port;
  qma_uint16 p_q330_data_port;
  qma_uint64 p_q330_serial_number;
  qma_uint64 p_q330_auth_code;
  qma_uint32 p_qma_ipport;
  char 	     p_startmsg[80];
  qma_uint32 p_statusinterval;
  qma_uint32 p_verbosity;
  qma_uint32 p_diagnostic;
  bool       p_configured;
  qma_uint32 p_logLevel;
  char       p_contFileDir[256];
  qma_uint16 p_sourcePortData;
  qma_uint16 p_sourcePortControl;
  qma_uint16 p_failedRegistrationsBeforeSleep;
  qma_uint16 p_minutesToSleepBeforeRetry;
  qma_uint16 p_dutycycle_maxConnectTime;
  qma_uint16 p_dutycycle_sleepTime;
  qma_uint16 p_dutycycle_bufferLevel;
  qma_int8   p_multicast_enabled;
  qma_uint16 p_multicast_port;
  char       p_multicast_host[256];
};

#endif
