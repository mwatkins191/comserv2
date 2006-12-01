/*
 * File     :
 *   qmacfg.h
 *
 * Purpose  :
 *  This is a c language header file for use when qma reads the comserv
 *  configuration file.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  28 March 2002
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

#ifndef QMACFG_H
#define QMACFG_H

struct qma_cfg
{
  char station_code[80];
  char udpaddr[80];
  char ipport[80];
  char baseport[80];
  char dataport[80];
  char serialnumber[80];
  char authcode[80];
  char verbosity[80];
  char diagnostic[80]; 
  char startmsg[80];
  char statusinterval[80];
  char datarateinterval[80];
  char loglevel[256];
};

#ifdef __cplusplus
extern "C" {
#endif

int getQmacfg(struct qma_cfg* qmacfg);
void clearConfig(struct qma_cfg* qmacfg);

#ifdef __cplusplus
}
#endif

#endif
