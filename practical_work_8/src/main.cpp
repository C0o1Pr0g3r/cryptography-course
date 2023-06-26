#include <iostream>
#include "big-int.h"
#include "elliptic-cryptography/curve.h"
#include "elliptic-cryptography/point.h"
#include "elliptic-cryptography/functions.h"

using namespace std;

const string SUCCESSFUL_MESSAGE = "Test passed";
const string FAILURE_MESSAGE = "Test failed";

int main() {
    const BuiltinCurve curve = BuiltinCurve::getRandomBuiltinCurve();
    const Point g = getGenerator(curve);
    BigInt k = BigInt::generateInRange(0, g.getCoordinates(curve).x);
    BigInt d = BigInt::generateInRange(0, g.getCoordinates(curve).x);

    const Point h1 = multiply(curve, d, g);
    const Point h2 = multiply(curve, k, h1);
    const Point h3 = multiply(curve, k, g);
    const Point h4 = multiply(curve, d, h3);

    cout << "1. Equation test" << endl;
    cout << (isEqual(curve, h2, h4) ? SUCCESSFUL_MESSAGE : FAILURE_MESSAGE) << endl;
    cout << endl;

    const BuiltinCurve curve2 = BuiltinCurve::getRandomBuiltinCurve();
    const BuiltinCurve curve3(curve2);

    cout << "First curve: " << curve.getId() << ", " << curve.getName() << endl;
    cout << "Second curve: " << curve2.getId() << ", " << curve2.getName() << endl;
    cout << "Third curve: " << curve3.getId() << ", " << curve3.getName() << endl;

    cout << "getGenerator(curve2).getCoordinates(curve3): " << getGenerator(curve2).getCoordinates(curve3) << endl;
    cout << endl;

    cout << "2. Other tests" << endl;

    cout << "curve == curve2: " << (curve == curve2 ? "true" : "false") << endl;
    cout << "curve2 == curve3: " << (curve2 == curve3 ? "true" : "false") << endl;

    cout << "isPointOnCurve(getGenerator(curve2), curve: " << (isPointOnCurve(getGenerator(curve2), curve) ? "true" : "false") << endl;
    cout << "isPointOnCurve(getGenerator(curve2), curve3: " << (isPointOnCurve(getGenerator(curve2), curve3) ? "true" : "false") << endl;

    return 0;
}
