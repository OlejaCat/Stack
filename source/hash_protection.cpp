#include "hash_protection.h"

#include <stdint.h>
#include <time.h>
#include <stdlib.h>

#include "logger.h"

// static --------------------------------------------------------------------------------------------------------------

uint HASH_FACTOR = 31;

// public --------------------------------------------------------------------------------------------------------------


hash_type calculateHash(const void* data, const size_t size) {
    Log(LogLevel_INFO, "HASH_FACTOR: %c", HASH_FACTOR);

    hash_type hash = 0;
    const uint8_t* bytes = (const uint8_t*)data;

    for (size_t i = 0; i < size; i++) {
        hash = hash * HASH_FACTOR + bytes[i];
        Log(LogLevel_INFO, "Hash: %llu", hash);
    }

    Log(LogLevel_INFO, "Hash: %llu", hash);

    return hash;
}


HashProtectionState checkHash(const hash_type hash_given, const void* data, const size_t size)
{
    Log(LogLevel_INFO, "size given: %lu, hash_given: %llu, data current: %llu", size, hash_given, calculateHash(data, size));

    if (calculateHash(data, size) != hash_given)
    {
        return HashProtectionState_CORRUPTED;
    }

    return HashProtectionState_OK;
}


// static --------------------------------------------------------------------------------------------------------------



