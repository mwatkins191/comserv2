/*
 * File     :
 *  DetectorOptionsVO.h
 *
 * Purpose  :
 *   This encapsulates the detector option information retreived
 *   from a Logical Port Token.
 *
 * Author   :
 *   Phil Maechling
 *
 * Mod Date :
 *   22 April 2002
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
#ifndef DETECTOROPTIONS_H
#define DETECTOROPTIONS_H

class DetectorOptionsVO
{
  public:

     DetectorOptionsVO();
    ~DetectorOptionsVO() {};

    int getDetectorBaseNumber() const;
    void setDetectorBaseNumber(int detNum);
   
    int getDetectorInvocationNumber() const;
    void setDetectorInvocationNumber(int detNum);
   
    bool getRunsByDefault() const;
    void setRunsByDefault(bool torf);
    
    bool getLoggingEnabled() const;
    void setLoggingEnabled(bool torf);
 
    bool getTextToMessageLog() const;
    void setTextToMessageLog(bool torf);
 
  private:

    int p_detector_base_number;
    int p_detector_invocation_number;
    bool p_runsByDefault;
    bool p_loggingEnabled;
    bool p_textToMessageLog;
};

#endif
