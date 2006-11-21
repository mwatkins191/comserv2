/*
 * File     :
 *  UnpackComp.h
 *
 * Purpose  :
 *  This is a unpack (decompress) routine for Q330 DC_COMP blockettes
 *
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *  8 June 2002
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

#include "QmaTypes.h"
#include "DC.h"

#ifndef UNPACKCOMP_H
#define UNPACKCOMP_H

#define STEIM1_SPECIAL_MASK     0
#define STEIM1_BYTE_MASK        1
#define STEIM1_HALFWORD_MASK    2
#define STEIM1_FULLWORD_MASK    3

#define STEIM2_SPECIAL_MASK     0
#define STEIM2_BYTE_MASK        1
#define STEIM2_123_MASK         2
#define STEIM2_567_MASK         3

int unpack_DCComp
(   QMABLOCK	*bp,		/* ptr to Steim2A blockette */
    qma_int32   *diffbuff,	/* ptr to unpacked data array.		*/
    qma_int32	*databuff);

int positionOfCompWord(const int wordNumber, /* find this words Last sample */
                       QMABLOCK *bp,     /* ptr to Steim2A blockette */
                       qma_int32   *diffbuff,/* ptr to unpacked data array.*/
                       qma_int32        *databuff);




#endif
