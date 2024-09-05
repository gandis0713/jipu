#include "webgpu_bind_group.h"

namespace jipu
{

WebGPUBindGroup* WebGPUBindGroup::create(WebGPUDevice* device, WGPUBindGroupDescriptor const* descriptor)
{
    return nullptr;
}

WebGPUBindGroup::WebGPUBindGroup(WebGPUDevice* device, std::unique_ptr<BindingGroup> layout, WGPUBindGroupDescriptor const* descriptor)
    : m_wgpuDevice(device)
    , m_descriptor(*descriptor)
    , m_layout(std::move(layout))
{
}

BindingGroup* WebGPUBindGroup::getBindingGroup() const
{
    return m_layout.get();
}

} // namespace jipu