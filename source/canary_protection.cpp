#include "canary_protection.h"

#include <string.h>
#include <time.h>


// static --------------------------------------------------------------------------------------------------------------


uint8_t* getRawData(void* data);

const uint8_t CANARY[SIZE_OF_CANARY] = {'B', 'A', 'D', 'C', 'O', 'F', 'F', 'E'};


// public --------------------------------------------------------------------------------------------------------------


void* canaryCalloc(size_t   size_called,
                   size_t   size_of_element,
                   uint8_t* data_canary_start,
                   uint8_t* data_canary_end)
{
    size_t size_to_calloc = size_called * size_of_element + 2 * SIZE_OF_CANARY;

    uint8_t* pointer_to_start = (uint8_t*)calloc(size_to_calloc, sizeof(uint8_t));
    if (!pointer_to_start)
    {
        return NULL;
    }

    uint8_t* canary_start = pointer_to_start;
    uint8_t* canary_end   = pointer_to_start + size_to_calloc - SIZE_OF_CANARY;

    memcpy(canary_start, data_canary_start, SIZE_OF_CANARY);
    memcpy(canary_end,   data_canary_end,   SIZE_OF_CANARY);

    void* data_pointer = (void*)(pointer_to_start + SIZE_OF_CANARY);

    return data_pointer;
}


void* canaryRealloc(void*    address,
                    size_t   original_size,
                    size_t   called_size,
                    uint8_t* data_canary_start,
                    uint8_t* data_canary_end)
{
    uint8_t* original_start = (uint8_t*)address - SIZE_OF_CANARY;

    memset(original_start + original_size + SIZE_OF_CANARY, 0, SIZE_OF_CANARY);

    size_t new_size = called_size + 2 * SIZE_OF_CANARY;

    uint8_t* new_pointer = (uint8_t*)realloc(original_start, new_size);
    if (!new_pointer)
    {
        return NULL;
    }

    memcpy(new_pointer, data_canary_start, SIZE_OF_CANARY);
    memcpy(new_pointer + called_size + SIZE_OF_CANARY, data_canary_end, SIZE_OF_CANARY);

    void* data_pointer = (void*)(new_pointer + SIZE_OF_CANARY);

    return data_pointer;
}

void freeCanary(void* address)
{
    uint8_t* original_address = (uint8_t*)address - SIZE_OF_CANARY;

    free(original_address);
}


void generateCanaryRandom(uint8_t* canary)
{
    srand((unsigned int)time(0));

    for (size_t i = 0; i < SIZE_OF_CANARY; i++)
    {
        canary[i] = (uint8_t)(rand() % 256);
    }
}


void generateCanaryStatic(uint8_t* canary)
{
    for (size_t i = 0; i < SIZE_OF_CANARY; i++)
    {
        canary[i] = CANARY[i];
    }
}


CanaryProtectionState checkDataCanaries(void*    data,
                                        size_t   size_of_data,
                                        uint8_t* canary_start,
                                        uint8_t* canary_end)
{
    const uint8_t* raw_data = getRawData(data);

    if (memcmp(raw_data, canary_start, SIZE_OF_CANARY) != 0
     || memcmp(raw_data + size_of_data + SIZE_OF_CANARY, canary_end, SIZE_OF_CANARY) != 0)
    {
        return CanaryProtectionState_CORRUPTED;
    }

    return CanaryProtectionState_OK;
}

CanaryProtectionState checkStructCanaries(uint8_t* canary_start,
                                          uint8_t* canary_end)
{
    if (memcmp(canary_start, canary_end, SIZE_OF_CANARY) != 0)
    {
        return CanaryProtectionState_CORRUPTED;
    }

    return CanaryProtectionState_OK;
}


// static --------------------------------------------------------------------------------------------------------------


uint8_t* getRawData(void* data)
{
    return ((uint8_t*)data - SIZE_OF_CANARY);
}


