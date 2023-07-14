#ifndef BIG_INT_H_INCLUDED
#define BIG_INT_H_INCLUDED

#include <ostream>
#include <functional>
#include <stdexcept>
#include <bitset>
#include <openssl/bn.h>
#include <openssl/ec.h>
#include "definitions.h"

using namespace std;

using Word = BN_ULONG;

enum class Radix {
    BIN = 2,
    DEC = 10,
    HEX = 16,
};

namespace EllipticCryptography {
    class Curve;
    class BuiltinCurve;
    class Point;
}

class BigInt {
private:
    friend class EllipticCryptography::Curve;
    friend class EllipticCryptography::BuiltinCurve;
    friend class EllipticCryptography::Point;
    friend ostream& operator<<(ostream& out, const BigInt& bigInt);
    friend bool areEqual(const EC_GROUP* a, const EC_GROUP* b);

    using BinaryOperation = function<
        int (BIGNUM*, const BIGNUM*, const BIGNUM*
    )>;
    using BinaryOperationWithContext = function<
        int (BIGNUM*, const BIGNUM*, const BIGNUM*, BN_CTX*)
    >;
    using BinaryOperationWithWord = function<int (BIGNUM*, Word)>;
    using TernaryOperationWithContext = function<
        int (BIGNUM*, const BIGNUM*, const BIGNUM*, const BIGNUM*, BN_CTX *ctx)
    >;

    class Context {
    private:
        friend class BigInt;
        friend class EllipticCryptography::Curve;
        friend class EllipticCryptography::BuiltinCurve;
        friend class EllipticCryptography::Point;
        friend bool areEqual(const EC_GROUP* a, const EC_GROUP* b);

    private:
        BN_CTX* data = nullptr;

    public:
        Context() {
            this->data = BN_CTX_new();
            if (!this->data) {
                throw bad_alloc();
            }
        }

        ~Context() {
            BN_CTX_free(this->data);
        }
    };

private:
    BIGNUM* data = nullptr;

private:
    static BigInt perform(
        const BinaryOperation& op, const BigInt& a, const BigInt& b
    ) {
        BigInt result;
        if (!op(result.data, a.data, b.data)) {
            throw runtime_error(OPERATION_FAILED);
        }
        return result;
    }

    static BigInt perform(
        const BinaryOperationWithContext& op, const BigInt& a, const BigInt& b
    ) {
        Context ctx;
        BigInt result;
        if (!op(result.data, a.data, b.data, ctx.data)) {
            throw runtime_error(OPERATION_FAILED);
        }
        return result;
    }

    static BigInt perform(
        const BinaryOperationWithWord& op, const BigInt& a, const Word& w
    ) {
        BigInt result(a);
        if (!op(result.data, w)) {
            throw runtime_error(OPERATION_FAILED);
        }
        return result;
    }

    static BigInt perform(
        const TernaryOperationWithContext& op,
        const BigInt& a,
        const BigInt& b,
        const BigInt& c
    ) {
        Context ctx;
        BigInt result;
        if (!op(result.data, a.data, b.data, c.data, ctx.data)) {
            throw runtime_error(OPERATION_FAILED);
        }
        return result;
    }

public:
    BigInt() {
        this->data = BN_new();
        if (!this->data) {
            throw bad_alloc();
        }
    }

    BigInt(const BigInt& other) : BigInt() {
        BN_copy(this->data, other.data);
    }

    BigInt(const Word& word) : BigInt() {
        BN_set_word(this->data, word);
    }

    BigInt(const string& str, const Radix& radix = Radix::DEC) : BigInt() {
        const auto throwInvalidArgument = [&]() {
            throw invalid_argument(
                "Failed to parse '" + str
                + "' in base " + to_string(static_cast<int>(radix))
            );
        };

        const int numberOfBytes = str.length() % BITS_PER_BYTE != 0
            ? str.length() / BITS_PER_BYTE + 1 : str.length() / BITS_PER_BYTE;
        vector<Byte> bytes(numberOfBytes);
        if (radix == Radix::BIN) {
            for (
                int i = str.length(), j = numberOfBytes - 1;
                i >= 0;
                i -= BITS_PER_BYTE, --j
            ) {
                const size_t pos = static_cast<int>(i - BITS_PER_BYTE) >= 0
                    ? i - BITS_PER_BYTE : 0;
                const size_t n = static_cast<int>(i - BITS_PER_BYTE) >= 0
                    ? BITS_PER_BYTE : str.length() % BITS_PER_BYTE;
                try {
                    bytes[j] = bitset<BITS_PER_BYTE>(
                        str.substr(pos, n)).to_ullong() & LOW_BYTE_MASK;
                } catch(const exception& e) {
                    throwInvalidArgument();
                }
            }

            if (!BN_bin2bn(
                reinterpret_cast<const unsigned char *>(bytes.data()),
                bytes.size(),
                this->data)
            ) {
                throwInvalidArgument();
            }
        } else if (radix == Radix::DEC) {
            if (!BN_dec2bn(&this->data, str.c_str())) {
                throwInvalidArgument();
            }
        } else if (radix == Radix::HEX) {
            if (!BN_hex2bn(&this->data, str.c_str())) {
                throwInvalidArgument();
            }
        } else {
            throw invalid_argument(
                "The 'radix' parameter cannot be equal to "
                + to_string(static_cast<int>(radix)));
        }
    }

    ~BigInt() {
        BN_clear_free(this->data);
    }

