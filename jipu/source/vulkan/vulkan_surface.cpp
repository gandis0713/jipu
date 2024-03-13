#include "vulkan_surface.h"
#include "vulkan_driver.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace jipu
{

VulkanSurface::VulkanSurface(VulkanDriver* driver, const SurfaceDescriptor& descriptor)
    : VulkanSurface(driver, generateVulkanSurfaceDescriptor(descriptor))
{
}

VulkanSurface::VulkanSurface(VulkanDriver* driver, const VulkanSurfaceDescriptor& descriptor)
    : m_driver(driver)
    , m_descriptor(descriptor)
{
    createSurfaceKHR();
}

VulkanSurface::~VulkanSurface()
{
    auto vulkanDriver = downcast(m_driver);
    const VulkanAPI& vkAPI = vulkanDriver->vkAPI;

    vkAPI.DestroySurfaceKHR(vulkanDriver->getVkInstance(), m_surface, nullptr);
}

VkSurfaceKHR VulkanSurface::getVkSurface() const
{
    return m_surface;
}

}; // namespace jipu
