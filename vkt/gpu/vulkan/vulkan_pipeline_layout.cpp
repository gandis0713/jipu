#include "vulkan_pipeline_layout.h"
#include "vulkan_device.h"

namespace vkt
{

VulkanPipelineLayout::VulkanPipelineLayout(VulkanDevice* device, const PipelineLayoutDescriptor& descriptor)
    : PipelineLayout(device, descriptor)
{
}

} // namespace vkt