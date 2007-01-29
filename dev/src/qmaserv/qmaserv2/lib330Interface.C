#include "global.h"
#include "lib330Interface.h"
#include "clink.h"

double g_timestampOfLastRecord = 0;

Lib330Interface::Lib330Interface(char *stationName, ConfigVO ourConfig) {
  pmodules mods;
  tmodule *mod;
  int x;

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
    g_log << mod->name << ":" << mod->ver << " ";
  }
  g_log << std::endl;
  g_log << "+++ Initializing station thread" << std::endl;
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

int Lib330Interface::ping() {
  lib_unregistered_ping(this->stationContext, &(this->registrationInfo));
}

int Lib330Interface::waitForState(enum tlibstate waitFor, int maxSecondsToWait) {
  for(int i=0; i < maxSecondsToWait; i++) {
    if(this->getLibState() != waitFor) {
      sleep(1);
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
      g_log << libStatus.accstats[AC_READ][i] << "Bps";
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
      g_log << libStatus.accstats[AC_PACKETS][i] << "Pkts";
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

void Lib330Interface::miniseed_callback(pointer p) {
  tminiseed_call *data = (tminiseed_call *) p;

  /*
   * Handle non-data miniseed records
   */
  if(data->packet_class != PKC_DATA) {
    char msgCopy[512];
    char *msgStart;
    char *newline;
    switch(data->packet_class) {
    case PKC_EVENT:
      //g_log << "ooo PKC_EVENT" << std::endl;
      break;
    case PKC_CALIBRATE:
      //g_log << "ooo PKC_CALIBRATE" << std::endl;
      break;
    case PKC_TIMING:
      //g_log << "ooo PKC_TIMING" << std::endl;
      break;
    case PKC_MESSAGE:
      // lets not muck with memory that we didn't allocate
      memcpy(msgCopy, data->data_address, 512);
      newline = msgStart = msgCopy + 56;
      // run through the message, printing one line at a time
      for(int i=0; i < data->data_size-56; i++) {
	newline = msgCopy + 56 + i;
	if(*newline == '\0') {
	  break;
	} else if(*newline == '\n') {
	  *(newline) = '\0';
	  g_log << data->channel << " " << msgStart << std::endl;
	  msgStart = newline + 1;
	}
      }
      break;
    case PKC_OPAQUE:
      //g_log << "ooo PKC_OPAQUE" << std::endl;
      break;
    }
  } else {
    if(data->timestamp < g_timestampOfLastRecord) {
      g_log << "XXX Packets being received out of order" << std::endl;
      g_timestampOfLastRecord = data->timestamp;
    }
  }
  comlink_send((char *)data->data_address, data->data_size, DATA_PACKET);

}

void Lib330Interface::archival_miniseed_callback(pointer p) {
  //tminiseed_call *data = (tminiseed_call *) p;
  //g_log << "Archival (" << data->channel << ") with timestamp: " << data->timestamp << " " << data->data_size << " bytes" << std::endl;
}

void Lib330Interface::msg_callback(pointer p) {
  tmsg_call *msg = (tmsg_call *) p;
  string95 msgText;
  lib_get_msg(msg->code, &msgText);
  // g_log << "MSG: " <<  msgText << " " << msg->suffix << std::endl;
}

/**
 * For internal use only
 */
int Lib330Interface::sendUserMessage(char *msg) {}

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
  this->creationInfo.opt_secfilter = 0;
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
  this->creationInfo.call_secdata = NULL;
  this->creationInfo.call_lowlatency = NULL;
}
