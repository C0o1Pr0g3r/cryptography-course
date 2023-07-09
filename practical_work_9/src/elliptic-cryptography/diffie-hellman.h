#ifndef DIFFIE_HELLMAN_H_INCLUDED
#define DIFFIE_HELLMAN_H_INCLUDED

#include <ostream>
#include <iostream>
#include <vector>
#include "../big-int.h"
#include "curve.h"
#include "point.h"

namespace EllipticCryptography {
    const BuiltinCurve BUILTIN_CURVE = BuiltinCurve::getRandomBuiltinCurve();
    const Point BASE_POINT = BUILTIN_CURVE.getBasePoint();
    const BigInt BASE_POINT_ORDER = BUILTIN_CURVE.getBasePointOrder();

    using PrivateKey = BigInt;
    using PublicKey = Point;

    class KeyPair {
    private:
        friend ostream& operator<<(ostream& out, const KeyPair& keyPair);

    private:
        PrivateKey privateKey;
        PublicKey publicKey;

    public:
        KeyPair(const PrivateKey& privateKey, const PublicKey& publicKey)
        :   privateKey(privateKey), publicKey(publicKey)
        {}

        PrivateKey getPrivateKey() const {
            return this->privateKey;
        }

        PublicKey getPublicKey() const {
            return this->publicKey;
        }
    };

    ostream& operator<<(ostream& out, const KeyPair& keyPair) {
        out << "(" << "private: " << keyPair.privateKey << "; "
            << "public: " << keyPair.publicKey << ")";
        return out;
    }

    class Participant {
    private:
        friend class DiffieHellman;

    private:
        string name;
        KeyPair keyPair;
        BigInt sharedSecret;

    public:
        Participant(
            const string& name,
            const KeyPair& keyPair
        )
        :   keyPair(keyPair),
            sharedSecret(0),
            name(name)
        {}

        KeyPair getKeyPair() const {
            return this->keyPair;
        }

        BigInt getSharedSecret() const {
            return this->sharedSecret;
        }

        string getName() const {
            return this->name;
        }
    };

    KeyPair generateKeyPair() {
        const PrivateKey privateKey = BigInt::generateInRange(
            0, BASE_POINT_ORDER - 1
        );
        const PublicKey publicKey = privateKey * BASE_POINT;
        return KeyPair(privateKey, publicKey);
    }


    struct Expression {
    public:
        vector<string> variables;
        Point result;

        Expression() : variables(0), result(BUILTIN_CURVE.group) {}
        Expression(const vector<string>& participants, const Point& point)
        :   variables(participants),
            result(point)
        {}
    };

    ostream& operator<<(ostream& out, const Expression& expression) {
        for (int i = 0; i < expression.variables.size(); ++i) {
            out << expression.variables[i] << " * ";
        }
        out << "G = " << expression.result;
        return out;
    }

    class DiffieHellman {
    private:
        static bool enoughVariablesToGenerateSharedSecret(
            const int numberOfVariables,
            const int numberOfParticipants
        ) {
            return numberOfVariables == numberOfParticipants - 1;
        }

        static void computeSharedSecretForCurrentParticipant(
            vector<Expression>& expressions,
            Participant& currentParticipant
        ) {
            currentParticipant.sharedSecret = (
                currentParticipant.keyPair.getPrivateKey() * expressions[0].result
            ).getCoordinates().x;
            expressions.erase(expressions.begin(), expressions.begin() + 1);
            cout << currentParticipant.name
                << " generated a shared secret: "
                <<  currentParticipant.sharedSecret << endl << endl;
        }

        static void convertExpressionsForNextParticipant(
            vector<Expression>& expressions,
            const Participant& currentParticipant,
            const Participant& nextParticipant,
            const int numberOfParticipants,
            const int step
        ) {
            for (auto& expression : expressions) {
                expression.variables.push_back(currentParticipant.name);
                expression.result *= currentParticipant.keyPair.getPrivateKey();
            }

            if (step <= numberOfParticipants) {
                expressions.push_back(Expression(
                    vector<string>({{currentParticipant.name}}),
                    currentParticipant.keyPair.getPublicKey()
                ));
            }

            cout
                << currentParticipant.name
                << " sends " << nextParticipant.name
                << " the following expressions:" << endl;
            for (auto& expression : expressions) {
                cout << expression << endl;
            }
            cout << endl;
        }

    public:
        static void generateSharedSecretFor(vector<Participant>& participants) {
            vector<Expression> expressions;
            int cur = 0;
            int next = cur + 1;
            int numberOfGeneratedSharedSecrets = 0;
            int step = 1;

            while (true) {
                cout << step << ") " << endl;
                if (next >= participants.size()) {
                    next = 0;
                }

                if (expressions.size() > 0) {
                    if (enoughVariablesToGenerateSharedSecret(
                        expressions[0].variables.size(), participants.size())
                    ) {
                        computeSharedSecretForCurrentParticipant(
                            expressions, participants[cur]
                        );
                        ++numberOfGeneratedSharedSecrets;
                        if (numberOfGeneratedSharedSecrets >= participants.size()) {
                            break;
                        }
                    }
                }

                convertExpressionsForNextParticipant(
                    expressions,
                    participants[cur],
                    participants[next],
                    participants.size(),
                    step
                );

                ++cur;
                ++next;
                ++step;
                if (cur >= participants.size()) {
                    cur = 0;
                }
            }
        }
    };
}

#endif // DIFFIE_HELLMAN_H_INCLUDED
