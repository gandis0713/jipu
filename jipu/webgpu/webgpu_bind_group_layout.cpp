#include "webgpu_bind_group_layout.h"

namespace jipu
{

WebGPUBindGroupLayout* WebGPUBindGroupLayout::create(WebGPUDevice* device, WGPUBindGroupLayoutDescriptor const* descriptor)
{
    return nullptr;
}

WebGPUBindGroupLayout::WebGPUBindGroupLayout(WebGPUDevice* device, std::unique_ptr<BindingGroupLayout> layout, WGPUBindGroupLayoutDescriptor const* descriptor)
    : m_wgpuDevice(device)
    , m_descriptor(*descriptor)
    , m_layout(std::move(layout))
{
}

BindingGroupLayout* WebGPUBindGroupLayout::getBindingGroupLayout() const
{
    return m_layout.get();
}

} // namespace jipu