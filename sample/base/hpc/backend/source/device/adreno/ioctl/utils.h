#pragma once

#include "a6xx.h"
#include "types.h"
#include <stdint.h>

namespace hpc
{
namespace backend
{

inline AdrenoSeries getSeries(int gpu_id)
{
    if ((gpu_id >= 600 && gpu_id < 700) || gpu_id == 702)
        return AdrenoSeries::HPC_GPU_ADRENO_SERIES_A6XX;
    if (gpu_id >= 500 && gpu_id < 600)
        return AdrenoSeries::HPC_GPU_ADRENO_SERIES_A5XX;
    return AdrenoSeries::HPC_GPU_ADRENO_SERIES_UNKNOWN;
}

inline uint32_t getGroup(adreno_a6xx_counter counter)
{
    return static_cast<uint32_t>(counter) >> 8u;
}

inline uint32_t getSelector(adreno_a6xx_counter counter)
{
    return static_cast<uint32_t>(counter) & (256u - 1u);
}

} // namespace backend
} // namespace hpc
