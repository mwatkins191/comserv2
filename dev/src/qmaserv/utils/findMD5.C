/*
 * Program: Mountainair
 *
 *
 *
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it with the sole restriction that:
 * You must cause any work that you distribute or publish, that in
 * whole or in part contains or is derived from the Program or any
 * part thereof, to be licensed as a whole at no charge to all third parties.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#ifndef _WIN32
# include <unistd.h>
# include <netdb.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <arpa/inet.h>
#endif

#include "md5.h"
#include "ctype.h"

extern int int2charvalue(int x0);


void findMD5(
#ifdef _WIN32
	     unsigned _int64 cv,
#else
	     unsigned long long cv,
#endif
	     unsigned long serverip,
	     unsigned short udpport,
	     unsigned short regnum,
#ifdef _WIN32
	     unsigned _int64 authcode,
	     unsigned _int64 sn,
	     unsigned _int64 rand,
#else
	     unsigned long long authcode,
	     unsigned long long sn,
	     unsigned long long rand,
#endif
	     unsigned char *result)
{

  //
  // String this input togehter into an 80 byte string.
  // Iput to MD5 and return a 16 byte result
  //

  char inputbuffer[80];
  memset(inputbuffer,0,80);

  //
  // Start with cv;
  //

  int position = 15;
  int i;
  for (i=16;i>0;i--)
  {
      unsigned int xt = (0x0000000f & cv);
      int charvalue = int2charvalue(xt);
      inputbuffer[position] = charvalue;
      --position;
      cv = cv>>4;
  }

  position = 23;
  for (i=8;i>0;i--)
  {
      unsigned int xt = (0x000f & serverip);
      int charvalue = int2charvalue(xt);
      inputbuffer[position] = charvalue;

      --position;
      serverip = serverip>>4;
  }
  position = 27;
  for (i=4;i>0;i--)
  {
      unsigned int xt = (0x0f & udpport);
      int charvalue = int2charvalue(xt);
      inputbuffer[position] = charvalue;

      --position;
      udpport = udpport>>4;
  }

  position = 31;
  for (i=4;i>0;i--)
  {
      unsigned int xt = (0x0f & regnum);
      int charvalue = int2charvalue(xt);
      inputbuffer[position] = charvalue;
      --position;
      regnum = regnum>>4;
  }

  position = 47;
  for (i=16;i>0;i--)
  {
      unsigned int xt = (0x0000000f & authcode);
      int charvalue = int2charvalue(xt);
      inputbuffer[position] = charvalue;
      --position;
      authcode = authcode>>4;
  }

  position = 63;
  for (i=16;i>0;i--)
  {
      unsigned int xt = (0x0000000f & sn);
      int charvalue = int2charvalue(xt);
      inputbuffer[position] = charvalue;
      --position;
      sn = sn>>4;
  }

  position = 79;
  for (i=16;i>0;i--)
  {
      unsigned int xt = (0x0000000f & rand);
      int charvalue = int2charvalue(xt);
      inputbuffer[position] = charvalue;
      --position;
      rand = rand>>4;
  }

  struct MD5Context md5c;
  unsigned char signature[16];

  MD5Init(&md5c);
  MD5Update(&md5c,(unsigned char*)inputbuffer,80);
  MD5Final(signature,&md5c);
  memcpy(result,signature,16);
  return;

}

int int2charvalue(int xt)
{

      if(xt<=9)
      {
	xt = xt+0x30; // Add 0x30 to get ascii equivalent
	return xt;
      }
      else if(xt<16)
      {
	xt = xt+0x57; // add 87 to get lower case ascii values
	return xt;
      }
      else
      {
        std::cout << "invalid input number" << std::endl;
	return -1;
      }
}
