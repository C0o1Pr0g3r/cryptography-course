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


    class Expression {
    private:
        friend ostream& operator<<(ostream& out, const Expression& expression);

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
        friend class Curve;
        friend class Point;

    private:
        static bool generateSharedSecretIfThereIsEnoughData(
            vector<Expression>& expressions,
            vector<Participant>& participants,
            const int currentParticipantIndex
        ) {
            Participant& currentParticipant =
                participants[currentParticipantIndex];
            if (
                expressions.size() > 0
                &&
                expressions[0].variables.size() == participants.size() - 1
            ) {
                currentParticipant.sharedSecret = (
                    currentParticipant.getKeyPair().getPrivateKey()
                    *
                    expressions[0].result
                ).getCoordinates().x;
                expressions.erase(
                    expressions.begin(), expressions.begin() + 1
                );
                cout << currentParticipant.getName()
                    << " generated a shared secret: "
                    << currentParticipant.getSharedSecret() << endl << endl;
                return true;
            }
            return false;
        }

        static vector<Expression> calculateDataForNextParticipant(
            vector<Expression>& expressions,
            vector<Participant>& participants,
            const int currentParticipantIndex,
            const int nextParticipantIndex,
            const int step
        ) {
            for (auto& calculation: expressions) {
                calculation.variables.push_back(
                    participants[currentParticipantIndex].getName()
                );
                calculation.result = participants[currentParticipantIndex]
                    .getKeyPair().getPrivateKey() * calculation.result;
            }
            if (step < participants.size()) {
                expressions.push_back(Expression(vector<string>(
                    {{participants[currentParticipantIndex].getName()}}
                ),
                participants[currentParticipantIndex]
                    .getKeyPair().getPrivateKey() * BASE_POINT));
            }
            cout << step + 1 << ") "
                << participants[currentParticipantIndex].getName()
                << " sends " << participants[nextParticipantIndex].getName()
                << " the following intermediate calculations:" << endl;
            for (const auto& calculation: expressions) {
                cout << calculation << endl;
            }
            cout << endl;

            return expressions;
        }

    public:
        static void generateSharedSecret(vector<Participant>& participants) {
            vector<vector<Expression>> expressionsOfParticipants(
                participants.size()
            );
            int numberOfGeneratedSharedSecrets = 0;
            int step = 0;
            int currentParticipantIndex = 0;
            int nextParticipantIndex = 1;

            while (true) {
                if (nextParticipantIndex >= participants.size()) {
                    nextParticipantIndex = 0;
                }
                vector<Expression> expressions =
                    expressionsOfParticipants[currentParticipantIndex];

                if (generateSharedSecretIfThereIsEnoughData(
                    expressions, participants, currentParticipantIndex
                )) {
                    ++numberOfGeneratedSharedSecrets;
                }

                if (numberOfGeneratedSharedSecrets == participants.size()) {
                    break;
                }

                expressionsOfParticipants[
                    nextParticipantIndex
                ] = calculateDataForNextParticipant(
                    expressions,
                    participants,
                    currentParticipantIndex,
                    nextParticipantIndex,
                    step
                );

                ++step;
                ++currentParticipantIndex;
                ++nextParticipantIndex;
                if (currentParticipantIndex >= participants.size()) {
                    currentParticipantIndex = 0;
                }
            }
        }
    };
}

#endif // DIFFIE_HELLMAN_H_INCLUDED
