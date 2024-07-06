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
    explicit AdrenoSampler(const AdrenoGPU& gpu, std::unique_ptr<Handle> handle);
    ~AdrenoSampler() override = default;

    std::error_code start() override;
    std::error_code stop() override;
    void setCounters(const std::unordered_set<uint32_t>& counters) override;

protected:
    const AdrenoGPU m_gpu;
    std::unique_ptr<Handle> m_handle = nullptr;
    std::unordered_set<uint32_t> m_counters{};
};

} // namespace adreno
} // namespace backend
} // namespace hpc