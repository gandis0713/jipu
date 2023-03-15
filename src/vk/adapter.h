#pragma once

#include "vulkan_api.h"

#include <memory>

namespace vkt
{

class Driver;

struct AdapterCreateInfo
{
    std::shared_ptr<Driver> driver;
    VkPhysicalDevice physicalDevice;
};

class Adapter
{
public:
    Adapter() = default;
    Adapter(AdapterCreateInfo info);
    ~Adapter();

    std::shared_ptr<Driver> getDriver();

private:
    std::shared_ptr<Driver> m_driver;

public: // Vulkan objects.
    VkPhysicalDevice getPhysicalDevice();

private: // Vulkan objects.
    VkPhysicalDevice m_physicalDevice{};
};

} // namespace vkt
