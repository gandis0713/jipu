#include "webgpu_adapter.h"

#include "webgpu_device.h"
#include "webgpu_instance.h"

namespace jipu
{

WebGPUAdapter* WebGPUAdapter::create(WebGPUInstance* wgpuInstance, WGPU_NULLABLE WGPURequestAdapterOptions const* options)
{
    std::unique_ptr<Instance> instance = nullptr;
    switch (options->backendType)
    {
    case WGPUBackendType::WGPUBackendType_Vulkan: {
        instance = Instance::create({ .type = InstanceType::kVulkan });
    }
    break;
    default:
        // TODO: log error
        return nullptr;
    }

    // TODO: get correct physical device
    std::unique_ptr<PhysicalDevice> physicalDevice = std::move(instance->getPhysicalDevices()[0]);

    return new WebGPUAdapter(wgpuInstance, std::move(instance), std::move(physicalDevice), options);
}

WebGPUAdapter::WebGPUAdapter(WebGPUInstance* wgpuInstance, std::unique_ptr<Instance> instance, std::unique_ptr<PhysicalDevice> physicalDevice, WGPURequestAdapterOptions const* options)
    : m_wgpuInstance(wgpuInstance)
    , m_options(*options)
    , m_instance(std::move(instance))
    , m_physicalDevice(std::move(physicalDevice))
{
}

void WebGPUAdapter::requestDevice(WGPUDeviceDescriptor const* descriptor, WGPURequestDeviceCallback callback, void* userdata)
{
    // TODO: Correct the device creation logic.
    auto device = new WebGPUDevice(this, descriptor);
    callback(WGPURequestDeviceStatus::WGPURequestDeviceStatus_Success, reinterpret_cast<WGPUDevice>(device), "", userdata);
}

} // namespace jipu