#include "gtest/gtest.h"

#include "vk/instance.h"

#include <vector>

TEST(instance, test_)
{
  std::vector<const char*> instanceExtensions
  {
    "VK_KHR_surface",
/*
    https://stackoverflow.com/questions/5919996/how-to-detect-reliably-mac-os-x-ios-linux-windows-in-c-preprocessor
    https://sourceforge.net/p/predef/wiki/OperatingSystems/
*/
#if defined(__linux__)
    "VK_KHR_xcb_surface", // for glfw on linux(ubuntu)
#elif defined(_WIN64)
    "VK_KHR_win32_surface",
    "VK_MVK_macos_surface",
#else
    "VK_MVK_macos_surface",
    "VK_EXT_metal_surface",
    VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
#endif
  };
  const std::vector<const char *> layers = {};

  vk::Instance instance(instanceExtensions, layers);
}