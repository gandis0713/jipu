#pragma once

#include "utils/cast.h"
#include "vkt/gpu/pipeline_layout.h"
#include "vulkan_api.h"

namespace vkt
{

class VulkanDevice;

class VulkanBindingLayout : public BindingLayout
{
public:
    VulkanBindingLayout() = delete;
    VulkanBindingLayout(VulkanDevice* device, const BindingLayoutDescriptor& descriptor);
    ~VulkanBindingLayout() override = default;

private:
    VkDescriptorSetLayout m_layout = VK_NULL_HANDLE;
};
DOWN_CAST(VulkanBindingLayout, BindingLayout);

class VulkanPipelineLayout : public PipelineLayout
{
public:
    VulkanPipelineLayout() = delete;
    VulkanPipelineLayout(VulkanDevice* device, const PipelineLayoutDescriptor& descriptor);
    ~VulkanPipelineLayout() override = default;
};

DOWN_CAST(VulkanPipelineLayout, PipelineLayout);

} // namespace vkt