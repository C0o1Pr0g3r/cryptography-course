#ifndef BIGINT_H_INCLUDED
#define BIGINT_H_INCLUDED

#include <ostream>
#include <openssl/bn.h>

using namespace std;

using Int = BN_ULONG;
using BigIntContext = BN_CTX;

enum class Radix {
    BIN,
    DEC,
    HEX
};

class BigInt {
private:
    BIGNUM* data;

public:
    BigInt() : data(BN_new()) {}

    BigInt(const Int& number) : BigInt() {
        BN_set_word(this->data, number);
    }

    BigInt(const BigInt& other) : BigInt() {
        BN_copy(this->data, other.data);
    }

    BigInt(const string& str, const Radix& radix) : BigInt() {
        switch (radix) {
        case Radix::BIN:
            BN_bin2bn(reinterpret_cast<const unsigned char*>(str.c_str()), str.length(), this->data);
            break;
        case Radix::DEC:
            BN_dec2bn(&this->data, str.c_str());
            break;
        case Radix::HEX:
            BN_hex2bn(&this->data, str.c_str());
            break;
        default:
            break;
        }
    }

    BigInt& operator=(const Int& number) {
        BN_clear(this->data);
        BN_set_word(this->data, number);

        return *this;
    }

    BigInt& operator=(const BigInt& other) {
        BN_clear(this->data);
        BN_copy(this->data, other.data);

        return *this;
    }

    BigInt operator-() const {
        BigInt result(*this);

        BN_set_negative(result.data, !BN_is_negative(result.data));

        return result;
    }

    BigInt operator+(const BigInt& other) const {
        BigInt result;

        BN_add(result.data, this->data, other.data);

        return result;
    }

    BigInt operator-(const BigInt& other) const {
        BigInt result;

        BN_sub(result.data, this->data, other.data);

        return result;
    }

    BigInt operator*(const BigInt& other) const {
        BigIntContext* ctx = BN_CTX_new();
        BigInt result;

        BN_mul(result.data, this->data, other.data, ctx);
        BN_CTX_free(ctx);

        return result;
    }

    BigInt operator%(const BigInt& other) const {
        BigIntContext* ctx = BN_CTX_new();
        BigInt result;

        BN_mod(result.data, this->data, other.data, ctx);
        BN_CTX_free(ctx);

        return result;
    }

    bool operator<(const BigInt& other) const {
        return BN_cmp(this->data, other.data) == -1;
    }

    bool operator>(const BigInt& other) const {
        return BN_cmp(this->data, other.data) == 1;
    }

    bool operator==(const BigInt& other) const {
        return BN_cmp(this->data, other.data) == 0;
    }

    operator int() {
        return BN_get_word(this->data);
    }

    static BigInt pow(const BigInt& a, const BigInt& p) {
        BigIntContext* ctx = BN_CTX_new();
        BigInt result;

        BN_exp(result.data, a.data, p.data, ctx);
        BN_CTX_free(ctx);

        return result;
    }

    static BigInt mod(const BigInt& a, const BigInt& b) {
        BigIntContext* ctx = BN_CTX_new();
        BigInt result;

        BN_nnmod(result.data, a.data, b.data, ctx);
        BN_CTX_free(ctx);

        return result;
    }

    static BigInt powMod(const BigInt& a, const BigInt& p, const BigInt& m) {
        BigIntContext* ctx = BN_CTX_new();
        BigInt result;

        BN_mod_exp(result.data, a.data, p.data, m.data, ctx);
        BN_CTX_free(ctx);

        return result;
    }

    static BigInt modInv(const BigInt& a, const BigInt& n) {
        BigIntContext* ctx = BN_CTX_new();
        BigInt result;

        BN_mod_inverse(result.data, a.data, n.data, ctx);
        BN_CTX_free(ctx);

        return result;
    }

    static BigInt generatePrime(const size_t length) {
        BigInt result;

        BN_generate_prime_ex(result.data, length, 0, nullptr, nullptr, nullptr);

        return result;
    }

    bool isPrime() const {
        BigIntContext* ctx = BN_CTX_new();

        const bool result = BN_is_prime_ex(this->data, BN_prime_checks, ctx, nullptr);
        BN_CTX_free(ctx);

        return result;
    }

    static BigInt calculateGCD(const BigInt& a, const BigInt& b) {
        BigIntContext* ctx = BN_CTX_new();
        BigInt result;

        BN_gcd(result.data, a.data, b.data, ctx);
        BN_CTX_free(ctx);

        return result;
    }

    static BigInt randomInRange(const BigInt& min, const BigInt& max) {
        const BigInt difference = max - min + BigInt(1);
        BigInt result;

        BN_pseudo_rand_range(result.data, difference.data);

        return result + min;
    }

    static BigInt randomInInterval(const BigInt& min, const BigInt& max) {
        return BigInt::randomInRange(min + BigInt(1), max - BigInt(1));
    }

    ~BigInt() {
        BN_clear_free(this->data);
    }

    friend ostream& operator<<(ostream &out, const BigInt &number);
};

ostream& operator<<(ostream &out, const BigInt &number) {
    char* str = BN_bn2dec(number.data);
    out << str;
    OPENSSL_clear_free(str, 0);

    return out;
}

#endif // BIGINT_H_INCLUDED
