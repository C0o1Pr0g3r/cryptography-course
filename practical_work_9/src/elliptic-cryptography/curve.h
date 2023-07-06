#ifndef CURVE_H_INCLUDED
#define CURVE_H_INCLUDED

#include <stdexcept>
#include <openssl/ec.h>
#include "../definitions.h"
#include "../utilities.h"
#include "../big-int.h"
#include "point.h"
#include "shared.h"

using namespace std;
using namespace Utilities;

const size_t MAX_NUMBER_OF_BUILTIN_CURVES = 256;

namespace EllipticCryptography {
    class Curve {
    private:
        friend class Point;
        friend class Expression;

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

    private:
        BigInt getCofactor() const {
            BigInt result;
            if (!EC_GROUP_get_cofactor(
                this->group, result.data, BigInt::Context().data
            )) {
                throw runtime_error(OPERATION_FAILED);
            }
            return result;
        }

    public:
        Curve(const BigInt& p, const BigInt& a, const BigInt& b)
        :   group(EC_GROUP_new_curve_GFp(
                p.data, a.data, b.data, BigInt::Context().data
            ))
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

        BigInt getBasePointOrder() const {
            BigInt result;
            if (!EC_GROUP_get_order(
                this->group, result.data, BigInt::Context().data
            )) {
                throw runtime_error(OPERATION_FAILED);
            }
            return result;
        }

        BigInt getOrder() const {
            return this->getCofactor() * this->getBasePointOrder();
        }

        Point getBasePoint() const {
            Point point(EC_GROUP_get0_generator(this->group), this->group);
            if (!point.data) {
                throw runtime_error(OPERATION_FAILED);
            }
            return point;
        }

        bool contains(const Point& point) const {
            const int result = EC_POINT_is_on_curve(
                this->group, point.data, BigInt::Context().data
            );
            if (result == -1) {
                throw runtime_error(OPERATION_FAILED);
            }
            return result;
        }

        Curve& operator=(const Curve& other) {
            if (this != &other) {
                if (!EC_GROUP_copy(this->group, other.group)) {
                    throw runtime_error(OPERATION_FAILED);
                }
            }
            return *this;
        }

        bool operator==(const Curve& other) const {
            return areEqual(this->group, other.group);
        }

        bool operator!=(const Curve& other) const {
            return !(*this == other);
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
                EC_get_builtin_curves(
                    builtinCurves, MAX_NUMBER_OF_BUILTIN_CURVES
                );
            int index = generateIntegerInRange(
                0, actualNumberOfBuiltinCurves - 1
            );
            BuiltinCurve builtinCurve(
                EC_GROUP_new_by_curve_name(builtinCurves[index].nid),
                builtinCurves[index].nid,
                builtinCurves[index].comment
            );
            return builtinCurve;
        }

        int getId() const {
            return this->id;
        }

        string getName() const {
            return this->name;
        }
    };
}

#endif // CURVE_H_INCLUDED
