/*
 *
 * File     :
 *   c1_fix.h
 *
 * Purpose  :
 *   This is a Fixed Value after Reboot message.
 *
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   4 March 2002
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

#ifndef C1_FIX_H
#define C1_FIX_H

#include "Field.h"
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "PacketElement.h"

class c1_fix : public PacketElement
{
  public:

    c1_fix();
    ~c1_fix() {};

    //
    // Operations on fields
    //
    
    qma_uint32 getTimeOfLastReboot();
    qma_uint32 getTotalNumberOfReboots();
    qma_uint32 getBackupDataStructureBitmap();
    qma_uint32 getDefaultDataStructureBitmap();
    qma_uint16 getCalibratorType();
    qma_uint16 getCalibratorVersion();
    qma_uint16 getAuxType();
    qma_uint16 getAuxVersion();
    qma_uint16 getClockType();
    qma_uint16 getEthernetInstalled();
    qma_uint16 getSystemVersion();
    qma_uint16 getSlaveProcessorVersion();
    qma_uint16 getPLDVersion();
    qma_uint16 getMemoryBlockSize();
    qma_uint32 getKMIPropertyTag();
    qma_uint64 getSystemSerialNumber();
    qma_uint64 getAnalogSerialNumber();
    qma_uint64 getSeismometer1SerialNumber();
    qma_uint64 getSeismometer2SerialNumber();
    qma_uint64 getSeismometer3SerialNumber();
    qma_uint64 getSeismometer4SerialNumber();
    qma_uint32 getQAPCHP1SerialNumber();
    qma_uint32 getInternalDataMemorySize();
    qma_uint32 getInternalDataMemoryUsed();
    qma_uint32 getExternalDataMemorySize();
    qma_uint32 getFlashDataMemorySize();
    qma_uint32 getQAPCHP2SerialNumber();
    qma_uint32 getDataPort1PacketMemorySize();
    qma_uint32 getDataPort2PacketMemorySize();
    qma_uint32 getDataPort3PacketMemorySize();
    qma_uint32 getDataPort4PacketMemorySize();
    qma_uint8  getBit7Freq();
    qma_uint8  getBit6Freq();
    qma_uint8  getBit5Freq();
    qma_uint8  getBit4Freq();
    qma_uint8  getBit3Freq();
    qma_uint8  getBit2Freq();
    qma_uint8  getBit1Freq();
    qma_uint8  getBit0Freq();
    qma_uint8  getBitFreq(qma_uint8 bitnum);

    qma_int32 getChannels13Bit7FreqDelay();
    qma_int32 getChannels13Bit6FreqDelay();
    qma_int32 getChannels13Bit5FreqDelay();
    qma_int32 getChannels13Bit4FreqDelay();
    qma_int32 getChannels13Bit3FreqDelay();
    qma_int32 getChannels13Bit2FreqDelay();
    qma_int32 getChannels13Bit1FreqDelay();
    qma_int32 getChannels13Bit0FreqDelay();

    qma_int32 getChannels46Bit7FreqDelay();
    qma_int32 getChannels46Bit6FreqDelay();
    qma_int32 getChannels46Bit5FreqDelay();
    qma_int32 getChannels46Bit4FreqDelay();
    qma_int32 getChannels46Bit3FreqDelay();
    qma_int32 getChannels46Bit2FreqDelay();
    qma_int32 getChannels46Bit1FreqDelay();
    qma_int32 getChannels46Bit0FreqDelay();

    qma_uint32 frequencyBit2Integer(qma_uint8 fbit);
    double     frequencyBit2Double(qma_uint8 fbit);

    Field  p_fieldList[FIELDS_IN_C1_FIX];

};

#endif
