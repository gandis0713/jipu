#pragma once

#include "handle.h"
#include "syscall/interface.h"

#include "a6xx.h"
#include "types.h"
#include <stdint.h>

namespace hpc
{
namespace backend
{

inline int getGPUId(int chipId)
{
    uint8_t coreId = (chipId >> (8 * 3)) & 0xffu;
    uint8_t majorId = (chipId >> (8 * 2)) & 0xffu;
    uint8_t minorId = (chipId >> (8 * 1)) & 0xffu;
    auto gpuId = coreId * 100 + majorId * 10 + minorId;
}

inline AdrenoSeries getSeries(int gpuId)
{
    if ((gpuId >= 600 && gpuId < 700) || gpuId == 702)
        return AdrenoSeries::HPC_GPU_ADRENO_SERIES_A6XX;
    if (gpuId >= 500 && gpuId < 600)
        return AdrenoSeries::HPC_GPU_ADRENO_SERIES_A5XX;
    return AdrenoSeries::HPC_GPU_ADRENO_SERIES_UNKNOWN;
}

inline AdrenoSeries getSeries(Handle& handle)
{
    adreno_device_info devinfo{};

    adreno_device_get_property deviceGetProperty{};
    deviceGetProperty.type = ADRENO_PROPERTY_DEVICE_INFO;
    deviceGetProperty.value = &devinfo;
    deviceGetProperty.num_bytes = sizeof(adreno_device_info);

    auto result = syscall::Interface::ioctl(handle.fd(), ADRENO_IOCTL_DEVICE_GET_PROPERTY, &deviceGetProperty);
    auto error = result.first;
    if (error)
    {
        return AdrenoSeries::HPC_GPU_ADRENO_SERIES_UNKNOWN;
    }

    return getSeries(getGPUId(devinfo.chip_id));
}

inline uint32_t getGroup(uint32_t counter)
{
    return counter >> 8u;
}

inline uint32_t getSelector(uint32_t counter)
{
    return counter & (256u - 1u);
}

} // namespace backend
} // namespace hpc
