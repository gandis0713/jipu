#include "vulkan_binding_group.h"
#include "vulkan_device.h"

namespace vkt
{

VulkanBindingGroup::VulkanBindingGroup(VulkanDevice* device, const BindingGroupDescriptor& descriptor)
    : BindingGroup(device, descriptor)
{
}

VulkanBindingGroup::~VulkanBindingGroup()
{
}

VkDescriptorSet VulkanBindingGroup::getVkDescriptorSet() const
{
    return m_descriptorSet;
}

} // namespace vkt