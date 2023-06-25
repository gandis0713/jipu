#pragma once

#include "utils/cast.h"
#include "vkt/gpu/pipeline_layout.h"

namespace vkt
{

class VulkanBindingResource : BindingResource
{
public:
    VulkanBindingResource() = delete;
    VulkanBindingResource(VulkanDevice* device, const BindingResourceDescriptor& descriptor);
    ~VulkanBindingResource() override = default;
};

class VulkanBindingLayout : BindingLayout
{
public:
    VulkanBindingLayout() = delete;
    VulkanBindingLayout(VulkanDevice* device, const BindingLayoutDescriptor& descriptor);
    ~VulkanBindingLayout() override = default;
};

class VulkanDevice;
class VulkanPipelineLayout : public PipelineLayout
{
public:
    VulkanPipelineLayout() = delete;
    VulkanPipelineLayout(VulkanDevice* device, const PipelineLayoutDescriptor& descriptor);
    ~VulkanPipelineLayout() override = default;
};

DOWN_CAST(VulkanPipelineLayout, PipelineLayout);

} // namespace vkt