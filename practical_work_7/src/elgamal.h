#ifndef ELGAMAL_H_INCLUDED
#define ELGAMAL_H_INCLUDED

#include <array>
#include <vector>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include "big-int.h"

using Byte = uint8_t;
using Data = vector<Byte>;
using CipherText = vector<BigInt>;
const size_t BITS_PER_BYTE = 8;
const size_t HEX_DIGITS_PER_BYTE = 2;
const size_t HASH_LENGTH = 160 / BITS_PER_BYTE;
using Hash = array<Byte, HASH_LENGTH>;
const size_t CIPHER_UNIT_PER_PLAINTEXT_BLOCK = 2;

const BigInt P = BigInt::generatePrime(2048);
const BigInt G = 5;

using PrivateKey = BigInt;

ostream& operator<<(ostream& out, const CipherText& cipherText) {
    for (int i = 0; i < cipherText.size(); ++i) {
        cout << cipherText[i];
        if (i < cipherText.size() - 1) {
            cout << ", ";
        }
    }
    return out;
}

class PublicKey {
private:
    friend ostream& operator<<(ostream& out, const PublicKey& publicKey);

private:
    BigInt p;
    BigInt g;
    BigInt y;

public:
    PublicKey(const BigInt& p, const BigInt& g, const PrivateKey& x)
    :   p(p),
        g(g),
        y(BigInt::powMod(g, x, p))
    {}

    BigInt getP() const {
        return this->p;
    }

    BigInt getG() const {
        return this->g;
    }

    BigInt getY() const {
        return this->y;
    }
};

ostream& operator<<(ostream& out, const PublicKey& publicKey) {
    out << "(" << publicKey.p << ", " << publicKey.g << ", " << publicKey.y << ")";
    return out;
}

class KeyPair {
private:
    friend ostream& operator<<(ostream& out, const KeyPair& keyPair);

private:
    PrivateKey privateKey;
    PublicKey publicKey;

public:
    KeyPair(const PrivateKey& privateKey, const PublicKey& publicKey)
    :   privateKey(privateKey), publicKey(publicKey)
    {}

    PrivateKey getPrivateKey() const {
        return this->privateKey;
    }

    PublicKey getPublicKey() const {
        return this->publicKey;
    }
};

ostream& operator<<(ostream& out, const KeyPair& keyPair) {
    out << "(" << keyPair.privateKey << ", " << keyPair.publicKey << ")";
    return out;
}

class Signature {
private:
    friend ostream& operator<<(ostream& out, const Signature& signature);

private:
    BigInt r;
    BigInt s;

public:
    Signature(const BigInt& r, const BigInt& s) : r(r), s(s) {}

    BigInt getR() const {
        return this->r;
    }

    BigInt& getR() {
        return this->r;
    }

    BigInt getS() const {
        return this->s;
    }

    BigInt& getS() {
        return this->s;
    }
};

ostream& operator<<(ostream& out, const Signature& signature) {
    out << "(" << signature.r << ", " << signature.s << ")";
    return out;
}

template<class T>
class SignedMessage {
private:
    T message;
    Signature signature;

public:
    SignedMessage(const T& message, const Signature& signature)
    :   message(message),
        signature(signature)
    {}

    T getMessage() const {
        return this->message;
    }

    T& getMessage() {
        return this->message;
    }

    Signature getSignature() const {
        return this->signature;
    }

    Signature& getSignature() {
        return this->signature;
    }
};

class Elgamal {
private:
    static Hash computeHash(const Data& data) {
        Hash hash;
        SHA1(
            reinterpret_cast<const unsigned char*>(data.data()),
            data.size(),
            hash.data()
        );
        return hash;
    }

    static string convertHashToHex(const Hash& hash) {
        std::stringstream ss;
        for (int i = 0; i < hash.size(); ++i) {
            ss << setfill('0') << setw(HEX_DIGITS_PER_BYTE) << hex << static_cast<int>(hash[i]);
        }
        return ss.str();
    }

    static BigInt getHashAsBigInt(const Data& data) {
        return BigInt(convertHashToHex(computeHash(data)), Radix::HEX);
    }

public:
    static KeyPair generateKeyPair() {
        const PrivateKey privateKey(BigInt::generateRandomInInterval(1, P - 1));
        const PublicKey publicKey(P, G, privateKey);
        return KeyPair(privateKey, publicKey);
    }

