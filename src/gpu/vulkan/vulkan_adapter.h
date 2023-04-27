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

struct VulkanSurfaceInfo
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
    std::vector<bool> supportedQueueFamilies;
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
    void gatherSurfaceInfo();

private: // vulkan object
    VkPhysicalDevice m_physicalDevice{ nullptr };
    VulkanDeviceInfo m_deviceInfo{};
    VulkanSurfaceInfo m_surfaceInfo{};
};
} // namespace vkt
