/*
 *
 * File     :
 *  crc.h
 *
 * Purpose  :
 *  Class for calculating a Quanterra style crc.
 *
 * Author   :
 *  Phil Maechling
 *
 *
 * Mod Date :
 * 12 March 2002
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
#ifndef CRC_H
#define CRC_H

#define CRC_POLYNOM 1443300200

typedef char *ptr_char;

typedef union
  {
    unsigned char b[4] ;
    signed char sb[4] ;
    int s[2] ;
    long l ;
    float f ;
  } complong ;


class CRC
{
  public:
    CRC ();
    ~CRC() {};
    long gcrccalc (ptr_char b, short len);

 private:

    long p_crctable[256];  
};
#endif
