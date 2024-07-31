#include "vulkan_adapter.h"

#include "vulkan_physical_device.h"

#include <thread>

namespace jipu
{

VulkanAdapter::VulkanAdapter(VulkanInstance& instance, const AdapterDescriptor& descriptor)
    : m_instance(instance)
    , m_descriptor(descriptor)
{
    // TODO: remove
    auto& instance1 = m_instance;
    auto& descriptor1 = m_descriptor;
}

AdapterInfo VulkanAdapter::wgpuGetInfo()
{
    return {};
}

void VulkanAdapter::wgpuRequestDevice(const DeviceDescriptor& descriptor, AdapterRequestDeviceCallback callback, void* userdata)
{
    auto t = std::thread([&]() {
        auto vulkanPhysicalDevice = downcast(m_descriptor.physicalDevice);
        vulkanPhysicalDevice.createDevice({});

        callback(WGPURequestDeviceStatus_Success, nullptr, "", userdata);
    });

    t.detach();
}

} // namespace jipu