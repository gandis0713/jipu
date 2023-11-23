#pragma once

#include "utils/cast.h"
#include "utils/dylib.h"
#include "vkt/gpu/driver.h"
#include "vulkan_api.h"

#include <memory>
#include <vector>

namespace jipu
{

struct VulkanDriverInfo : VulkanDriverKnobs
{
    std::vector<VkLayerProperties> layerProperties;
    std::vector<VkExtensionProperties> extensionProperties;
};

class VulkanDriver : public Driver
{

public:
    VulkanDriver() = delete;
    VulkanDriver(const DriverDescriptor& descriptor) noexcept(false);
    ~VulkanDriver() override;

    VulkanDriver(const VulkanDriver&) = delete;
    VulkanDriver& operator=(const VulkanDriver&) = delete;

public:
    std::unique_ptr<PhysicalDevice> createPhysicalDevice(const PhysicalDeviceDescriptor& descriptor) override;
    std::unique_ptr<Surface> createSurface(const SurfaceDescriptor& descriptor) override;

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
    void gatherPhysicalDevices() noexcept(false);

    void gatherDriverInfo();

    bool checkInstanceExtensionSupport(const std::vector<const char*> requiredInstanceExtensions);
    const std::vector<const char*> getRequiredInstanceExtensions();
    bool checkInstanceLayerSupport(const std::vector<const char*> requiredInstanceLayers);
    const std::vector<const char*> getRequiredInstanceLayers();

private:
    VkInstance m_instance = VK_NULL_HANDLE;
    std::vector<VkPhysicalDevice> m_physicalDevices{};

    DyLib m_vulkanLib{};
    VulkanDriverInfo m_driverInfo{};
};

DOWN_CAST(VulkanDriver, Driver);

} // namespace jipu
