#pragma once

#include "cast.h"
#include "hpc/sampler.h"

#include "adreno_gpu.h"

#include <hwcpipe/sampler.hpp>

namespace hpc
{
namespace adreno
{

class AdrenoSampler final : public Sampler
{
public:
    explicit AdrenoSampler(const AdrenoGPU gpu, const SamplerDescriptor& descriptor);

public:
    void start() override;
    void stop() override;
    std::vector<Sample> samples(std::unordered_set<Counter> counters = {}) override;

private:
    const AdrenoGPU m_gpu{};

private:
    SamplerDescriptor m_descriptor{};
};
DOWN_CAST(AdrenoSampler, Sampler);

} // namespace adreno
} // namespace hpc