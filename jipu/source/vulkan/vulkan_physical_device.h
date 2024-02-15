#pragma once

#include "export.h"

#include "jipu/physical_device.h"
#include "utils/cast.h"
#include "vulkan_api.h"

namespace jipu
{

struct VulkanPhysicalDeviceInfo : VulkanDeviceKnobs
{
    VkPhysicalDeviceFeatures physicalDeviceFeatures{};
    VkPhysicalDeviceProperties physicalDeviceProperties{};

    std::vector<VkQueueFamilyProperties> queueFamilyProperties{};

    std::vector<VkLayerProperties> layerProperties;
    std::vector<VkExtensionProperties> extensionProperties;

    std::vector<VkMemoryType> memoryTypes;
    std::vector<VkMemoryHeap> memoryHeaps;
};

struct VulkanPhysicalDeviceDescriptor
{
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
};

class VulkanDriver;
class JIPU_EXPERIMENTAL_EXPORT VulkanPhysicalDevice : public PhysicalDevice
{
public:
    VulkanPhysicalDevice() = delete;
    VulkanPhysicalDevice(VulkanDriver* driver, const VulkanPhysicalDeviceDescriptor& descriptor);
    ~VulkanPhysicalDevice() override;

    std::unique_ptr<Device> createDevice(DeviceDescriptor descriptor) override;

    PhysicalDeviceInfo getInfo() const override;

public:
    VulkanDriver* getDriver() const;

public:
    const VulkanPhysicalDeviceInfo& getVulkanPhysicalDeviceInfo() const;

    int findMemoryTypeIndex(VkMemoryPropertyFlags flags) const;
    bool isDepthStencilSupported(VkFormat format) const;

public:
    VkInstance getVkInstance() const;
    VkPhysicalDevice getVkPhysicalDevice() const;

private:
    void gatherPhysicalDeviceInfo();

protected:
    VulkanDriver* m_driver = nullptr;

private:
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VulkanPhysicalDeviceInfo m_info{};
};

DOWN_CAST(VulkanPhysicalDevice, PhysicalDevice);

} // namespace jipu
