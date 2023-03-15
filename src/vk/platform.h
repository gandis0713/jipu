#pragma once

#include "vk/surface.h"

#include <memory>

namespace vkt
{

struct PlatformCreateInfo
{
    void* windowHandle;
};

struct PlatformVulkanHandles
{
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
};

class Platform
{
public:
    Platform(PlatformVulkanHandles handles, PlatformCreateInfo info) noexcept;
    virtual ~Platform() noexcept;

    virtual std::unique_ptr<Surface> createSurface(SurfaceCreateInfo info) = 0;

protected:
    void* m_windowHandle{ nullptr };

    VkInstance m_instance{ nullptr };
    VkPhysicalDevice m_physicalDevice{ nullptr };
};

} // namespace vkt
