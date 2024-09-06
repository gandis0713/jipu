#include "webgpu_sampler.h"

namespace jipu
{

WebGPUSampler* create(WebGPUDevice* device, WGPUSamplerDescriptor const* descriptor)
{
    return new WebGPUSampler(device, nullptr, descriptor);
}

WebGPUSampler::WebGPUSampler(WebGPUDevice* device, std::unique_ptr<Sampler> sampler, WGPUSamplerDescriptor const* descriptor)
    : m_wgpuDevice(device)
    , m_descriptor(*descriptor)
    , m_sampler(std::move(sampler))
{
}

} // namespace jipu