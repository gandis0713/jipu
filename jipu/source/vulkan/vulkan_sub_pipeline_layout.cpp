#include "vulkan_sub_pipeline_layout.h"

#include "vulkan_device.h"

namespace jipu
{

VulkanSubPipelineLayout::VulkanSubPipelineLayout(VulkanDevice* device, const VulkanSubPipelineLayoutDescriptor& descriptor)
    : m_device(device)
    , m_descriptor(descriptor)
{
    auto device1 = m_device;
    auto descriptor1 = m_descriptor;
}

}; // namespace jipu