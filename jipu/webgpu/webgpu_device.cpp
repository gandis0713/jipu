#include "webgpu_device.h"

#include "webgpu_adapter.h"
#include "webgpu_bind_group.h"
#include "webgpu_bind_group_layout.h"
#include "webgpu_buffer.h"
#include "webgpu_command_encoder.h"
#include "webgpu_pipeline_layout.h"
#include "webgpu_queue.h"
#include "webgpu_render_pipeline.h"
#include "webgpu_shader_module.h"
#include "webgpu_texture.h"

namespace jipu
{

WebGPUDevice* WebGPUDevice::create(WebGPUAdapter* wgpuAdapter, WGPUDeviceDescriptor const* descriptor)
{
    WGPUDeviceDescriptor wgpuDescriptor = descriptor ? *descriptor : GenerateWGPUDeviceDescriptor(wgpuAdapter);

    auto physicalDevice = wgpuAdapter->getPhysicalDevice();
    auto device = physicalDevice->createDevice(DeviceDescriptor{});
    return new WebGPUDevice(wgpuAdapter, std::move(device), &wgpuDescriptor);
}

WebGPUDevice::WebGPUDevice(WebGPUAdapter* wgpuAdapter, std::unique_ptr<Device> device, WGPUDeviceDescriptor const* descriptor)
    : m_wgpuAdapter(wgpuAdapter)
    , m_wgpuQueue(nullptr)
    , m_descriptor(*descriptor)
    , m_device(std::move(device))
{
}

WebGPUQueue* WebGPUDevice::getQueue()
{
    if (!m_wgpuQueue)
    {
        m_wgpuQueue = WebGPUQueue::create(this, &m_descriptor.defaultQueue);
    }

    return m_wgpuQueue;
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

WebGPUTexture* WebGPUDevice::createTexture(Texture* texture)
{
    return WebGPUTexture::create(this, texture);
}

WebGPUTexture* WebGPUDevice::createTexture(WGPUTextureDescriptor const* descriptor)
{
    return WebGPUTexture::create(this, descriptor);
}

WebGPUBuffer* WebGPUDevice::createBuffer(WGPUBufferDescriptor const* descriptor)
{
    return WebGPUBuffer::create(this, descriptor);
}

WebGPUCommandEncoder* WebGPUDevice::createCommandEncoder(WGPUCommandEncoderDescriptor const* descriptor)
{
    return WebGPUCommandEncoder::create(this, descriptor);
}

Device* WebGPUDevice::getDevice() const
{
    return m_device.get();
}

// Generators
WGPUDeviceDescriptor GenerateWGPUDeviceDescriptor(WebGPUAdapter* wgpuAdapter)
{
    WGPUDeviceDescriptor descriptor{};
    // TODO
    return descriptor;
}

} // namespace jipu