#ifndef HASH_PROTECTION_H
#define HASH_PROTECTION_H

#include <stdlib.h>

typedef unsigned long long hash_type;

typedef enum HashProtectionState
{
    HashProtectionState_OK        = 0,
    HashProtectionState_CORRUPTED = 1,
} HashProtectionState;

#ifndef NDEBUG
#define checkHash(hash_given, data, size) checkHash_(hash_given, data, size);
#else
#define checkHash(hash_given, data, size) HasHashProtectionState_OK;
#endif

#ifndef NDEBUG
#define calculateHash(data, size) calculateHash_(data, size);
#else
#define calculateHash(data, size) 0;
#endif

hash_type calculateHash_(const void* data, size_t size);
HashProtectionState checkHash_(hash_type hash_given, const void* data, size_t size);

#endif // HASH_PROTECTION_H
