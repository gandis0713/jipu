#include "gpu/driver.h"

#include "gpu/vulkan/vulkan_driver.h"

namespace vkt
{

std::unique_ptr<Driver> Driver::create(DriverCreateInfo info)
{
    switch (info.apiType)
    {
    case API_TYPE::VULKAN:
        return std::make_unique<VulkanDriver>(info);
    default:
        break;
    }

    return nullptr;
}

} // namespace vkt
