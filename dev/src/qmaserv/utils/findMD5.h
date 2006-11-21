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
#ifndef FINDMD5_H
#define FINDMD5_H

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
	     unsigned char *result);
#endif
