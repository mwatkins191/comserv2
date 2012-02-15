#ifndef __LIB330INTERFACE_H__
#define __LIB330INTERFACE_H__

/*
** pascal.h and C++ don't get along well (and, or, xor etc... mean something in C++)
*/
#define pascal_h

/*
** These are C functions, and should be treated as such
*/
extern "C" {
#include <libclient.h>
#include <libtypes.h>
#include <libmsgs.h>
#include <libsupport.h>
}

#include "ConfigVO.h"
#include "PacketQueue.h"

struct onesec_pkt{
  char net[4];
  char station[16];
  char channel[16];
  char location[4];
  uint32_t rate;
  uint32_t timestamp_sec;
  uint32_t timestamp_usec;
  int32_t samples[MAX_RATE];
};

#define ONESEC_PKT_HDR_LEN 52

// Strict compilers and loaders only allow externals to be defined in 1 file.
// One and only one source file should define DEFINE_EXTERNAL.

#ifdef	DEFINE_EXTERNAL
#define	EXTERN
#else
#define	EXTERN extern
#endif

EXTERN PacketQueue packetQueue;

EXTERN struct sockaddr_in mcastAddr;
EXTERN int mcastSocketFD;
EXTERN char multicastChannelList[256][5];

class Lib330Interface {
 public:
  Lib330Interface(char *, ConfigVO configInfo);
  ~Lib330Interface();

  void startRegistration();
  void startDeregistration();
  void changeState(enum tlibstate, enum tliberr);
  void startDataFlow();
  void displayStatusUpdate();
  int waitForState(enum tlibstate, int, void(*)());
  enum tlibstate getLibState();
  void ping();
  int processPacketQueue();
 
  static void state_callback(pointer p);
  static void miniseed_callback(pointer p);
  static void archival_miniseed_callback(pointer p);
  static void msg_callback(pointer p);
  static void onesec_callback(pointer p);


 private:
  int sendUserMessage(char *);
  void initializeCreationInfo(char *, ConfigVO);
  void initializeRegistrationInfo(ConfigVO);
  void handleError(enum tliberr);
  void setLibState(enum tlibstate);

  tcontext stationContext;
  tpar_register registrationInfo;
  tpar_create   creationInfo;
  enum tlibstate currentLibState;

};
#endif
