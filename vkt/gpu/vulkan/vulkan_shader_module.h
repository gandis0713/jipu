#pragma once

#include "vkt/gpu/shader_module.h"

namespace vkt
{

class VulkanDevice;
class VulkanShaderModule : public ShaderModule
{
public:
    VulkanShaderModule() = delete;
    VulkanShaderModule(VulkanDevice* device, const ShaderModuleDescriptor& descriptor);
    ~VulkanShaderModule() override = default;
};

} // namespace vkt
