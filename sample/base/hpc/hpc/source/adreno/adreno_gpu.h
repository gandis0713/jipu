#pragma once

#include "hpc/gpu.h"

#include "adreno_counter.h"

namespace hpc
{
namespace adreno
{

class AdrenoGPU final : public GPU
{
public:
    explicit AdrenoGPU();

public:
    /**
     * @brief create a sampler.
     */
    std::unique_ptr<Sampler> create(const SamplerDescriptor& descriptor) override;

    /**
     * @brief available counters.
     */
    const std::unordered_set<Counter> counters() const override;
};

} // namespace adreno
} // namespace hpc