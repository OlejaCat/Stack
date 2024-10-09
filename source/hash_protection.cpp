#include "hash_protection.h"

#include <stdint.h>
#include <time.h>
#include <stdlib.h>

#include "logger.h"


// static --------------------------------------------------------------------------------------------------------------

const uint8_t HASH_FACTOR = 31;

// public --------------------------------------------------------------------------------------------------------------


hash_type calculateHash_(const void* data, const size_t size) {
    hash_type hash = 0;
    const uint8_t* bytes = (const uint8_t*)data;

    for (size_t i = 0; i < size; i++) {
        hash = hash * HASH_FACTOR + bytes[i];
    }

    return hash;
}


HashProtectionState checkHash_(const hash_type hash_given, const void* data, const size_t size)
{
    Log(LogLevel_INFO, "Hash given: %llu, output hash: %llu, size_given: %lu", hash_given, calculateHash_(data, size), size);

    if (calculateHash_(data, size) != hash_given)
    {
        return HashProtectionState_CORRUPTED;
    }

    return HashProtectionState_OK;
}


// static --------------------------------------------------------------------------------------------------------------



