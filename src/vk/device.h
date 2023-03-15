#pragma once

#include "vulkan_api.h"
#include <memory>

namespace vkt
{

struct DeviceCreateInfo
{
};

struct DeviceVulkanHandles
{
    VkPhysicalDevice physicalDevice{};
};

class Driver;

class Device
{
public:
    Device() = delete;
    Device(DeviceVulkanHandles handles, DeviceCreateInfo info);
    ~Device();

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    VkDevice getDevice() const;

private:
    VkDevice m_device{};
    VkPhysicalDevice m_physicalDevice{};

    VkQueue m_graphicsQueue{};
    VkQueue m_presentQueue{};

    friend Driver;
};
} // namespace vkt
