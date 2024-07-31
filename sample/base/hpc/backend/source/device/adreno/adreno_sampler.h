#pragma once

#include "hpc/backend/sampler.h"

#include "adreno_gpu.h"
#include "handle.h"

namespace hpc
{
namespace backend
{
namespace adreno
{

class AdrenoSampler : public Sampler
{
public:
    explicit AdrenoSampler(const AdrenoGPU& gpu, std::unique_ptr<Handle> handle, const SamplerDescriptor& descriptor);
    ~AdrenoSampler() override = default;

    std::error_code start() override;
    std::error_code stop() override;
    /**
     * @brief Sample the counter.
     * @param counter The counter to sample.
     * @return The sampled value. The key is counter, The value is the sampled value.
     */
    std::unordered_map<Counter, Sample> sample(std::unordered_set<Counter> counter) override;

protected:
    std::error_code activate();
    std::error_code deactivate();

protected:
    virtual uint32_t getGroup(uint32_t counter) = 0;
    virtual uint32_t getSelector(Counter counter) = 0;
    virtual Counter getCounter(uint32_t group, uint32_t selector) = 0;

protected:
    const AdrenoGPU m_gpu;
    std::unique_ptr<Handle> m_handle = nullptr;
    const SamplerDescriptor m_descriptor{};
};

} // namespace adreno
} // namespace backend
} // namespace hpc