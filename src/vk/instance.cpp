#include "../inc/vk/instance.h"

#include "vk/allocation_callbacks.h"

namespace vk {

Instance::Instance(const std::vector<const char *> &instanceExtensions,
                   const std::vector<const char *> &layers,
                   uint32_t vulkan_api_version,
                   AllocationCallbacks *allocator)
{
  // application info
  VkApplicationInfo appInfo = {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Vulkan Application";
  appInfo.pEngineName = "Vulkan Engine";
  appInfo.engineVersion =
      VK_MAKE_VERSION(VKT_VERSION_MAJOR, VKT_VERSION_MINOR, VKT_VERSION_PATCH);
  appInfo.apiVersion = vulkan_api_version;

  // instance info
  VkInstanceCreateInfo instanceCreateInfo = {};
  instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceCreateInfo.pApplicationInfo = &appInfo;

  instanceCreateInfo.enabledExtensionCount =
      static_cast<uint32_t>(instanceExtensions.size());
  instanceCreateInfo.ppEnabledExtensionNames =
      instanceExtensions.empty() ? nullptr : instanceExtensions.data();

  instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
  instanceCreateInfo.ppEnabledLayerNames = layers.empty() ? nullptr : layers.data();

  instanceCreateInfo.pNext = nullptr;

#if defined(__APPLE__)
  instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

  VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance);        
  if (result != VK_SUCCESS) 
  {
      // std::cerr << "Failed to create instance [Error code : " << result  << "]" << std::endl;
  }
}

} // namespace vk
