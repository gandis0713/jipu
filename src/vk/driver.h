#pragma once

// #include "vk/context.h"
#include "vk/adapter.h"
#include "vk/platform.h"
#include "vulkan_api.h"

#include <memory>
#include <vector>

namespace vkt
{

struct DriverCreateInfo
{
    std::unique_ptr<Platform> platform{ nullptr };
};

class Driver
{
public:
    explicit Driver(DriverCreateInfo info);
    ~Driver();

    std::vector<std::unique_ptr<Adapter>> generateAdapters();

private:
    void terminate();

private:
    VkInstance m_instance;
    std::vector<VkPhysicalDevice> m_physicalDevices;

    std::unique_ptr<Platform> m_platform;
};

} // namespace vkt
