#pragma once

#if defined(__linux__)
    #define VK_USE_PLATFORM_XCB_KHR 1
#elif defined(__APPLE__)
    #define VK_USE_PLATFORM_MACOS_MVK 1
#elif defined(WIN32)
    #define VK_USE_PLATFORM_WIN32_KHR 1
#endif

#include <vulkan/vulkan.h>