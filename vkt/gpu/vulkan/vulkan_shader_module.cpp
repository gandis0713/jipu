#include "vulkan_shader_module.h"
#include "vulkan_device.h"

namespace vkt
{

VulkanShaderModule::VulkanShaderModule(VulkanDevice* device, const ShaderModuleDescriptor& descriptor)
    : ShaderModule(device, descriptor)
{
}

} // namespace vkt