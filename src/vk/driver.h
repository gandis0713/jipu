#pragma once

// #include "vk/context.h"
#include "vk/adapter.h"
#include "vk/platform.h"
#include "vulkan_api.h"

#include <memory>
#include <stdexcept>
#include <vector>

namespace vkt
{

struct DriverCreateInfo
{
    std::shared_ptr<Platform> platform{ nullptr };
};

class Driver : public std::enable_shared_from_this<Driver>
{
public:
    static std::shared_ptr<Driver> create(DriverCreateInfo info) { return std::make_shared<Driver>(info); }

public:
    Driver() = delete;
    Driver(DriverCreateInfo info);
    virtual ~Driver();

    Driver(const Driver&) = delete;
    Driver& operator=(const Driver&) = delete;

    void* operator new(std::size_t) = delete;
    void* operator new[](std::size_t) = delete;

public:
    std::vector<Adapter> getAdapters();

private:
    void terminate();

private:
    VkInstance m_instance;
    std::vector<VkPhysicalDevice> m_physicalDevices;

    std::shared_ptr<Platform> m_platform;
};

} // namespace vkt
