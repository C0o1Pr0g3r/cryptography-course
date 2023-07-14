
#ifndef SIGNED_MESSAGE_H_INCLUDED
#define SIGNED_MESSAGE_H_INCLUDED

#include "signature.h"

namespace EllipticCryptography {
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
}

#endif // SIGNED_MESSAGE_H_INCLUDED
