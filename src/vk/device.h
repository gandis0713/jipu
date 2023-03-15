#pragma once

#include "vk/adapter.h"
#include "vulkan_api.h"
#include <memory>

namespace vkt
{

struct DeviceCreateInfo
{
    Adapter adapter;
};

class Device
{
public:
    static std::shared_ptr<Device> create(DeviceCreateInfo info) { return std::make_shared<Device>(info); }

public:
    Device() = delete;
    Device(DeviceCreateInfo info);
    ~Device();

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    void* operator new(std::size_t) = delete;
    void* operator new[](std::size_t) = delete;

private:
    Adapter m_adapter;

private:
    VkDevice m_device;

    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
};
} // namespace vkt