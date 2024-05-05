#pragma once

#include "cast.h"
#include "hpc/sampler.h"

#include "mali_gpu.h"

#include <hwcpipe/sampler.hpp>

namespace hpc
{
namespace mali
{

class MaliSampler final : public Sampler
{
public:
    explicit MaliSampler(MaliGPU gpu, SamplerDescriptor descriptor);

public:
    void start() override;
    void stop() override;
    std::vector<Sample> samples(std::vector<Counter> counters = {}) override;
    Sample sample(const Counter counter);

private:
    uint64_t time();

private:
    const MaliGPU m_gpu{ -1 };
    hwcpipe::sampler<> m_sampler;
};
DOWN_CAST(MaliSampler, Sampler);

} // namespace mali
} // namespace hpc