#include <iostream>
#include "big-int.h"
#include "elliptic-cryptography/diffie-hellman.h"

using namespace std;
using namespace EllipticCryptography;

const string SUCCESSFUL_MESSAGE = "Test passed";
const string FAILURE_MESSAGE = "Test failed";

bool areSharedSecretsEqual(const vector<Participant>& participants);
void test(vector<Participant>& participants);

int main() {
    Participant Alice = Participant("Alice", generateKeyPair());
    Participant Bob = Participant("Bob", generateKeyPair());
    Participant Carol = Participant("Carol", generateKeyPair());
    Participant Dan = Participant("Dan", generateKeyPair());
    Participant Evans = Participant("Evans", generateKeyPair());
    Participant Florence = Participant("Florence", generateKeyPair());
    Participant Gabriella = Participant("Gabriella", generateKeyPair());

    vector<Participant> participants {{
        Alice,
        Bob,
    }};

    cout << "Curve: " << BUILTIN_CURVE.getName() << endl;
    cout << "Curve base point: " << BUILTIN_CURVE.getBasePoint() << endl;
    cout << "Curve base point order: " << BUILTIN_CURVE.getBasePointOrder() << endl;
    cout << endl << endl;

    test(participants);
    cout << endl << endl;

    participants.push_back(Carol);
    participants.push_back(Dan);
    participants.push_back(Evans);
    participants.push_back(Florence);
    participants.push_back(Gabriella);

    test(participants);

    return 0;
}

bool areSharedSecretsEqual(const vector<Participant>& participants) {
    return all_of(
        participants.begin(),
        participants.end(),
        [&](const Participant& participant) {
            return participant.getSharedSecret() == participants.front().getSharedSecret();
        }
    );
}

void test(vector<Participant>& participants) {
    cout << "Elliptic curve Diffie-Hellman test for "
        << participants.size() << " participants" << endl << endl;
    for (const auto& participant : participants) {
        cout << participant.getName() << " key pair: "
            << participant.getKeyPair() << endl;
    }
    cout << endl;
    DiffieHellman::generateSharedSecretFor(participants);
    cout << (areSharedSecretsEqual(participants)
        ? SUCCESSFUL_MESSAGE : FAILURE_MESSAGE) << endl;
}
