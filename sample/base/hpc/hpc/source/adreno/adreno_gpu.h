#pragma once

#include "hpc/backend/gpu.h"
#include "hpc/gpu.h"

#include "adreno_counter.h"

namespace hpc
{
namespace adreno
{

class AdrenoInstance;
class AdrenoGPU final : public GPU
{
public:
    explicit AdrenoGPU(AdrenoInstance& instance, std::unique_ptr<hpc::backend::GPU> gpu);

public:
    /**
     * @brief create a sampler.
     */
    std::unique_ptr<Sampler> create(const SamplerDescriptor& descriptor) override;

    /**
     * @brief available counters.
     */
    const std::unordered_set<Counter> counters() const override;

private:
    AdrenoInstance& m_instance;
    std::unique_ptr<hpc::backend::GPU> m_gpu = nullptr;
};

} // namespace adreno
} // namespace hpc