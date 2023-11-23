#pragma once

#include "jipu/binding_group_layout.h"
#include "utils/cast.h"
#include "vulkan_api.h"

namespace jipu
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
VkDescriptorType ToVkDescriptorType(BufferBindingType type, bool dynamicOffset = false);
BufferBindingType ToBufferBindingType(VkDescriptorType type);
VkShaderStageFlags ToVkShaderStageFlags(BindingStageFlags flags);
BindingStageFlags ToBindingStageFlags(VkShaderStageFlags flags);
} // namespace jipu