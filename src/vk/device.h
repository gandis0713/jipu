#pragma once

#include "vk/adapter.h"
#include "vulkan_api.h"
#include <memory>

namespace vkt
{

struct DeviceCreateInfo
{
    std::unique_ptr<Adapter> adapter;
};
class Device
{
    explicit Device(DeviceCreateInfo info);
    ~Device();

private:
    std::unique_ptr<Adapter> m_adapter;

private:
    VkDevice m_device;

    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
};
} // namespace vkt