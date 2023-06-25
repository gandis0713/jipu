#include "vulkan_pipeline_layout.h"
#include "vulkan_device.h"

namespace vkt
{

VulkanBindingResource::VulkanBindingResource(VulkanDevice* device, const BindingResourceDescriptor& descriptor)
    : BindingResource(device, descriptor)
{
}

VulkanBindingLayout::VulkanBindingLayout(VulkanDevice* device, const BindingLayoutDescriptor& descriptor)
    : BindingLayout(device, descriptor)
{
}

VulkanPipelineLayout::VulkanPipelineLayout(VulkanDevice* device, const PipelineLayoutDescriptor& descriptor)
    : PipelineLayout(device, descriptor)
{
}

} // namespace vkt