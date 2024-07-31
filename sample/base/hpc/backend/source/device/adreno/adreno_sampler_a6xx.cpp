#include "adreno_sampler_a6xx.h"

namespace hpc
{
namespace backend
{
namespace adreno
{

AdrenoSamplerA6XX::AdrenoSamplerA6XX(const AdrenoGPU& gpu, std::unique_ptr<Handle> handle, const SamplerDescriptor& descriptor)
    : AdrenoSampler(gpu, std::move(handle), descriptor)
{
}

uint32_t AdrenoSamplerA6XX::getGroup(uint32_t counter)
{
    return counter >> 8u;
}

uint32_t AdrenoSamplerA6XX::getSelector(Counter counter)
{
    return counter & (256u - 1u);
}

Counter AdrenoSamplerA6XX::getCounter(uint32_t group, uint32_t selector)
{
    return (group << 8) | selector;
}

} // namespace adreno
} // namespace backend
} // namespace hpc