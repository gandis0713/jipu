#include "adreno_expression.h"

#include "adreno_counter.h"

namespace hpc
{
namespace adreno
{
namespace expression
{

hpc::Sample nonFragmentUtilization(const hpc::Counter& counter, const Samples& samples)
{
    auto a = samples.at(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::HPC_GPU_ADRENO_A6XX_SP_FS_STAGE_WAVE_CYCLES));
    auto b = samples.at(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::HPC_GPU_ADRENO_A6XX_SP_VS_STAGE_WAVE_CYCLES));

    hpc::Sample::Value value{ (static_cast<double>(b) / (a + b)) * 100.0 };
    return { .counter = counter,
             .timestamp = 0,
             .value = value,
             .type = Sample::Type::float64 };
}

hpc::Sample fragmentUtilization(const hpc::Counter& counter, const Samples& samples)
{
    auto a = samples.at(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::HPC_GPU_ADRENO_A6XX_SP_FS_STAGE_WAVE_CYCLES));
    auto b = samples.at(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::HPC_GPU_ADRENO_A6XX_SP_VS_STAGE_WAVE_CYCLES));

    hpc::Sample::Value value{ (static_cast<double>(a) / (a + b)) * 100.0 };
    return { .counter = counter,
             .timestamp = 0,
             .value = value,
             .type = Sample::Type::float64 };
}

} // namespace expression
} // namespace adreno
} // namespace hpc