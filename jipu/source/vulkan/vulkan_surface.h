#pragma once

#include "jipu/surface.h"
#include "utils/cast.h"
#include "vulkan_api.h"
#include "vulkan_export.h"

#include <vector>

namespace jipu
{

struct VulkanSurfaceInfo
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
    std::vector<bool> supportedQueueFamilies; // TODO: remove or not.
};

struct VulkanSurfaceDescriptor
{
    const void* next = nullptr;
#if defined(__ANDROID__) || defined(ANDROID)
    VkAndroidSurfaceCreateFlagsKHR flags;
    struct ANativeWindow* window = nullptr;
#elif defined(__linux__)
#elif defined(__APPLE__)
#if defined(VK_USE_PLATFORM_METAL_EXT)
    VkMetalSurfaceCreateFlagsEXT flags;
    const CAMetalLayer* layer = nullptr;
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
    VkMacOSSurfaceCreateFlagsMVK flags;
    const void* view = nullptr;
#endif
#elif defined(WIN32)
    VkWin32SurfaceCreateFlagsKHR flags;
    HINSTANCE hinstance;
    HWND hwnd;
#endif
};

class VulkanDriver;
class VULKAN_EXPORT VulkanSurface : public Surface
{
public:
    VulkanSurface() = delete;
    VulkanSurface(VulkanDriver* driver, const SurfaceDescriptor& descriptor);
    VulkanSurface(VulkanDriver* driver, const VulkanSurfaceDescriptor& descriptor);
    ~VulkanSurface() override;

    VkSurfaceKHR getVkSurface() const;

private:
    void createSurfaceKHR();

private:
    VulkanDriver* m_driver = nullptr;
    const VulkanSurfaceDescriptor m_descriptor{};

private:
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;

    VulkanSurfaceInfo m_surfaceInfo{};
};

DOWN_CAST(VulkanSurface, Surface);

// Generate Helper
VulkanSurfaceDescriptor VULKAN_EXPORT generateVulkanSurfaceDescriptor(const SurfaceDescriptor& descriptor);

}; // namespace jipu
