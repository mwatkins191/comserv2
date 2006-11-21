/*
 *
 * File     :
 *   TimeOfDay.h
 *
 * Purpose  :
 *   This provides utilities on the unix time of day functions.
 *
 * Author   :
 *   Phil Maechling
 *
 *
 * Mod Date :
 *  15 March 2002
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
#ifndef TIMEOFDAY_H
#define TIMEOFDAY_H

class TimeOfDay
{
  public:
    TimeOfDay() {};
    ~TimeOfDay() {};

    double getCurrentTime();
    double getTimeDifference(double earlierTime, double laterTime);
};
#endif
