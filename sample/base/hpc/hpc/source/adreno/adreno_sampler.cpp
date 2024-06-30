#include "adreno_sampler.h"

#include "adreno_counter.h"

#include <spdlog/spdlog.h>
#include <stdexcept>

namespace hpc
{
namespace adreno
{

AdrenoSampler::AdrenoSampler(const AdrenoGPU gpu, const SamplerDescriptor& descriptor)
    : Sampler()
    , m_gpu(gpu)
    , m_descriptor(descriptor)
{
}

void AdrenoSampler::start()
{
    spdlog::debug("start sampling {}", __func__);
}

void AdrenoSampler::stop()
{
    spdlog::debug("stop sampling {}", __func__);
}

std::vector<Sample> AdrenoSampler::samples(std::unordered_set<Counter> counters)
{
    std::vector<hpc::Sample> samples{};
    for (const auto counter : counters)
    {
        // TODO
    }

    return samples;
}

} // namespace adreno
} // namespace hpc