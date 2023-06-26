#ifndef UTILITIES_H_INCLUDED
#define UTILITIES_H_INCLUDED

#include <random>

using namespace std;

namespace Utilities {
    int generateIntegerInRange(int min, int max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> distr(min, max);

        return distr(gen);
    }
};

#endif // UTILITIES_H_INCLUDED
