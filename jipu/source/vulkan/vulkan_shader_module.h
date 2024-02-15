#pragma once

#include "jipu/shader_module.h"
#include "vulkan_api.h"

#include "utils/cast.h"

namespace jipu
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
    VulkanDevice* m_device = nullptr;

private:
    VkShaderModule m_shaderModule = VK_NULL_HANDLE;
};

DOWN_CAST(VulkanShaderModule, ShaderModule);

} // namespace jipu
