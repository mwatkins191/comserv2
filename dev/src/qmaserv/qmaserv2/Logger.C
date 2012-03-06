// 
// ======================================================================
// Copyright (C) 2000-2003 Instrumental Software Technologies, Inc. (ISTI)
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. If modifications are performed to this code, please enter your own 
// copyright, name and organization after that of ISTI.
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in
// the documentation and/or other materials provided with the
// distribution.
// 3. All advertising materials mentioning features or use of this
// software must display the following acknowledgment:
// "This product includes software developed by Instrumental
// Software Technologies, Inc. (http://www.isti.com)"
// 4. If the software is provided with, or as part of a commercial
// product, or is used in other commercial software products the
// customer must be informed that "This product includes software
// developed by Instrumental Software Technologies, Inc.
// (http://www.isti.com)"
// 5. The names "Instrumental Software Technologies, Inc." and "ISTI"
// must not be used to endorse or promote products derived from
// this software without prior written permission. For written
// permission, please contact "info@isti.com".
// 6. Products derived from this software may not be called "ISTI"
// nor may "ISTI" appear in their names without prior written
// permission of Instrumental Software Technologies, Inc.
// 7. Redistributions of any form whatsoever must retain the following
// acknowledgment:
// "This product includes software developed by Instrumental
// Software Technologies, Inc. (http://www.isti.com/)."
// 8. Redistributions of source code, or portions of this source code,
// must retain the above copyright notice, this list of conditions
// and the following disclaimer.
// THIS SOFTWARE IS PROVIDED BY INSTRUMENTAL SOFTWARE
// TECHNOLOGIES, INC. "AS IS" AND ANY EXPRESSED OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.  IN NO EVENT SHALL INSTRUMENTAL SOFTWARE TECHNOLOGIES,
// INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
// 


#include <iostream>
#include "Logger.h"
#include "stdio.h"
#include "string.h"
#include "QmaTypes.h"
#include "portingtools.h"

#ifdef Q3302EW
extern "C" {
#  include "earthworm_simple_funcs.h"
};
#endif

Logger::Logger() {
    //clearBuffer();
    stdoutLogging = false;
    fileLogging = false;
    memset (&this->logger_mutex, 0, sizeof(pthread_mutex_t));
    int rc = pthread_mutex_init (&logger_mutex, NULL);
    if (rc != 0) {
	fprintf (stderr, "Error %d iniiialing logger logger_mutex\n", rc);
	exit (rc);
    }
}

Logger::~Logger() {
    if(strlen(logBuff.str().c_str()) != 0) {
        endEntry();
    }
    int rc = pthread_mutex_destroy (&logger_mutex);
    if (rc != 0) {
	fprintf (stderr, "Error %d destroying logger logger_mutex\n", rc);
	exit (rc);
    }
}


void Logger::logToStdout(bool val) {
  stdoutLogging = val;
}

void Logger::logToFile(bool val) {
  fileLogging = val;
}

Logger& Logger::operator<<(char *val) {
  int rc = pthread_mutex_lock (&logger_mutex);
  logBuff << (char *) val;
  rc = pthread_mutex_unlock (&logger_mutex);
  return *this;
}

Logger& Logger::operator<<(char val) {
  int rc = pthread_mutex_lock (&logger_mutex);
  logBuff << val;
  rc = pthread_mutex_unlock (&logger_mutex);
  return *this;
}

Logger& Logger::operator<<(qma_int8 val) {
  int rc = pthread_mutex_lock (&logger_mutex);
  logBuff << val;
  rc = pthread_mutex_unlock (&logger_mutex);
  return *this;
}

Logger& Logger::operator<<(qma_uint8 val) {
  int rc = pthread_mutex_lock (&logger_mutex);
  logBuff << val;
  rc = pthread_mutex_unlock (&logger_mutex);
  return *this;
}

Logger& Logger::operator<<(qma_int16 val) {
  int rc = pthread_mutex_lock (&logger_mutex);
  logBuff << val;
  rc = pthread_mutex_unlock (&logger_mutex);
  return *this;
}

