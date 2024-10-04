#include "hash_protection.h"

#include <stdint.h>


hash_type calculateHash(const void* data, const size_t size) {
    hash_type hash = 0;
    const uint8_t* bytes = (const uint8_t*)data;

    for (size_t i = 0; i < size; i++) {
        hash = hash * 31 + bytes[i];
    }

    return hash;
}

HashProtectionState checkDataHash(const hash_type hash_given, const void* data, const size_t size)
{
    if (calculateHash(data, size) != hash_given)
    {
        return HashProtectionState_CORRUPTED;
    }

    return HashProtectionState_OK;
}




