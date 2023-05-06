#pragma once

#include "utils/cast.h"
#include "vkt/gpu/physical_device.h"
#include "vulkan_api.h"

namespace vkt
{

struct VulkanPhysicalDeviceInfo : VulkanDeviceKnobs
{
    VkPhysicalDeviceFeatures physicalDeviceFeatures{};
    VkPhysicalDeviceProperties physicalDeviceProperties{};

    std::vector<VkQueueFamilyProperties> queueFamilyProperties{};

    std::vector<VkLayerProperties> layerProperties;
    std::vector<VkExtensionProperties> extensionProperties;
};

class VulkanDriver;
class VKT_EXPORT VulkanPhysicalDevice : public PhysicalDevice
{
public:
    VulkanPhysicalDevice() = delete;
    VulkanPhysicalDevice(VulkanDriver* driver, PhysicalDeviceDescriptor descriptor);
    ~VulkanPhysicalDevice() override;

    std::unique_ptr<Device> createDevice(DeviceDescriptor descriptor) override;

    const VulkanPhysicalDeviceInfo& getInfo() const;

public:
    VkInstance getInstance() const;
    VkPhysicalDevice getPhysicalDevice() const;

private:
    void gatherPhysicalDeviceInfo();

private:
    VkPhysicalDevice m_physicalDevice{ VK_NULL_HANDLE };
    VulkanPhysicalDeviceInfo m_Info{};
};

DOWN_CAST(VulkanPhysicalDevice, PhysicalDevice);

} // namespace vkt
