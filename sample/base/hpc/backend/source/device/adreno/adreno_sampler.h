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

protected:
    std::error_code activeCounters();

protected:
    virtual uint32_t getGroup(uint32_t counter) = 0;
    virtual uint32_t getSelector(uint32_t counter) = 0;

protected:
    const AdrenoGPU m_gpu;
    std::unique_ptr<Handle> m_handle = nullptr;
    const SamplerDescriptor m_descriptor{};
};

} // namespace adreno
} // namespace backend
} // namespace hpc