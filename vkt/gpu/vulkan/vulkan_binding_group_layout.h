#pragma once

#include "utils/cast.h"
#include "vkt/gpu/binding_group_layout.h"
#include "vulkan_api.h"

namespace vkt
{

class VulkanDevice;
class VulkanBindingGroupLayout : public BindingGroupLayout
{
public:
    VulkanBindingGroupLayout() = delete;
    VulkanBindingGroupLayout(VulkanDevice* device, const BindingGroupLayoutDescriptor& descriptor);
    ~VulkanBindingGroupLayout() override;

    VkDescriptorSetLayout getVkDescriptorSetLayout() const;

private:
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
};
DOWN_CAST(VulkanBindingGroupLayout, BindingGroupLayout);

// Convert Helper
VkDescriptorType ToVkDescriptorType(BufferBindingType type);
BufferBindingType ToBufferBindingType(VkDescriptorType type);
} // namespace vkt