#include "adreno_gpu.h"

#include "adreno_sampler.h"

#include <spdlog/spdlog.h>

namespace hpc
{
namespace adreno
{

AdrenoGPU::AdrenoGPU()
{
}

std::unique_ptr<Sampler> AdrenoGPU::create(const SamplerDescriptor& descriptor)
{
    return std::make_unique<AdrenoSampler>(*this, descriptor);
}

const std::unordered_set<Counter> AdrenoGPU::counters() const
{
    std::unordered_set<Counter> counters{};

    // TODO

    return counters;
}

} // namespace adreno
} // namespace hpc