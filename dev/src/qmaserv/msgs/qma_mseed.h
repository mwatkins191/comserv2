/*
 * File     :
 *  qma_mseed.h
 *
 * Purpose  :
 *  Define the structure of a miniseed packet. This is the outgoing
 *  512 byte miniseed packet that is passed to comserv.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 * 18 June 2002
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
#ifndef QMA_MSEED_H
#define QMA_MSEED_H

typedef struct qmaframe
{
  qma_int32 words[64];
} QMAFRAME;


typedef struct qmaframe1
{
  qma_uint32 map;
  qma_int32 x0;
  qma_int32 x1;
  qma_int32 difs[13];
} QMAFIRSTDATAFRAME;

typedef struct qmaframe7
{
  qma_uint32 map;
  qma_int32 difs[15];
} QMADATAFRAME;

typedef struct packet
{
  QMAFRAME          header;
  QMAFIRSTDATAFRAME firstdataframe;
  QMADATAFRAME      dataframes[6];
} QMAPACKET;

typedef union qma_miniseed
{
  char      data[512];
  QMAPACKET packet;
} QMAMINISEED;

#endif
