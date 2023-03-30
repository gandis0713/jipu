#pragma once

#include "gpu/adapter.h"
#include "vulkan_api.h"

namespace vkt
{
class VulkanDriver;
class VulkanAdapter : public Adapter
{
public:
    VulkanAdapter() = delete;
    VulkanAdapter(VulkanDriver* driver, AdapterDescriptor descriptor);
    ~VulkanAdapter() = default;

    std::unique_ptr<Device> createDevice(DeviceDescriptor descriptor) override;
    std::unique_ptr<Platform> createPlatform(PlatformDescriptor descriptor) override;

public:
    VkInstance getInstance() const;
    VkPhysicalDevice getPhysicalDevice() const;

private: // vulkan object
    VkPhysicalDevice m_physicalDevice{ nullptr };
};
} // namespace vkt