    static SignedMessage<Data> sign(const Data& message, const KeyPair& keyPair) {
        const BigInt p = keyPair.getPublicKey().getP();
        const BigInt g = keyPair.getPublicKey().getG();
        const BigInt t = keyPair.getPublicKey().getY();
        const BigInt x = keyPair.getPrivateKey();

        const BigInt m = getHashAsBigInt(message);

        BigInt k(0);
        while (BigInt::computeGreatestCommonDivisor(k, p - 1) != 1) {
            k = BigInt::generateRandomInInterval(1, p - 1);
        }

        const BigInt r = BigInt::powMod(g, k, p);
        const BigInt inverseModuloForK = BigInt::computeInverseModulo(k, p - 1);
        const BigInt s = BigInt::mulMod((m - x * r), inverseModuloForK, p - 1);

        return SignedMessage(message, Signature(r, s));
    }

    static SignedMessage<string> sign(const string& message, const KeyPair& keyPair) {
        const Data data(message.begin(), message.end());
        const SignedMessage signedMessage = sign(data, keyPair);
        return SignedMessage(message, signedMessage.getSignature());
    }

    static bool verify(const SignedMessage<Data>& signedMessage, const PublicKey& publicKey) {
        const BigInt p = publicKey.getP();
        const BigInt g = publicKey.getG();
        const BigInt y = publicKey.getY();
        const BigInt r = signedMessage.getSignature().getR();
        const BigInt s = signedMessage.getSignature().getS();

        const BigInt m = getHashAsBigInt(signedMessage.getMessage());

        return
            r > 0 && r < p && s > 0 && s < p - 1
            &&
            BigInt::mulMod(BigInt::powMod(y, r, p), BigInt::powMod(r, s, p), p) == BigInt::powMod(g, m, p);
    }

    static bool verify(const SignedMessage<string>& signedMessage, const PublicKey& publicKey) {
        Data data(signedMessage.getMessage().length());
        for (int i = 0; i < signedMessage.getMessage().length(); ++i) {
            data[i] = signedMessage.getMessage()[i];
        }
        return verify(SignedMessage(data, signedMessage.getSignature()), publicKey);
    }

    static CipherText encrypt(const Data& message, const PublicKey& publicKey) {
        const BigInt p = publicKey.getP();
        const BigInt g = publicKey.getG();
        const BigInt y = publicKey.getY();

        CipherText cipherText(message.size() * CIPHER_UNIT_PER_PLAINTEXT_BLOCK);

        for (int i = 0; i < message.size(); ++i) {
            BigInt k(0);
            while (BigInt::computeGreatestCommonDivisor(k, p - 1) != 1) {
                k = BigInt::generateRandomInInterval(1, p - 1);
            }

            const BigInt m = message[i];
            const BigInt a = BigInt::powMod(g, k, p);
            const BigInt b = BigInt::mulMod(BigInt::powMod(y, k, p), m, p);

            cipherText[i * CIPHER_UNIT_PER_PLAINTEXT_BLOCK] = a;
            cipherText[i * CIPHER_UNIT_PER_PLAINTEXT_BLOCK + 1] = b;
        }

        return cipherText;
    }

    static CipherText encrypt(const string& message, const PublicKey& publicKey) {
        return encrypt(Data(message.begin(), message.end()), publicKey);
    }

    static Data decrypt(const CipherText& cipherText, const KeyPair& keyPair) {
        const BigInt p = keyPair.getPublicKey().getP();
        const BigInt g = keyPair.getPublicKey().getG();
        const BigInt y = keyPair.getPublicKey().getY();
        const BigInt x = keyPair.getPrivateKey();

        Data decryptedMessage(cipherText.size() / CIPHER_UNIT_PER_PLAINTEXT_BLOCK);

        for (int i = 0; i < decryptedMessage.size(); ++i) {
            const BigInt a = cipherText[i * CIPHER_UNIT_PER_PLAINTEXT_BLOCK];
            const BigInt b = cipherText[i * CIPHER_UNIT_PER_PLAINTEXT_BLOCK + 1];
            const BigInt m = BigInt::mulMod(b, BigInt::powMod(a, p - 1 - x, p), p);
            decryptedMessage[i] = BigInt::mask(m, BITS_PER_BYTE);
        }

        return decryptedMessage;
    }
};

#endif // ELGAMAL_H_INCLUDED
