#ifndef CANARY_PROTECTION_H
#define CANARY_PROTECTION_H

#include <stdlib.h>
#include <stdint.h>

#ifdef _CANARY_PROTECT
#define generateCanaryRandom(pointer_canary) generateCanaryRandom_(pointer_canary);
#else
#define generateCanaryRandom(pointer_canary) ;
#endif

#ifdef _CANARY_PROTECT
#define generateCanaryStatic(pointer_canary) generateCanaryStatic__(pointer_canary);
#else
#define generateCanaryStatic(pointer_canary) ;
#endif

#ifdef _CANARY_PROTECT
#define checkDataCanaries(data, size_of_data, canary_start, canary_end) \
    checkDataCanaries_(data, size_of_data, canary_start, canary_end);
#else
#define checkDataCanaries(data, size_of_data, canary_start, canary_end) CanaryProtectionState_OK;
#endif

#ifdef _CANARY_PROTECT
#define checkStructCanaries(canary_start, canary_end) \
    checkStructCanaries_(canary_start, canary_end);
#else
#define checkStructCanaries(canary_start, canary_end) CanaryProtectionState_OK;
#endif

#ifdef _CANARY_PROTECT
#define canaryCalloc(start_size, size_of_element, data_canary_start, data_canary_end) \
    canaryCalloc_(start_size, size_of_element, data_canary_start, data_canary_end);
#else
#define canaryCalloc(start_size, size_of_element, data_canary_start, data_canary_end) \
    calloc(start_size, size_of_element);
#endif

#ifdef _CANARY_PROTECT
#define canaryRealloc(start, original_size, size_need, data_canary_start, data_canary_end) \
    canaryRealloc_(start, original_size, size_need, data_canary_start, data_canary_end);
#else
#define canaryRealloc(start, original_size, size_need, data_canary_start, data_canary_end) \
    realloc(start, size_need);
#endif

#ifdef _CANARY_PROTECT
#define freeCanary(pointer) \
    freeCanary_(pointer);
#else
#define freeCanary(pointer) \
    free(pointer);
#endif

typedef enum CanaryProtectionState
{
    CanaryProtectionState_OK        = 0,
    CanaryProtectionState_CORRUPTED = 1,
} CanaryProtectionState;

static const size_t SIZE_OF_CANARY = 8;

void freeCanary_(void* address);

void* canaryCalloc_(size_t   size_called,
                    size_t   size_of_element,
                    uint8_t* canary_start,
                    uint8_t* canary_end);

void* canaryRealloc_(void*    address,
                     size_t   original_size,
                     size_t   called_size,
                     uint8_t* data_canary_start,
                     uint8_t* data_canary_end);

void generateCanaryRandom_(uint8_t* canary);
void generateCanaryStatic_(uint8_t* canary);

CanaryProtectionState checkDataCanaries_(void*    data,
                                         size_t   size_of_data,
                                         uint8_t* canary_start,
                                         uint8_t* canary_end);

CanaryProtectionState checkStructCanaries_(uint8_t* canary_start,
                                           uint8_t* canary_end);

#endif // CANARY_PROTECTION_H
