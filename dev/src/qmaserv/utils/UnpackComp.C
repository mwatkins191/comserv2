/*
 * File     :
 *  UnpackComp.C
 *
 * Purpose  :
 *  This unpacks q330 DC_COMP packets.
 *
 * Author   :
 *  Phil Maechling
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
/************************************************************************/
#include <iostream>
#include <stdio.h>
#include "QmaTypes.h"
#include "DC.h"
#include "UnpackComp.h"
#include "qmaswap.h"

int unpack_DCComp
(   QMABLOCK	*bp,		/* ptr to Steim2A blockette */
    qma_int32   *diffbuff,	/* ptr to unpacked data array.		*/
    qma_int32	*databuff)	/* ptr to unpacked diff array.		*/
{

    int		nd = 0;		/* # of data points in packet.		*/
    int		wn;		/* current work number in the blockette	*/
    int		c;		/* current compression flag.		*/

    qma_int32	i;
    qma_int32	n, bits, m1, m2;
    qma_int32	val, dnib;

    qma_int32  *diff = diffbuff;

    if (bp->numberOfSamples <= 0)
    { 
      return (false);
    }

    //
    // Vars used to walk through the map bytes
    //
    int curposition = -1;
    qma_uint8 curmap;
    qma_uint8 tmap;

    int numberOfMaps = bp->mapLengthInBytes*4;
    for (wn = 0; wn < numberOfMaps; wn++) 
    {
      if (nd >= bp->numberOfSamples)
      {
        break;
      } 
      if( (wn%4) == 0)
      {  
        ++curposition;
        curmap = bp->map[curposition];
      }

      //rotate the map two bits to put the values into mask position
      tmap = curmap >> ((4-(wn%4)-1)*2);

      c = tmap & 0x03;

      //
      // Code to ignore the maps of type 0.
      //
      if(!true)
      {
        //std::cout << "--- Rotate value : " << ((4-(wn%4)-1)*2) << std::endl;
        //std::cout << "--- wn value : " << wn << std::endl;
        //std::cout << "--- CompType : " << c << " tmap " << 
	// (qma_uint16) tmap << std::endl;
        //std::cout << "--- NumberofSample :  " << bp->numberOfSamples 
	//	<< std::endl;
        std::cout << "--- Total maps  :      " << numberOfMaps << std::endl;
        std::cout << "--- Maps processed : " << wn << std::endl;
        std::cout << "--- Samples processed: " << nd << std::endl;
      }

      switch (c) 
      {
        case STEIM2_SPECIAL_MASK:
	{
		/* Headers info -- skip it.				*/
		break;
	}
	case STEIM2_BYTE_MASK:
	{
		/* Next 4 bytes are 4 1-byte differences.		*/
		/* NOTE: THIS CODE ASSUMES THAT CHAR IS SIGNED.	*/
	        /* Note this was converted to handles words without unions */
                qma_int8 mbyte[4];
		for (i=0; i<4 && nd<bp->numberOfSamples; i++,nd++)
                {
		    if(i==0)
                    {
		        qma_int32 tmpInt = qma_ntohl(bp->w[wn]);
			memcpy((char*)&mbyte[0],&tmpInt,4); 
                    }
		    *diff++ = mbyte[i];
                }
		break;
	}
	case STEIM2_123_MASK:
	{
	  val = qma_ntohl(bp->w[wn]);
	  dnib =  val >> 30 & 0x3;
	  switch (dnib) 
          {
	    case 1:	/*	1 30-bit difference.		*/
	      bits = 30; n = 1; m1 = 0x3fffffff; m2 = 0x20000000; break;
	    case 2:	/*  2 15-bit differences.		*/
	      bits = 15; n = 2; m1 = 0x00007fff; m2 = 0x00004000; break;
	    case 3:	/*  3 10-bit differences.		*/
	      bits = 10; n = 3; m1 = 0x000003ff; m2 = 0x00000200; break;
	    default:	
            {
	      /*	should NEVER get here.		*/
	      printf ("xxx (1) Invalid ck, dnib, wn = %d, %d, %d\n", 
			     c, dnib, wn);
              printf("xxx Number of samples %d\n",nd);
		    return 0;
            }
	  }
	  /*  Uncompress the differences.			*/
	  for (i=(n-1)*bits; i>=0 && nd<bp->numberOfSamples; i-=bits,nd++) 
          {
	    *diff = (val >> i) & m1;
	    *diff = (*diff & m2) ? *diff | ~m1 : *diff;
	    diff++;
	  }
	  break;
	}
	case STEIM2_567_MASK:
	{
	  // Swapped -Hal
	  val = qma_ntohl(bp->w[wn]);
	  dnib =  val >> 30 & 0x3;
	  switch (dnib) 
          {
	    case 0:	/*  5 6-bit differences.		*/
		    bits = 6; n = 5; m1 = 0x0000003f; m2 = 0x00000020; break;
	    case 1:	/*  6 5-bit differences.		*/
		    bits = 5; n = 6; m1 = 0x0000001f; m2 = 0x00000010; break;
	    case 2:	/*  7 4-bit differences.		*/
		    bits = 4; n = 7; m1 = 0x0000000f; m2 = 0x00000008; break;
	    default:
            {
	      printf ("xxx (2) Invalid ck, dnib, wn = %d, %d, %d\n",
			     c, dnib, wn);
              printf("xxx Number of samples %d\n",nd);
	      return 0;
             }
	  }
	  /*  Uncompress the differences.			*/
	  for (i=(n-1)*bits; i>=0 && nd < bp->numberOfSamples; i-=bits,nd++) 
          {
	    *diff = (val >> i) & m1;
	    *diff = (*diff & m2) ? *diff | ~m1 : *diff;
	    diff++;
	  }
	  break;
	}
	default:
	{
	  /* Should NEVER get here.				*/
	  printf ("--- Error: unpack_steim2 - invalid ck, wn = %d, %d\n", c);
          printf("--- Number of samples %d\n",nd);

		return 0;
	}
      }
    }

    /* Compute first value based on last_value from previous buffer.	*/
    /* In Steim 2A, the last data is sent as part of each blockette */

    if (nd > 0) 
    {
      databuff[0] = bp->previousSample + diffbuff[0];
    }

    //
    // Find the sample value by adding differences to values
    //

    for(int x=1;x<nd;x++)
    {
      databuff[x] = diffbuff[x] + databuff[x-1];
    }
    return nd;
}

