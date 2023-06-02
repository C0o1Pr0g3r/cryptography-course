#ifndef SHA1_INCLUDED
#define SHA1_INCLUDED

#include <cstdint>
#include <cstddef>
#include <array>

using namespace std;

using Byte = uint8_t;

const size_t BITS_PER_BYTE = 8;
const size_t LOW_BYTE_MASK = 0xff;
const size_t HASH_LENGTH = 160 / BITS_PER_BYTE;
const size_t BLOCK_LENGTH = 512 / BITS_PER_BYTE;
const size_t WORD_LENGTH = 32 / BITS_PER_BYTE;
const Byte FIRST_PADDING_BYTE = 0x80;
const Byte REMAINING_PADDING_BYTES = 0;

using Word = uint32_t;
using Hash = array<Byte, HASH_LENGTH>;
using Block = Byte[BLOCK_LENGTH];

class OwnSHA1 {
private:
    struct Context {
        Word intermediateHash[HASH_LENGTH / WORD_LENGTH];
        size_t length;
        size_t messageBlockIndex;
        Block messageBlock;
        bool computed;
        bool corrupted;
    };

    Context context;

    void processBlock();
    void padMessage();

public:
    struct RawMessage {
        const Byte* data;
        size_t length;

        RawMessage(const Byte* data, size_t length) :
            data(data),
            length(length)
        {}
    };

    OwnSHA1();
    void reset();
    void update(const RawMessage& message);
    void update(const string& message);
    Hash result();
};

#endif // SHA1_INCLUDED
