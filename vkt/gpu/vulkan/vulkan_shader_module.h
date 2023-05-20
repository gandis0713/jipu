#pragma once

#include "vkt/gpu/shader_module.h"
#include "vulkan_api.h"

namespace vkt
{

class VulkanDevice;
class VulkanShaderModule : public ShaderModule
{
public:
    VulkanShaderModule() = delete;
    VulkanShaderModule(VulkanDevice* device, const ShaderModuleDescriptor& descriptor);
    ~VulkanShaderModule() override;

    VkShaderModule getVkShaderModule() const;

private:
    VkShaderModule m_shaderModule = VK_NULL_HANDLE;
};

} // namespace vkt
