#pragma once

#include "vk/device.h"
#include "vk/platform.h"
#include "vulkan_api.h"

#include <memory>
#include <vector>

namespace vkt
{

struct DriverCreateInfo
{
};

class Driver
{
public:
    static std::unique_ptr<Driver> create(DriverCreateInfo info) { return std::make_unique<Driver>(info); }

public:
    Driver() = delete;
    Driver(DriverCreateInfo info);
    virtual ~Driver();

    Driver(const Driver&) = delete;
    Driver& operator=(const Driver&) = delete;

public:
    std::unique_ptr<Device> createDevice(DeviceCreateInfo info);
    std::unique_ptr<Platform> createPlatform(PlatformCreateInfo info);

public: // vulkan object
    VkInstance getInstance() const;

private:
    VkInstance m_instance{};
    std::vector<VkPhysicalDevice> m_physicalDevices{};
};

} // namespace vkt
