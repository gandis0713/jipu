#include "adreno_gpu.h"

#include "adreno_instance.h"
#include "adreno_sampler.h"

#include <spdlog/spdlog.h>

namespace hpc
{
namespace adreno
{

AdrenoGPU::AdrenoGPU(AdrenoInstance& instance, std::unique_ptr<hpc::backend::GPU> gpu)
    : m_instance(instance)
    , m_gpu(std::move(gpu))
{
}

std::unique_ptr<Sampler> AdrenoGPU::create(const SamplerDescriptor& descriptor)
{
    std::vector<uint32_t> counters{};
    for (const auto& counter : descriptor.counters)
    {
        const auto& adrenoCounter = counterDependencies.at(counter);
        // TODO: set counter correctly...
        counters.push_back(static_cast<uint32_t>(adrenoCounter[0]));
    }

    auto sampler = m_gpu->createSampler({ counters });
    if (!sampler)
    {
        spdlog::error("Failed to create sampler");
        return nullptr;
    }

    return std::make_unique<AdrenoSampler>(*this, std::move(sampler));
}

const std::unordered_set<Counter> AdrenoGPU::counters() const
{
    std::unordered_set<Counter> counters{};

    for (const auto& [counter, adrenoCounter] : counterDependencies)
    {
        // TODO: generate counter from available adreno counters.
        counters.insert(counter);
    }

    return counters;
}

} // namespace adreno
} // namespace hpc