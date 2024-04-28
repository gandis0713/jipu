#include "mali_counter.h"
#include "mali_sampler.h"

namespace jipu
{
namespace hpc
{
namespace mali
{

MaliCounter::MaliCounter(MaliGPU gpu)
    : Counter()
    , m_gpu(gpu)
{
}

Sampler::Ptr MaliCounter::create()
{
    return std::make_unique<MaliSampler>(*this);
}

} // namespace mali
} // namespace hpc
} // namespace jipu