/*
 * File     :
 *  OptionBitsVO.C
 *
 * Purpose  :
 *   This VO encapsulated the information stored by the options bits in 
 *    an LCQ token.
 *    Some of the entries a flags, and if the flag is set, there is 
 *    additional data. A purer implementation might exclude access
 *    to fields unless the flag was set. This implementation allows access
 *    to fields even if the flag is not set. If this is done, you get
 *    invalid results from your call.
 *
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *  22 April 2002
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
#include "DetectorOptionsVO.h"
#include "OptionBitsVO.h"

OptionBitsVO::OptionBitsVO()
{
  p_bit0 = false;
  p_numberOfPreEventBuffers =0;

  p_bit1 = false;
  p_bit2 = false;
  p_bit3 = false;
  p_bit4 = false;
  p_gapThreshold = 0.0;

  p_bit5 = false;
  p_calibrationDelay = 0;

  p_bit6 = false;
  p_maxFrameCount = 0;

  p_bit7 = false;
  p_FIRMultiplier = 0.0;

  p_bit8 = false;
  p_samplesBetweenReports = 0;
  p_FIRFilterToUse = 0;

  p_bit9 = false;
  p_controlDetectorNumber = 0;    

  p_bit10 = false;
  p_sourceLCQNumber = 0;
  p_decimationFIRFilterNumber =0;
    
  //
  // The VO will be initialized with a default constructor
  // when the class is instantiated. As long as the DetectorOPtions
  // default constructor is written to init the objects, we don't
  // need to init them here.
  //

  p_bit12 = false;
  p_bit13 = false;
  p_bit14 = false;
  p_bit15 = false;
  p_bit16 = false;
  p_bit17 = false;
  p_bit18 = false;
  p_bit19 = false;

  p_bit28 = false;
  p_bit29 = false;
  p_bit30 = false;
  p_bit31 = false;
}

bool OptionBitsVO::getBit0()
{
  return p_bit0;
}

void OptionBitsVO::setBit0(bool torf)
{
  p_bit0 = torf;
}

void OptionBitsVO::setNumberOfPreEventBuffers(int peb)
{
  p_numberOfPreEventBuffers = peb;
}

int OptionBitsVO::getNumberOfPreEventBuffers()
{
  return p_numberOfPreEventBuffers;
}

bool OptionBitsVO::getBit1()
{
  return p_bit1;
}

void OptionBitsVO::setBit1(bool torf)
{
  p_bit1 = torf;
}

bool OptionBitsVO::getBit2()
{
  return p_bit2;
}

void OptionBitsVO::setBit2(bool torf)
{
  p_bit2 = torf;
}

bool OptionBitsVO::getBit3()
{
  return p_bit3;
}

void OptionBitsVO::setBit3(bool torf)
{
  p_bit3 = torf;
}

bool OptionBitsVO::getBit4()
{
  return p_bit4;
}

void OptionBitsVO::setBit4(bool torf)
{
  p_bit4 = torf;
}

float OptionBitsVO::getGapThreshold()
{
  return p_gapThreshold;
}

void  OptionBitsVO::setGapThreshold(float thresh)
{
  p_gapThreshold = thresh;
}

bool OptionBitsVO::getBit5()
{
  return p_bit5;
}
  
void OptionBitsVO::setBit5(bool torf)
{
  p_bit5 = torf;
}

int  OptionBitsVO::getCalibrationDelay()
{
  return p_calibrationDelay;
}

void OptionBitsVO::setCalibrationDelay(int secs)
{
  p_calibrationDelay = secs;
}

bool OptionBitsVO::getBit6()
{
  return p_bit6;
}
  
void OptionBitsVO::setBit6(bool torf)
{
  p_bit6 = torf;
}

int  OptionBitsVO::getMaxFrameCount()
{
  return p_maxFrameCount;
}

void OptionBitsVO::setMaxFrameCount(int maxCount)
{
  p_maxFrameCount = maxCount;
}

bool OptionBitsVO::getBit7()
{
  return p_bit7;
}

void OptionBitsVO::setBit7(bool torf)
{
  p_bit7 = torf;
}

float OptionBitsVO::getFIRMultiplier()
{
  return p_FIRMultiplier;
}
  
void  OptionBitsVO::setFIRMultiplier(float fm)
{
  p_FIRMultiplier = fm;
}

bool OptionBitsVO::getBit8()
{
  return p_bit8;
}

void OptionBitsVO::setBit8(bool torf)
{
  p_bit8 = torf;
}
  
int  OptionBitsVO::getNumberSamplesBetweenReports()
{
  return p_samplesBetweenReports;
}

void OptionBitsVO::setNumberSamplesBetweenReports(int samps)
{
  p_samplesBetweenReports = samps;
}

int  OptionBitsVO::getFIRFilterToUse()
{
  return p_FIRFilterToUse;
}

void OptionBitsVO::setFIRFilterToUse(int fftu)
{
  p_FIRFilterToUse = fftu;
}

bool OptionBitsVO::getBit9()
{
  return p_bit9;
}

void OptionBitsVO::setBit9(bool torf)
{
  p_bit9 = torf;
}

int  OptionBitsVO::getControlDetectorNumber()
{
  return p_controlDetectorNumber;
}

void OptionBitsVO::setControlDetectorNumber(int cdn)
{
  p_controlDetectorNumber = cdn;
}

bool OptionBitsVO::getBit10()
{
  return p_bit10;
}

void OptionBitsVO::setBit10(bool torf)
{
  p_bit10 = torf;
}

int  OptionBitsVO::getSourceLCQ()
{
  return p_sourceLCQNumber;
}
  
void OptionBitsVO::setSourceLCQ(int slcq)
{
  p_sourceLCQNumber = slcq;
}

int  OptionBitsVO::getDecimationFIRFilter()
{
  return p_decimationFIRFilterNumber;
}

void OptionBitsVO::setDecimationFIRFilter(int dff)
{
  p_decimationFIRFilterNumber = dff;
}

bool OptionBitsVO::getBit11()
{
  return p_bit11;
}

void OptionBitsVO::setBit11(bool torf)
{
  p_bit11 = torf;
}

bool OptionBitsVO::getBit12()
{
  return p_bit12;
}

void OptionBitsVO::setBit12(bool torf)
{
  p_bit12 = torf;
}
  
DetectorOptionsVO OptionBitsVO::getDetector1VO()
{
  return p_detector1;
}

void OptionBitsVO::setDetector1VO(DetectorOptionsVO optVO)
{
  p_detector1 = optVO;
}

bool OptionBitsVO::getBit13()
{
  return p_bit13;
}

void OptionBitsVO::setBit13(bool torf)
{
  p_bit13 = torf;
}
  
DetectorOptionsVO OptionBitsVO::getDetector2VO()
{
  return p_detector2;
}
 
void OptionBitsVO::setDetector2VO(DetectorOptionsVO optVO)
{
  p_detector2 = optVO;
}

bool OptionBitsVO::getBit14()
{
  return p_bit14;
}
  
void OptionBitsVO::setBit14(bool torf)
{
  p_bit14 = torf;
}
 
DetectorOptionsVO OptionBitsVO::getDetector3VO()
{
  return p_detector3;
}

void OptionBitsVO::setDetector3VO(DetectorOptionsVO optVO)
{
  p_detector3 = optVO;
}

bool OptionBitsVO::getBit15()
{
  return p_bit15;
}
  
void OptionBitsVO::setBit15(bool torf)
{
  p_bit15 = torf;
}
  
DetectorOptionsVO OptionBitsVO::getDetector4VO()
{
  return p_detector4;
}
  
void OptionBitsVO::setDetector4VO(DetectorOptionsVO optVO)
{
  p_detector4 = optVO;
}

bool OptionBitsVO::getBit16()
{
  return p_bit16;
}
  
void OptionBitsVO::setBit16(bool torf)
{
  p_bit16 = torf;
}
  
DetectorOptionsVO OptionBitsVO::getDetector5VO()
{
  return p_detector5;
}
  
void OptionBitsVO::setDetector5VO(DetectorOptionsVO optVO)
{
  p_detector5 = optVO;
}

bool OptionBitsVO::getBit17()
{
  return p_bit17;
}
  
void OptionBitsVO::setBit17(bool torf)
{
  p_bit17 = torf;
}
 
DetectorOptionsVO OptionBitsVO::getDetector6VO()
{
  return p_detector6;
}
  
void OptionBitsVO::setDetector6VO(DetectorOptionsVO optVO)
{
  p_detector6 = optVO;
}

bool OptionBitsVO::getBit18()
{
  return p_bit18;
}
  
void OptionBitsVO::setBit18(bool torf)
{
  p_bit18 = torf;
}
  
DetectorOptionsVO OptionBitsVO::getDetector7VO()
{
  return p_detector7;
}
  
void OptionBitsVO::setDetector7VO(DetectorOptionsVO optVO)
{
  p_detector7 = optVO;
}

bool OptionBitsVO::getBit19()
{
  return p_bit19;
}
  
void OptionBitsVO::setBit19(bool torf)
{
  p_bit19 = torf;
}
  
DetectorOptionsVO OptionBitsVO::getDetector8VO()
{
  return p_detector8;
}
  
void OptionBitsVO::setDetector8VO(DetectorOptionsVO optVO)
{
  p_detector8 = optVO;
}

bool OptionBitsVO::getBit28()
{
  return p_bit28;
}

void OptionBitsVO::setBit28(bool torf)
{
  p_bit28 = torf;
}

bool OptionBitsVO::getBit29()
{
  return p_bit29;
}
 
void OptionBitsVO::setBit29(bool torf)
{
  p_bit29 = torf;
}

bool OptionBitsVO::getBit30()
{
  return p_bit30;
}

void OptionBitsVO::setBit30(bool torf)
{
  p_bit30 = torf;
}

bool OptionBitsVO::getBit31()
{
  return p_bit31;
}

void OptionBitsVO::setBit31(bool torf)
{
  p_bit31 = torf;
}
