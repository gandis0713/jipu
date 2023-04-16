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

public: // vulkan object
    VkInstance getInstance() const;
    std::vector<VkPhysicalDevice> getPhysicalDevices() const;

private:
    void initialize() noexcept(false);

private:
    VkInstance m_instance{};
    std::vector<VkPhysicalDevice> m_physicalDevices{};

    DynamicLib m_vulkanLib{};
    VulkanAPI m_api{};
};

} // namespace vkt
