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
#include "BlockingQueue.h"

PacketQueue blockingQueue;

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
  int ping();
  int processBlockingQueue();
 
  static void state_callback(pointer p);
  static void miniseed_callback(pointer p);
  static void archival_miniseed_callback(pointer p);
  static void msg_callback(pointer p);

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
