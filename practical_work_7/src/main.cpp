#include <iostream>
#include "big-int.h"
#include "elgamal.h"

using namespace std;

const string SUCCESSFUL_MESSAGE = "Test passed";
const string FAILURE_MESSAGE = "Test failed";

int main() {
    const KeyPair keyPair = Elgamal::generateKeyPair();
    cout << "Key pair: " << keyPair << endl;
    cout << endl;

    const string message = "Asymmetric cryptography";

    const SignedMessage signedMessage = Elgamal::sign(message, keyPair);
    SignedMessage signedMessageWithCorruptedMessage = signedMessage;
    signedMessageWithCorruptedMessage.getMessage()[1] = 'h';
    SignedMessage signedMessageWithCorruptedSignature = signedMessage;
    signedMessageWithCorruptedSignature.getSignature().getR() = 96;

    cout << "1. Digital signature." << endl;
    cout << "Signed message: " << signedMessage.getMessage() << endl;
    cout << "Signature: " << signedMessage.getSignature() << endl;

    cout
        << "Signature verification for correct message and correct signature: "
        << (Elgamal::verify(signedMessage, keyPair.getPublicKey())
            ? SUCCESSFUL_MESSAGE : FAILURE_MESSAGE)
        << endl;
    cout
        << "Signature verification"
            " for corrupted message and correct signature: "
        << (Elgamal::verify(
            signedMessageWithCorruptedMessage, keyPair.getPublicKey()
        )
            ? SUCCESSFUL_MESSAGE : FAILURE_MESSAGE)
        << endl;
    cout
        << "Signature verification"
            " for correct message and corrupted signature: "
        << (Elgamal::verify(
            signedMessageWithCorruptedSignature, keyPair.getPublicKey()
        )
            ? SUCCESSFUL_MESSAGE : FAILURE_MESSAGE)
        << endl;
    cout << endl;

    const CipherText cipherText = Elgamal::encrypt(
        message, keyPair.getPublicKey()
    );
    const Data decryptedData = Elgamal::decrypt(cipherText, keyPair);
    const string decryptedMessage(decryptedData.begin(), decryptedData.end());

    cout << "2. Encryption and decryption." << endl;
    cout << "Message: " << message << endl;
    cout << "Cipher text: " << cipherText << endl;
    cout << "Decrypted message: " << decryptedMessage << endl;

    cout
        << (message == decryptedMessage ? SUCCESSFUL_MESSAGE : FAILURE_MESSAGE)
        << endl;

    return 0;
}
