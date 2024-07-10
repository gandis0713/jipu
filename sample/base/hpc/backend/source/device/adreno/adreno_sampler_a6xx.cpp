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

uint32_t AdrenoSamplerA6XX::getSelector(uint32_t counter)
{
    return counter & (256u - 1u);
}

} // namespace adreno
} // namespace backend
} // namespace hpc