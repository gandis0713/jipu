#pragma once

#include "gpu/adapter.h"
#include "vulkan_api.h"

namespace vkt
{

struct VulkanDeviceInfo
{
    VkPhysicalDeviceFeatures physicalDeviceFeatures{};
    VkPhysicalDeviceProperties physicalDeviceProperties{};
    std::vector<VkQueueFamilyProperties> queueFamilyProperties{};
    bool isSurfaceSupported;
};

class VulkanDriver;
class VulkanAdapter : public Adapter
{
public:
    VulkanAdapter() = delete;
    VulkanAdapter(VulkanDriver* driver, AdapterDescriptor descriptor);
    ~VulkanAdapter() override;

    std::unique_ptr<Device> createDevice(DeviceDescriptor descriptor) override;
    std::unique_ptr<Platform> createPlatform(PlatformDescriptor descriptor) override;

    const VulkanDeviceInfo& getDeviceInfo() const;

public:
    VkInstance getInstance() const;
    VkPhysicalDevice getPhysicalDevice() const;

private:
    void gatherDeviceInfo();

private: // vulkan object
    VkPhysicalDevice m_physicalDevice{ nullptr };
    VulkanDeviceInfo m_deviceInfo{};
};
} // namespace vkt
