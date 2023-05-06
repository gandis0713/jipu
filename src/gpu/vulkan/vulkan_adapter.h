#pragma once

#include "utils/cast.h"
#include "vkt/gpu/adapter.h"
#include "vulkan_api.h"

namespace vkt
{

struct VulkanDeviceInfo : VulkanDeviceKnobs
{
    VkPhysicalDeviceFeatures physicalDeviceFeatures{};
    VkPhysicalDeviceProperties physicalDeviceProperties{};

    std::vector<VkQueueFamilyProperties> queueFamilyProperties{};

    std::vector<VkLayerProperties> layerProperties;
    std::vector<VkExtensionProperties> extensionProperties;
};

class VulkanDriver;
class VKT_EXPORT VulkanAdapter : public Adapter
{
public:
    VulkanAdapter() = delete;
    VulkanAdapter(VulkanDriver* driver, AdapterDescriptor descriptor);
    ~VulkanAdapter() override;

    std::unique_ptr<Device> createDevice(DeviceDescriptor descriptor) override;

    const VulkanDeviceInfo& getDeviceInfo() const;

public:
    VkInstance getInstance() const;
    VkPhysicalDevice getPhysicalDevice() const;

private:
    void gatherDeviceInfo();

private:
    VkPhysicalDevice m_physicalDevice{ VK_NULL_HANDLE };
    VulkanDeviceInfo m_deviceInfo{};
};

DOWN_CAST(VulkanAdapter, Adapter);

} // namespace vkt
