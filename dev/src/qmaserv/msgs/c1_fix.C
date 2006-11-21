/*
 *
 * File     :
 *   c1_fix.C
 *
 * Purpose  :
 *   Read an interpret the c1_fix message.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *   5 March 2002
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
#include <iostream>
#include <string.h>
#include "QmaTypes.h"
#include "QmaLimits.h"
#include "Field.h"
#include "c1_fix.h"
#include "qmaswap.h"

c1_fix::c1_fix()
{
  p_fieldListPtr = &p_fieldList[0];
  p_number_of_fields = 0;
  Field f1;
  f1.p_start_position = 0;
  f1.p_number_bytes = 4;
  f1.p_data_type = UnsignedInt32;
  strcpy(f1.p_name,"TimeOfLastReboot");
  p_fieldList[p_number_of_fields] = f1;
  ++p_number_of_fields;

  Field f2;
  f2.p_start_position = f1.p_start_position + f1.p_number_bytes;
  f2.p_number_bytes = 4;
  f2.p_data_type = UnsignedInt32;
  strcpy(f2.p_name,"TotalNumberOfReboots");
  p_fieldList[p_number_of_fields] = f2;
  ++p_number_of_fields;

  Field f3;
  f3.p_start_position = f2.p_start_position + f2.p_number_bytes;
  f3.p_number_bytes = 4;
  f3.p_data_type = UnsignedInt32;
  strcpy(f3.p_name,"BackupDataStructureBitmap");
  p_fieldList[p_number_of_fields] = f3;
  ++p_number_of_fields;

  Field f4;
  f4.p_start_position = f3.p_start_position + f3.p_number_bytes;
  f4.p_number_bytes = 4;
  f4.p_data_type = UnsignedInt32;
  strcpy(f4.p_name,"DefaultDataStructureBitmap");
  p_fieldList[p_number_of_fields] = f4;
  ++p_number_of_fields;

  Field f5;
  f5.p_start_position = f4.p_start_position + f4.p_number_bytes;
  f5.p_number_bytes = 100;
  //f5.p_data_type = UnsignedInt32;
  f5.p_data_type = SegmentBuffer;
  strcpy(f5.p_name,"PlaceHolder");
  p_fieldList[p_number_of_fields] = f5;
  ++p_number_of_fields;

  Field f6;
  f6.p_start_position = f5.p_start_position + f5.p_number_bytes;
  f6.p_number_bytes = 1;
  f6.p_data_type = UnsignedInt8;
  strcpy(f6.p_name,"Bit7Freq");
  p_fieldList[p_number_of_fields] = f6;
  ++p_number_of_fields;

  Field f7;
  f7.p_start_position = f6.p_start_position + f6.p_number_bytes;
  f7.p_number_bytes = 1;
  f7.p_data_type = UnsignedInt8;
  strcpy(f7.p_name,"Bit6Freq");
  p_fieldList[p_number_of_fields] = f7;
  ++p_number_of_fields;

  Field f8;
  f8.p_start_position = f7.p_start_position + f7.p_number_bytes;
  f8.p_number_bytes = 1;
  f8.p_data_type = UnsignedInt8;
  strcpy(f8.p_name,"Bit5Freq");
  p_fieldList[p_number_of_fields] = f8;
  ++p_number_of_fields;

  Field f9;
  f9.p_start_position = f8.p_start_position + f8.p_number_bytes;
  f9.p_number_bytes = 1;
  f9.p_data_type = UnsignedInt8;
  strcpy(f9.p_name,"Bit4Freq");
  p_fieldList[p_number_of_fields] = f9;
  ++p_number_of_fields;

  Field f10;
  f10.p_start_position = f9.p_start_position + f9.p_number_bytes;
  f10.p_number_bytes = 1;
  f10.p_data_type = UnsignedInt8;
  strcpy(f10.p_name,"Bit3Freq");
  p_fieldList[p_number_of_fields] = f10;
  ++p_number_of_fields;

  Field f11;
  f11.p_start_position = f10.p_start_position + f10.p_number_bytes;
  f11.p_number_bytes = 1;
  f11.p_data_type = UnsignedInt8;
  strcpy(f11.p_name,"Bit2Freq");
  p_fieldList[p_number_of_fields] = f11;
  ++p_number_of_fields;

  Field f12;
  f12.p_start_position = f11.p_start_position + f11.p_number_bytes;
  f12.p_number_bytes = 1;
  f12.p_data_type = UnsignedInt8;
  strcpy(f12.p_name,"Bit1Freq");
  p_fieldList[p_number_of_fields] = f12;
  ++p_number_of_fields;

  Field f13;
  f13.p_start_position = f12.p_start_position + f12.p_number_bytes;
  f13.p_number_bytes = 1;
  f13.p_data_type = UnsignedInt8;
  strcpy(f13.p_name,"Bit0Freq");
  p_fieldList[p_number_of_fields] = f13;
  ++p_number_of_fields;

  Field f14;
  f14.p_start_position = f13.p_start_position + f13.p_number_bytes;
  f14.p_number_bytes = 4;
  f14.p_data_type = Int32;
  strcpy(f14.p_name,"Channels13Bit7FreqDelay");
  p_fieldList[p_number_of_fields] = f14;
  ++p_number_of_fields;

  Field f15;
  f15.p_start_position = f14.p_start_position + f14.p_number_bytes;
  f15.p_number_bytes = 4;
  f15.p_data_type = Int32;
  strcpy(f15.p_name,"Channels13Bit6FreqDelay");
  p_fieldList[p_number_of_fields] = f15;
  ++p_number_of_fields;

  Field f16;
  f16.p_start_position = f15.p_start_position + f15.p_number_bytes;
  f16.p_number_bytes = 4;
  f16.p_data_type = Int32;
  strcpy(f16.p_name,"Channels13Bit5FreqDelay");
  p_fieldList[p_number_of_fields] = f16;
  ++p_number_of_fields;

  Field f17;
  f17.p_start_position = f16.p_start_position + f16.p_number_bytes;
  f17.p_number_bytes = 4;
  f17.p_data_type = Int32;
  strcpy(f17.p_name,"Channels13Bit4FreqDelay");
  p_fieldList[p_number_of_fields] = f17;
  ++p_number_of_fields;

  Field f18;
  f18.p_start_position = f17.p_start_position + f17.p_number_bytes;
  f18.p_number_bytes = 4;
  f18.p_data_type = Int32;
  strcpy(f18.p_name,"Channels13Bit3FreqDelay");
  p_fieldList[p_number_of_fields] = f18;
  ++p_number_of_fields;

  Field f19;
  f19.p_start_position = f18.p_start_position + f18.p_number_bytes;
  f19.p_number_bytes = 4;
  f19.p_data_type = Int32;
  strcpy(f19.p_name,"Channels13Bit2FreqDelay");
  p_fieldList[p_number_of_fields] = f19;
  ++p_number_of_fields;

  Field f20;
  f20.p_start_position = f19.p_start_position + f19.p_number_bytes;
  f20.p_number_bytes = 4;
  f20.p_data_type = Int32;
  strcpy(f20.p_name,"Channels13Bit1FreqDelay");
  p_fieldList[p_number_of_fields] = f20;
  ++p_number_of_fields;

  Field f21;
  f21.p_start_position = f20.p_start_position + f20.p_number_bytes;
  f21.p_number_bytes = 4;
  f21.p_data_type = Int32;
  strcpy(f21.p_name,"Channels13Bit0FreqDelay");
  p_fieldList[p_number_of_fields] = f21;
  ++p_number_of_fields;

  Field f22;
  f22.p_start_position = f21.p_start_position + f21.p_number_bytes;
  f22.p_number_bytes = 4;
  f22.p_data_type = Int32;
  strcpy(f22.p_name,"Channels46Bit7FreqDelay");
  p_fieldList[p_number_of_fields] = f22;
  ++p_number_of_fields;

  Field f23;
  f23.p_start_position = f22.p_start_position + f22.p_number_bytes;
  f23.p_number_bytes = 4;
  f23.p_data_type = Int32;
  strcpy(f23.p_name,"Channels46Bit6FreqDelay");
  p_fieldList[p_number_of_fields] = f23;
  ++p_number_of_fields;

  Field f24;
  f24.p_start_position = f23.p_start_position + f23.p_number_bytes;
  f24.p_number_bytes = 4;
  f24.p_data_type = Int32;
  strcpy(f24.p_name,"Channels46Bit5FreqDelay");
  p_fieldList[p_number_of_fields] = f24;
  ++p_number_of_fields;

  Field f25;
  f25.p_start_position = f24.p_start_position + f24.p_number_bytes;
  f25.p_number_bytes = 4;
  f25.p_data_type = Int32;
  strcpy(f25.p_name,"Channels46Bit4FreqDelay");
  p_fieldList[p_number_of_fields] = f25;
  ++p_number_of_fields;

  Field f26;
  f26.p_start_position = f25.p_start_position + f25.p_number_bytes;
  f26.p_number_bytes = 4;
  f26.p_data_type = Int32;
  strcpy(f26.p_name,"Channels46Bit3FreqDelay");
  p_fieldList[p_number_of_fields] = f26;
  ++p_number_of_fields;

  Field f27;
  f27.p_start_position = f26.p_start_position + f26.p_number_bytes;
  f27.p_number_bytes = 4;
  f27.p_data_type = Int32;
  strcpy(f27.p_name,"Channels46Bit2FreqDelay");
  p_fieldList[p_number_of_fields] = f27;
  ++p_number_of_fields;

  Field f28;
  f28.p_start_position = f27.p_start_position + f27.p_number_bytes;
  f28.p_number_bytes = 4;
  f28.p_data_type = Int32;
  strcpy(f28.p_name,"Channels46Bit1FreqDelay");
  p_fieldList[p_number_of_fields] = f28;
  ++p_number_of_fields;

  Field f29;
  f29.p_start_position = f28.p_start_position + f28.p_number_bytes;
  f29.p_number_bytes = 4;
  f29.p_data_type = Int32;
  strcpy(f29.p_name,"Channels46Bit0FreqDelay");
  p_fieldList[p_number_of_fields] = f29;

  p_length_in_bytes =
    p_fieldList[p_number_of_fields].p_start_position + 
    p_fieldList[p_number_of_fields].p_number_bytes;

  memset((char*)&p_bits[0],0,p_length_in_bytes);

  if(!true)
  {
    std::cout << "Fields in c1_fix : " << (p_number_of_fields +1) << std::endl;
    std::cout << "Creating c1_fix with length : " << 
      p_length_in_bytes << std::endl;
    std::cout << "Before : " << p_fieldList[p_number_of_fields].p_start_position 
	      << std::endl;
  }
  p_number_of_fields++;

}

//
// Operations on fields
//
qma_uint32  c1_fix::getDataPort1PacketMemorySize()
{
  qma_uint32 val;
  int offsetToData = 100;
  memcpy((char*)&val,
         (char*)&p_bits[offsetToData],4);
  return qma_ntohl(val);
}

qma_uint32  c1_fix::getDataPort2PacketMemorySize()
{
  qma_uint32 val;
  int offsetToData = 104;
  memcpy((char*)&val,
         (char*)&p_bits[offsetToData],4);
  return qma_ntohl(val);
}

qma_uint32  c1_fix::getDataPort3PacketMemorySize()
{
  qma_uint32 val;
  int offsetToData = 108;
  memcpy((char*)&val,
         (char*)&p_bits[offsetToData],4);
  return qma_ntohl(val);
}

qma_uint32  c1_fix::getDataPort4PacketMemorySize()
{
  qma_uint32 val;
  int offsetToData = 112;
  memcpy((char*)&val,
         (char*)&p_bits[offsetToData],4);
  return qma_ntohl(val);
}


qma_uint16  c1_fix::getSystemVersion()
{
  qma_uint16 val;
  int offsetToSystemVersion = 28;
  memcpy((char*)&val,
         (char*)&p_bits[offsetToSystemVersion],2);
  return qma_ntohs(val);
}

qma_uint32  c1_fix::getTimeOfLastReboot()
{
  qma_uint32 val;
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[0].p_start_position],
         (unsigned int)p_fieldList[0].p_number_bytes);
  return val;
}


qma_uint32  c1_fix::getTotalNumberOfReboots()
{
  qma_uint32 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[1].p_start_position],
         (unsigned int)p_fieldList[1].p_number_bytes);
  return val;
}


qma_uint32  c1_fix::getBackupDataStructureBitmap()
{
  qma_uint32 val;  
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[2].p_start_position],
         (unsigned int)p_fieldList[2].p_number_bytes);
  return val;
}

qma_uint32  c1_fix::getDefaultDataStructureBitmap()
{
  qma_uint32 val;
  memcpy((char*)&val,
         (char*)&p_bits[p_fieldList[3].p_start_position],
         (unsigned int)p_fieldList[3].p_number_bytes);
  return val;
}

//
//
// 

qma_uint8 c1_fix::getBit7Freq()
{
  qma_uint8 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[5].p_start_position],
	 (unsigned int)p_fieldList[5].p_number_bytes);

  return val;
}

qma_uint8 c1_fix::getBit6Freq()
{
  qma_uint8 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[6].p_start_position],
	 (unsigned int)p_fieldList[6].p_number_bytes);

  return val;
}

qma_uint8 c1_fix::getBit5Freq()
{
  qma_uint8 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[7].p_start_position],
	 (unsigned int)p_fieldList[7].p_number_bytes);

  return val;
}

qma_uint8 c1_fix::getBit4Freq()
{
  qma_uint8 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[8].p_start_position],
	 (unsigned int)p_fieldList[8].p_number_bytes);

  return val;
}

qma_uint8 c1_fix::getBit3Freq()
{

  qma_uint8 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[9].p_start_position],
	 (unsigned int)p_fieldList[9].p_number_bytes);
  return val;
}

qma_uint8 c1_fix::getBit2Freq()
{
  qma_uint8 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[10].p_start_position],
	 (unsigned int)p_fieldList[10].p_number_bytes);

  return val;
}

qma_uint8 c1_fix::getBit1Freq()
{
  qma_uint8 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[11].p_start_position],
	 (unsigned int)p_fieldList[11].p_number_bytes);

  return val;
}

qma_uint8 c1_fix::getBit0Freq()
{
  qma_uint8 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[12].p_start_position],
	 (unsigned int)p_fieldList[12].p_number_bytes);

  return val;
}


qma_uint8 c1_fix::getBitFreq(qma_uint8 bitnum)
{
  qma_uint16 fval = bitnum;
  switch(bitnum)
  {
     case 0:
     {
       return getBit0Freq();
       break;
     }
     case 1:
     {
       return getBit1Freq();
       break;
     }
     case 2:
     {
       return getBit2Freq();
       break;
     }
     case 3:
     {
       return getBit3Freq();
       break;
     }
     case 4:
     {
       return getBit4Freq();
       break;
     }
     case 5:
     {
       return getBit5Freq();
       break;
     }
     case 6:
     {
       return getBit6Freq();
       break;
     }
     case 7:
     {
       return getBit7Freq();
       break;
     }
     default:
     {
      std::cout << "xxx Error converting bit number to frequence : " <<
		bitnum << std::endl;
      return 0;
     }
  }
}

double c1_fix::frequencyBit2Double(qma_uint8 fbit)
{
  if(fbit == 255)
  {
    return 0.1;
  }
 
  if((fbit & 0x80) != 0)
  {
    qma_uint8 temp = (fbit & 0x0f);
    if(temp == 4)
    {
      return 200.0;
    }
    if(temp == 5)
    {
      return 250.0;
    }
    if(temp == 6)
    {
      return 300.0;
    }
    if(temp == 7)
    {
      return 400.0;
    }
    if(temp == 8)
    {
      return 500.0;
    }
    if(temp == 9)
    {
      return 800.0;
    }
    if(temp == 10)
    {
      return 1000.0;
    }
  }

  double retVal = fbit;
  return retVal; 
}

qma_uint32 c1_fix::frequencyBit2Integer(qma_uint8 fbit)
{
  double val = frequencyBit2Double(fbit); 
  return ((qma_uint32) val);
}

qma_int32 c1_fix::getChannels13Bit7FreqDelay()
{
  qma_int32 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[13].p_start_position],
	 (unsigned int)p_fieldList[13].p_number_bytes);

  return val;

}

qma_int32 c1_fix::getChannels13Bit6FreqDelay()
{
  qma_int32 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[14].p_start_position],
	 (unsigned int)p_fieldList[14].p_number_bytes);
  return val;
}

qma_int32 c1_fix::getChannels13Bit5FreqDelay()
{
  qma_int32 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[15].p_start_position],
	 (unsigned int)p_fieldList[15].p_number_bytes);
  return val;
}

qma_int32 c1_fix::getChannels13Bit4FreqDelay()
{
  qma_int32 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[16].p_start_position],
	 (unsigned int)p_fieldList[16].p_number_bytes);
  return val;
}

qma_int32 c1_fix::getChannels13Bit3FreqDelay()
{
  qma_int32 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[17].p_start_position],
	 (unsigned int)p_fieldList[17].p_number_bytes);
  return val;
}

qma_int32 c1_fix::getChannels13Bit2FreqDelay()
{
  qma_int32 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[18].p_start_position],
	 (unsigned int)p_fieldList[18].p_number_bytes);
  return val;
}


qma_int32 c1_fix::getChannels13Bit1FreqDelay()
{
  qma_int32 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[19].p_start_position],
	 (unsigned int)p_fieldList[19].p_number_bytes);
  return val;
}

qma_int32 c1_fix::getChannels13Bit0FreqDelay()
{
  qma_int32 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[20].p_start_position],
	 (unsigned int)p_fieldList[20].p_number_bytes);
  return val;
}


qma_int32 c1_fix::getChannels46Bit7FreqDelay()
{
  qma_int32 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[21].p_start_position],
	 (unsigned int)p_fieldList[21].p_number_bytes);

  return val;

}

qma_int32 c1_fix::getChannels46Bit6FreqDelay()
{
  qma_int32 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[22].p_start_position],
	 (unsigned int)p_fieldList[22].p_number_bytes);
  return val;
}

qma_int32 c1_fix::getChannels46Bit5FreqDelay()
{
  qma_int32 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[23].p_start_position],
	 (unsigned int)p_fieldList[23].p_number_bytes);
  return val;
}

qma_int32 c1_fix::getChannels46Bit4FreqDelay()
{
  qma_int32 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[24].p_start_position],
	 (unsigned int)p_fieldList[24].p_number_bytes);
  return val;
}

qma_int32 c1_fix::getChannels46Bit3FreqDelay()
{
  qma_int32 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[25].p_start_position],
	 (unsigned int)p_fieldList[25].p_number_bytes);
  return val;
}

qma_int32 c1_fix::getChannels46Bit2FreqDelay()
{
  qma_int32 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[26].p_start_position],
	 (unsigned int)p_fieldList[26].p_number_bytes);
  return val;
}


qma_int32 c1_fix::getChannels46Bit1FreqDelay()
{
  qma_int32 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[27].p_start_position],
	 (unsigned int)p_fieldList[27].p_number_bytes);
  return val;
}

qma_int32 c1_fix::getChannels46Bit0FreqDelay()
{
  qma_int32 val;
  memcpy((char*)&val,
	 (char*)&p_bits[p_fieldList[28].p_start_position],
	 (unsigned int)p_fieldList[28].p_number_bytes);
  return val;
}
