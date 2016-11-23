#include "global.h"
#include "clink.h"
#include "lib330Interface.h"
//#include "PacketQueue.h"
#include "portingtools.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

/* 
 * Modifications:
 *   21 May 2012 - DSN - Close mcastSocketFD on Lib330Interface destruction.
 *   27 Feb 2013 - DSN - fix memset arg in Lib330Interface::initializeRegistrationInfo
 *   2015/09/25  - DSN - added log message when creating lib330 interface.
 */

double g_timestampOfLastRecord = 0;

Lib330Interface::Lib330Interface(char *stationName, ConfigVO ourConfig) {
  pmodules mods;
  tmodule *mod;
  int x;

  g_log << "+++ lib330 Interface created" << std::endl;	//::
  this->currentLibState = LIBSTATE_IDLE;
  this->initializeCreationInfo(stationName, ourConfig);
  this->initializeRegistrationInfo(ourConfig);

  mods = lib_get_modules();
  g_log << "+++ Lib330 Modules:" << std::endl;
  for(x=0; x <= MAX_MODULES - 1; x++) {
    mod = &(*mods)[x];
    if(!mod->name[0]) {
      continue;
    }
    if( !(x % 5)) {
      if(x > 0) {
	g_log << std::endl;
      }
      if(x < MAX_MODULES-1) {
	g_log << "+++ ";
      }
    }
    g_log << (char *) mod->name << ":" << mod->ver << " ";
  }
  g_log << std::endl;
  g_log << "+++ Initializing station thread" << std::endl;

  if(ourConfig.getMulticastEnabled()) {
    g_log << "+++ Multicast Enabled:" << std::endl;
    if( (mcastSocketFD = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
      ourConfig.setMulticastEnabled((char *)"0");
      g_log << "XXX Unable to create multicast socket: errno=" << errno << " (" << strerror(errno) << ")"<< std::endl;
    }
    
    memset(&(mcastAddr), 0, sizeof(mcastAddr));
    mcastAddr.sin_family = AF_INET;
    mcastAddr.sin_addr.s_addr = inet_addr(ourConfig.getMulticastHost());
    mcastAddr.sin_port = htons(ourConfig.getMulticastPort());
    memcpy(&multicastChannelList, (char *)ourConfig.getMulticastChannelList(), sizeof(multicastChannelList));
    g_log << "+++    Multicast IP: " << ourConfig.getMulticastHost() << std::endl;
    g_log << "+++    Multicast Port: " << ourConfig.getMulticastPort() << std::endl;
    g_log << "+++    Multicast Channels:" << std::endl;
    for(int c=0; c < 256; c++) {
        char *chan = multicastChannelList[c];
	if(! *chan) {
            break;
        }
        g_log << "+++        " << (char *)chan << std::endl;
    }
  }
  lib_create_context(&(this->stationContext), &(this->creationInfo));
  if(this->creationInfo.resp_err == LIBERR_NOERR) {
    g_log << "+++ Station thread created" << std::endl;
  } else {
    this->handleError(creationInfo.resp_err);
  }

  
}


Lib330Interface::~Lib330Interface() {
  enum tliberr errcode;
  g_log << "+++ Cleaning up lib330 Interface" << std::endl;
  this->startDeregistration();
  while(this->getLibState() != LIBSTATE_IDLE) {
    sleep(1);
  }
  this->changeState(LIBSTATE_TERM, LIBERR_CLOSED);
  while(getLibState() != LIBSTATE_TERM) {
    sleep(1);
  }
  if (mcastSocketFD >= 0) {
    g_log << "+++ Multicast socket close" << std::endl;
    int err = close(mcastSocketFD);
    if (err != 0) g_log << "XXX Error closing multicast socket: errno=" << errno << " (" << strerror(errno) << ")"<< std::endl;
    mcastSocketFD = -1;
  }
  errcode = lib_destroy_context(&(this->stationContext));
  if(errcode != LIBERR_NOERR) {
    this->handleError(errcode);
  }
  g_log << "+++ lib330 Interface destroyed" << std::endl;
}

void Lib330Interface::startRegistration() { 
  enum tliberr errcode;
  this->ping();
  g_log << "+++ Starting registration with Q330" << std::endl;
  errcode = lib_register(this->stationContext, &(this->registrationInfo));
  if(errcode != LIBERR_NOERR) {
    this->handleError(errcode);
  }  
}

void Lib330Interface::startDeregistration() {
  g_log << "+++ Starting deregistration from Q330" << std::endl;
  this->changeState(LIBSTATE_IDLE, LIBERR_NOERR);
}

void Lib330Interface::changeState(enum tlibstate newState, enum tliberr reason) {
  lib_change_state(this->stationContext, newState, reason);
}

void Lib330Interface::startDataFlow() {
  g_log << "+++ Requesting dataflow to start" << std::endl;
  this->changeState(LIBSTATE_RUN, LIBERR_NOERR);
}

void Lib330Interface::ping() {
  lib_unregistered_ping(this->stationContext, &(this->registrationInfo));
}

int Lib330Interface::waitForState(enum tlibstate waitFor, int maxSecondsToWait, void(*funcToCallWhileWaiting)()) {
  struct timespec t;
  t.tv_sec = 0;
  t.tv_nsec = 250000000;
  for(int i=0; i < (maxSecondsToWait*4); i++) {
    if(this->getLibState() != waitFor) {
      nanosleep(&t, NULL);
      funcToCallWhileWaiting();
    } else {
      return 1;
    }
  }
  return 0;
}


void Lib330Interface::displayStatusUpdate() {
  enum tlibstate currentState;
  enum tliberr lastError;
  topstat libStatus;
  time_t rightNow = time(NULL);

  currentState = lib_get_state(this->stationContext, &lastError, &libStatus);

  // do some internal maintenence if required (this should NEVER happen)
  if(currentState != getLibState()) {
    string63 newStateName;
    g_log << "XXX Current lib330 state mismatch.  Fixing..." << std::endl;
    lib_get_statestr(currentState, &newStateName);
    g_log << "+++ State change to '" << newStateName << "'" << std::endl;
    this->setLibState(currentState);
  }


  // version and localtime
  g_log << "+++ " << APP_VERSION_STRING << " status for " << libStatus.station_name 
	<< ". Local time: " << ctime(&rightNow);
  
  // BPS entries
  g_log << "--- BPS from Q330 (min/hour/day): ";
  for(int i=(int)AD_MINUTE; i <= (int)AD_DAY; i = i + 1) {
    if(libStatus.accstats[AC_READ][i] != INVALID_ENTRY) {
      g_log << (int) libStatus.accstats[AC_READ][i] << "Bps";
    } else {
      g_log << "---";
    }
    if(i != AD_DAY) {
      g_log << "/";
    } else {
      g_log << std::endl;
    }
  }

  // packet entries
  g_log << "--- Packets from Q330 (min/hour/day): ";
  for(int i=(int)AD_MINUTE; i <= (int)AD_DAY; i = i + 1) {
    if(libStatus.accstats[AC_PACKETS][i] != INVALID_ENTRY) {
      g_log << (int) libStatus.accstats[AC_PACKETS][i] << "Pkts";
    } else {
      g_log << "---";
    }
    if(i != AD_DAY) {
      g_log << "/";
    } else {
      g_log << std::endl;
    }
  }


  // percent of the buffer left, and the clock quality
  g_log << "--- Q330 Packet Buffer Available: " << 100-(libStatus.pkt_full) << "%, Clock Quality: " << 
    libStatus.clock_qual << "%" << std::endl;
}

enum tlibstate Lib330Interface::getLibState() {
  return this->currentLibState;
}

/**
 * lib330 callbacks
 */

void Lib330Interface::state_callback(pointer p) {
  tstate_call *state;

  state = (tstate_call *)p;

  if(state->state_type == ST_STATE) {
    g_libInterface->setLibState((enum tlibstate)state->info);
  }
}


// Caltech 2011 modification to one-second multicast packet:
//  1.  New structure with explicitly sized data types.
//  2.  All values are in network byte order.
void Lib330Interface::onesec_callback(pointer p) {
  onesec_pkt msg;
  tonesec_call *src = (tonesec_call*)p;
  int retval;
  char *filterItem;
  char temp[32];

  //translate tonesec_call to onesec_pkt;
  strncpy(temp,src->station_name,9);
  strcpy(msg.net,strtok((char*)temp,(char*)"-"));
  strcpy(msg.station,strtok(NULL,(char*)"-"));
  strcpy(msg.channel,src->channel);
  strcpy(msg.location,src->location);
  
  msg.rate = htonl((int)src->rate);

  msg.timestamp_sec = htonl((int)src->timestamp);
  msg.timestamp_usec = htonl((int)((src->timestamp - (double)(((int)src->timestamp)))*1000000));

  //#ifdef ENDIAN_LITTLE
  //std::cout <<" TimeStamp for "<<msg.net<<"."<<msg.station<<"."<<msg.channel<<std::endl;
  //g_log <<" : "<<msg.timestamp<<std::endl;
  //SwapDouble((double*)&msg.timestamp);
  //g_log <<" (after swap) TimeStamp for "<<msg.net<<"."<<msg.station<<"."<<msg.channel;
  //g_log <<" : "<<msg.timestamp<<std::endl;
  //#endif

  for(int i=0;i<src->rate;i++){
    msg.samples[i] = htonl((int)src->samples[i]);
  }
  int msgsize = ONESEC_PKT_HDR_LEN + src->rate * sizeof(int);

  for(int i=0; i < 255; i++) {
    filterItem = &(multicastChannelList[i][0]);
    if(! *filterItem) {
      break;
    }
    if(!strcmp(filterItem, msg.channel)) {
      retval = sendto(mcastSocketFD, &msg, msgsize, 0, (struct sockaddr *) &(mcastAddr), sizeof(mcastAddr));
      if(retval < 0) {
	g_log << "XXX Unable to send multicast packet: " << strerror(errno) << std::endl;
      } 
    } 
  }
}


void Lib330Interface::miniseed_callback(pointer p) {
  tminiseed_call *data = (tminiseed_call *) p;
  int sendFailed;
  short packetType = 0;

  /*
   * Handle non-data miniseed records
   * this whole construct is bizarre looking and the if/else
   * should be rolled into the switch.  The if/else once had a
   * purpose but not anymore.
   */
  if(data->packet_class != PKC_DATA) {
    switch(data->packet_class) {
    case PKC_EVENT:
      packetType = DETECTION_RESULT;
      break;
    case PKC_CALIBRATE:
      packetType = CALIBRATION;
      break;
    case PKC_TIMING:
      packetType = CLOCK_CORRECTION;
      break;
    case PKC_MESSAGE:
      packetType = COMMENTS;
      break;
    case PKC_OPAQUE:
      packetType = BLOCKETTE;
      break;
    }
  } else {
    packetType = RECORD_HEADER_1;
    if(data->timestamp < g_timestampOfLastRecord) {
      g_log << "XXX Packets being received out of order" << std::endl;
      g_timestampOfLastRecord = data->timestamp;
    }
  }

  // put the packet in the queue
  packetQueue.enqueuePacket((char *)data->data_address, data->data_size, packetType);
  
  /*
   * Sending to comserv is now done in the main thread, to avoid problems
   * arising from comserv not being threadsafe

  sendFailed = comlink_send((char *)data->data_address, data->data_size, packetType);
  
  if(sendFailed) {
    if(sendFailed == -1) {
      // this means that there is something wrong with this packet
      g_log << "XXX Malformed packet - failed to write to comserv (Size: " << data->data_size << ")" << std::endl;
    } else if(sendFailed == 1) {
      // this means that a client is blocking
      g_log << "XXX Client blocking.  Queueing packet" << std::endl;
      packetQueue.enqueuePacket((char *)data->data_address, data->data_size, packetType);
      g_reset = 1;
    }
  }
  */

}

/**
 * TODO - add interface to PacketQueue to check which queue we need
 * next, and make sure it's not blocking before we dequeue
 */
int Lib330Interface::processPacketQueue() {
  int sendFailed = 0;

  if(comlink_dataQueueBlocking()) {
    return 0;
  }

  QueuedPacket thisPacket = packetQueue.dequeuePacket();

  while(thisPacket.dataSize != 0) {
    sendFailed = comlink_send((char *)thisPacket.data, thisPacket.dataSize, thisPacket.packetType);
    if(sendFailed) {
      return 0;
    } else {
      thisPacket = packetQueue.dequeuePacket();
    }
  }  
  return 1;
}

void Lib330Interface::archival_miniseed_callback(pointer p) {
  //tminiseed_call *data = (tminiseed_call *) p;
  //g_log << "Archival (" << data->channel << ") with timestamp: " << data->timestamp << " " << data->data_size << " bytes" << std::endl;
}

void Lib330Interface::msg_callback(pointer p) {
  tmsg_call *msg = (tmsg_call *) p;
  string95 msgText;
  char dataTime[32];

  lib_get_msg(msg->code, &msgText);
  
  if(!msg->datatime) {
    g_log << "LOG {" << msg->code << "} " << msgText << " " << msg->suffix << std::endl;
  } else {
    jul_string(msg->datatime, (char *) (&dataTime));
    g_log << "LOG {" << msg->code << "} " << " [" << dataTime << "] " << msgText << " "  << msg->suffix << std::endl;
  }    

}


/**
 * For internal use only
 */
int Lib330Interface::sendUserMessage(char *msg) { return 0;}

/*
** Tell us what state the lib is currently in
*/
void Lib330Interface::setLibState(enum tlibstate newState) {
  string63 newStateName;

  lib_get_statestr(newState, &newStateName);
  g_log << "+++ State change to '" << newStateName << "'" << std::endl;
  this->currentLibState = newState;

  /*
  ** We have no good reason for sitting in RUNWAIT, so lets just go
  */
  if(this->currentLibState == LIBSTATE_RUNWAIT) {
    this->startDataFlow();
  }
}

/*
** Handle an error condition.  Log, cleanup etc...
*/
void Lib330Interface::handleError(enum tliberr errcode) {
    string63 errmsg;
    lib_get_errstr(errcode, &errmsg);
    g_log << "XXX : Encountered error: " << errmsg << std::endl;
}

void Lib330Interface::initializeRegistrationInfo(ConfigVO ourConfig) {
  // First zero the creationInfo structure.
  memset (&this->registrationInfo, 0, sizeof(this->registrationInfo));
  qma_uint64 auth = ourConfig.getQ330AuthCode();
  memcpy(this->registrationInfo.q330id_auth, &auth, sizeof(qma_uint64));
  strcpy(this->registrationInfo.q330id_address, ourConfig.getQ330UdpAddr());
  this->registrationInfo.q330id_baseport = ourConfig.getQ330BasePort();
  this->registrationInfo.host_mode = HOST_ETH;
  strcpy(this->registrationInfo.host_interface, "");
  this->registrationInfo.host_mincmdretry = 5;
  this->registrationInfo.host_maxcmdretry = 40;
  this->registrationInfo.host_ctrlport = ourConfig.getSourcePortControl();
  this->registrationInfo.host_dataport = ourConfig.getSourcePortData();
  this->registrationInfo.opt_latencytarget = 0;
  this->registrationInfo.opt_closedloop = 0;
  this->registrationInfo.opt_dynamic_ip = 0;
  this->registrationInfo.opt_hibertime = ourConfig.getMinutesToSleepBeforeRetry();
  this->registrationInfo.opt_conntime = ourConfig.getDutyCycle_MaxConnectTime();
  this->registrationInfo.opt_connwait = ourConfig.getDutyCycle_SleepTime();
  this->registrationInfo.opt_regattempts = ourConfig.getFailedRegistrationsBeforeSleep();
  this->registrationInfo.opt_ipexpire = 0;
  this->registrationInfo.opt_buflevel = ourConfig.getDutyCycle_BufferLevel();
}

void Lib330Interface::initializeCreationInfo(char *stationName, ConfigVO ourConfig) {
  // First zero the creationInfo structure.
  memset (&this->creationInfo, 0, sizeof(this->creationInfo));	
  // Fill out the parts of the creationInfo that we know about
  qma_uint64 serial = ourConfig.getQ330SerialNumber();
  char continuityFile[512];
  
  memcpy(this->creationInfo.q330id_serial, &serial, sizeof(qma_uint64));
  switch(ourConfig.getQ330DataPortNumber()) {
    case 1:
      this->creationInfo.q330id_dataport = LP_TEL1;
      break;
    case 2:
      this->creationInfo.q330id_dataport = LP_TEL2;
      break;
    case 3:
      this->creationInfo.q330id_dataport = LP_TEL3;
      break;
    case 4:
      this->creationInfo.q330id_dataport = LP_TEL4;
      break;
  }
  strncpy(this->creationInfo.q330id_station, stationName, 5);
  this->creationInfo.host_timezone = 0;
  strcpy(this->creationInfo.host_software, APP_VERSION_STRING);
  if(strlen(ourConfig.getContinuityFileDir())) {
    sprintf(continuityFile, "%s/qmaserv_cont_%s.bin", ourConfig.getContinuityFileDir(), stationName);
  } else {
    sprintf(continuityFile, "qmaserv_cont_%s.bin", stationName);
  }
  strcpy(this->creationInfo.opt_contfile, continuityFile);
  this->creationInfo.opt_verbose = ourConfig.getLogLevel();
  this->creationInfo.opt_zoneadjust = 1;
  this->creationInfo.opt_secfilter = OSF_ALL;
  this->creationInfo.opt_minifilter = OMF_ALL;
  this->creationInfo.opt_aminifilter = OMF_ALL;
  this->creationInfo.amini_exponent = 0;
  this->creationInfo.amini_512highest = -1000;
  this->creationInfo.mini_embed = 0;
  this->creationInfo.mini_separate = 1;
  this->creationInfo.mini_firchain = 0;
  this->creationInfo.call_minidata = this->miniseed_callback;
  this->creationInfo.call_aminidata = NULL;
  this->creationInfo.resp_err = LIBERR_NOERR;
  this->creationInfo.call_state = this->state_callback;
  this->creationInfo.call_messages = this->msg_callback;
  if(ourConfig.getMulticastEnabled()) {
    this->creationInfo.call_secdata = this->onesec_callback;
  } else {
    this->creationInfo.call_secdata = NULL;
   }
  this->creationInfo.call_lowlatency = NULL;
}