Logger& Logger::operator<<(qma_uint16 val) {
  int rc = pthread_mutex_lock (&logger_mutex);
  logBuff << val;
  rc = pthread_mutex_unlock (&logger_mutex);
  return *this;
}

Logger& Logger::operator<<(qma_int32 val) {
  int rc = pthread_mutex_lock (&logger_mutex);
  logBuff << val;
  rc = pthread_mutex_unlock (&logger_mutex);
  return *this;
}

Logger& Logger::operator<<(qma_uint32 val) {
  int rc = pthread_mutex_lock (&logger_mutex);
  logBuff << val;
  rc = pthread_mutex_unlock (&logger_mutex);
  return *this;
}

Logger& Logger::operator<<(qma_int64 val) {
#ifdef _WIN32
  my64 m;
  m.longVal = val;
  char f[18];
  sprintf(f, "%8.8x%8.8x", m.highAndLow.highVal, m.highAndLow.lowVal);
  logBuff << f;
#else
  int rc = pthread_mutex_lock (&logger_mutex);
  logBuff << val;
  rc = pthread_mutex_unlock (&logger_mutex);
#endif
  return *this;
}

Logger& Logger::operator<<(qma_uint64 val) {
#ifdef _WIN32
  my64 m;
  m.longVal = val;
  char f[18];
  sprintf(f, "%8.8x%8.8x", m.highAndLow.highVal, m.highAndLow.lowVal);
  logBuff << f;
#else
  int rc = pthread_mutex_lock (&logger_mutex);
  logBuff << val;
  rc = pthread_mutex_unlock (&logger_mutex);
#endif
  return *this;
}

Logger& Logger::operator<<(float val) {
  int rc = pthread_mutex_lock (&logger_mutex);
  logBuff << val;
  rc = pthread_mutex_unlock (&logger_mutex);
  return *this;
}

Logger& Logger::operator<<(double val) {
  int rc = pthread_mutex_lock (&logger_mutex);
  logBuff << val;
  rc = pthread_mutex_unlock (&logger_mutex);
  return *this;
}

// the following are needed for rendering of std::endl
Logger& Logger::operator<<(std::ostream& (*f)(std::ostream&)){
  // we'll consider an endl a plea for an endEntry()
  int rc = pthread_mutex_lock (&logger_mutex);
  logBuff << f;
  if(logBuff.str().c_str()[strlen(logBuff.str().c_str())-1] == '\n') {
    endEntry();
  }
  rc = pthread_mutex_unlock (&logger_mutex);
  return *this;
}
Logger& Logger::operator<<(std::ios& (*f)(std::ios&)){
  int rc = pthread_mutex_lock (&logger_mutex);
  logBuff << f;
  rc = pthread_mutex_unlock (&logger_mutex);
  return *this;
}

Logger& Logger::operator<<(std::ios_base& (*f)(std::ios_base&)){
  int rc = pthread_mutex_lock (&logger_mutex);
  logBuff << f;
  rc = pthread_mutex_unlock (&logger_mutex);
  return *this;
}

Logger& Logger::endEntry() {
#ifndef Q3302EW
  if(stdoutLogging) {
    printf(logBuff.str().c_str());
//    std::cout << logBuff.str();
  }
  if(fileLogging) {
    // file logging stuff
  }
#else
  char msg[1024];
  strcpy(msg, logBuff.str().c_str());  
  if(stdoutLogging && fileLogging) {
    logit("o", msg);
  } else {
    if(stdoutLogging) {
      logit("o", msg);
    } 
    if(fileLogging) {
      logit("", msg);
    }
  }
#endif
  clearBuffer();
  return *this;
}

void Logger::clearBuffer() {
  logBuff.str("");
}


#ifdef _TEST_
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main(int argc, char **argv) {
  Logger log;
  struct in_addr in;
  log.logToStdout(true);
  log << "Configured to send packets to " <<
    inet_ntoa(in) << " on port " <<
//    int rc = pthread_mutex_lock (&logger_mutex);
    std::hex <<  5330 << std::endl;
//    rc = pthread_mutex_unlock (&logger_mutex);
}
#endif
