#pragma once

#include "cast.h"
#include "hpc/sampler.h"

#include "adreno_gpu.h"

namespace hpc
{
namespace adreno
{

class AdrenoSampler final : public Sampler
{
public:
    explicit AdrenoSampler(AdrenoGPU& gpu, std::unique_ptr<hpc::backend::Sampler> sampler, const SamplerDescriptor& descriptor);

public:
    std::error_code start() override;
    std::error_code stop() override;
    std::vector<Sample> samples(std::unordered_set<Counter> counters = {}) override;

private:
    AdrenoGPU& m_gpu;
    std::unique_ptr<hpc::backend::Sampler> m_sampler = nullptr;

private:
    const SamplerDescriptor m_descriptor{};
    std::vector<std::unordered_map<hpc::backend::Counter, hpc::backend::Sample>> m_samples{};
};
DOWN_CAST(AdrenoSampler, Sampler);

} // namespace adreno
} // namespace hpc