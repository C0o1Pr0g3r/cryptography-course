#ifndef DEFINITIONS_H_INCLUDED
#define DEFINITIONS_H_INCLUDED

#include <cstdint>
#include <string>

using namespace std;

#define HIGH_TETRAD_MASK 0xf0
#define LOW_TETRAD_MASK 0x0f
#define BITS_PER_BYTE 8
#define BITS_PER_TETRAD 4
#define LOW_BYTE_MASK 0xff
#define MIN_MULTIBYTE_NUMBER 0
#define MAX_MULTIBYTE_NUMBER 0xffffffffffffffff

using Block = uint8_t;
using Number = uint32_t;
using String = string;

#endif // DEFINITIONS_H_INCLUDED
