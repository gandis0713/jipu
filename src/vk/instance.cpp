#include "../inc/vk/instance.h"

#include "vk/allocation_callbacks.h"

namespace vkt {

Instance::Instance(const std::vector<const char *> &instance_extensions,
                   const std::vector<const char *> &layers,
                   uint32_t vulkan_api_version,
                   AllocationCallbacks *allocator) {
  // application info
  VkApplicationInfo appInfo = {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "VulkanSceneGraph application";
  appInfo.pEngineName = "VulkanSceneGraph";
  appInfo.engineVersion =
      VK_MAKE_VERSION(VKT_VERSION_MAJOR, VKT_VERSION_MINOR, VKT_VERSION_PATCH);
  appInfo.apiVersion = vulkan_api_version;

  // instance info
  VkInstanceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(instance_extensions.size());
  createInfo.ppEnabledExtensionNames =
      instance_extensions.empty() ? nullptr : instance_extensions.data();

  createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
  createInfo.ppEnabledLayerNames = layers.empty() ? nullptr : layers.data();

  createInfo.pNext = nullptr;
}

} // namespace vkt
