#ifndef SHARED_H_INCLUDED
#define SHARED_H_INCLUDED

#include <openssl/ec.h>
#include "../definitions.h"
#include "../big-int.h"

bool areEqual(const EC_GROUP* a, const EC_GROUP* b) {
    const int result = EC_GROUP_cmp(a, b, BigInt::Context().data);
    if (result == -1) {
        throw runtime_error(OPERATION_FAILED);
    }
    return !result;
}

#endif // SHARED_H_INCLUDED
