#include "vkt/gpu/sampler.h"

namespace jipu
{

Sampler::Sampler(Device* device, const SamplerDescriptor& descriptor)
    : m_device(device)
{
}

} // namespace jipu