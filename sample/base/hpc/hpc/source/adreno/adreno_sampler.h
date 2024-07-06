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
    explicit AdrenoSampler(AdrenoGPU& gpu, std::unique_ptr<hpc::backend::Sampler> sampler);

public:
    std::error_code start() override;
    std::error_code stop() override;
    std::vector<Sample> samples(std::unordered_set<Counter> counters = {}) override;

private:
    AdrenoGPU& m_gpu;
    std::unique_ptr<hpc::backend::Sampler> m_sampler = nullptr;
};
DOWN_CAST(AdrenoSampler, Sampler);

} // namespace adreno
} // namespace hpc