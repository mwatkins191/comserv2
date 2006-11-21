/*
 *
 * File     :
 *  crc.C
 *  
 * Purpose  :
 *  Implements the Quanterra style crc calculations
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  12 September 2003, Chad Trabant
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

#include "CRC.h"
#include "qmaswap.h"
#ifdef _WIN32
#include <winsock2.h>
#endif
CRC::CRC()
{
  short count, bits ;
  long  tdata, accum, naccum;

  for (count = 0 ; count < 256 ; count++)
  {
        tdata = ((long) count) << 24 ;
        accum = 0 ;
        for (bits = 1 ; bits <= 8 ; bits++)
          {
            if ((tdata ^ accum) < 0)
		{
                accum = (accum << 1) ^ CRC_POLYNOM ;
		}
              else
 		{
                accum = (accum << 1) ;
		}
            tdata = tdata << 1 ;
          }
	naccum = htonl (accum);
        p_crctable[count] = naccum ;
   }
}

long CRC::gcrccalc (ptr_char b, short len)
{
    long temp;
    complong crc;

    crc.l = 0 ;

    // Two versions, one each for big and little endian
    // architectures
 
    if ( qma_htons (0x1234) == 0x1234 )
      while (len-- > 0)  // big endian
	{
	  // crc.l = (crc.l << 8) ^ p_crctable[(crc.b[0] ^ *b++) & 255] ;
	  temp = (crc.b[0] ^ *b++) & 255;
	  crc.l = (crc.l << 8);
	  crc.l = crc.l ^ p_crctable[temp];

	}
    else
      while (len-- > 0)  // little endian
	{
	  temp = (crc.b[0] ^ *b++) & 255;
	  crc.l = (crc.l >> 8) & (long) 0xffffff;
	  crc.l = crc.l ^ p_crctable[temp];
	}

    return crc.l ;
}
