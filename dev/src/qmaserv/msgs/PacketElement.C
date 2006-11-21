/*
 *
 * File     :
 *   PacketElement.C
 *
 * Purpose  :
 *
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   24 February 2002
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
#include <iostream>
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "PacketElement.h"
#include "Field.h"
#include "qmaswap.h"
#include "global.h"
//
// Size of packet will be established in a constructor based on the
// field size and length. Assuming the methods overwrite the constructor
// everything works without reference to length of DataElement.
//

PacketElement::PacketElement() 
{
	p_number_of_fields = 0;
}


//
// Copy bit string into this header
//

void PacketElement::setBitString(unsigned char* buf)
{

#ifdef QMA_LITTLE_ENDIAN
  int i;
  
  /*
   * Allocate the memory for the swap buffer
   */

  unsigned char *swappedBuf = (unsigned char *)malloc(getLengthInBytes());
  if(!swappedBuf) {
    std::cout << "+++ Unable to allocate memory for byteswap buffer" << std::endl;
    return;
  }

  /*
   * Uncomment this to see the bytes as they stream in
   * 
  for(i=0; i < getLengthInBytes(); i++) {
    if((i % 8) == 0) {
      printf("\n");
    }
    printf("%02x ", buf[i]);
  }
  printf("\n");
  */

  for(i=0; i < p_number_of_fields; i++) {
    Field thisField = p_fieldListPtr[i];
    qma_uint16 swapped16;
    qma_uint32 swapped32;
    qma_uint64 swapped64;
    qma_uint16 raw16;
    qma_uint32 raw32;
    qma_uint64 raw64;


    /*
     * Allocate the memory for this value
     */
    unsigned char *bytes = (unsigned char *)malloc(thisField.p_number_bytes);
    if(!bytes) {
      std::cout << "+++ Unable to allocate memory for byteswap element buffer" << std::endl;
      return;
    }

    /*
     * Copy the bytes
     */
    memcpy(bytes, &buf[thisField.p_start_position], thisField.p_number_bytes);

    switch (thisField.p_data_type) {
      case UnsignedInt16:
      case Int16:
	memcpy(&raw16, bytes, thisField.p_number_bytes);
	swapped16 = qma_htons(raw16);
	memcpy(bytes, &swapped16, thisField.p_number_bytes);
	break;
      case UnsignedInt32:
      case Int32:
	memcpy(&raw32, bytes, thisField.p_number_bytes);
	swapped32 = qma_htonl(raw32);
	memcpy(bytes, &swapped32, thisField.p_number_bytes);
	break;
      case UnsignedInt64:
      case Int64:
	memcpy(&raw64, bytes, thisField.p_number_bytes);
	swapped64 = qma_htonll(raw64);
	memcpy(bytes, &swapped64, thisField.p_number_bytes);
	break;
      case UnsignedInt128:
      case Int128:
	memcpy(&raw64, bytes, 8);
	swapped64 = qma_htonll(raw64);
	memcpy(bytes, &swapped64, 8);

	memcpy(&raw64, (&bytes[0])+8, 8);
	swapped64 = qma_htonll(raw64);
	memcpy((&bytes[0])+8, &swapped64, 8);

	break;
      case UnsignedInt8:
      case Int8:
      case ASCII:
	qma_uint8 tmp;
	memcpy(&tmp, bytes, thisField.p_number_bytes);
	break;
      case SegmentBuffer:
      case VariableLength:
	if(i == p_number_of_fields-1) {
	  thisField.p_number_bytes = p_length_in_bytes - thisField.p_start_position;
	}
	break;
    default:
      g_log << "+++ Unknown datatype: " << thisField.p_data_type << std::endl;
      break;
    }
    memcpy(&swappedBuf[thisField.p_start_position], bytes, thisField.p_number_bytes);
    free(bytes);

    //printf("setField (%d): %s\n", p_length_in_bytes, thisField.p_name);

  }
  memcpy(p_bits, swappedBuf, getLengthInBytes());
  free(swappedBuf);
#else
  memcpy(p_bits,buf,getLengthInBytes());
#endif
}


unsigned char* PacketElement::getBitString() const
{
#ifdef QMA_LITTLE_ENDIAN
  int i;
  for(i=0; i < p_number_of_fields; i++) {
    Field thisField = p_fieldListPtr[i];
    qma_uint16 swapped16;
    qma_uint32 swapped32;
    qma_uint64 swapped64;
    qma_uint16 raw16;
    qma_uint32 raw32;
    qma_uint64 raw64;
    /*
     * Allocate the memory for this value
     */
    unsigned char *bytes = (unsigned char *) malloc(thisField.p_number_bytes);
    if(!bytes) {
      std::cout << "+++ Unable to allocate memory for byteswap element buffer" << std::endl;
      return NULL;
    }

    /*
     * Copy the bytes
     */
    memcpy(bytes, &p_bits[thisField.p_start_position], thisField.p_number_bytes);

    switch (thisField.p_data_type) {
      case UnsignedInt16:
      case Int16:
	memcpy(&raw16, bytes, thisField.p_number_bytes);
	swapped16 = qma_htons(raw16);
	memcpy(bytes, &swapped16, thisField.p_number_bytes);
	break;
      case UnsignedInt32:
      case Int32:
	memcpy(&raw32, bytes, thisField.p_number_bytes);
	swapped32 = qma_htonl(raw32);
	memcpy(bytes, &swapped32, thisField.p_number_bytes);
	break;
      case UnsignedInt64:
      case Int64:
	memcpy(&raw64, bytes, thisField.p_number_bytes);
	swapped64 = qma_htonll(raw64);
	memcpy(bytes, &swapped64, thisField.p_number_bytes);
	break;
      case UnsignedInt128:
      case Int128:
	memcpy(&raw64, bytes, 8);
	swapped64 = qma_htonll(raw64);
	memcpy(bytes, &swapped64, 8);

	memcpy(&raw64, (&bytes[0])+8, 8);
	swapped64 = qma_htonll(raw64);
	memcpy((&bytes[0])+8, &swapped64, 8);

	break;
      case UnsignedInt8:
      case Int8:
      case ASCII:
	break;
      case SegmentBuffer:
      case VariableLength:
	if(i == p_number_of_fields-1) {
	  // this is a "catch all" field, rather than a placeholder
	  thisField.p_number_bytes = p_length_in_bytes - thisField.p_start_position;
	}
	break;
    default:
		g_log << "+++ Unknown datatype: " << thisField.p_data_type << " numField: " <<
			p_number_of_fields << std::endl;
      break;
    }

    memcpy((void *)&p_bits_swapped[thisField.p_start_position], bytes, thisField.p_number_bytes);
    free(bytes);

    //printf("GetField (%d:%d): %s\n",thisField.p_start_position,  p_length_in_bytes, thisField.p_name);

  }
  return (unsigned char *) &p_bits_swapped[0];
  //return (unsigned char *) &p_bits[0];
#else
  return (unsigned char*)&p_bits[0];
#endif
}

int PacketElement::getLengthInBytes() const
{
  return p_length_in_bytes;
}
