#pragma once

#include "gpu/device.h"
#include "vulkan_api.h"
#include <memory>

namespace vkt
{

class VulkanAdapter;

class VulkanDevice : public Device
{
public:
    VulkanDevice() = delete;
    VulkanDevice(VulkanAdapter* adapter, DeviceCreateInfo info);
    ~VulkanDevice() override = default;

    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice& operator=(const VulkanDevice&) = delete;

    VkDevice getDevice() const;

private:
    VkDevice m_device{};

    VkQueue m_graphicsQueue{};
    VkQueue m_presentQueue{};
};
} // namespace vkt
