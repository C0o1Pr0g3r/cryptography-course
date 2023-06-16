#ifndef ELGAMAL_H_INCLUDED
#define ELGAMAL_H_INCLUDED

#include <array>
#include <vector>
#include <openssl/sha.h>
#include "big-int.h"

using namespace std;

using Byte = uint8_t;
using Data = vector<Byte>;
const size_t BITS_PER_BYTE = 8;
const size_t HASH_LENGTH = 160 / BITS_PER_BYTE;
using Hash = array<Byte, HASH_LENGTH>;

const BigInt P(277);
const BigInt G(5);

using PrivateKey = BigInt;

class PublicKey {
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

class KeyPair {
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

class Signature {
private:
    BigInt r;
    BigInt s;

public:
    Signature(const BigInt& r, const BigInt& s) : r(r), s(s) {}

    BigInt getR() const {
        return this->r;
    }

    BigInt getS() const {
        return this->s;
    }
};

template<class T>
class SignedData {
private:
    T message;
    Signature signature;

public:
    SignedData(const T& message, const Signature& signature)
    :   message(message),
        signature(signature)
    {}

    T& getMessage() {
        return this->message;
    }

    T getMessage() const {
        return this->message;
    }

    Signature getSignature() const {
        return this->signature;
    }
};

class Elgamal {
private:
    static Hash calculateHash(const Data& data) {
        Hash hash;
        SHA1(
            reinterpret_cast<const unsigned char*>(data.data()),
            data.size(),
            hash.data()
        );

        return hash;
    }

public:
    static KeyPair generateKeyPair() {
        const PrivateKey privateKey(BigInt::randomInInterval(1, P - 1));
        const PublicKey publicKey(P, G, privateKey);
        return KeyPair(privateKey, publicKey);
    }

    static SignedData<Data> sign(const Data& data, const KeyPair& keyPair) {
        const BigInt p = keyPair.getPublicKey().getP();
        const BigInt g = keyPair.getPublicKey().getG();
        const BigInt y = keyPair.getPublicKey().getY();
        const BigInt x = keyPair.getPrivateKey();

        Hash digest = Elgamal::calculateHash(data);
        const BigInt m(digest.at(digest.size() - 1));

        BigInt k;
        BigInt multiplicativeInverseK(0);
        while (multiplicativeInverseK == BigInt(0)) {
            k = BigInt::randomInInterval(BigInt(1), p - BigInt(1));
            multiplicativeInverseK = BigInt::modInv(k, p - BigInt(1));
        }

        const BigInt r(BigInt::powMod(g, k, p));
        const BigInt s(BigInt::mod((m - x * r) * multiplicativeInverseK, p - BigInt(1)));

        return SignedData<Data>(data, Signature(r, s));
    }

    static bool verify(const SignedData<Data>& signedMessage, const PublicKey& publicKey) {
        const BigInt p = publicKey.getP();
        const BigInt g = publicKey.getG();
        const BigInt y = publicKey.getY();
        const BigInt r = signedMessage.getSignature().getR();
        const BigInt s = signedMessage.getSignature().getS();

        if (r > BigInt(0) && r < P && s > BigInt(0) && s < P - BigInt(1)) {
            Hash digest = Elgamal::calculateHash(signedMessage.getMessage());
            const BigInt m(digest.at(digest.size() - 1));

            return BigInt::mod(BigInt::pow(y, r) * BigInt::pow(r, s), p) == BigInt::powMod(g, m, p);
        }

        return false;
    }

    static Data encrypt(const Data& data, const PublicKey& publicKey) {
        const BigInt p = publicKey.getP();
        const BigInt g = publicKey.getG();
        const BigInt y = publicKey.getY();
        Data encryptedData(data.size() * 2);

        BigInt k(p - 2);

        for (BigInt i = BigInt(2); i < BigInt(p - 1); i = i + BigInt(1)) {
            if (BigInt::calculateGCD(i, p) == BigInt(1)) {
                k = i;
                break;
            }
        }

        for (int i = 0; i < data.size(); ++i) {
            BigInt a = BigInt::powMod(g, k, p);
            BigInt b = BigInt::mod(BigInt::pow(y, k) * BigInt(data[i]), p);
            encryptedData[i * 2] = a;
            encryptedData[i * 2 + 1] = b;
        }

        return encryptedData;
    }

    static Data decrypt(const Data& encryptedData, const KeyPair& keyPair) {
        const BigInt p = keyPair.getPublicKey().getP();
        const BigInt g = keyPair.getPublicKey().getG();
        const BigInt y = keyPair.getPublicKey().getY();
        const BigInt x = keyPair.getPrivateKey();
        Data decryptedData(encryptedData.size() / 2);

        for (int i = 0; i < decryptedData.size(); ++i) {
            const BigInt a = encryptedData[i * 2];
            const BigInt b = encryptedData[i * 2 + 1];
            decryptedData[i] = BigInt::mod(b * BigInt::pow(a, p - BigInt(1) - x), p);
        }

        return decryptedData;
    }
};

#endif // ELGAMAL_H_INCLUDED
