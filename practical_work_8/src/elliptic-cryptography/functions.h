#include "../definitions.h"
#include "curve.h"
#include "point.h"

Point getGenerator(const Curve& curve) {
    Point point(curve.group);
    point.data = EC_POINT_dup(EC_GROUP_get0_generator(curve.group), curve.group);
    if (!point.data) {
        throw runtime_error(OPERATION_FAILED);
    }
    return point;
}

bool isPointOnCurve(const Point& point, const Curve& curve) {
    const int result = EC_POINT_is_on_curve(curve.group, point.data, nullptr);
    if (result == -1) {
        throw runtime_error(OPERATION_FAILED);
    }
    return result;
}

Point sum(const Curve& curve, const Point& a, const Point& b) {
    Point result(curve);
    if (!EC_POINT_add(curve.group, result.data, a.data, b.data, nullptr)) {
        throw runtime_error(OPERATION_FAILED);
    }
    return result;
}

Point redouble(const Curve& curve, const Point& a) {
    Point result(curve);
    if (!EC_POINT_dbl(curve.group, result.data, a.data, nullptr)) {
        throw runtime_error(OPERATION_FAILED);
    }
    return result;
}

Point multiply(const Curve& curve, const BigInt& n, const Point& a) {
    Point result(curve);
    if (!EC_POINT_mul(curve.group, result.data, nullptr, a.data, n.data, nullptr)) {
        throw runtime_error(OPERATION_FAILED);
    }
    return result;
}

bool isEqual(const Curve& curve, const Point& a, const Point& b) {
    const int result = EC_POINT_cmp(curve.group, a.data, b.data, nullptr);
    if (result == -1) {
        throw runtime_error(OPERATION_FAILED);
    }
    return !result;
}
