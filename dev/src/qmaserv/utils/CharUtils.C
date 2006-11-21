/*
 * File     :
 *  CharUtils.C
 *
 * Purpose  :
 *  For use building SEED headers.
 *
 * Author   :
 *  Phil Maechling (based on Doug Neuhausers qlib2)
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
#include <string.h>
#include <stdio.h>
#include "CharUtils.h"

/************************************************************************/
/*  capnstr:                                                            */
/*      Copy the contents of a string to a char array, and blank pad    */
/*      to the specified  output length.  Do not null-terminate the     */
/*      output char array and do not exceed the specified output length.*/
/*  return:                                                             */
/*      pointer to the destination char array.                          */
/************************************************************************/
char* capnstr
   (char        *dst,           /* destination char array to fill.      */
    char        *src,           /* source string to copy.               */
    int         n)              /* char length for destination array.   */
{
    char *dp = dst;
    char *sp = src;
    int sl = strlen(src);
    int i;
    for (i=0; i<n; i++) *dp++ = (i<sl) ? *sp++ : ' ';
    return (dst);
}


/************************************************************************/
/*  capnint:                                                            */
/*      Copy the 0-padded ascii representation of an integer to a       */
/*      char array, and blank pad the char array to the specified       */
/*      output length.  Do not null-terminate the output char array     */
/*      and to not exceed the specified output length.                  */
/*  return:                                                             */
/*      pointer to the destination char array.                          */
/************************************************************************/
char* capnint
   (char        *dst,           /* destination char array to fill.      */
    int         ival,           /* integer to be encoded.               */
    int         n)              /* char length for destination array.   */
{
    char tmpstr[80];
    char tmpfmt[10];
    char *dp = dst;
    sprintf (tmpfmt, "%%0%dd",n);
    sprintf (tmpstr, tmpfmt, ival);
    strncpy (dst, tmpstr, n);
    return (dst);
}
