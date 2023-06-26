#ifndef GROUP_H_INCLUDED
#define GROUP_H_INCLUDED

#include <stdexcept>
#include <openssl/ec.h>
#include "../definitions.h"
#include "../utilities.h"
#include "../big-int.h"

using namespace std;
using namespace Utilities;

class Point;

const size_t MAX_NUMBER_OF_BUILTIN_CURVES = 256;

class Curve {
protected:
    friend Point getGenerator(const Curve& curve);
    friend bool isPointOnCurve(const Point& point, const Curve& curve);
    friend Point sum(const Curve& curve, const Point& a, const Point& b);
    friend Point redouble(const Curve& curve, const Point& a);
    friend Point multiply(const Curve& curve, const BigInt& n, const Point& a);
    friend bool isEqual(const Curve& curve, const Point& a, const Point& b);
    friend class Point;

protected:
    EC_GROUP* group = nullptr;

protected:
    Curve(const EC_GROUP* group)
    :   group(EC_GROUP_dup(group))
    {
        if (!this->group) {
            throw runtime_error(OPERATION_FAILED);
        }
    }

public:
    Curve(const BigInt& p, const BigInt& a, const BigInt& b)
    :   group(EC_GROUP_new_curve_GFp(p.data, a.data, b.data, nullptr))
    {
        if (!this->group) {
            throw runtime_error(OPERATION_FAILED);
        }
    }

    Curve(const Curve& other)
    :   group(EC_GROUP_dup(other.group))
    {
        if (!this->group) {
            throw runtime_error(OPERATION_FAILED);
        }
    }

    virtual ~Curve() {
        EC_GROUP_clear_free(this->group);
    }

    bool operator==(const Curve& other) const {
        const int result = EC_GROUP_cmp(this->group, other.group, nullptr);
        if (result == -1) {
            throw runtime_error(OPERATION_FAILED);
        }
        return !result;
    }
};

class BuiltinCurve : public Curve {
private:
    int id;
    string name;

private:
    BuiltinCurve(const EC_GROUP* group, const int id, const string& name)
    :   Curve(group),
        id(id),
        name(name)
    {}

public:
    static BuiltinCurve getRandomBuiltinCurve() {
        EC_builtin_curve builtinCurves[MAX_NUMBER_OF_BUILTIN_CURVES];
        size_t actualNumberOfBuiltinCurves =
            EC_get_builtin_curves(builtinCurves, MAX_NUMBER_OF_BUILTIN_CURVES);
        int index = generateIntegerInRange(0, actualNumberOfBuiltinCurves - 1);
        BuiltinCurve builtinCurve(
            EC_GROUP_new_by_curve_name(builtinCurves[index].nid),
            builtinCurves[index].nid,
            builtinCurves[index].comment
        );
        return builtinCurve;
    }

    const int getId() const {
        return this->id;
    }

    const string& getName() const {
        return this->name;
    }
};

#endif // GROUP_H_INCLUDED
