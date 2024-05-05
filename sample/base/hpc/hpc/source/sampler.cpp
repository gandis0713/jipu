#include "hpc/sampler.h"

namespace jipu
{
namespace hpc
{

Sampler::Sampler(SamplerDescriptor descriptor)
    : m_descriptor(descriptor)
{
}

const std::vector<Counter>& Sampler::counters() const
{
    return m_descriptor.counters;
}

} // namespace hpc
} // namespace jipu