/*
 * File     :
 *  clink.h
 *
 * Purpose  :
 *  Comserv link modified to accept a packet handed into a subroutine
 *
 * Author   :
 *  Phil Maechling
 *
 *
 * Mod Date :
 *  1 April 2002
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
#ifdef __cplusplus
extern "C" {
#endif

  int comlink_send(char* buf,int len, int type);
  int comlink_dataQueueBlocking();
#ifdef __cplusplus
}
#endif
