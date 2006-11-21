/*
 *
 * File     :
 *  Field.h
 *
 *
 * Purpose  :
 *   This defines a field, typically a bit field in a q330 message.
 *   A message will consist of multiple fields.
 *
 * Author   :
 *  Phil Maechling
 *
 *
 * Mod Date :
 *
 * 14 Feb 2002
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
#ifndef FIELD_H
#define FIELD_H

#include "QmaTypes.h"
#include "QmaLimits.h"

class Field
{
  public:
	
    Field();
    ~Field();

    qma_uint32     p_start_position; 
    qma_uint32     p_number_bytes;
    DataTypes      p_data_type;
    char           p_name[EIGHTY_CHARS_STRING];
};
/*
std::ostream& operator<<(std::ostream& os, const Field& dt)
{
  os << " Name: " << dt.p_name << " - DataType: " << dt.p_data_type <<
        " - Length: " <<
        dt.p_number_bytes << " - StartPosition: " << dt.p_start_position 
	<< std::endl;
  return os;
}

*/
#endif
