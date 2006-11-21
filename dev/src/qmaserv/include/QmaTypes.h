/*
 *
 * File     :
 *  QmaTypes.h
 *
 * Purpose  :
 *  These types are used to make sure that all types are correctly
 *  defined on all platforms.
 *
 * The storage size is primarily an issue on data that is going on or
 * off the wire. If the wrong type is used, data can be mis-interpreted.
 *
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  17 March 2002
 *
 * File     :
 *
 *
 * Purpose  :
 *
 *
 * Author   :
 *
 *
 * Mod Date :
 *
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
#ifndef QMATYPES_H
#define QMATYPES_H
//
// These are the basic data types used throught the program.
// The current definitions are for Solaris 2.8
// If this is ported to another platform, the type names should be left alone
// and the definition types changed to the appropriate ones for the platform.
// The qma prefix is used prevent conflict between Quanterra Mountainair 
// (qma) programs, and other programs defining similiar typdefs.
//
// There are unsigned, and signed version for most definitions.
//
typedef  unsigned char          qma_uint8;     // 8 bits unsigned
typedef  unsigned short int     qma_uint16;    // 16 bits unsigned
typedef  unsigned long int      qma_uint32;    // 32 bits unsigned

#ifdef _WINNT
typedef  unsigned __int64 qma_uint64;    // 64 bit unsigned int
typedef  __int64          qma_int64;     // 64 bits signed
#else
typedef  unsigned long long int qma_uint64;    // 64 bit unsigned int
typedef  long long int          qma_int64;     // 64 bits signed
#endif

typedef  signed char            qma_int8;     // 8 bits signed
typedef  short int              qma_int16;    // 16 bits signed
typedef  int                    qma_int32;    // 32 bits signed

typedef  char			qma_char;      // character type
//
// The Q330 version goes in every packet. If it doesn't match this
// value, maybe this verion of Mountainair won't work.
//
const int QDP_VERSION =  2;   /* QDP Version for Software Version 1.xx */

//
// Simple return result values. We can refer to success and failure
// not 1 and -1
//

const int QMA_SUCCESS  = 1;
const int QMA_FAILURE = -1;

//
// These enum types are used in Field definitions.
//
enum DataTypes {UnsignedInt8,
                UnsignedInt16,
                UnsignedInt32,
                UnsignedInt64,
                UnsignedInt128,
                Int8,
                Int16,
                Int32,
                Int64,
                Int128,
		ASCII,
		SegmentBuffer,
		VariableLength};

enum CheckResults {NothingToDo,
                   Process,
                   Error};


//
// This id goes into a reserved block in the MSEED Header.
// Currently identifies the Datalogger (6 = Q330). Eventually
// will identify datalogger and MSEED writer.
//
const int QMA_BLOCKETTE_1001_ID = 6;

//
// The following are message type definitions in the packets themselves.
//
const int C1_FLGS_VAL   = 0xb1;   /* flags response */
const int C1_CACK_VAL   = 0xa0;   /* Command Acknowledge */
const int C1_RQSRV_VAL  = 0x10;   /* Request Server Registration */
const int C1_SRVCH_VAL  = 0xa1;   /* Server Challenge */
const int C1_SRVRSP_VAL = 0x11;   /* Server Response */
const int C1_CERR_VAL   = 0xa2;   /* Command Error */
const int C1_DSRV_VAL   = 0x12;   /* Delete Server */
const int C1_RQFLGS_VAL = 0x34;   /* request flags */
const int C1_RQMEM_VAL  = 0x41;   /* request memory segment (tokens) */
const int C1_MEM_VAL    = 0xb8;   /* memory contents (tokens) */

const int C1_RQSTAT_VAL  = 0x1f;   /* request status message */
const int C1_STAT_VAL    = 0xa9;   /* Status message */
const int C1_UMSG_VAL    = 0x30;   /* User Messages to Q330 */

const int DT_OPEN_VAL   = 0x0b;
const int DT_DATA_VAL   = 0x00;
const int DT_FILL_VAL   = 0x06;
const int DT_DACK_VAL   = 0x0a;

//
// These are error types returned in the error field
//
const int CERR_PERM   =    0;   /* No Permission */
const int CERR_TMSERV =    1;   /* Too many servers */
const int CERR_NOTR   =    2;   /* You are not registered */
const int CERR_INVREG =    3;   /* Invalid Registration Request */
const int CERR_PAR    =    4;   /* Parameter Error */
const int CERR_SNV    =    5;   /* Structure not valid */
const int CERR_CTRL   =    6;   /* Control Port Only */
const int CERR_SPEC   =    7;   /* Special Port Only */
const int CERR_MEM    =    8;   /* Memory operation already in progress */
const int CERR_CIP    =    9;   /* Calibration in Progress */
const int CERR_DNA    =   10;   /* Data not Available */
const int CERR_DB9    =   11;   /* Console Port Only */

//
// These are message type defines
//

