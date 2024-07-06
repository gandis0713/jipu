#include "adreno_sampler.h"

#include "adreno_counter.h"

#include <spdlog/spdlog.h>
#include <stdexcept>

namespace hpc
{
namespace adreno
{

AdrenoSampler::AdrenoSampler(AdrenoGPU& gpu, std::unique_ptr<hpc::backend::Sampler> sampler)
    : Sampler()
    , m_gpu(gpu)
    , m_sampler(std::move(sampler))
{
}

std::error_code AdrenoSampler::start()
{
    spdlog::debug("start sampling {}", __func__);

    m_sampler->start();

    return {};
}

std::error_code AdrenoSampler::stop()
{
    spdlog::debug("stop sampling {}", __func__);

    return {};
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