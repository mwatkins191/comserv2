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
#include "QmaTypes.h"

#ifdef Q3302EW
extern "C" {
#  include "earthworm_simple_funcs.h"
};
#endif

Logger::Logger() {
    clearBuffer();
    stdoutLogging = false;
    fileLogging = false;
}

Logger::~Logger() {
    if(logBuffer[0]) {
        endEntry();
    }
}


void Logger::logToStdout(bool val) {
  stdoutLogging = val;
}

void Logger::logToFile(bool val) {
  fileLogging = val;
}

Logger& Logger::operator<<(char *val) {
  strcat(logBuffer, val);
  return *this;
}

Logger& Logger::operator<<(char val) {
  char tmp[2];
  sprintf(tmp, "%c", val);
  strcat(logBuffer, tmp);
  return *this;
}

Logger& Logger::operator<<(qma_int8 val) {
  char tmp[25];
  sprintf(tmp, "%dh", val);
  strcat(logBuffer, tmp);
  return *this;
}

Logger& Logger::operator<<(qma_uint8 val) {
  char tmp[25];
  sprintf(tmp, "%uh", val);
  strcat(logBuffer, tmp);
  return *this;
}

Logger& Logger::operator<<(qma_int16 val) {
  char tmp[25];
  sprintf(tmp, "%d", val);
  strcat(logBuffer, tmp);
  return *this;
}

Logger& Logger::operator<<(qma_uint16 val) {
  char tmp[25];
  sprintf(tmp, "%u", val);
  strcat(logBuffer, tmp);
  return *this;
}

Logger& Logger::operator<<(qma_int32 val) {
  char tmp[25];
  sprintf(tmp, "%dl", val);
  strcat(logBuffer, tmp);
  return *this;
}

Logger& Logger::operator<<(qma_uint32 val) {
  char tmp[25];
  sprintf(tmp, "%ul", val);
  strcat(logBuffer, tmp);
  return *this;
}

Logger& Logger::operator<<(qma_int64 val) {
  char tmp[25];
  sprintf(tmp, "%dll", val);
  strcat(logBuffer, tmp);
  return *this;
}

Logger& Logger::operator<<(qma_uint64 val) {
  char tmp[25];
  sprintf(tmp, "%ull", val);
  strcat(logBuffer, tmp);
  return *this;
}

Logger& Logger::operator<<(float val) {
  char tmp[50];
  sprintf(tmp, "%f", val);
  strcat(logBuffer, tmp);
  return *this;
}

Logger& Logger::operator<<(double val) {
  char tmp[50];
  sprintf(tmp, "%f", val);
  strcat(logBuffer, tmp);
  return *this;
}

// the following are needed for rendering of std::endl
Logger& Logger::operator<<(std::ostream& (*f)(std::ostream&)){
  char tmp[2];
  sprintf(tmp, "%c", '\n');
  strcat(logBuffer, tmp);
  
  // we'll consider an endl a plea for an endEntry()
  endEntry();
  return *this;

}
Logger& Logger::operator<<(std::ios& (*f)(std::ios&)){
}
Logger& Logger::operator<<(std::ios_base& (*f)(std::ios_base&)){
}

Logger& Logger::endEntry() {
#ifndef Q3302EW
  if(stdoutLogging) {
    std::cout << logBuffer;
  }
  if(fileLogging) {
    // file logging stuff
  }
#else
  if(stdoutLogging && fileLogging) {
    logit("o", logBuffer);
  } else {
    if(stdoutLogging) {
      logit("o", logBuffer);
    } 
    if(fileLogging) {
      logit("", logBuffer);
    }
  }
#endif
  clearBuffer();
}

void Logger::clearBuffer() {
  for(int x = 0; x < LOG_BUFFER_SIZE; x++) {
    logBuffer[x] = '\0';
  }
}


#ifdef _TEST_
int main(int argc, char **argv) {
  Logger log;
  log.logToStdout(true);
  log << "testing" << 1 << 2 << 3 << std::endl;
  log.endEntry();
}
#endif
