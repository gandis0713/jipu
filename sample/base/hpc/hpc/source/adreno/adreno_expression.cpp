#include "adreno_expression.h"

#include "adreno_counter.h"

namespace hpc
{
namespace adreno
{
namespace expression
{

hpc::Sample nonFragmentUtilization(const hpc::Counter& counter, const Samples& src, const Samples& dst)
{
    if (!src.contains(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_FS_STAGE_FULL_ALU_INSTRUCTIONS)) ||
        !src.contains(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_VS_STAGE_FULL_ALU_INSTRUCTIONS)) ||
        !dst.contains(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_FS_STAGE_FULL_ALU_INSTRUCTIONS)) ||
        !dst.contains(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_VS_STAGE_FULL_ALU_INSTRUCTIONS)))
    {
        return { .counter = counter,
                 .timestamp = 0,
                 .value = hpc::Sample::Value(0.0),
                 .type = hpc::Sample::Type::float64 };
    }

    auto a = dst.at(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_FS_STAGE_FULL_ALU_INSTRUCTIONS)) - src.at(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_FS_STAGE_FULL_ALU_INSTRUCTIONS));
    auto b = dst.at(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_VS_STAGE_FULL_ALU_INSTRUCTIONS)) - src.at(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_VS_STAGE_FULL_ALU_INSTRUCTIONS));

    hpc::Sample::Value value{ (static_cast<double>(b) / (a + b)) * 100.0 };
    return { .counter = counter,
             .timestamp = 0,
             .value = value,
             .type = hpc::Sample::Type::float64 };
}

hpc::Sample fragmentUtilization(const hpc::Counter& counter, const Samples& src, const Samples& dst)
{
    if (!src.contains(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_FS_STAGE_FULL_ALU_INSTRUCTIONS)) ||
        !src.contains(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_VS_STAGE_FULL_ALU_INSTRUCTIONS)) ||
        !dst.contains(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_FS_STAGE_FULL_ALU_INSTRUCTIONS)) ||
        !dst.contains(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_VS_STAGE_FULL_ALU_INSTRUCTIONS)))
    {
        return { .counter = counter,
                 .timestamp = 0,
                 .value = hpc::Sample::Value(0.0),
                 .type = hpc::Sample::Type::float64 };
    }

    auto a = dst.at(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_FS_STAGE_FULL_ALU_INSTRUCTIONS)) - src.at(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_FS_STAGE_FULL_ALU_INSTRUCTIONS));
    auto b = dst.at(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_VS_STAGE_FULL_ALU_INSTRUCTIONS)) - src.at(static_cast<hpc::backend::Counter>(AdrenoCounterA6XX::A6XX_SP_VS_STAGE_FULL_ALU_INSTRUCTIONS));

    hpc::Sample::Value value{ (static_cast<double>(a) / (a + b)) * 100.0 };
    return { .counter = counter,
             .timestamp = 0,
             .value = value,
             .type = hpc::Sample::Type::float64 };
}

hpc::Sample convert(const hpc::Counter& counter, const Samples& src, const Samples& dst)
{
    auto counters = convertCounter(counter);
    if (counters.size() != 1)
    {
        return { .counter = counter,
                 .timestamp = 0,
                 .value = hpc::Sample::Value(uint64_t(0)),
                 .type = hpc::Sample::Type::uint64 };
    }

    auto backendCounter = *counters.begin();
    if (!src.contains(backendCounter) || !dst.contains(backendCounter))
    {
        return { .counter = counter,
                 .timestamp = 0,
                 .value = hpc::Sample::Value(uint64_t(0)),
                 .type = hpc::Sample::Type::uint64 };
    }

    hpc::Sample::Value value{ dst.at(backendCounter) - src.at(backendCounter) };
    return { .counter = counter,
             .timestamp = 0,
             .value = value,
             .type = hpc::Sample::Type::uint64 };
}

} // namespace expression
} // namespace adreno
} // namespace hpc