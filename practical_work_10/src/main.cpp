#include <iostream>
#include "elliptic-cryptography/digital-signature-algorithm.h"
#include "elliptic-cryptography/schnorr-signature.h"

using namespace std;
using namespace EllipticCryptography;

const string SUCCESSFUL_MESSAGE = "Test passed";
const string FAILURE_MESSAGE = "Test failed";

void test(const string& algorithmName, const SignatureAlgorithm& signatureAlgorithm, const KeyPair& keyPair, const string& message);

int main() {
    const BuiltinCurve curve = BuiltinCurve::getById(BuiltinCurve::ID::SECP256K1);
    DigitalSignatureAlgorithm digitalSignatureAlgorithm(curve);
    SchnorrSignature schnorrSignature(curve);
    const KeyPair keyPair = KeyPair::generate(curve);

    cout << "Curve: " << curve << endl;
    cout << "Key pair: " <<  keyPair << endl;
    cout << endl;

    const string message = "Schnorr VS ECDSA";

    test("ECDSA", digitalSignatureAlgorithm, keyPair, message);
    cout << endl;
    test("EC-Schnorr", schnorrSignature, keyPair, message);

    return 0;
}

void test(const string& algorithmName, const SignatureAlgorithm& signatureAlgorithm, const KeyPair& keyPair, const string& message) {
    const SignedMessage signedMessage = signatureAlgorithm.sign(message, keyPair.getPrivateKey());
    SignedMessage signedMessageWithCorruptedMessage = signedMessage;
    signedMessageWithCorruptedMessage.getMessage()[3] = '3';
    SignedMessage signedMessageWithCorruptedSignature = signedMessage;
    signedMessageWithCorruptedSignature.getSignature().getS() = 84239;
    const PublicKey corruptedPublicKey = 54 * keyPair.getPublicKey();

    cout << "Testing the '" << algorithmName << "' algorithm." << endl;

    cout << "Signed message: '" << signedMessage.getMessage() << "'" << endl;
    cout << "Signature: " << signedMessage.getSignature() << endl;

    cout
        << "Signature verification for correct message and correct signature: "
        << (signatureAlgorithm.verify(signedMessage, keyPair.getPublicKey())
            ? SUCCESSFUL_MESSAGE : FAILURE_MESSAGE)
        << endl;
    cout
        << "Signature verification"
            " for corrupted message and correct signature: "
        << (signatureAlgorithm.verify(
            signedMessageWithCorruptedMessage, keyPair.getPublicKey()
        )
            ? SUCCESSFUL_MESSAGE : FAILURE_MESSAGE)
        << endl;
    cout
        << "Signature verification"
            " for correct message and corrupted signature: "
        << (signatureAlgorithm.verify(
            signedMessageWithCorruptedSignature, keyPair.getPublicKey()
        )
            ? SUCCESSFUL_MESSAGE : FAILURE_MESSAGE)
        << endl;
    cout
        << "Signature verification"
            " for correct message and signature but corrupted public key: "
        << (signatureAlgorithm.verify(
            signedMessage, corruptedPublicKey
        )
            ? SUCCESSFUL_MESSAGE : FAILURE_MESSAGE)
        << endl;
}
