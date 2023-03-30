#include "gpu/driver.h"

#include "gpu/vulkan/vulkan_driver.h"

namespace vkt
{

std::unique_ptr<Driver> Driver::create(DriverDescriptor descriptor)
{
    switch (descriptor.apiType)
    {
    case DRIVER_TYPE::VULKAN:
        return std::make_unique<VulkanDriver>(descriptor);
    default:
        break;
    }

    return nullptr;
}

} // namespace vkt
