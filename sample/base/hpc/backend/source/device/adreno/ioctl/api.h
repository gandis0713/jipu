#pragma once

#include "types.h"

#include <stddef.h>
#include <stdint.h>

#define ADRENO_IOCTL_TYPE 0x09

#define ADRENO_PROPERTY_DEVICE_INFO 0x1

struct AdrenoDeviceInfo
{
    unsigned int device_id;
    unsigned int chip_id;
    unsigned int mmu_enabled;
    unsigned long gmem_gpu_base_address;
    unsigned int gpu_id;
    size_t gmem_sizebytes;
};

struct AdrenoDeviceGetProperty
{
    unsigned int type;
    void* value;
    size_t num_bytes;
};

struct AdrenoPerfcounterQuery
{
    unsigned int groupid;
    /* Array to return the current countable for up to size counters */
    unsigned int* countables;
    unsigned int count;
    unsigned int max_counters;
    /* private: reserved for future use */
    unsigned int __pad[2]; /* For future binary compatibility */
};

struct AdrenoCounterGet
{
    unsigned int group_id;
    unsigned int countable_selector;
    unsigned int regster_offset_low;
    unsigned int regster_offset_high;
    unsigned int __pad;
};

struct AdrenoCounterPut
{
    unsigned int group_id;
    unsigned int countable_selector;
    unsigned int __pad[2];
};

struct AdrenoIoctlCounterReadCounter
{
    uint32_t group_id;
    uint32_t countable_selector;
    uint64_t value;
};

struct AdrenoCounterRead
{
    AdrenoIoctlCounterReadCounter* counters;
    unsigned int num_counters;
    unsigned int __pad[2];
};

#define ADRENO_IOCTL_DEVICE_GET_PROPERTY \
    _IOWR(ADRENO_IOCTL_TYPE, 0x2, struct AdrenoDeviceGetProperty)
#define ADRENO_IOCTL_PERFCOUNTER_QUERY \
    _IOWR(ADRENO_IOCTL_TYPE, 0x3A, struct AdrenoPerfcounterQuery)
#define ADRENO_IOCTL_COUNTER_GET \
    _IOWR(ADRENO_IOCTL_TYPE, 0x38, struct AdrenoCounterGet)
#define ADRENO_IOCTL_COUNTER_PUT \
    _IOW(ADRENO_IOCTL_TYPE, 0x39, struct AdrenoCounterPut)
#define ADRENO_IOCTL_COUNTER_READ \
    _IOWR(ADRENO_IOCTL_TYPE, 0x3B, struct AdrenoCounterRead)