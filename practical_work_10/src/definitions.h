#ifndef DEFINITIONS_H_INCLUDED
#define DEFINITIONS_H_INCLUDED

#include <cstdint>
#include <string>
#include <vector>

using namespace std;

using Byte = uint8_t;
const size_t BITS_PER_BYTE = 8;
const Byte LOW_BYTE_MASK = 0xff;
using OctetString = vector<Byte>;
const size_t HEX_DIGITS_PER_BYTE = 2;
const size_t HASH_LENGTH = 160 / BITS_PER_BYTE;
using Hash = array<Byte, HASH_LENGTH>;

const string OPERATION_FAILED = "Operation failed";

#endif // DEFINITIONS_H_INCLUDED
