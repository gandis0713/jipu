#pragma once

#include "counter.h"
#include "mali_gpu.h"
#include "sampler.h"

namespace jipu
{
namespace hpc
{
namespace mali
{

class MaliCounter : public Counter
{
public:
    explicit MaliCounter(MaliGPU gpu);

public:
    Sampler::Ptr create() override;

private:
    const MaliGPU m_gpu;
};

} // namespace mali
} // namespace hpc
} // namespace jipu