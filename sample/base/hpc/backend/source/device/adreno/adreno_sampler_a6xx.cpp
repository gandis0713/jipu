#include "adreno_sampler_a6xx.h"

namespace hpc
{
namespace backend
{
namespace adreno
{

AdrenoSamplerA6XX::AdrenoSamplerA6XX(const AdrenoGPU& gpu, std::unique_ptr<Handle> handle)
    : AdrenoSampler(gpu, std::move(handle))
{
}

} // namespace adreno
} // namespace backend
} // namespace hpc