int positionOfCompWord
(   const int          wordPosition,/* return the sample number of this word */
    QMABLOCK    *bp,            /* ptr to Steim2A blockette */
    qma_int32   *diffbuff,      /* ptr to unpacked data array.          */
    qma_int32   *databuff)      /* ptr to unpacked diff array.          */
{

    int         nd = 0;         /* # of data points in packet.          */
    int         wn;             /* current word number in the blockette */
    int         c;              /* current compression flag.            */

    qma_int32   i;
    qma_int32   n, bits, m1, m2;
    qma_int32   val, dnib;

    qma_int32  *diff = diffbuff;

    if (bp->numberOfSamples <= 0)
    {
      std::cout << "--- Number of samples <= 0:" << bp->numberOfSamples 
        << std::endl;
      return (false);
    }

    if(wordPosition < 0)
    {
      std::cout << "--- wordPosition: <0 " << wordPosition << std::endl;
      return (false);
    }

    if(!true)
    {
      std::cout << "--- Number Of Samples: " << bp->numberOfSamples << 
	std::endl;
      std::cout << "--- Map Length in Bytes: " << bp->mapLengthInBytes <<
	std::endl;
      std::cout << "--- Previous Sample: " << bp->previousSample <<
	std::endl;
      std::cout << "--- Data Length In Bytes: " << bp->dataLengthInBytes <<
	std::endl;
     for(int i=0;i<bp->mapLengthInBytes;i++)
     {
       std::cout << "Byte: " << i << " : " << std::hex << 
         (qma_uint16) bp->map[i] << " " << (qma_int32) bp->w[i] << std::endl;
     }
     std::cout << "Final value: " << std::dec << 10 << std::endl;
    }
    //
    // Vars used to walk through the map bytes
    //
    int curposition = -1;
    qma_uint8 curmap;
    qma_uint8 tmap;

    int numberOfMaps = bp->mapLengthInBytes*4;

    for (wn = 0; wn < numberOfMaps; wn++)
    {
      if (nd >= bp->numberOfSamples)
      {
        break;
      }
      if( (wn%4) == 0)
      { 
        ++curposition;
        // here's critical test. Have we decompressed all of the samples
        // up to and including the word we were interested in.
        if((wn) >= wordPosition)
        {
          return nd;
        }
        curmap = bp->map[curposition];
      }

      //rotate the map two bits to put the values into mask position
      tmap = curmap >> ((4-(wn%4)-1)*2);

      c = tmap & 0x03;

      //
      // Code to ignore the maps of type 0.
      //
      if(!true)
      {
        //std::cout << "--- Rotate value : " << ((4-(wn%4)-1)*2) << std::endl;
        //std::cout << "--- wn value : " << wn << std::endl;
        //std::cout << "--- CompType : " << c << " tmap " <<
        // (qma_uint16) tmap << std::endl;
        //std::cout << "--- NumberofSample :  " << bp->numberOfSamples
        //      << std::endl;
        std::cout << "--- Total maps  :      " << numberOfMaps << std::endl;
        std::cout << "--- Maps processed : " << wn << std::endl;
        std::cout << "--- Samples processed: " << nd << std::endl;
      }

      switch (c)
      {
        case STEIM2_SPECIAL_MASK:
        {
                /* Headers info -- skip it.                             */
                break;
        }
        case STEIM2_BYTE_MASK:
        {
                /* Next 4 bytes are 4 1-byte differences.               */
                /* NOTE: THIS CODE ASSUMES THAT CHAR IS SIGNED. */
                /* Note this was converted to handles words without unions */
                qma_int8 mbyte[4];
                for (i=0; i<4 && nd<bp->numberOfSamples; i++,nd++)
                {
                    if(i==0)
                    {
                        memcpy((char*)&mbyte[0],(char*)&bp->w[wn],4);
                    }
                    *diff++ = mbyte[i];
                }
                break;
        }
        case STEIM2_123_MASK:
        {
          val = qma_ntohl(bp->w[wn]);
          dnib =  val >> 30 & 0x3;
          switch (dnib)
          {
            case 1:     /*      1 30-bit difference.            */
              bits = 30; n = 1; m1 = 0x3fffffff; m2 = 0x20000000; break;
            case 2:     /*  2 15-bit differences.               */
              bits = 15; n = 2; m1 = 0x00007fff; m2 = 0x00004000; break;
            case 3:     /*  3 10-bit differences.               */
              bits = 10; n = 3; m1 = 0x000003ff; m2 = 0x00000200; break;
            default:
            {
              /*        should NEVER get here.          */
              printf ("xxx (3) Invalid ck, dnib, wn = %d, %d, %d\n",
                             c, dnib, wn);
              printf("xxx Number of samples %d\n",nd);
                    return 0;
            }
          }
          /*  Uncompress the differences.                       */
          for (i=(n-1)*bits; i>=0 && nd<bp->numberOfSamples; i-=bits,nd++)
          {
            *diff = (val >> i) & m1;
            *diff = (*diff & m2) ? *diff | ~m1 : *diff;
            diff++;
          }
          break;
        }
        case STEIM2_567_MASK:
        {
          val = qma_ntohl(bp->w[wn]);
          dnib =  val >> 30 & 0x3;
          switch (dnib)
          {
            case 0:     /*  5 6-bit differences.                */
                    bits = 6; n = 5; m1 = 0x0000003f; m2 = 0x00000020; break;
            case 1:     /*  6 5-bit differences.                */
                    bits = 5; n = 6; m1 = 0x0000001f; m2 = 0x00000010; break;
            case 2:     /*  7 4-bit differences.                */
                    bits = 4; n = 7; m1 = 0x0000000f; m2 = 0x00000008; break;
            default:
            {
              printf ("xxx (4) Invalid ck, dnib, wn = %d, %d, %d\n",
                             c, dnib, wn);
              printf("xxx Number of samples %d\n",nd);
              return 0;
             }
          }
          /*  Uncompress the differences.                       */
          for (i=(n-1)*bits; i>=0 && nd < bp->numberOfSamples; i-=bits,nd++)
          {
            *diff = (val >> i) & m1;
            *diff = (*diff & m2) ? *diff | ~m1 : *diff;
            diff++;
          }
          break;
        }
        default:
        {
          /* Should NEVER get here.                             */
          printf ("--- Error: unpack_steim2 - invalid ck, wn = %d, %d\n", c);
          printf("--- Number of samples %d\n",nd);

                return 0;
        }
      }
    }

    /* Compute first value based on last_value from previous buffer.    */
    /* In Steim 2A, the last data is sent as part of each blockette */

    if (nd > 0)
    {
      databuff[0] = bp->previousSample + diffbuff[0];
    }

    //
    // Find the sample value by adding differences to values
    //

    for(int x=1;x<nd;x++)
    {
      databuff[x] = diffbuff[x] + databuff[x-1];
    }
    return nd;
}
