#include "vulkan_surface.h"
#include "vulkan_instance.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace jipu
{

VulkanSurface::VulkanSurface(VulkanInstance& instance, const SurfaceDescriptor& descriptor)
    : VulkanSurface(instance, generateVulkanSurfaceDescriptor(descriptor))
{
}

VulkanSurface::VulkanSurface(VulkanInstance& instance, const VulkanSurfaceDescriptor& descriptor)
    : m_instance(instance)
    , m_descriptor(descriptor)
{
    createSurfaceKHR();
}

VulkanSurface::~VulkanSurface()
{
    auto& vulkanInstance = downcast(m_instance);
    const VulkanAPI& vkAPI = vulkanInstance.vkAPI;

    vkAPI.DestroySurfaceKHR(vulkanInstance.getVkInstance(), m_surface, nullptr);
}

VkSurfaceKHR VulkanSurface::getVkSurface() const
{
    return m_surface;
}

}; // namespace jipu
