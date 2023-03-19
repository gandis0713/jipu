#pragma once

#include "gpu/device.h"
#include "vulkan_api.h"
#include <memory>

namespace vkt
{

struct DeviceCreateHandles
{
    VkPhysicalDevice physicalDevice{};
};

class VulkanDevice : public Device
{
public:
    VulkanDevice() = delete;
    VulkanDevice(DeviceCreateHandles handles, DeviceCreateInfo info);
    ~VulkanDevice() override = default;

    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice& operator=(const VulkanDevice&) = delete;

    VkDevice getDevice() const;

private:
    VkDevice m_device{};
    VkPhysicalDevice m_physicalDevice{};

    VkQueue m_graphicsQueue{};
    VkQueue m_presentQueue{};
};
} // namespace vkt
