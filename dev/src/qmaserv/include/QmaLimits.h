/*

 * File     :
 *  QmaLimits.h
 *
 * Purpose  :
 *
 * These types are used to make sure that all types are correctly
 * defined on all platforms.
 *
 * The storage size is primarily an issue on data that is going on or
 * off the wire. If the wrong type is used, data can be mis-interpreted.
 *
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  14 Feb 2002
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
 *
 */

#ifndef QMALIMITS_H
#define QMALIMITS_H


//
// Incoming Q300 packet values
// 
const int PACKET_MODULUS = 65536; //2^16
const int MAX_SLIDING_WINDOW_SIZE = 128;


const int MAX_BITS_DATA = 536;
const int MAX_BYTES_IN_DATA = 536;
const int BYTES_IN_QDP_HEADER = 12;
const int MAX_BYTES_IN_PACKET = 548;
const int BYTES_IN_CRC = 4;
const int QDP_HEADER_SIZE_IN_BYTES = 12;
const int MAXPACKETSIZE = 576;  // max bytes we can send at once
const int MAXDATASIZE   = 536;  // max num. of bytes w/o the IP,UDP,QDP heade
const int C1_MAXSEG     = 438;  // max bytes in token mem segment buffer
const int C1_MAXCFG     = 7884; // max bytest in token buffer
const int C1_OVERHEAD   = 10;   // segment overhead on q330
const int MAX_SAMPLES_IN_BLOCKETTE = 200; // Based on the max sample rate.
const int MAX_BLOCKETTES_IN_SECOND = 32; // Based on DP Writers guide
const int MAX_WORDS_IN_PACKET = 103;  // 13 (1st frame) + 6x15  = 7 frames in packet
//
// The following is based on map values as 2 bit values. Then there
// are 4 map values per bytes.
//
const int MAX_BYTES_IN_BLOCKETTE_MAP = MAX_SAMPLES_IN_BLOCKETTE/4;
//
// LCQ and Outgoing MINISEED packet values
//
const int MAX_SAMPLES_IN_SECOND = 200; // Used to define an array size
const int MAX_SAMPLES_IN_PACKET = 500; // Assuming 8 bit samples and 512
					// byte packets. There is header 
			                // so 500 samples should be safely
					// above any max in compression queue
const int XXX_SAMPLES_IN_PACKET = 100; //
  // Temporary work around to generate packets. Send out packet when we
  // have at least this many samples
  //

const int BYTES_IN_MINISEED_PACKET = 512;
const int MAX_BLOCKETTES_IN_LCQ = 129; // 128 plus 1
const double ONE_MILLION = 1.0e6;  // define one mill
const double ONE_USEC    = 1.0e-6; // define microsecond
const int ONE_MILLION_INTEGER = 1000000; // one mill integer
//
// These are string lengths. These are actual lengths
// and the terminating null needs to be added if needed.
//
const int LOCATION_CODE_LEN = 2;
const int SEED_NAME_LEN     = 3; 
const int NETWORK_CODE_LEN  = 2; // This is a Q330 limit
const int STATION_CODE_LEN  = 5;
const int DSS_PASSWORD_LEN  = 21; // 3 * 7 in Q330
const int MAX_CHARS_IN_STATION_CODE = 4; // This is a comserv limit
                                         // Mountainair must use this max
//
// These values are for the LCQ definitions of 
// channels and frequencies derived from a channel.
//
const int MAX_CHANNELS = 6; /* Given in comm protocol guide as range 0-5 */
const int MAX_FREQS   = 8;  /* Givein in com protocol guide as bits 0-2 */

//
// Constants for handling Main and Analog channels.
// Define Max_Type and Max_Freqs.
// Use Max Channels from above, although currently define max for
// analog and main channels are 4 (opto Inputs) not 6.

const int MAX_TYPES = 16; /* drop high bit, then 4 bits left = 16 */

// Highest main and analog channel frequency byte is 3 based on 3 channels
// of data in one blockette.
const int MAX_MAIN_FREQS = 3; 


