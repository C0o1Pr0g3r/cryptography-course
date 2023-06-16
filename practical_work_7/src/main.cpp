#include <iostream>
#include "big-int.h"
#include "elgamal.h"

using namespace std;

const string SUCCESSFUL_MESSAGE = "Test passed";
const string FAILURE_MESSAGE = "Test failed";

int main() {
    const KeyPair keyPair = Elgamal::generateKeyPair();

    const Data message {{5, 29, 98, 237, 124}};

    SignedData<Data> signedMessage = Elgamal::sign(message, keyPair);

    cout << "Message: ";
    for (const auto& elem: message) {
        cout << static_cast<int>(elem) << " ";
    }
    cout << endl;

    cout << "Signature verification with holistic data: "
        << (Elgamal::verify(signedMessage, keyPair.getPublicKey())
            ? SUCCESSFUL_MESSAGE
            : FAILURE_MESSAGE)
        << endl;

    signedMessage.getMessage()[0] = 97;

    cout << "Corrupted message: ";
    for (const auto& elem: signedMessage.getMessage()) {
        cout << static_cast<int>(elem) << " ";
    }
    cout << endl;

    cout << "Signature verification with corrupted data: "
        << (Elgamal::verify(signedMessage, keyPair.getPublicKey())
            ? SUCCESSFUL_MESSAGE
            : FAILURE_MESSAGE)
        << endl;

    const Data encryptedMessage = Elgamal::encrypt(message, keyPair.getPublicKey());
    const Data decryptedMessage = Elgamal::decrypt(encryptedMessage, keyPair);



    cout << "Encrypted message: ";
    for (const auto& elem: encryptedMessage) {
        cout << static_cast<int>(elem) << " ";
    }
    cout << endl;

    cout << "Decrypted message: ";
    for (const auto& elem: decryptedMessage) {
        cout << static_cast<int>(elem) << " ";
    }
    cout << endl;

    cout << "Checking encryption and decryption: "
        << (message == decryptedMessage
            ? SUCCESSFUL_MESSAGE
            : FAILURE_MESSAGE)
        << endl;

    return 0;
}