    string toString(const Radix& radix = Radix::DEC) const {
        string result("");
        if (radix == Radix::BIN) {
            char* str = new char[BN_num_bytes(this->data) + 1];
            const int length = BN_bn2bin(
                this->data, reinterpret_cast<unsigned char*>(str)
            );
            for (int i = 0; i < length; ++i) {
                result += bitset<BITS_PER_BYTE>(str[i]).to_string();
            }
            result = result.substr(result.find('1'));
            delete[] str;
        } else if (radix == Radix::DEC) {
            char* str = BN_bn2dec(this->data);
            if (str) {
                result = str;
                OPENSSL_clear_free(str, 0);
            }
        } else if (radix == Radix::HEX) {
            char* str = BN_bn2hex(this->data);
            if (str) {
                result = str;
                OPENSSL_clear_free(str, 0);
            }
        } else {
            throw invalid_argument(
                "The 'radix' parameter cannot be equal to "
                + to_string(static_cast<int>(radix)));
        }

        if (result == "") {
            throw runtime_error(
                "Failed to convert big integer"
                    " to string representation in base "
                + to_string(static_cast<int>(radix))
            );
        }
        return result;
    }

    BigInt& operator=(const BigInt& other) {
        if (this != &other) {
            BN_clear(this->data);
            BN_copy(this->data, other.data);
        }
        return *this;
    }

    BigInt operator-() const {
        BigInt result(*this);
        BN_set_negative(result.data, !BN_is_negative(result.data));
        return result;
    }

    BigInt operator+(const BigInt& other) const {
        return perform(BN_add, *this, other);
    }

    BigInt operator-(const BigInt& other) const {
        return perform(BN_sub, *this, other);
    }

    BigInt operator*(const BigInt& other) const {
        return perform(BN_mul, *this, other);
    }

    BigInt operator/(const BigInt& other) const {
        Context ctx;
        BigInt result;
        if (!BN_div(result.data, nullptr, this->data, other.data, ctx.data)) {
            throw runtime_error(OPERATION_FAILED);
        }
        return result;
    }

    BigInt operator%(const BigInt& other) const {
        Context ctx;
        BigInt result;
        if (!BN_mod(result.data, this->data, other.data, ctx.data)) {
            throw runtime_error(OPERATION_FAILED);
        }
        return result;
    }

    BigInt operator+(const Word& word) const {
        return perform(BN_add_word, *this, word);
    }

    BigInt operator-(const Word& word) const {
        return perform(BN_sub_word, *this, word);
    }

    BigInt operator*(const Word& word) const {
        return perform(BN_mul_word, *this, word);
    }

    BigInt operator/(const Word& word) const {
        return perform(BN_div_word, *this, word);
    }

    BigInt operator%(const Word& word) const {
        return perform(BN_mod_word, *this, word);
    }

    bool operator<(const BigInt& other) const {
        return BN_cmp(this->data, other.data) == -1;
    }

    bool operator<(const Word& word) const {
        return BN_cmp(this->data, BigInt(word).data) == -1;
    }

    bool operator>(const BigInt& other) const {
        return BN_cmp(this->data, other.data) == 1;
    }

    bool operator>(const Word& word) const {
        return BN_cmp(this->data, BigInt(word).data) == 1;
    }

    bool operator==(const BigInt& other) const {
        return BN_cmp(this->data, other.data) == 0;
    }

    bool operator==(const Word& word) const {
        return BN_cmp(this->data, BigInt(word).data) == 0;
    }

    bool operator!=(const Word& word) const {
        return !(*this == word);
    }

    explicit operator Word() const {
        return BN_get_word(this->data);
    }

    operator string() const {
        return this->toString();
    }

    static BigInt mod(const BigInt& a, const BigInt& m) {
        return perform(BN_nnmod, a, m);
    }

    static BigInt mulMod(const BigInt& a, const BigInt& b,  const BigInt& m) {
        return perform(BN_mod_mul, a, b, m);
    }

    static BigInt powMod(const BigInt& a, const BigInt& p,  const BigInt& m) {
        return perform(BN_mod_exp, a, p, m);
    }

    static BigInt computeInverseModulo(const BigInt& a, const BigInt& n) {
        Context ctx;
        BigInt result;
        if (!BN_mod_inverse(result.data, a.data, n.data, ctx.data)) {
            throw runtime_error(OPERATION_FAILED);
        }
        return result;
    }

    static BigInt generatePrime(const size_t length) {
        BigInt result;
        if (!BN_generate_prime_ex(
            result.data, length, false, nullptr, nullptr, nullptr
        )) {
            throw runtime_error(OPERATION_FAILED);
        }
        return result;
    }

    static BigInt computeGreatestCommonDivisor(
        const BigInt& a, const BigInt& b
    ) {
        return perform(BN_gcd, a, b);
    }

    static BigInt generateInRange(
        const BigInt& min, const BigInt& max
    ) {
        const BigInt range = max - min + 1;
        BigInt result;
        if (!BN_rand_range(result.data, range.data)) {
            throw runtime_error(OPERATION_FAILED);
        }
        return result + min;
    }

    static BigInt generateInInterval(
        const BigInt& min, const BigInt& max
    ) {
        return generateInRange(min + 1, max - 1);
    }

    static BigInt mask(const BigInt& a, size_t n) {
        BigInt result(a);
        BN_mask_bits(result.data, n);
        return result;
    }
};

ostream& operator<<(ostream& out, const BigInt& bigInt) {
    char* str = BN_bn2dec(bigInt.data);
    out << str;
    OPENSSL_clear_free(str, 0);
    return out;
}

#endif // BIG_INT_H_INCLUDED
