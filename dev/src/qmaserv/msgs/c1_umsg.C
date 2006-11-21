/*
 *
 * File     :
 *   c1_umsg.C
 *
 * Purpose  :
 *  This implements a c1_umsg packet.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   28 July 2002
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
 *
 */
#include <iostream>
#include <string.h>
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "Field.h"
#include "c1_umsg.h"
#if defined(LINUX) || defined(_WIN32)
#include <portingtools.h>
#endif
c1_umsg::c1_umsg()
{
  p_fieldListPtr = &p_fieldList[0];
  p_number_of_fields = 2;
  Field f_pad;
  f_pad.p_start_position = 0;
  f_pad.p_number_bytes = 4;
  f_pad.p_data_type = UnsignedInt32;
  strcpy(f_pad.p_name,"PaddedWord");
  p_fieldList[0] = f_pad;

  Field f_msg;
  f_msg.p_start_position = 4;
  f_msg.p_number_bytes = 80;
  f_msg.p_data_type = ASCII;
  strcpy(f_msg.p_name,"UserMessage");
  p_fieldList[1] = f_msg;

  if(!true)
  {
    std::cout << "Creating c1_umsg with length : " << 
 	p_fieldList[1].p_start_position  << " " <<
    	p_fieldList[1].p_number_bytes << std::endl;	
  }

  p_length_in_bytes =
    p_fieldList[1].p_start_position + 
    p_fieldList[1].p_number_bytes;

  memset((char*)&p_bits[0],0,p_length_in_bytes);

}


//
// Operations on fields
//

void  c1_umsg::setUserMessage(char *aMsg)
{
  char outmsg[80];
  char inmsg[80];

  memset(inmsg,0,80);
  int res = strlen(aMsg);
  if(res < 1)
  {
    strcpy(inmsg,"Mountainair registering on this Q330");
  }
  else if(res > 79)
  {
    strlcpy(inmsg,aMsg,79);
  }
  else
  {
    strcpy(inmsg,aMsg);
  }
  strpas(outmsg,inmsg);
  memcpy((char*)&p_bits[p_fieldList[1].p_start_position],
	 (char*)&outmsg,
	 p_fieldList[1].p_number_bytes);
}

void strpas (char* outstring, char* instring)
{
  short i;
  i = 0 ;
  while (instring[i])
  {
    outstring[i + 1] = instring[i++] ;
  }
    outstring[0] = i ;
}
