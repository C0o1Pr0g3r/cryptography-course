#ifndef CURVE_H_INCLUDED
#define CURVE_H_INCLUDED

#include <stdexcept>
#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include "../definitions.h"
#include "../big-int.h"
#include "point.h"
#include "shared.h"

using namespace std;

const size_t MAX_NUMBER_OF_BUILTIN_CURVES = 256;

namespace EllipticCryptography {
    class Curve {
    private:
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
            EC_GROUP_free(this->group);
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

        bool isAtInfinity(const Point& point) const {
            const int result = EC_POINT_is_at_infinity(
                this->group, point.data
            );
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
        friend ostream& operator<<(ostream& out, const BuiltinCurve& curve);

    public:
        enum class ID {
            SECP256K1 = NID_secp256k1,
        };

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
        static BuiltinCurve getById(const ID id) {
            if (id != ID::SECP256K1) {
                throw invalid_argument(
                    "The 'id' parameter cannot be equal to "
                    + to_string(static_cast<int>(id)));
            }
            EC_builtin_curve builtinCurves[MAX_NUMBER_OF_BUILTIN_CURVES];
            size_t actualNumberOfBuiltinCurves =
                EC_get_builtin_curves(
                    builtinCurves, MAX_NUMBER_OF_BUILTIN_CURVES
                );
            int index = 0;
            for (int i = 0; i < actualNumberOfBuiltinCurves; ++i) {
                if (builtinCurves[i].nid == static_cast<int>(id)) {
                    index = i;
                    break;
                }
            }
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

    ostream& operator<<(ostream& out, const BuiltinCurve& curve) {
        out << "(id: " << curve.id << "; name: " << curve.name << ")";
        return out;
    }
}

#endif // CURVE_H_INCLUDED
