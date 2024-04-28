#include "mali_sampler.h"

namespace jipu
{
namespace hpc
{
namespace mali
{

MaliSampler::MaliSampler(MaliCounter counter)
    : Sampler()
    , m_counter(counter)
{
}

} // namespace mali
} // namespace hpc
} // namespace jipu