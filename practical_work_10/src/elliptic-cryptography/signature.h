#ifndef SIGNATURE_H_INCLUDED
#define SIGNATURE_H_INCLUDED

#include <ostream>
#include "../big-int.h"

namespace EllipticCryptography {
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
        out << "(r: " << signature.r << "; s: " << signature.s << ")";
        return out;
    }
}

#endif // SIGNATURE_H_INCLUDED
