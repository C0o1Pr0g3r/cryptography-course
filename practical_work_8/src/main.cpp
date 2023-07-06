#include <iostream>
#include "big-int.h"
#include "elliptic-cryptography/curve.h"
#include "elliptic-cryptography/point.h"

using namespace std;
using namespace EllipticCryptography;

const string SUCCESSFUL_MESSAGE = "Test passed";
const string FAILURE_MESSAGE = "Test failed";

int main() {
    const BuiltinCurve curve = BuiltinCurve::getRandomBuiltinCurve();
    const Point basePoint = curve.getBasePoint();
    const BigInt basePointOrder = curve.getBasePointOrder();
    BigInt k = BigInt::generateInRange(0, basePointOrder - 1);
    BigInt d = BigInt::generateInRange(0, basePointOrder - 1);

    const Point& g = basePoint;

    const Point h1 = d * g;
    const Point h2 = k * h1;
    const Point h3 = k * g;
    const Point h4 = d * h3;

    cout << "1. Equation test" << endl;
    cout << (h2 == h4 ? SUCCESSFUL_MESSAGE : FAILURE_MESSAGE) << endl;
    cout << endl;

    const BuiltinCurve curve2 = BuiltinCurve::getRandomBuiltinCurve();
    const BuiltinCurve curve3(curve2);

    cout << "First curve: " << curve.getId() << ", " << curve.getName()
        << endl;
    cout << "Second curve: " << curve2.getId() << ", " << curve2.getName()
        << endl;
    cout << "Third curve: " << curve3.getId() << ", " << curve3.getName()
        << endl;

    cout << "2. Other tests" << endl;

    cout << "curve == curve2: " << (curve == curve2 ? "true" : "false")
        << endl;
    cout << "curve2 == curve3: " << (curve2 == curve3 ? "true" : "false")
        << endl;

    cout << "curve.contains(curve.getBasePoint().doubled()): "
        << (curve.contains(curve.getBasePoint().doubled())
            ? "true" : "false")
        << endl;
    cout << "curve2.contains("
                "BigInt::generateInRange(0, curve3.getBasePointOrder())"
                " * curve3.getBasePoint().doubled()): "
            << (curve2.contains(
                BigInt::generateInRange(0, curve3.getBasePointOrder())
                * curve3.getBasePoint().doubled())
                    ? "true" : "false"
                )
        << endl;

    return 0;
}
