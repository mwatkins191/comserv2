/*
 * File     :
 *  QmaDiag.h
 *
 * Purpose  :
 *  These are diagnostic values used by the program.
 *  The standard values specified by the program are included here.
 *  Also specific target are defined here, to give the ability to
 *  print specific diagnostic statements if needed.
 *
 * Author   :
 *  Phil Maechling.
 *
 * Mod Date :
 *  27 July 2002
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
#ifndef QMADIAG_H
#define QMADIAG_H

const int D_SILENT        = 0;
const int D_MAJOR         = 1;
const int D_MINOR         = 2;
const int D_EVERYTHING    = 3;

//
// These are specific targets in the source code.
// The software will print diagnostics about the
// routines in the log file, if you select one
// of interest.
//
const int D_NO_TARGET	       = 0;
const int D_EPOCH              = 1;
const int D_NETIP              = 2;
const int D_PORT_ON_WRITE      = 3;
const int D_PORT_ON_READ       = 4;
const int D_PRINT_PACKET_ON_TX = 5;
const int D_PRINT_PACKET_ON_RX = 6;
const int D_SET_STATE          = 7;   
const int D_PORT_RESET         = 8;
const int D_QMA_CONFIG         = 9;
const int D_SHOW_STATE         = 10;
const int D_OPEN_LOCAL_PORT    = 11;
const int D_SEND_SERVER_CHALLENGE = 12;
const int D_RX_CMD_PACKET         = 13;
const int D_SEND_USER_MESSAGE     = 14;
const int D_FIXED_FLAGS           = 15;
const int D_SENSORCONTROL_FLAGS   = 16;
const int D_LOGFLAGS_FLAGS        = 17;

const int D_SLIDING_WINDOW          = 18;
const int D_SLIDING_WINDOW_ERRORS   = 19;
const int D_TOKEN_REQUEST           = 20;
const int D_RECEIVE_TOKENS	    = 21;
const int D_DATA_PACKETS	    = 22; 
const int D_LCQ_CREATE		    = 23;
const int D_READ_TOKENS		    = 24;
const int D_INSERT_BLOCKETTE        = 25;
const int D_MOVE_BLOCKETTE          = 26;
const int D_CHECK_PENDING_BLOCKETTE = 27;
const int D_ACK			    = 28;
const int D_RX_SEQUENCE		    = 29;
const int D_ACK_PACKET              = 30;
const int D_DRSN		    = 31;
const int D_LCQ		            = 32;
const int D_COMP_PACKET             = 33;
const int D_CHECK_LCQ	            = 34;
const int D_LCQ_REMOVE              = 35;
const int D_COMSERV		    = 36;
const int D_CRC_CHECK               = 37;
const int D_ACK_COUNTER             = 38;
const int D_PROCESS_IQ              = 39;
const int D_ACK_BIT_POSITION        = 40;
const int D_SEQNO                   = 41;
const int D_INPUT_QUEUE             = 42;
const int D_SPLITS	            = 43;		
const int D_FILL_PACKETS            = 44;
const int D_STATUS_PACKETS          = 45;
const int D_STATUS_RESPONSE         = 46;
const int D_CLOCKQUALITY            = 47;
const int D_CMD_MSG                 = 48; 
const int D_STATUS_FLAGS            = 49;
const int D_PARTIALPACKETS          = 50;
const int D_MULTS                   = 51;
const int D_FRAME_COUNT             = 52;
#endif
