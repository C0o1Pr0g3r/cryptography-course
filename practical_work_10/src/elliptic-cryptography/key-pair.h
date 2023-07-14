#ifndef KEY_PAIR_H_INCLUDED
#define KEY_PAIR_H_INCLUDED

#include <ostream>
#include "curve.h"
#include "point.h"

namespace EllipticCryptography {
    using PrivateKey = BigInt;
    using PublicKey = Point;

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

        static KeyPair generate(const Curve& curve) {
            const PrivateKey privateKey = BigInt::generateInRange(
                1, curve.getBasePointOrder() - 1
            );
            const PublicKey publicKey = privateKey * curve.getBasePoint();
            return KeyPair(privateKey, publicKey);
        }
    };

    ostream& operator<<(ostream& out, const KeyPair& keyPair) {
        out << "(" << "private: " << keyPair.privateKey << "; "
            << "public: " << keyPair.publicKey << ")";
        return out;
    }
}

#endif // KEY_PAIR_H_INCLUDED
