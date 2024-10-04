#ifndef HASH_PROTECTION_H
#define HASH_PROTECTION_H

#include <stdlib.h>

typedef unsigned long long hash_type;

typedef enum HashProtectionState
{
    HashProtectionState_OK        = 0,
    HashProtectionState_CORRUPTED = 1,
} HashProtectionState;

hash_type calculateHash(const void* data, size_t size);
HashProtectionState checkDataHash(hash_type hash_given, const void* data, size_t size);

#endif // HASH_PROTECTION_H
