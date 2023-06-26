#ifndef POINT_H_INCLUDED
#define POINT_H_INCLUDED

#include <ostream>
#include <openssl/ec.h>
#include "../definitions.h"
#include "../big-int.h"
#include "curve.h"

using namespace std;

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

class Point {
private:
    friend Point getGenerator(const Curve& curve);
    friend bool isPointOnCurve(const Point& point, const Curve& curve);
    friend Point sum(const Curve& curve, const Point& a, const Point& b);
    friend Point redouble(const Curve& curve, const Point& a);
    friend Point multiply(const Curve& curve, const BigInt& n, const Point& a);
    friend bool isEqual(const Curve& curve, const Point& a, const Point& b);
    friend class Curve;
private:
    EC_POINT* data;

public:
    Point(const Curve& curve)
    :   data(EC_POINT_new(curve.group))
    {
        if (!this->data) {
            throw runtime_error(OPERATION_FAILED);
        }
    }

    ~Point() {
        EC_POINT_clear_free(this->data);
    }

    Vector2 getCoordinates(const Curve& curve) const {
        Vector2 coordinates;
        if (!EC_POINT_get_affine_coordinates(
            curve.group, this->data, coordinates.x.data, coordinates.y.data, nullptr
        )) {
            throw runtime_error(OPERATION_FAILED);
        }
        return coordinates;
    }
};

#endif // POINT_H_INCLUDED
