/*
 * File     :
 *   Q330Version.h
 *
 * Purpose  :
 *   Storage object for q330 version information.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  20 October 2002
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
#ifndef Q330VERSION_H
#define Q330VERSION_H

#include "QmaTypes.h"

class Q330Version
{
  public:
   Q330Version ();
   ~Q330Version() {};

   int  getVersionRelease() const;
   int  getVersionFeature() const;
   void setVersion(const qma_uint16 ver);

  private:

   qma_uint16 p_version;
};

#endif
