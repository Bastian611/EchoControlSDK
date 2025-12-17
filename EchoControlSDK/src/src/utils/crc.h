
#pragma once
#include "crc.hpp"

ECCS_BEGIN


typedef CRC<u8,  0x07>       CRC8_07;
typedef CRC<u8,  0x31>       CRC8_31;
typedef CRC<u16, 0x8005>     CRC16_8005;
typedef CRC<u16, 0x1021>     CRC16_1021;
typedef CRC<u16, 0x3D65>     CRC16_3D65;
typedef CRC<u32, 0x04C11DB7> CRC32_04C11DB7;


ECCS_END
