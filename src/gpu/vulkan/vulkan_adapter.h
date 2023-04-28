#pragma once

#include "gpu/adapter.h"
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
class VulkanAdapter : public Adapter
{
public:
    VulkanAdapter() = delete;
    VulkanAdapter(VulkanDriver* driver, AdapterDescriptor descriptor);
    ~VulkanAdapter() override;

    std::unique_ptr<Device> createDevice(DeviceDescriptor descriptor) override;
    std::unique_ptr<Surface> createSurface(SurfaceDescriptor descriptor) override;

    const VulkanDeviceInfo& getDeviceInfo() const;

public:
    VkInstance getInstance() const;
    VkPhysicalDevice getPhysicalDevice() const;

private:
    void gatherDeviceInfo();

private: // vulkan object
    VkPhysicalDevice m_physicalDevice{ VK_NULL_HANDLE };
    VulkanDeviceInfo m_deviceInfo{};
};
} // namespace vkt
