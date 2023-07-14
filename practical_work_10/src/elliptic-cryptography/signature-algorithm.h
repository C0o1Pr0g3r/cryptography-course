#ifndef SIGNATURE_ALGORITHM_H_INCLUDED
#define SIGNATURE_ALGORITHM_H_INCLUDED

#include <string>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include "../definitions.h"
#include "curve.h"
#include "key-pair.h"
#include "signed-message.h"

namespace EllipticCryptography {
    class SignatureAlgorithm {
    protected:
        Curve curve;

    protected:
        static Hash computeHash(const OctetString& data) {
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
                ss
                    << setfill('0') << setw(HEX_DIGITS_PER_BYTE)
                    << hex << static_cast<int>(hash[i]);
            }
            return ss.str();
        }

        static BigInt getHashAsBigInt(const OctetString& data) {
            return BigInt(convertHashToHex(computeHash(data)), Radix::HEX);
        }

    public:
        SignatureAlgorithm(const Curve& curve) : curve(curve) {}
        SignatureAlgorithm(const SignatureAlgorithm& other) : curve(other.curve) {}

        SignedMessage<string> sign(const string& message, const PrivateKey& privateKey) const {
            const OctetString data(message.begin(), message.end());
            const SignedMessage signedMessage = this->sign(data, privateKey);
            return SignedMessage(message, signedMessage.getSignature());
        }

        bool verify(
            const SignedMessage<string>& signedMessage, const PublicKey& publicKey
        ) const {
            OctetString data(signedMessage.getMessage().length());
            for (int i = 0; i < signedMessage.getMessage().length(); ++i) {
                data[i] = signedMessage.getMessage()[i];
            }
            return verify(SignedMessage(
                data, signedMessage.getSignature()), publicKey
            );
        }

        virtual SignedMessage<OctetString> sign(const OctetString& message, const PrivateKey& privateKey) const = 0;
        virtual bool verify(const SignedMessage<OctetString>& signedMessage, const PublicKey& publicKey) const = 0;
        virtual ~SignatureAlgorithm() {}


        SignatureAlgorithm& operator=(const SignatureAlgorithm& other) {
            if (this != &other) {
                this->curve = other.curve;
            }
            return *this;
        }
    };
}

#endif // SIGNATURE_ALGORITHM_H_INCLUDED
