#pragma once

#include <stddef.h>
#include <stdint.h>

#define ADRENO_IOCTL_TYPE 0x09

#define ADRENO_PROPERTY_DEVICE_INFO 0x1

struct adreno_device_info
{
    unsigned int device_id;
    unsigned int chip_id;
    unsigned int mmu_enabled;
    unsigned long gmem_gpu_base_address;
    unsigned int gpu_id;
    size_t gmem_sizebytes;
};

struct adreno_device_get_property
{
    unsigned int type;
    void* value;
    size_t num_bytes;
};

struct adreno_perfcounter_query
{
    unsigned int groupid;
    /* Array to return the current countable for up to size counters */
    unsigned int* countables;
    unsigned int count;
    unsigned int max_counters;
    /* private: reserved for future use */
    unsigned int __pad[2]; /* For future binary compatibility */
};

#define ADRENO_IOCTL_PERFCOUNTER_QUERY \
    _IOWR(ADRENO_IOCTL_TYPE, 0x3A, struct adreno_perfcounter_query)

struct hpc_gpu_adreno_ioctl_counter_read_counter_t
{
    uint32_t group_id;
    uint32_t countable_selector;
    uint64_t value;
};

#define ADRENO_IOCTL_DEVICE_GET_PROPERTY \
    _IOWR(ADRENO_IOCTL_TYPE, 0x2, struct adreno_device_get_property)

struct adreno_counter_get
{
    unsigned int group_id;
    unsigned int countable_selector;
    unsigned int regster_offset_low;
    unsigned int regster_offset_high;
    unsigned int __pad;
};

#define ADRENO_IOCTL_COUNTER_GET \
    _IOWR(ADRENO_IOCTL_TYPE, 0x38, struct adreno_counter_get)

struct adreno_counter_put
{
    unsigned int group_id;
    unsigned int countable_selector;
    unsigned int __pad[2];
};

#define ADRENO_IOCTL_COUNTER_PUT \
    _IOW(ADRENO_IOCTL_TYPE, 0x39, struct adreno_counter_put)

struct adreno_counter_read
{
    hpc_gpu_adreno_ioctl_counter_read_counter_t* counters;
    unsigned int num_counters;
    unsigned int __pad[2];
};

#define ADRENO_IOCTL_COUNTER_READ \
    _IOWR(ADRENO_IOCTL_TYPE, 0x3B, struct adreno_counter_read)

/// Adreno GPU series.
enum class AdrenoSeries : uint32_t
{
    HPC_GPU_ADRENO_SERIES_UNKNOWN = 0,
    /// Adreno A6XX GPUs ((>= 600 && < 700) || == 702)
    HPC_GPU_ADRENO_SERIES_A6XX,
    /// Adreno A5XX GPUs (>= 500 && < 600)
    HPC_GPU_ADRENO_SERIES_A5XX,
};
