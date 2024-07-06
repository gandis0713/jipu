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

class AdrenoSampler final : public Sampler
{
public:
    explicit AdrenoSampler(const AdrenoGPU& gpu, std::unique_ptr<Handle> handle);
    ~AdrenoSampler() override = default;

    std::error_code start() override;
    std::error_code stop() override;
    void setCounters(const std::vector<uint32_t>& counters) override;

private:
    const AdrenoGPU m_gpu;
    std::unique_ptr<Handle> m_handle = nullptr;
};

} // namespace adreno
} // namespace backend
} // namespace hpc