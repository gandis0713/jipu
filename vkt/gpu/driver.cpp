#include "vkt/gpu/driver.h"

#include "gpu/vulkan/vulkan_driver.h"

#include <spdlog/spdlog.h>

namespace vkt
{

std::unique_ptr<Driver> Driver::create(const DriverDescriptor& descriptor)
{
    switch (descriptor.type)
    {
    case DRIVER_TYPE::VULKAN:
        return std::make_unique<VulkanDriver>(descriptor);
    default:
        break;
    }

    return nullptr;
}

Driver::Driver(const DriverDescriptor& descriptor)
{
    spdlog::set_level(spdlog::level::trace);
}

} // namespace vkt
