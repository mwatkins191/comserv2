#include <stdio.h>
#include <errno.h>

int syserr(char *msg)
{
  extern int errno, sys_nerr;
#ifndef LINUX  /*IGD -stdio.h LINUX */
  extern char *sys_errlist[];
#endif 

  fprintf(stdout,"ERROR: %s ( errno: %d",msg,errno);
  if (errno > 0 && errno < sys_nerr)
    fprintf(stdout,"; Description: %s)\n",sys_errlist[errno]);
  else
    fprintf(stdout,")\n");  
  fflush(stdout);
  return(1);
}

void fatalsyserr(char *msg)
{
  extern int errno, sys_nerr;
#ifndef LINUX  /*IGD*/
  extern char *sys_errlist[];
#endif 

  fprintf(stdout,"FATAL ERROR: %s ( errno: %d",msg,errno);
  if (errno > 0 && errno < sys_nerr)
    fprintf(stdout,"; Description: %s)\n",sys_errlist[errno]);
  else
    fprintf(stdout,")\n");  
  fflush(stdout);
  exit(1);
}

