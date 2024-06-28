#include "hpc/sampler.h"

namespace hpc
{

Sampler::Sampler(SamplerDescriptor descriptor)
    : m_descriptor(descriptor)
{
}

const std::unordered_set<Counter>& Sampler::counters() const
{
    return m_descriptor.counters;
}

} // namespace hpc