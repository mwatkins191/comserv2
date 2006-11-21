/*
 * 16, 32, and 64 bit swapping routines
 *
 * These should do nothing on a big-endian architectures (i.e. SPARC)
 * but should swap to/from network byte order on little-endian
 * architectures (i.e. x86).
 *
 * Author:
 * Chad Trabant, ORFEUS Data Center
 *
 * PS - leaving the source extension .C so that it isn't clobbered
 * by the Makefile.
 */

#include <memory.h>
#ifdef _WIN32
# include <winsock2.h>
#endif
#include "qmaswap.h"

//
// Wrappers for system equivalents
//

qma_uint16 qma_htons (qma_uint16 val)
{
  return (qma_uint16) htons (val);
}
qma_uint16 qma_ntohs (qma_uint16 val)
{
  return (qma_uint16) ntohs (val);
}

qma_uint32 qma_htonl (qma_uint32 val)
{
  return (qma_uint32) htonl (val);
}
qma_uint32 qma_ntohl (qma_uint32 val)
{
  return (qma_uint32) ntohl (val);
}

//
// A 64-bit version of the host/net order functions
//

qma_uint64 qma_htonll (qma_uint64 val)
{
  // Check if our arch is *not* using network byte order,
  // swap the 64 bit value if not.
  if ( htons(0x1234) != 0x1234 )
    {
      qma_uint8  temp;
      union {
	qma_uint8  c[8];
      } dat;
      
      memcpy (&dat, &val, sizeof(dat));
      temp     = dat.c[0];
      dat.c[0] = dat.c[7];
      dat.c[7] = temp;
      
      temp     = dat.c[1];
      dat.c[1] = dat.c[6];
      dat.c[6] = temp;
      
      temp     = dat.c[2];
      dat.c[2] = dat.c[5];
      dat.c[5] = temp;
      
      temp     = dat.c[3];
      dat.c[3] = dat.c[4];
      dat.c[4] = temp;

      memcpy (&val, &dat, sizeof(dat));
    }

  return val;
}


qma_uint64 qma_ntohll (qma_uint64 val)
{
  return qma_htonll (val);
}

