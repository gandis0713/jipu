#pragma once

#include "gpu/adapter.h"
#include "vulkan_api.h"

namespace vkt
{
class VulkanAdapter : public Adapter
{
public:
    VulkanAdapter() = delete;
    VulkanAdapter(Driver* driver, AdapterCreateInfo info);
    ~VulkanAdapter() = default;

    std::unique_ptr<Device> createDevice(DeviceCreateInfo info) override;
    std::unique_ptr<Platform> createPlatform(PlatformCreateInfo info) override;

public:
    VkInstance getInstance() const;
    VkPhysicalDevice getPhysicalDevice() const;

private: // vulkan object
    VkPhysicalDevice m_physicalDevice{ nullptr };
};
} // namespace vkt
