#pragma once

#include "gpu/driver.h"
#include "utils/dynamic_lib.h"
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

class VulkanDriver : public Driver
{

public:
    VulkanDriver() = delete;
    VulkanDriver(DriverDescriptor descriptor) noexcept(false);
    ~VulkanDriver() override;

    VulkanDriver(const VulkanDriver&) = delete;
    VulkanDriver& operator=(const VulkanDriver&) = delete;

public:
    std::unique_ptr<Adapter> createAdapter(AdapterDescriptor descriptor) override;

public: // vulkan
    VkInstance getInstance() const;
    std::vector<VkPhysicalDevice> getPhysicalDevices() const;

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
    VkInstance m_instance{};
    std::vector<VkPhysicalDevice> m_physicalDevices{};

    DynamicLib m_vulkanLib{};
    VulkanDriverInfo m_driverInfo{};
};

} // namespace vkt
