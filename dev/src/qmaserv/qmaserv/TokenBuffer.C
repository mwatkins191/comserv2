/*
 * File     :
 *   TokenBuffer.C
 *
 * Purpose  :
 *  This is a token buffer reading routine. It provides access
 *  to tokens that have been read from the q330. This encapsulate
 *  the logic determining length of tokens and types of tokens.
 *  The access is provide in a collection iterator style, with
 *  hasMoreTokens, and nextToken.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   21 April 2002
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
#include <string.h>
#include <iostream>
#include <iomanip>
#include "TokenBuffer.h"
#include "StateMachine.h"
#include "qmaswap.h"
#include "global.h"

extern StateMachine g_stateMachine;

TokenBuffer::TokenBuffer()
{

}

void TokenBuffer::initialize(char* buf, int len)
{
  if(true)
  {
    g_log << "--- Starting TokenBuffer init with Buffer of length : " 
	    << len << std::endl;
  }
  p_currentPosition = -1;
  p_currentToken = 0;
  p_bytes_in_buffer = len;
  p_data_length = -1;
  p_token_length = -1;
  memset((char*)&p_buf[0],0,C1_MAXCFG);
  memcpy((char*)&p_buf[0],buf,p_bytes_in_buffer);
  if(!true)
  {
    printTokenBuffer();
  }
  return;
}

//
// Moves to next token. This controls the behavior of the other
// methods which operate on the current token. For Example,
// getTokenBigString() gets the bit string for the current token.
//

int TokenBuffer::hasMoreTokens()
{
  if(p_currentPosition < p_bytes_in_buffer)
  {
   return true;
  }
  else
  {
    return false;
  }
}

//
// It's an error to call nextToken if the
// hasMoreTokens returns false. If you do,
// you get back a NOP token which is
// likely to be misinterpreted.
//
qma_uint8 TokenBuffer::nextToken()
{
  //g_log << "--- In nextToken() " << std::endl;
  if(!hasMoreTokens())
  {
    g_log << "xxx Restart on Token Error: - Called nextToken when hasMoreTokens is false. " << std::endl;
    g_stateMachine.setState(Exitting);
    p_currentToken = 0;
    return p_currentToken;
  }
  else
  {
    int bytesToNextToken = findBytesToNextToken();
    p_currentPosition = p_currentPosition + bytesToNextToken;
    memcpy((char*)&p_currentToken,
	   (char*)&p_buf[p_currentPosition],1);
    //
    // This call to findTokenAndDataLength will set the data_length
    // and token_length correctly for the current token.
    //

    findTokenAndDataLength();

    int temp = p_currentToken;
    if(!true)
    {
      g_log << "--- Token        : "<< std::hex << temp << std::endl;
      g_log << "--- Data  Length : "<< std::hex << p_data_length <<
      std::endl;
      g_log << "--- Token Length : "<< std::hex << p_token_length << 
      std::endl;
      g_log << "--- New Position : "<< p_currentPosition << std::endl;
    }
    return p_currentToken;
  }
}

int TokenBuffer::findBytesToNextToken()
{
  if(p_currentPosition == -1)
  {
    g_log << "--- Processing first token" << std::endl;
    p_currentPosition = 0;
    return 0;
  }
  else
  {
    return (p_data_length + p_token_length);
  }
}

//
// Sets the value of p_data_length and p_token_length in bytes.
//

void TokenBuffer::findTokenAndDataLength()
{
  if(!true)
  {
    int temp = p_currentToken;
    g_log << "--- Finding token and data lengths for: " << temp 
	<< std::endl;
  }
  qma_uint16 len16 = 0;
  qma_uint8  len8 = 0;  
  qma_uint8  msbflag = 0x80;
  qma_uint8  nmsbflag = 0x40;

  p_data_length  = 0; // default to NOP token
  p_token_length = 1; // all these are fixed length tokens

  if((p_currentToken & msbflag) == 0) // MSB clear means fix len token
  {
    //g_log << "--- MSB Clear in token." << std::endl;
    p_token_length = 1; // all these are fixed length tokens
    switch(p_currentToken)
    {
      case 0:
      {
        p_data_length = 0;
        break;
      }
      case 1:
      {
        p_data_length = 1;
        break;
      }
      case 2:
      {
        p_data_length = 7;
        break;
      }
      case 3:
      {
        p_data_length = 2;
        break;
      }
      case 4:
      {
        p_data_length = 37;
        break;
      }
      case 5:
      {
        p_data_length = 2;
        break;
      }
      case 6:
      {
        p_data_length = 16;
        break;
      }
      case 7:
      {
        p_data_length = 10;
        break;
      }
      case 8:
      {
        p_data_length = 8;
        break;
      }
      case 9:
      {
        p_data_length = 2;
        break;
      }
      default:
      {
         g_log << "xxx Restarting on Token Error: - Unknown token type : " << (qma_uint32) p_currentToken  
		<< std::endl;
         g_stateMachine.setState(Exitting);
         p_currentToken = 0;
         break;
      }
    }
  }
  else  // Variable length tokens have bit 7 set
  {
    if((p_currentToken & nmsbflag)==0) // if bit 6 is clear - 1 byte len field
    {
      if(!true)
      {
        g_log << "--- One byte var length : " << p_currentToken  << std::endl;    }
      len8 = 0;
      memcpy((char*)&len8,(char*)&p_buf[p_currentPosition+1],1);
      //
      // length include themselves. However they aren't data
      // so remove them from the datalength, and add them to the
      // token length
      //
      if(len8 > 0)
      {
        p_data_length = len8 - 1;
      }
      else
      { 
        g_log << "xxx Restarting on Token error: - Found one byte token data length less than one : " 
	  << (qma_uint32) p_currentToken << std::endl;
        g_stateMachine.setState(Exitting);
        p_currentToken = 0;
      }
      p_token_length = 2; // add in token plus 1 len byte;
    }
    else
    {
      if(!true)
      {
        g_log << "--- Two byte var length : " << p_currentToken << std::endl;
      }
      len16 =0;
      memcpy((char*)&len16,(char*)&p_buf[p_currentPosition+1],2);
      // we need to make sure the length is in host byte order
      len16 = qma_ntohs(len16);
      if(len16 > 1)
      {
        p_data_length = len16 - 2;
      }
      else
      { 
        g_log << "xxx Restarting on Token Error - Found two byte token data length less than two : " 
	  << (qma_uint32) p_currentToken << std::endl;
        g_stateMachine.setState(Exitting);
        p_currentToken = 0;
      }
      p_token_length = 3; // add in the token plus 2 len bytes; 
    }
  }
  return;
}
//
// For this to work, findDataLength must have been called.
// It is called in nextToken, so calls should be valid
// if that accessor is used with this method
//
char* TokenBuffer::getTokenBitString()
{
  if(!true)
  {
    g_log << "--- gettingTokenBitString with :" <<  std::endl;
    g_log << "--- Current Position : " << p_currentPosition << std::endl;
    g_log << "--- Token Length : " << p_token_length << std::endl;
    g_log << "--- Data Length : " << p_data_length << std::endl;
  }
  
  // 
  // The current_position, is the last token read, and we
  // are returning the position to the start of the current
  // token, the new one.
  //
  return  (char*)&p_buf[p_currentPosition + p_token_length];
}

int TokenBuffer::getTokenBitStringLength()
{
  return p_data_length;
}

void TokenBuffer::printTokenBuffer()
{
  g_log << "--- Token Buffer Contents " << std::endl;
  for(int x = 0;x<p_bytes_in_buffer;(x=x+4))
  {
    qma_uint16 b1;
    qma_uint16 b2;

    memcpy(&b1,&p_buf[x],2);
    memcpy(&b2,&p_buf[x+2],2);
    /*
    g_log << "--- " << std::hex << std::setw(4) << std::setiosflags(std::ios::right) 
	<< b1 << " "
        <<  std::hex << std::setw(4) << std::setiosflags(std::ios::right) <<
	b2 << std::endl;
    */
  }
}
