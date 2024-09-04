#include "webgpu_device.h"

#include "webgpu_adapter.h"
#include "webgpu_queue.h"

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

Device* WebGPUDevice::getDevice() const
{
    return m_device.get();
}

} // namespace jipu