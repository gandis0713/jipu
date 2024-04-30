#pragma once

#include "cast.h"
#include "sampler.h"

#include "mali_gpu.h"

namespace jipu
{
namespace hpc
{
namespace mali
{

class MaliSampler final : public Sampler
{
public:
    explicit MaliSampler(MaliGPU gpu, SamplerDescriptor descriptor);

public:
    Sample getSample(Counter counter) override;

private:
    const MaliGPU m_gpu{ -1 };
};
DOWN_CAST(MaliSampler, Sampler);

} // namespace mali
} // namespace hpc
} // namespace jipu