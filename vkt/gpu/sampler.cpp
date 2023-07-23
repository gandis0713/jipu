#include "vkt/gpu/sampler.h"

namespace vkt
{

Sampler::Sampler(Device* device, const SamplerDescriptor& descriptor)
    : m_device(device)
{
}

} // namespace vkt