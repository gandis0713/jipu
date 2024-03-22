#pragma once

#include "export.h"

#include "jipu/binding_group.h"
#include "utils/cast.h"
#include "vulkan_api.h"
#include "vulkan_export.h"

namespace jipu
{

struct VulkanBindingGroupDescriptor
{
    const BindingGroupLayout& layout;
    std::vector<VkDescriptorBufferInfo> buffers{};
    std::vector<VkDescriptorImageInfo> samplers{};
    std::vector<VkDescriptorImageInfo> textures{};
};

class VulkanDevice;
class VULKAN_EXPORT VulkanBindingGroup : public BindingGroup
{
public:
    VulkanBindingGroup() = delete;
    VulkanBindingGroup(VulkanDevice* device, const BindingGroupDescriptor& descriptor);
    VulkanBindingGroup(VulkanDevice* device, const VulkanBindingGroupDescriptor& descriptor);
    ~VulkanBindingGroup() override;

    VkDescriptorSet getVkDescriptorSet() const;

private:
    VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;

private:
    VulkanDevice* m_device = nullptr;
    const VulkanBindingGroupDescriptor m_descriptor;
};
DOWN_CAST(VulkanBindingGroup, BindingGroup);

// Generate Helper
VulkanBindingGroupDescriptor VULKAN_EXPORT generateVulkanBindingGroupDescriptor(const BindingGroupDescriptor& descriptor);

} // namespace jipu