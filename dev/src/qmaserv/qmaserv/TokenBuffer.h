/*
 * File     :
 *   TokenBuffer.h
 *
 * Purpose  :
 *   This has the logic to read a token buffer, and retrieve the
 *   tokens, and their bit fields
 *
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *   20 April 2002
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
#ifndef TOKEN_BUFFER_H
#define TOKEN_BUFFER_H

#include "QmaTypes.h"
#include "QmaLimits.h"

class TokenBuffer
{
  public:
    TokenBuffer();
    ~TokenBuffer() {};
  
    void       initialize(char* buf,int len);
    int        hasMoreTokens();
    qma_uint8  nextToken(); 
    void       findTokenAndDataLength();
    int        findBytesToNextToken();

    char*      getTokenBitString();
    int        getTokenBitStringLength();
    void       printTokenBuffer();

  private:

    char      p_buf[C1_MAXCFG];
    int       p_bytes_in_buffer;
    int       p_currentPosition;
    int       p_data_length;
    int       p_token_length;
    qma_uint8 p_currentToken;
};
#endif
