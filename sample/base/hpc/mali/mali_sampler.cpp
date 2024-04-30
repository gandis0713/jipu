#include "mali_sampler.h"

namespace jipu
{
namespace hpc
{
namespace mali
{

MaliSampler::MaliSampler(MaliGPU gpu, SamplerDescriptor descriptor)
    : Sampler(descriptor)
    , m_gpu(gpu)
{
}

Sample MaliSampler::getSample(Counter counter)
{
    // TODO
    return {};
}

} // namespace mali
} // namespace hpc
} // namespace jipu