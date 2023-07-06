#ifndef POINT_H_INCLUDED
#define POINT_H_INCLUDED

#include <stdexcept>
#include <openssl/ec.h>
#include "../definitions.h"
#include "../big-int.h"
#include "shared.h"

struct Vector2 {
public:
    BigInt x;
    BigInt y;

public:
    Vector2(const BigInt& x = 0, const BigInt& y = 0)
    :   x(x),
        y(y)
    {}
};

ostream& operator<<(ostream& out, const Vector2& vec) {
    out << "(" << vec.x << "; " << vec.y << ")";
    return out;
}

namespace EllipticCryptography {
    class Point {
    private:
        friend class Curve;

    private:
        EC_POINT* data = nullptr;
        EC_GROUP* group = nullptr;

    private:
        Point(const EC_GROUP* group)
        :   data(EC_POINT_new(group)),
            group(EC_GROUP_dup(group))
        {
            if (!this->data) {
                throw runtime_error(OPERATION_FAILED);
            }
        }

        Point(const EC_POINT* data, const EC_GROUP* group)
        :   data(EC_POINT_dup(data, group)),
            group(EC_GROUP_dup(group))
        {
            if (!this->data) {
                throw runtime_error(OPERATION_FAILED);
            }
        }

    public:
        ~Point() {
            EC_POINT_clear_free(this->data);
            EC_GROUP_clear_free(this->group);
        }

    public:
        Vector2 getCoordinates() const {
            Vector2 coordinates;
            if (!EC_POINT_get_affine_coordinates(
                this->group,
                this->data,
                coordinates.x.data,
                coordinates.y.data,
                BigInt::Context().data
            )) {
                throw runtime_error(OPERATION_FAILED);
            }
            return coordinates;
        }

        Point doubled() const {
            Point result(this->group);
            if (!EC_POINT_dbl(
                this->group, result.data, this->data, BigInt::Context().data
            )) {
                throw runtime_error(OPERATION_FAILED);
            }
            return result;
        }

        Point operator+(const Point& other) const {
            Point result(this->group);
            if (
                !areEqual(this->group, other.group)
                ||
                !EC_POINT_add(
                    this->group,
                    result.data,
                    this->data,
                    other.data,
                    BigInt::Context().data
            )) {
                throw runtime_error(OPERATION_FAILED);
            }
            return result;
        }

        Point operator*(const BigInt& n) const {
            Point result(this->group);
            if (!EC_POINT_mul(
                this->group,
                result.data,
                nullptr,
                this->data,
                n.data,
                BigInt::Context().data
            )) {
                throw runtime_error(OPERATION_FAILED);
            }
            return result;
        }

        bool operator==(const Point& other) const {
            if (!areEqual(this->group, other.group)) {
                throw runtime_error(OPERATION_FAILED);
            }
            const int result = EC_POINT_cmp(
                this->group, this->data, other.data, BigInt::Context().data
            );
            if (result == -1) {
                throw runtime_error(OPERATION_FAILED);
            }
            return !result;
        }
    };
}

ostream& operator<<(ostream& out, const EllipticCryptography::Point& point) {
    out << point.getCoordinates();
    return out;
}

EllipticCryptography::Point operator*(
    const BigInt& n, const EllipticCryptography::Point& point
) {
    return point * n;
}

#endif // POINT_H_INCLUDED
