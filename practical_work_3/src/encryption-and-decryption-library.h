#ifndef ENCRYPTION_AND_DECRYPTION_LIBRARY_H_INCLUDED
#define ENCRYPTION_AND_DECRYPTION_LIBRARY_H_INCLUDED

#include <functional>
#include "definitions.h"
#include "substitution-box.h"
#include "permutation-box.h"

using namespace std;

using BlockEncryptionOrDecryptionFunction = function<Block (const Block&)>;
template<class T>
using EncryptionOrDecryptionFunction = function<T (const T&)>;

Block getByteByIndex(const Number& number, const size_t index) {
    return number >> (BITS_PER_BYTE * index) & LOW_BYTE_MASK;
}

Number setByteByIndex(const Number& number, const Block& value, const size_t index) {
    const Number MASK = ~(LOW_BYTE_MASK << BITS_PER_BYTE * index);
    
    return number & MASK | (value & LOW_BYTE_MASK)
        << BITS_PER_BYTE * index;
}

Number convertNumber(
    const Number& number, const BlockEncryptionOrDecryptionFunction& convertBlock
) {
    Number convertedNumber = 0;

    for (size_t i = 0; i < sizeof(number); ++i) {
        convertedNumber = setByteByIndex(
            convertedNumber, convertBlock(getByteByIndex(number, i)), i
        );
    }

    return convertedNumber;
}

String convertString(
    const String& string, const BlockEncryptionOrDecryptionFunction& convertBlock
) {
    String convertedString;
    convertedString.resize(string.length());

    for (size_t i = 0; i < string.length(); ++i) {
        convertedString[i] = convertBlock(string[i]);
    }

    return convertedString; 
}

class SubstitutionOrPermutationBox {
private:
    BlockEncryptionOrDecryptionFunction encryptBlock;
    BlockEncryptionOrDecryptionFunction decryptBlock;

public:
    SubstitutionOrPermutationBox(
        const BlockEncryptionOrDecryptionFunction& encryptBlock,
        const BlockEncryptionOrDecryptionFunction& decryptBlock
    ) :
        encryptBlock(encryptBlock),
        decryptBlock(decryptBlock)
    {}

    Number encrypt(const Number& number) const {
        return convertNumber(number, this->encryptBlock);
    }

    String encrypt(const String& string) const {
        return convertString(string, this->encryptBlock);
    }

    Number decrypt(const Number& number) const {
        return convertNumber(number, this->decryptBlock);
    }

    String decrypt(const String& string) const {
        return convertString(string, this->decryptBlock);
    }
};

#endif // ENCRYPTION_AND_DECRYPTION_LIBRARY_H_INCLUDED
