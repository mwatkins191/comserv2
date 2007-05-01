#ifndef __GLOBAL_H__
#define __GLOBAL_H__


 
#include "Logger.h"
#include "lib330Interface.h"


#define MAX_CHARS_IN_STATION_CODE 4

#define DEFAULT_STATUS_INTERVAL 100
#define MIN_STATUS_INTERVAL 5
#define MAX_STATUS_INTERVAL 200

#define DEFAULT_DATA_RATE_INTERVAL 3
#define MIN_DATA_RATE_INTERVAL 1
#define MAX_DATA_RATE_INTERVAL 100

#define MAJOR_VERSION 2
#define MINOR_VERSION 0
#define RELEASE_VERSION 0
#define APP_VERSION_STRING "Qmaserv v2.0.0 $Rev$"
extern Logger g_log;
extern Lib330Interface *g_libInterface;
extern bool g_reset;
#endif
