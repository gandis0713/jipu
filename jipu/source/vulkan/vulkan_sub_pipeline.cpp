#include "vulkan_sub_pipeline.h"

#include "vulkan_device.h"

namespace jipu
{

VulkanSubPipeline::VulkanSubPipeline(VulkanDevice* device, const VulkanSubPipelineDescriptor& descriptor)
    : m_device(device)
    , m_descriptor(descriptor)
{
    auto device1 = m_device;
    auto descriptor1 = m_descriptor;
}

} // namespace jipu