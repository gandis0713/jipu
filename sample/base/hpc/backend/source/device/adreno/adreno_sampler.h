#pragma once

#include "hpc/backend/sampler.h"

#include "adreno_gpu.h"

namespace hpc
{
namespace backend
{
namespace adreno
{

class AdrenoSampler final : public Sampler
{
public:
    explicit AdrenoSampler(const AdrenoGPU& gpu);
    ~AdrenoSampler() override = default;

private:
    const AdrenoGPU m_gpu;
};

} // namespace adreno
} // namespace backend
} // namespace hpc