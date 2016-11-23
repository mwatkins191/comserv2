

#define CS_LOG_MODE_TO_STDOUT 0
#define CS_LOG_MODE_TO_LOGFILE 1

#define CS_LOG_TYPE_INFO 0
#define CS_LOG_TYPE_DEBUG 1
#define CS_LOG_TYPE_ERROR 2

void GetLogParamsFromNetwork(char *logdir, char *logtype); 
int LogInit(int mode, char *path, char *logname, int buf_size);
int LogMessage(int type, char *format, ...);
