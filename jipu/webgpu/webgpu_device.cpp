#include "webgpu_device.h"

#include "webgpu_adapter.h"
#include "webgpu_bind_group.h"
#include "webgpu_bind_group_layout.h"
#include "webgpu_pipeline_layout.h"
#include "webgpu_queue.h"
#include "webgpu_render_pipeline.h"
#include "webgpu_shader_module.h"

namespace jipu
{

WebGPUDevice* WebGPUDevice::create(WebGPUAdapter* wgpuAdapter, WGPUDeviceDescriptor const* wgpuDescriptor)
{
    auto physicalDevice = wgpuAdapter->getPhysicalDevice();
    auto device = physicalDevice->createDevice(DeviceDescriptor{});
    return new WebGPUDevice(wgpuAdapter, std::move(device), wgpuDescriptor);
}

WebGPUDevice::WebGPUDevice(WebGPUAdapter* wgpuAdapter, std::unique_ptr<Device> device)
    : m_wgpuAdapter(wgpuAdapter)
    , m_descriptor({})
    , m_device(std::move(device))
{
}

WebGPUDevice::WebGPUDevice(WebGPUAdapter* wgpuAdapter, std::unique_ptr<Device> device, WGPUDeviceDescriptor const* descriptor)
    : m_wgpuAdapter(wgpuAdapter)
    , m_descriptor(*descriptor)
    , m_device(std::move(device))
{
}

WebGPUQueue* WebGPUDevice::getQueue()
{
    return WebGPUQueue::create(this);
}

WebGPUBindGroup* WebGPUDevice::createBindGroup(WGPUBindGroupDescriptor const* descriptor)
{
    return WebGPUBindGroup::create(this, descriptor);
}

WebGPUBindGroupLayout* WebGPUDevice::createBindGroupLayout(WGPUBindGroupLayoutDescriptor const* descriptor)
{
    return WebGPUBindGroupLayout::create(this, descriptor);
}

WebGPUPipelineLayout* WebGPUDevice::createPipelineLayout(WGPUPipelineLayoutDescriptor const* descriptor)
{
    return WebGPUPipelineLayout::create(this, descriptor);
}

WebGPURenderPipeline* WebGPUDevice::createRenderPipeline(WGPURenderPipelineDescriptor const* descriptor)
{
    return WebGPURenderPipeline::create(this, descriptor);
}

WebGPUShaderModule* WebGPUDevice::createShaderModule(WGPUShaderModuleDescriptor const* descriptor)
{
    return WebGPUShaderModule::create(this, descriptor);
}

Device* WebGPUDevice::getDevice() const
{
    return m_device.get();
}

} // namespace jipu