const int SEQNUM_LEN = 8;
const int MAX_REFERENCE_NUMBER = 96; // based on DP Writer Guide
//
// The following is used to size an array in the ChanMap routine. The array
// must be big enough to hold a value for each status channel, or each
// main channel, but not both. I assume that the number of main channels
// (96) is more than the number of status channels, so I use that as the 
// largest possible.

const int MAX_STATUS_OR_MAIN_CHANNELS = 96; 
const int EIGHTY_CHARS_STRING = 81;

//
// Message lengths. These are the "currently defined" field counts.
// A number of messages have more fields, but MA does not interpret
// them all so the counter here is lower than the actual count.
//
const int FIELDS_IN_QDPHEADER = 6;
const int FIELDS_IN_C1_STAT   = 7;
const int FIELDS_IN_C1_RQSTAT = 1;
const int FIELDS_IN_C1_MEM    = 4;
const int FIELDS_IN_C1_RQMEM  = 4;
const int FIELDS_IN_C1_DSRV   = 1;
const int FIELDS_IN_C1_RQSRV  = 1;
const int FIELDS_IN_C1_SRVCH  = 4;
const int FIELDS_IN_C1_SRVRSP = 7;
const int FIELDS_IN_C1_RQFLGS = 1;
const int FIELDS_IN_C1_FLGS   = 4;
const int FIELDS_IN_C1_CERR   = 1;
const int FIELDS_IN_C1_LOG    = 24;
const int FIELDS_IN_C1_SGLOB  = 20;
const int FIELDS_IN_C1_FIX    = 29;
const int FIELDS_IN_C1_SC     = 8;
const int FIELDS_IN_C1_UMSG   = 2;

const int FIELDS_IN_DT_DATA   = 2;
const int FIELDS_IN_DT_FILL   = 1; 
const int FIELDS_IN_DT_DACK   = 7;
const long SECONDS_BETWEEN_1970_2000   = 946684800;


/* The software can empty an entire LCQ into a PCQ if one blockette */
/* was holding up the queue. If the queue is nearly full (say 102) then */
/* 129 could come in. The PCQ must be big enough to hold all these packets */
/* without overflowing. The size is now calculated as */
/* 103 (blockettes in 1 sps data + 129 blockettes in LCQ = 232 */
/* High sample rate max is now estimated this way : 100 sps data */
/* all split packets. The number of packets in the PCQ will not exceed */
/* 5 seconds x 2 = 10 packets plus 127 seconds x 2 = 254. */
/* This gives 264, rounded up to 265 */

const int MAX_SECONDS_IN_COMPRESSION_QUEUE = 265; 

//
// State retries
//
const int CHALLENGE_RETRY_INTERVAL = 10;
const int CHALLENGE_RETRY_COUNT = 10;
const int STATUS_RETRY_INTERVAL = 5;
const int STATUS_RETRY_COUNT = 10;
const int FLAGS_RETRY_INTERVAL = 5;
const int FLAGS_RETRY_COUNT = 10;

const int DISCONNECT_RETRY_INTERVAL = 5;
const int DISCONNECT_RETRY_COUNT = 10;

const int TOKEN_RETRY_INTERVAL = 5;
const int TOKEN_RETRY_COUNT = 10;

const int USER_MESSAGE_RETRY_INTERVAL = 5;
const int USER_MESSAGE_RETRY_COUNT = 10;

const int OPEN_DATA_PORT_RETRY_INTERVAL = 100;
const int OPEN_DATA_PORT_RETRY_COUNT = 10;

const int STATUS_REQUEST_RETRY_INTERVAL = 100;
const int STATUS_REQUEST_RETRY_COUNT    = 100;
const int Q330_SYSTEM_VERSION = 0;

const int MIN_STATUS_INTERVAL = 5;
const int MAX_STATUS_INTERVAL = 200;
const int DEFAULT_STATUS_INTERVAL = STATUS_REQUEST_RETRY_INTERVAL;

const int MIN_DATA_RATE_INTERVAL = 1;
const int MAX_DATA_RATE_INTERVAL = 100;
const int DEFAULT_DATA_RATE_INTERVAL = 3;

const int MAX_TERMINATE_WAIT = 1200; /* Seconds */
const int MAX_UNSIGNED_8INT = 255;
#endif
