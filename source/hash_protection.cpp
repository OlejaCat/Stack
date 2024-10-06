#include "hash_protection.h"

#include <stdint.h>
#include <time.h>
#include <stdlib.h>

// static --------------------------------------------------------------------------------------------------------------

uint8_t HASH_FACTOR;

void initHash(void);

// public --------------------------------------------------------------------------------------------------------------

hash_type calculateHash(const void* data, const size_t size) {
    hash_type hash = 0;
    const uint8_t* bytes = (const uint8_t*)data;

    for (size_t i = 0; i < size; i++) {
        hash = hash * HASH_FACTOR + bytes[i];
    }

    return hash;
}

HashProtectionState checkHash(const hash_type hash_given, const void* data, const size_t size)
{
    if (calculateHash(data, size) != hash_given)
    {
        return HashProtectionState_CORRUPTED;
    }

    return HashProtectionState_OK;
}


// static --------------------------------------------------------------------------------------------------------------

void initHash(void)
{
    srand((unsigned int)time(0));

    HASH_FACTOR = (uint8_t)(rand() % 64);
}



