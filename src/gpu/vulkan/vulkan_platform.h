#pragma once

#include "gpu/platform.h"
#include "gpu/surface.h"
#include "vulkan_api.h"

#include <memory>

namespace vkt
{

struct PlatformCreateHandles
{
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
};

class VulkanPlatform : public Platform
{
public:
    VulkanPlatform(PlatformCreateHandles handles, PlatformCreateInfo info) noexcept;
    ~VulkanPlatform() override = default;

    std::unique_ptr<Surface> createSurface(SurfaceCreateInfo info) override { return nullptr; };

protected:
    void* m_windowHandle{ nullptr };

    VkInstance m_instance{ nullptr };
    VkPhysicalDevice m_physicalDevice{ nullptr };
};

} // namespace vkt
