#ifndef CANARY_PROTECTION_H
#define CANARY_PROTECTION_H

#include <stdlib.h>
#include <stdint.h>

typedef enum CanaryProtectionState
{
    CanaryProtectionState_OK        = 0,
    CanaryProtectionState_CORRUPTED = 1,
} CanaryProtectionState;

static const size_t SIZE_OF_CANARY = 8;

void freeCanary(void* address);

void* canaryCalloc(size_t   size_called,
                   size_t   size_of_element,
                   uint8_t* canary_start,
                   uint8_t* canary_end);

void* canaryRealloc(void*    address,
                    size_t   original_size,
                    size_t   called_size,
                    uint8_t* data_canary_start,
                    uint8_t* data_canary_end);

void generateCanary(uint8_t* canary);

CanaryProtectionState checkDataCanaries(void*    data,
                                        size_t   size_of_data,
                                        uint8_t* canary_start,
                                        uint8_t* canary_end);

#endif // CANARY_PROTECTION_H
