#ifndef __PORTINGTOOLS_H_
#define _PORTINGTOOLS_H_
size_t strlcpy(char *dst, const char *src, size_t destsize);
size_t strlcat(char *dst, const char *src, size_t size);
void SwapDouble( double *data );


#ifdef _WIN32
typedef struct {
  long lowVal;
  long highVal;
} split64;

typedef union {
  _int64 longVal;
  split64 highAndLow;
} my64;

struct timezone {
  int  tz_minuteswest;
  int  tz_dsttime;
};

int gettimeofday(struct timeval *tv, struct timezone *tz);
#endif

#endif
