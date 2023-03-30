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
    VulkanDriver(DriverDescriptor descriptor);
    ~VulkanDriver() override;

    VulkanDriver(const VulkanDriver&) = delete;
    VulkanDriver& operator=(const VulkanDriver&) = delete;

public:
    std::unique_ptr<Adapter> createAdapter(AdapterDescriptor descriptor) override;

public: // vulkan object
    VkInstance getInstance() const;
    std::vector<VkPhysicalDevice> getPhysicalDevices() const;

private:
    VkInstance m_instance{};
    std::vector<VkPhysicalDevice> m_physicalDevices{};
};

} // namespace vkt
