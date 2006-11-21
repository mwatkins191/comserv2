/*
 * File     :
 *  OptionBitsVO.h
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
#ifndef OPTIONBITSVO_H
#define OPTIONBITSVO_H

#include "DetectorOptionsVO.h"

class OptionBitsVO
{
  public:
    OptionBitsVO();
    ~OptionBitsVO() {};

  //
  // List of All the getters and setters. 
  // If a bit has a field associated it is listed next to it
  //

  bool getBit0();
  void setBit0(bool torf);
  void setNumberOfPreEventBuffers(int peb);
  int  getNumberOfPreEventBuffers();


  bool getBit1();  
  void setBit1(bool torf);

  bool getBit2();
  void setBit2(bool torf);

  bool getBit3();
  void setBit3(bool torf);

  bool getBit4();
  void setBit4(bool torf);
  float getGapThreshold();
  void  setGapThreshold(float thresh);

  bool getBit5();
  void setBit5(bool torf);
  int  getCalibrationDelay();
  void setCalibrationDelay(int secs);

  bool getBit6();
  void setBit6(bool torf);
  int  getMaxFrameCount();
  void setMaxFrameCount(int maxCount);

  bool getBit7();
  void setBit7(bool torf);
  float getFIRMultiplier();
  void  setFIRMultiplier(float fm);

  bool getBit8();
  void setBit8(bool torf);
  int  getNumberSamplesBetweenReports();
  void setNumberSamplesBetweenReports(int nsbr);
  int  getFIRFilterToUse();
  void setFIRFilterToUse(int fftu);

  bool getBit9();
  void setBit9(bool torf);
  int  getControlDetectorNumber();
  void setControlDetectorNumber(int cdn);

  bool getBit10();
  void setBit10(bool torf);
  int  getSourceLCQ();
  void setSourceLCQ(int slcq);
  int  getDecimationFIRFilter();
  void setDecimationFIRFilter(int dff);

  bool getBit11();
  void setBit11(bool torf);

  bool getBit12();
  void setBit12(bool torf);
  DetectorOptionsVO getDetector1VO();
  void setDetector1VO(DetectorOptionsVO optVO);

  bool getBit13();
  void setBit13(bool torf);
  DetectorOptionsVO getDetector2VO();
  void setDetector2VO(DetectorOptionsVO optVO);

  bool getBit14();
  void setBit14(bool torf);
  DetectorOptionsVO getDetector3VO();
  void setDetector3VO(DetectorOptionsVO optVO);

  bool getBit15();
  void setBit15(bool torf);
  DetectorOptionsVO getDetector4VO();
  void setDetector4VO(DetectorOptionsVO optVO);

  bool getBit16();
  void setBit16(bool torf);
  DetectorOptionsVO getDetector5VO();
  void setDetector5VO(DetectorOptionsVO optVO);

  bool getBit17();
  void setBit17(bool torf);
  DetectorOptionsVO getDetector6VO();
  void setDetector6VO(DetectorOptionsVO optVO);

  bool getBit18();
  void setBit18(bool torf);
  DetectorOptionsVO getDetector7VO();
  void setDetector7VO(DetectorOptionsVO optVO);

  bool getBit19();
  void setBit19(bool torf);
  DetectorOptionsVO getDetector8VO();
  void setDetector8VO(DetectorOptionsVO optVO);

  bool getBit28();
  void setBit28(bool torf);

  bool getBit29();
  void setBit29(bool torf);

  bool getBit30();
  void setBit30(bool torf);

  bool getBit31();
  void setBit31(bool torf);

  private:
    bool p_bit0;
    int  p_numberOfPreEventBuffers;

    bool  p_bit1;
    bool  p_bit2;
    bool  p_bit3;
    bool  p_bit4;
    float p_gapThreshold;

    bool p_bit5;
    int  p_calibrationDelay;

    bool p_bit6;
    int  p_maxFrameCount;

    bool  p_bit7;
    float p_FIRMultiplier;

    bool p_bit8;
    int  p_samplesBetweenReports;
    int  p_FIRFilterToUse;

    bool p_bit9;
    int  p_controlDetectorNumber;    

    bool p_bit10;
    int p_sourceLCQNumber;
    int p_decimationFIRFilterNumber;
   
    bool p_bit11;
 
    bool p_bit12;
    DetectorOptionsVO p_detector1;

    bool p_bit13;
    DetectorOptionsVO p_detector2;

    bool p_bit14;
    DetectorOptionsVO p_detector3;

    bool p_bit15;
    DetectorOptionsVO p_detector4;

    bool p_bit16;
    DetectorOptionsVO p_detector5;

    bool p_bit17;
    DetectorOptionsVO p_detector6;

    bool p_bit18;
    DetectorOptionsVO p_detector7;

    bool p_bit19;
    DetectorOptionsVO p_detector8;

    bool p_bit28;
    bool p_bit29;
    bool p_bit30;
    bool p_bit31;

};

#endif
