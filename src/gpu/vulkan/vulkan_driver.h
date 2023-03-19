#pragma once

#include "gpu/driver.h"
#include "vulkan_api.h"

#include <memory>
#include <vector>

namespace vkt
{

class VulkanDriver : public Driver
{

public:
    VulkanDriver() = delete;
    VulkanDriver(DriverCreateInfo info);
    ~VulkanDriver() override;

    VulkanDriver(const VulkanDriver&) = delete;
    VulkanDriver& operator=(const VulkanDriver&) = delete;

public:
    std::unique_ptr<Device> createDevice(DeviceCreateInfo info) override;
    std::unique_ptr<Platform> createPlatform(PlatformCreateInfo info) override;

public: // vulkan object
    VkInstance getInstance() const;

private:
    VkInstance m_instance{};
    std::vector<VkPhysicalDevice> m_physicalDevices{};
};

} // namespace vkt
