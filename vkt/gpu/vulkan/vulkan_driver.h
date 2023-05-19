#pragma once

#include "utils/cast.h"
#include "utils/dynamic_lib.h"
#include "vkt/gpu/driver.h"
#include "vulkan_api.h"

#include <memory>
#include <vector>

namespace vkt
{

struct VulkanDriverInfo : VulkanDriverKnobs
{
    std::vector<VkLayerProperties> layerProperties;
    std::vector<VkExtensionProperties> extensionProperties;
};

class VKT_EXPORT VulkanDriver : public Driver
{

public:
    VulkanDriver() = delete;
    VulkanDriver(DriverDescriptor descriptor) noexcept(false);
    ~VulkanDriver() override;

    VulkanDriver(const VulkanDriver&) = delete;
    VulkanDriver& operator=(const VulkanDriver&) = delete;

public:
    std::unique_ptr<PhysicalDevice> createPhysicalDevice(PhysicalDeviceDescriptor descriptor) override;
    std::unique_ptr<Surface> createSurface(SurfaceDescriptor descriptor) override;

public: // vulkan
    VkInstance getVkInstance() const;
    const std::vector<VkPhysicalDevice>& getVkPhysicalDevices() const;
    VkPhysicalDevice getVkPhysicalDevice(uint32_t index) const;

    const VulkanDriverInfo& getDriverInfo() const;

public:
    VulkanAPI vkAPI{};

private:
    void initialize() noexcept(false);
    void createInstance() noexcept(false);
    void createPhysicalDevices() noexcept(false);

    void gatherDriverInfo();

    bool checkInstanceExtensionSupport(const std::vector<const char*> requiredInstanceExtensions);
    const std::vector<const char*> getRequiredInstanceExtensions();

private:
    VkInstance m_instance = VK_NULL_HANDLE;
    std::vector<VkPhysicalDevice> m_physicalDevices{};

    DynamicLib m_vulkanLib{};
    VulkanDriverInfo m_driverInfo{};
};

DOWN_CAST(VulkanDriver, Driver);

} // namespace vkt
