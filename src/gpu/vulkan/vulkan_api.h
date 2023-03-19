#pragma once

#if defined(__linux__)
    #define VK_USE_PLATFORM_XCB_KHR
#elif defined(__APPLE__)
    #define VK_USE_PLATFORM_METAL_EXT // use Metal Extension
// #define VK_USE_PLATFORM_MACOS_MVK
#elif defined(WIN32)
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.h>