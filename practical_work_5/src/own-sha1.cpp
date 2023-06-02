#include <stdexcept>
#include "own-sha1.h"

using LogicalFunction = Word (const Word&, const Word&, const Word&);

const size_t NUMBER_OF_RANGES = 4;
const array<Word, NUMBER_OF_RANGES> K {{
    0x5a827999,
    0x6ed9eba1,
    0x8f1bbcdc,
    0xca62c1d6
}};
const array<LogicalFunction*, NUMBER_OF_RANGES> F {{
    [](const Word& b, const Word& c, const Word& d) {
        return b & c | ~b & d;
    },
    [](const Word& b, const Word& c, const Word& d) {
        return b ^ c ^ d;
    },
    [](const Word& b, const Word& c, const Word& d) {
        return b & c | b & d | c & d;
    },
    [](const Word& b, const Word& c, const Word& d) {
        return b ^ c ^ d;
    }
}};

Word circularBitShiftLeft(const Word& word, size_t size) {
    return word << size | word >> WORD_LENGTH * BITS_PER_BYTE - size;
}

OwnSHA1::OwnSHA1() {
    this->reset();
}

void OwnSHA1::processBlock() {
    Word temp;
    Word w[BLOCK_LENGTH + BLOCK_LENGTH / WORD_LENGTH];
    array<Word, HASH_LENGTH / WORD_LENGTH> abcde;
    Word& a = abcde[0];
    Word& b = abcde[1];
    Word& c = abcde[2];
    Word& d = abcde[3];
    Word& e = abcde[4];

    for(size_t t = 0; t < BLOCK_LENGTH / WORD_LENGTH; ++t) {
        w[t] = 0;
        for (size_t i = 0; i < WORD_LENGTH; ++i) {
            w[t] |= this->context.messageBlock[t * WORD_LENGTH + i]
                << WORD_LENGTH * BITS_PER_BYTE - BITS_PER_BYTE * (i + 1);
        }
    }

    for(
        size_t t = BLOCK_LENGTH / WORD_LENGTH;
        t < BLOCK_LENGTH + BLOCK_LENGTH / WORD_LENGTH;
        ++t
    ) {
       w[t] = circularBitShiftLeft(w[t-3] ^ w[t-8] ^ w[t-14] ^ w[t-16], 1);
    }

    for (size_t i = 0; i < abcde.size(); ++i) {
        abcde[i] = this->context.intermediateHash[i];
    }

    for(size_t t = 0; t < BLOCK_LENGTH + BLOCK_LENGTH / WORD_LENGTH; ++t) {
        const size_t rangeIndex = t / (
            (BLOCK_LENGTH + BLOCK_LENGTH / WORD_LENGTH) / NUMBER_OF_RANGES
        );
        temp = circularBitShiftLeft(a, 5) + F[rangeIndex](b, c, d)
            + e + w[t] + K[rangeIndex];
        e = d;
        d = c;
        c = circularBitShiftLeft(b, 30);
        b = a;
        a = temp;
    }

    for (size_t i = 0; i < abcde.size(); ++i) {
        this->context.intermediateHash[i] += abcde[i];
    }

    this->context.messageBlockIndex = 0;
}

void OwnSHA1::padMessage() {
    const size_t BLOCK_LENGTH_WITHOUT_FIELD_FOR_MESSAGE_LENGTH =
        BLOCK_LENGTH - sizeof(OwnSHA1::Context::length);

    if (this->context.messageBlockIndex
        >= BLOCK_LENGTH_WITHOUT_FIELD_FOR_MESSAGE_LENGTH
    ) {
        this->context.messageBlock[this->context.messageBlockIndex++] =
            FIRST_PADDING_BYTE;
        while(this->context.messageBlockIndex < BLOCK_LENGTH) {
            this->context.messageBlock[this->context.messageBlockIndex++] =
                REMAINING_PADDING_BYTES;
        }

        this->processBlock();

        while(this->context.messageBlockIndex
            < BLOCK_LENGTH_WITHOUT_FIELD_FOR_MESSAGE_LENGTH
        ) {
            this->context.messageBlock[this->context.messageBlockIndex++] =
                REMAINING_PADDING_BYTES;
        }
    } else {
        this->context.messageBlock[this->context.messageBlockIndex++] =
            FIRST_PADDING_BYTE;
        while(this->context.messageBlockIndex
            < BLOCK_LENGTH_WITHOUT_FIELD_FOR_MESSAGE_LENGTH
        ) {
            this->context.messageBlock[this->context.messageBlockIndex++] =
                REMAINING_PADDING_BYTES;
        }
    }

    for (size_t i = 0; i < sizeof(OwnSHA1::Context::length); ++i) {
        this->context.messageBlock[
            i + BLOCK_LENGTH_WITHOUT_FIELD_FOR_MESSAGE_LENGTH
        ] = this->context.length
            >> BLOCK_LENGTH_WITHOUT_FIELD_FOR_MESSAGE_LENGTH
            - i * BITS_PER_BYTE;
    }

    this->processBlock();
}

void OwnSHA1::reset() {
    this->context.length = 0;
    this->context.messageBlockIndex = 0;

    this->context.intermediateHash[0] = 0x67452301;
    this->context.intermediateHash[1] = 0xefcdab89;
    this->context.intermediateHash[2] = 0x98badcfe;
    this->context.intermediateHash[3] = 0x10325476;
    this->context.intermediateHash[4] = 0xc3d2e1f0;

    this->context.computed = false;
    this->context.corrupted = false;
}

void OwnSHA1::update(const RawMessage& message) {
    if (!message.length) {
        throw logic_error("Message length cannot be zero");
    }

    if (!message.data) {
        throw logic_error("Message data is invalid");
    }

    if (this->context.computed) {
        this->context.corrupted = true;
        throw logic_error("Context already contains the computed hash");
    }

    if (this->context.corrupted) {
        throw logic_error("Context corrupted");
    }

    for (size_t i = 0; i < message.length && !this->context.corrupted; ++i) {
        this->context.messageBlock[this->context.messageBlockIndex++] =
            (message.data[i] & LOW_BYTE_MASK);
        this->context.length += BITS_PER_BYTE;
        if (!this->context.length) {
            this->context.corrupted = true;
        }
        if (this->context.messageBlockIndex == BLOCK_LENGTH) {
            this->processBlock();
        }
    }
}

void OwnSHA1::update(const string& message) {
    this->update(RawMessage(
        reinterpret_cast<const Byte*>(message.c_str()), message.length()
    ));
}

Hash OwnSHA1::digest() {
    Hash hash;

    if (this->context.corrupted) {
        throw logic_error("Context corrupted");
    }

    if (!this->context.computed) {
        this->padMessage();
        for(size_t i = 0; i < BLOCK_LENGTH; ++i) {
            this->context.messageBlock[i] = 0;
        }
        this->context.length = 0;
        this->context.computed = true;
    }

    for(size_t i = 0; i < HASH_LENGTH / WORD_LENGTH; ++i) {
        for (size_t j = 0; j < WORD_LENGTH; ++j) {
            const size_t a = this->context.intermediateHash[i]
                >> BITS_PER_BYTE * (WORD_LENGTH - j - 1) & LOW_BYTE_MASK;
            hash[i * WORD_LENGTH + j] = a;
        }
    }

    return hash;
}