const qma_uint8 DATA_BLOCKETTE_FLAG =  0x80;   // bit 15 set if data blockette 
const qma_uint8 DCM_ST    =  0xe0;   // Status mask */
const qma_uint8 DCM       =  0xf8;   // Mask for all others */
const qma_uint8 DC_ST38   =  0x00;   // Status - 3 x 8 bit parameters */
const qma_uint8 DC_ST816  =  0x20;   /* Status - 8 and 16 */
const qma_uint8 DC_ST32   =  0x40;   /* Status - 8, 16, and 32 */
const qma_uint8 DC_ST232  =  0x60;   /* Status - 8, 16, and 2 X 32 */
const qma_uint8 DC_MN38   =  0x80;   /* Main data sample - 3 x 8 */
const qma_uint8 DC_MN816  =  0x88;   /* Main - 8 and 16 */
const qma_uint8 DC_MN32   =  0x90;   /* Main - 8, 16, and 32 */
const qma_uint8 DC_MN232  =  0x98;   /* Main - 8, 16, and 2 X 32 */
const qma_uint8 DC_AG38   =  0xa0;   /* Analog sample - 3 x 8 */
const qma_uint8 DC_AG816  =  0xa8;   /* Analog sample - 8 and 16 */
const qma_uint8 DC_AG32   =  0xb0;   /* Analog sample - 8, 16, and 32 */
const qma_uint8 DC_AG232  =  0xb8;   /* Analog sample - 8, 16, and 2 x 32 */
const qma_uint8 DC_CNP38  =  0xc0;   /* CNP - 3 x 8 */
const qma_uint8 DC_CNP816 =  0xc8;   /* CNP - 8 and 16 */
const qma_uint8 DC_CNP316 =  0xd0;   /* CNP - 3 x 16 */
const qma_uint8 DC_CNP232 =  0xd8;   /* CNP - 8, 16, and 2 x 32 */
const qma_uint8 DC_D32    =  0xe0;   /* Digitizer - 8, 16, and 32, used for 1Hz */
const qma_uint8 DC_COMP   =  0xe8;   /* Digitizer - compress map+multiple samples */
const qma_uint8 DC_MULT   =  0xf0;   /* Digitizer - continuation of above */
const qma_uint8 DC_SPEC   =  0xf8;   /* Special purpose packets */

/* DC_MULT flags */
const qma_uint16 DMLS      = 0x8000L;   /* Last segment */
const qma_uint16 DMSZ      = 0x3ff;     /* blockette size mask */

/* Manually Blocked Blockettes */
const int MBB_GPS      =   1;       /* Gps Power cycling */
const int MBB_DIG      =   2;       /* Digitizer Phase Changes */
const int MBB_CHRG     =   4;       /* Power Supply charging phase */
const int MBB_CAL      =   8;       /* Calibration */
const int MBB_CFG      =   0x10;    /* Configuration changes */

/* Automatic Blocked Blockettes */
const int ABB_MN38       = 1;       /* DC_MN38 blockettes */
const int ABB_MN816      = 2;       /* DC_MN816 blockettes */
const int ABB_BOOM       = 4;       /* Boom positions */
const int ABB_STEMP      = 8;       /* Seismo Temperatures */
const int ABB_AVOLT      = 0x10;    /* Analog Voltage */
const int ABB_PWR        = 0x20;    /* Power supply parameters */
const int ABB_CBLK       = 0x40;    /* CNP block data */

/* Digitizer Phase Change Constants, first parameter */
const int DPC_STARTUP    = 0;
const int DPC_WAIT       = 1;
const int DPC_INT        = 2;
const int DPC_EXT        = 3;

/* Digitizer Phase Change COnstants, second parameter */
const int DPR_NORM       = 0;
const int DPR_DRIFT      = 1;
const int DPR_GOTCLK     = 2;
const int DPR_CMD        = 3;

/* Recording Window status, first parameter */
const int RWR_START     =  0;
const int RWR_STOP      =  1;

/* Special Purpose Blockettes */
const int SP_CALSTART    = 0;   /* calibration start */
const int SP_CALABORT    = 1;   /* calibration abort */
const int SP_CNPBLK      = 2;   /* CNP Block data */
const int SP_CFGBLK      = 3;   /* Configuration Blockette */

/* From DRM_SEISMO.h - Packet types for comserv_send() */

const int DATA_PACKET    =  1;  /*From drm_seismo.h */

/* Clock Values */

const qma_uint16 CQ_LOCK   = 0x0001;
const qma_uint16 CQ_2D     = 0x0002;
const qma_uint16 CQ_3D     = 0x0004;
const qma_uint16 CQ_1D     = 0x0008;
const qma_uint16 CQ_FILT   = 0x0010;
const qma_uint16 CQ_SPEC   = 0x0020;

const qma_uint16 PLL_OFF   = 0x0000;
const qma_uint16 PLL_HOLD  = 0x0040;
const qma_uint16 PLL_TRACK = 0x0080;
const qma_uint16 PLL_LOCK  = 0x00C0;

const qma_uint8  SIF_LOCKED = 0x20;
const qma_uint8  SQF_QUESTIONABLE_TIMETAG = 0x80;
const qma_uint8  CALIBRATION_ON = 0x01;
const qma_uint8  CALIBRATION_OFF = 0x00;
const int        QMA_TERMINATE = 14;

#endif
