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
 */

#ifndef _WIN32
#include <netinet/in.h>
#endif

#include "QmaTypes.h"

#ifndef QMASWAP_H
#define QMASWAP_H

#ifdef __cplusplus
extern "C" {
#endif

extern qma_uint16 qma_htons (qma_uint16 val);
extern qma_uint16 qma_ntohs (qma_uint16 val);

extern qma_uint32 qma_htonl (qma_uint32 val);
extern qma_uint32 qma_ntohl (qma_uint32 val);

extern qma_uint64 qma_htonll (qma_uint64 val);
extern qma_uint64 qma_ntohll (qma_uint64 val);

#ifdef __cplusplus
}
#endif

#endif
