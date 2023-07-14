#ifndef SCHNORR_SIGNATURE_H_INCLUDED
#define SCHNORR_SIGNATURE_H_INCLUDED

#include "signature-algorithm.h"

namespace EllipticCryptography {
    class SchnorrSignature : public SignatureAlgorithm {
    public:
        SchnorrSignature(const Curve& curve) : SignatureAlgorithm(curve) {}

        virtual SignedMessage<OctetString> sign(const OctetString& message, const PrivateKey& privateKey) const override {
            const BigInt n = this->curve.getBasePointOrder();
            const Point G = this->curve.getBasePoint();
            while (true) {
                const BigInt k = BigInt::generateInRange(1, n - 1);
                const Point Q = k * G;
                const OctetString xQasOctetString = Q.getX().toOctetString();
                OctetString e = message;
                e.insert(e.end(), xQasOctetString.begin(), xQasOctetString.end());
                const BigInt r = getHashAsBigInt(e);
                if (BigInt::mod(r, n) == 0) {
                    continue;
                }
                const BigInt s = BigInt::mod(k - BigInt::mulMod(r, privateKey, n), n);
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
            const Point Q = s * G + r * publicKey;
            if (this->curve.isAtInfinity(Q)) {
                return false;
            }
            const OctetString xQasOctetString = Q.getX().toOctetString();
            OctetString e = signedMessage.getMessage();
            e.insert(e.end(), xQasOctetString.begin(), xQasOctetString.end());
            const BigInt v = getHashAsBigInt(e);
            return v == r;
        }
    };
}

#endif // SCHNORR_SIGNATURE_H_INCLUDED
