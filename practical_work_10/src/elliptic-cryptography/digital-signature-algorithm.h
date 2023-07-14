#ifndef DIGITAL_SIGNATURE_ALGORITHM_H_INCLUDED
#define DIGITAL_SIGNATURE_ALGORITHM_H_INCLUDED

#include "signature-algorithm.h"

namespace EllipticCryptography {
    class DigitalSignatureAlgorithm : public SignatureAlgorithm {
    public:
        DigitalSignatureAlgorithm(const Curve& curve) : SignatureAlgorithm(curve) {}

        virtual SignedMessage<OctetString> sign(const OctetString& message, const PrivateKey& privateKey) const override {
            const BigInt n = this->curve.getBasePointOrder();
            const Point G = this->curve.getBasePoint();
            const BigInt m = getHashAsBigInt(message);
            while (true) {
                const BigInt k = BigInt::generateInRange(1, n - 1);
                const Point Q = k * G;
                const BigInt r = BigInt::mod(Q.getX(), n);
                if (r == 0) {
                    continue;
                }
                const BigInt inverseK = BigInt::computeInverseModulo(k, n);
                const BigInt s = BigInt::mulMod(inverseK, (r * privateKey + m), n);
                if (s == 0) {
                    continue;
                }
                return SignedMessage(message, Signature(r, s));
            }
        }

        virtual bool verify(const SignedMessage<OctetString>& signedMessage, const PublicKey& publicKey) const override {
            const BigInt n = this->curve.getBasePointOrder();
            const Point G = this->curve.getBasePoint();
            const BigInt m = getHashAsBigInt(signedMessage.getMessage());
            const BigInt r = signedMessage.getSignature().getR();
            const BigInt s = signedMessage.getSignature().getS();
            if (!(r > 0 && r < n && s > 0 && s < n)) {
                return false;
            }
            const BigInt inverseS = BigInt::computeInverseModulo(s, n);
            const BigInt u1 = BigInt::mulMod(inverseS, m, n);
            const BigInt u2 = BigInt::mulMod(inverseS, r, n);
            const Point Q = u1 * G + u2 * publicKey;
            if (this->curve.isAtInfinity(Q)) {
                return false;
            }
            const BigInt v = BigInt::mod(Q.getX(), n);
            return v == r;
        }
    };
}

#endif // DIGITAL_SIGNATURE_ALGORITHM_H_INCLUDED
