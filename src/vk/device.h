#pragma once

#include "vulkan_api.h"

namespace vkt
{

struct DeviceCreateInfo
{
    VkInstance instance;
};
class Device
{
    explicit Device(const DeviceCreateInfo&);
    ~Device();

private:
    VkInstance m_instance;

private:
    VkDevice m_handle;
};
} // namespace vkt