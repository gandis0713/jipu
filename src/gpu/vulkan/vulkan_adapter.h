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
    ~VulkanAdapter() override;

    std::unique_ptr<Device> createDevice(DeviceDescriptor descriptor) override;
    std::unique_ptr<Platform> createPlatform(PlatformDescriptor descriptor) override;

public:
    VkInstance getInstance() const;
    VkPhysicalDevice getPhysicalDevice() const;

    std::vector<VkQueueFamilyProperties> getQueueFamilyProperties() const;

private: // vulkan object
    VkPhysicalDevice m_physicalDevice{ nullptr };
};
} // namespace vkt
