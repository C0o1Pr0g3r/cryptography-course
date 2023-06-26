#ifndef DEFINITIONS_H_INCLUDED
#define DEFINITIONS_H_INCLUDED

#include <cstdint>
#include <cstddef>

using namespace std;

using Byte = uint8_t;
const size_t BITS_PER_BYTE = 8;
const Byte LOW_BYTE_MASK = 0xff;

const string OPERATION_FAILED = "Operation failed";

#endif // DEFINITIONS_H_INCLUDED
