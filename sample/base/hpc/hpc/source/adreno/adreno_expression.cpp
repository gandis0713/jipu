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
    if (!samples.contains(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_FS_STAGE_WAVE_CYCLES)) ||
        !samples.contains(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_VS_STAGE_WAVE_CYCLES)))
    {
        return { .counter = counter,
                 .timestamp = 0,
                 .value = hpc::Sample::Value(0.0),
                 .type = hpc::Sample::Type::float64 };
    }

    auto a = samples.at(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_FS_STAGE_WAVE_CYCLES));
    auto b = samples.at(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_VS_STAGE_WAVE_CYCLES));

    hpc::Sample::Value value{ (static_cast<double>(b) / (a + b)) * 100.0 };
    return { .counter = counter,
             .timestamp = 0,
             .value = value,
             .type = hpc::Sample::Type::float64 };
}

hpc::Sample fragmentUtilization(const hpc::Counter& counter, const Samples& samples)
{
    if (!samples.contains(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_FS_STAGE_WAVE_CYCLES)) ||
        !samples.contains(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_VS_STAGE_WAVE_CYCLES)))
    {
        return { .counter = counter,
                 .timestamp = 0,
                 .value = hpc::Sample::Value(0.0),
                 .type = hpc::Sample::Type::float64 };
    }

    auto a = samples.at(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_FS_STAGE_WAVE_CYCLES));
    auto b = samples.at(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_VS_STAGE_WAVE_CYCLES));

    hpc::Sample::Value value{ (static_cast<double>(a) / (a + b)) * 100.0 };
    return { .counter = counter,
             .timestamp = 0,
             .value = value,
             .type = hpc::Sample::Type::float64 };
}

hpc::Sample convert(const hpc::Counter& counter, const Samples& samples)
{
    auto counters = convertCounter(counter);
    if (counters.size() != 1)
    {
        return { .counter = counter,
                 .timestamp = 0,
                 .value = hpc::Sample::Value(0.0),
                 .type = hpc::Sample::Type::float64 };
    }

    auto backendCounter = *counters.begin();
    if (!samples.contains(backendCounter))
    {
        return { .counter = counter,
                 .timestamp = 0,
                 .value = hpc::Sample::Value(0.0),
                 .type = hpc::Sample::Type::float64 };
    }

    hpc::Sample::Value value{ samples.at(backendCounter) };
    return { .counter = counter,
             .timestamp = 0,
             .value = value,
             .type = hpc::Sample::Type::float64 };
}

} // namespace expression
} // namespace adreno
} // namespace hpc