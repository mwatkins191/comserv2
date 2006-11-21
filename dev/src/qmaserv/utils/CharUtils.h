/*
 * File     :
 *  CharUtils.h
 *
 * Purpose  :
 *  Borrowed from qlib2. Rewritten to avoid dependancy on qlib2.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  14 July 2002
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
#ifndef CHARUTILS_H
#define CHARUTILS_H

//
// These routines copy strings and chars and pad with spaces
//
char*               capnstr (char        *dst,
                             char        *src,
                             int         n);

char*               capnint (char        *dst,
                             int         ival,
                             int         n);

#endif
