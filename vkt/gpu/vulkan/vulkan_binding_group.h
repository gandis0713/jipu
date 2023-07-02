#pragma once

#include "utils/cast.h"
#include "vkt/gpu/binding_group.h"
#include "vulkan_api.h"

namespace vkt
{

class VulkanDevice;
class VulkanBindingGroup : public BindingGroup
{
public:
    VulkanBindingGroup() = delete;
    VulkanBindingGroup(VulkanDevice* device, const BindingGroupDescriptor& descriptor);
    ~VulkanBindingGroup() override;

    VkDescriptorSet getVkDescriptorSet() const;

private:
    VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
};
DOWN_CAST(VulkanBindingGroup, BindingGroup);
} // namespace vkt