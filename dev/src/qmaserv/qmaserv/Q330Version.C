/*
 * File     :
 *   Q330Version.C
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

#include "QmaTypes.h"
#include "Q330Version.h"

Q330Version::Q330Version()
{
  p_version = 0;
}

int  Q330Version::getVersionRelease() const
{
  int val = (int) p_version;
  return (val/256);
}

int  Q330Version::getVersionFeature() const
{
  int val = (int) p_version;
  return (int) (val%256);
}

void Q330Version::setVersion(const qma_uint16 ver)
{
  p_version = ver;
}
